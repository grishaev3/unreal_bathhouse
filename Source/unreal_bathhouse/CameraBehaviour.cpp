#include "CameraBehaviour.h"
#include "UniqueRandom.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Algo/SelectRandomWeighted.h"
#include "DrawDebugHelpers.h"

FORCEINLINE FVector GetRandomVectorInBounds(const FBoxBounds& Bounds)
{
	return FVector(
		FMath::RandRange(Bounds.Min.X, Bounds.Max.X),
		FMath::RandRange(Bounds.Min.Y, Bounds.Max.Y),
		FMath::RandRange(Bounds.Min.Z, Bounds.Max.Z)
	);
}

FORCEINLINE static FVector ConvertUnityToUE(const FVector& UnityVector, bool bIsLocation)
{
	// 1. Меняем оси местами: Unity Z становится вперед (X), Unity X становится вправо (Y), Unity Y становится вверх (Z)
	FVector ReplacedAxes = FVector(UnityVector.Z, UnityVector.X, UnityVector.Y);

	// 2. Если это точка/позиция, переводим метры в сантиметры. Если это вектор направления — оставляем масштаб 1:1
	if (bIsLocation)
	{
		return ReplacedAxes * 100.0f;
	}

	return ReplacedAxes;
}

// --- REALIZATION: UCameraBase ---
TPair<FVector, FVector> UCameraBase::Invoke(float NormalizedTime)
{
	if (FuncLookFrom)
	{
		From = FuncLookFrom(NormalizedTime, this);
	}

	if (FuncLookAt)
	{
		At = FuncLookAt(NormalizedTime, this);
	}
	// Если камера должна смотреть в "Центр" (как в Linear и Random из Unity)
	else if (DirectionType == ECameraDirectionType::Center)
	{
		// В Unity было: Vector3(from.x, from.y, -2f) -> где Z была глубиной.
		// В UE5 глубина/вперед — это ось X. Поэтому фиксируем ось X на значении центра сцены (например, -200 см),
		// а по горизонтали Y и высоте Z камера плавно следит за своими координатами.
		At = FVector(-200.0f, From.Y, From.Z);
	}
	// Если камера смотрит строго перед собой
	else if (DirectionType == ECameraDirectionType::Direct)
	{
		// В Unity: Vector3(from.x, from.y, from.z - 1f) -> смотрела по оси Z назад.
		// В UE5: смотрим вперед по оси X (добавляем 100 см к X)
		At = FVector(From.X + 100.0f, From.Y, From.Z);
	}

	return TPair<FVector, FVector>(From, At);
}

// --- REALIZATION: FLinearBase ---
FLinearBase::FLinearBase(float InFreq, float InDuration, FCameraMathFunc InFunc, FString InName, const FBoundParameters& Bounds, ECameraDirectionType InDirType)
{
	Freq = InFreq;
	DirectionType = InDirType;
	FuncLookFrom = InFunc;
	Duration = InDuration;
	Name = InName;
	Reset(Bounds);
}

bool FLinearBase::IsDirection(float Value) { return FMath::Abs(Value) == 1.0f; }
bool FLinearBase::IsBound(float Value) { return FMath::Abs(Value) == BIG_NUMBER; }

EDirectionType FLinearBase::GetDirectionType(const FVector& V)
{
	if (IsDirection(V.X)) return EDirectionType::X;
	if (IsDirection(V.Y)) return EDirectionType::Y;
	if (IsDirection(V.Z)) return EDirectionType::Z;
	return EDirectionType::Zero;
}

void FLinearBase::Reset(const FBoundParameters& BoundParameters)
{
	FBoxBounds Bound = BoundParameters.Bound;

	// Защита от пустого мувсета
	FVector UnityDirection = BoundParameters.CameraMovesets.Num() > 0 ? BoundParameters.CameraMovesets[0] : FVector::ZeroVector;

	// В Unity глубина рассчитывалась по Z. В UE5 это ось X (Вперед)
	auto CalculateDepthUE = [&](const FBoxBounds& InBound) -> float {
		if (IsBound(UnityDirection.Z))
		{
			return (UnityDirection.Z > 0.0f) ? InBound.Max.X : InBound.Min.X;
		}
		return InBound.Min.X + (InBound.Size.X / 2.0f); // По центру глубины, если не задано
	};

	float DepthX = CalculateDepthUE(Bound);
	float Start = 0.0f, End = 0.0f;

	// Переводим логику выбора осей из Unity в координаты UE5
	switch (GetDirectionType(UnityDirection))
	{
	case EDirectionType::X: // В Unity это была ширина (X). В UE5 это ось Y (Вправо)
		if (UnityDirection.X > 0.0f)
		{
			Start = Bound.Min.Y;
			End = Bound.Max.Y;
		}
		else
		{
			Start = Bound.Max.Y;
			End = Bound.Min.Y;
		}
		{
			// Высота в Unity (Y) стала высотой в UE5 (Z)
			float HeightZ = Bound.Min.Z + Bound.Size.Z * UnityDirection.Y;
			A = FVector(DepthX, Start, HeightZ);
			B = FVector(DepthX, End, HeightZ);
		}
		break;

	case EDirectionType::Y: // В Unity это была высота (Y). В UE5 это ось Z (Вверх)
		if (UnityDirection.Y > 0.0f)
		{
			Start = Bound.Min.Z;
			End = Bound.Max.Z;
		}
		else
		{
			Start = Bound.Max.Z;
			End = Bound.Min.Z;
		}
		// Ось X в Unity стала осью Y в UE5
		A = FVector(DepthX, UnityDirection.X * 100.0f, Start);
		B = FVector(DepthX, UnityDirection.X * 100.0f, End);
		break;

	default:
		// Если направление Zero, просто берем центр объема
		A = Bound.Min;
		B = Bound.Max;
		break;
	}
}

// --- REALIZATION: FLinearRandom ---
FLinearRandom::FLinearRandom(float InFreq, float InDuration, FCameraMathFunc InFunc, FString InName, const FBoundParameters& Bounds, ECameraDirectionType InDirType)
	: FLinearBase(InFreq, InDuration, InFunc, InName, Bounds, InDirType)
{
	Reset(Bounds);
}

void FLinearRandom::Reset(const FBoundParameters& BoundParameters)
{
	A = GetRandomVectorInBounds(BoundParameters.Bound);
	B = GetRandomVectorInBounds(BoundParameters.Bound);
}

// --- REALIZATION: FStaticCamera ---
FStaticCamera::FStaticCamera(float InFreq, float InDuration, FCameraMathFunc InFuncLookFrom, FCameraMathFunc InFuncLookTo, FVector InA, FVector InB, FString InName)
{
	Freq = InFreq;
	A = InA;
	B = InB;
	FuncLookFrom = InFuncLookFrom;
	FuncLookAt = InFuncLookTo;
	Duration = InDuration;
	Name = InName;
}

// --- REALIZATION: FTimeManager ---
FTimeManager::FTimeManager(UWorld* InWorld)
{
	WorldRef = InWorld;
	CurrentHour = 12;
	MsCurrentTime = 0.0;
	NormalizedTime = 0.0;
}

void FTimeManager::UpdateNormalizedTime(UCameraBase* Model, float& OutNormalizedTime)
{
	if (!Model || !WorldRef) return;
	float DeltaTime = WorldRef->GetDeltaSeconds();
	float TimeScale = UGameplayStatics::GetGlobalTimeDilation(WorldRef);

	MsCurrentTime += DeltaTime * 1000.0;
	double ModelMsDuration = Model->Duration * 1000.0;
	NormalizedTime = MsCurrentTime / (ModelMsDuration / (1.0 / TimeScale));
	OutNormalizedTime = (float)NormalizedTime;
}

ESunCircle FTimeManager::GetSunCircle() const
{
	return (CurrentHour >= 4 && CurrentHour <= 20) ? ESunCircle::Day : ESunCircle::Night;
}

void FTimeManager::Reset(double D) { MsCurrentTime = D; }

// --- REALIZATION: FBoundManager ---
FBoxBounds FBoundManager::FromZeroUE(FVector Center, FVector Size)
{
	// Переводим метры из Unity в сантиметры Unreal Engine 5
	Center *= 100.0f;
	Size *= 100.0f;

	FBoxBounds B;

	// Перенос осей: Unity Z,X,Y -> UE5 X,Y,Z
	FVector AdjustedCenter = FVector(Center.Z, Center.X, Center.Y);
	FVector AdjustedSize = FVector(Size.Z, Size.X, Size.Y);

	// --- ИСПРАВЛЕНИЕ ВЫСОТЫ ---
	// Добавляем к координате Z половину высоты бокса (AdjustedSize.Z / 2.0f),
	// чтобы компенсировать разницу в точке отсчета пола (Floor Pivot) между движками.
	AdjustedCenter.Z += (AdjustedSize.Z / 2.0f);

	// Строим бокс от минимальной до максимальной точки
	B.Min = AdjustedCenter - (AdjustedSize / 2.0f);
	B.Max = AdjustedCenter + (AdjustedSize / 2.0f);
	B.Size = AdjustedSize;
	return B;
}

FBoxBounds FBoundManager::FromMinMaxUE(FVector Min, FVector Max)
{
	Min *= 100.0f;
	Max *= 100.0f;

	FBoxBounds B;
	// Разворачиваем оси для векторов Min и Max по тому же правилу
	FVector AdjustedMin = FVector(Min.Z, Min.X, Min.Y);
	FVector AdjustedMax = FVector(Max.Z, Max.X, Max.Y);

	// Выравниваем, чтобы Min гарантированно был меньше Max по всем осям в UE5
	B.Min = FVector(FMath::Min(AdjustedMin.X, AdjustedMax.X), FMath::Min(AdjustedMin.Y, AdjustedMax.Y), FMath::Min(AdjustedMin.Z, AdjustedMax.Z));
	B.Max = FVector(FMath::Max(AdjustedMin.X, AdjustedMax.X), FMath::Max(AdjustedMin.Y, AdjustedMax.Y), FMath::Max(AdjustedMin.Z, AdjustedMax.Z));
	B.Size = B.Max - B.Min;
	return B;
}

FBoundManager::FBoundManager()
{
	// Инициализируем дефолтный мувсет (в UE MaxValue заменяем на BIG_NUMBER)
	TArray<FVector> DefaultMoveset = {
		FVector(1.0f, 0.5f, BIG_NUMBER),
		FVector(1.0f, 0.5f, -BIG_NUMBER),
		FVector(-1.0f, 0.5f, BIG_NUMBER),
		FVector(-1.0f, 0.5f, -BIG_NUMBER)
	};

	// Наполняем массив зон с автоматической конвертацией координат под UE5
	BoundsList.Add(FBoundParameters{ FromZeroUE(FVector(0.0f, 0.35f, -2.0f), FVector(5.0f, 2.40f, 3.0f)), DefaultMoveset, TEXT("Внутри дома 1-ый эт.") });
	BoundsList.Add(FBoundParameters{ FromZeroUE(FVector(0.0f, 3.35f, -2.0f), FVector(3.40f, 2.00f, 3.0f)), DefaultMoveset, TEXT("Внутри дома 2-ой эт.") });
	BoundsList.Add(FBoundParameters{ FromZeroUE(FVector(0.0f, 0.5f, -2.0f), FVector(8.0f, 6.0f, 12.0f)), DefaultMoveset, TEXT("Глобальный объём") });
}

void FBoundManager::Reset(FString Description)
{
	for (int32 i = 0; i < BoundsList.Num(); i++)
	{
		if (BoundsList[i].Description == Description) 
		{
			CurrentBoundIndex = i; 
			return; 
		}
	}

	TArray<float> Weights = { 0.5f, 0.5f };

	// --- ИСПРАВЛЕНИЕ: Используем алгоритм Unreal Engine вместо FMath ---
	// Функция принимает массив весов и возвращает указатель на выбранный элемент.
	// С помощью лямбды мы просто возвращаем сам вес, а затем вычисляем индекс элемента в массиве.
	float* SelectedWeight = Algo::SelectRandomWeightedBy(Weights, [](float W) { return W; });

	if (SelectedWeight != nullptr)
	{
		CurrentBoundIndex = Weights.Find(*SelectedWeight);
	}
	else
	{
		CurrentBoundIndex = 0; // Защита, если массив оказался пустым
	}
}

// --- REALIZATION: FCameraModelManager ---
FVector FCameraModelManager::LinearLerp(float NormalizedTime, UCameraBase* Model)
{
	if (!Model) return FVector::ZeroVector;

	// Прямой полиморфный Lerp без искажения осей
	return FMath::Lerp(Model->GetA(), Model->GetB(), NormalizedTime);
}

FCameraModelManager::FCameraModelManager(FBoundParameters& BoundParameters)
{
	float DurationInSeconds = 10.0f;
	float OftenFreq = 0.5f; 
	float RarelyFreq = 0.3f;
	float StaticFreq = (1.0f - (OftenFreq + RarelyFreq)) / 6.0f;

	FVector Static0_From = FVector(6.00f, -0.63f, 1.97f) * 100.0f;
	FVector Static0_A = FVector(0.00f, -5.00f, 1.97f) * 100.0f;
	FVector Static0_B = FVector(0.00f, 5.00f, 1.97f) * 100.0f;

	CameraModels.Add(std::make_shared<FLinearBase>(OftenFreq, DurationInSeconds, LinearLerp, TEXT("Linear"), BoundParameters, ECameraDirectionType::Center));
	CameraModels.Add(std::make_shared<FLinearRandom>(RarelyFreq, DurationInSeconds, LinearLerp, TEXT("Random"), BoundParameters, ECameraDirectionType::Center));

	CameraModels.Add(std::make_shared<FStaticCamera>(StaticFreq, DurationInSeconds, [Static0_From](float, UCameraBase*) { return Static0_From; }, LinearLerp, Static0_A, Static0_B, TEXT("Static0")));

	for (int32 i = 0; i < CameraModels.Num(); ++i) 
	{
		CameraModels[i]->Index = i;
	}

	std::vector<double> Probabilities;
	for (const auto& Model : CameraModels) 
	{
		Probabilities.push_back((double)Model->Freq);
	}

	UniqueRandomProvider = std::make_unique<FUniqueRandom>(TEXT("CameraModels"), 0, CameraModels.Num(), Probabilities);

	CurrentModelIndex = UniqueRandomProvider->Next();
}

UCameraBase* FCameraModelManager::GetActiveModel() const
{
	for (const auto& Model : CameraModels)
	{
		if (Model->Index == CurrentModelIndex)
		{
			return Model.get();
		}
	}

	return nullptr;
}

void FCameraModelManager::ResetModel() 
{ 
	CurrentModelIndex = UniqueRandomProvider->Next(CurrentModelIndex); 
}

bool FCameraModelManager::ActiveModelUsesBounds() const 
{ 
	UCameraBase* Active = GetActiveModel();
	if (!Active) 
	{
		return false;
	}

	return Active->UsesBounds();
}

// --- REALIZATION: ACameraBehaviour (MAIN ACTOR) ---
ACameraBehaviour::ACameraBehaviour()
{
	PrimaryActorTick.bCanEverTick = true;
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	RootComponent = CameraComponent;
}

void ACameraBehaviour::BeginPlay()
{
	Super::BeginPlay();

	// Инициализация менеджеров
	TimeManager = std::make_unique<FTimeManager>(GetWorld());
	BoundManager = std::make_unique<FBoundManager>();

	FBoundParameters ActiveBound = BoundManager->GetActiveBound();
	ModelManager = std::make_unique<FCameraModelManager>(ActiveBound);

	OnPeriodEnd(BIG_NUMBER);
}

void ACameraBehaviour::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ModelManager || !TimeManager || !BoundManager) 
	{
		return;
	}

	UCameraBase* ActiveModel = ModelManager->GetActiveModel();
	if (!ActiveModel) 
	{
		return;
	}

	float NormalizedTime = 0.0f;
	TimeManager->UpdateNormalizedTime(ActiveModel, NormalizedTime);

	TPair<FVector, FVector> TransformPair = ActiveModel->Invoke(NormalizedTime);
	FVector LookFrom = TransformPair.Key;
	FVector LookAt = TransformPair.Value;

	// Применяем позицию и поворот к камере
	SetActorLocation(LookFrom);
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(LookFrom, LookAt);
	SetActorRotation(TargetRotation);

	/*
	// --- ОТЛАДОЧНЫЙ БЛОК ДОЛЖЕН СТОЯТЬ СТРОГО ТУТ (В САМОМ КОНЦЕ TICK) ---
	if (GetWorld())
	{
		FBoundParameters ActiveBound = BoundManager->GetActiveBound();
		FVector Center = ActiveBound.Bound.Min + (ActiveBound.Bound.Size / 2.0f);

		// Рисуем коробку зоны съёмки (синяя)
		DrawDebugBox(GetWorld(), Center, ActiveBound.Bound.Size / 2.0f, FColor::Blue, false, 0.0f, 0, 5.0f);

		// Рисуем целевую точку, куда камера сейчас смотрит (зеленая сфера)
		// Теперь компилятор видит переменную LookAt, созданную выше!
		DrawDebugSphere(GetWorld(), LookAt, 50.0f, 8, FColor::Green, false, 0.0f, 0, 2.0f);
	}
	*/

	OnPeriodEnd(NormalizedTime);
}

void ACameraBehaviour::OnPeriodEnd(float NormalizedTime)
{
	if (NormalizedTime < 0.999f) return;

	// Сбрасываем таймер
	if (TimeManager)
	{
		TimeManager->Reset();
	}

	// Переключаем модель камеры в менеджере
	if (ModelManager)
	{
		ModelManager->ResetModel();

		// Если новая выбранная модель требует объема (Bounds), обновляем BoundManager
		if (ModelManager->ActiveModelUsesBounds())
		{
			if (BoundManager)
			{
				BoundManager->Reset(TEXT("")); // Сброс на случайный объем
				FBoundParameters ActiveBound = BoundManager->GetActiveBound();

				// Переинициализируем активную модель под новые границы
				UCameraBase* ActiveModel = ModelManager->GetActiveModel();
				if (ActiveModel)
				{
					ActiveModel->Reset(ActiveBound);
				}
			}
		}
	}

	// Аналог Debug.Log в Unreal Engine 5. Выводит сообщение в лог и на экран
	if (GEngine && ModelManager)
	{
		UCameraBase* ActiveModel = ModelManager->GetActiveModel();
		if (ActiveModel)
		{
			FString LogMessage = FString::Printf(TEXT("Camera Period Ended. New Model: %s"), *ActiveModel->Name);

			// Вывод на экран (зеленый текст на 5 секунд)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, LogMessage);

			// Вывод в системную консоль Output Log
			UE_LOG(LogTemp, Log, TEXT("%s"), *LogMessage);
		}
	}
}


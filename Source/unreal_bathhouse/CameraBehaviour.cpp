#include "CameraBehaviour.h"
#include "UniqueRandom.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Algo/SelectRandomWeighted.h"

// Вспомогательная функция для генерации случайной точки в коробке
FORCEINLINE FVector GetRandomVectorInBounds(const FBoxBounds& Bounds)
{
	return FVector(
		FMath::RandRange(Bounds.Min.X, Bounds.Max.X),
		FMath::RandRange(Bounds.Min.Y, Bounds.Max.Y),
		FMath::RandRange(Bounds.Min.Z, Bounds.Max.Z)
	);
}

// --- REALIZATION: UCameraBase ---
TPair<FVector, FVector> UCameraBase::Invoke(float NormalizedTime)
{
	if (FuncLookFrom) From = FuncLookFrom(NormalizedTime, this);
	if (FuncLookAt)
	{
		At = FuncLookAt(NormalizedTime, this);
	}
	else if (DirectionType == ECameraDirectionType::Direct)
	{
		At = FVector(From.X, From.Y, From.Z - 100.0f);
	}
	else if (DirectionType == ECameraDirectionType::Center)
	{
		At = FVector(From.X, From.Y, -200.0f);
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
	FVector Direction = BoundParameters.CameraMovesets.Num() > 0 ? BoundParameters.CameraMovesets[0] : FVector::ZeroVector;

	float Start = 0.0f, End = 0.0f;
	auto CalculateDepth = [&](const FBoxBounds& InBound) -> float {
		if (IsBound(Direction.Z)) return (Direction.Z > 0.0f) ? InBound.Max.Z : InBound.Min.Z;
		return 0.0f;
	};

	float Z = CalculateDepth(Bound);
	switch (GetDirectionType(Direction))
	{
	case EDirectionType::X:
		Start = (Direction.X > 0.0f) ? Bound.Min.X : Bound.Max.X;
		End = (Direction.X > 0.0f) ? Bound.Max.X : Bound.Min.X;
		{
			float Height = Bound.Min.Y + Bound.Size.Y * Direction.Y;
			A = FVector(Start, Height, Z);
			B = FVector(End, Height, Z);
		}
		break;
	case EDirectionType::Y:
		Start = (Direction.Y > 0.0f) ? Bound.Min.Y : Bound.Max.Y;
		End = (Direction.Y > 0.0f) ? Bound.Max.Y : Bound.Min.Y;
		A = FVector(Direction.X, Start, Z);
		B = FVector(Direction.X, End, Z);
		break;
	default:
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
FTimeManager::FTimeManager(UWorld* InWorld) : WorldRef(InWorld) { CurrentHour = 12; }

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
	// Пересчитываем метры в сантиметры (* 100.0f)
	Center *= 100.0f;
	Size *= 100.0f;

	FBoxBounds B;
	// В UE5: X - вперед/назад, Y - влево/вправо, Z - вверх/вниз
	// Адаптируем логику Unity: К центру по горизонтали добавляем/отнимаем половину размера
	B.Min = FVector(-Size.X / 2.0f, Center.Y + (-Size.Z / 2.0f), Center.Z);
	B.Max = FVector(Size.X / 2.0f, Center.Y + (Size.Z / 2.0f), Center.Z + Size.Y);
	B.Size = Size;
	return B;
}

FBoxBounds FBoundManager::FromMinMaxUE(FVector Min, FVector Max)
{
	FBoxBounds B;
	// Перевод метров в сантиметры с сохранением осей (требует калибровки под сцену)
	B.Min = Min * 100.0f;
	B.Max = Max * 100.0f;
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
		if (BoundsList[i].Description == Description) { CurrentBoundIndex = i; return; }
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

	return FMath::Lerp(Model->GetA(), Model->GetB(), NormalizedTime);
}

FCameraModelManager::FCameraModelManager(FBoundParameters& BoundParameters)
{
	float DurationInSeconds = 10.0f;
	float OftenFreq = 0.5f; float RarelyFreq = 0.3f;
	float StaticFreq = (1.0f - (OftenFreq + RarelyFreq)) / 2.0f;

	FVector Static0_From = FVector(6.00f, -0.63f, 1.97f) * 100.0f;
	FVector Static0_A = FVector(0.00f, -5.00f, 1.97f) * 100.0f;
	FVector Static0_B = FVector(0.00f, 5.00f, 1.97f) * 100.0f;

	CameraModels.Add(std::make_shared<FLinearBase>(OftenFreq, DurationInSeconds, LinearLerp, TEXT("Linear"), BoundParameters, ECameraDirectionType::Center));
	CameraModels.Add(std::make_shared<FLinearRandom>(RarelyFreq, DurationInSeconds, LinearLerp, TEXT("Random"), BoundParameters, ECameraDirectionType::Center));
	CameraModels.Add(std::make_shared<FStaticCamera>(StaticFreq, DurationInSeconds, [Static0_From](float, UCameraBase*) { return Static0_From; }, LinearLerp, Static0_A, Static0_B, TEXT("Static0")));

	for (int32 i = 0; i < CameraModels.Num(); ++i) CameraModels[i]->Index = i;

	std::vector<double> Probabilities;
	for (const auto& Model : CameraModels) Probabilities.push_back((double)Model->Freq);
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

	UCameraBase* ActiveModel = ModelManager->GetActiveModel();
	if (!ActiveModel)
	{
		return;
	}
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


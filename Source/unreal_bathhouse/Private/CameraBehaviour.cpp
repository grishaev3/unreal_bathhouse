#include "CameraBehaviour.h"
#include "BathhouseMath.h"
#include "UniqueRandom.h"
#include "BoundManager.h"
#include "StaticCamera.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

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

/// <summary>
/// Точка синхронизации координат Unity <=> UE
/// </summary>
/// <param name="DeltaTime"></param>
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

	// 1. Получаем чистые Unity-координаты из математической модели
	TPair<FUnityVector, FUnityVector> TransformPair = ActiveModel->Invoke(NormalizedTime);
	FUnityVector UnityFrom = TransformPair.Key;
	FUnityVector UnityAt = TransformPair.Value;

	// 2. ТОЧКА СИНХРОНИЗАЦИИ: Конвертируем в UE5 только в момент применения к трансформу
	FVector UELocation = UnityFrom.ToUE(true);  // Перевод позиции (метры -> см + оси)
	FVector UELookAt = UnityAt.ToUE(true);    // Перевод точки взгляда

	// 3. Применяем к Актору в Unreal Engine 5
	SetActorLocation(UELocation);
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(UELocation, UELookAt);
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


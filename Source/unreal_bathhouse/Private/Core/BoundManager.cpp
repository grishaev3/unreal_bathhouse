#include "BoundManager.h"
#include "UnityVector.h"
#include "DirectorCameraTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Algo/SelectRandomWeighted.h"

// --- REALIZATION: FBoundManager ---
FBoxBounds FBoundManager::FromZeroUE(FUnityVector Center, FUnityVector Size)
{
	FBoxBounds B;

	B.Min = FUnityVector(-Size.X / 2.0f, Center.Y, Center.Z + (-Size.Z / 2.0f));
	B.Max = FUnityVector(+Size.X / 2.0f, Center.Y + Size.Y, Center.Z + (+Size.Z / 2.0f));
	B.Size = Size;

	return B;
}

FBoxBounds FBoundManager::FromMinMaxUE(FUnityVector Min, FUnityVector Max)
{
	FBoxBounds B;

	B.Min = FUnityVector(
		FMath::Min(Min.X, Max.X),
		FMath::Min(Min.Y, Max.Y),
		FMath::Min(Min.Z, Max.Z)
	);

	B.Max = FUnityVector(
		FMath::Max(Min.X, Max.X),
		FMath::Max(Min.Y, Max.Y),
		FMath::Max(Min.Z, Max.Z)
	);

	B.Size = B.Max - B.Min;

	return B;
}

FBoundManager::FBoundManager()
{
	// Инициализируем дефолтный мувсет (в UE MaxValue заменяем на BIG_NUMBER)
	TArray<FUnityVector> DefaultMoveset = {
		FUnityVector(+1.0f, 0.5f, +BIG_NUMBER),
		FUnityVector(+1.0f, 0.5f, -BIG_NUMBER),
		FUnityVector(-1.0f, 0.5f, +BIG_NUMBER),
		FUnityVector(-1.0f, 0.5f, -BIG_NUMBER),

		FUnityVector(+1.5f, +1.0f, +BIG_NUMBER),
		FUnityVector(-1.5f, +1.0f, -BIG_NUMBER),
		FUnityVector(+1.5f, -1.0f, +BIG_NUMBER),
		FUnityVector(-1.5f, -1.0f, -BIG_NUMBER),
	};

	// Наполняем массив зон с автоматической конвертацией координат под UE5
	//BoundsList.Add(FBoundParameters{ FromZeroUE(FUnityVector(0.0f, 0.35f, -2.0f), FUnityVector(5.0f, 2.40f, 3.0f)), DefaultMoveset, TEXT("Внутри дома 1-ый эт.") });
	//BoundsList.Add(FBoundParameters{ FromZeroUE(FUnityVector(0.0f, 3.35f, -2.0f), FUnityVector(3.40f, 2.00f, 3.0f)), DefaultMoveset, TEXT("Внутри дома 2-ой эт.") });
	BoundsList.Add(FBoundParameters{ FromZeroUE(FUnityVector(0.0f, 0.5f, -2.0f), FUnityVector(8.0f, 6.0f, 12.0f)), DefaultMoveset, TEXT("Глобальный объём") });
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
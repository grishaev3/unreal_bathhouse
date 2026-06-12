#include "UnityVector.h"
#include "BoundManager.h"
#include "DirectorCameraTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Algo/SelectRandomWeighted.h"

// --- REALIZATION: FBoundManager ---
FBoxBounds FBoundManager::FromZeroUE(FUnityVector Center, FUnityVector Size)
{
	// Переводим метры из Unity в сантиметры Unreal Engine 5
	Center *= 100.0f;
	Size *= 100.0f;

	FBoxBounds B;

	// Перенос осей: Unity Z,X,Y -> UE5 X,Y,Z
	FUnityVector AdjustedCenter = FUnityVector(Center.Z, Center.X, Center.Y);
	FUnityVector AdjustedSize = FUnityVector(Size.Z, Size.X, Size.Y);

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

FBoxBounds FBoundManager::FromMinMaxUE(FUnityVector Min, FUnityVector Max)
{
	Min *= 100.0f;
	Max *= 100.0f;

	FBoxBounds B;
	// Разворачиваем оси для векторов Min и Max по тому же правилу
	FUnityVector AdjustedMin = FUnityVector(Min.Z, Min.X, Min.Y);
	FUnityVector AdjustedMax = FUnityVector(Max.Z, Max.X, Max.Y);

	// Выравниваем, чтобы Min гарантированно был меньше Max по всем осям в UE5
	B.Min = FUnityVector(FMath::Min(AdjustedMin.X, AdjustedMax.X), FMath::Min(AdjustedMin.Y, AdjustedMax.Y), FMath::Min(AdjustedMin.Z, AdjustedMax.Z));
	B.Max = FUnityVector(FMath::Max(AdjustedMin.X, AdjustedMax.X), FMath::Max(AdjustedMin.Y, AdjustedMax.Y), FMath::Max(AdjustedMin.Z, AdjustedMax.Z));
	B.Size = B.Max - B.Min;
	return B;
}

FBoundManager::FBoundManager()
{
	// Инициализируем дефолтный мувсет (в UE MaxValue заменяем на BIG_NUMBER)
	TArray<FUnityVector> DefaultMoveset = {
		FUnityVector(1.0f, 0.5f, BIG_NUMBER),
		FUnityVector(1.0f, 0.5f, -BIG_NUMBER),
		FUnityVector(-1.0f, 0.5f, BIG_NUMBER),
		FUnityVector(-1.0f, 0.5f, -BIG_NUMBER)
	};

	// Наполняем массив зон с автоматической конвертацией координат под UE5
	BoundsList.Add(FBoundParameters{ FromZeroUE(FUnityVector(0.0f, 0.35f, -2.0f), FUnityVector(5.0f, 2.40f, 3.0f)), DefaultMoveset, TEXT("Внутри дома 1-ый эт.") });
	BoundsList.Add(FBoundParameters{ FromZeroUE(FUnityVector(0.0f, 3.35f, -2.0f), FUnityVector(3.40f, 2.00f, 3.0f)), DefaultMoveset, TEXT("Внутри дома 2-ой эт.") });
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
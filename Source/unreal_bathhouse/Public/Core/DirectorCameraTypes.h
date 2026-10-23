#pragma once

#include "CoreMinimal.h"
#include "UnityVector.h"

// Все координаты — в системе Unity (метры, Y — вверх). В UE переводятся только в ACameraBehaviour.

struct FBoxBounds
{
    FUnityVector Min;
    FUnityVector Max;
    FUnityVector Size;
};

// Объём, в котором летает камера
struct FBoundParameters
{
    FBoxBounds Bound;

    // Направление пролёта: компонента ±1 — ось движения, ±UE_BIG_NUMBER — граница объёма по Z.
    // Используется только первый элемент.
    TArray<FUnityVector> CameraMovesets;

    FString Description;
};

// Положение камеры и точка, на которую она смотрит
struct FCameraPose
{
    FUnityVector From;
    FUnityVector At;
};

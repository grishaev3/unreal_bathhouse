#pragma once

#include "Math/Vector.h" // Вместо CoreMinimal.h — дает доступ к FVector и FORCEINLINE
#include "Math/UnrealMathUtility.h"
#include "DirectorCameraTypes.h"

class FBathhouseMath
{
public:
    FORCEINLINE static FVector GetRandomVectorInBounds(const FBoxBounds& Bounds)
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
};


#pragma once

#include "CoreMinimal.h"
#include "UnityVector.h"
#include "DirectorCameraTypes.h"

class FBathhouseMath
{
public:
    FORCEINLINE static FUnityVector GetRandomVectorInBounds(const FBoxBounds& Bounds)
    {
        return FUnityVector(
            FMath::RandRange(Bounds.Min.X, Bounds.Max.X),
            FMath::RandRange(Bounds.Min.Y, Bounds.Max.Y),
            FMath::RandRange(Bounds.Min.Z, Bounds.Max.Z)
        );
    }
};


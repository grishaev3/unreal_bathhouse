#pragma once

#include "Math/Vector.h"
#include "DirectorCameraTypes.h"

class UCameraBase
{
public:
    virtual ~UCameraBase() = default;

    ECameraDirectionType DirectionType;
    float Freq;
    float Duration;
    FString Name;
    int32 Index;

    FVector From;
    FVector At;

    FCameraMathFunc FuncLookFrom;
    FCameraMathFunc FuncLookAt;

    virtual FVector GetA() const { return FVector::ZeroVector; }
    virtual FVector GetB() const { return FVector::ZeroVector; }
    virtual bool UsesBounds() const { return true; }

    TPair<FVector, FVector> Invoke(float NormalizedTime);

    virtual void Reset(const FBoundParameters& Bounds) = 0;
};
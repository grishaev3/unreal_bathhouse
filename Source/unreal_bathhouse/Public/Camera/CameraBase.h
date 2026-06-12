#pragma once

#include "UnityVector.h"
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

    FUnityVector  From;
    FUnityVector  At;

    FCameraMathFunc FuncLookFrom;
    FCameraMathFunc FuncLookAt;

    virtual FUnityVector GetA() const { return FUnityVector::ZeroVector; }
    virtual FUnityVector GetB() const { return FUnityVector::ZeroVector; }
    virtual bool UsesBounds() const { return true; }

    TPair<FUnityVector, FUnityVector> Invoke(float NormalizedTime);

    virtual void Reset(const FBoundParameters& Bounds) = 0;
};
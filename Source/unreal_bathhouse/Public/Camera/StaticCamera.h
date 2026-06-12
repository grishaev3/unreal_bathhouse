#pragma once

#include "Math/Vector.h"
#include "CameraBase.h"
#include "DirectorCameraTypes.h"

class FStaticCamera : public UCameraBase
{
public:
    FVector A;
    FVector B;

    FStaticCamera(float InFreq, float InDuration, FCameraMathFunc InFuncLookFrom, FCameraMathFunc InFuncLookTo, FVector InA, FVector InB, FString InName);

    virtual FVector GetA() const override { return A; }
    virtual FVector GetB() const override { return B; }
    virtual bool UsesBounds() const override { return false; }

    virtual void Reset(const FBoundParameters& Bounds) override {}
};
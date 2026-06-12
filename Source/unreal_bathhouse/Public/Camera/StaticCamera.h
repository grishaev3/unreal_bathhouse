#pragma once

#include "UnityVector.h"
#include "Math/Vector.h"
#include "CameraBase.h"
#include "DirectorCameraTypes.h"

class FStaticCamera : public UCameraBase
{
public:
    FUnityVector A;
    FUnityVector B;

    FStaticCamera(float InFreq, float InDuration, FCameraMathFunc InFuncLookFrom, FCameraMathFunc InFuncLookTo, FUnityVector InA, FUnityVector InB, FString InName);

    virtual FUnityVector GetA() const override { return A; }
    virtual FUnityVector GetB() const override { return B; }
    virtual bool UsesBounds() const override { return false; }

    virtual void Reset(const FBoundParameters& Bounds) override {}
};
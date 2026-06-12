#pragma once

#include "UnityVector.h"
#include "Math/Vector.h"
#include "DirectorCameraTypes.h"
#include "CameraBase.h"

class FLinearBase : public UCameraBase
{
public:
    FUnityVector A;
    FUnityVector B;

    FLinearBase(float InFreq, float InDuration, FCameraMathFunc InFunc, FString InName, const FBoundParameters& Bounds, ECameraDirectionType InDirType);

    static EDirectionType GetDirectionType(const FUnityVector& V);

    static bool IsDirection(float Value);

    static bool IsBound(float Value);

    virtual FUnityVector GetA() const override { return A; }
    virtual FUnityVector GetB() const override { return B; }

    virtual void Reset(const FBoundParameters& BoundParameters) override;
};

#pragma once

#include "Math/Vector.h"
#include "DirectorCameraTypes.h"
#include "CameraBase.h"

class FLinearBase : public UCameraBase
{
public:
    FVector A;
    FVector B;

    FLinearBase(float InFreq, float InDuration, FCameraMathFunc InFunc, FString InName, const FBoundParameters& Bounds, ECameraDirectionType InDirType);

    static EDirectionType GetDirectionType(const FVector& V);

    static bool IsDirection(float Value);

    static bool IsBound(float Value);

    virtual FVector GetA() const override { return A; }
    virtual FVector GetB() const override { return B; }

    virtual void Reset(const FBoundParameters& BoundParameters) override;
};

#pragma once

#include "Math/Vector.h"
#include "LinearBase.h"

class FLinearRandom : public FLinearBase
{
public:
    FLinearRandom(float InFreq, float InDuration, FCameraMathFunc InFunc, FString InName, const FBoundParameters& Bounds, ECameraDirectionType InDirType);

    virtual void Reset(const FBoundParameters& BoundParameters) override;
};
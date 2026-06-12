#include "LinearRandom.h"
#include "DirectorCameraTypes.h"
#include "BathhouseMath.h"

// --- REALIZATION: FLinearRandom ---
FLinearRandom::FLinearRandom(float InFreq, float InDuration, FCameraMathFunc InFunc, FString InName, const FBoundParameters& Bounds, ECameraDirectionType InDirType)
	: FLinearBase(InFreq, InDuration, InFunc, InName, Bounds, InDirType)
{
	Reset(Bounds);
}

void FLinearRandom::Reset(const FBoundParameters& BoundParameters)
{
	A = FBathhouseMath::GetRandomVectorInBounds(BoundParameters.Bound);
	B = FBathhouseMath::GetRandomVectorInBounds(BoundParameters.Bound);
}

#include "BathhouseMath.h"
#include "CameraBase.h"

// --- REALIZATION: UCameraBase ---
TPair<FUnityVector, FUnityVector> UCameraBase::Invoke(float NormalizedTime)
{
	if (FuncLookFrom)
	{
		From = FuncLookFrom(NormalizedTime, this);
	}

	if (FuncLookAt)
	{
		At = FuncLookAt(NormalizedTime, this);
	}
	else if (DirectionType == ECameraDirectionType::Center)
	{
		At = FUnityVector(From.X, From.Y, -2.0f);
	}
	else if (DirectionType == ECameraDirectionType::Direct)
	{
		At = FUnityVector(From.X, From.Y, From.Z - 1.0f);
	}

	return TPair<FUnityVector, FUnityVector>(From, At);
}
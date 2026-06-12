#include "StaticCamera.h"

// --- REALIZATION: FStaticCamera ---
FStaticCamera::FStaticCamera(float InFreq, float InDuration, FCameraMathFunc InFuncLookFrom, FCameraMathFunc InFuncLookTo, FUnityVector InA, FUnityVector InB, FString InName)
{
	Freq = InFreq;
	A = InA;
	B = InB;
	FuncLookFrom = InFuncLookFrom;
	FuncLookAt = InFuncLookTo;
	Duration = InDuration;
	Name = InName;
}
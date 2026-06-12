#pragma once

#include "CoreMinimal.h"
#include "DirectorCameraTypes.h"

class FBoundManager
{
private:
    int CurrentBoundIndex = 0;

    TArray<FBoundParameters> BoundsList;

    static FBoxBounds FromZeroUE(FUnityVector Center, FUnityVector Size);

    static FBoxBounds FromMinMaxUE(FUnityVector Min, FUnityVector Max);

public:
    FBoundManager();

    FBoundParameters GetActiveBound() const { return BoundsList[CurrentBoundIndex]; }

    void Reset(FString Description);
};
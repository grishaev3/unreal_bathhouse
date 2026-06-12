#pragma once

#include "CoreMinimal.h"
#include "DirectorCameraTypes.h"

class FBoundManager
{
private:
    int CurrentBoundIndex = 0;

    TArray<FBoundParameters> BoundsList;

    // Конвертация Unity FromZero -> UE5
    // Unity Center (X, Y_высота, Z_вперед) и Size (X, Y_высота, Z_вперед)
    static FBoxBounds FromZeroUE(FVector Center, FVector Size);

    static FBoxBounds FromMinMaxUE(FVector Min, FVector Max);

public:
    FBoundManager();

    FBoundParameters GetActiveBound() const { return BoundsList[CurrentBoundIndex]; }

    void Reset(FString Description);
};
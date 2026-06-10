#pragma once

#include "CoreMinimal.h"
#include <vector>
#include <numeric>

class FUniqueRandom
{
private:
    FString Name;
    std::vector<double> CumulativeProbs;
    std::vector<int32> Values;

public:
    FUniqueRandom(FString InName, int32 MinValue, int32 MaxValue, const std::vector<double>& Probabilities);

    int32 Next(int32 PreviousValue = -1);

    static bool NextBool();
};

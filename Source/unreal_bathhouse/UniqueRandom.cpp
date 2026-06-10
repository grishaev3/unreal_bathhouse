#include "UniqueRandom.h"

FUniqueRandom::FUniqueRandom(FString InName, int32 MinValue, int32 MaxValue, const std::vector<double>& Probabilities)
{
    Name = InName;

    // Заполнение диапазона значений (Аналог Enumerable.Range)
    for (int32 i = MinValue; i < MaxValue; ++i)
    {
        Values.push_back(i);
    }

    double TotalProb = 0.0;
    for (double P : Probabilities) TotalProb += P;

    double Sum = 0.0;
    for (double P : Probabilities)
    {
        Sum += P / TotalProb;
        CumulativeProbs.push_back(Sum);
    }
}

int32 FUniqueRandom::Next(int32 PreviousValue)
{
    int32 Result = -1;
    do
    {
        double R = FMath::FRand(); // Аналог NextDouble() от 0.0 до 1.0

        // Бинарный поиск верхнего предела (Аналог вашего цикла while(left <= right))
        auto It = std::lower_bound(CumulativeProbs.begin(), CumulativeProbs.end(), R);
        int32 Index = std::distance(CumulativeProbs.begin(), It);

        // Защита от выхода за границы массива
        if (Index >= Values.size()) Index = Values.size() - 1;

        Result = Values[Index];
    } while (Result == PreviousValue && Values.size() > 1);

    return Result;
}

bool FUniqueRandom::NextBool()
{
    return FMath::RandRange(0, 100) % 2 == 0;
}
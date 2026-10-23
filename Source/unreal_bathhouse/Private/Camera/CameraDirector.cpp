#include "CameraDirector.h"

#include "Engine/Engine.h"

namespace
{
    constexpr float ShotDuration = 10.0f;   // секунды

    // Порог окончания плана по нормализованному времени
    constexpr float ShotEndThreshold = 0.999f;

    // Объём по центру (X, Z) с нижней гранью на высоте Center.Y
    FBoxBounds BoxFromCenterAndBottom(const FUnityVector& Center, const FUnityVector& Size)
    {
        FBoxBounds B;
        B.Min = FUnityVector(-Size.X / 2.0f, Center.Y, Center.Z - Size.Z / 2.0f);
        B.Max = FUnityVector(+Size.X / 2.0f, Center.Y + Size.Y, Center.Z + Size.Z / 2.0f);
        B.Size = Size;
        return B;
    }
}

FCameraDirector::FCameraDirector()
{
    Bounds.Bound = BoxFromCenterAndBottom(FUnityVector(0.0f, 0.5f, -2.0f), FUnityVector(8.0f, 6.0f, 12.0f));
    Bounds.Description = TEXT("Глобальный объём");
    Bounds.CameraMovesets = {
        FUnityVector(+1.0f, 0.5f, +UE_BIG_NUMBER),
        FUnityVector(+1.0f, 0.5f, -UE_BIG_NUMBER),
        FUnityVector(-1.0f, 0.5f, +UE_BIG_NUMBER),
        FUnityVector(-1.0f, 0.5f, -UE_BIG_NUMBER),

        FUnityVector(+1.5f, +1.0f, +UE_BIG_NUMBER),
        FUnityVector(-1.5f, +1.0f, -UE_BIG_NUMBER),
        FUnityVector(+1.5f, -1.0f, +UE_BIG_NUMBER),
        FUnityVector(-1.5f, -1.0f, -UE_BIG_NUMBER),
    };

    // Объёмы внутри дома (пока не используются):
    //   центр (0, 0.35, -2), размер (5, 2.40, 3) — 1-й этаж
    //   центр (0, 3.35, -2), размер (3.40, 2.00, 3) — 2-й этаж

    constexpr float OftenWeight = 0.5f;
    constexpr float RarelyWeight = 0.3f;
    constexpr float StaticWeight = (1.0f - (OftenWeight + RarelyWeight)) / 6.0f;

    const FUnityVector StaticFrom(-0.63f, 1.97f, +6.00f);
    const FUnityVector StaticLeft(-5.0f, 1.97f, 0.0f);
    const FUnityVector StaticRight(+5.0f, 1.97f, 0.0f);

    Shots.Add(MakeUnique<FLinearShot>(TEXT("Linear"), OftenWeight, ShotDuration));
    Shots.Add(MakeUnique<FRandomShot>(TEXT("Random"), RarelyWeight, ShotDuration));
    Shots.Add(MakeUnique<FStaticShot>(TEXT("Static0"), StaticWeight, ShotDuration, StaticFrom, StaticLeft, StaticRight));
    Shots.Add(MakeUnique<FStaticShot>(TEXT("Static1"), StaticWeight, ShotDuration, StaticFrom, StaticRight, StaticLeft));

    for (const TUniquePtr<FCameraShot>& Shot : Shots)
    {
        if (Shot->UsesBounds())
        {
            Shot->Reset(Bounds);
        }
    }

    ActiveIndex = PickShotIndex(INDEX_NONE);

    // Как и раньше, стартовый план сразу сменяется следующим
    SwitchToNextShot();
}

FCameraPose FCameraDirector::Tick(float DeltaSeconds, float TimeDilation)
{
    const FCameraShot& Shot = *Shots[ActiveIndex];

    // DeltaSeconds уже учитывает замедление времени, и TimeDilation применяется повторно.
    // Сохранено из исходной логики; при замедлении 1 разницы нет.
    ElapsedSeconds += DeltaSeconds;
    const float T = (float)(ElapsedSeconds * TimeDilation / Shot.Duration);

    const FCameraPose Pose = Shot.Evaluate(T);

    if (T >= ShotEndThreshold)
    {
        SwitchToNextShot();
    }

    return Pose;
}

void FCameraDirector::SwitchToNextShot()
{
    ElapsedSeconds = 0.0;
    ActiveIndex = PickShotIndex(ActiveIndex);

    FCameraShot& Shot = *Shots[ActiveIndex];
    if (Shot.UsesBounds())
    {
        Shot.Reset(Bounds);
    }

    const FString Message = FString::Printf(TEXT("Camera Period Ended. New Model: %s %s"), *Shot.Name, *Bounds.Description);
    UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, Message);
    }
}

int32 FCameraDirector::PickShotIndex(int32 Previous) const
{
    double TotalWeight = 0.0;
    for (const TUniquePtr<FCameraShot>& Shot : Shots)
    {
        TotalWeight += Shot->Weight;
    }

    int32 Result;
    do
    {
        // Первый план, у которого накопленная вероятность >= R
        const double R = FMath::FRand();
        double Cumulative = 0.0;
        Result = Shots.Num() - 1;
        for (int32 i = 0; i < Shots.Num(); ++i)
        {
            Cumulative += Shots[i]->Weight / TotalWeight;
            if (Cumulative >= R)
            {
                Result = i;
                break;
            }
        }
    }
    while (Result == Previous && Shots.Num() > 1);

    return Result;
}

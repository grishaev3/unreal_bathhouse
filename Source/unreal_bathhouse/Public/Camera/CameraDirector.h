#pragma once

#include "CoreMinimal.h"
#include "CameraShots.h"

// Выбирает планы камеры по весам (без повтора подряд), ведёт время плана и переключает их
class FCameraDirector
{
public:
    FCameraDirector();

    // DeltaSeconds — время кадра мира, TimeDilation — глобальное замедление.
    // Возвращает позу камеры на этот кадр; по окончании плана сразу выбирает следующий.
    FCameraPose Tick(float DeltaSeconds, float TimeDilation);

private:
    void SwitchToNextShot();
    int32 PickShotIndex(int32 Previous) const;

    TArray<TUniquePtr<FCameraShot>> Shots;
    FBoundParameters Bounds;
    int32 ActiveIndex = INDEX_NONE;
    double ElapsedSeconds = 0.0;
};

#pragma once

#include "CoreMinimal.h"
#include "CameraBase.h"

class FTimeManager
{
private:
    int32 CurrentHour;
    double MsCurrentTime = 0.0;
    double NormalizedTime = 0.0;
    UWorld* WorldRef;

public:
    FTimeManager(UWorld* InWorld);

    int32 GetCurrentHour() const { return CurrentHour; }

    float GetNormalizedTime() const { return (float)NormalizedTime; }

    void UpdateNormalizedTime(UCameraBase* Model, float& OutNormalizedTime);

    ESunCircle GetSunCircle() const;

    void Reset(double D = 0.0);
};
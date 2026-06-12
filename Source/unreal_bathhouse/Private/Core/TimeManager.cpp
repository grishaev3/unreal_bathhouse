#include "TimeManager.h"
#include "Kismet/GameplayStatics.h"

// --- REALIZATION: FTimeManager ---
FTimeManager::FTimeManager(UWorld* InWorld)
{
	WorldRef = InWorld;
	CurrentHour = 12;
	MsCurrentTime = 0.0;
	NormalizedTime = 0.0;
}

void FTimeManager::UpdateNormalizedTime(UCameraBase* Model, float& OutNormalizedTime)
{
	if (!Model || !WorldRef) return;
	float DeltaTime = WorldRef->GetDeltaSeconds();
	float TimeScale = UGameplayStatics::GetGlobalTimeDilation(WorldRef);

	MsCurrentTime += DeltaTime * 1000.0;
	double ModelMsDuration = Model->Duration * 1000.0;
	NormalizedTime = MsCurrentTime / (ModelMsDuration / (1.0 / TimeScale));
	OutNormalizedTime = (float)NormalizedTime;
}

ESunCircle FTimeManager::GetSunCircle() const
{
	return (CurrentHour >= 4 && CurrentHour <= 20) ? ESunCircle::Day : ESunCircle::Night;
}

void FTimeManager::Reset(double D) { MsCurrentTime = D; }

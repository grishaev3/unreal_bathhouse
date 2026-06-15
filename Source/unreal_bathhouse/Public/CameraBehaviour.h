#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "TimeManager.h"          
#include "BoundManager.h"
#include "CameraModelManager.h"

#include <memory>
#include "CameraBehaviour.generated.h"

UCLASS()
class UNREAL_BATHHOUSE_API ACameraBehaviour : public AActor
{
	GENERATED_BODY()

public:
	ACameraBehaviour();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")

	class UCameraComponent* CameraComponent;

private:
	void OnPeriodEnd(float NormalizedTime);

	TUniquePtr<FTimeManager> TimeManager;
	TUniquePtr<FBoundManager> BoundManager;
	TUniquePtr<FCameraModelManager> ModelManager;
};

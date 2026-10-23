#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraDirector.h"

#include "CameraBehaviour.generated.h"

// Актор-камера: каждый кадр берёт позу у FCameraDirector и применяет её
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
	TUniquePtr<FCameraDirector> Director;
};

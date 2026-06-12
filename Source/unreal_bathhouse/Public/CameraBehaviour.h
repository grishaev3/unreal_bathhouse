#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DirectorCameraTypes.h"
#include "UniqueRandom.h"
#include "BoundManager.h"
#include "TimeManager.h"
#include "LinearBase.h"
#include "LinearRandom.h"
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

    std::unique_ptr<FTimeManager> TimeManager;
    std::unique_ptr<class FBoundManager> BoundManager; // class-forward declaration, так как он в другом файле
    std::unique_ptr<FCameraModelManager> ModelManager;
};

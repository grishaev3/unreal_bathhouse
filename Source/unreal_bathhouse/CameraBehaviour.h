#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DirectorCameraTypes.h"
#include "UniqueRandom.h"
#include <memory>
#include "CameraBehaviour.generated.h"

FORCEINLINE FVector GetRandomVectorInBounds(const FBoxBounds& Bounds);

FORCEINLINE static FVector ConvertUnityToUE(const FVector& UnityVector, bool bIsLocation = true);

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

class UCameraBase
{
public:
    virtual ~UCameraBase() = default;

    ECameraDirectionType DirectionType;
    float Freq;
    FString Name;
    int32 Index;
    float Duration;

    FVector From;
    FVector At;

    FCameraMathFunc FuncLookFrom;
    FCameraMathFunc FuncLookAt;

    virtual FVector GetA() const { return FVector::ZeroVector; }
    virtual FVector GetB() const { return FVector::ZeroVector; }
    virtual bool UsesBounds() const { return true; }

    TPair<FVector, FVector> Invoke(float NormalizedTime);

    virtual void Reset(const FBoundParameters& Bounds) = 0;
};

class FLinearBase : public UCameraBase
{
public:
    FVector A;
    FVector B;

    FLinearBase(float InFreq, float InDuration, FCameraMathFunc InFunc, FString InName, const FBoundParameters& Bounds, ECameraDirectionType InDirType);

    static EDirectionType GetDirectionType(const FVector& V);

    static bool IsDirection(float Value);

    static bool IsBound(float Value);

    virtual FVector GetA() const override { return A; }
    virtual FVector GetB() const override { return B; }

    virtual void Reset(const FBoundParameters& BoundParameters) override;
};

class FLinearRandom : public FLinearBase
{
public:
    FLinearRandom(float InFreq, float InDuration, FCameraMathFunc InFunc, FString InName, const FBoundParameters& Bounds, ECameraDirectionType InDirType);

    virtual void Reset(const FBoundParameters& BoundParameters) override;
};

class FStaticCamera : public UCameraBase
{
public:
    FVector A;
    FVector B;

    FStaticCamera(float InFreq, float InDuration, FCameraMathFunc InFuncLookFrom, FCameraMathFunc InFuncLookTo, FVector InA, FVector InB, FString InName);

    virtual FVector GetA() const override { return A; }
    virtual FVector GetB() const override { return B; }
    virtual bool UsesBounds() const override { return false; }

    virtual void Reset(const FBoundParameters& Bounds) override {}
};

enum class ESunCircle : uint8 { Day, Night };

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

class FCameraModelManager
{
private:
    int32 CurrentModelIndex;
    
    TArray<std::shared_ptr<UCameraBase>> CameraModels;

    std::unique_ptr<class FUniqueRandom> UniqueRandomProvider; // Используем class-forward declaration

    static FVector LinearLerp(float NormalizedTime, UCameraBase* Model);

public:
    FCameraModelManager(FBoundParameters& BoundParameters);

    UCameraBase* GetActiveModel() const;

    bool ActiveModelUsesBounds() const;

    void ResetModel();
};

// --- ГЛАВНЫЙ АКТОР КАМЕРЫ ---
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

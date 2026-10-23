#pragma once

#include "CoreMinimal.h"
#include "DirectorCameraTypes.h"

// План (шот) камеры: как она движется и куда смотрит за время Duration
class FCameraShot
{
public:
    FCameraShot(const FString& InName, float InWeight, float InDuration)
        : Name(InName), Weight(InWeight), Duration(InDuration) {}

    virtual ~FCameraShot() = default;

    const FString Name;
    const float Weight;     // относительная вероятность выбора
    const float Duration;   // секунды

    // Нужен ли плану объём; если да — перед показом вызывается Reset
    virtual bool UsesBounds() const { return true; }

    virtual void Reset(const FBoundParameters& Bounds) {}

    // T — нормализованное время плана, 0..1 (к концу может немного превысить 1)
    virtual FCameraPose Evaluate(float T) const = 0;
};

// Прямой пролёт вдоль границы объёма; взгляд параллельно оси в сторону плоскости Z = -2
class FLinearShot : public FCameraShot
{
public:
    using FCameraShot::FCameraShot;

    virtual void Reset(const FBoundParameters& Bounds) override;
    virtual FCameraPose Evaluate(float T) const override;

protected:
    FUnityVector A;
    FUnityVector B;
};

// Прямой пролёт между двумя случайными точками объёма
class FRandomShot : public FLinearShot
{
public:
    using FLinearShot::FLinearShot;

    virtual void Reset(const FBoundParameters& Bounds) override;
};

// Неподвижная камера, взгляд плавно переходит из LookA в LookB
class FStaticShot : public FCameraShot
{
public:
    FStaticShot(const FString& InName, float InWeight, float InDuration,
                const FUnityVector& InFrom, const FUnityVector& InLookA, const FUnityVector& InLookB)
        : FCameraShot(InName, InWeight, InDuration), From(InFrom), LookA(InLookA), LookB(InLookB) {}

    virtual bool UsesBounds() const override { return false; }
    virtual FCameraPose Evaluate(float T) const override;

private:
    FUnityVector From;
    FUnityVector LookA;
    FUnityVector LookB;
};

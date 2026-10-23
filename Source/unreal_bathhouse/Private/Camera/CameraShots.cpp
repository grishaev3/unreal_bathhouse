#include "CameraShots.h"

namespace
{
    // Плоскость, в сторону которой смотрят пролёты (Unity Z)
    constexpr float LookPlaneZ = -2.0f;

    bool IsAxis(float Value) { return FMath::Abs(Value) == 1.0f; }
    bool IsBoundMarker(float Value) { return FMath::Abs(Value) == UE_BIG_NUMBER; }

    FUnityVector RandomPointInBounds(const FBoxBounds& Bounds)
    {
        return FUnityVector(
            FMath::RandRange(Bounds.Min.X, Bounds.Max.X),
            FMath::RandRange(Bounds.Min.Y, Bounds.Max.Y),
            FMath::RandRange(Bounds.Min.Z, Bounds.Max.Z));
    }
}

// --- FLinearShot ---

void FLinearShot::Reset(const FBoundParameters& Bounds)
{
    const FBoxBounds& Bound = Bounds.Bound;
    const FUnityVector Dir = Bounds.CameraMovesets.Num() > 0 ? Bounds.CameraMovesets[0] : FUnityVector::ZeroVector;

    // Глубина по Z: ближняя или дальняя граница объёма
    const float DepthZ = IsBoundMarker(Dir.Z) ? (Dir.Z > 0.0f ? Bound.Max.Z : Bound.Min.Z) : 0.0f;

    if (IsAxis(Dir.X))
    {
        // Вдоль X на высоте Dir.Y (доля высоты объёма)
        const float Start = Dir.X > 0.0f ? Bound.Min.X : Bound.Max.X;
        const float End   = Dir.X > 0.0f ? Bound.Max.X : Bound.Min.X;
        const float Height = Bound.Min.Y + Bound.Size.Y * Dir.Y;

        A = FUnityVector(Start, Height, DepthZ);
        B = FUnityVector(End, Height, DepthZ);
    }
    else if (IsAxis(Dir.Y))
    {
        // Вертикальный пролёт; X берётся из мувсета как есть
        const float Start = Dir.Y > 0.0f ? Bound.Min.Y : Bound.Max.Y;
        const float End   = Dir.Y > 0.0f ? Bound.Max.Y : Bound.Min.Y;

        A = FUnityVector(Dir.X, Start, DepthZ);
        B = FUnityVector(Dir.X, End, DepthZ);
    }
    else
    {
        // Пролёт по Z не реализован
        A = FUnityVector::ZeroVector;
        B = FUnityVector::ZeroVector;
    }
}

FCameraPose FLinearShot::Evaluate(float T) const
{
    const FUnityVector From = FUnityVector::Lerp(A, B, T);
    return { From, FUnityVector(From.X, From.Y, LookPlaneZ) };
}

// --- FRandomShot ---

void FRandomShot::Reset(const FBoundParameters& Bounds)
{
    A = RandomPointInBounds(Bounds.Bound);
    B = RandomPointInBounds(Bounds.Bound);
}

// --- FStaticShot ---

FCameraPose FStaticShot::Evaluate(float T) const
{
    return { From, FUnityVector::Lerp(LookA, LookB, T) };
}

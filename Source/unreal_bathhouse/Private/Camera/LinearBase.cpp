#include "LinearBase.h"

// --- REALIZATION: FLinearBase ---
FLinearBase::FLinearBase(float InFreq, float InDuration, FCameraMathFunc InFunc, FString InName, const FBoundParameters& Bounds, ECameraDirectionType InDirType)
{
	Freq = InFreq;
	DirectionType = InDirType;
	FuncLookFrom = InFunc;
	Duration = InDuration;
	Name = InName;
	Reset(Bounds);
}

bool FLinearBase::IsDirection(float Value) { return FMath::Abs(Value) == 1.0f; }

bool FLinearBase::IsBound(float Value) { return FMath::Abs(Value) == BIG_NUMBER; }

EDirectionType FLinearBase::GetDirectionType(const FUnityVector& V)
{
	if (IsDirection(V.X)) return EDirectionType::X;
	if (IsDirection(V.Y)) return EDirectionType::Y;
	if (IsDirection(V.Z)) return EDirectionType::Z;
	return EDirectionType::Zero;
}

void FLinearBase::Reset(const FBoundParameters& BoundParameters)
{
    FBoxBounds Bound = BoundParameters.Bound;

    // Защита от пустого мувсета: направление движения берем в исходном формате Unity
    FUnityVector UnityDirection = BoundParameters.CameraMovesets.Num() > 0 ? BoundParameters.CameraMovesets[0] : FUnityVector::ZeroVector;

    // Локальная лямбда-функция (Аналог CalculateDepth в C# Unity)
    // Рассчитывает глубину по оси Z в метрах
    auto CalculateDepthUnity = [&](const FBoxBounds& InBound) -> float
    {
        if (IsBound(UnityDirection.Z))
        {
            return (UnityDirection.Z > 0.0f) ? InBound.Max.Z : InBound.Min.Z;
        }
        return 0.0f;
    };

    float DepthZ = CalculateDepthUnity(Bound);
    float Start = 0.0f, End = 0.0f;

    // Полное соответствие оригинальной switch-логике из Unity
    switch (GetDirectionType(UnityDirection))
    {
    case EDirectionType::Zero:
        break;

    case EDirectionType::X: // Движение вдоль оси X (Ширина комнаты)
        if (UnityDirection.X > 0.0f)
        {
            Start = Bound.Min.X;
            End = Bound.Max.X;
        }
        else
        {
            Start = Bound.Max.X;
            End = Bound.Min.X;
        }
        {
            // В Unity высота — это ось Y. Рассчитываем её в метрах
            float HeightY = Bound.Min.Y + (Bound.Size.Y * UnityDirection.Y);

            // Формируем точки траектории: X движется, Y и Z зафиксированы
            A = FUnityVector(Start, HeightY, DepthZ);
            B = FUnityVector(End, HeightY, DepthZ);
        }
        break;

    case EDirectionType::Y: // Движение вдоль оси Y (Высота/Вертикальный пролет)
        if (UnityDirection.Y > 0.0f)
        {
            Start = Bound.Min.Y;
            End = Bound.Max.Y;
        }
        else
        {
            Start = Bound.Max.Y;
            End = Bound.Min.Y;
        }

        // Формируем точки траектории: Y движется, X и Z зафиксированы
        A = FUnityVector(UnityDirection.X, Start, DepthZ);
        B = FUnityVector(UnityDirection.X, End, DepthZ);
        break;

    case EDirectionType::Z:
        // Аналог throw new NotImplementedException() в Unity
        A = FUnityVector::ZeroVector;
        B = FUnityVector::ZeroVector;
        break;
    }
}

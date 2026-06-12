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

EDirectionType FLinearBase::GetDirectionType(const FVector& V)
{
	if (IsDirection(V.X)) return EDirectionType::X;
	if (IsDirection(V.Y)) return EDirectionType::Y;
	if (IsDirection(V.Z)) return EDirectionType::Z;
	return EDirectionType::Zero;
}

void FLinearBase::Reset(const FBoundParameters& BoundParameters)
{
	FBoxBounds Bound = BoundParameters.Bound;

	// Защита от пустого мувсета
	FVector UnityDirection = BoundParameters.CameraMovesets.Num() > 0 ? BoundParameters.CameraMovesets[0] : FVector::ZeroVector;

	// В Unity глубина рассчитывалась по Z. В UE5 это ось X (Вперед)
	auto CalculateDepthUE = [&](const FBoxBounds& InBound) -> float {
		if (IsBound(UnityDirection.Z))
		{
			return (UnityDirection.Z > 0.0f) ? InBound.Max.X : InBound.Min.X;
		}
		return InBound.Min.X + (InBound.Size.X / 2.0f); // По центру глубины, если не задано
	};

	float DepthX = CalculateDepthUE(Bound);
	float Start = 0.0f, End = 0.0f;

	// Переводим логику выбора осей из Unity в координаты UE5
	switch (GetDirectionType(UnityDirection))
	{
	case EDirectionType::X: // В Unity это была ширина (X). В UE5 это ось Y (Вправо)
		if (UnityDirection.X > 0.0f)
		{
			Start = Bound.Min.Y;
			End = Bound.Max.Y;
		}
		else
		{
			Start = Bound.Max.Y;
			End = Bound.Min.Y;
		}
		{
			// Высота в Unity (Y) стала высотой в UE5 (Z)
			float HeightZ = Bound.Min.Z + Bound.Size.Z * UnityDirection.Y;
			A = FVector(DepthX, Start, HeightZ);
			B = FVector(DepthX, End, HeightZ);
		}
		break;

	case EDirectionType::Y: // В Unity это была высота (Y). В UE5 это ось Z (Вверх)
		if (UnityDirection.Y > 0.0f)
		{
			Start = Bound.Min.Z;
			End = Bound.Max.Z;
		}
		else
		{
			Start = Bound.Max.Z;
			End = Bound.Min.Z;
		}
		// Ось X в Unity стала осью Y в UE5
		A = FVector(DepthX, UnityDirection.X * 100.0f, Start);
		B = FVector(DepthX, UnityDirection.X * 100.0f, End);
		break;

	default:
		// Если направление Zero, просто берем центр объема
		A = Bound.Min;
		B = Bound.Max;
		break;
	}
}

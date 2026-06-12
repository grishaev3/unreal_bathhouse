#include "CameraBase.h"

// --- REALIZATION: UCameraBase ---
TPair<FVector, FVector> UCameraBase::Invoke(float NormalizedTime)
{
	if (FuncLookFrom)
	{
		From = FuncLookFrom(NormalizedTime, this);
	}

	if (FuncLookAt)
	{
		At = FuncLookAt(NormalizedTime, this);
	}
	else if (DirectionType == ECameraDirectionType::Center)
	{
		// В Unity было: Vector3(from.x, from.y, -2f) -> где Z была глубиной.
		// В UE5 глубина/вперед — это ось X. Поэтому фиксируем ось X на значении центра сцены (например, -200 см),
		// а по горизонтали Y и высоте Z камера плавно следит за своими координатами.
		At = FVector(-200.0f, From.Y, From.Z);
	}
	else if (DirectionType == ECameraDirectionType::Direct)
	{
		// В Unity: Vector3(from.x, from.y, from.z - 1f) -> смотрела по оси Z назад.
		// В UE5: смотрим вперед по оси X (добавляем 100 см к X)
		At = FVector(From.X + 100.0f, From.Y, From.Z);
	}

	return TPair<FVector, FVector>(From, At);
}
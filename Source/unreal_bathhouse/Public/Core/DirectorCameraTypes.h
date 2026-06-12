#pragma once

#include "Math/Vector.h"
#include <functional>

// --- ПЕРЕЧИСЛЕНИЯ (С поддержкой Blueprints) ---
UENUM(BlueprintType)
enum class ECameraDirectionType : uint8
{
    Direct,
    Center
};

enum class EDirectionType : uint8
{
    Zero,
    X,
    Y,
    Z
};

enum class ESunCircle : uint8 
{
    Day,
    Night
};

// --- СТРУКТУРЫ ДАННЫХ ---
struct FBoxBounds
{
    FVector Min;
    FVector Max;
    FVector Size;
};

struct FBoundParameters
{
    FBoxBounds Bound;
    TArray<FVector> CameraMovesets;
    FString Description;
};

// --- ДЕЛЕГАТЫ И ОПЕРЕЖАЮЩИЕ ОБЪЯВЛЕНИЯ ---
class UCameraBase;

// Аналог C# Func<float, CameraBase, Vector3>
typedef std::function<FVector(float, UCameraBase*)> FCameraMathFunc;

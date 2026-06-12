#pragma once

#include "UnityVector.h"
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
    FUnityVector Min;
    FUnityVector Max;
    FUnityVector Size;
};

struct FBoundParameters
{
    FBoxBounds Bound;
    TArray<FUnityVector> CameraMovesets;
    FString Description;
};

// --- ДЕЛЕГАТЫ И ОПЕРЕЖАЮЩИЕ ОБЪЯВЛЕНИЯ ---
class UCameraBase;

// Аналог C# Func<float, CameraBase, Vector3>
typedef std::function<FUnityVector(float, UCameraBase*)> FCameraMathFunc;

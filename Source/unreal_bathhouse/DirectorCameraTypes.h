#pragma once

#include "CoreMinimal.h"
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

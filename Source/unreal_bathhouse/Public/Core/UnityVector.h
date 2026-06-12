#pragma once

#include "CoreMinimal.h"
#include "UnityVector.generated.h"

/**
 * Структура, хранящая координаты строго в системе Unity (Левосторонняя, метры).
 * Ось Y - вверх, ось Z - вперед.
 */
USTRUCT(BlueprintType)
struct FUnityVector
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unity Coordinates")
    float X = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unity Coordinates")
    float Y = 0.0f; // В Unity это ВВЕРХ

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unity Coordinates")
    float Z = 0.0f; // В Unity это ВПЕРЕД

    // Конструкторы
    FUnityVector() = default;
    FUnityVector(float InX, float InY, float InZ) : X(InX), Y(InY), Z(InZ) {}
    FUnityVector(const FUnityVector& UEVector) : X(UEVector.X), Y(UEVector.Y), Z(UEVector.Z) {}

    /**
     * Главный метод трансформации: вызывается ТОЛЬКО при финальном применении к Actors/Components в UE5.
     * Переводит: Unity(X, Y_высота, Z_вперед) -> UE5(X_вперед=Z, Y_вправо=X, Z_вверх=Y) * 100
     */
    FORCEINLINE FVector ToUE(bool bIsLocation = true) const
    {
        return bIsLocation ? FVector(Z, X, Y) * 100.0f : FVector(Z, X, Y);
    }

    // --- Математические операторы для сохранения совместимости с формулами Unity ---
    FORCEINLINE FUnityVector& operator*=(float Scalar)
    {
        X *= Scalar;
        Y *= Scalar;
        Z *= Scalar;
        return *this; // Возвращаем измененный объект
    }

    FORCEINLINE FUnityVector operator+(const FUnityVector& Other) const
    {
        return FUnityVector(X + Other.X, Y + Other.Y, Z + Other.Z);
    }
    FORCEINLINE FUnityVector operator-(const FUnityVector& Other) const
    {
        return FUnityVector(X - Other.X, Y - Other.Y, Z - Other.Z);
    }
    FORCEINLINE FUnityVector operator*(float Scalar) const
    {
        return FUnityVector(X * Scalar, Y * Scalar, Z * Scalar);
    }

    FORCEINLINE FUnityVector operator/(float Scalar) const
    {
        // Защита от деления на ноль (аналог безопасного деления в Unity)
        if (FMath::IsNearlyZero(Scalar))
        {
            return ZeroVector;
        }

        float InvScalar = 1.0f / Scalar;
        return FUnityVector(X * InvScalar, Y * InvScalar, Z * InvScalar);
    }

    FORCEINLINE FUnityVector& operator/=(float Scalar)
    {
        if (FMath::IsNearlyZero(Scalar))
        {
            X = 0.0f; Y = 0.0f; Z = 0.0f;
            return *this;
        }

        float InvScalar = 1.0f / Scalar;
        X *= InvScalar;
        Y *= InvScalar;
        Z *= InvScalar;
        return *this;
    }

    static const FUnityVector ZeroVector;

    // Аналог Vector3.Lerp в Unity, работающий внутри "родных" координат
    FORCEINLINE static FUnityVector Lerp(const FUnityVector& A, const FUnityVector& B, float T)
    {
        return FUnityVector(
            FMath::Lerp(A.X, B.X, T),
            FMath::Lerp(A.Y, B.Y, T),
            FMath::Lerp(A.Z, B.Z, T)
        );
    }
};

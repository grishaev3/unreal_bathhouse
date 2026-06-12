#pragma once

#include <limits>

// Константы, заменяющие «магические» числа.
namespace Config {
    // Максимальное значение для обозначения “не ограничено”
    constexpr float BIG_NUMBER = std::numeric_limits<float>::max();

    // Переход из метров в сантиметры (UE работает с cm)
    constexpr float METERS_TO_CM = 100.0f;

    // Частоты переключения моделей камеры
    constexpr float FREQ_OFTEN   = 0.5f;   // частая смена
    constexpr float FREQ_RARELY  = 0.3f;   // редкая смена

    // Статический коэффициент (1 - часто - редко) / 2
    constexpr float STATIC_FREQ_FACTOR = (1.0f - (FREQ_OFTEN + FREQ_RARELY)) / 2.0f;

    // Пример: единичное значение, чтобы избежать «1.0f» в коде
    constexpr float ONE_FLOAT   = 1.0f;
    constexpr float HALF_FLOAT  = 0.5f;
}

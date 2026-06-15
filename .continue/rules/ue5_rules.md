---
name: Unreal Engine 5 C++ Include Rules
---
При анализе заголовков C++ всегда проверяй:
1. Строка `#include "FileName.generated.h"` обязана идти самой последней.
2. Вместо подключения тяжелых инклудов классов используй forward declaration (`class AMyCharacter;`).
3. Заголовки должны идти от частных (проектных) к общим (движковым).
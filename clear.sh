#!/bin/bash

echo "=== Запуск очистки кэша Unreal Engine ==="

# Проверяем наличие .uproject в текущей папке
if ! ls *.uproject &> /dev/null; then
    echo ":: [ОШИБКА] Файл .uproject не найден!"
    echo ":: Перенесите этот скрипт в КОРНЕВУЮ папку проекта."
    exit 1
fi

# Массив папок для удаления
FOLDERS=("Binaries" "DerivedDataCache" "Intermediate" "Saved/StagedBuilds")

for folder in "${FOLDERS[@]}"; do
    if [ -d "$folder" ]; then
        echo ":: Удаление $folder..."
        rm -rf "$folder"
        echo "   [Успешно удалено]"
    else
        echo "   [Пропущено] Папка $folder не существует"
    fi
done

echo "=== Очистка завершена! Закройте терминал и запустите проект. ==="

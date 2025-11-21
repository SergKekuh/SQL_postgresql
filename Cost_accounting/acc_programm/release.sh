#!/bin/bash

# ANSI цвета
NC=$'\e[0m'
BLUE=$'\e[0;34m'
GREEN=$'\e[0;32m'
YELLOW=$'\e[1;33m'
RED=$'\e[0;31m'

# Функция show_progress_bar
show_progress_bar() {
    local cmd="$1"
    local msg="$2"

    echo -ne "${BLUE}${msg}...   ${NC}"
    eval "$cmd" > /dev/null 2>&1 &
    local pid=$!

    local progress=0
    local total=20

    while kill -0 "$pid" 2>/dev/null; do
        sleep 0.1
        ((progress++))
        if [ $progress -gt $total ]; then
            progress=$total
        fi

        # Выводим бар без переноса строки
        printf $'\r%s[%s%s] %s%3d%%%%%s' \
            "$BLUE" \
            "$(printf '#%.0s' $(seq 1 $progress))" \
            " " \
            "$YELLOW" \
            "$((progress * 5))" \
            "$NC"
    done

    wait $pid
    local exit_code=$?
    
    # Очистим текущую строку и покажем окончательный статус
    printf $'\r%s[%s] %s%3d%%%%  %s%s\n' \
        "$BLUE" \
        "$(printf '#%.0s' $(seq 1 $total))" \
        "$NC" \
        "100" \
        "$GREEN[OK]$NC"

    return $exit_code
}

# Переход в корень проекта
cd "$(dirname "$0")" || { echo -e "${RED}❌ Не могу перейти в папку проекта${NC}"; exit 1; }

echo -e "${BLUE}🚀 release.sh — полная очистка и сборка проекта (Finance Classifier)${NC}"

# --- ШАГ 1: Очистка и сборка через CMake ---
echo -e "${BLUE}🏗️ Собираем проект через CMake --fresh...${NC}"

rm -rf build && mkdir -p build
cd build || exit 1

show_progress_bar "cmake .." "CMake configure"
show_progress_bar "cmake --build ." "CMake build"

cd ..

# --- ШАГ 2: Запуск исполняемого файла ---
echo -e "${BLUE}▶️ Запускаем исполняемый файл: ./build/finance_classifier${NC}"

./build/finance_classifier

# Проверяем код завершения программы
EXIT_CODE=$?
if [ $EXIT_CODE -ne 0 ]; then
    echo -e "${RED}❌ Программа завершилась с ошибкой (код выхода: $EXIT_CODE)${NC}"
    exit 1
fi

echo -e "${GREEN}✅ Программа успешно завершена! Результаты выведены в консоль.${NC}"
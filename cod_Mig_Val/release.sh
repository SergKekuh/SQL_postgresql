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

echo -e "${BLUE}🚀 release.sh — полная очистка и сборка проекта${NC}"

# --- ШАГ 1: Генерация версии ---
echo -e "${BLUE}🔄 Получаем версию из Git...${NC}"
./version/generate_version.sh

if [ $? -ne 0 ]; then
    echo -e "${RED}❌ Не удалось сгенерировать версию${NC}"
    exit 1
fi

APP_VERSION=$(cat version/version.hpp | grep APP_VERSION | awk '{print $3}' | tr -d '"')
echo -e "${GREEN}📦 Версия: ${APP_VERSION}${NC}"

# --- ШАГ 2: Очистка и сборка через CMake --
echo -e "${BLUE}🏗️ Собираем проект через CMake --fresh...${NC}"

rm -rf build && mkdir -p build
cd build || exit 1

show_progress_bar "cmake .." "CMake configure"
show_progress_bar "cmake --build . --clean-first" "CMake build"

cd ..

# --- ШАГ 3: Запуск тестов ---
echo -e "${BLUE}🧪 Запускаем тесты...${NC}"

cd build || exit 1
ctest --verbose > /dev/null 2>&1
TEST_EXIT_CODE=$?
cd ..

if [ $TEST_EXIT_CODE -ne 0 ]; then
    echo -e "${RED}❌ Тесты провалены!${NC}"
    exit 1
fi

echo -e "${YELLOW}✅ Тесты пройдены. Запускаем программу...${NC}"

# Запрашиваем ввод данных у пользователя
read -p $'\e[1;33m🟡 Введите год: \e[0m' YEAR
read -p $'\e[1;33m🟡 Пороговое значение продаж: \e[0m' SALES_THRESHOLD
read -p $'\e[1;33m🟡 Множитель (например, 1.0): \e[0m' MULTIPLIER

# Проверяем, что все аргументы введены
if [[ -z "$YEAR" || -z "$SALES_THRESHOLD" || -z "$MULTIPLIER" ]]; then
    echo -e "${RED}❌ Необходимо ввести все три параметра: год, порог продаж и множитель.${NC}"
    exit 1
fi

# Получаем имя файла main_версия
MAIN_EXE="main_$(cat version/version.hpp | grep APP_VERSION | awk '{print $3}' | tr -d '"')"

# Копируем main, если его нет в корне
if [ ! -f "./$MAIN_EXE" ]; then
    cp ./build/main ./$MAIN_EXE
fi

# Выполняем программу
echo -e "${BLUE}▶️ Выполняем: ./${MAIN_EXE} ${YEAR} ${SALES_THRESHOLD} ${MULTIPLIER}${NC}"
"./${MAIN_EXE}" "$YEAR" "$SALES_THRESHOLD" "$MULTIPLIER"

# Проверяем код завершения программы
EXIT_CODE=$?
if [ $EXIT_CODE -ne 0 ]; then
    echo -e "${RED}❌ Программа завершилась с ошибкой (код выхода: $EXIT_CODE)${NC}"
    exit 1
fi

echo -e "${GREEN}✅ Программа успешно завершена!${NC}"
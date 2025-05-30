#!/bin/bash

# ANSI цвета
NC='\033[0m'
BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'

# Функция прогресс-бара
progress_bar() {
    local msg="$1"
    echo -ne "${BLUE}$msg...   ${NC}"
    for ((i = 0; i < 20; i++)); do
        echo -ne "${GREEN}#${NC}"
        sleep 0.1
    done
    echo -e " ${YELLOW}[OK]${NC}"
}

# Переход в корень проекта
cd "$(dirname "$0")" || { echo -e "${RED}❌ Не удалось перейти в папку проекта${NC}"; exit 1; }

echo -e "${BLUE}🚀 release.sh — полный процесс сборки и запуска${NC}"

# --- ШАГ 1: Генерация версии ---
echo -e "${BLUE}🔄 Получаем версию из Git...${NC}"
./version/generate_version.sh
progress_bar "Генерируем version.hpp"

# --- ШАГ 2: Сборка проекта ---
echo -e "${BLUE}🏗️ Собираем проект через CMake --fresh${NC}"
rm -rf build && mkdir -p build
cd build || exit 1

cmake .. > /dev/null 2>&1
cmake --build . --clean-first > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo -e "${RED}❌ Ошибка сборки${NC}"
    cd ..
    exit 1
fi

cd ..
progress_bar "Сборка завершена"

# --- ШАГ 3: Запуск тестов ---
echo -e "${BLUE}🧪 Запускаем тесты...${NC}"

cd build || exit 1
ctest --verbose > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo -e "${RED}❌ Тесты провалены!${NC}"
    cd ..
    exit 1
fi

cd ..
progress_bar "Тесты пройдены"

# --- ШАГ 4: Запрос параметров и запуск программы ---
echo -e "${GREEN}🟢 Тесты пройдены. Запускаем программу...${NC}"

read -p $'\e[1;33m🟡 Введите год: \e[0m' YEAR
read -p $'\e[1;33m🟡 Пороговое значение продаж: \e[0m' SALES_THRESHOLD

# Выполняем main с версией в имени файла
MAIN_EXE="main_$(cat version/version.hpp | grep APP_VERSION | awk '{print $3}' | tr -d '"')"

echo -e "${BLUE}▶️ Выполняем: ./${MAIN_EXE} $YEAR $SALES_THRESHOLD${NC}"

# Проверяем, существует ли файл
if [ ! -f "./${MAIN_EXE}" ]; then
    echo -e "${RED}❌ Файл ./${MAIN_EXE} не найден!${NC}"
    exit 1
fi

# Запускаем с параметрами
"./${MAIN_EXE}" "$YEAR" "$SALES_THRESHOLD"
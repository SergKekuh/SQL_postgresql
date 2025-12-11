#!/bin/bash

# ANSI цвета
NC=$'\e[0m'
BLUE=$'\e[0;34m'
GREEN=$'\e[0;32m'
YELLOW=$'\e[1;33m'
RED=$'\e[0;31m'

# Файл для логов (будет перезаписываться при каждом запуске)
LOG_FILE="build_process.log"

# Функция show_progress_bar с обработкой ошибок
show_progress_bar() {
    local cmd="$1"
    local msg="$2"

    # Очищаем лог перед новой командой
    echo "" > "$LOG_FILE"

    echo -ne "${BLUE}${msg}...    ${NC}"
    
    # ЗАПУСК КОМАНДЫ: пишем вывод в файл, а не в /dev/null
    eval "$cmd" > "$LOG_FILE" 2>&1 &
    local pid=$!

    local progress=0
    local total=20

    # Пока процесс жив
    while kill -0 "$pid" 2>/dev/null; do
        sleep 0.1
        ((progress++))
        if [ $progress -gt $total ]; then
            progress=$total
        fi

        # Рисуем бар
        printf $'\r%s[%s%s] %s%3d%%%%%s' \
            "$BLUE" \
            "$(printf '#%.0s' $(seq 1 $progress))" \
            " " \
            "$YELLOW" \
            "$((progress * 5))" \
            "$NC"
    done

    # Ждем код возврата
    wait $pid
    local exit_code=$?
    
    # Если успех (код 0)
    if [ $exit_code -eq 0 ]; then
        printf $'\r%s[%s] %s%3d%%%%  %s%s\n' \
            "$BLUE" \
            "$(printf '#%.0s' $(seq 1 $total))" \
            "$NC" \
            "100" \
            "$GREEN[OK]$NC"
    else
        # ЕСЛИ ОШИБКА
        printf $'\r%s[%s] %s%3d%%%%  %s%s\n' \
            "$RED" \
            "$(printf '#%.0s' $(seq 1 $progress))" \
            "$NC" \
            "$((progress * 5))" \
            "$RED[FAIL]$NC"
            
        echo -e "${RED}❌ Ошибка при выполнении: $msg${NC}"
        echo -e "${YELLOW}--- Детали ошибки (из лога) ---${NC}"
        cat "$LOG_FILE" # Выводим содержимое лога на экран
        echo -e "${YELLOW}-------------------------------${NC}"
        return $exit_code
    fi
}

# Переход в корень проекта
cd "$(dirname "$0")" || { echo -e "${RED}❌ Не могу перейти в папку проекта${NC}"; exit 1; }

echo -e "${BLUE}🚀 release.sh — Сборка проекта (Finance Classifier)${NC}"

# --- ШАГ 1: Очистка и сборка через CMake ---
echo -e "${BLUE}🏗️ Конфигурация CMake...${NC}"

rm -rf build && mkdir -p build
cd build || exit 1

# Вызываем функцию (если вернет ошибку, скрипт остановится)
show_progress_bar "cmake .." "Configuring"
if [ $? -ne 0 ]; then exit 1; fi 

echo -e "${BLUE}🔨 Компиляция проекта...${NC}"
show_progress_bar "cmake --build ." "Building"
if [ $? -ne 0 ]; then exit 1; fi

cd ..

# --- ШАГ 2: Запуск исполняемого файла ---
echo -e "${BLUE}▶️ Запускаем исполняемый файл: ./build/finance_classifier${NC}"
echo "---------------------------------------------------"

./build/finance_classifier

# Проверяем код завершения программы
EXIT_CODE=$?
if [ $EXIT_CODE -ne 0 ]; then
    echo -e "${RED}❌ Программа завершилась с ошибкой (код выхода: $EXIT_CODE)${NC}"
    exit 1
fi

echo -e "${GREEN}✅ Программа успешно завершена!${NC}"
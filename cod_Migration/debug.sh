#!/bin/bash

# --- Получаем версию ---
APP_VERSION=$(cat version/version.hpp | grep APP_VERSION | awk '{print $3}' | tr -d '"')
export APP_VERSION

# --- Переход в корень проекта ---
cd ~/Documents/SQL_postgresql/cod_Migration || {
    echo -e "\e[1;31m❌ Не могу перейти в cod_Migration\e[0m"
    exit 1
}

# --- Пересобираем debug-версию ---
cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make clean && make
cd ..

# --- Запуск VSCode и начало отладки ---
echo -e "\e[1;32m✅ Запускаем VSCode с отладкой\e[0m"

# code --reuse-window .
#code --debugId "C++ Debug(Debug)"  # ← замени на ID твоей конфигурации (смотрится в launch.json)
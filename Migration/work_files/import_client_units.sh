#!/bin/bash

# Имя файла CSV
CSV_FILE="client_units.csv"

# Имя базы данных
DB_NAME="bd_migrations"

# Пользователь PostgreSQL
DB_USER="postgres"

echo "=== Импорт $CSV_FILE в таблицу client_units ==="

# Шаг 1: Очистка таблицы перед импортом (при необходимости)
echo "Очищаем таблицу client_units..."
psql -U "$DB_USER" -d "$DB_NAME" -c "TRUNCATE TABLE client_units CASCADE;"

# Шаг 2: Проверяем, существует ли файл
if [ ! -f "$CSV_FILE" ]; then
    echo "❌ Файл $CSV_FILE не найден!"
    exit 1
fi

# Шаг 3: Выбираем способ импорта

# 🔹 ВАРИАНТ 1: Если в CSV НЕТ unit_id (БД сама присвоит ID)
# echo "Загружаем данные без указания unit_id..."
# psql -U "$DB_USER" -d "$DB_NAME" -c "
# COPY client_units (okpo_code, okpo_s1c8, ipn, unit_name, full_unit_name, legal_entity_type, note)
# FROM '$(pwd)/$CSV_FILE'
# DELIMITER ';'
# CSV HEADER;
# "

# 🔹 ВАРИАНТ 2: Если в CSV ЕСТЬ unit_id (например, для восстановления связей)
echo "Загружаем данные с указанием unit_id..."
psql -U "$DB_USER" -d "$DB_NAME" -c "
COPY client_units (unit_id, okpo_code, okpo_s1c8, ipn, unit_name, full_unit_name, legal_entity_type, note)
FROM '$(pwd)/$CSV_FILE'
DELIMITER ';'
CSV HEADER;
"

# Проверка результата
if [ $? -eq 0 ]; then
    echo "✅ Данные успешно импортированы в таблицу 'client_units'."
else
    echo "❌ Ошибка при импорте данных."
fi

#!/bin/bash

# Название БД и пользователя
DB_NAME="bd_migrations"
DB_USER="postgres"

# Путь к CSV-файлу (измени на свой)
CSV_FILE="/home/serg/Documents/SQL_postgresql/Migraciya/client_activity.csv"

# Цвета для вывода
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Импорт client_activity.csv ===${NC}"

# Проверяем, существует ли файл
if [ ! -f "$CSV_FILE" ]; then
    echo -e "${RED}❌ Ошибка: Файл $CSV_FILE не найден.${NC}"
    exit 1
fi

# Очищаем таблицу (CASCADE нужен, если есть зависимости)
echo -e "${GREEN}Очищаем таблицу client_activity...${NC}"
psql -U "$DB_USER" -d "$DB_NAME" -c "TRUNCATE TABLE client_activity;"

# Запускаем импорт
echo -e "${GREEN}Загружаем данные из client_activity.csv...${NC}"
psql -U "$DB_USER" -d "$DB_NAME" -c "\copy client_activity(client_activity_id, unit_id, year_id, group_id, expense_invoices, sales, note) FROM '$CSV_FILE' DELIMITER ';' CSV HEADER;"

# Проверяем, что загрузилось
echo -e "${GREEN}Проверяем содержимое таблицы client_activity:${NC}"
psql -U "$DB_USER" -d "$DB_NAME" -c "SELECT * FROM client_activity LIMIT 5;"

echo -e "${GREEN}✅ Данные успешно импортированы в таблицу 'client_activity'.${NC}"

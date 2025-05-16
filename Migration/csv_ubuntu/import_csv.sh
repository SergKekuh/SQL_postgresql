#!/bin/bash

# Название базы данных
DB_NAME="bd_migrations"

# Пользователь PostgreSQL
DB_USER="postgres"

# Путь к CSV-файлам (меняй на свой, если нужно)
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Файлы
GROUPS_CSV="$DIR/groups.csv"
YEARS_CSV="$DIR/years.csv"
CLIENT_UNITS_CSV="$DIR/client_units.csv"
CLIENT_ACTIVITY_CSV="$DIR/client_activity.csv"

# ANSI цвета
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Начинаем импорт CSV в PostgreSQL ===${NC}"

# Проверка существования файлов
for file in "$GROUPS_CSV" "$YEARS_CSV" "$CLIENT_UNITS_CSV" "$CLIENT_ACTIVITY_CSV"; do
    if [ ! -f "$file" ]; then
        echo -e "${RED}Ошибка: Файл $file не найден.${NC}"
        exit 1
    fi
done

# 1. Очистка таблиц (если нужно)
echo -e "${GREEN}Очищаем таблицы перед импортом...${NC}"
psql -U "$DB_USER" -d "$DB_NAME" -c "TRUNCATE TABLE client_activity, groups, years, client_units CASCADE;"

# 2. Загрузка данных
echo -e "${GREEN}1. Загружаем группы...${NC}"
psql -U "$DB_USER" -d "$DB_NAME" -c "\copy groups(group_id, group_name, note) FROM '$GROUPS_CSV' DELIMITER ';' CSV HEADER;"

echo -e "${GREEN}2. Загружаем годы...${NC}"
psql -U "$DB_USER" -d "$DB_NAME" -c "\copy years(year_id, year, note) FROM '$YEARS_CSV' DELIMITER ';' CSV HEADER;"

echo -e "${GREEN}3. Загружаем клиентские единицы...${NC}"
psql -U "$DB_USER" -d "$DB_NAME" -c "\copy client_units(unit_id, okpo_code, okpo_s1c8, ipn, unit_name, full_unit_name, legal_entity_type, note) FROM '$CLIENT_UNITS_CSV' DELIMITER ';' CSV HEADER;"

echo -e "${GREEN}4. Загружаем активность клиентов...${NC}"
psql -U "$DB_USER" -d "$DB_NAME" -c "\copy client_activity(client_activity_id, unit_id, year_id, group_id, expense_invoices, sales, note) FROM '$CLIENT_ACTIVITY_CSV' DELIMITER ';' CSV HEADER;"

echo -e "${GREEN}✅ Все данные успешно импортированы в БД '$DB_NAME'.${NC}"

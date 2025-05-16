#!/bin/bash

DB_NAME="bd_migrations"
DB_USER="postgres"
YEAR_CSV="/home/serg/Documents/SQL_postgresql/Migraciya/years.csv"  # <-- замени на свой путь

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${GREEN}=== Импорт years.csv ===${NC}"

if [ ! -f "$YEAR_CSV" ]; then
    echo -e "${RED}Ошибка: Файл $YEAR_CSV не найден.${NC}"
    exit 1
fi

echo -e "${GREEN}Очищаем таблицу years...${NC}"
psql -U "$DB_USER" -d "$DB_NAME" -c "TRUNCATE TABLE years CASCADE;"

echo -e "${GREEN}Загружаем годы...${NC}"
psql -U "$DB_USER" -d "$DB_NAME" -c "\copy years(year_id, year, note) FROM '$YEAR_CSV' DELIMITER ';' CSV HEADER;"

echo -e "${GREEN}✅ Данные успешно импортированы в таблицу 'years'.${NC}"

echo -e "${GREEN}Проверяем содержимое:${NC}"
psql -U "$DB_USER" -d "$DB_NAME" -c "SELECT * FROM years;"

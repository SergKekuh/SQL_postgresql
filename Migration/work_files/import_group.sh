#!/bin/bash

DB_NAME="bd_migrations"
DB_USER="postgres"
GROUP_CSV="/home/serg/Documents/SQL_postgresql/Migraciya/groups.csv"  # <-- замени на свой путь

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${GREEN}=== Импорт groups.csv ===${NC}"

if [ ! -f "$GROUP_CSV" ]; then
    echo -e "${RED}Ошибка: Файл $GROUP_CSV не найден.${NC}"
    exit 1
fi

echo -e "${GREEN}Очищаем таблицу groups...${NC}"
psql -U "$DB_USER" -d "$DB_NAME" -c "TRUNCATE TABLE groups CASCADE;"

echo -e "${GREEN}Загружаем группы...${NC}"
psql -U "$DB_USER" -d "$DB_NAME" -c "\copy groups(group_id, group_name, note) FROM '$GROUP_CSV' DELIMITER ';' CSV HEADER;"

echo -e "${GREEN}✅ Данные успешно импортированы в таблицу 'groups'.${NC}"

echo -e "${GREEN}Проверяем содержимое:${NC}"
psql -U "$DB_USER" -d "$DB_NAME" -c "SELECT * FROM groups;"

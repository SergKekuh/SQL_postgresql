#!/bin/bash

# Название БД и пользователя
DB_NAME="bd_migrations"
DB_USER="postgres"

# Путь к CSV-файлу
CSV_FILE="/home/serg/Documents/SQL_postgresql/Migration/client_activity_1.csv"

# Цвета для красивого вывода
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Импорт client_activity_1.csv ===${NC}"

# Проверяем, существует ли файл
if [ ! -f "$CSV_FILE" ]; then
    echo -e "${RED}❌ Ошибка: Файл $CSV_FILE не найден.${NC}"
    exit 1
fi

# Создаем временную таблицу и загружаем данные
echo -e "${GREEN}Создаём временную таблицу и загружаем данные...${NC}"
psql -U "$DB_USER" -d "$DB_NAME" <<EOF
DROP TABLE IF EXISTS temp_client_activity;

-- Временная таблица СО столбцом client_activity_id
CREATE TEMP TABLE temp_client_activity (
    client_activity_id integer,
    unit_id integer NOT NULL,
    year_id integer NOT NULL,
    group_id integer NOT NULL,
    expense_invoices numeric(15, 2),
    sales numeric(15, 2),
    note text
);

-- Загружаем все данные из CSV
COPY temp_client_activity FROM '$CSV_FILE' DELIMITER ';' CSV HEADER;

-- Вставляем только те записи, которых ещё нет в таблице client_activity
INSERT INTO public.client_activity (client_activity_id, unit_id, year_id, group_id, expense_invoices, sales, note)
SELECT client_activity_id, unit_id, year_id, group_id, expense_invoices, sales, note
FROM temp_client_activity
ON CONFLICT (client_activity_id) DO NOTHING;
EOF

# Проверка выполнения
if [ $? -ne 0 ]; then
    echo -e "${RED}❌ Ошибка при загрузке или вставке данных.${NC}"
    exit 1
fi

# Проверяем, есть ли новые данные с year_id = 5
echo -e "${GREEN}🔍 Проверяем наличие данных за year_id = 5...${NC}"

# Выполняем SQL-запрос и сохраняем результат в переменную
DATA_EXISTS=$(psql -U "$DB_USER" -d "$DB_NAME" -t -A -c "SELECT EXISTS (SELECT 1 FROM public.client_activity WHERE year_id = 5 LIMIT 1);")

# Проверяем результат
if [[ "$DATA_EXISTS" == "t" ]]; then
    echo -e "${GREEN}✅ Есть данные за year_id = 5. Выводим 10 последних записей:${NC}"
    psql -U "$DB_USER" -d "$DB_NAME" -c "SELECT * FROM public.client_activity WHERE year_id = 5 ORDER BY client_activity_id DESC LIMIT 10;"
else
    echo -e "${RED}❌ Данные за year_id = 5 не найдены. Возможно, они не были добавлены.${NC}"
fi

echo -e "${GREEN}✅ Импорт завершён.${NC}"

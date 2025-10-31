#!/bin/bash

# Переменные
DB_NAME="bd_migrations"
USER="postgres"
CSV_FILE="/home/serg/Documents/SQL_postgresql/Migration/client_units_1.csv"
LOG_FILE="/var/lib/postgresql/import_log.txt"

# Установка пароля для PostgreSQL
export PGPASSWORD="root"

# Очистка предыдущего лога (если нужно)
> "$LOG_FILE" 2>/dev/null || touch "$LOG_FILE"

# Проверка наличия CSV-файла
if [ ! -f "$CSV_FILE" ]; then
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] Ошибка: Файл $CSV_FILE не найден." >> "$LOG_FILE"
    echo "❌ Ошибка: Файл $CSV_FILE не найден."
    exit 1
fi

# Временная таблица и импорт данных
echo "[$(date '+%Y-%m-%d %H:%M:%S')] Запуск импорта данных из $CSV_FILE в базу $DB_NAME..." >> "$LOG_FILE"
echo "📌 Подготовка временной таблицы и импорт..."

psql -U "$USER" -d "$DB_NAME" >> "$LOG_FILE" 2>&1 <<EOF
DROP TABLE IF EXISTS temp_client_units;

-- Создание временной таблицы
CREATE TEMP TABLE temp_client_units (
    unit_id integer NOT NULL,
    okpo_code character varying(12),
    okpo_s1c8 character varying(12),
    ipn character varying(12),
    unit_name character varying(255),
    full_unit_name text,
    legal_entity_type character varying(10),
    note text
);

-- Загрузка данных из CSV
COPY temp_client_units (unit_id, okpo_code, okpo_s1c8, ipn, unit_name, full_unit_name, legal_entity_type, note)
FROM '$CSV_FILE'
DELIMITER ';'
CSV HEADER;

-- Подсчёт количества записей во временной таблице
DO \$\$
DECLARE
    temp_count INTEGER;
BEGIN
    SELECT COUNT(*) INTO temp_count FROM temp_client_units;
    RAISE NOTICE 'Количество записей во временной таблице: %', temp_count;
END \$\$;

-- Подсчёт количества уже существующих записей
DO \$\$
DECLARE
    existing_count INTEGER;
BEGIN
    SELECT COUNT(*) INTO existing_count
    FROM temp_client_units t
    WHERE EXISTS (
        SELECT 1
        FROM public.client_units cu
        WHERE cu.unit_id = t.unit_id
    );
    RAISE NOTICE 'Количество уже существующих записей: %', existing_count;
END \$\$;

-- Добавление новых записей
INSERT INTO public.client_units (unit_id, okpo_code, okpo_s1c8, ipn, unit_name, full_unit_name, legal_entity_type, note)
SELECT t.unit_id, t.okpo_code, t.okpo_s1c8, t.ipn, t.unit_name, t.full_unit_name, t.legal_entity_type, t.note
FROM temp_client_units t
ON CONFLICT (unit_id) DO NOTHING;

-- Подсчёт количества добавленных записей
DO \$\$
DECLARE
    inserted_count INTEGER;
BEGIN
    GET DIAGNOSTICS inserted_count = ROW_COUNT;
    RAISE NOTICE 'Количество добавленных записей: %', inserted_count;
END \$\$;
EOF

# Проверка статуса выполнения
if [ $? -eq 0 ]; then
    echo "✅ Данные успешно импортированы или уже существуют."
else
    echo "❌ Ошибка при импорте данных."
fi

# Вывод последних строк лога
echo ""
echo "📌 Последние 10 строк лога:"
tail -n 10 "$LOG_FILE"

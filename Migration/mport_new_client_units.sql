-- Начало транзакции
BEGIN;

-- 1. Создаем временную таблицу для загрузки данных из CSV
CREATE TEMP TABLE temp_client_units (
    unit_id integer NOT NULL,
    okpo_code character varying(12),
    okpo_s1c8 character varying(12),
    ipn character varying(12),
    unit_name character varying(255),
    full_unit_name text,
    legal_entity_type character varying(10),
    note text
) ON COMMIT DROP;

-- 2. Загружаем данные из CSV во временную таблицу
-- Используем разделитель ';' и указываем путь к вашему файлу
COPY temp_client_units (unit_id, okpo_code, okpo_s1c8, ipn, unit_name, full_unit_name, legal_entity_type, note)
FROM '/home/serg/Documents/SQL_postgresql/Migration/client_units_1.csv'
DELIMITER ';'
CSV HEADER;

-- 3. Вставляем только те записи, которых ещё нет в таблице client_units
INSERT INTO public.client_units (unit_id, okpo_code, okpo_s1c8, ipn, unit_name, full_unit_name, legal_entity_type, note)
SELECT t.unit_id, t.okpo_code, t.okpo_s1c8, t.ipn, t.unit_name, t.full_unit_name, t.legal_entity_type, t.note
FROM temp_client_units t
WHERE NOT EXISTS (
    SELECT 1 FROM public.client_units c
    WHERE c.unit_id = t.unit_id
);

-- Зафиксировать изменения
COMMIT;

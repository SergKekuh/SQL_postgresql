#!/bin/bash

# === Настройки ===
CSV_DIR="/home/serg/Documents/SQL_postgresql/Cost_accounting/document"
DB_NAME="finance_db"

# === Функция проверки формата даты в CSV с разделителем ; ===
validate_date_format() {
    local file="$1"
    # Проверяем, что все строки (кроме заголовка) содержат дату в формате DD.MM.YYYY в первом столбце
    # Используем sed, чтобы пропустить первую строку (заголовок)
    # Используем awk с -F';' чтобы разделитель был ;
    if sed '1d' "$file" | awk -F';' '{print $1}' | grep -vE '^[0-9]{2}\.[0-9]{2}\.[0-9]{4}$' | grep -q .; then
        echo "❌ Ошибка: найдены строки с неверным форматом даты в файле $file (ожидается DD.MM.YYYY)"
        return 1
    else
        echo "✅ Формат даты корректен в файле $file"
        return 0
    fi
}

# === Функция для загрузки CSV с проверкой даты и разделителем ; ===
load_csv() {
    local file="$1"
    local table="$2"
    if [ -f "$file" ]; then
        echo "Проверяю формат дат в $file..."
        if ! validate_date_format "$file"; then
            echo "⚠️ Пропускаю загрузку $file из-за ошибок формата даты."
            return 1
        fi

        echo "Загружаю $file в таблицу $table..."
        # Устанавливаем datestyle в сеансе перед COPY
        sudo -u postgres psql -d "$DB_NAME" -c "
SET datestyle = 'German, DMY';
COPY $table (operation_date, purpose, amount)
FROM '$file'
WITH (FORMAT csv, HEADER true, DELIMITER ';');
"
        if [ $? -eq 0 ]; then
            echo "✅ Успешно загружено в $table"
        else
            echo "❌ Ошибка при загрузке в $table"
            return 1
        fi
    else
        echo "⚠️ Файл $file не найден, пропускаю."
        return 1
    fi
}

# === Загрузка CSV-файлов ===
echo "Начинаем загрузку CSV-файлов..."

load_csv "$CSV_DIR/utsk_p.csv" "payments_privat_utsk"
#load_csv "$CSV_DIR/utsk_a.csv" "payments_aval_utsk"
#load_csv "$CSV_DIR/forpost_p.csv" "payments_privat_forpost"
# load_csv "$CSV_DIR/forpost_a.csv" "payments_aval_forpost"  # раскомментируйте, если нужно

echo "Загрузка завершена."

#!/bin/bash

# Путь к папке с CSV-файлами
CSV_DIR="/home/serg/Documents/SQL_postgresql/Banks"

# Файл для логов
LOG_FILE="/home/serg/Documents/SQL_postgresql/Banks/load_csv.log"

# Пароль пользователя postgres (если используется)
export PGPASSWORD="root"

# Очистка лог-файла
> "$LOG_FILE"

echo "Начинаем загрузку файлов..." | tee -a "$LOG_FILE"

# Проходим по всем CSV-файлам в папке
for file in "$CSV_DIR"/*.csv; do
    echo "Загрузка файла: $file" | tee -a "$LOG_FILE"

    # Удаление времени из даты (например, 01.04.2025 12:30 → 01.04.2025)
    sed -i 's/\([0-9]\{2\}\.[0-9]\{2\}\.[0-9]\{4\}\) [0-9]\{2\}:[0-9]\{2\}/\1/g' "$file"

    # Подключаемся к БД и выполняем SET + COPY в одной сессии
   {
	    echo "SET datestyle = 'ISO, DMY';"
	    echo "\\copy operations (
		edrpou, mfo, account, currency, operation_date, operation_code,
		bank_mfo, bank_name, correspondent_account, correspondent_edrpou,
		correspondent, document, document_date, debit, credit, payment_purpose, hryvnia_coverage
	    )
	    FROM '$file'
	    WITH (FORMAT csv, HEADER true, DELIMITER ';');"
    } | psql -h localhost -d csv_database -U postgres >> "$LOG_FILE" 2>&1

    if [ $? -eq 0 ]; then
        echo "Файл $file успешно загружен." | tee -a "$LOG_FILE"
    else
        echo "❌ Ошибка при загрузке файла $file." | tee -a "$LOG_FILE"
    fi
done

echo "✅ Все файлы обработаны!" | tee -a "$LOG_FILE"
echo "Логи сохранены в: $LOG_FILE"

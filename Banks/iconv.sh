#!/bin/bash

# Переходим в папку с файлами (если нужно)
cd /home/serg/Documents/SQL_postgresql/Banks

# Обрабатываем все CSV файлы в папке
for file in *.csv; do
    # Конвертируем каждый файл
    iconv -f WINDOWS-1251 -t UTF-8 "$file" -o "${file%.csv}_utf8.csv"
    echo "Файл $file обработан."
done

echo "Все файлы обработаны!"

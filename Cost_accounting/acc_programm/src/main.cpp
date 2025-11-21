#include "../include/CategoryMatcher.h"
#include "../include/Database.h"
#include <iostream>
#include <vector>
#include <string>

// Подключаем libxl C++ API
#include <libxl.h>

int main() {
    // Подключение к БД через класс Database
    Database db("localhost", "finance_db", "postgres", "root");
    CategoryMatcher matcher("categories.csv"); // ✅ Новый класс

    std::vector<std::string> tables = {
        "payments_privat_utsk",
        "payments_aval_utsk",
        "payments_privat_forpost",
        "payments_aval_forpost"
    };

    // Создаём книгу Excel
    libxl::Book* book = xlCreateBook(); // ✅ C++ API
    if (!book) {
        std::cerr << "Ошибка: не удалось создать XLSX-файл (xlCreateBook вернул nullptr)." << std::endl;
        return 1;
    }

    // Собираем нераспознанные платежи
    std::vector<PaymentRecord> unclassified_records;

    for (const auto& table : tables) {
        // Загружаем данные из БД через класс Database
        auto records = db.loadPaymentsFromDB(table);

        if (records.empty()) {
            std::cout << "Предупреждение: не загружено ни одной записи из " << table << ". Пропуск." << std::endl;
            continue;
        }

        // Создаём лист с названием таблицы
        libxl::Sheet* sheet = book->addSheet(table.c_str()); // ✅ C++ API
        if (!sheet) {
            std::cerr << "Ошибка: не удалось создать лист для " << table << std::endl;
            continue;
        }

        // Заголовки
        sheet->writeStr(0, 0, "operation_date"); // ✅ C++ API
        sheet->writeStr(0, 1, "purpose");         // ✅ C++ API
        sheet->writeStr(0, 2, "amount");         // ✅ C++ API
        sheet->writeStr(0, 3, "category_id");    // ✅ C++ API

        // Записываем данные
        for (size_t i = 0; i < records.size(); ++i) {
            auto& record = records[i];
            int categoryId = matcher.match(record.purpose);

            if (categoryId == 0) {
                std::cerr << "Не распознано: " << record.purpose << std::endl;
                unclassified_records.push_back(record); // добавляем в список нераспознанных
            } else {
                std::cout << "[ID:" << categoryId << "] " << record.purpose << " | Сумма: " << record.amount << std::endl;
            }

            // Всё равно записываем в XLSX, даже если category_id == 0
            sheet->writeStr(i + 1, 0, record.operation_date.c_str()); // ✅ C++ API
            sheet->writeStr(i + 1, 1, record.purpose.c_str());         // ✅ C++ API
            sheet->writeNum(i + 1, 2, record.amount);                 // ✅ C++ API
            sheet->writeNum(i + 1, 3, categoryId);                    // ✅ C++ API
        }

        std::cout << "Обработано " << records.size() << " записей из " << table << std::endl;
    }

    // Создаём лист с нераспознанными платежами
    if (!unclassified_records.empty()) {
        libxl::Sheet* unclassified_sheet = book->addSheet("Unclassified"); // ✅ C++ API
        if (unclassified_sheet) {
            // Заголовки
            unclassified_sheet->writeStr(0, 0, "operation_date"); // ✅ C++ API
            unclassified_sheet->writeStr(0, 1, "purpose");         // ✅ C++ API
            unclassified_sheet->writeStr(0, 2, "amount");         // ✅ C++ API

            // Записываем данные
            for (size_t i = 0; i < unclassified_records.size(); ++i) {
                const auto& record = unclassified_records[i];
                unclassified_sheet->writeStr(i + 1, 0, record.operation_date.c_str()); // ✅ C++ API
                unclassified_sheet->writeStr(i + 1, 1, record.purpose.c_str());         // ✅ C++ API
                unclassified_sheet->writeNum(i + 1, 2, record.amount);                 // ✅ C++ API
            }

            // Итоговая сумма
            double total = 0.0;
            for (const auto& record : unclassified_records) {
                total += record.amount;
            }
            size_t last_row = unclassified_records.size() + 2;
            unclassified_sheet->writeStr(last_row, 0, "Итого по нераспознанным:");
            unclassified_sheet->writeNum(last_row, 2, total);
        }
    }

    // Сохраняем XLSX-файл
    if (book->save("classified_payments_new_matcher.xlsx")) { // ✅ C++ API
        std::cout << "Результаты сохранены в classified_payments_new_matcher.xlsx" << std::endl;
    } else {
        std::cerr << "Ошибка: не удалось сохранить XLSX-файл." << std::endl;
    }

    // Удаляем книгу
    delete book; // ✅ C++ API

    return 0;
}
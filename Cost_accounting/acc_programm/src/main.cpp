#include "../include/PaymentClassifier.h"
#include "../include/Database.h"
#include <iostream>
#include <vector>
#include <string>

// Подключаем libxl
extern "C" {
    #include <libxl.h>
}

int main() {
    // Подключение к БД через класс Database
    Database db("localhost", "finance_db", "your_user", "your_password");
    PaymentClassifier classifier("categories.csv");

    std::vector<std::string> tables = {
        "payments_privat_utsk",
        "payments_aval_utsk",
        "payments_privat_forpost",
        "payments_aval_forpost"
    };

    // Создаём книгу Excel
    BookHandle book = xlCreateBook();
    if (!book) {
        std::cerr << "Ошибка: не удалось создать XLSX-файл." << std::endl;
        return 1;
    }

    for (const auto& table : tables) {
        // Загружаем данные из БД через класс Database
        auto records = db.loadPaymentsFromDB(table);

        // Создаём лист с названием таблицы
        SheetHandle sheet = xlBookAddSheet(book, table.c_str(), nullptr);
        if (!sheet) {
            std::cerr << "Ошибка: не удалось создать лист для " << table << std::endl;
            continue;
        }

        // Заголовки
        xlSheetWriteStr(sheet, 0, 0, "operation_date", nullptr);
        xlSheetWriteStr(sheet, 0, 1, "purpose", nullptr);
        xlSheetWriteStr(sheet, 0, 2, "amount", nullptr);
        xlSheetWriteStr(sheet, 0, 3, "category_id", nullptr);

        // Записываем данные
        for (size_t i = 0; i < records.size(); ++i) {
            auto& record = records[i];
            record.category_id = classifier.classify(record.purpose);

            if (record.category_id == 0) {
                std::cerr << "Не распознано: " << record.purpose << std::endl;
            }

            xlSheetWriteStr(sheet, i + 1, 0, record.operation_date.c_str(), nullptr);
            xlSheetWriteStr(sheet, i + 1, 1, record.purpose.c_str(), nullptr);
            xlSheetWriteNum(sheet, i + 1, 2, record.amount, nullptr);
            xlSheetWriteNum(sheet, i + 1, 3, record.category_id, nullptr);
        }

        std::cout << "Обработано " << records.size() << " записей из " << table << std::endl;
    }

    // Сохраняем XLSX-файл
    if (xlBookSave(book, "classified_payments_from_db.xlsx")) {
        std::cout << "Результаты сохранены в classified_payments_from_db.xlsx" << std::endl;
    } else {
        std::cerr << "Ошибка: не удалось сохранить XLSX-файл." << std::endl;
    }

    // Освобождаем ресурсы
    xlBookRelease(book);

    return 0;
}
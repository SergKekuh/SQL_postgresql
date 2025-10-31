#include "../include/PaymentClassifier.h"
#include "../include/Database.h"
#include <iostream>
#include <vector>

int main() {
    // Настройки БД
    Database db("localhost", "finance_db", "your_user", "your_password");
    PaymentClassifier classifier;

    // Список таблиц для обработки
    std::vector<std::string> tables = {
        "payments_privat_utsk",
        "payments_aval_utsk",
        "payments_privat_forpost",
        "payments_aval_forpost"
    };

    // Обработка каждой таблицы
    for (const auto& table : tables) {
        std::string csvPath = table + ".csv";
        auto records = db.loadPaymentsFromCSV(csvPath);

        for (auto& record : records) {
            record.category_id = classifier.classify(record.purpose);
            db.insertPayment(table, record);
        }
        std::cout << "Обработано " << records.size() << " записей в таблице " << table << std::endl;
    }

    return 0;
}
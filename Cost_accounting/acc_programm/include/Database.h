#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>

struct PaymentRecord {
    std::string operation_date;
    std::string purpose;
    double amount;
    int category_id = 0; // будет заполнено после классификации
};

class Database {
private:
    std::string conninfo_;

public:
    Database(const std::string& host, const std::string& dbname,
             const std::string& user, const std::string& password);
    ~Database();

    // Удалим старый метод
    // std::vector<PaymentRecord> loadPaymentsFromCSV(const std::string& csvPath);

    // Добавим новый метод
    std::vector<PaymentRecord> loadPaymentsFromDB(const std::string& table);
};

#endif // DATABASE_H
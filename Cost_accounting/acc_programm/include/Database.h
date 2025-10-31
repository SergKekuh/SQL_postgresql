#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>

struct PaymentRecord {
    std::string operation_date;
    std::string purpose;
    double amount;
    int category_id;
};

class Database {
private:
    std::string conninfo_;

public:
    Database(const std::string& host, const std::string& dbname,
             const std::string& user, const std::string& password);
    ~Database();

    void insertPayment(const std::string& table, const PaymentRecord& record);
    std::vector<PaymentRecord> loadPaymentsFromCSV(const std::string& csvPath);
};

#endif // DATABASE_H
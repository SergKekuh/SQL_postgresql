#include "../include/Database.h"
#include <libpq-fe.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

Database::Database(const std::string& host, const std::string& dbname,
                   const std::string& user, const std::string& password) {
    conninfo_ = "host=" + host + " dbname=" + dbname +
                " user=" + user + " password=" + password;
}

Database::~Database() {}

void Database::insertPayment(const std::string& table, const PaymentRecord& record) {
    PGconn* conn = PQconnectdb(conninfo_.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Ошибка подключения к БД: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return;
    }

    std::ostringstream query;
    query << "INSERT INTO " << table
          << " (operation_date, purpose, amount, category_id) VALUES ("
          << "'" << record.operation_date << "', "
          << "'" << record.purpose << "', "
          << record.amount << ", "
          << record.category_id << ");";

    PGresult* res = PQexec(conn, query.str().c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Ошибка вставки: " << PQerrorMessage(conn) << std::endl;
    }
    PQclear(res);
    PQfinish(conn);
}

std::vector<PaymentRecord> Database::loadPaymentsFromCSV(const std::string& csvPath) {
    std::vector<PaymentRecord> records;
    std::ifstream file(csvPath);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть CSV: " << csvPath << std::endl;
        return records;
    }

    std::string line;
    std::getline(file, line); // пропускаем заголовок

    while (std::getline(file, line)) {
        std::replace(line.begin(), line.end(), ',', '.'); // замена запятых на точки в сумме
        std::replace(line.begin(), line.end(), ';', '\t'); // замена ; на таб для парсинга

        std::istringstream ss(line);
        std::string date, purpose, amount_str;
        if (std::getline(ss, date, '\t') &&
            std::getline(ss, purpose, '\t') &&
            std::getline(ss, amount_str, '\t')) {

            // Убираем пробелы и заменяем запятые в сумме
            amount_str.erase(std::remove(amount_str.begin(), amount_str.end(), ' '), amount_str.end());
            std::replace(amount_str.begin(), amount_str.end(), ',', '.');

            double amount = std::stod(amount_str);

            records.push_back({date, purpose, amount, 0}); // category_id пока 0
        }
    }
    file.close();
    return records;
}
#include "../include/Database.h"
#include <libpq-fe.h>
#include <iostream>

Database::Database(const std::string& host, const std::string& dbname,
                   const std::string& user, const std::string& password) {
    conninfo_ = "host=" + host + " dbname=" + dbname +
                " user=" + user + " password=" + password;
}

Database::~Database() {}

std::vector<PaymentRecord> Database::loadPaymentsFromDB(const std::string& table) {
    std::vector<PaymentRecord> records;

    PGconn* conn = PQconnectdb(conninfo_.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Ошибка подключения к БД: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return records;
    }

    std::string query = "SELECT operation_date, purpose, amount FROM " + table + ";";
    PGresult* res = PQexec(conn, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка выполнения запроса к таблице " << table << ": " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        PQfinish(conn);
        return records;
    }

    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        PaymentRecord rec;
        rec.operation_date = PQgetvalue(res, i, 0) ? PQgetvalue(res, i, 0) : "";
        rec.purpose = PQgetvalue(res, i, 1) ? PQgetvalue(res, i, 1) : "";
        rec.amount = std::stod(PQgetvalue(res, i, 2) ? PQgetvalue(res, i, 2) : "0");

        records.push_back(rec);
    }

    PQclear(res);
    PQfinish(conn);
    return records;
}
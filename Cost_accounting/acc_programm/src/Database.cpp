#include "../include/Database.h"
#include <libpq-fe.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <memory>

// RAII deleter для PGconn
struct PgConnDeleter {
    void operator()(PGconn* p) const { PQfinish(p); }
};
using ConnPtr = std::unique_ptr<PGconn, PgConnDeleter>;

// RAII deleter для PGresult
struct PgResultDeleter {
    void operator()(PGresult* p) const { PQclear(p); }
};
using ResultPtr = std::unique_ptr<PGresult, PgResultDeleter>;

Database::Database(const std::string& host, const std::string& dbname,
                   const std::string& user, const std::string& password) {
    conninfo_ = "host=" + host + " dbname=" + dbname +
                " user=" + user + " password=" + password;
}

Database::~Database() {}

std::vector<PaymentRecord> Database::loadPaymentsFromDB(const std::string& table) {
    std::vector<PaymentRecord> records;

    static const std::vector<std::string> allowed_tables = {
        "payments_privat_utsk",
        "payments_aval_utsk",
        "payments_privat_forpost",
        "payments_aval_forpost"
    };

    if (std::find(allowed_tables.begin(), allowed_tables.end(), table) == allowed_tables.end()) {
        std::cerr << "Предупреждение: недопустимое имя таблицы: " << table << std::endl;
        return records;
    }

    ConnPtr conn(PQconnectdb(conninfo_.c_str()));
    if (!conn || PQstatus(conn.get()) != CONNECTION_OK) {
        std::cerr << "Ошибка подключения к БД: " << (conn ? PQerrorMessage(conn.get()) : "no conn") << std::endl;
        return records;
    }

    std::string query = "SELECT operation_date, purpose, amount FROM " + table + ";";
    ResultPtr res(PQexec(conn.get(), query.c_str()));
    if (!res || PQresultStatus(res.get()) != PGRES_TUPLES_OK) {
        std::cerr << "Ошибка выполнения запроса к таблице " << table << ": " << PQerrorMessage(conn.get()) << std::endl;
        return records;
    }

    int rows = PQntuples(res.get());
    for (int i = 0; i < rows; ++i) {
        PaymentRecord rec;
        const char* date_cstr = PQgetvalue(res.get(), i, 0);
        const char* purpose_cstr = PQgetvalue(res.get(), i, 1);
        const char* amount_cstr = PQgetvalue(res.get(), i, 2);

        rec.operation_date = date_cstr ? date_cstr : "";
        rec.purpose = purpose_cstr ? purpose_cstr : "";

        if (amount_cstr && *amount_cstr) {
            try {
                rec.amount = std::stod(amount_cstr);
            } catch (const std::exception&) {
                std::cerr << "Ошибка парсинга amount в таблице " << table << " строка " << i << ": '" << amount_cstr << "'" << std::endl;
                rec.amount = 0.0;
            }
        } else {
            rec.amount = 0.0;
        }

        records.push_back(std::move(rec));
    }

    return records;
}
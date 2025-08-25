#include "year.h"
#include <libpq-fe.h>
#include <iostream>

Year::Year(int id, int yearValue, const std::string& noteStr)
    : year_id(id), year(yearValue), note(noteStr) {}

void Year::createTable(Database& db) {
    PGconn* conn = db.getConnection();
    const char* query =
        "CREATE TABLE IF NOT EXISTS years ("
        "year_id SERIAL PRIMARY KEY,"
        "year INTEGER NOT NULL UNIQUE,"
        "note TEXT"
        ");";

    PGresult* res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Error creating table 'years': " << PQresultErrorMessage(res) << std::endl;
    } else {
        std::cout << "Table 'years' created or already exists." << std::endl;
    }
    PQclear(res);
}

void Year::saveToDB(Database& db) const {
    PGconn* conn = db.getConnection();
    std::string query = "INSERT INTO years (year, note) VALUES (" +
                        std::to_string(year) + ", '" + note + "')";

    PGresult* res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Insert error into 'years': " << PQresultErrorMessage(res) << std::endl;
    }
    PQclear(res);
}
#include "ClientActivity.h"
#include <libpq-fe.h>
#include <iostream>

ClientActivity::ClientActivity(int id, int unitId, int yearId, int groupId,
                               double expense, double salesVal, const std::string& noteStr)
    : client_activity_id(id), unit_id(unitId), year_id(yearId),
      group_id(groupId), expense_invoices(expense), sales(salesVal), note(noteStr) {}

void ClientActivity::createTable(Database& db) {
    PGconn* conn = db.getConnection();
    const char* query =
        "CREATE TABLE IF NOT EXISTS client_activity ("
        "client_activity_id SERIAL PRIMARY KEY,"
        "unit_id INTEGER NOT NULL REFERENCES client_units(unit_id),"
        "year_id INTEGER NOT NULL REFERENCES years(year_id),"
        "group_id INTEGER NOT NULL REFERENCES groups(group_id),"
        "expense_invoices DECIMAL(15, 2),"
        "sales DECIMAL(15, 2),"
        "note TEXT,"
        "UNIQUE (unit_id, year_id)"
        ");";

    PGresult* res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Error creating table 'client_activity': " << PQresultErrorMessage(res) << std::endl;
    } else {
        std::cout << "Table 'client_activity' created or already exists." << std::endl;
    }
    PQclear(res);
}

void ClientActivity::saveToDB(Database& db) const {
    PGconn* conn = db.getConnection();
    std::string query = "INSERT INTO client_activity (unit_id, year_id, group_id, "
                        "expense_invoices, sales, note) VALUES ("+
                        std::to_string(unit_id) + ", " +
                        std::to_string(year_id) + ", " +
                        std::to_string(group_id) + ", " +
                        std::to_string(expense_invoices) + ", " +
                        std::to_string(sales) + ", '" +
                        note + "')";

    PGresult* res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Insert error into 'client_activity': " << PQresultErrorMessage(res) << std::endl;
    }
    PQclear(res);
}
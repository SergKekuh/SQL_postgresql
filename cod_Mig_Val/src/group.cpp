#include "group.h"
#include <libpq-fe.h>
#include <iostream>

Group::Group(int id, const std::string& groupName, const std::string& noteStr)
    : group_id(id), group_name(groupName), note(noteStr) {}

void Group::createTable(Database& db) {
    PGconn* conn = db.getConnection();
    const char* query =
        "CREATE TABLE IF NOT EXISTS groups ("
        "group_id SERIAL PRIMARY KEY,"
        "group_name VARCHAR(50) NOT NULL UNIQUE,"
        "note TEXT"
        ");";

    PGresult* res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Error creating table 'groups': " << PQresultErrorMessage(res) << std::endl;
    } else {
        std::cout << "Table 'groups' created or already exists." << std::endl;
    }
    PQclear(res);
}

void Group::saveToDB(Database& db) const {
    PGconn* conn = db.getConnection();
    std::string query = "INSERT INTO groups (group_name, note) VALUES ('" +
                        group_name + "', '" + note + "')";

    PGresult* res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Insert error into 'groups': " << PQresultErrorMessage(res) << std::endl;
    }
    PQclear(res);
}
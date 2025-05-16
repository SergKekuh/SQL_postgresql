#include "ClientUnit.h"
#include <libpq-fe.h>
#include <iostream>

ClientUnit::ClientUnit(int id, const std::string& okpo, const std::string& s1c8,
                       const std::string& ipn, const std::string& name,
                       const std::string& fullName, const std::string& legalType,
                       const std::string& noteStr)
    : unit_id(id), okpo_code(okpo), okpo_s1c8(s1c8), ipn(ipn),
      unit_name(name), full_unit_name(fullName),
      legal_entity_type(legalType), note(noteStr) {}

void ClientUnit::createTable(Database& db) {
    PGconn* conn = db.getConnection();
    const char* query =
        "CREATE TABLE IF NOT EXISTS client_units ("
        "unit_id SERIAL PRIMARY KEY,"
        "okpo_code VARCHAR(10) NOT NULL UNIQUE,"
        "okpo_s1c8 VARCHAR(10),"
        "ipn VARCHAR(10),"
        "unit_name VARCHAR(255),"
        "full_unit_name VARCHAR(255),"
        "legal_entity_type VARCHAR(10),"
        "note TEXT,"
        "CONSTRAINT valid_legal_entity CHECK (legal_entity_type IN ('Юр.', 'Фіз.'))"
        ");";

    PGresult* res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Error creating table: " << PQresultErrorMessage(res) << std::endl;
    } else {
        std::cout << "Table 'client_units' created or already exists." << std::endl;
    }
    PQclear(res);
}

void ClientUnit::saveToDB(Database& db) const {
    PGconn* conn = db.getConnection();
    std::string query = "INSERT INTO client_units (okpo_code, okpo_s1c8, ipn, "
                        "unit_name, full_unit_name, legal_entity_type, note) VALUES ("
                        "'" + okpo_code + "', "
                        "'" + okpo_s1c8 + "', "
                        "'" + ipn + "', "
                        "'" + unit_name + "', "
                        "'" + full_unit_name + "', "
                        "'" + legal_entity_type + "', "
                        "'" + note + "')"; // ⚠️ SQL Injection опасно! Позже заменим на prepared statements

    PGresult* res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Insert error: " << PQresultErrorMessage(res) << std::endl;
    }
    PQclear(res);
}
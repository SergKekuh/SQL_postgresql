#include "Database.h"
#include <iostream>

Database::Database(const std::string& dbname,
                   const std::string& user,
                   const std::string& password,
                   const std::string& host,
                   int port) {
    std::string connInfo = "host=" + host +
                           " port=" + std::to_string(port) +
                           " dbname=" + dbname +
                           " user=" + user +
                           " password=" + password;

    conn = PQconnectdb(connInfo.c_str());

    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn);
        PQfinish(conn);
        conn = nullptr;
    } else {
        std::cout << "Connected to database successfully!" << std::endl;
    }
}

Database::~Database() {
    if (conn) {
        PQfinish(conn);
    }
}

bool Database::isConnected() const {
    return conn && PQstatus(conn) == CONNECTION_OK;
}

PGconn* Database::getConnection() const {
    return conn;
}
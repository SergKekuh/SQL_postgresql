#ifndef DATABASE_H
#define DATABASE_H

#include <libpq-fe.h>
#include <string>

class Database {
private:
    PGconn* conn;

public:
    Database(const std::string& dbname,
             const std::string& user,
             const std::string& password,
             const std::string& host = "localhost",
             int port = 5432);

    ~Database();

    bool isConnected() const;
    PGconn* getConnection() const;
};

#endif // DATABASE_H
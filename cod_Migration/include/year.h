#ifndef YEAR_H
#define YEAR_H

#include <string>
#include "Database.h"

class Year {
private:
    int year_id;
    int year;
    std::string note;

public:
    // Конструктор
    Year(int id, int yearValue, const std::string& noteStr);

    // Сохранение в БД
    static void createTable(Database& db);
    void saveToDB(Database& db) const;
};

#endif // YEAR_H
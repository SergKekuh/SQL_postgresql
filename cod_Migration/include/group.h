#ifndef GROUP_H
#define GROUP_H

#include <string>
#include "Database.h"

class Group {
private:
    int group_id;
    std::string group_name;
    std::string note;

public:
    // Конструктор
    Group(int id, const std::string& groupName, const std::string& noteStr);

    // Сохранение в БД
    static void createTable(Database& db);
    void saveToDB(Database& db) const;
};

#endif // GROUP_H
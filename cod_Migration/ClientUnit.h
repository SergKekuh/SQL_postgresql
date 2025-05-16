#ifndef CLIENTUNIT_H
#define CLIENTUNIT_H

#include <string>
#include "Database.h"

class ClientUnit {
private:
    int unit_id;
    std::string okpo_code;
    std::string okpo_s1c8;
    std::string ipn;
    std::string unit_name;
    std::string full_unit_name;
    std::string legal_entity_type;
    std::string note;

public:
    // Конструктор
    ClientUnit(int id, const std::string& okpo, const std::string& s1c8,
               const std::string& ipn, const std::string& name,
               const std::string& fullName, const std::string& legalType,
               const std::string& noteStr);

    // Метод сохранения в БД
    static void createTable(Database& db);
    void saveToDB(Database& db) const;
};

#endif // CLIENTUNIT_H
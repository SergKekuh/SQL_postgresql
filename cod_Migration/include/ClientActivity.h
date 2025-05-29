#ifndef CLIENTACTIVITY_H
#define CLIENTACTIVITY_H

#include <string>
#include "Database.h"

class ClientActivity {
private:
    int client_activity_id;
    int unit_id;
    int year_id;
    int group_id;
    double expense_invoices;
    double sales;
    std::string note;

public:
    // Конструктор
    ClientActivity(int id, int unitId, int yearId, int groupId,
                   double expense, double salesVal, const std::string& noteStr);

    // Сохранение в БД
    static void createTable(Database& db);
    void saveToDB(Database& db) const;
};

#endif // CLIENTACTIVITY_H
#ifndef DATABASE_PROCEDURE_HANDLER_H
#define DATABASE_PROCEDURE_HANDLER_H

#include <string>
#include <vector>
#include "Database.h"

// Структура для хранения данных из временной таблицы
struct GroupReportRow {
    std::string group_name;
    double total_sale;
    int total_companies;
};

class DatabaseProcedureHandler {
public:
    // Конструктор принимает объект базы данных
    explicit DatabaseProcedureHandler(Database& db);

    // Метод для вызова хранимой процедуры generate_group_report_procedure
    bool executeGroupReportProcedure(int year, double multiplier);

    // Метод для получения данных из временной таблицы
    std::vector<GroupReportRow> fetchGroupReportData();

private:
    Database& db; // Ссылка на объект базы данных
};

#endif // DATABASE_PROCEDURE_HANDLER_H
#ifndef DATABASEPROCEDURES_H
#define DATABASEPROCEDURES_H

#include "Database.h"
#include "GroupReport.h" // Включаем структуру GroupReport
#include <string>
#include <vector>
#include "Logger.h"

class DatabaseProcedures {
public:
    explicit DatabaseProcedures(Database& db);

    /**
     * Вызывает хранимую процедуру generate_group_report_procedure
     * и возвращает результат из временной таблицы temp_group_report
     * @param year Год для отчета
     * @param multiplier Множитель для пороговых значений
     * @return Вектор с данными отчета
     */
    std::vector<GroupReport> callGroupReportProcedure(int year, double multiplier = 1.0);

    /**
     * Проверяет существование процедуры в базе данных
     * @return true если процедура существует
     */
    bool isGroupReportProcedureExists();

private:
    Database& db_;

    /**
     * Преобразует результат SQL-запроса в вектор GroupReport
     */
    std::vector<GroupReport> parseGroupReportResult(PGresult* result);
};

#endif // DATABASEPROCEDURES_H
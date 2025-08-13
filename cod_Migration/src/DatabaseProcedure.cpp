#include "DatabaseProcedure.h"
#include <libpq-fe.h>
#include <iostream>
#include <sstream>

DatabaseProcedures::DatabaseProcedures(Database& db) : db_(db) {}

std::vector<GroupReport> DatabaseProcedures::callGroupReportProcedure(int year, double multiplier) {
    std::vector<GroupReport> report;

    try {
        PGconn* conn = db_.getConnection();
        if (!conn || PQstatus(conn) != CONNECTION_OK) {
            throw std::runtime_error("Database connection is not initialized or failed.");
        }

        // 1. Вызываем хранимую процедуру
        std::ostringstream queryStream;
        queryStream << "CALL generate_group_report_procedure(" << year << ", " << multiplier << ")";
        std::string query = queryStream.str();

        PGresult* res = PQexec(conn, query.c_str());
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            std::string errorMessage = PQerrorMessage(conn);
            PQclear(res);
            throw std::runtime_error("Error executing procedure: " + errorMessage);
        }
        PQclear(res);

        // 2. Получаем результаты из временной таблицы
        const char* selectQuery = "SELECT group_name, total_sale, total_companies "
                                  "FROM temp_group_report "
                                  "ORDER BY CASE group_name "
                                  "   WHEN 'A1' THEN 1 "
                                  "   WHEN 'A2' THEN 2 "
                                  "   WHEN 'A3' THEN 3 "
                                  "   WHEN 'B1' THEN 4 "
                                  "   WHEN 'B2' THEN 5 "
                                  "   WHEN 'C1' THEN 6 "
                                  "   WHEN 'C2' THEN 7 "
                                  "   ELSE 8 END";

        res = PQexec(conn, selectQuery);
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            std::string errorMessage = PQerrorMessage(conn);
            PQclear(res);
            throw std::runtime_error("Error fetching data from temp_group_report: " + errorMessage);
        }

        // Преобразуем результат в вектор GroupReport
        report = parseGroupReportResult(res);
        PQclear(res);
    } catch (const std::exception& e) {
        throw std::runtime_error("Ошибка при выполнении generate_group_report_procedure: " + std::string(e.what()));
    }

    return report;
}

bool DatabaseProcedures::isGroupReportProcedureExists() {
    try {
        PGconn* conn = db_.getConnection();
        if (!conn || PQstatus(conn) != CONNECTION_OK) {
            throw std::runtime_error("Database connection is not initialized or failed.");
        }

        const char* query = "SELECT 1 FROM information_schema.routines "
                            "WHERE routine_name = 'generate_group_report_procedure' "
                            "AND routine_type = 'PROCEDURE'";

        PGresult* res = PQexec(conn, query);
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            std::string errorMessage = PQerrorMessage(conn);
            PQclear(res);
            throw std::runtime_error("Error checking procedure existence: " + errorMessage);
        }

        bool exists = PQntuples(res) > 0;
        PQclear(res);
        return exists;
    } catch (const std::exception& e) {
        throw std::runtime_error("Ошибка при проверке процедуры: " + std::string(e.what()));
    }
}

std::vector<GroupReport> DatabaseProcedures::parseGroupReportResult(PGresult* result) {
    std::vector<GroupReport> report;

    int rows = PQntuples(result);
    for (int i = 0; i < rows; ++i) {
        GroupReport item;
        item.group_name = PQgetvalue(result, i, 0); // group_name
        item.total_sale = std::stod(PQgetvalue(result, i, 1)); // total_sale
        item.total_companies = std::stoi(PQgetvalue(result, i, 2)); // total_companies
        report.push_back(item);
    }

    return report;
}
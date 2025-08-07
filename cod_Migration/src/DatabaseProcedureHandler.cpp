#include "DatabaseProcedureHandler.h"
#include "include/Logger.h"

DatabaseProcedureHandler::DatabaseProcedureHandler(Database& database)
    : db(database) {}

bool DatabaseProcedureHandler::executeGroupReportProcedure(int year, double multiplier) {
    Logger logger;

    try {
        // Вызов хранимой процедуры generate_group_report_procedure
        std::string query = "CALL generate_group_report_procedure($1, $2);";

        // Выполнение запроса с параметрами
        db.executeQuery(query, {std::to_string(year), std::to_string(multiplier)});
        logger.log(LOG("Хранимая процедура generate_group_report_procedure успешно выполнена"));
        return true;
    } catch (const std::exception& e) {
        logger.log(LOG("Ошибка при выполнении хранимой процедуры: " + std::string(e.what())));
        return false;
    }
}

std::vector<GroupReportRow> DatabaseProcedureHandler::fetchGroupReportData() {
    Logger logger;
    std::vector<GroupReportRow> result;

    try {
        // Запрос для получения данных из временной таблицы
        std::string query = "SELECT group_name, total_sale, total_companies FROM temp_group_report;";
        auto rows = db.executeQuery(query);

        for (const auto& row : rows) {
            GroupReportRow reportRow;
            reportRow.group_name = row["group_name"];
            reportRow.total_sale = std::stod(row["total_sale"]);
            reportRow.total_companies = std::stoi(row["total_companies"]);
            result.push_back(reportRow);
        }

        logger.log(LOG("Данные из временной таблицы успешно получены"));
    } catch (const std::exception& e) {
        logger.log(LOG("Ошибка при получении данных из временной таблицы: " + std::string(e.what())));
    }

    return result;
}
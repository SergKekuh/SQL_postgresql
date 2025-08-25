#include "include/Database.h"
#include "include/ClientUnit.h"
#include "include/StatisticsService.h"
#include "include/ExcelExporter.h"
#include "include/Logger.h"
#include "include/GroupReport.h"
#include "include/DatabaseProcedure.h"
#include "version/version.hpp"

#include <iostream>
#include <string>
#include <iomanip> // Для std::setw
#include <libxl.h>

int main(int argc, char* argv[]) {

   
    // Проверка на количество аргументов
    if (argc < 4) {
        std::cerr << "❗ Нужно передать: год, пороговое значение продаж и множитель\n";
        std::cerr << "Использование: " << argv[0] << " <год> <порог_продаж> <множитель>\n";
        return 1;
    }

    std::cout << "PostgreSQL Migration Tool " << APP_VERSION << std::endl;

    // Инициализируем логгер — создаёт папку logs/, если её нет
    Logger logger;
    logger.log(LOG("Запуск программы версии: " + std::string(APP_VERSION)));

    // Подключение к базе данных
    Database db("bd_migrations", "postgres", "root");

    if (!db.isConnected()) {
        const char* msg = "Failed to connect to the database.";
        std::cerr << msg << std::endl;
        logger.log(LOG(msg));
        return 1;
    }

    logger.log(LOG("Successfully connected to the database"));

    // Сервис статистики
    StatisticsService stats(db);
    DatabaseProcedures dbProcedures(db);

    // Проверяем существование процедуры
    if (!dbProcedures.isGroupReportProcedureExists()) {
        std::cerr << "❗ Процедура generate_group_report_procedure не найдена в БД" << std::endl;
        logger.log(LOG("Процедура generate_group_report_procedure не найдена"));
        return 1;
    }

    int year = 0;               // Год по умолчанию
    double sales_threshold = 0.0; // Порог продаж по умолчанию
    double multiplier = 1.0;    // Множитель по умолчанию

    // Парсим аргументы
    try {       
        year = std::stoi(argv[1]);
        sales_threshold = std::stod(argv[2]);
        multiplier = std::stod(argv[3]);
        
        logger.log(Logger::format("Fetching data for year: %d, sales threshold: %.2f, multiplier: %.2f",
                                  year, sales_threshold, multiplier));
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при парсинге аргументов: " << e.what() << std::endl;
        logger.log(LOG("Ошибка при парсинге аргументов командной строки"));
        return 1;
    }

    // Получаем данные из БД
    auto allStats = stats.getAllStatistics(year);
    auto belowStats = stats.getBelowStatistics(year, sales_threshold);
    auto higherStats = stats.getHigherStatistics(year, sales_threshold);
    auto groupReport = dbProcedures.callGroupReportProcedure(year, multiplier);

    // Выводим на консоль (для проверки)
    std::cout << "=== All statistics ===" << std::endl;
    logger.log(LOG("Printing statistics to console"));

    for (const auto& stat : allStats) {
        std::cout << std::setw(10) << stat.category << " | "
                  << std::setw(10) << stat.companies << " | "
                  << std::setw(10) << stat.total_invoices << " | "
                  << std::setw(10) << stat.total_sales << " | "
                  << std::setw(10) << stat.avg_sales << " | "
                  << std::setw(10) << stat.percent_share << "%" << std::endl;
    }

    std::cout << "=== Отчет по группам ===" << std::endl;
    for (const auto& item : groupReport) {
        std::cout << std::setw(10) << item.group_name << " | "
                  << std::setw(10) << item.total_sale << " | "
                  << std::setw(10) << item.total_companies << std::endl;
    }

    // Создаём папку reports/, если её нет
    if (!ExcelExporter::createReportsDirectoryIfNotExists("reports/")) {
        const char* msg = "Error creating reports directory!";
        std::cerr << msg << std::endl;
        logger.log(LOG(msg));
        return 1;
    }

    std::string templatePath = "template/template.xlsx";
    //std::string templatePathVal = "template/template_empty_val_1.xlsx";

    std::string filename = "reports/" + ExcelExporter::generateFilenameWithTimestamp("statistics_report", ".xlsx");
    //std::string filenameVal = "reports/" + ExcelExporter::generateFilenameWithTimestamp("group_report", ".xlsx");

    try {
        // --- ШАГ 1: Открываем шаблон ---
        libxl::Book* bookStatic = xlCreateBook();
       

        if (!bookStatic->load(templatePath.c_str())) {
            std::cerr << "Ошибка загрузки шаблона: " << templatePath << std::endl;            
            throw std::runtime_error("Не удалось открыть шаблон Excel Static");
        }

             
        

        // --- ШАГ 2: Заполняем данные в Excel (Static) ---
        ExcelExporter::writeYearToSheet(year);

        int col = 2;  // C
        for (size_t i = 0; i < belowStats.size(); ++i) {
            bool isLast = (i == belowStats.size() - 1);
            ExcelExporter::exportSingleStatToColumn(belowStats[i], col++, /*startRow=*/6, isLast);
        }

        col = 2;
        for (size_t i = 0; i < higherStats.size(); ++i) {
            bool isLast = (i == higherStats.size() - 1);
            ExcelExporter::exportSingleStatToColumn(higherStats[i], col++, /*startRow=*/12, isLast);
        }

        col = 2;
        for (size_t i = 0; i < allStats.size(); ++i) {
            bool isLast = (i == allStats.size() - 1);
            ExcelExporter::exportSingleStatToColumn(allStats[i], col++, /*startRow=*/18, isLast);
        }

        // Сохраняем книгу Static
        if (!bookStatic->save(filename.c_str())) {
            throw std::runtime_error("Ошибка при сохранении Excel-файла Static");
        }
        bookStatic->release();
        
        

        // Логирование успешного завершения
        std::string logMsg = "✅ Данные успешно экспортированы в: " + filename;
        //std::string logMsgVal = "✅ Данные успешно экспортированы в: " + filenameVal;
        std::cout << logMsg << "\n";
        //std::cout << logMsgVal << "\n";
        logger.log(LOG(logMsg));  
        //logger.log(LOG(logMsgVal));        
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Ошибка при работе с Excel: " << e.what() << std::endl;
        logger.log(LOG("Ошибка при работе с Excel: " + std::string(e.what())));
        return 1;
    }

    logger.log(LOG("Program finished successfully"));
    return 0;
}
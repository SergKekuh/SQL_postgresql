#include "Database.h"
#include "ClientUnit.h"
#include "StatisticsService.h"
#include "ExcelExporter.h"
#include "Logger.h"
#include "version/version.hpp"

#include <iostream>
#include <string>


int main(int argc, char* argv[]) {
    // Проверка на количество аргументов
    if (argc < 3) {
        std::cerr << "❗ Нужно передать: год и пороговое значение продаж\n";
        std::cerr << "Использование: " << argv[0] << " <год> <порог_продаж>\n";
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
   
    // Парсим аргументы
    int year = std::atoi(argv[1]);
    double sales_threshold = std::atof(argv[2]);

    logger.log(Logger::format("Fetching data for year: %d", year));

    // Получаем данные из БД
    auto allStats = stats.getAllStatistics(year);
    auto belowStats = stats.getBelowStatistics(year, sales_threshold);
    auto higherStats = stats.getHigherStatistics(year, sales_threshold);

    // Выводим на консоль (для проверки)
    std::cout << "=== All statistics ===" << std::endl;
    logger.log(LOG("Printing statistics to console"));

    for (const auto& stat : allStats) {
        std::cout << stat.category << " \t| "
                  << stat.companies << " \t| "
                  << stat.total_invoices << " \t| "
                  << stat.total_sales << " \t| "
                  << stat.avg_sales << " \t| "
                  << stat.percent_share << "%" << std::endl;
    }

    // Создаём папку reports/, если её нет
    if (!ExcelExporter::createReportsDirectoryIfNotExists("reports/")) {
        const char* msg = "Error creating reports directory!";
        std::cerr << msg << std::endl;
        logger.log(LOG(msg));
        return 1;
    }

    // Путь к шаблону
    std::string templatePath = "template/template_empty.xlsx";

    // Генерируем имя выходного файла
    std::string filename = "reports/" + ExcelExporter::generateFilenameWithTimestamp("statistics_report", ".xlsx");

    // Убедимся, что папка reports существует
    if (!ExcelExporter::createReportsDirectoryIfNotExists("reports/")) {
        logger.log(LOG("Ошибка создания папки reports/"));
        return 1;
    }

    // Экспортируем данные на один лист из шаблона

    bool success = true;

    success &= ExcelExporter::exportToSheetFromTemplate(templatePath, filename, belowStats, /*startRow=*/6, /*totalRow=*/12);
    success &= ExcelExporter::exportToSheetFromTemplate(templatePath, filename, higherStats, /*startRow=*/12, /*totalRow=*/18);
    success &= ExcelExporter::exportToSheetFromTemplate(templatePath, filename, allStats, /*startRow=*/18, /*totalRow=*/24);

    if (success) {
        std::string logMsg = "Данные успешно экспортированы в Excel: " + filename;
        std::cout << logMsg << "\n";
        logger.log(LOG(logMsg.c_str()));
        std::cout << "✅ Отчёт создан: " << filename << "\n";
    } else {
        logger.log(LOG("❌ Ошибка при экспорте в Excel"));
        std::cerr << "❌ Не удалось сохранить отчёт!\n";
        return 1;
    }

    logger.log(LOG("Program finished successfully"));
    return 0;
}
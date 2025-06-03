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

    std::string templatePath = "template/template_empty.xlsx";
    std::string filename = "reports/" + ExcelExporter::generateFilenameWithTimestamp("statistics_report", ".xlsx");

    if (!ExcelExporter::createReportsDirectoryIfNotExists("reports/")) {
        logger.log(LOG("Ошибка создания папки reports/"));
        return 1;
    }

    // --- ШАГ 1: Открываем шаблон ---
    if (!ExcelExporter::openTemplate(templatePath)) {
        logger.log(LOG("Не удалось открыть шаблон Excel"));
        return 1;
    }

    // // --- ШАГ 2: Записываем все три набора данных ---
    // ExcelExporter::exportToSheet(belowStats, /*startRow=*/6);
    // ExcelExporter::exportToSheet(higherStats, /*startRow=*/12);
    // ExcelExporter::exportToSheet(allStats, /*startRow=*/18);

    // Выводим год в Excel
    ExcelExporter::writeYearToSheet(year);

    // belowStats → начинаем с строки 6 (C7), колонка I (итог) в строке 12 (I13)
    int col = 2;  // C
    for (size_t i = 0; i < belowStats.size(); ++i) {
        bool isLast = (i == belowStats.size() - 1);
        ExcelExporter::exportSingleStatToColumn(belowStats[i], col++, /*startRow=*/6, isLast);
    }

    // higherStats → начинаем с строки 12 (C13)
    col = 2;
    for (size_t i = 0; i < higherStats.size(); ++i) {
        bool isLast = (i == higherStats.size() - 1);
        ExcelExporter::exportSingleStatToColumn(higherStats[i], col++, /*startRow=*/12, isLast);
    }

    // allStats → начинаем с строки 18 (C19)
    col = 2;
    for (size_t i = 0; i < allStats.size(); ++i) {
        bool isLast = (i == allStats.size() - 1);
        ExcelExporter::exportSingleStatToColumn(allStats[i], col++, /*startRow=*/18, isLast);
    }

     // --- ШАГ 3: Сохраняем книгу ---
    if (!ExcelExporter::saveWorkbook(filename)) {
        logger.log(LOG("❌ Ошибка при сохранении Excel-файла"));
        std::cerr << "❌ Не удалось сохранить файл\n";
        return 1;
    }

    std::string logMsg = "✅ Данные успешно экспортированы в: " + filename;
    std::cout << logMsg << "\n";
    logger.log(LOG(logMsg));

    logger.log(LOG("Program finished successfully"));
    return 0;
}
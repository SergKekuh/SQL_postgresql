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
        std::cout << stat.category << " | "
                  << stat.companies << " | "
                  << stat.total_invoices << " | "
                  << stat.total_sales << " | "
                  << stat.avg_sales << " | "
                  << stat.percent_share << "%" << std::endl;
    }

    // Создаём папку reports/, если её нет
    if (!ExcelExporter::createReportsDirectoryIfNotExists("reports/")) {
        const char* msg = "Error creating reports directory!";
        std::cerr << msg << std::endl;
        logger.log(LOG(msg));
        return 1;
    }

    // Генерируем имя файла с датой и временем
    std::string filename = "reports/" + ExcelExporter::generateFilenameWithTimestamp("statistics_report", ".xlsx");

    // Сохраняем все данные в один Excel-файл на разных листах
    if (ExcelExporter::exportAllToXlsx(filename, allStats, belowStats, higherStats)) {
        std::string logMsg = "Data successfully exported to: " + filename;
        std::cout << logMsg << std::endl;
        logger.log(LOG(logMsg.c_str()));
        std::cout << "✅ Отчёт создан: " << filename << "\n";
    } else {
        const char* msg = "Error exporting data to Excel!";
        std::cerr << msg << std::endl;
        logger.log(LOG(msg));
        std::cerr << "❌ Ошибка при экспорте в Excel!\n";
    }

    logger.log(LOG("Program finished successfully"));
    return 0;
}
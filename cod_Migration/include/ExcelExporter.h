#ifndef EXCELEXPORTER_H
#define EXCELEXPORTER_H

#include <string>
#include <vector>
#include "ClientStatistics.h"
#include "GroupReport.h" // Включаем структуру GroupReport
#include "Logger.h"      // Включаем класс Logger
#include <map>
// Подключаем libxl заранее, чтобы использовать типы
#include <libxl.h>

class ExcelExporter {
public:
    // Открывает шаблон Excel
    static bool openTemplate(libxl::Book*& book, const std::string& templatePath);

    // Записывает одну группу данных по столбцу (например, C, D, E...)
    static bool exportToSheet(libxl::Book* book, const std::vector<ClientStatistics>& stats, int startRow);

    // Записывает данные отчета по группам в лист
    static bool exportGroupReportToSheet(libxl::Book* book, const std::vector<GroupReport>& groupReport, int startRow);

    // Экспорт одного статистического объекта
    static bool exportSingleStatToColumn(
        libxl::Book* book,
        const ClientStatistics& stat,
        int col,
        int startRow,
        bool isLast = false);  // Указывает, является ли эта запись последней в группе

    // Сохраняет книгу
    static bool saveWorkbook(libxl::Book* book, const std::string& outputFilename);

    // Записываем год в Excel
    static bool writeYearToSheet(libxl::Book* book, int year);

    // Генерирует имя файла с временной меткой
    static std::string generateFilenameWithTimestamp(
        const std::string& baseName = "statistics_report",
        const std::string& extension = ".xlsx");

    // Создает папку для отчетов, если её нет
    static bool createReportsDirectoryIfNotExists(
        const std::string& dirPath = "reports/");

private:
    
    // Сохраняет формулы перед вставкой столбцов
    static void saveFormulas(libxl::Sheet* sheet, std::map<std::pair<int, int>, std::string>& formulasMap);

    // Восстанавливает формулы после вставки столбцов
    static void restoreFormulas(libxl::Sheet* sheet, const std::map<std::pair<int, int>, std::string>& formulasMap, int numColsInserted);

    // Обновляет ссылки на ячейки в формулах (R1C1-нотация)
    static std::string updateCellReferences(const std::string& formula, int numColsInserted);

    // Логгер для записи сообщений
    static Logger logger;
};

#endif // EXCELEXPORTER_H
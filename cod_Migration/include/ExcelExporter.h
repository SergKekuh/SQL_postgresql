#ifndef EXCELEXPORTER_H
#define EXCELEXPORTER_H

#include <string>
#include <vector>
#include "ClientStatistics.h"

// Подключаем libxl заранее, чтобы использовать типы
#include <libxl.h>

class ExcelExporter {
public:
    // Открывает шаблон Excel
    static bool openTemplate(const std::string& templatePath);

    // Записывает одну группу данных по столбцу (например, C, D, E...)
    static bool exportToSheet(const std::vector<ClientStatistics>& stats, int startRow);

     // Экспорт одного статистического объекта
    static bool exportSingleStatToColumn(
        const ClientStatistics& stat,
        int col,
        int startRow,
        bool isLast = false);  // Указывает, является ли эта запись последней в группе

    // Сохраняет книгу
    static bool saveWorkbook(const std::string& outputFilename);

    // Записываем год в Excel
    static bool writeYearToSheet(int year);

    // Вспомогательные методы
    static std::string generateFilenameWithTimestamp(
        const std::string& baseName = "statistics_report",
        const std::string& extension = ".xlsx");

    static bool createReportsDirectoryIfNotExists(
        const std::string& dirPath = "reports/");

private:
    // Статический указатель на книгу
    static libxl::Book* sharedBook;
};

#endif // EXCELEXPORTER_H
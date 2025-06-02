#ifndef EXCELEXPORTER_H
#define EXCELEXPORTER_H

#include <string>
#include <vector>
#include "ClientStatistics.h"

class ExcelExporter {
public:
    // Открывает шаблон и готовит его для нескольких записей
    static bool openTemplate(const std::string& templatePath);

    // Записывает одну группу статистики
    static bool exportToSheet(
        const std::vector<ClientStatistics>& stats,
        int startRow);  // Начальная строка для этой группы

    // Сохраняет книгу в файл
    static bool saveWorkbook(const std::string& outputFilename);

    // Создание папки reports/
    static bool createReportsDirectoryIfNotExists(const std::string& dirPath = "reports/");
    static std::string generateFilenameWithTimestamp(const std::string& baseName = "statistics_report", const std::string& extension = ".xlsx");

private:
    static libxl::Book* sharedBook;  // Указатель на общий workbook
};

#endif // EXCELEXPORTER_H
#ifndef EXCELEXPORTER_H
#define EXCELEXPORTER_H

#include <vector>
#include "ClientStatistics.h"

class ExcelExporter {
public:
    // Экспорт всех трёх статистик на один лист из шаблона
    static bool exportToSheetFromTemplate(
        const std::string& templatePath,
        const std::string& outputFilename,
        const std::vector<ClientStatistics>& stats,
        int startRow,         // Например: 6 (строка 7)
        int totalRow = 0);   // Например: 6 → I7

    // Генерация имени файла с датой
    static std::string generateFilenameWithTimestamp(const std::string& baseName = "statistics_report", const std::string& extension = ".xlsx");

    // Создание папки reports/
    static bool createReportsDirectoryIfNotExists(const std::string& dirPath = "reports/");
};

#endif // EXCELEXPORTER_H
#ifndef EXCELEXPORTER_H
#define EXCELEXPORTER_H

#include <vector>
#include "ClientStatistics.h"

class ExcelExporter {
public:
    // Экспорт всех наборов данных в один файл на разных листах
    static bool exportAllToXlsx(const std::string& filename,
                                const std::vector<ClientStatistics>& allStats,
                                const std::vector<ClientStatistics>& belowStats,
                                const std::vector<ClientStatistics>& higherStats);

    // Генерация имени файла с датой
    static std::string generateFilenameWithTimestamp(const std::string& baseName = "report", const std::string& extension = ".xlsx");

    // Создать папку reports, если её нет
    static bool createReportsDirectoryIfNotExists(const std::string& dirPath = "reports/");
};

#endif // EXCELEXPORTER_H
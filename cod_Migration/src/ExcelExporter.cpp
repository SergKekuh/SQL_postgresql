#include "ExcelExporter.h"
#include <libxl.h>
#include <ctime>
#include <sys/stat.h>
#include <iostream>

bool ExcelExporter::exportToSheetFromTemplate(
    const std::string& templatePath,
    const std::string& outputFilename,
    const std::vector<ClientStatistics>& stats,
    int startRow,
    int totalRow) {

    using namespace libxl;

    Book* book = xlCreateXMLBook();
    if (!book->load(templatePath.c_str())) {
        std::cerr << "❌ Не удалось загрузить шаблон: " << templatePath << std::endl;
        book->release();
        return false;
    }

    Sheet* sheet = book->getSheet(0);
    if (!sheet) {
        std::cerr << "❌ В шаблоне нет активного листа!" << std::endl;
        book->release();
        return false;
    }

    // Пишем данные начиная со строки startRow + 1 (т.к. в Excel нумерация с 0)
    for (size_t i = 0; i < stats.size(); ++i) {
        const auto& stat = stats[i];

        // Столбцы B - G (C - H по индексу)
        sheet->writeNum(startRow + i, 2, stat.companies);
        sheet->writeNum(startRow + i, 3, stat.total_invoices);
        sheet->writeNum(startRow + i, 4, stat.total_sales);
        sheet->writeNum(startRow + i, 5, stat.avg_sales);
        sheet->writeNum(startRow + i, 6, stat.percent_share);
    }

    // Если указан totalRow — пишем итоговое значение в ячейку I(totalRow)
    if (totalRow > 0) {
        double totalSales = 0;
        for (const auto& stat : stats) {
            totalSales += stat.total_sales;
        }
        sheet->writeStr(totalRow, 8, "Итог:");
        sheet->writeNum(totalRow, 9, totalSales);
    }

    // Сохраняем файл
    bool success = book->save(outputFilename.c_str());
    book->release();

    return success;
}
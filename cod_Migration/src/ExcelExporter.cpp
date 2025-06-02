#include "ExcelExporter.h"
#include <libxl.h>
#include <iostream>

// Статический указатель на книгу
libxl::Book* ExcelExporter::sharedBook = nullptr;

bool ExcelExporter::openTemplate(const std::string& templatePath) {
    if (sharedBook) {
        delete sharedBook;
        sharedBook = nullptr;
    }

    sharedBook = xlCreateXMLBook();
    if (!sharedBook->load(templatePath.c_str())) {
        std::cerr << "❌ Не удалось загрузить шаблон: " << templatePath << "\n";
        return false;
    }

    return true;
}

bool ExcelExporter::exportToSheet(const std::vector<ClientStatistics>& stats, int startRow) {
    using namespace libxl;

    if (!sharedBook) {
        std::cerr << "❌ Книга не открыта. Вызови openTemplate(...) сначала\n";
        return false;
    }

    Sheet* sheet = sharedBook->getSheet(0);
    if (!sheet) {
        std::cerr << "❌ В шаблоне нет листа\n";
        return false;
    }

    for (size_t i = 0; i < stats.size(); ++i) {
        const auto& stat = stats[i];
        int row = startRow + i;

        sheet->writeNum(row, 2, stat.companies);         // C
        sheet->writeNum(row, 3, stat.total_invoices);    // D
        sheet->writeNum(row, 4, stat.total_sales);       // E
        sheet->writeNum(row, 5, stat.avg_sales);         // F
        sheet->writeNum(row, 6, stat.percent_share);     // G
        sheet->writeNum(row, 7, stat.percent_share);     // H
    }

    // Итоговая информация в I столбце
    if (!stats.empty()) {
        double companies = 0;
        double invoices = 0;
        double sales = 0;
        double avg = 0;
        double share = 0;

        for (const auto& s : stats) {
            companies += s.companies;
            invoices += s.total_invoices;
            sales += s.total_sales;
            share += s.percent_share;
        }

        avg = sales / stats.size();

        sheet->writeStr(startRow + 1, 8, "Итого:");
        sheet->writeNum(startRow + 1, 9, companies);
        sheet->writeNum(startRow + 1, 10, invoices);
        sheet->writeNum(startRow + 1, 11, sales);
        sheet->writeNum(startRow + 1, 12, avg);
        sheet->writeNum(startRow + 1, 13, share);
    }

    return true;
}

bool ExcelExporter::saveWorkbook(const std::string& outputFilename) {
    if (!sharedBook) {
        std::cerr << "❌ Книга не была создана или уже сохранена\n";
        return false;
    }

    bool success = sharedBook->save(outputFilename.c_str());
    sharedBook->release();
    sharedBook = nullptr;

    return success;
}
#include "ExcelExporter.h"
#include <libxl.h>
#include <ctime>
#include <sys/stat.h>  // Для mkdir (Linux)
#include <iostream>

bool ExcelExporter::exportAllToXlsx(const std::string& filename,
                                   const std::vector<ClientStatistics>& allStats,
                                   const std::vector<ClientStatistics>& belowStats,
                                   const std::vector<ClientStatistics>& higherStats) {
    using namespace libxl;

    Book* book = xlCreateXMLBook();
    if (!book) return false;

    // Лист 1: All Statistics
    Sheet* sheet1 = book->addSheet("All Statistics");
    if (sheet1) {
        int row = 1;
        sheet1->writeStr(0, 0, "Category"); sheet1->writeStr(0, 1, "Companies");
        sheet1->writeStr(0, 2, "Total Invoices"); sheet1->writeStr(0, 3, "Total Sales");
        sheet1->writeStr(0, 4, "Avg Sales"); sheet1->writeStr(0, 5, "Percent Share (%)");

        for (const auto& stat : allStats) {
            sheet1->writeStr(row, 0, stat.category.c_str());
            sheet1->writeNum(row, 1, stat.companies);
            sheet1->writeNum(row, 2, stat.total_invoices);
            sheet1->writeNum(row, 3, stat.total_sales);
            sheet1->writeNum(row, 4, stat.avg_sales);
            sheet1->writeNum(row, 5, stat.percent_share);
            ++row;
        }
    }

    // Лист 2: Below Threshold
    Sheet* sheet2 = book->addSheet("Below Threshold");
    if (sheet2) {
        int row = 1;
        sheet2->writeStr(0, 0, "Category"); sheet2->writeStr(0, 1, "Companies");
        sheet2->writeStr(0, 2, "Total Invoices"); sheet2->writeStr(0, 3, "Total Sales");
        sheet2->writeStr(0, 4, "Avg Sales"); sheet2->writeStr(0, 5, "Percent Share (%)");

        for (const auto& stat : belowStats) {
            sheet2->writeStr(row, 0, stat.category.c_str());
            sheet2->writeNum(row, 1, stat.companies);
            sheet2->writeNum(row, 2, stat.total_invoices);
            sheet2->writeNum(row, 3, stat.total_sales);
            sheet2->writeNum(row, 4, stat.avg_sales);
            sheet2->writeNum(row, 5, stat.percent_share);
            ++row;
        }
    }

    // Лист 3: Higher Threshold
    Sheet* sheet3 = book->addSheet("Higher Threshold");
    if (sheet3) {
        int row = 1;
        sheet3->writeStr(0, 0, "Category"); sheet3->writeStr(0, 1, "Companies");
        sheet3->writeStr(0, 2, "Total Invoices"); sheet3->writeStr(0, 3, "Total Sales");
        sheet3->writeStr(0, 4, "Avg Sales"); sheet3->writeStr(0, 5, "Percent Share (%)");

        for (const auto& stat : higherStats) {
            sheet3->writeStr(row, 0, stat.category.c_str());
            sheet3->writeNum(row, 1, stat.companies);
            sheet3->writeNum(row, 2, stat.total_invoices);
            sheet3->writeNum(row, 3, stat.total_sales);
            sheet3->writeNum(row, 4, stat.avg_sales);
            sheet3->writeNum(row, 5, stat.percent_share);
            ++row;
        }
    }

    // Сохраняем
    bool success = book->save(filename.c_str());
    book->release();

    return success;
}

std::string ExcelExporter::generateFilenameWithTimestamp(const std::string& baseName, const std::string& extension) {
    time_t now = time(nullptr);
    tm* localTime = localtime(&now);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", localTime);

    return baseName + "_" + buffer + extension;
}

bool ExcelExporter::createReportsDirectoryIfNotExists(const std::string& dirPath) {
    struct stat info;

    if (stat(dirPath.c_str(), &info) != 0) {
        // Папка не существует — создаём
#ifdef _WIN32
        // Windows
        if (_mkdir(dirPath.c_str()) != 0) {
#else
        // Linux / macOS
        if (mkdir(dirPath.c_str(), 0777) != 0) {
#endif
            std::cerr << "Failed to create directory: " << dirPath << std::endl;
            return false;
        }
        std::cout << "Created directory: " << dirPath << std::endl;
    }

    return true;
}
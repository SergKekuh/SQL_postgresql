#include "ExcelExporter.h"
#include <iostream>
#include <ctime>
#include <sys/stat.h>

// Определяем статическую переменную


bool ExcelExporter::openTemplate(libxl::Book*& book, const std::string& templatePath) {
    if (book) {
        delete book;
        book = nullptr;
    }

    book = xlCreateXMLBook(); // Используем только xlCreateXMLBook() для .xlsx
    if (!book->load(templatePath.c_str())) {
        const char* errorMessage = book->errorMessage();
        std::cerr << "❌ Ошибка загрузки шаблона: " << templatePath << "\n"
                  << "  Сообщение libxl: " << (errorMessage ? errorMessage : "Неизвестная ошибка") << "\n";
        book->release();
        book = nullptr;
        return false;
    }

    return true;
}

// Записывает данные отчета по группам в лист
bool ExcelExporter::exportGroupReportToSheet(libxl::Book* book, const std::vector<GroupReport>& groupReport, int startRow) {
    try {
        if (!book) {
            throw std::runtime_error("Excel book is not initialized.");
        }

        libxl::Sheet* sheet = book->getSheet(0); // Первый лист
        if (!sheet) {
            throw std::runtime_error("Sheet not found in the workbook.");
        }

        int row = startRow;
        for (const auto& item : groupReport) {
            sheet->writeStr(row, 1, item.group_name.c_str()); // Колонка B
            sheet->writeNum(row, 2, item.total_sale);         // Колонка C
            sheet->writeNum(row, 3, item.total_companies);    // Колонка D
            ++row;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error exporting group report to sheet: " << e.what() << std::endl;
        return false;
    }
}


bool ExcelExporter::exportToSheet(libxl::Book* book, const std::vector<ClientStatistics>& stats, int startRow) {
    using namespace libxl;

    if (!book) {
        std::cerr << "❌ Книга не открыта.\n";
        return false;
    }

    Sheet* sheet = book->getSheet(0);
    if (!sheet) {
        std::cerr << "❌ Лист не найден в шаблоне\n";
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
    }

    // Итоговое значение в I столбце
    if (!stats.empty()) {
        double totalSales = 0;
        for (const auto& s : stats) {
            totalSales += s.total_sales;
        }

        int totalRow = startRow + stats.size() + 1;

        sheet->writeStr(totalRow, 7, "Итого:");
        sheet->writeNum(totalRow, 8, totalSales);
    }

    return true;
}


bool ExcelExporter::exportSingleStatToColumn(
    libxl::Book* book,
    const ClientStatistics& stat,
    int col,
    int startRow,
    bool isLast) {

    using namespace libxl;

    if (!book) {
        std::cerr << "❌ Книга не открыта.\n";
        return false;
    }

    Sheet* sheet = book->getSheet(0);
    if (!sheet) {
        std::cerr << "❌ Лист не найден в шаблоне\n";
        return false;
    }
    if (!isLast) {
        // Пишем данные по строкам, начиная со startRow
        sheet->writeNum(startRow + 0, col, stat.companies);         // C7, D7...
        sheet->writeNum(startRow + 1, col, stat.total_invoices);   // C8, D8...
        sheet->writeNum(startRow + 2, col, stat.total_sales);      // C9, D9...
        sheet->writeNum(startRow + 3, col, stat.avg_sales);        // C10, D10...
        sheet->writeNum(startRow + 4, col, stat.percent_share);   // C11, D11...
        sheet->writeNum(startRow + 5, col, stat.percent_share);    // C12, D12...
    } else {
        Font* boldFont = book->addFont();
        boldFont->setSize(12);
        boldFont->setBold(true);

        Format* boldFormat = book->addFormat();
        boldFormat->setFont(boldFont);

        sheet->writeNum(startRow + 0, 8, stat.companies);         // C7, D7...
        sheet->writeNum(startRow + 1, 8, stat.total_invoices);   // C8, D8...
        sheet->writeNum(startRow + 2, 8, stat.total_sales);      // C9, D9...
        sheet->writeNum(startRow + 3, 8, stat.avg_sales);        // C10, D10...
        sheet->writeNum(startRow + 4, 8, stat.percent_share);   // C11, D11...
        sheet->writeNum(startRow + 5, 8, stat.percent_share);    // C12, D12...
    }

    return true;
}

bool ExcelExporter::saveWorkbook(libxl::Book* book, const std::string& outputFilename) {
    if (!book) {
        std::cerr << "❌ Книга не была создана или уже сохранена\n";
        return false;
    }

    bool success = book->save(outputFilename.c_str());
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
#ifdef _WIN32
        if (_mkdir(dirPath.c_str()) != 0) {
#else
        if (mkdir(dirPath.c_str(), 0777) != 0) {
#endif
            std::cerr << "❌ Не могу создать папку reports/\n";
            return false;
        }
        std::cout << "🟢 Создана папка: " << dirPath << "\n";
    }

    return true;
}

bool ExcelExporter::writeYearToSheet(libxl::Book* book, int year) {
    using namespace libxl;

    if (!book) {
        std::cerr << "❌ Книга не открыта.\n";
        return false;
    }

    Sheet* sheet = book->getSheet(0);
    if (!sheet) {
        std::cerr << "❌ Лист не найден в шаблоне\n";
        return false;
    }

    // Пишем год в ячейку B7 (строка 6, столбец 1)
    sheet->writeNum(5, 1, year);  // Ячейка B7

    return true;
}
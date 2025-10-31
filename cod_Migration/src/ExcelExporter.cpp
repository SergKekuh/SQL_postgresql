#include "ExcelExporter.h"
#include <iostream>
#include <ctime>
#include <sys/stat.h>
#include <filesystem>
#include <fstream>
#include <map>

// Определяем статическую переменную
Logger ExcelExporter::logger("logs/");

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
            throw std::runtime_error("Лист не найден в шаблоне.");
        }

        // --- Шаг 1: Сохраняем все формулы ---
        std::map<std::pair<int, int>, std::string> formulasMap;
        saveFormulas(sheet, formulasMap);

        // --- Шаг 2: Вставляем два новых столбца, начиная со столбца B (индекс 1) ---
        int numColsInserted = 2; // Количество вставленных столбцов
        sheet->insertCol(1, 2); // Вставляем два столбца, начиная со столбца B (индекс 1)

        // --- Шаг 3: Восстанавливаем формулы ---
        restoreFormulas(sheet, formulasMap, numColsInserted);

        // --- Шаг 4: Записываем новые данные в вставленные столбцы ---
        int num = 0;
        int row = startRow + 1;
        for (const auto& item : groupReport) {
            if (row == 9) {
                sheet->writeNum(row, 2, num); // Колонка F (была D)
                ++row;
            }
            sheet->writeNum(row, 1, item.total_sale);         // Колонка E (была C)
            sheet->writeNum(row, 2, item.total_companies);    // Колонка F (была D)
            num += item.total_companies;
            ++row;
        }

        return true;
    } catch (const std::exception& e) {
        logger.log(logger.format("❌ Ошибка экспорта отчета по группам: %s", e.what()));
        return false;
    }
}

std::string ExcelExporter::updateCellReferences(const std::string& formula, int numColsInserted) {
    std::string updatedFormula = formula;

    // Ищем все ссылки на столбцы в R1C1-нотации (например, "RC[2]" или "R[-7]C")
    size_t pos = 0;
    while ((pos = updatedFormula.find("C[", pos)) != std::string::npos) {
        size_t start = pos + 2; // Начало числа после "C["
        size_t end = updatedFormula.find(']', start);

        if (end != std::string::npos) {
            std::string offsetStr = updatedFormula.substr(start, end - start);
            int offset = std::stoi(offsetStr);

            // Обновляем смещение столбца
            offset += numColsInserted;

            // Заменяем старое значение на новое
            updatedFormula.replace(start, end - start, std::to_string(offset));
            pos = end; // Продолжаем поиск с конца текущего совпадения
        } else {
            break; // Если ']' не найден, выходим из цикла
        }
    }

    return updatedFormula;
}

void ExcelExporter::saveFormulas(libxl::Sheet* sheet, std::map<std::pair<int, int>, std::string>& formulasMap) {
    if (!sheet) {
        logger.log("❌ Лист не найден.");
        return;
    }

    int rowCount = sheet->lastRow(); // Последняя непустая строка
    int colCount = 255;              // Максимальное количество столбцов (A-ZZ)

    for (int row = 0; row <= rowCount; ++row) {
        for (int col = 0; col <= colCount; ++col) {
            const char* formula = sheet->readFormula(row, col);
            if (formula) {
                formulasMap[{row, col}] = formula;
                logger.log(logger.format("Найдена формула: %s в ячейке (%d, %d)", formula, row, col));
            }
        }
    }
}

void ExcelExporter::restoreFormulas(libxl::Sheet* sheet, const std::map<std::pair<int, int>, std::string>& formulasMap, int numColsInserted) {
    if (!sheet) {
        logger.log("❌ Лист не найден.");
        return;
    }

    for (const auto& [coords, formula] : formulasMap) {
        int originalRow = coords.first;
        int originalCol = coords.second;

        // Обновляем столбец с учетом смещения
        int updatedCol = originalCol + numColsInserted;

        // Обновляем ссылки в формуле
        std::string updatedFormula = updateCellReferences(formula, numColsInserted);

        if (!updatedFormula.empty()) {
            sheet->writeFormula(originalRow, updatedCol, updatedFormula.c_str());
            logger.log(logger.format("Формула восстановлена: %s в ячейке (%d, %d)", updatedFormula.c_str(), originalRow, updatedCol));
        } else {
            logger.log(logger.format("❌ Обновленная формула пуста для ячейки (%d, %d).", originalRow, updatedCol));
        }
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

    // Проверяем, существует ли директория для сохранения файла
    std::filesystem::path dir = std::filesystem::path(outputFilename).parent_path();
    if (!std::filesystem::exists(dir)) {
        std::cerr << "❌ Директория для сохранения файла не существует: " << dir << "\n";
        book->release();
        return false;
    }

    std::cout << "Сохраняем файл: " << outputFilename << "\n";

    bool success = book->save(outputFilename.c_str());
    if (!success) {
        const char* errorMessage = book->errorMessage();
        std::cerr << "❌ Ошибка при сохранении книги: " << outputFilename << "\n"
                  << "  Сообщение libxl: " << (errorMessage ? errorMessage : "Неизвестная ошибка") << "\n";
        book->release();
        return true; // Продолжаем выполнение программы
    }
    std::cout << "Файл сохранён: " << outputFilename << "\n";
    book->release();
    return true;
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
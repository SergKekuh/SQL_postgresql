#include "../include/CategoryMatcher.h"
#include "../include/Database.h" // Предполагаем, что этот файл содержит PaymentRecord и Database
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>

// Подключаем libxl C++ API
#include <libxl.h>

int main() {
    setlocale(LC_ALL, ""); // Для кириллицы

    std::cout << "🚀 Запуск Finance Classifier..." << std::endl;

    // 1. Подключение к БД
    Database db("localhost", "finance_db", "postgres", "root");

    // 2. Инициализация матчера
    std::cout << "📂 Загрузка категорий..." << std::endl;
    CategoryMatcher matcher("categories.csv");

    // Списки таблиц (порядок важен для столбцов общей таблицы)
    // 0: Приват УТСК, 1: Аваль УТСК, 2: Приват Форпост, 3: Аваль Форпост
    std::vector<std::string> tables = {
        "payments_privat_utsk",
        "payments_aval_utsk",
        "payments_privat_forpost",
        "payments_aval_forpost"
    };

    // Структура для накопления сумм:
    // Key: ID категории
    // Value: вектор из 4 чисел (сумма по каждой таблице)
    std::map<int, std::vector<double>> summaryData;
    
    // Предзаполняем map нулями для всех ID от 1 до 116, чтобы таблица была полной
    for (int i = 1; i <= 116; ++i) {
        summaryData[i] = {0.0, 0.0, 0.0, 0.0};
    }

    // 3. Создаём книгу Excel
    libxl::Book* book = xlCreateXMLBook();
    if (!book) {
        std::cerr << "❌ Ошибка: не удалось создать объект Excel." << std::endl;
        return 1;
    }

    // --- СТИЛИ ---
    // Шрифт жирный
    libxl::Font* boldFont = book->addFont();
    boldFont->setBold(true);
    boldFont->setSize(10);
    boldFont->setName("Arial");

    // Заголовки (Серый фон)
    libxl::Format* headerFormat = book->addFormat();
    headerFormat->setFont(boldFont);
    headerFormat->setAlignH(libxl::ALIGNH_CENTER);
    headerFormat->setAlignV(libxl::ALIGNV_CENTER);
    headerFormat->setBorder(libxl::BORDERSTYLE_THIN);
    headerFormat->setFillPattern(libxl::FILLPATTERN_SOLID);
    headerFormat->setPatternForegroundColor(libxl::COLOR_GRAY25); 
    headerFormat->setWrap(true);

    // Обычная ячейка с рамкой
    libxl::Format* cellFormat = book->addFormat();
    cellFormat->setBorder(libxl::BORDERSTYLE_THIN);

    // Денежный формат с рамкой (для красоты: 0.00)
    libxl::Format* moneyFormat = book->addFormat();
    moneyFormat->setBorder(libxl::BORDERSTYLE_THIN);
    moneyFormat->setNumFormat(book->addCustomNumFormat("#,##0.00"));

    // Желтый стиль (для Зарплаты)
    libxl::Format* yellowFormat = book->addFormat();
    yellowFormat->setBorder(libxl::BORDERSTYLE_THIN);
    yellowFormat->setFillPattern(libxl::FILLPATTERN_SOLID);
    yellowFormat->setPatternForegroundColor(libxl::COLOR_LIGHTYELLOW); // Или COLOR_YELLOW
    yellowFormat->setNumFormat(book->addCustomNumFormat("#,##0.00"));
    
    // Желтый текстовый
    libxl::Format* yellowTextFormat = book->addFormat();
    yellowTextFormat->setBorder(libxl::BORDERSTYLE_THIN);
    yellowTextFormat->setFillPattern(libxl::FILLPATTERN_SOLID);
    yellowTextFormat->setPatternForegroundColor(libxl::COLOR_LIGHTYELLOW);
    // -------------

    // Вектор нераспознанных
    std::vector<PaymentRecord> all_unclassified;

    // --- ЦИКЛ ПО ТАБЛИЦАМ (СБОР ДАННЫХ) ---
    for (size_t tIndex = 0; tIndex < tables.size(); ++tIndex) {
        std::string tableName = tables[tIndex];
        std::cout << "\n📊 Обработка: " << tableName << "..." << std::endl;

        auto records = db.loadPaymentsFromDB(tableName);
        
        // Создаем детальный лист для каждой выписки
        libxl::Sheet* sheet = book->addSheet(tableName.c_str());
        if (sheet) {
            // !!! ВАЖНО: Добавляем столбцы для Подраздела и Назначения
            sheet->setCol(0, 0, 12); 
            sheet->setCol(1, 1, 60); 
            sheet->setCol(2, 2, 15); 
            sheet->setCol(3, 3, 8);
            sheet->setCol(4, 4, 30); // Подраздел
            sheet->setCol(5, 5, 40); // Назначение
            
            sheet->writeStr(0, 0, "Дата", headerFormat);
            sheet->writeStr(0, 1, "Назначение платежа", headerFormat);
            sheet->writeStr(0, 2, "Сумма", headerFormat);
            sheet->writeStr(0, 3, "ID", headerFormat);
            sheet->writeStr(0, 4, "Подраздел", headerFormat); // НОВЫЙ СТОЛБЕЦ
            sheet->writeStr(0, 5, "Категория", headerFormat); // НОВЫЙ СТОЛБЕЦ
            
            int row = 1;
            for (const auto& rec : records) {
                int id = matcher.match(rec.purpose);
                
                if (id == 0) {
                    all_unclassified.push_back(rec);
                } else {
                    // НАКОПЛЕНИЕ СУММ ДЛЯ ОБЩЕЙ ТАБЛИЦЫ
                    summaryData[id][tIndex] += rec.amount;
                }

                // Запись в детальный лист
                sheet->writeStr(row, 0, rec.operation_date.c_str(), cellFormat);
                sheet->writeStr(row, 1, rec.purpose.c_str(), cellFormat);
                sheet->writeNum(row, 2, rec.amount, moneyFormat);
                sheet->writeNum(row, 3, id, cellFormat);
                // НОВЫЕ СТОЛБЦЫ:
                sheet->writeStr(row, 4, matcher.getCategorySubcategory(id).c_str(), cellFormat);
                sheet->writeStr(row, 5, matcher.getCategoryName(id).c_str(), cellFormat);
                
                row++;
            }
        }
    }

    // --- СОЗДАНИЕ ОБЩЕЙ ТАБЛИЦЫ ---
    std::cout << "\n📈 Генерация 'Общая таблица'..." << std::endl;
    libxl::Sheet* sumSheet = book->addSheet("Общая таблица");
    if (sumSheet) {
        // Настройка ширины
        sumSheet->setCol(0, 0, 5);  // №
        sumSheet->setCol(1, 1, 20); // Принадлежность
        sumSheet->setCol(2, 2, 30); // <<< НОВЫЙ: Подраздел
        sumSheet->setCol(3, 3, 40); // Назначение
        sumSheet->setCol(4, 7, 18); // Столбцы банков (теперь начинаются с 4)
        sumSheet->setCol(8, 8, 20); // Итого (теперь 8)

        // Заголовки
        sumSheet->writeStr(0, 0, "№ п/п", headerFormat);
        sumSheet->writeStr(0, 1, "Принадлежность", headerFormat);
        sumSheet->writeStr(0, 2, "Подраздел", headerFormat); // <<< НОВЫЙ СТОЛБЕЦ
        sumSheet->writeStr(0, 3, "Назначение", headerFormat);
        sumSheet->writeStr(0, 4, "Приват УТСК", headerFormat);
        sumSheet->writeStr(0, 5, "Аваль УТСК", headerFormat);
        sumSheet->writeStr(0, 6, "Приват Форпост", headerFormat);
        sumSheet->writeStr(0, 7, "Аваль Форпост", headerFormat);
        sumSheet->writeStr(0, 8, "ИТОГО", headerFormat); // Сдвинут

        // Итоговые суммы по столбцам (внизу)
        std::vector<double> colTotals(5, 0.0); // 4 банка + 1 общий итог

        int row = 1;
        // Проходим по всем известным ID (1..116)
        for (int id = 1; id <= 116; ++id) {
            std::string parent = matcher.getCategoryParent(id);
            // <<< НОВЫЙ: Получаем подраздел
            std::string subcategory = matcher.getCategorySubcategory(id); 
            std::string name = matcher.getCategoryName(id);
            
            // Проверка: пустая категория (если в CSV есть дырки)?
            if (name.empty() || name == "Unknown") continue; 

            // Определяем стиль строки (желтый для Зарплаты)
            libxl::Format* currentTextFmt = cellFormat;
            libxl::Format* currentMoneyFmt = moneyFormat;

            if (parent == "Зарплата") {
                currentTextFmt = yellowTextFormat;
                currentMoneyFmt = yellowFormat;
            }

            // Данные сумм
            const auto& sums = summaryData[id];
            double rowTotal = 0.0;

            sumSheet->writeNum(row, 0, id, currentTextFmt);
            sumSheet->writeStr(row, 1, parent.c_str(), currentTextFmt);
            // <<< НОВЫЙ СТОЛБЕЦ: Подраздел
            sumSheet->writeStr(row, 2, subcategory.c_str(), currentTextFmt); 
            sumSheet->writeStr(row, 3, name.c_str(), currentTextFmt);

            // Пишем суммы по банкам (теперь с 4-го столбца: 3 + k)
            for (int k = 0; k < 4; ++k) {
                double val = sums[k];
                sumSheet->writeNum(row, 4 + k, val, currentMoneyFmt);
                
                rowTotal += val;
                colTotals[k] += val; // копим итог столбца
            }

            // Пишем итог строки (теперь в 8-м столбце: 4 + 4 = 8)
            sumSheet->writeNum(row, 8, rowTotal, currentMoneyFmt);
            colTotals[4] += rowTotal;

            row++;
        }

        // Строка ИТОГО в самом низу
        libxl::Format* totalRowFormat = book->addFormat();
        totalRowFormat->setFont(boldFont);
        totalRowFormat->setBorder(libxl::BORDERSTYLE_THIN);
        totalRowFormat->setNumFormat(book->addCustomNumFormat("#,##0.00"));
        totalRowFormat->setFillPattern(libxl::FILLPATTERN_SOLID);
        totalRowFormat->setPatternForegroundColor(libxl::COLOR_GRAY25);

        // Итоговая строка теперь сдвинута
        sumSheet->writeStr(row, 3, "ВСЕГО:", headerFormat); // Было 2, стало 3
        sumSheet->writeNum(row, 4, colTotals[0], totalRowFormat);
        sumSheet->writeNum(row, 5, colTotals[1], totalRowFormat);
        sumSheet->writeNum(row, 6, colTotals[2], totalRowFormat);
        sumSheet->writeNum(row, 7, colTotals[3], totalRowFormat);
        sumSheet->writeNum(row, 8, colTotals[4], totalRowFormat);
    }

    // 4. Лист нераспознанных (остается без изменений)
    if (!all_unclassified.empty()) {
        libxl::Sheet* badSheet = book->addSheet("Нераспознанные");
        if (badSheet) {
            badSheet->setCol(1, 1, 60);
            badSheet->writeStr(0, 0, "Дата", headerFormat);
            badSheet->writeStr(0, 1, "Назначение", headerFormat);
            badSheet->writeStr(0, 2, "Сумма", headerFormat);

            int row = 1;
            double totalBad = 0.0;
            for (const auto& r : all_unclassified) {
                badSheet->writeStr(row, 0, r.operation_date.c_str(), cellFormat);
                badSheet->writeStr(row, 1, r.purpose.c_str(), cellFormat);
                badSheet->writeNum(row, 2, r.amount, moneyFormat);
                totalBad += r.amount;
                row++;
            }
            badSheet->writeStr(row, 1, "ИТОГО НЕ РАСПРЕДЕЛЕНО:", headerFormat);
            badSheet->writeNum(row, 2, totalBad, moneyFormat);
        }
    }

    // 5. Сохранение
    const char* filename = "classified_payments_result.xlsx";
    if (book->save(filename)) {
        std::cout << "\n✅ Файл " << filename << " успешно создан!" << std::endl;
    } else {
        std::cerr << "❌ Ошибка сохранения: " << book->errorMessage() << std::endl;
    }

    book->release();
    return 0;
}
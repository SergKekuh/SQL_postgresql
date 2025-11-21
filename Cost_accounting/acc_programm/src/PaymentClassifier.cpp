#include "../include/PaymentClassifier.h"
#include "../include/CategoryClassifier.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

// Вспомогательная функция для приведения строки к нижнему регистру
static std::string toLower(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return result;
}

PaymentClassifier::PaymentClassifier(const std::string& csvPath) {
    loadCategoriesFromCSV(csvPath);
}

void PaymentClassifier::loadCategoriesFromCSV(const std::string& csvPath) {
    std::ifstream file(csvPath);
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл категорий: " << csvPath << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line); // пропускаем заголовок

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string id_str, parent, subcat, name, keywords_str;

        if (std::getline(ss, id_str, ',') &&
            std::getline(ss, parent, ',') &&
            std::getline(ss, subcat, ',') &&
            std::getline(ss, name, ',') &&
            std::getline(ss, keywords_str, '\n')) {

            int id = std::stoi(id_str);

            // Парсим ключевые слова
            std::vector<std::string> keywords;
            std::stringstream kwStream(keywords_str);
            std::string kw;
            while (std::getline(kwStream, kw, ';')) {
                // Убираем лишние пробелы
                kw.erase(kw.find_last_not_of(" \t\r\n") + 1);
                kw.erase(0, kw.find_first_not_of(" \t\r\n"));
                if (!kw.empty()) {
                    keywords.push_back(kw);
                }
            }

            // Создаём классификатор
            classifiers_.push_back(std::make_unique<CategoryClassifier>(id, keywords));

            // Сохраняем информацию о категории
            CategoryInfo info;
            info.parent = parent;
            info.subcategory = subcat;
            info.name = name;
            categoryMap[id] = info;

            // DEBUG: Выводим, что загрузили
            // std::cout << "[DEBUG] Загружена категория ID: " << id << ", keywords: ";
            // for (const auto& k : keywords) std::cout << k << " ";
            // std::cout << std::endl;
        }
    }
    file.close();
}

int PaymentClassifier::classify(const std::string& purpose) const {
    std::string lowerPurpose = toLower(purpose);

    for (const auto& classifier : classifiers_) {
        // DEBUG: Проверяем каждый классификатор
        // std::cout << "[DEBUG] Проверяем категорию ID: " << classifier->getId() << std::endl;

        if (classifier->matches(lowerPurpose)) {
            // DEBUG: Успешное совпадение
            // std::cout << "[DEBUG] Совпадение! ID: " << classifier->getId() << " для purpose: " << purpose << std::endl;
            return classifier->getId();
        }
    }

    // DEBUG: Ничего не нашли
    // std::cout << "[DEBUG] Не распознано: " << purpose << std::endl;
    return 0; // не распознано
}

std::string PaymentClassifier::getCategoryParent(int id) const {
    auto it = categoryMap.find(id);
    if (it != categoryMap.end()) {
        return it->second.parent;
    }
    return "Неизвестно";
}

std::string PaymentClassifier::getCategorySubcategory(int id) const {
    auto it = categoryMap.find(id);
    if (it != categoryMap.end()) {
        return it->second.subcategory;
    }
    return "";
}

std::string PaymentClassifier::getCategoryName(int id) const {
    auto it = categoryMap.find(id);
    if (it != categoryMap.end()) {
        return it->second.name;
    }
    return "Неизвестно";
}
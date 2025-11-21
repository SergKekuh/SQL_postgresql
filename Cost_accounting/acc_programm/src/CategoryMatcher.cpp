#include "../include/CategoryMatcher.h"
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

CategoryMatcher::CategoryMatcher(const std::string& csvPath) {
    std::cout << "[DEBUG] Попытка открыть файл: " << csvPath << std::endl;

    std::ifstream file(csvPath);
    if (!file.is_open()) {
        std::cerr << "❌ Ошибка: не удалось открыть файл категорий: " << csvPath << std::endl;
        std::cerr << "🔧 Проверь, что файл существует и путь указан правильно." << std::endl;
        return;
    } else {
        std::cout << "✅ Файл успешно открыт." << std::endl;
    }

    std::string line;
    std::getline(file, line); // пропускаем заголовок

    int lineNumber = 1;

    while (std::getline(file, line)) {
        lineNumber++;
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
                kw.erase(kw.find_last_not_of(" \t\r\n") + 1);
                kw.erase(0, kw.find_first_not_of(" \t\r\n"));
                if (!kw.empty()) {
                    keywords.push_back(kw);
                }
            }

            // DEBUG: Выводим, что загрузили
            std::cout << "[DEBUG CSV] Строка " << lineNumber << " | ID: " << id << ", ключевые слова: ";
            for (const auto& k : keywords) std::cout << "'" << k << "' ";
            std::cout << std::endl;

            // Создаём категорию
            Category cat;
            cat.id = id;
            cat.parent = parent;
            cat.subcategory = subcat;
            cat.name = name;
            cat.keywords = keywords;

            categories_.push_back(cat);
        } else {
            std::cerr << "[DEBUG CSV] Ошибка парсинга строки " << lineNumber << ": " << line << std::endl;
        }
    }
    file.close();
    std::cout << "[DEBUG] Загружено " << categories_.size() << " категорий из CSV." << std::endl;
}

int CategoryMatcher::match(const std::string& purpose) const {
    std::string lowerPurpose = toLower(purpose);

    for (const auto& cat : categories_) {
        for (const auto& kw : cat.keywords) {
            if (lowerPurpose.find(toLower(kw)) != std::string::npos) {
                std::cout << "[DEBUG MATCH] Совпадение! ID: " << cat.id << ", ключевое слово: '" << kw << "', строка: '" << purpose << "'" << std::endl;
                return cat.id;
            }
        }
    }
    return 0; // не распознано
}

std::string CategoryMatcher::getCategoryName(int id) const {
    for (const auto& cat : categories_) {
        if (cat.id == id) {
            return cat.name;
        }
    }
    return "Неизвестно";
}

std::string CategoryMatcher::getCategoryParent(int id) const {
    for (const auto& cat : categories_) {
        if (cat.id == id) {
            return cat.parent;
        }
    }
    return "Неизвестно";
}
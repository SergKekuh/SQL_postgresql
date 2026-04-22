#include "../include/CategoryMatcher.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>

CategoryMatcher::CategoryMatcher(const std::string& csvPath) {
    std::cout << "[DEBUG] Открытие файла категорий: " << csvPath << std::endl;

    std::ifstream file(csvPath);
    if (!file.is_open()) {
        std::cerr << "❌ Ошибка: Файл не найден: " << csvPath << std::endl;
        return;
    }

    std::string line;
    // Пропускаем заголовок
    if (!std::getline(file, line)) return;

    int lineNumber = 1;
    while (std::getline(file, line)) {
        lineNumber++;
        if (line.empty()) continue;

        std::vector<std::string> fields = parseCsvLine(line);

        if (fields.size() < 5) {
            continue; // Пропускаем битые строки
        }

        try {
            Category cat;
            cat.id = std::stoi(fields[0]);
            cat.parent = fields[1];
            cat.subcategory = fields[2];
            cat.name = fields[3];
            
            // Парсинг ключевых слов
            std::string rawKeywords = fields[4]; 
            std::vector<std::string> tokens = split(rawKeywords, ';');
            
            for (const auto& token : tokens) {
                if (!token.empty()) {
                    // Сохраняем ключевое слово в нижнем регистре
                    cat.keywords.push_back(toLower(token));
                }
            }

            categories_.push_back(cat);

        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Ошибка в строке " << lineNumber << ": " << e.what() << std::endl;
        }
    }
    std::cout << "✅ Успешно загружено " << categories_.size() << " категорий." << std::endl;
}

// --- ГЛАВНОЕ ОБНОВЛЕНИЕ: СИСТЕМА ВЕСОВ ---
int CategoryMatcher::match(const std::string& purpose) const {
    std::string lowerPurpose = toLower(purpose);
    
    int bestCategoryId = 0;
    int maxScore = 0;

    // Проходим по ВСЕМ категориям
    for (const auto& cat : categories_) {
        int currentScore = 0;

        for (const auto& kw : cat.keywords) {
            // Если ключевое слово найдено в назначении платежа
            if (lowerPurpose.find(kw) != std::string::npos) {
                // Начисляем баллы
                // Можно усложнить: давать больше баллов за длинные фразы
                // Но пока просто: 1 совпадение = 10 баллов
                currentScore += 10;
            }
        }

        // Если эта категория набрала больше баллов, чем текущий лидер
        if (currentScore > maxScore) {
            maxScore = currentScore;
            bestCategoryId = cat.id;
        }
    }

    // Возвращаем ID победителя (или 0, если совпадений нет)
    return bestCategoryId;
}

// --- Вспомогательные функции (без изменений) ---

std::vector<std::string> CategoryMatcher::parseCsvLine(const std::string& line) {
    std::vector<std::string> result;
    std::string cell;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                cell += '"'; ++i; 
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            result.push_back(cell); cell.clear();
        } else {
            cell += c;
        }
    }
    result.push_back(cell);
    return result;
}

std::vector<std::string> CategoryMatcher::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        size_t first = token.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) continue;
        size_t last = token.find_last_not_of(" \t\r\n");
        tokens.push_back(token.substr(first, (last - first + 1)));
    }
    return tokens;
}

std::string CategoryMatcher::toLower(const std::string& s) const {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return result;
}

std::string CategoryMatcher::getCategoryName(int id) const {
    for (const auto& cat : categories_) {
        if (cat.id == id) return cat.name;
    }
    return "Unknown";
}

std::string CategoryMatcher::getCategoryParent(int id) const {
    for (const auto& cat : categories_) {
        if (cat.id == id) return cat.parent;
    }
    return "";
}
std::string CategoryMatcher::getCategorySubcategory(int id) const {
    for (const auto& cat : categories_) {
        if (cat.id == id) return cat.subcategory;
    }
    return ""; // Возвращаем пустую строку, если ID не найден
}
#include "../include/CategoryMatcher.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

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

        // ИСПОЛЬЗУЕМ УМНЫЙ ПАРСЕР
        std::vector<std::string> fields = parseCsvLine(line);

        // Ожидаем минимум 5 полей: id, parent, sub, name, keywords
        if (fields.size() < 5) {
            std::cerr << "[WARN] Строка " << lineNumber << " пропущена (неверный формат): " << line << std::endl;
            continue;
        }

        try {
            Category cat;
            cat.id = std::stoi(fields[0]); // ID
            cat.parent = fields[1];        // Parent
            cat.subcategory = fields[2];   // Subcategory
            cat.name = fields[3];          // Name
            
            // Обработка ключевых слов
            // 1. Убираем кавычки, если они есть вокруг списка
            std::string rawKeywords = fields[4]; 
            
            // 2. Разбиваем по точке с запятой
            std::vector<std::string> tokens = split(rawKeywords, ';');
            
            // 3. Сохраняем сразу в нижнем регистре, чтобы не делать это при каждом поиске
            for (const auto& token : tokens) {
                if (!token.empty()) {
                    cat.keywords.push_back(toLower(token));
                }
            }

            categories_.push_back(cat);

        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Ошибка конвертации данных в строке " << lineNumber << ": " << e.what() << std::endl;
        }
    }

    std::cout << "✅ Успешно загружено " << categories_.size() << " категорий." << std::endl;
}

int CategoryMatcher::match(const std::string& purpose) const {
    // Преобразуем входящую строку в нижний регистр один раз
    std::string lowerPurpose = toLower(purpose);

    for (const auto& cat : categories_) {
        for (const auto& kw : cat.keywords) {
            // Ищем подстроку. Ключевые слова в cat.keywords УЖЕ в нижнем регистре.
            if (lowerPurpose.find(kw) != std::string::npos) {
                // [DEBUG MATCH] - раскомментируй для отладки, если нужно
                // std::cout << "Found: " << kw << " -> ID " << cat.id << std::endl;
                return cat.id;
            }
        }
    }
    return 0; // Не найдено
}

// --- Вспомогательные функции ---

// Главное улучшение: парсер, который не ломается об запятые внутри кавычек
std::vector<std::string> CategoryMatcher::parseCsvLine(const std::string& line) {
    std::vector<std::string> result;
    std::string cell;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];

        if (c == '"') {
            // Если кавычка внутри кавычек ("" -> "), обрабатываем, иначе переключаем режим
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                cell += '"';
                ++i; // пропускаем вторую кавычку
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            // Запятая вне кавычек — это разделитель полей
            result.push_back(cell);
            cell.clear();
        } else {
            cell += c;
        }
    }
    result.push_back(cell); // добавляем последнее поле
    return result;
}

std::vector<std::string> CategoryMatcher::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        // Простая очистка от пробелов по краям
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
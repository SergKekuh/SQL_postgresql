#ifndef CATEGORY_MATCHER_H
#define CATEGORY_MATCHER_H

#include <string>
#include <vector>
#include <iostream>

// Структура для хранения одной категории
struct Category {
    int id;
    std::string parent;
    std::string subcategory;
    std::string name;
    std::vector<std::string> keywords; // Уже в нижнем регистре
};

class CategoryMatcher {
public:
    // Конструктор загружает CSV и парсит его
    explicit CategoryMatcher(const std::string& csvPath);

    // Метод поиска категории по тексту назначения платежа
    int match(const std::string& purpose) const;

    // Вспомогательные методы
    std::string getCategoryName(int id) const;
    std::string getCategoryParent(int id) const;
    std::string getCategorySubcategory(int id) const;

private:
    std::vector<Category> categories_;

    // Умный парсер строки CSV (учитывает кавычки)
    std::vector<std::string> parseCsvLine(const std::string& line);
    
    // Разбиение строки на токены
    std::vector<std::string> split(const std::string& s, char delimiter);
    
    // Приведение к нижнему регистру (для UTF-8 корректно работать сложно без библиотек, 
    // но этот метод работает байтово, что обычно ок для поиска совпадений)
    std::string toLower(const std::string& s) const;
};

#endif // CATEGORY_MATCHER_H
#ifndef PAYMENTCLASSIFIER_H
#define PAYMENTCLASSIFIER_H

#include "ICategoryClassifier.h"
#include <memory>
#include <vector>
#include <string>
#include <map>

// Структура для хранения информации о категории
struct CategoryInfo {
    std::string parent;
    std::string subcategory;
    std::string name;
};

class PaymentClassifier {
private:
    std::vector<std::unique_ptr<ICategoryClassifier>> classifiers_;
    std::map<int, CategoryInfo> categoryMap; // id -> информация о категории

    void loadCategoriesFromCSV(const std::string& csvPath);

public:
    PaymentClassifier(const std::string& csvPath = "categories.csv");
    int classify(const std::string& purpose) const;

    // Методы для получения информации о категории по ID
    std::string getCategoryParent(int id) const;
    std::string getCategorySubcategory(int id) const;
    std::string getCategoryName(int id) const;
    CategoryInfo getCategoryInfo(int id) const;
};

#endif
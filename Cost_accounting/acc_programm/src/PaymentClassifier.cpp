// src/PaymentClassifier.cpp
#include "../include/PaymentClassifier.h"
#include "../include/CategoryClassifier.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm> 

PaymentClassifier::PaymentClassifier(const std::string& csvPath) {
    loadClassifiersFromCSV(csvPath);
}

void PaymentClassifier::loadClassifiersFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл категорий: " << filename << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line); // пропускаем заголовок

    while (std::getline(file, line)) {
        std::replace(line.begin(), line.end(), '"', ' '); // убираем кавычки
        std::stringstream ss(line);
        std::string id_str, parent, subcat, name, keywords_str;

        if (std::getline(ss, id_str, ',') &&
            std::getline(ss, parent, ',') &&
            std::getline(ss, subcat, ',') &&
            std::getline(ss, name, ',') &&
            std::getline(ss, keywords_str, '\n')) {

            int id = std::stoi(id_str);
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
            classifiers_.push_back(std::make_unique<CategoryClassifier>(id, keywords));
        }
    }
    file.close();
}

int PaymentClassifier::classify(const std::string& purpose) const {
    for (const auto& classifier : classifiers_) {
        if (classifier->matches(purpose)) {
            return classifier->getId();
        }
    }
    return 0; // не распознано
}
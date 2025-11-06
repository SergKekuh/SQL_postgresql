// include/PaymentClassifier.h
#ifndef PAYMENTCLASSIFIER_H
#define PAYMENTCLASSIFIER_H

#include "ICategoryClassifier.h"
#include <memory>
#include <vector>
#include <string>

class PaymentClassifier {
private:
    std::vector<std::unique_ptr<ICategoryClassifier>> classifiers_;
    void loadClassifiersFromCSV(const std::string& filename);

public:
    PaymentClassifier(const std::string& csvPath = "categories.csv");
    int classify(const std::string& purpose) const;
};

#endif
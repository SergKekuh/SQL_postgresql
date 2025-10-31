#ifndef PAYMENTCLASSIFIER_H
#define PAYMENTCLASSIFIER_H

#include "ICategoryClassifier.h"
#include <memory>
#include <vector>

class PaymentClassifier {
private:
    std::vector<std::unique_ptr<ICategoryClassifier>> classifiers_;

    void buildClassifiers();

public:
    PaymentClassifier();
    int classify(const std::string& purpose) const;
};

#endif // PAYMENTCLASSIFIER_H
#ifndef CATEGORYCLASSIFIER_H
#define CATEGORYCLASSIFIER_H

#include "ICategoryClassifier.h"
#include <vector>
#include <string>

class CategoryClassifier : public ICategoryClassifier {
private:
    int id_;
    std::vector<std::string> keywords_;

    std::string toLower(const std::string& s) const;

public:
    CategoryClassifier(int id, const std::vector<std::string>& keywords);
    int getId() const override;
    bool matches(const std::string& purpose) const override;
};

#endif // CATEGORYCLASSIFIER_H
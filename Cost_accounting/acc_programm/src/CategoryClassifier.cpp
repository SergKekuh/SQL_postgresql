#include "../include/CategoryClassifier.h"
#include <algorithm>
#include <cctype>

std::string CategoryClassifier::toLower(const std::string& s) const {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return result;
}

CategoryClassifier::CategoryClassifier(int id, const std::vector<std::string>& keywords)
    : id_(id), keywords_(keywords) {}

int CategoryClassifier::getId() const {
    return id_;
}

bool CategoryClassifier::matches(const std::string& purpose) const {
    std::string lowerPurpose = toLower(purpose);
    for (const auto& kw : keywords_) {
        if (lowerPurpose.find(toLower(kw)) != std::string::npos) {
            return true;
        }
    }
    return false;
}
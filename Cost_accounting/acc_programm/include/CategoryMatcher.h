#ifndef CATEGORYMATCHER_H
#define CATEGORYMATCHER_H

#include <vector>
#include <string>
#include <memory>

struct Category {
    int id;
    std::string parent;
    std::string subcategory;
    std::string name;
    std::vector<std::string> keywords;
};

class CategoryMatcher {
private:
    std::vector<Category> categories_;

public:
    CategoryMatcher(const std::string& csvPath);
    int match(const std::string& purpose) const;
    std::string getCategoryName(int id) const;
    std::string getCategoryParent(int id) const;
};

#endif
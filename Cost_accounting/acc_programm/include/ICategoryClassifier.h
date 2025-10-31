#ifndef ICATEGORYCLASSIFIER_H
#define ICATEGORYCLASSIFIER_H

#include <string>

class ICategoryClassifier {
public:
    virtual ~ICategoryClassifier() = default;
    virtual int getId() const = 0;
    virtual bool matches(const std::string& purpose) const = 0;
};

#endif // ICATEGORYCLASSIFIER_H
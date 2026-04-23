#ifndef RECOMMENDATION_HPP
#define RECOMMENDATION_HPP

#include <string>
#include <pqxx/pqxx>

namespace utsk {

class Recommendation {
public:
    Recommendation() = default;

    const std::string& getClientCode() const { return m_clientCode; }
    void setClientCode(const std::string& code) { m_clientCode = code; }

    const std::string& getProductCode() const { return m_productCode; }
    void setProductCode(const std::string& code) { m_productCode = code; }

    const std::string& getProductName() const { return m_productName; }
    void setProductName(const std::string& name) { m_productName = name; }

    const std::string& getReason() const { return m_reason; }
    void setReason(const std::string& reason) { m_reason = reason; }

    int getPriority() const { return m_priority; }
    void setPriority(int priority) { m_priority = priority; }

    double getInStockBalance() const { return m_inStockBalance; }
    void setInStockBalance(double bal) { m_inStockBalance = bal; }

    static Recommendation fromRow(const pqxx::row& row);

private:
    std::string m_clientCode;
    std::string m_productCode;
    std::string m_productName;
    std::string m_reason;
    int m_priority = 0;
    double m_inStockBalance = 0.0;
};

} // namespace utsk

#endif // RECOMMENDATION_HPP

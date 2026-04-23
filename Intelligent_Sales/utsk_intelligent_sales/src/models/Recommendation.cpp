#include "models/Recommendation.hpp"

namespace utsk {

Recommendation Recommendation::fromRow(const pqxx::row& row) {
    Recommendation rec;
    
    rec.setClientCode(row["client_code"].as<std::string>());
    rec.setProductCode(row["product_code"].as<std::string>());
    rec.setProductName(row["product_name"].as<std::string>());
    rec.setReason(row["recommendation_reason"].as<std::string>());
    rec.setPriority(row["priority"].as<int>());
    
    try { if(!row["in_stock_balance"].is_null()) rec.setInStockBalance(row["in_stock_balance"].as<double>()); } catch(...) {}
    
    return rec;
}

} // namespace utsk

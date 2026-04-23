#include "services/RecommendationService.hpp"
#include "core/Logger.hpp"

namespace utsk {
RecommendationService::RecommendationService(Database& db) : m_db(db) {}

std::vector<Recommendation> RecommendationService::getForClient(const std::string& clientCode) {
    std::vector<Recommendation> recs;
    try {
        auto result = m_db.executeParams("SELECT * FROM v_smart_recommendations WHERE client_code = $1 ORDER BY priority DESC", {clientCode});
        for (const auto& row : result) recs.push_back(Recommendation::fromRow(row));
    } catch (const std::exception& e) { LOG_ERROR(e.what()); }
    return recs;
}

std::vector<Recommendation> RecommendationService::getTopRecommendations(int limit) {
    std::vector<Recommendation> recs;
    try {
        auto result = m_db.executeParams("SELECT * FROM v_smart_recommendations ORDER BY priority DESC LIMIT $1", {std::to_string(limit)});
        for (const auto& row : result) recs.push_back(Recommendation::fromRow(row));
    } catch (const std::exception& e) { LOG_ERROR(e.what()); }
    return recs;
}
}

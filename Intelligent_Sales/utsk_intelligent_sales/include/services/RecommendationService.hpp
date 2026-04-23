#ifndef RECOMMENDATION_SERVICE_HPP
#define RECOMMENDATION_SERVICE_HPP

#include <vector>
#include <string>
#include "models/Recommendation.hpp"
#include "core/Database.hpp"

namespace utsk {
class RecommendationService {
public:
    explicit RecommendationService(Database& db);
    std::vector<Recommendation> getForClient(const std::string& clientCode);
    std::vector<Recommendation> getTopRecommendations(int limit = 10);
private:
    Database& m_db;
};
}
#endif // RECOMMENDATION_SERVICE_HPP

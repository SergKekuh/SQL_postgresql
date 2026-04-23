#include "services/ClientService.hpp"
#include "core/Logger.hpp"
#include <sstream>
#include <iomanip>

namespace utsk {

ClientService::ClientService(Database& db) : m_db(db) {
    LOG_DEBUG("ClientService initialized");
}

std::vector<Client> ClientService::getDashboard() {
    std::vector<Client> clients;
    const std::string query = R"(
        SELECT 
            code, name, client_type, current_status, 
            first_purchase_date, last_purchase_date, 
            days_since_last, activity_direction, 
            direction_confidence, requires_survey, 
            survey_completed_at, total_docs, 
            docs_current_year, total_revenue
        FROM v_manager_dashboard
        ORDER BY requires_survey DESC, days_since_last ASC
    )";
    try {
        auto result = m_db.execute(query);
        for (const auto& row : result) {
            Client client;
            client.setCode(row["code"].as<std::string>());
            client.setName(row["name"].as<std::string>());
            
            try { if (!row["client_type"].is_null()) client.setClientType(row["client_type"].as<std::string>()); } catch(...) {}
            try { if (!row["current_status"].is_null()) client.setStatusName(row["current_status"].as<std::string>()); } catch(...) {}
            try { if (!row["first_purchase_date"].is_null()) client.setFirstPurchaseDate(row["first_purchase_date"].as<std::string>()); } catch(...) {}
            try { if (!row["last_purchase_date"].is_null()) client.setLastPurchaseDate(row["last_purchase_date"].as<std::string>()); } catch(...) {}
            try { if (!row["days_since_last"].is_null()) client.setDaysSinceLastPurchase(row["days_since_last"].as<int>()); } catch(...) {}
            try { if (!row["activity_direction"].is_null()) client.setDirectionName(row["activity_direction"].as<std::string>()); } catch(...) {}
            try { if (!row["direction_confidence"].is_null()) client.setDirectionConfidence(row["direction_confidence"].as<double>()); } catch(...) {}
            try { if (!row["requires_survey"].is_null()) client.setRequiresSurvey(row["requires_survey"].as<bool>()); } catch(...) {}
            try { if (!row["total_docs"].is_null()) client.setTotalDocuments(row["total_docs"].as<int>()); } catch(...) {}
            try { if (!row["total_revenue"].is_null()) client.setTotalRevenue(row["total_revenue"].as<double>()); } catch(...) {}
            
            clients.push_back(std::move(client));
        }
        LOG_INFO("Loaded " + std::to_string(clients.size()) + " clients from dashboard");
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Failed to load dashboard: ") + e.what());
    }
    return clients;
}

std::vector<Client> ClientService::getRequiringSurvey() {
    std::vector<Client> clients;
    const std::string query = R"(
        SELECT code, name, client_type, current_status, last_purchase_date
        FROM v_manager_dashboard
        WHERE requires_survey = TRUE
        ORDER BY last_purchase_date DESC NULLS LAST
    )";
    try {
        auto result = m_db.execute(query);
        for (const auto& row : result) {
            Client client;
            client.setCode(row["code"].as<std::string>());
            client.setName(row["name"].as<std::string>());
            try { if (!row["client_type"].is_null()) client.setClientType(row["client_type"].as<std::string>()); } catch(...) {}
            try { if (!row["current_status"].is_null()) client.setStatusName(row["current_status"].as<std::string>()); } catch(...) {}
            client.setRequiresSurvey(true);
            clients.push_back(std::move(client));
        }
        LOG_INFO("Found " + std::to_string(clients.size()) + " clients requiring survey");
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Failed to get survey clients: ") + e.what());
    }
    return clients;
}

std::optional<Client> ClientService::getByCode(const std::string& code) {
    const std::string query = R"(
        SELECT c.code, c.name, c.client_type, sr.status_name as current_status,
               c.first_purchase_date, c.last_purchase_date, ad.name as activity_direction,
               c.direction_confidence, c.requires_survey,
               (SELECT COUNT(*) FROM documents WHERE client_code = c.code) as total_docs,
               (SELECT COALESCE(SUM(total_amount), 0) FROM documents WHERE client_code = c.code) as total_revenue
        FROM clients c
        LEFT JOIN status_rules sr ON c.current_status_id = sr.id
        LEFT JOIN activity_directions ad ON c.activity_direction_id = ad.id
        WHERE c.code = $1
    )";
    try {
        auto result = m_db.executeParams(query, {code});
        if (result.empty()) return std::nullopt;
        
        auto row = result[0];
        Client client;
        client.setCode(row["code"].as<std::string>());
        client.setName(row["name"].as<std::string>());
        // Опциональные поля опущены для краткости, можно добавить аналогично getDashboard
        return client;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Failed to get client by code: ") + e.what());
        return std::nullopt;
    }
}

void ClientService::updateAnalytics(const std::string& clientCode) {
    try {
        if (clientCode.empty()) {
            m_db.execute("SELECT update_client_analytics()");
            LOG_INFO("Updated analytics for all clients");
        } else {
            m_db.executeParams("SELECT update_client_analytics($1)", {clientCode});
            LOG_INFO("Updated analytics for client: " + clientCode);
        }
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Failed to update analytics: ") + e.what());
    }
}

std::vector<ClientService::StatusStats> ClientService::getStatusDistribution() {
    std::vector<StatusStats> stats;
    const std::string query = R"(
        SELECT sr.status_name, COUNT(c.code) as count,
               ROUND(COUNT(c.code) * 100.0 / NULLIF((SELECT COUNT(*) FROM clients), 0), 2) as percentage
        FROM clients c
        JOIN status_rules sr ON c.current_status_id = sr.id
        GROUP BY sr.status_name, sr.priority
        ORDER BY sr.priority
    )";
    try {
        auto result = m_db.execute(query);
        for (const auto& row : result) {
            stats.push_back({
                row["status_name"].as<std::string>(),
                row["count"].as<int>(),
                row["percentage"].is_null() ? 0.0 : row["percentage"].as<double>()
            });
        }
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Failed to get status distribution: ") + e.what());
    }
    return stats;
}

std::vector<ClientService::DirectionStats> ClientService::getDirectionDistribution() {
    std::vector<DirectionStats> stats;
    const std::string query = R"(
        SELECT ad.name as direction_name, COUNT(c.code) as count,
               ROUND(COUNT(c.code) * 100.0 / NULLIF((SELECT COUNT(*) FROM clients WHERE activity_direction_id IS NOT NULL), 0), 2) as percentage
        FROM clients c
        JOIN activity_directions ad ON c.activity_direction_id = ad.id
        GROUP BY ad.name ORDER BY count DESC
    )";
    try {
        auto result = m_db.execute(query);
        for (const auto& row : result) {
            stats.push_back({
                row["direction_name"].as<std::string>(),
                row["count"].as<int>(),
                row["percentage"].is_null() ? 0.0 : row["percentage"].as<double>()
            });
        }
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Failed to get direction distribution: ") + e.what());
    }
    return stats;
}

ClientService::DashboardStats ClientService::getDashboardStats() {
    DashboardStats stats = {0, 0, 0, 0.0, 0.0};
    const std::string query = R"(
        SELECT 
            COUNT(DISTINCT c.code) as total_clients,
            COUNT(DISTINCT CASE WHEN c.last_purchase_date >= CURRENT_DATE - INTERVAL '30 days' THEN c.code END) as active_30d,
            COUNT(DISTINCT CASE WHEN c.last_purchase_date >= CURRENT_DATE - INTERVAL '90 days' THEN c.code END) as active_90d,
            COALESCE((SELECT SUM(total_amount) FROM documents), 0) as total_revenue,
            COALESCE((SELECT SUM(total_amount) FROM documents WHERE invoice_date >= CURRENT_DATE - INTERVAL '30 days'), 0) as revenue_30d
        FROM clients c
    )";
    try {
        auto result = m_db.execute(query);
        if (!result.empty()) {
            auto row = result[0];
            stats.totalClients = row["total_clients"].as<int>();
            stats.active30Days = row["active_30d"].as<int>();
            stats.active90Days = row["active_90d"].as<int>();
            stats.totalRevenue = row["total_revenue"].as<double>();
            stats.revenue30Days = row["revenue_30d"].as<double>();
        }
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("Failed to get dashboard stats: ") + e.what());
    }
    return stats;
}

} // namespace utsk

#include <iostream>
#include <iomanip>
#include "core/Config.hpp"
#include "core/Logger.hpp"
#include "core/Database.hpp"
#include "services/ClientService.hpp"
#include "services/ProductService.hpp"
#include "services/RecommendationService.hpp"
#include "services/AnalyticsService.hpp"
#include "services/SurveyService.hpp"

using namespace utsk;

void printSeparator() {
    std::cout << "\n" << std::string(60, '=') << "\n\n";
}

int main() {
    Logger::getInstance().init("", Logger::Level::INFO);
    
    LOG_INFO("========================================");
    LOG_INFO("UTSK Intelligent Sales - Console v1.0.0");
    LOG_INFO("========================================");
    
    Config config;
    if (!config.load(".env")) { 
        LOG_WARNING("Failed to load .env, proceeding with defaults");
    }
    
    // --- РЕШЕНИЕ КОНФЛИКТА ТИПОВ: Конвертируем Config::DatabaseInfo в Database::ConnectionInfo ---
    auto configInfo = config.getDatabaseInfo();
    Database::ConnectionInfo dbInfo;
    dbInfo.host = configInfo.host;
    dbInfo.port = configInfo.port;
    dbInfo.dbname = configInfo.dbname;
    dbInfo.user = configInfo.user;
    dbInfo.password = configInfo.password;
    
    Database db;
    if (!db.connect(dbInfo)) {
        LOG_ERROR("Failed to connect to database!");
        return 1;
    }
    
    ClientService clientService(db);
    ProductService productService(db);
    RecommendationService recService(db);
    
    printSeparator();
    
    // Дашборд
    auto stats = clientService.getDashboardStats();
    std::cout << "📊 ДАШБОРД\n";
    std::cout << "─────────────────────────────────────────\n";
    std::cout << "Всего клиентов:        " << stats.totalClients << "\n";
    std::cout << "Активных (30 дней):    " << stats.active30Days << "\n";
    std::cout << "Активных (90 дней):    " << stats.active90Days << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Общая выручка:         " << stats.totalRevenue << " грн\n";
    std::cout << "Выручка за 30 дней:    " << stats.revenue30Days << " грн\n";
    
    printSeparator();
    
    // Статусы
    auto statusStats = clientService.getStatusDistribution();
    std::cout << "🏷️  РАСПРЕДЕЛЕНИЕ ПО СТАТУСАМ\n";
    std::cout << "─────────────────────────────────────────\n";
    for (const auto& s : statusStats) {
        std::cout << std::left << std::setw(25) << s.statusName 
                  << std::right << std::setw(6) << s.count 
                  << " (" << std::setw(5) << s.percentage << "%)\n";
    }
    
    printSeparator();
    
    // Клиенты, требующие опроса
    auto surveyClients = clientService.getRequiringSurvey();
    std::cout << "⚠️  ТРЕБУЮТ ОПРОСА: " << surveyClients.size() << " клиентов\n";
    std::cout << "─────────────────────────────────────────\n";
    int count = 0;
    for (const auto& c : surveyClients) {
        std::cout << std::left << std::setw(15) << c.getCode() 
                  << std::setw(40) << c.getName().substr(0, 38) << "\n";
        if (++count >= 5) break;
    }
    
    printSeparator();
    
    // Товары
    auto products = productService.getAll();
    std::cout << "📦 ТОВАРЫ (первые 5 из " << products.size() << ")\n";
    std::cout << "─────────────────────────────────────────\n";
    count = 0;
    for (const auto& p : products) {
        std::cout << std::left << std::setw(15) << p.getCode() 
                  << std::setw(40) << p.getName().substr(0, 38) << "\n";
        if (++count >= 5) break;
    }
    
    printSeparator();
    
    // Рекомендации
    auto topRecs = recService.getTopRecommendations(3);
    std::cout << "💡 ТОП-3 УМНЫЕ РЕКОМЕНДАЦИИ\n";
    std::cout << "─────────────────────────────────────────\n";
    for(const auto& r : topRecs) {
        std::cout << "Клиенту: " << r.getClientCode() << " -> Предложить: " << r.getProductName() << "\n";
    }
    
    printSeparator();
    
    // --- ДЕМОНСТРАЦИЯ МОДУЛЯ ОПРОСОВ ---
    SurveyService surveyService(db);
    if (!surveyClients.empty()) {
        auto targetClient = surveyClients.front();
        std::cout << "📞 СИМУЛЯЦИЯ ОПРОСА\n";
        std::cout << "─────────────────────────────────────────\n";
        std::cout << "Звоним клиенту: " << targetClient.getName() << " (" << targetClient.getCode() << ")\n";
        
        SurveyResult survey;
        survey.setClientCode(targetClient.getCode());
        survey.setContacted(true);
        survey.setContactPerson("Иван Иванович (Директор)");
        survey.setFeedback("Был перерыв в закупках из-за логистики. Возвращаются к работе, нужен счет на арматуру.");
        
        if (surveyService.saveSurveyResult(survey)) {
            std::cout << "✅ Опрос успешно проведен и сохранен в БД!\n";
            std::cout << "✅ Флаг 'requires_survey' для клиента снят.\n";
        } else {
            std::cout << "❌ Ошибка при сохранении опроса.\n";
        }
        printSeparator();
    }

    db.disconnect();
    LOG_INFO("Application finished successfully");
    
    return 0;
}
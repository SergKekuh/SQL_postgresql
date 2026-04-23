#include <iostream>
#include <iomanip>
#include "core/Config.hpp"
#include "core/Logger.hpp"
#include "core/Database.hpp"
#include "services/ClientService.hpp"
#include "services/ProductService.hpp"

using namespace utsk;

void printSeparator() {
    std::cout << "\n" << std::string(60, '=') << "\n\n";
}

int main() {
    Logger::getInstance().init("", Logger::Level::INFO);
    
    LOG_INFO("UTSK Intelligent Sales - Console v1.0.0");
    
    // Загрузка конфигурации
    Config config;
    if (!config.load("config/db_config.json")) {
        std::cerr << "Failed to load config/db_config.json!" << std::endl;
        return 1;
    }
    
    // Подключение к БД
    Database db;
    if (!db.connect(config.getDatabaseInfo())) {
        std::cerr << "Failed to connect to database!" << std::endl;
        return 1;
    }
    
    ClientService clientService(db);
    ProductService productService(db);
    
    printSeparator();
    
    // Дашборд
    auto stats = clientService.getDashboardStats();
    std::cout << "📊 ДАШБОРД\n";
    std::cout << "─────────────────────────────────────────\n";
    std::cout << "Всего клиентов:        " << stats.totalClients << "\n";
    std::cout << "Активных (30 дней):    " << stats.active30Days << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Общая выручка:         " << stats.totalRevenue << " грн\n";
    
    printSeparator();
    
    // Статусы
    auto statusStats = clientService.getStatusDistribution();
    std::cout << "🏷️  РАСПРЕДЕЛЕНИЕ ПО СТАТУСАМ\n";
    std::cout << "─────────────────────────────────────────\n";
    for (const auto& s : statusStats) {
        std::cout << std::left << std::setw(20) << s.statusName 
                  << std::right << std::setw(6) << s.count 
                  << " (" << std::setw(6) << s.percentage << "%)\n";
    }
    
    printSeparator();
    
    // Направления
    auto dirStats = clientService.getDirectionDistribution();
    std::cout << "🎯 РАСПРЕДЕЛЕНИЕ ПО НАПРАВЛЕНИЯМ\n";
    std::cout << "─────────────────────────────────────────\n";
    for (const auto& d : dirStats) {
        std::cout << std::left << std::setw(30) << d.directionName 
                  << std::right << std::setw(6) << d.count 
                  << " (" << std::setw(6) << d.percentage << "%)\n";
    }
    
    printSeparator();
    
    // Клиенты на опрос
    auto surveyClients = clientService.getRequiringSurvey();
    std::cout << "⚠️  ТРЕБУЮТ ОПРОСА: " << surveyClients.size() << " клиентов\n";
    std::cout << "─────────────────────────────────────────\n";
    for (const auto& c : surveyClients) {
        std::cout << std::left << std::setw(12) << c.getCode() 
                  << std::setw(40) << c.getName().substr(0, 38) << "\n";
    }
    
    printSeparator();
    
    // Товары
    auto products = productService.getAll();
    std::cout << "📦 ТОВАРЫ (первые 10 из " << products.size() << ")\n";
    std::cout << "─────────────────────────────────────────\n";
    int count = 0;
    for (const auto& p : products) {
        std::cout << std::left << std::setw(12) << p.getCode() 
                  << std::setw(48) << p.getName().substr(0, 46) << "\n";
        if (++count >= 10) break;
    }
    
    printSeparator();
    
    db.disconnect();
    LOG_INFO("Application finished successfully");
    
    return 0;
}

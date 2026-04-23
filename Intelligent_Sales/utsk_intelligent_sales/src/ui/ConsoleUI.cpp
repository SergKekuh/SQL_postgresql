#include "ui/ConsoleUI.hpp"
#include "core/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace utsk {

ConsoleUI::ConsoleUI(Database& db, ClientService& clientSvc, ProductService& productSvc)
    : m_db(db)
    , m_clientService(clientSvc)
    , m_productService(productSvc) {
    
    m_menu.setTitle("UTSK INTELLIGENT SALES v1.0.0");
    
    m_menu.addItem("1", "📊 Дашборд", 
        [this]() { showDashboard(); });
    m_menu.addItem("2", "👥 Клиенты (требуют опроса)", 
        [this]() { showRequiringSurvey(); });
    m_menu.addItem("3", "🏷️  Статусы клиентов", 
        [this]() { showStatusDistribution(); });
    m_menu.addItem("4", "🎯 Направления деятельности", 
        [this]() { showDirectionDistribution(); });
    m_menu.addItem("5", "📦 Товары", 
        [this]() { showProducts(); });
    m_menu.addSeparator();
    m_menu.addItem("0", "🚪 Выход", 
        [this]() { m_menu.exit(); });
}

void ConsoleUI::run() {
    LOG_INFO("Starting console UI");
    m_menu.run();
}

void ConsoleUI::showDashboard() {
    auto stats = m_clientService.getDashboardStats();
    
    TablePrinter table;
    table.setTitle("📊 ДАШБОРД");
    table.addColumn("Показатель", 30);
    table.addColumn("Значение", 25, TablePrinter::Alignment::Right);
    table.addRow({"Всего клиентов", std::to_string(stats.totalClients)});
    table.addRow({"Активных (30 дней)", std::to_string(stats.active30Days)});
    table.addRow({"Активных (90 дней)", std::to_string(stats.active90Days)});
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << stats.totalRevenue << " грн";
    table.addRow({"Общая выручка", ss.str()});
    
    ss.str("");
    ss << std::fixed << std::setprecision(2) << stats.revenue30Days << " грн";
    table.addRow({"Выручка за 30 дней", ss.str()});
    
    table.print();
    waitForKey();
}

void ConsoleUI::showRequiringSurvey() {
    auto clients = m_clientService.getRequiringSurvey();
    
    TablePrinter table;
    table.setTitle("⚠️  КЛИЕНТЫ, ТРЕБУЮЩИЕ ОПРОСА (" + std::to_string(clients.size()) + ")");
    table.addColumn("Код", 12);
    table.addColumn("Название", 40);
    table.addColumn("Тип", 18);
    table.addColumn("Статус", 16);
    
    for (const auto& c : clients) {
        table.addRow({
            c.getCode(),
            c.getName().substr(0, 38),
            c.getClientType().value_or("-"),
            c.getStatusName().value_or("-")
        });
    }
    
    table.print();
    waitForKey();
}

void ConsoleUI::showStatusDistribution() {
    auto stats = m_clientService.getStatusDistribution();
    
    TablePrinter table;
    table.setTitle("🏷️  РАСПРЕДЕЛЕНИЕ ПО СТАТУСАМ");
    table.addColumn("Статус", 22);
    table.addColumn("Количество", 12, TablePrinter::Alignment::Right);
    table.addColumn("%", 10, TablePrinter::Alignment::Right);
    
    for (const auto& s : stats) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << s.percentage;
        table.addRow({s.statusName, std::to_string(s.count), ss.str() + "%"});
    }
    
    table.print();
    waitForKey();
}

void ConsoleUI::showDirectionDistribution() {
    auto stats = m_clientService.getDirectionDistribution();
    
    TablePrinter table;
    table.setTitle("🎯 РАСПРЕДЕЛЕНИЕ ПО НАПРАВЛЕНИЯМ");
    table.addColumn("Направление", 32);
    table.addColumn("Количество", 12, TablePrinter::Alignment::Right);
    table.addColumn("%", 10, TablePrinter::Alignment::Right);
    
    for (const auto& d : stats) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << d.percentage;
        table.addRow({d.directionName, std::to_string(d.count), ss.str() + "%"});
    }
    
    table.print();
    waitForKey();
}

void ConsoleUI::showProducts() {
    auto products = m_productService.getAll();
    
    TablePrinter table;
    table.setTitle("📦 ТОВАРЫ (первые 20)");
    table.addColumn("Код", 12);
    table.addColumn("Название", 45);
    table.addColumn("Направление", 22);
    
    int count = 0;
    for (const auto& p : products) {
        table.addRow({
            p.getCode(),
            p.getName().substr(0, 43),
            p.getDirectionName().value_or("-")
        });
        if (++count >= 20) break;
    }
    
    table.print();
    waitForKey();
}

void ConsoleUI::waitForKey() {
    std::cout << "\nНажмите Enter для продолжения...";
    std::cin.get();
}

} // namespace utsk

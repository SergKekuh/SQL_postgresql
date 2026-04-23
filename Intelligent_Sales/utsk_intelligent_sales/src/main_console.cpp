#include "core/Config.hpp"
#include "core/Logger.hpp"
#include "core/Database.hpp"
#include "services/ClientService.hpp"
#include "services/ProductService.hpp"
#include "ui/ConsoleUI.hpp"

using namespace utsk;

int main() {
    Logger::getInstance().init("", Logger::Level::INFO);
    
    LOG_INFO("UTSK Intelligent Sales - Console v1.0.0");
    
    // Загрузка конфигурации из JSON-файла
    Config config;
    if (!config.load("config/db_config.json")) {
        std::cerr << "Failed to load config/db_config.json!" << std::endl;
        std::cerr << "Create it from config/db_config.example.json" << std::endl;
        return 1;
    }
    
    // Подключение к БД
    Database db;
    if (!db.connect(config.getDatabaseInfo())) {
        std::cerr << "Failed to connect to database!" << std::endl;
        return 1;
    }
    
    // Сервисы
    ClientService clientService(db);
    ProductService productService(db);
    
    // Запуск UI
    ConsoleUI ui(db, clientService, productService);
    ui.run();
    
    db.disconnect();
    LOG_INFO("Application finished");
    
    return 0;
}

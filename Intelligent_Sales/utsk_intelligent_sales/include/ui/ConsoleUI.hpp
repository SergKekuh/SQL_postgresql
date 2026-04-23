#ifndef CONSOLE_UI_HPP
#define CONSOLE_UI_HPP

#include "core/Database.hpp"
#include "services/ClientService.hpp"
#include "services/ProductService.hpp"
#include "ui/ConsoleMenu.hpp"
#include "ui/TablePrinter.hpp"

namespace utsk {

/**
 * @brief Главный класс консольного интерфейса
 */
class ConsoleUI {
public:
    ConsoleUI(Database& db, ClientService& clientSvc, ProductService& productSvc);
    ~ConsoleUI() = default;

    void run();

private:
    void showDashboard();
    void showClients();
    void showProducts();
    void showStatusDistribution();
    void showDirectionDistribution();
    void showRequiringSurvey();
    void waitForKey();

    Database& m_db;
    ClientService& m_clientService;
    ProductService& m_productService;
    ConsoleMenu m_menu;
};

} // namespace utsk

#endif // CONSOLE_UI_HPP

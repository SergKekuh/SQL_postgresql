#ifndef CLIENT_STATISTICS_H
#define CLIENT_STATISTICS_H

#include <string>

class ClientStatistics {
public:
    std::string category;
    int companies;
    double total_invoices;
    double total_sales;
    double avg_sales;
    double percent_share;

    // Конструктор
    ClientStatistics(
        const std::string& category,
        int companies,
        double total_invoices,
        double total_sales,
        double avg_sales,
        double percent_share
    );
};

#endif // CLIENT_STATISTICS_H
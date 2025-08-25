#ifndef STATISTICS_SERVICE_H
#define STATISTICS_SERVICE_H

#include <vector>
#include "ClientStatistics.h"
#include "Database.h"

class StatisticsService {
private:
    Database& db;

public:
    explicit StatisticsService(Database& database);

    std::vector<ClientStatistics> getAllStatistics(int year);
    std::vector<ClientStatistics> getBelowStatistics(int year, double maxSales);
    std::vector<ClientStatistics> getHigherStatistics(int year, double minSales);
};

#endif // STATISTICS_SERVICE_H
#include "StatisticsService.h"
#include <libpq-fe.h>
#include <iostream>
#include <vector>

StatisticsService::StatisticsService(Database& database) : db(database) {}

std::vector<ClientStatistics> StatisticsService::getAllStatistics(int year) {
    PGconn* conn = db.getConnection();
    std::string query = "SELECT * FROM calculate_client_statistics_all(" + std::to_string(year) + ");";

    PGresult* res = PQexec(conn, query.c_str());

    std::vector<ClientStatistics> result;

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Query failed: " << PQresultErrorMessage(res) << std::endl;
        PQclear(res);
        return result;
    }

    int rows = PQntuples(res);

    for (int i = 0; i < rows; ++i) {
        std::string category = PQgetvalue(res, i, 0);
        int companies = std::stoi(PQgetvalue(res, i, 1));
        double total_invoices = std::stod(PQgetvalue(res, i, 2));
        double total_sales = std::stod(PQgetvalue(res, i, 3));
        double avg_sales = std::stod(PQgetvalue(res, i, 4));
        double percent_share = std::stod(PQgetvalue(res, i, 5));

        result.emplace_back(category, companies, total_invoices, total_sales, avg_sales, percent_share);
    }

    PQclear(res);
    return result;
}

std::vector<ClientStatistics> StatisticsService::getBelowStatistics(int year, double maxSales) {
    PGconn* conn = db.getConnection();
    std::string query = "SELECT * FROM calculate_client_statistics_below(" +
                        std::to_string(year) + ", " +
                        std::to_string(maxSales) + ");";

    PGresult* res = PQexec(conn, query.c_str());

    std::vector<ClientStatistics> result;

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Query failed: " << PQresultErrorMessage(res) << std::endl;
        PQclear(res);
        return result;
    }

    int rows = PQntuples(res);

    for (int i = 0; i < rows; ++i) {
        std::string category = PQgetvalue(res, i, 0);
        int companies = std::stoi(PQgetvalue(res, i, 1));
        double total_invoices = std::stod(PQgetvalue(res, i, 2));
        double total_sales = std::stod(PQgetvalue(res, i, 3));
        double avg_sales = std::stod(PQgetvalue(res, i, 4));
        double percent_share = std::stod(PQgetvalue(res, i, 5));

        result.emplace_back(category, companies, total_invoices, total_sales, avg_sales, percent_share);
    }

    PQclear(res);
    return result;
}

std::vector<ClientStatistics> StatisticsService::getHigherStatistics(int year, double minSales) {
    PGconn* conn = db.getConnection();
    std::string query = "SELECT * FROM calculate_client_statistics_higher(" +
                        std::to_string(year) + ", " +
                        std::to_string(minSales) + ");";

    PGresult* res = PQexec(conn, query.c_str());

    std::vector<ClientStatistics> result;

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Query failed: " << PQresultErrorMessage(res) << std::endl;
        PQclear(res);
        return result;
    }

    int rows = PQntuples(res);

    for (int i = 0; i < rows; ++i) {
        std::string category = PQgetvalue(res, i, 0);
        int companies = std::stoi(PQgetvalue(res, i, 1));
        double total_invoices = std::stod(PQgetvalue(res, i, 2));
        double total_sales = std::stod(PQgetvalue(res, i, 3));
        double avg_sales = std::stod(PQgetvalue(res, i, 4));
        double percent_share = std::stod(PQgetvalue(res, i, 5));

        result.emplace_back(category, companies, total_invoices, total_sales, avg_sales, percent_share);
    }

    PQclear(res);
    return result;
}
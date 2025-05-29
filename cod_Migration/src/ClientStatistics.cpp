#include "ClientStatistics.h"

ClientStatistics::ClientStatistics(
    const std::string& category,
    int companies,
    double total_invoices,
    double total_sales,
    double avg_sales,
    double percent_share
)
    : category(category),
      companies(companies),
      total_invoices(total_invoices),
      total_sales(total_sales),
      avg_sales(avg_sales),
      percent_share(percent_share) {}
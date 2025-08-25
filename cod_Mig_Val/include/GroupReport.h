#ifndef GROUPREPORT_H
#define GROUPREPORT_H

#include <string>

// Структура для хранения данных отчета по группам
struct GroupReport {
    std::string group_name;      // Название группы
    double total_sale;          // Общий объем продаж
    int total_companies;        // Количество компаний
};

#endif // GROUPREPORT_H
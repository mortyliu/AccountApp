#include "StatisticsController.h"
#include "../model/DatabaseManager.h"

StatisticsController::StatisticsController(QObject* parent) : QObject(parent) {}

QMap<QString, double> StatisticsController::getCategoryStatistics(const QDate& start, const QDate& end, int type) {
    QMap<QString, double> result;
    QSqlQuery query = DatabaseManager::instance().getCategoryStatistics(start, end, type);
    
    while (query.next()) {
        QString name = query.value("name").toString();
        double total = query.value("total").toDouble();
        result[name] = total;
    }
    
    return result;
}

QMap<int, double> StatisticsController::getMonthlyStatistics(int year, int type) {
    QMap<int, double> result;
    QSqlQuery query = DatabaseManager::instance().getMonthlyStatistics(year, type);
    
    while (query.next()) {
        int month = query.value("month").toInt();
        double total = query.value("total").toDouble();
        result[month] = total;
    }
    
    return result;
}

double StatisticsController::getTotalAmount(const QDate& start, const QDate& end, int type) {
    QMap<QString, double> stats = getCategoryStatistics(start, end, type);
    double total = 0.0;
    for (double val : stats.values()) {
        total += val;
    }
    return total;
}

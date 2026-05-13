#ifndef STATISTICSCONTROLLER_H
#define STATISTICSCONTROLLER_H

#include <QObject>
#include <QDate>
#include <QMap>

class StatisticsController : public QObject {
    Q_OBJECT
public:
    explicit StatisticsController(QObject* parent = nullptr);

    QMap<QString, double> getCategoryStatistics(const QDate& start, const QDate& end, int type);
    QMap<int, double> getMonthlyStatistics(int year, int type);
    double getTotalAmount(const QDate& start, const QDate& end, int type);

private:
};

#endif // STATISTICSCONTROLLER_H

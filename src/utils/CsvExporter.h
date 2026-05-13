#ifndef CSVEXPORTER_H
#define CSVEXPORTER_H

#include <QString>
#include <QDate>

class CsvExporter {
public:
    static bool exportTransactions(const QString& filePath, const QDate& start = QDate(), const QDate& end = QDate());
    static bool importTransactions(const QString& filePath);
};

#endif // CSVEXPORTER_H

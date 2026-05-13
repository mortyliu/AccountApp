#include "CsvExporter.h"
#include "../model/DatabaseManager.h"
#include <QFile>
#include <QTextStream>
#include <QStringConverter>
#include <QSqlQuery>
#include <QDate>

bool CsvExporter::exportTransactions(const QString& filePath, const QDate& start, const QDate& end) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << QString::fromUtf8("日期,分类,账户,类型,金额,备注\n");

    QSqlQuery query;
    if (start.isValid() && end.isValid()) {
        query = DatabaseManager::instance().getTransactionsByDateRange(start, end);
    } else {
        query = DatabaseManager::instance().getAllTransactions();
    }

    while (query.next()) {
        QString date = query.value("date").toString();
        QString categoryName = query.value("category_name").toString();
        QString accountName = query.value("account_name").toString();
        QString type = query.value("category_type").toInt() == 1 ? QString::fromUtf8("收入") : QString::fromUtf8("支出");
        QString amount = QString::number(query.value("amount").toDouble());
        QString note = query.value("note").toString();

        out << date << ","
            << "\"" << categoryName << "\","
            << "\"" << accountName << "\","
            << type << ","
            << amount << ","
            << "\"" << note << "\"\n";
    }

    file.close();
    return true;
}

bool CsvExporter::importTransactions(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    QString header = in.readLine();

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");
        
        if (fields.size() < 5) continue;

        QDate date = QDate::fromString(fields[0].trimmed(), Qt::ISODate);
        QString categoryName = fields[1].trimmed().remove("\"");
        QString accountName = fields[2].trimmed().remove("\"");
        QString typeStr = fields[3].trimmed();
        double amount = fields[4].trimmed().toDouble();
        QString note = fields.size() > 5 ? fields[5].trimmed().remove("\"") : "";

        int categoryId = -1;
        QSqlQuery catQuery = DatabaseManager::instance().executeSelectQuery(
            QString("SELECT id FROM categories WHERE name = '%1'").arg(categoryName));
        if (catQuery.next()) {
            categoryId = catQuery.value("id").toInt();
        }

        int accountId = -1;
        QSqlQuery accQuery = DatabaseManager::instance().executeSelectQuery(
            QString("SELECT id FROM accounts WHERE name = '%1'").arg(accountName));
        if (accQuery.next()) {
            accountId = accQuery.value("id").toInt();
        }

        if (categoryId >= 0 && accountId >= 0) {
            DatabaseManager::instance().insertTransaction(categoryId, accountId, amount, date, note);
        }
    }

    file.close();
    return true;
}

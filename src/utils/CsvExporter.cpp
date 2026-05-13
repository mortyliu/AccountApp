#include "CsvExporter.h"
#include "../model/DatabaseManager.h"
#include <QFile>
#include <QTextStream>
#include <QStringConverter>
#include <QSqlQuery>
#include <QDate>
#include <QDebug>

static int getOrCreateCategory(const QString& categoryName, int type) {
    QSqlQuery query = DatabaseManager::instance().executeSelectQuery(
        QString("SELECT id FROM categories WHERE name = '%1' AND type = %2").arg(categoryName, QString::number(type)));
    if (query.next()) {
        return query.value("id").toInt();
    }
    DatabaseManager::instance().insertCategory(categoryName, type);
    query = DatabaseManager::instance().executeSelectQuery(
        QString("SELECT id FROM categories WHERE name = '%1' AND type = %2").arg(categoryName, QString::number(type)));
    if (query.next()) {
        return query.value("id").toInt();
    }
    return -1;
}

static int getOrCreateAccount(const QString& accountName) {
    if (accountName.isEmpty()) return -1;
    QSqlQuery query = DatabaseManager::instance().executeSelectQuery(
        QString("SELECT id FROM accounts WHERE name = '%1'").arg(accountName));
    if (query.next()) {
        return query.value("id").toInt();
    }
    DatabaseManager::instance().insertAccount(accountName, "");
    query = DatabaseManager::instance().executeSelectQuery(
        QString("SELECT id FROM accounts WHERE name = '%1'").arg(accountName));
    if (query.next()) {
        return query.value("id").toInt();
    }
    return -1;
}

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
        qDebug() << "Cannot open file:" << filePath;
        return false;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    QString header = in.readLine();

    QStringList headers = header.split(",");
    QMap<QString, int> headerMap;
    for (int i = 0; i < headers.size(); ++i) {
        headerMap[headers[i].trimmed()] = i;
    }

    int importedCount = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().isEmpty()) continue;

        QStringList fields = line.split(",");
        if (fields.size() < 8) continue;

        QString id = fields[headerMap.value("ID", 0)].trimmed();
        QString time = fields[headerMap.value("时间", 1)].trimmed();
        QString categoryName = fields[headerMap.value("分类", 2)].trimmed();
        QString subCategory = fields[headerMap.value("二级分类", 3)].trimmed();
        QString typeStr = fields[headerMap.value("类型", 4)].trimmed();
        QString amountStr = fields[headerMap.value("金额", 5)].trimmed();
        QString currency = fields[headerMap.value("币种", 6)].trimmed();
        QString account1 = fields[headerMap.value("账户1", 7)].trimmed();
        QString account2 = fields[headerMap.value("账户2", 8)].trimmed();
        QString note = fields.size() > headerMap.value("备注", 9) ? fields[headerMap.value("备注", 9)].trimmed() : "";

        if (currency != "CNY") continue;

        QDateTime dateTime = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss");
        QDate date = dateTime.date();

        double amount = amountStr.toDouble();
        if (amount == 0) continue;

        int categoryType = 0;
        if (typeStr == QString::fromUtf8("收入") || typeStr == QString::fromUtf8("退款")) {
            categoryType = 1;
        } else if (typeStr == QString::fromUtf8("转账")) {
            int fromAccountId = getOrCreateAccount(account1);
            int toAccountId = getOrCreateAccount(account2);
            int transferCategoryId = getOrCreateCategory(QString::fromUtf8("转账"), 0);

            if (fromAccountId >= 0 && toAccountId >= 0) {
                DatabaseManager::instance().insertTransaction(
                    transferCategoryId, fromAccountId, qAbs(amount), date,
                    QString::fromUtf8("转出至%1").arg(account2));
                DatabaseManager::instance().insertTransaction(
                    transferCategoryId, toAccountId, qAbs(amount), date,
                    QString::fromUtf8("转入自%1").arg(account1));
                importedCount++;
            }
            continue;
        } else if (typeStr == QString::fromUtf8("还款")) {
            int accountId = getOrCreateAccount(account1);
            int categoryId = getOrCreateCategory(categoryName.isEmpty() ? QString::fromUtf8("还款") : categoryName, categoryType);
            if (categoryId >= 0 && accountId >= 0) {
                DatabaseManager::instance().insertTransaction(categoryId, accountId, qAbs(amount), date, note);
                importedCount++;
            }
            continue;
        }

        QString fullCategoryName = categoryName;
        if (!subCategory.isEmpty() && subCategory != categoryName) {
            fullCategoryName = QString::fromUtf8("%1-%2").arg(categoryName, subCategory);
        }

        int categoryId = getOrCreateCategory(fullCategoryName.isEmpty() ? QString::fromUtf8("其它") : fullCategoryName, categoryType);
        int accountId = getOrCreateAccount(account1);

        if (categoryId >= 0 && accountId >= 0) {
            DatabaseManager::instance().insertTransaction(categoryId, accountId, qAbs(amount), date, note);
            importedCount++;
        }
    }

    file.close();
    qDebug() << "Imported" << importedCount << "transactions";
    return importedCount > 0;
}

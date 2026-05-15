#include "CsvExporter.h"
#include "../model/DatabaseManager.h"
#include "../model/Constants.h"
#include <QFile>
#include <QTextStream>
#include <QStringConverter>
#include <QSqlQuery>
#include <QDate>
#include <QDebug>

static int getOrCreateCategory(const QString& categoryName, int type, int parentId = -1) {
    QSqlQuery query;
    if (parentId < 0) {
        query = DatabaseManager::instance().executeSelectQuery(
            QString("SELECT id FROM categories WHERE name = '%1' AND type = %2 AND parent_id IS NULL").arg(categoryName, QString::number(type)));
    } else {
        query = DatabaseManager::instance().executeSelectQuery(
            QString("SELECT id FROM categories WHERE name = '%1' AND type = %2 AND parent_id = %3").arg(categoryName, QString::number(type), QString::number(parentId)));
    }
    if (query.next()) {
        return query.value("id").toInt();
    }
    DatabaseManager::instance().insertCategory(categoryName, type, parentId);
    return DatabaseManager::instance().getLastInsertId();
}

static int getOrCreateAccount(const QString& accountName) {
    if (accountName.isEmpty()) return -1;
    QSqlQuery query = DatabaseManager::instance().executeSelectQuery(
        QString("SELECT id FROM accounts WHERE name = '%1'").arg(accountName));
    if (query.next()) {
        return query.value("id").toInt();
    }
    DatabaseManager::instance().insertAccount(accountName, "");
    return DatabaseManager::instance().getLastInsertId();
}

static int getOrCreateAccountCached(const QString& accountName, QMap<QString, int>& cache) {
    if (accountName.isEmpty()) return -1;
    if (cache.contains(accountName)) {
        return cache[accountName];
    }
    QSqlQuery query = DatabaseManager::instance().executeSelectQuery(
        QString("SELECT id FROM accounts WHERE name = '%1'").arg(accountName));
    if (query.next()) {
        int id = query.value("id").toInt();
        cache[accountName] = id;
        return id;
    }
    DatabaseManager::instance().insertAccount(accountName, "");
    int id = DatabaseManager::instance().getLastInsertId();
    cache[accountName] = id;
    return id;
}

static int getOrCreateCategoryCached(const QString& categoryName, int type, int parentId, QMap<QString, int>& cache) {
    QString key = QString("%1_%2_%3").arg(categoryName, QString::number(type), QString::number(parentId));
    if (cache.contains(key)) {
        return cache[key];
    }
    QSqlQuery query;
    if (parentId < 0) {
        query = DatabaseManager::instance().executeSelectQuery(
            QString("SELECT id FROM categories WHERE name = '%1' AND type = %2 AND parent_id IS NULL").arg(categoryName, QString::number(type)));
    } else {
        query = DatabaseManager::instance().executeSelectQuery(
            QString("SELECT id FROM categories WHERE name = '%1' AND type = %2 AND parent_id = %3").arg(categoryName, QString::number(type), QString::number(parentId)));
    }
    if (query.next()) {
        int id = query.value("id").toInt();
        cache[key] = id;
        return id;
    }
    DatabaseManager::instance().insertCategory(categoryName, type, parentId);
    int id = DatabaseManager::instance().getLastInsertId();
    cache[key] = id;
    return id;
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
        QString parentCategoryName = query.value("parent_category_name").toString();
        QString accountName = query.value("account_name").toString();
        int catType = query.value("category_type").toInt();
        double amt = query.value("amount").toDouble();
        QString type;
        if (catType == static_cast<int>(CategoryType::INCOME)) {
            type = QString::fromUtf8("收入");
        } else if (catType == static_cast<int>(CategoryType::TRANSFER)) {
            if (categoryName == TransferCategory::TRANSFER_OUT) {
                type = TransferCategory::TRANSFER_OUT;
            } else if (categoryName == TransferCategory::TRANSFER_IN) {
                type = TransferCategory::TRANSFER_IN;
            } else {
                type = TransferCategory::TRANSFER;
            }
        } else {
            type = QString::fromUtf8("支出");
        }
        QString amount = QString::number(qAbs(amt));
        QString note = query.value("note").toString();

        QString fullCategory = categoryName;
        if (!parentCategoryName.isEmpty()) {
            fullCategory = parentCategoryName + "-" + categoryName;
        }

        out << date << ","
            << "\"" << fullCategory << "\","
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

    // 缓存机制，避免重复查询
    QMap<QString, int> accountCache;
    QMap<QString, int> categoryCache;

    // 预加载现有账户和分类到缓存
    QSqlQuery accountQuery = DatabaseManager::instance().executeSelectQuery("SELECT id, name FROM accounts");
    while (accountQuery.next()) {
        accountCache[accountQuery.value("name").toString()] = accountQuery.value("id").toInt();
    }

    QSqlQuery categoryQuery = DatabaseManager::instance().executeSelectQuery("SELECT id, name, type, parent_id FROM categories");
    while (categoryQuery.next()) {
        QString key = QString("%1_%2_%3").arg(
            categoryQuery.value("name").toString(),
            QString::number(categoryQuery.value("type").toInt()),
            QString::number(categoryQuery.value("parent_id").toInt()));
        categoryCache[key] = categoryQuery.value("id").toInt();
    }

    int importedCount = 0;

    // 开启事务批量插入
    DatabaseManager::instance().beginTransaction();

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
            categoryType = static_cast<int>(CategoryType::INCOME);
        } else if (typeStr == TransferCategory::TRANSFER) {
            int fromAccountId = getOrCreateAccountCached(account1, accountCache);
            int toAccountId = getOrCreateAccountCached(account2, accountCache);
            int transferParentId = getOrCreateCategoryCached(TransferCategory::TRANSFER, static_cast<int>(CategoryType::TRANSFER), -1, categoryCache);
            int transferOutId = getOrCreateCategoryCached(TransferCategory::TRANSFER_OUT, static_cast<int>(CategoryType::TRANSFER), transferParentId, categoryCache);
            int transferInId = getOrCreateCategoryCached(TransferCategory::TRANSFER_IN, static_cast<int>(CategoryType::TRANSFER), transferParentId, categoryCache);

            if (fromAccountId >= 0 && toAccountId >= 0) {
                DatabaseManager::instance().insertTransaction(
                    transferOutId, fromAccountId, -qAbs(amount), date,
                    QString::fromUtf8("转出至%1").arg(account2));
                DatabaseManager::instance().insertTransaction(
                    transferInId, toAccountId, qAbs(amount), date,
                    QString::fromUtf8("转入自%1").arg(account1));
                importedCount++;
            }
            continue;
        } else if (typeStr == QString::fromUtf8("还款")) {
            int accountId = getOrCreateAccountCached(account1, accountCache);
            int categoryId = getOrCreateCategoryCached(categoryName.isEmpty() ? QString::fromUtf8("还款") : categoryName, categoryType, -1, categoryCache);
            if (categoryId >= 0 && accountId >= 0) {
                DatabaseManager::instance().insertTransaction(categoryId, accountId, qAbs(amount), date, note);
                importedCount++;
            }
            continue;
        }

        int finalCategoryId = -1;
        if (!subCategory.isEmpty() && subCategory != categoryName) {
            int parentId = getOrCreateCategoryCached(categoryName, categoryType, -1, categoryCache);
            finalCategoryId = getOrCreateCategoryCached(subCategory, categoryType, parentId, categoryCache);
        } else {
            finalCategoryId = getOrCreateCategoryCached(categoryName.isEmpty() ? QString::fromUtf8("其它") : categoryName, categoryType, -1, categoryCache);
        }

        int accountId = getOrCreateAccountCached(account1, accountCache);

        if (finalCategoryId >= 0 && accountId >= 0) {
            DatabaseManager::instance().insertTransaction(finalCategoryId, accountId, qAbs(amount), date, note);
            importedCount++;
        }
    }

    // 提交事务
    DatabaseManager::instance().commit();

    file.close();
    qDebug() << "Imported" << importedCount << "transactions";
    return importedCount > 0;
}

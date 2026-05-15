#include "DatabaseManager.h"
#include "Constants.h"
#include <QStandardPaths>
#include <QDir>

DatabaseManager::DatabaseManager() {
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(dataPath);
    }
    m_dbPath = dataPath + "/account.db";
}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::openDatabase() {
    if (m_database.isOpen()) {
        return true;
    }

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(m_dbPath);

    if (!m_database.open()) {
        return false;
    }

    QSqlQuery q;
    q.exec("PRAGMA encoding = 'UTF-8'");
    
    createTables();
    migrateDatabase();
    return true;
}

void DatabaseManager::closeDatabase() {
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool DatabaseManager::isOpen() const {
    return m_database.isOpen();
}

bool DatabaseManager::executeQuery(const QString& query) {
    QSqlQuery q;
    return q.exec(query);
}

QSqlQuery DatabaseManager::executeSelectQuery(const QString& query) {
    QSqlQuery q;
    q.exec(query);
    return q;
}

bool DatabaseManager::createTables() {
    QString categoryTable = R"(
        CREATE TABLE IF NOT EXISTS categories (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            type INTEGER NOT NULL,
            parent_id INTEGER DEFAULT NULL,
            icon TEXT DEFAULT '',
            FOREIGN KEY (parent_id) REFERENCES categories(id)
        )
    )";

    QString accountTable = R"(
        CREATE TABLE IF NOT EXISTS accounts (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            icon TEXT
        )
    )";

    QString transactionTable = R"(
        CREATE TABLE IF NOT EXISTS transactions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            category_id INTEGER NOT NULL,
            account_id INTEGER NOT NULL,
            amount REAL NOT NULL,
            date TEXT NOT NULL,
            note TEXT,
            FOREIGN KEY (category_id) REFERENCES categories(id),
            FOREIGN KEY (account_id) REFERENCES accounts(id)
        )
    )";

    if (!executeQuery(categoryTable)) return false;
    if (!executeQuery(accountTable)) return false;
    if (!executeQuery(transactionTable)) return false;

    QSqlQuery checkCategories = executeSelectQuery("SELECT COUNT(*) FROM categories");
    checkCategories.next();
    if (checkCategories.value(0).toInt() == 0) {
        insertCategory(QString::fromUtf8("工资"), 1);
        insertCategory(QString::fromUtf8("奖金"), 1);
        insertCategory(QString::fromUtf8("投资收益"), 1);
        insertCategory(QString::fromUtf8("其他收入"), 1);

        int catId = 0;

        insertCategory(QString::fromUtf8("餐饮"), 0);
        catId = getLastInsertId();
        insertCategory(QString::fromUtf8("早餐"), 0, catId);
        insertCategory(QString::fromUtf8("午餐"), 0, catId);
        insertCategory(QString::fromUtf8("晚餐"), 0, catId);
        insertCategory(QString::fromUtf8("零食饮料"), 0, catId);

        insertCategory(QString::fromUtf8("交通"), 0);
        catId = getLastInsertId();
        insertCategory(QString::fromUtf8("公交地铁"), 0, catId);
        insertCategory(QString::fromUtf8("打车"), 0, catId);
        insertCategory(QString::fromUtf8("加油"), 0, catId);

        insertCategory(QString::fromUtf8("购物"), 0);
        catId = getLastInsertId();
        insertCategory(QString::fromUtf8("衣物"), 0, catId);
        insertCategory(QString::fromUtf8("数码"), 0, catId);
        insertCategory(QString::fromUtf8("日用"), 0, catId);

        insertCategory(QString::fromUtf8("娱乐"), 0);
        catId = getLastInsertId();
        insertCategory(QString::fromUtf8("游戏"), 0, catId);
        insertCategory(QString::fromUtf8("电影"), 0, catId);
        insertCategory(QString::fromUtf8("旅游"), 0, catId);

        insertCategory(QString::fromUtf8("医疗"), 0);
        catId = getLastInsertId();
        insertCategory(QString::fromUtf8("门诊"), 0, catId);
        insertCategory(QString::fromUtf8("药品"), 0, catId);

        insertCategory(QString::fromUtf8("教育"), 0);
        catId = getLastInsertId();
        insertCategory(QString::fromUtf8("书籍"), 0, catId);
        insertCategory(QString::fromUtf8("培训"), 0, catId);

        insertCategory(QString::fromUtf8("住房"), 0);
        catId = getLastInsertId();
        insertCategory(QString::fromUtf8("房租"), 0, catId);
        insertCategory(QString::fromUtf8("物业"), 0, catId);
        insertCategory(QString::fromUtf8("水电"), 0, catId);

        insertCategory(QString::fromUtf8("日常"), 0);
        catId = getLastInsertId();
        insertCategory(QString::fromUtf8("话费"), 0, catId);
        insertCategory(QString::fromUtf8("理发"), 0, catId);

        insertCategory(QString::fromUtf8("其他支出"), 0);

        insertCategory(TransferCategory::TRANSFER, static_cast<int>(CategoryType::TRANSFER), -1, QStringLiteral("transfer.svg"));
        catId = getLastInsertId();
        insertCategory(TransferCategory::TRANSFER_IN, static_cast<int>(CategoryType::TRANSFER), catId, QStringLiteral("transfer.svg"));
        insertCategory(TransferCategory::TRANSFER_OUT, static_cast<int>(CategoryType::TRANSFER), catId, QStringLiteral("transfer.svg"));
    }

    QSqlQuery checkAccounts = executeSelectQuery("SELECT COUNT(*) FROM accounts");
    checkAccounts.next();
    if (checkAccounts.value(0).toInt() == 0) {
        insertAccount(QString::fromUtf8("现金"), "");
        insertAccount(QString::fromUtf8("银行卡"), "");
        insertAccount(QString::fromUtf8("微信"), "");
        insertAccount(QString::fromUtf8("支付宝"), "");
    }

    return true;
}

bool DatabaseManager::insertCategory(const QString& name, int type, int parentId, const QString& icon) {
    QSqlQuery q;
    if (parentId < 0) {
        q.prepare("INSERT INTO categories (name, type, parent_id, icon) VALUES (:name, :type, NULL, :icon)");
    } else {
        q.prepare("INSERT INTO categories (name, type, parent_id, icon) VALUES (:name, :type, :parent_id, :icon)");
        q.bindValue(":parent_id", parentId);
    }
    q.bindValue(":name", name);
    q.bindValue(":type", type);
    q.bindValue(":icon", icon);
    return q.exec();
}

bool DatabaseManager::updateCategory(int id, const QString& name, int type, int parentId, const QString& icon) {
    QSqlQuery q;
    if (parentId < 0) {
        q.prepare("UPDATE categories SET name = :name, type = :type, parent_id = NULL, icon = :icon WHERE id = :id");
    } else {
        q.prepare("UPDATE categories SET name = :name, type = :type, parent_id = :parent_id, icon = :icon WHERE id = :id");
        q.bindValue(":parent_id", parentId);
    }
    q.bindValue(":name", name);
    q.bindValue(":type", type);
    q.bindValue(":icon", icon);
    q.bindValue(":id", id);
    return q.exec();
}

bool DatabaseManager::deleteCategory(int id) {
    QSqlQuery q;
    q.prepare("DELETE FROM categories WHERE id = :id");
    q.bindValue(":id", id);
    return q.exec();
}

QSqlQuery DatabaseManager::getAllCategories() {
    return executeSelectQuery("SELECT * FROM categories ORDER BY type, parent_id, id ASC");
}

QSqlQuery DatabaseManager::getCategoriesByType(int type) {
    QSqlQuery q;
    q.prepare("SELECT * FROM categories WHERE type = :type ORDER BY parent_id, id ASC");
    q.bindValue(":type", type);
    q.exec();
    return q;
}

QSqlQuery DatabaseManager::getSubCategories(int parentId) {
    QSqlQuery q;
    q.prepare("SELECT * FROM categories WHERE parent_id = :parent_id ORDER BY id ASC");
    q.bindValue(":parent_id", parentId);
    q.exec();
    return q;
}

bool DatabaseManager::hasSubCategories(int categoryId) {
    QSqlQuery q;
    q.prepare("SELECT COUNT(*) FROM categories WHERE parent_id = :id");
    q.bindValue(":id", categoryId);
    q.exec();
    if (q.next()) {
        return q.value(0).toInt() > 0;
    }
    return false;
}

bool DatabaseManager::hasTransactions(int categoryId) {
    QSqlQuery q;
    q.prepare("SELECT COUNT(*) FROM transactions WHERE category_id = :id");
    q.bindValue(":id", categoryId);
    q.exec();
    if (q.next()) {
        return q.value(0).toInt() > 0;
    }
    return false;
}

bool DatabaseManager::insertAccount(const QString& name, const QString& icon) {
    QSqlQuery q;
    q.prepare("INSERT INTO accounts (name, icon) VALUES (:name, :icon)");
    q.bindValue(":name", name);
    q.bindValue(":icon", icon);
    return q.exec();
}

bool DatabaseManager::updateAccount(int id, const QString& name, const QString& icon) {
    QSqlQuery q;
    q.prepare("UPDATE accounts SET name = :name, icon = :icon WHERE id = :id");
    q.bindValue(":name", name);
    q.bindValue(":icon", icon);
    q.bindValue(":id", id);
    return q.exec();
}

bool DatabaseManager::deleteAccount(int id) {
    QSqlQuery q;
    q.prepare("DELETE FROM accounts WHERE id = :id");
    q.bindValue(":id", id);
    return q.exec();
}

QSqlQuery DatabaseManager::getAllAccounts() {
    return executeSelectQuery("SELECT * FROM accounts ORDER BY id ASC");
}

bool DatabaseManager::insertTransaction(int categoryId, int accountId, double amount,
                                       const QDate& date, const QString& note) {
    QSqlQuery q;
    q.prepare("INSERT INTO transactions (category_id, account_id, amount, date, note) "
              "VALUES (:category_id, :account_id, :amount, :date, :note)");
    q.bindValue(":category_id", categoryId);
    q.bindValue(":account_id", accountId);
    q.bindValue(":amount", amount);
    q.bindValue(":date", date.toString(Qt::ISODate));
    q.bindValue(":note", note);
    return q.exec();
}

bool DatabaseManager::insertTransaction(int categoryId, int accountId, double amount,
                                       const QDate& date, const QString& note, int transferId) {
    QSqlQuery q;
    q.prepare("INSERT INTO transactions (category_id, account_id, amount, date, note, transfer_id) "
              "VALUES (:category_id, :account_id, :amount, :date, :note, :transfer_id)");
    q.bindValue(":category_id", categoryId);
    q.bindValue(":account_id", accountId);
    q.bindValue(":amount", amount);
    q.bindValue(":date", date.toString(Qt::ISODate));
    q.bindValue(":note", note);
    q.bindValue(":transfer_id", transferId);
    return q.exec();
}

bool DatabaseManager::updateTransaction(int id, int categoryId, int accountId, double amount,
                                       const QDate& date, const QString& note) {
    QSqlQuery q;
    q.prepare("UPDATE transactions SET category_id = :category_id, account_id = :account_id, "
              "amount = :amount, date = :date, note = :note WHERE id = :id");
    q.bindValue(":category_id", categoryId);
    q.bindValue(":account_id", accountId);
    q.bindValue(":amount", amount);
    q.bindValue(":date", date.toString(Qt::ISODate));
    q.bindValue(":note", note);
    q.bindValue(":id", id);
    return q.exec();
}

bool DatabaseManager::deleteTransaction(int id) {
    QSqlQuery q;
    q.prepare("DELETE FROM transactions WHERE id = :id");
    q.bindValue(":id", id);
    return q.exec();
}

QSqlQuery DatabaseManager::getAllTransactions() {
    return executeSelectQuery("SELECT t.*, c.name as category_name, c.type as category_type, "
                             "c.parent_id as category_parent_id, p.name as parent_category_name, "
                             "a.name as account_name FROM transactions t "
                             "JOIN categories c ON t.category_id = c.id "
                             "LEFT JOIN categories p ON c.parent_id = p.id "
                             "JOIN accounts a ON t.account_id = a.id "
                             "ORDER BY t.date DESC");
}

QSqlQuery DatabaseManager::getTransactionsByDateRange(const QDate& start, const QDate& end) {
    QSqlQuery q;
    q.prepare("SELECT t.*, c.name as category_name, c.type as category_type, "
              "c.parent_id as category_parent_id, p.name as parent_category_name, "
              "a.name as account_name FROM transactions t "
              "JOIN categories c ON t.category_id = c.id "
              "LEFT JOIN categories p ON c.parent_id = p.id "
              "JOIN accounts a ON t.account_id = a.id "
              "WHERE t.date >= :start AND t.date <= :end "
              "ORDER BY t.date DESC");
    q.bindValue(":start", start.toString(Qt::ISODate));
    q.bindValue(":end", end.toString(Qt::ISODate));
    q.exec();
    return q;
}

QSqlQuery DatabaseManager::getTransactionsByCategory(int categoryId) {
    QSqlQuery q;
    q.prepare("SELECT t.*, c.name as category_name, c.type as category_type, "
              "c.parent_id as category_parent_id, p.name as parent_category_name, "
              "a.name as account_name FROM transactions t "
              "JOIN categories c ON t.category_id = c.id "
              "LEFT JOIN categories p ON c.parent_id = p.id "
              "JOIN accounts a ON t.account_id = a.id "
              "WHERE t.category_id = :category_id "
              "ORDER BY t.date DESC");
    q.bindValue(":category_id", categoryId);
    q.exec();
    return q;
}

QSqlQuery DatabaseManager::getTransactionsByAccount(int accountId) {
    QSqlQuery q;
    q.prepare("SELECT t.*, c.name as category_name, c.type as category_type, "
              "c.parent_id as category_parent_id, p.name as parent_category_name, "
              "a.name as account_name FROM transactions t "
              "JOIN categories c ON t.category_id = c.id "
              "LEFT JOIN categories p ON c.parent_id = p.id "
              "JOIN accounts a ON t.account_id = a.id "
              "WHERE t.account_id = :account_id "
              "ORDER BY t.date DESC");
    q.bindValue(":account_id", accountId);
    q.exec();
    return q;
}

QSqlQuery DatabaseManager::getCategoryStatistics(const QDate& start, const QDate& end, int type) {
    QSqlQuery q;
    q.prepare("SELECT "
              "CASE WHEN c.type = 2 THEN c.name ELSE COALESCE(p.name, c.name) END as name, "
              "CASE WHEN c.type = 2 THEN SUM(ABS(t.amount)) ELSE SUM(t.amount) END as total "
              "FROM transactions t "
              "JOIN categories c ON t.category_id = c.id "
              "LEFT JOIN categories p ON c.parent_id = p.id "
              "WHERE c.type = :type AND t.date >= :start AND t.date <= :end "
              "GROUP BY CASE WHEN c.type = 2 THEN c.name ELSE COALESCE(p.name, c.name) END "
              "ORDER BY total DESC");
    q.bindValue(":type", type);
    q.bindValue(":start", start.toString(Qt::ISODate));
    q.bindValue(":end", end.toString(Qt::ISODate));
    q.exec();
    return q;
}

QSqlQuery DatabaseManager::getMonthlyStatistics(int year, int type) {
    QSqlQuery q;
    q.prepare("SELECT strftime('%m', t.date) as month, "
              "CASE WHEN c.type = 2 THEN SUM(ABS(t.amount)) ELSE SUM(t.amount) END as total "
              "FROM transactions t JOIN categories c ON t.category_id = c.id "
              "WHERE c.type = :type AND strftime('%Y', t.date) = :year "
              "GROUP BY month ORDER BY month");
    q.bindValue(":type", type);
    q.bindValue(":year", QString::number(year));
    q.exec();
    return q;
}

QSqlQuery DatabaseManager::getAccountBalance() {
    return executeSelectQuery(
        "SELECT a.id, a.name, "
        "COALESCE(SUM(CASE WHEN c.type = 1 THEN t.amount ELSE 0 END), 0) as income, "
        "COALESCE(SUM(CASE WHEN c.type = 0 THEN t.amount ELSE 0 END), 0) as expense, "
        "COALESCE(SUM(CASE WHEN c.type = 2 AND t.amount > 0 THEN t.amount ELSE 0 END), 0) as transfer_in, "
        "COALESCE(SUM(CASE WHEN c.type = 2 AND t.amount < 0 THEN ABS(t.amount) ELSE 0 END), 0) as transfer_out "
        "FROM accounts a LEFT JOIN transactions t ON a.id = t.account_id "
        "LEFT JOIN categories c ON t.category_id = c.id "
        "GROUP BY a.id, a.name");
}

double DatabaseManager::getAccountBalance(int accountId) {
    QSqlQuery q;
    q.prepare("SELECT COALESCE(SUM("
              "CASE WHEN c.type = 1 THEN t.amount "
              "WHEN c.type = 0 THEN -t.amount "
              "WHEN c.type = 2 THEN t.amount "
              "ELSE 0 END), 0) as balance "
              "FROM transactions t "
              "JOIN categories c ON t.category_id = c.id "
              "WHERE t.account_id = :account_id");
    q.bindValue(":account_id", accountId);
    q.exec();
    if (q.next()) {
        return q.value(0).toDouble();
    }
    return 0.0;
}

double DatabaseManager::getTotalAssets() {
    QSqlQuery q = executeSelectQuery(
        "SELECT COALESCE(SUM("
        "CASE WHEN c.type = 1 THEN t.amount "
        "WHEN c.type = 0 THEN -t.amount "
        "WHEN c.type = 2 THEN t.amount "
        "ELSE 0 END), 0) as total_assets "
        "FROM transactions t "
        "JOIN categories c ON t.category_id = c.id");
    if (q.next()) {
        return q.value(0).toDouble();
    }
    return 0.0;
}

bool DatabaseManager::deleteTransferPair(int transactionId) {
    QSqlQuery q;
    q.prepare("SELECT transfer_id FROM transactions WHERE id = :id");
    q.bindValue(":id", transactionId);
    q.exec();
    int pairedId = -1;
    if (q.next() && !q.value(0).isNull()) {
        pairedId = q.value(0).toInt();
    }

    if (pairedId > 0) {
        QSqlQuery delPaired;
        delPaired.prepare("DELETE FROM transactions WHERE id = :id");
        delPaired.bindValue(":id", pairedId);
        delPaired.exec();
    }

    QSqlQuery delOrig;
    delOrig.prepare("DELETE FROM transactions WHERE id = :id");
    delOrig.bindValue(":id", transactionId);
    return delOrig.exec();
}

bool DatabaseManager::updateAccountBalance(int accountId, double balance) {
    Q_UNUSED(accountId);
    Q_UNUSED(balance);
    return true;
}

QSqlDatabase DatabaseManager::getDatabase() {
    return m_database;
}

int DatabaseManager::getLastInsertId() {
    QSqlQuery q = executeSelectQuery("SELECT last_insert_rowid()");
    if (q.next()) {
        return q.value(0).toInt();
    }
    return -1;
}

bool DatabaseManager::beginTransaction() {
    return m_database.transaction();
}

bool DatabaseManager::commit() {
    return m_database.commit();
}

bool DatabaseManager::rollback() {
    return m_database.rollback();
}

bool DatabaseManager::migrateDatabase() {
    QSqlQuery checkCol = executeSelectQuery("PRAGMA table_info(categories)");
    bool hasParentId = false;
    bool hasIcon = false;
    while (checkCol.next()) {
        QString colName = checkCol.value("name").toString();
        if (colName == "parent_id") {
            hasParentId = true;
        }
        if (colName == "icon") {
            hasIcon = true;
        }
    }
    if (!hasParentId) {
        if (!executeQuery("ALTER TABLE categories ADD COLUMN parent_id INTEGER DEFAULT NULL REFERENCES categories(id)")) {
            return false;
        }
    }
    if (!hasIcon) {
        if (!executeQuery("ALTER TABLE categories ADD COLUMN icon TEXT DEFAULT ''")) {
            return false;
        }
    }

    QSqlQuery checkTransferId = executeSelectQuery(
        QString("PRAGMA table_info(transactions)"));
    bool hasTransferId = false;
    while (checkTransferId.next()) {
        if (checkTransferId.value("name").toString() == "transfer_id") {
            hasTransferId = true;
            break;
        }
    }
    if (!hasTransferId) {
        if (!executeQuery("ALTER TABLE transactions ADD COLUMN transfer_id INTEGER DEFAULT NULL")) {
            return false;
        }
    }

    QSqlQuery checkTransfer = executeSelectQuery(
        QString("SELECT COUNT(*) FROM categories WHERE type = 2"));
    checkTransfer.next();
    if (checkTransfer.value(0).toInt() == 0) {
        insertCategory(QString::fromUtf8("转账"), 2, -1, QStringLiteral("transfer.svg"));
        int transferId = getLastInsertId();
        insertCategory(QString::fromUtf8("转入"), 2, transferId, QStringLiteral("transfer.svg"));
        insertCategory(QString::fromUtf8("转出"), 2, transferId, QStringLiteral("transfer.svg"));
    } else {
        QSqlQuery checkSubTransfer = executeSelectQuery(
            QString("SELECT COUNT(*) FROM categories WHERE type = 2 AND parent_id IS NOT NULL"));
        checkSubTransfer.next();
        if (checkSubTransfer.value(0).toInt() == 0) {
            QSqlQuery getTransferId = executeSelectQuery(
                QString("SELECT id FROM categories WHERE type = 2 AND parent_id IS NULL LIMIT 1"));
            if (getTransferId.next()) {
                int transferId = getTransferId.value(0).toInt();
                insertCategory(QString::fromUtf8("转入"), 2, transferId, QStringLiteral("transfer.svg"));
                insertCategory(QString::fromUtf8("转出"), 2, transferId, QStringLiteral("transfer.svg"));
            }
        }
    }

    return true;
}

int DatabaseManager::getCategoryIdByNameAndType(const QString& name, int type) {
    QSqlQuery q;
    q.prepare("SELECT id FROM categories WHERE name = :name AND type = :type AND parent_id IS NULL LIMIT 1");
    q.bindValue(":name", name);
    q.bindValue(":type", type);
    q.exec();
    if (q.next()) {
        return q.value(0).toInt();
    }
    return -1;
}

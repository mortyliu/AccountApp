#include "DatabaseManager.h"
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

    m_database.exec("PRAGMA encoding = 'UTF-8'");
    
    createTables();
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
            type INTEGER NOT NULL
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
        insertCategory(QString::fromUtf8("餐饮"), 0);
        insertCategory(QString::fromUtf8("交通"), 0);
        insertCategory(QString::fromUtf8("购物"), 0);
        insertCategory(QString::fromUtf8("娱乐"), 0);
        insertCategory(QString::fromUtf8("医疗"), 0);
        insertCategory(QString::fromUtf8("教育"), 0);
        insertCategory(QString::fromUtf8("住房"), 0);
        insertCategory(QString::fromUtf8("其他支出"), 0);
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

bool DatabaseManager::insertCategory(const QString& name, int type) {
    QSqlQuery q;
    q.prepare("INSERT INTO categories (name, type) VALUES (:name, :type)");
    q.bindValue(":name", name);
    q.bindValue(":type", type);
    return q.exec();
}

bool DatabaseManager::updateCategory(int id, const QString& name, int type) {
    QSqlQuery q;
    q.prepare("UPDATE categories SET name = :name, type = :type WHERE id = :id");
    q.bindValue(":name", name);
    q.bindValue(":type", type);
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
    return executeSelectQuery("SELECT * FROM categories ORDER BY id ASC");
}

QSqlQuery DatabaseManager::getCategoriesByType(int type) {
    QSqlQuery q;
    q.prepare("SELECT * FROM categories WHERE type = :type ORDER BY name");
    q.bindValue(":type", type);
    q.exec();
    return q;
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
                             "a.name as account_name FROM transactions t "
                             "JOIN categories c ON t.category_id = c.id "
                             "JOIN accounts a ON t.account_id = a.id "
                             "ORDER BY t.date DESC");
}

QSqlQuery DatabaseManager::getTransactionsByDateRange(const QDate& start, const QDate& end) {
    QSqlQuery q;
    q.prepare("SELECT t.*, c.name as category_name, c.type as category_type, "
              "a.name as account_name FROM transactions t "
              "JOIN categories c ON t.category_id = c.id "
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
              "a.name as account_name FROM transactions t "
              "JOIN categories c ON t.category_id = c.id "
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
              "a.name as account_name FROM transactions t "
              "JOIN categories c ON t.category_id = c.id "
              "JOIN accounts a ON t.account_id = a.id "
              "WHERE t.account_id = :account_id "
              "ORDER BY t.date DESC");
    q.bindValue(":account_id", accountId);
    q.exec();
    return q;
}

QSqlQuery DatabaseManager::getCategoryStatistics(const QDate& start, const QDate& end, int type) {
    QSqlQuery q;
    q.prepare("SELECT c.name, SUM(t.amount) as total FROM transactions t "
              "JOIN categories c ON t.category_id = c.id "
              "WHERE c.type = :type AND t.date >= :start AND t.date <= :end "
              "GROUP BY c.id, c.name ORDER BY total DESC");
    q.bindValue(":type", type);
    q.bindValue(":start", start.toString(Qt::ISODate));
    q.bindValue(":end", end.toString(Qt::ISODate));
    q.exec();
    return q;
}

QSqlQuery DatabaseManager::getMonthlyStatistics(int year, int type) {
    QSqlQuery q;
    q.prepare("SELECT strftime('%m', t.date) as month, SUM(t.amount) as total "
              "FROM transactions t JOIN categories c ON t.category_id = c.id "
              "WHERE c.type = :type AND strftime('%Y', t.date) = :year "
              "GROUP BY month ORDER BY month");
    q.bindValue(":type", type);
    q.bindValue(":year", QString::number(year));
    q.exec();
    return q;
}

QSqlQuery DatabaseManager::getAccountBalance() {
    return executeSelectQuery("SELECT a.name, "
                             "COALESCE(SUM(CASE WHEN c.type = 1 THEN t.amount ELSE 0 END), 0) as income, "
                             "COALESCE(SUM(CASE WHEN c.type = 0 THEN t.amount ELSE 0 END), 0) as expense "
                             "FROM accounts a LEFT JOIN transactions t ON a.id = t.account_id "
                             "LEFT JOIN categories c ON t.category_id = c.id "
                             "GROUP BY a.id, a.name");
}

double DatabaseManager::getAccountBalance(int accountId) {
    QSqlQuery q;
    q.prepare("SELECT COALESCE(SUM(CASE WHEN c.type = 1 THEN t.amount ELSE 0 END), 0) - "
              "COALESCE(SUM(CASE WHEN c.type = 0 THEN t.amount ELSE 0 END), 0) as balance "
              "FROM accounts a LEFT JOIN transactions t ON a.id = t.account_id "
              "LEFT JOIN categories c ON t.category_id = c.id "
              "WHERE a.id = :account_id");
    q.bindValue(":account_id", accountId);
    q.exec();
    if (q.next()) {
        return q.value(0).toDouble();
    }
    return 0.0;
}

bool DatabaseManager::updateAccountBalance(int accountId, double balance) {
    Q_UNUSED(accountId);
    Q_UNUSED(balance);
    return true;
}

QSqlDatabase DatabaseManager::getDatabase() {
    return m_database;
}

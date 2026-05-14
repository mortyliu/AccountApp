#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariant>
#include <QDate>

class DatabaseManager {
public:
    static DatabaseManager& instance();
    ~DatabaseManager();

    bool openDatabase();
    void closeDatabase();
    bool isOpen() const;

    bool executeQuery(const QString& query);
    QSqlQuery executeSelectQuery(const QString& query);

    bool createTables();
    bool migrateDatabase();
    bool insertCategory(const QString& name, int type, int parentId = -1, const QString& icon = "");
    bool updateCategory(int id, const QString& name, int type, int parentId = -1, const QString& icon = "");
    int getCategoryIdByNameAndType(const QString& name, int type);
    bool deleteCategory(int id);
    QSqlQuery getAllCategories();
    QSqlQuery getCategoriesByType(int type);
    QSqlQuery getSubCategories(int parentId);
    bool hasSubCategories(int categoryId);
    bool hasTransactions(int categoryId);

    bool insertAccount(const QString& name, const QString& icon = "");
    bool updateAccount(int id, const QString& name, const QString& icon);
    bool deleteAccount(int id);
    QSqlQuery getAllAccounts();

    bool insertTransaction(int categoryId, int accountId, double amount, 
                           const QDate& date, const QString& note = "");
    bool updateTransaction(int id, int categoryId, int accountId, double amount,
                           const QDate& date, const QString& note);
    bool deleteTransaction(int id);
    QSqlQuery getAllTransactions();
    QSqlQuery getTransactionsByDateRange(const QDate& start, const QDate& end);
    QSqlQuery getTransactionsByCategory(int categoryId);
    QSqlQuery getTransactionsByAccount(int accountId);

    QSqlQuery getCategoryStatistics(const QDate& start, const QDate& end, int type);
    QSqlQuery getMonthlyStatistics(int year, int type);
    QSqlQuery getAccountBalance();
    double getAccountBalance(int accountId);
    bool updateAccountBalance(int accountId, double balance);
    QSqlDatabase getDatabase();
    int getLastInsertId();

private:
    DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase m_database;
    QString m_dbPath;
};

#endif // DATABASEMANAGER_H

#include "TransactionController.h"
#include "../model/DatabaseManager.h"

TransactionController::TransactionController(QObject* parent) : QObject(parent) {}

TransactionModel* TransactionController::getModel() {
    return &m_model;
}

bool TransactionController::addTransaction(int categoryId, int accountId, double amount,
                                          const QDate& date, const QString& note) {
    return m_model.addTransaction(categoryId, accountId, amount, date, note);
}

bool TransactionController::updateTransaction(int id, int categoryId, int accountId, double amount,
                                             const QDate& date, const QString& note) {
    return m_model.updateTransaction(id, categoryId, accountId, amount, date, note);
}

bool TransactionController::deleteTransaction(int id) {
    return m_model.deleteTransaction(id);
}

void TransactionController::refresh() {
    m_model.refresh();
}

void TransactionController::filterByDateRange(const QDate& start, const QDate& end) {
    m_model.filterByDateRange(start, end);
}

void TransactionController::filterByCategory(int categoryId) {
    m_model.filterByCategory(categoryId);
}

void TransactionController::filterByAccount(int accountId) {
    m_model.filterByAccount(accountId);
}

void TransactionController::clearFilters() {
    m_model.clearFilters();
}

double TransactionController::getTotalIncome() const {
    return m_model.getTotalIncome();
}

double TransactionController::getTotalExpense() const {
    return m_model.getTotalExpense();
}

bool TransactionController::transfer(int fromAccountId, int toAccountId, double amount,
                                    const QDate& date, const QString& note) {
    DatabaseManager& db = DatabaseManager::instance();
    
    double fromBalance = db.getAccountBalance(fromAccountId);
    if (fromBalance < amount) {
        return false;
    }

    int transferCategoryId = db.getCategoryIdByNameAndType(
        QString::fromUtf8("转账"), 2);
    if (transferCategoryId < 0) {
        db.insertCategory(QString::fromUtf8("转账"), 2, -1, QStringLiteral("transfer.svg"));
        transferCategoryId = db.getLastInsertId();
    }

    QSqlDatabase database = db.getDatabase();
    database.transaction();

    bool success = true;

    QSqlQuery query(database);
    query.prepare("INSERT INTO transactions (category_id, account_id, amount, date, note) "
                  "VALUES (?, ?, ?, ?, ?)");
    
    query.addBindValue(transferCategoryId);
    query.addBindValue(fromAccountId);
    query.addBindValue(-amount);
    query.addBindValue(date.toString("yyyy-MM-dd"));
    QString fromNote = note.isEmpty() ? QStringLiteral("transfer_out") : note + QStringLiteral("_out");
    query.addBindValue(fromNote);
    success &= query.exec();

    query.prepare("INSERT INTO transactions (category_id, account_id, amount, date, note) "
                  "VALUES (?, ?, ?, ?, ?)");
    
    query.addBindValue(transferCategoryId);
    query.addBindValue(toAccountId);
    query.addBindValue(amount);
    query.addBindValue(date.toString("yyyy-MM-dd"));
    QString toNote = note.isEmpty() ? QStringLiteral("transfer_in") : note + QStringLiteral("_in");
    query.addBindValue(toNote);
    success &= query.exec();

    if (success) {
        database.commit();
    } else {
        database.rollback();
    }

    return success;
}

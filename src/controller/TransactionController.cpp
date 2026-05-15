#include "TransactionController.h"
#include "../model/DatabaseManager.h"
#include "../model/Constants.h"

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

bool TransactionController::deleteTransactionWithTransferPair(int id) {
    bool success = DatabaseManager::instance().deleteTransferPair(id);
    if (success) {
        m_model.refresh();
    }
    return success;
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

double TransactionController::getTotalTransfer() const {
    return m_model.getTotalTransfer();
}

double TransactionController::getTotalTransferIn() const {
    return m_model.getTotalTransferIn();
}

double TransactionController::getTotalTransferOut() const {
    return m_model.getTotalTransferOut();
}

bool TransactionController::transfer(int fromAccountId, int toAccountId, double amount,
                                    const QDate& date, const QString& note) {
    DatabaseManager& db = DatabaseManager::instance();
    
    double fromBalance = db.getAccountBalance(fromAccountId);
    if (fromBalance < amount) {
        return false;
    }

    int transferCategoryId = db.getCategoryIdByNameAndType(
        TransferCategory::TRANSFER, static_cast<int>(CategoryType::TRANSFER));
    if (transferCategoryId < 0) {
        db.insertCategory(TransferCategory::TRANSFER, static_cast<int>(CategoryType::TRANSFER), -1, QStringLiteral("transfer.svg"));
        transferCategoryId = db.getLastInsertId();
        db.insertCategory(TransferCategory::TRANSFER_IN, static_cast<int>(CategoryType::TRANSFER), transferCategoryId, QStringLiteral("transfer.svg"));
        db.insertCategory(TransferCategory::TRANSFER_OUT, static_cast<int>(CategoryType::TRANSFER), transferCategoryId, QStringLiteral("transfer.svg"));
    }

    int transferOutId = -1;
    int transferInId = -1;
    QSqlQuery subQuery(db.getDatabase());
    subQuery.prepare("SELECT id, name FROM categories WHERE parent_id = ? AND type = ?");
    subQuery.addBindValue(transferCategoryId);
    subQuery.addBindValue(static_cast<int>(CategoryType::TRANSFER));
    subQuery.exec();
    while (subQuery.next()) {
        QString name = subQuery.value("name").toString();
        if (name == TransferCategory::TRANSFER_OUT) {
            transferOutId = subQuery.value("id").toInt();
        } else if (name == TransferCategory::TRANSFER_IN) {
            transferInId = subQuery.value("id").toInt();
        }
    }

    if (transferOutId < 0 || transferInId < 0) {
        return false;
    }

    QSqlDatabase database = db.getDatabase();
    database.transaction();

    bool success = true;

    QSqlQuery query(database);
    query.prepare("INSERT INTO transactions (category_id, account_id, amount, date, note) "
                  "VALUES (?, ?, ?, ?, ?)");
    
    query.addBindValue(transferOutId);
    query.addBindValue(fromAccountId);
    query.addBindValue(-amount);
    query.addBindValue(date.toString("yyyy-MM-dd"));
    QString fromNote = note.isEmpty() ? TransferCategory::TRANSFER_OUT : note + QStringLiteral("_out");
    query.addBindValue(fromNote);
    success &= query.exec();
    
    int outTransactionId = db.getLastInsertId();

    query.prepare("INSERT INTO transactions (category_id, account_id, amount, date, note, transfer_id) "
                  "VALUES (?, ?, ?, ?, ?, ?)");
    
    query.addBindValue(transferInId);
    query.addBindValue(toAccountId);
    query.addBindValue(amount);
    query.addBindValue(date.toString("yyyy-MM-dd"));
    QString toNote = note.isEmpty() ? TransferCategory::TRANSFER_IN : note + QStringLiteral("_in");
    query.addBindValue(toNote);
    query.addBindValue(outTransactionId);
    success &= query.exec();
    
    int inTransactionId = db.getLastInsertId();

    query.prepare("UPDATE transactions SET transfer_id = ? WHERE id = ?");
    query.addBindValue(inTransactionId);
    query.addBindValue(outTransactionId);
    success &= query.exec();

    if (success) {
        db.commit();
    } else {
        db.rollback();
    }

    return success;
}

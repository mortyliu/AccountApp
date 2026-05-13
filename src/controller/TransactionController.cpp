#include "TransactionController.h"

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

#ifndef TRANSACTIONCONTROLLER_H
#define TRANSACTIONCONTROLLER_H

#include <QObject>
#include <QDate>
#include "../model/TransactionModel.h"

class TransactionController : public QObject {
    Q_OBJECT
public:
    explicit TransactionController(QObject* parent = nullptr);

    TransactionModel* getModel();

    bool addTransaction(int categoryId, int accountId, double amount,
                       const QDate& date, const QString& note = "");
    bool updateTransaction(int id, int categoryId, int accountId, double amount,
                          const QDate& date, const QString& note);
    bool deleteTransaction(int id);
    bool deleteTransactionWithTransferPair(int id);
    void refresh();
    void filterByDateRange(const QDate& start, const QDate& end);
    void filterByCategory(int categoryId);
    void filterByAccount(int accountId);
    void clearFilters();

    double getTotalIncome() const;
    double getTotalExpense() const;
    double getTotalTransfer() const;
    double getTotalTransferIn() const;
    double getTotalTransferOut() const;

    static bool transfer(int fromAccountId, int toAccountId, double amount,
                        const QDate& date, const QString& note = "");

private:
    TransactionModel m_model;
};

#endif // TRANSACTIONCONTROLLER_H

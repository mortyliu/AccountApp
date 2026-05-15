#ifndef TRANSACTIONMODEL_H
#define TRANSACTIONMODEL_H

#include <QAbstractTableModel>
#include <QSqlQuery>
#include <QList>
#include <QDate>
#include "Constants.h"

struct Transaction {
    int id;
    int categoryId;
    QString categoryName;
    QString parentCategoryName;
    int categoryType;
    int categoryParentId;
    int accountId;
    QString accountName;
    double amount;
    QDate date;
    QString note;
    int transferId;

    bool isTransferOut() const {
        return categoryType == static_cast<int>(CategoryType::TRANSFER) && categoryName == TransferCategory::TRANSFER_OUT;
    }

    bool isTransferIn() const {
        return categoryType == static_cast<int>(CategoryType::TRANSFER) && categoryName == TransferCategory::TRANSFER_IN;
    }

    QString fullCategoryName() const {
        if (!parentCategoryName.isEmpty() && categoryType == static_cast<int>(CategoryType::TRANSFER)) {
            return categoryName;
        }
        if (!parentCategoryName.isEmpty()) {
            return parentCategoryName + "-" + categoryName;
        }
        return categoryName;
    }
};

class TransactionModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit TransactionModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool addTransaction(int categoryId, int accountId, double amount, 
                       const QDate& date, const QString& note = "");
    bool updateTransaction(int id, int categoryId, int accountId, double amount,
                          const QDate& date, const QString& note);
    bool deleteTransaction(int id);
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

    const QList<Transaction>& getTransactions() const { return m_transactions; }

private:
    QList<Transaction> m_transactions;
    QDate m_filterStart;
    QDate m_filterEnd;
    int m_filterCategoryId;
    int m_filterAccountId;
};

#endif // TRANSACTIONMODEL_H

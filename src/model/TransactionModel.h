#ifndef TRANSACTIONMODEL_H
#define TRANSACTIONMODEL_H

#include <QAbstractTableModel>
#include <QSqlQuery>
#include <QList>
#include <QDate>

struct Transaction {
    int id;
    int categoryId;
    QString categoryName;
    int categoryType;
    int accountId;
    QString accountName;
    double amount;
    QDate date;
    QString note;
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

private:
    QList<Transaction> m_transactions;
    QDate m_filterStart;
    QDate m_filterEnd;
    int m_filterCategoryId;
    int m_filterAccountId;
};

#endif // TRANSACTIONMODEL_H

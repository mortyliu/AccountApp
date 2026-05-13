#include "TransactionModel.h"
#include "DatabaseManager.h"

TransactionModel::TransactionModel(QObject* parent) 
    : QAbstractTableModel(parent), m_filterCategoryId(-1), m_filterAccountId(-1) {
    refresh();
}

int TransactionModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return m_transactions.size();
}

int TransactionModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return 6;
}

QVariant TransactionModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_transactions.size()) {
        return QVariant();
    }

    const Transaction& transaction = m_transactions[index.row()];

    if (role == Qt::UserRole && index.column() == 0) {
        return QVariant(transaction.id);
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case 0:
            return QVariant(transaction.date.toString("yyyy-MM-dd"));
        case 1:
            return QVariant(transaction.categoryName);
        case 2:
            return QVariant(transaction.accountName);
        case 3:
            return QVariant(transaction.categoryType == 1 ? QString::fromUtf8("收入") : QString::fromUtf8("支出"));
        case 4: {
            QString prefix = transaction.categoryType == 1 ? "+" : "-";
            return QVariant(prefix + QString("%1").arg(transaction.amount, 0, 'f', 2));
        }
        case 5:
            return QVariant(transaction.note);
        }
    }

    return QVariant();
}

QVariant TransactionModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return QVariant(QString::fromUtf8("日期"));
        case 1:
            return QVariant(QString::fromUtf8("分类"));
        case 2:
            return QVariant(QString::fromUtf8("账户"));
        case 3:
            return QVariant(QString::fromUtf8("类型"));
        case 4:
            return QVariant(QString::fromUtf8("金额"));
        case 5:
            return QVariant(QString::fromUtf8("备注"));
        }
    }
    return QVariant();
}

bool TransactionModel::addTransaction(int categoryId, int accountId, double amount,
                                      const QDate& date, const QString& note) {
    bool success = DatabaseManager::instance().insertTransaction(categoryId, accountId, amount, date, note);
    if (success) {
        refresh();
    }
    return success;
}

bool TransactionModel::updateTransaction(int id, int categoryId, int accountId, double amount,
                                         const QDate& date, const QString& note) {
    bool success = DatabaseManager::instance().updateTransaction(id, categoryId, accountId, amount, date, note);
    if (success) {
        refresh();
    }
    return success;
}

bool TransactionModel::deleteTransaction(int id) {
    bool success = DatabaseManager::instance().deleteTransaction(id);
    if (success) {
        refresh();
    }
    return success;
}

void TransactionModel::refresh() {
    beginResetModel();
    m_transactions.clear();

    QSqlQuery query;

    if (m_filterCategoryId >= 0) {
        query = DatabaseManager::instance().getTransactionsByCategory(m_filterCategoryId);
    } else if (m_filterAccountId >= 0) {
        query = DatabaseManager::instance().getTransactionsByAccount(m_filterAccountId);
    } else if (m_filterStart.isValid() && m_filterEnd.isValid()) {
        query = DatabaseManager::instance().getTransactionsByDateRange(m_filterStart, m_filterEnd);
    } else {
        query = DatabaseManager::instance().getAllTransactions();
    }

    while (query.next()) {
        Transaction transaction;
        transaction.id = query.value("id").toInt();
        transaction.categoryId = query.value("category_id").toInt();
        transaction.categoryName = query.value("category_name").toString();
        transaction.categoryType = query.value("category_type").toInt();
        transaction.accountId = query.value("account_id").toInt();
        transaction.accountName = query.value("account_name").toString();
        transaction.amount = query.value("amount").toDouble();
        transaction.date = QDate::fromString(query.value("date").toString(), Qt::ISODate);
        transaction.note = query.value("note").toString();
        m_transactions.append(transaction);
    }

    endResetModel();
}

void TransactionModel::filterByDateRange(const QDate& start, const QDate& end) {
    m_filterStart = start;
    m_filterEnd = end;
    m_filterCategoryId = -1;
    m_filterAccountId = -1;
    refresh();
}

void TransactionModel::filterByCategory(int categoryId) {
    m_filterCategoryId = categoryId;
    m_filterAccountId = -1;
    m_filterStart = QDate();
    m_filterEnd = QDate();
    refresh();
}

void TransactionModel::filterByAccount(int accountId) {
    m_filterAccountId = accountId;
    m_filterCategoryId = -1;
    m_filterStart = QDate();
    m_filterEnd = QDate();
    refresh();
}

void TransactionModel::clearFilters() {
    m_filterStart = QDate();
    m_filterEnd = QDate();
    m_filterCategoryId = -1;
    m_filterAccountId = -1;
    refresh();
}

double TransactionModel::getTotalIncome() const {
    double total = 0.0;
    for (const Transaction& t : m_transactions) {
        if (t.categoryType == 1) {
            total += t.amount;
        }
    }
    return total;
}

double TransactionModel::getTotalExpense() const {
    double total = 0.0;
    for (const Transaction& t : m_transactions) {
        if (t.categoryType == 0) {
            total += t.amount;
        }
    }
    return total;
}

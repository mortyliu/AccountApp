#include "AccountModel.h"
#include "DatabaseManager.h"

AccountModel::AccountModel(QObject* parent) : QAbstractTableModel(parent) {
    refresh();
}

int AccountModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return m_accounts.size();
}

int AccountModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return 3;
}

QVariant AccountModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_accounts.size()) {
        return QVariant();
    }

    const Account& account = m_accounts[index.row()];

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case 0:
            return QVariant(account.id);
        case 1:
            return QVariant(account.name);
        case 2:
            return QVariant(QString("%1").arg(account.balance, 0, 'f', 2));
        }
    }

    return QVariant();
}

QVariant AccountModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return QVariant(QString::fromUtf8("ID"));
        case 1:
            return QVariant(QString::fromUtf8("名称"));
        case 2:
            return QVariant(QString::fromUtf8("余额"));
        }
    }
    return QVariant();
}

bool AccountModel::addAccount(const QString& name, const QString& icon) {
    bool success = DatabaseManager::instance().insertAccount(name, icon);
    if (success) {
        refresh();
    }
    return success;
}

bool AccountModel::updateAccount(int id, const QString& name, const QString& icon) {
    bool success = DatabaseManager::instance().updateAccount(id, name, icon);
    if (success) {
        refresh();
    }
    return success;
}

bool AccountModel::deleteAccount(int id) {
    bool success = DatabaseManager::instance().deleteAccount(id);
    if (success) {
        refresh();
    }
    return success;
}

void AccountModel::refresh() {
    beginResetModel();
    m_accounts.clear();

    QSqlQuery query = DatabaseManager::instance().getAllAccounts();
    while (query.next()) {
        Account account;
        account.id = query.value("id").toInt();
        account.name = query.value("name").toString();
        account.icon = query.value("icon").toString();
        account.balance = 0.0;
        m_accounts.append(account);
    }

    QSqlQuery balanceQuery = DatabaseManager::instance().getAccountBalance();
    while (balanceQuery.next()) {
        QString name = balanceQuery.value("name").toString();
        double income = balanceQuery.value("income").toDouble();
        double expense = balanceQuery.value("expense").toDouble();
        
        for (Account& acc : m_accounts) {
            if (acc.name == name) {
                acc.balance = income - expense;
                break;
            }
        }
    }

    endResetModel();
}

QString AccountModel::getAccountName(int id) {
    for (const Account& acc : m_accounts) {
        if (acc.id == id) {
            return acc.name;
        }
    }
    return "";
}

QList<Account> AccountModel::getAllAccounts() {
    return m_accounts;
}

#ifndef ACCOUNTMODEL_H
#define ACCOUNTMODEL_H

#include <QAbstractTableModel>
#include <QSqlQuery>
#include <QList>

struct Account {
    int id;
    QString name;
    QString icon;
    double balance;
};

class AccountModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit AccountModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool addAccount(const QString& name, const QString& icon = "");
    bool updateAccount(int id, const QString& name, const QString& icon);
    bool deleteAccount(int id);
    void refresh();

    QString getAccountName(int id);
    QList<Account> getAllAccounts();

private:
    QList<Account> m_accounts;
};

#endif // ACCOUNTMODEL_H

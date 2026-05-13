#ifndef ACCOUNTCONTROLLER_H
#define ACCOUNTCONTROLLER_H

#include <QObject>
#include "../model/AccountModel.h"

class AccountController : public QObject {
    Q_OBJECT
public:
    explicit AccountController(QObject* parent = nullptr);

    AccountModel* getModel();

    bool addAccount(const QString& name, const QString& icon = "");
    bool updateAccount(int id, const QString& name, const QString& icon);
    bool deleteAccount(int id);
    void refresh();

private:
    AccountModel m_model;
};

#endif // ACCOUNTCONTROLLER_H

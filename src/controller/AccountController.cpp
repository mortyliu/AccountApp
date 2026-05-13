#include "AccountController.h"

AccountController::AccountController(QObject* parent) : QObject(parent) {}

AccountModel* AccountController::getModel() {
    return &m_model;
}

bool AccountController::addAccount(const QString& name, const QString& icon) {
    return m_model.addAccount(name, icon);
}

bool AccountController::updateAccount(int id, const QString& name, const QString& icon) {
    return m_model.updateAccount(id, name, icon);
}

bool AccountController::deleteAccount(int id) {
    return m_model.deleteAccount(id);
}

void AccountController::refresh() {
    m_model.refresh();
}

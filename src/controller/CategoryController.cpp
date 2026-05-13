#include "CategoryController.h"

CategoryController::CategoryController(QObject* parent) : QObject(parent) {}

CategoryModel* CategoryController::getModel() {
    return &m_model;
}

bool CategoryController::addCategory(const QString& name, int type) {
    return m_model.addCategory(name, type);
}

bool CategoryController::updateCategory(int id, const QString& name, int type) {
    return m_model.updateCategory(id, name, type);
}

bool CategoryController::deleteCategory(int id) {
    return m_model.deleteCategory(id);
}

void CategoryController::refresh() {
    m_model.refresh();
}

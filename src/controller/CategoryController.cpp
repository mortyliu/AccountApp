#include "CategoryController.h"

CategoryController::CategoryController(QObject* parent) : QObject(parent) {}

CategoryModel* CategoryController::getModel() {
    return &m_model;
}

bool CategoryController::addCategory(const QString& name, int type, int parentId, const QString& icon) {
    return m_model.addCategory(name, type, parentId, icon);
}

bool CategoryController::updateCategory(int id, const QString& name, int type, int parentId, const QString& icon) {
    return m_model.updateCategory(id, name, type, parentId, icon);
}

bool CategoryController::deleteCategory(int id) {
    return m_model.deleteCategory(id);
}

void CategoryController::refresh() {
    m_model.refresh();
}

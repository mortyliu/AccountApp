#include "CategoryModel.h"
#include "DatabaseManager.h"

CategoryModel::CategoryModel(QObject* parent) : QAbstractTableModel(parent) {
    refresh();
}

int CategoryModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return m_categories.size();
}

int CategoryModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return 3;
}

QVariant CategoryModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_categories.size()) {
        return QVariant();
    }

    const Category& category = m_categories[index.row()];

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case 0:
            return QVariant(category.id);
        case 1:
            return QVariant(category.name);
        case 2:
            return QVariant(category.type == 1 ? QString::fromUtf8("收入") : QString::fromUtf8("支出"));
        }
    }

    return QVariant();
}

QVariant CategoryModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return QVariant(QString::fromUtf8("ID"));
        case 1:
            return QVariant(QString::fromUtf8("名称"));
        case 2:
            return QVariant(QString::fromUtf8("类型"));
        }
    }
    return QVariant();
}

bool CategoryModel::addCategory(const QString& name, int type) {
    bool success = DatabaseManager::instance().insertCategory(name, type);
    if (success) {
        refresh();
    }
    return success;
}

bool CategoryModel::updateCategory(int id, const QString& name, int type) {
    bool success = DatabaseManager::instance().updateCategory(id, name, type);
    if (success) {
        refresh();
    }
    return success;
}

bool CategoryModel::deleteCategory(int id) {
    bool success = DatabaseManager::instance().deleteCategory(id);
    if (success) {
        refresh();
    }
    return success;
}

void CategoryModel::refresh() {
    beginResetModel();
    m_categories.clear();

    QSqlQuery query = DatabaseManager::instance().getAllCategories();
    while (query.next()) {
        Category category;
        category.id = query.value("id").toInt();
        category.name = query.value("name").toString();
        category.type = query.value("type").toInt();
        m_categories.append(category);
    }

    endResetModel();
}

QList<Category> CategoryModel::getCategoriesByType(int type) {
    QList<Category> result;
    for (const Category& cat : m_categories) {
        if (cat.type == type) {
            result.append(cat);
        }
    }
    return result;
}

QString CategoryModel::getCategoryName(int id) {
    for (const Category& cat : m_categories) {
        if (cat.id == id) {
            return cat.name;
        }
    }
    return "";
}

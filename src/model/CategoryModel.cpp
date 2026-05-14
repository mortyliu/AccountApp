#include "CategoryModel.h"
#include "DatabaseManager.h"

QString Category::displayName() const {
    if (parentId < 0) {
        return name;
    }
    return name;
}

CategoryModel::CategoryModel(QObject* parent) : QAbstractTableModel(parent) {
    refresh();
}

int CategoryModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return m_categories.size();
}

int CategoryModel::columnCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return 4;
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
            return QVariant(category.type == 1 ? QString::fromUtf8("收入") :
                           (category.type == 2 ? QString::fromUtf8("转账") : QString::fromUtf8("支出")));
        case 3: {
            if (category.parentId < 0) {
                return QVariant(QString::fromUtf8("—"));
            }
            QString parentName = getCategoryName(category.parentId);
            return QVariant(parentName);
        }
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
        case 3:
            return QVariant(QString::fromUtf8("上级分类"));
        }
    }
    return QVariant();
}

bool CategoryModel::addCategory(const QString& name, int type, int parentId, const QString& icon) {
    bool success = DatabaseManager::instance().insertCategory(name, type, parentId, icon);
    if (success) {
        refresh();
    }
    return success;
}

bool CategoryModel::updateCategory(int id, const QString& name, int type, int parentId, const QString& icon) {
    bool success = DatabaseManager::instance().updateCategory(id, name, type, parentId, icon);
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
        category.parentId = query.value("parent_id").isNull() ? -1 : query.value("parent_id").toInt();
        category.icon = query.value("icon").toString();
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

QList<Category> CategoryModel::getTopLevelCategories(int type) {
    QList<Category> result;
    for (const Category& cat : m_categories) {
        if (cat.type == type && cat.parentId < 0) {
            result.append(cat);
        }
    }
    return result;
}

QList<Category> CategoryModel::getSubCategories(int parentId) {
    QList<Category> result;
    for (const Category& cat : m_categories) {
        if (cat.parentId == parentId) {
            result.append(cat);
        }
    }
    return result;
}

QString CategoryModel::getCategoryName(int id) const {
    for (const Category& cat : m_categories) {
        if (cat.id == id) {
            return cat.name;
        }
    }
    return "";
}

Category CategoryModel::getCategoryById(int id) const {
    for (const Category& cat : m_categories) {
        if (cat.id == id) {
            return cat;
        }
    }
    return Category{-1, "", -1, -1, ""};
}

QString CategoryModel::getFullCategoryName(int id) const {
    Category cat = getCategoryById(id);
    if (cat.id < 0) return "";
    if (cat.parentId < 0) return cat.name;
    QString parentName = getCategoryName(cat.parentId);
    return parentName + "-" + cat.name;
}

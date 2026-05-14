#ifndef CATEGORYMODEL_H
#define CATEGORYMODEL_H

#include <QAbstractTableModel>
#include <QSqlQuery>
#include <QList>

struct Category {
    int id;
    QString name;
    int type;
    int parentId;
    QString icon;

    bool isTopLevel() const { return parentId < 0; }
    QString displayName() const;
};

class CategoryModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit CategoryModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool addCategory(const QString& name, int type, int parentId = -1, const QString& icon = "");
    bool updateCategory(int id, const QString& name, int type, int parentId = -1, const QString& icon = "");
    bool deleteCategory(int id);
    void refresh();

    QList<Category> getCategoriesByType(int type);
    QList<Category> getTopLevelCategories(int type);
    QList<Category> getSubCategories(int parentId);
    QString getCategoryName(int id) const;
    Category getCategoryById(int id) const;
    QString getFullCategoryName(int id) const;

private:
    QList<Category> m_categories;
};

#endif // CATEGORYMODEL_H

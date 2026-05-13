#ifndef CATEGORYMODEL_H
#define CATEGORYMODEL_H

#include <QAbstractTableModel>
#include <QSqlQuery>
#include <QList>

struct Category {
    int id;
    QString name;
    int type; // 0: 支出, 1: 收入
};

class CategoryModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit CategoryModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool addCategory(const QString& name, int type);
    bool updateCategory(int id, const QString& name, int type);
    bool deleteCategory(int id);
    void refresh();

    QList<Category> getCategoriesByType(int type);
    QString getCategoryName(int id);

private:
    QList<Category> m_categories;
};

#endif // CATEGORYMODEL_H

#ifndef CATEGORYCONTROLLER_H
#define CATEGORYCONTROLLER_H

#include <QObject>
#include "../model/CategoryModel.h"

class CategoryController : public QObject {
    Q_OBJECT
public:
    explicit CategoryController(QObject* parent = nullptr);

    CategoryModel* getModel();

    bool addCategory(const QString& name, int type, int parentId = -1, const QString& icon = "");
    bool updateCategory(int id, const QString& name, int type, int parentId = -1, const QString& icon = "");
    bool deleteCategory(int id);
    void refresh();

private:
    CategoryModel m_model;
};

#endif // CATEGORYCONTROLLER_H

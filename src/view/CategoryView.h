#ifndef CATEGORYVIEW_H
#define CATEGORYVIEW_H

#include <QWidget>
#include <QTableView>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include "../controller/CategoryController.h"

class CategoryView : public QWidget {
    Q_OBJECT
public:
    explicit CategoryView(QWidget* parent = nullptr);
    ~CategoryView();

private slots:
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onTableDoubleClicked(const QModelIndex& index);

private:
    void setupUI();

    CategoryController m_controller;
    QTableView* m_tableView;
    QLineEdit* m_nameEdit;
    QComboBox* m_typeCombo;
    QPushButton* m_addBtn;
    QPushButton* m_editBtn;
    QPushButton* m_deleteBtn;

    int m_editId;
};

#endif // CATEGORYVIEW_H

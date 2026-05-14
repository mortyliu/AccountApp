#ifndef CATEGORYVIEW_H
#define CATEGORYVIEW_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "../controller/CategoryController.h"
#include "CategoryGridWidget.h"

class CategoryView : public QWidget {
    Q_OBJECT
public:
    explicit CategoryView(QWidget* parent = nullptr);
    ~CategoryView();

private slots:
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onCategorySelected(int categoryId);
    void onTypeChanged(int index);
    void onIconSelectClicked();

private:
    void setupUI();
    void updateParentCombo();
    void clearInput();

    CategoryController m_controller;
    CategoryGridWidget* m_categoryGrid;
    QLineEdit* m_nameEdit;
    QComboBox* m_typeCombo;
    QComboBox* m_parentCombo;
    QPushButton* m_addBtn;
    QPushButton* m_editBtn;
    QPushButton* m_deleteBtn;
    QLabel* m_iconPreviewLabel;
    QPushButton* m_iconSelectBtn;
    QString m_currentIcon;

    int m_editId;
};

#endif // CATEGORYVIEW_H

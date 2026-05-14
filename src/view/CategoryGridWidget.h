#ifndef CATEGORYGRIDWIDGET_H
#define CATEGORYGRIDWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include "../model/CategoryModel.h"

class CategoryItem : public QFrame {
    Q_OBJECT
public:
    explicit CategoryItem(int categoryId, const QString& name, const QString& iconPath, QWidget* parent = nullptr);

    int categoryId() const { return m_categoryId; }
    void setSelected(bool selected);
    bool isSelected() const { return m_selected; }

signals:
    void clicked(int categoryId);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    int m_categoryId;
    QLabel* m_iconLabel;
    QLabel* m_nameLabel;
    bool m_selected = false;
};

class CategoryGridWidget : public QWidget {
    Q_OBJECT
public:
    explicit CategoryGridWidget(QWidget* parent = nullptr);

    void setCategoryModel(CategoryModel* model);
    void refresh();
    int selectedCategoryId() const;
    void setSelectedCategory(int categoryId);
    void clearSelection();

signals:
    void categorySelected(int categoryId);
    void addCategoryRequested();

private slots:
    void onTabChanged(int type);
    void onItemClicked(int categoryId);
    void onCloseClicked();
    void onAddClicked();

private:
    void setupUI();
    void setupHeader();
    void updateGrid();
    void showSubCategories(int parentId);

    static QString getDefaultIcon(const QString& categoryName);

    CategoryModel* m_model;

    QFrame* m_headerFrame;
    QPushButton* m_closeBtn;
    QHBoxLayout* m_tabLayout;
    QVector<QPushButton*> m_tabButtons;
    QPushButton* m_addBtn;

    QWidget* m_gridContainer;
    QGridLayout* m_gridLayout;
    QFrame* m_subAreaFrame;
    QGridLayout* m_subGridLayout;

    int m_currentType;
    int m_selectedParentId;
    int m_finalSelectedId;
};

#endif // CATEGORYGRIDWIDGET_H

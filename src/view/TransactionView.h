#ifndef TRANSACTIONVIEW_H
#define TRANSACTIONVIEW_H

#include <QWidget>
#include <QTableView>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include "../controller/TransactionController.h"
#include "../model/CategoryModel.h"
#include "../model/AccountModel.h"
#include "../model/DatabaseManager.h"
#include "CategoryGridWidget.h"

class TransactionView : public QWidget {
    Q_OBJECT
public:
    explicit TransactionView(QWidget* parent = nullptr);
    ~TransactionView();

    void refreshData();

private slots:
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onFilterClicked();
    void onClearFilterClicked();
    void onTableDoubleClicked(const QModelIndex& index);
    void onCategorySelected(int categoryId);
    void onToggleCategoryPanel();
    void onImportClicked();

private:
    void setupUI();
    void updateSummary();
    int getSelectedCategoryId() const;

    TransactionController m_controller;
    CategoryModel* m_categoryModel;
    AccountModel* m_accountModel;

    QTableView* m_tableView;

    QPushButton* m_toggleCategoryBtn;
    QLabel* m_selectedCategoryLabel;
    CategoryGridWidget* m_categoryGrid;

    QComboBox* m_accountCombo;
    QLineEdit* m_amountEdit;
    QDateEdit* m_dateEdit;
    QLineEdit* m_noteEdit;

    QComboBox* m_filterCategoryCombo;
    QComboBox* m_filterAccountCombo;
    QDateEdit* m_filterStartDate;
    QDateEdit* m_filterEndDate;

    QPushButton* m_addBtn;
    QPushButton* m_editBtn;
    QPushButton* m_deleteBtn;
    QPushButton* m_filterBtn;
    QPushButton* m_clearFilterBtn;
    QPushButton* m_importBtn;

    QLabel* m_incomeLabel;
    QLabel* m_expenseLabel;
    QLabel* m_balanceLabel;
    QLabel* m_assetsLabel;

    int m_editId;
};

#endif // TRANSACTIONVIEW_H

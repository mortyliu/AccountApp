#ifndef TRANSACTIONVIEW_H
#define TRANSACTIONVIEW_H

#include <QWidget>
#include <QTableView>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include "../controller/TransactionController.h"
#include "../model/CategoryModel.h"
#include "../model/AccountModel.h"
#include "../model/DatabaseManager.h"

class TransactionView : public QWidget {
    Q_OBJECT
public:
    explicit TransactionView(QWidget* parent = nullptr);
    ~TransactionView();

private slots:
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onFilterClicked();
    void onClearFilterClicked();
    void onTableDoubleClicked(const QModelIndex& index);
    void onTypeChanged(int index);

private:
    void setupUI();
    void updateCategoryCombo(int type);

    TransactionController m_controller;
    CategoryModel* m_categoryModel;
    AccountModel* m_accountModel;

    QTableView* m_tableView;
    
    QComboBox* m_typeCombo;
    QComboBox* m_categoryCombo;
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

    int m_editId;
};

#endif // TRANSACTIONVIEW_H

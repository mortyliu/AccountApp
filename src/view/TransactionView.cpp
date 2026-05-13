#include "TransactionView.h"
#include "../model/TransactionModel.h"
#include "../model/CategoryModel.h"
#include "../model/AccountModel.h"
#include "../controller/TransactionController.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTableView>
#include <QDateEdit>
#include <QMessageBox>
#include <QLabel>

TransactionView::TransactionView(QWidget* parent) : QWidget(parent) {
    m_categoryModel = new CategoryModel(this);
    m_accountModel = new AccountModel(this);
    setupUI();
}

void TransactionView::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* inputLayout = new QHBoxLayout();
    
    m_typeCombo = new QComboBox();
    m_typeCombo->addItem(QString::fromUtf8("支出"), 0);
    m_typeCombo->addItem(QString::fromUtf8("收入"), 1);
    
    m_categoryCombo = new QComboBox();
    updateCategoryCombo(0);
    
    m_accountCombo = new QComboBox();
    QList<Account> accounts = m_accountModel->getAllAccounts();
    for (const Account& acc : accounts) {
        m_accountCombo->addItem(acc.name, acc.id);
    }
    
    m_amountEdit = new QLineEdit();
    m_amountEdit->setPlaceholderText(QString::fromUtf8("金额"));
    
    m_dateEdit = new QDateEdit(QDate::currentDate());
    m_dateEdit->setDisplayFormat("yyyy-MM-dd");
    
    m_noteEdit = new QLineEdit();
    m_noteEdit->setPlaceholderText(QString::fromUtf8("备注"));
    
    m_addBtn = new QPushButton(QString::fromUtf8("添加"));
    m_editBtn = new QPushButton(QString::fromUtf8("修改"));
    m_editBtn->setEnabled(false);
    m_deleteBtn = new QPushButton(QString::fromUtf8("删除"));
    m_deleteBtn->setEnabled(false);

    inputLayout->addWidget(m_typeCombo);
    inputLayout->addWidget(m_categoryCombo);
    inputLayout->addWidget(m_accountCombo);
    inputLayout->addWidget(m_amountEdit);
    inputLayout->addWidget(m_dateEdit);
    inputLayout->addWidget(m_noteEdit);
    inputLayout->addWidget(m_addBtn);
    inputLayout->addWidget(m_editBtn);
    inputLayout->addWidget(m_deleteBtn);

    QHBoxLayout* filterLayout = new QHBoxLayout();
    
    m_filterCategoryCombo = new QComboBox();
    m_filterCategoryCombo->addItem(QString::fromUtf8("全部分类"), -1);
    QList<Category> categories = m_categoryModel->getCategoriesByType(0);
    categories += m_categoryModel->getCategoriesByType(1);
    for (const Category& cat : categories) {
        m_filterCategoryCombo->addItem(cat.name, cat.id);
    }
    
    m_filterAccountCombo = new QComboBox();
    m_filterAccountCombo->addItem(QString::fromUtf8("全部账户"), -1);
    QList<Account> filterAccounts = m_accountModel->getAllAccounts();
    for (const Account& acc : filterAccounts) {
        m_filterAccountCombo->addItem(acc.name, acc.id);
    }
    
    m_filterStartDate = new QDateEdit();
    m_filterStartDate->setDisplayFormat("yyyy-MM-dd");
    
    m_filterEndDate = new QDateEdit();
    m_filterEndDate->setDisplayFormat("yyyy-MM-dd");
    
    m_filterBtn = new QPushButton(QString::fromUtf8("筛选"));
    m_clearFilterBtn = new QPushButton(QString::fromUtf8("清除筛选"));

    filterLayout->addWidget(new QLabel(QString::fromUtf8("分类:")));
    filterLayout->addWidget(m_filterCategoryCombo);
    filterLayout->addWidget(new QLabel(QString::fromUtf8("账户:")));
    filterLayout->addWidget(m_filterAccountCombo);
    filterLayout->addWidget(new QLabel(QString::fromUtf8("开始日期:")));
    filterLayout->addWidget(m_filterStartDate);
    filterLayout->addWidget(new QLabel(QString::fromUtf8("结束日期:")));
    filterLayout->addWidget(m_filterEndDate);
    filterLayout->addWidget(m_filterBtn);
    filterLayout->addWidget(m_clearFilterBtn);

    m_tableView = new QTableView();
    m_tableView->setModel(m_controller.getModel());
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setColumnWidth(0, 100);
    m_tableView->setColumnWidth(1, 100);
    m_tableView->setColumnWidth(2, 100);
    m_tableView->setColumnWidth(3, 60);
    m_tableView->setColumnWidth(4, 100);
    m_tableView->setColumnWidth(5, 150);

    mainLayout->addLayout(inputLayout);
    mainLayout->addLayout(filterLayout);
    mainLayout->addWidget(m_tableView);

    connect(m_addBtn, &QPushButton::clicked, this, &TransactionView::onAddClicked);
    connect(m_editBtn, &QPushButton::clicked, this, &TransactionView::onEditClicked);
    connect(m_deleteBtn, &QPushButton::clicked, this, &TransactionView::onDeleteClicked);
    connect(m_tableView, &QTableView::doubleClicked, this, &TransactionView::onTableDoubleClicked);
    connect(m_typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TransactionView::onTypeChanged);
    connect(m_filterBtn, &QPushButton::clicked, this, &TransactionView::onFilterClicked);
    connect(m_clearFilterBtn, &QPushButton::clicked, this, &TransactionView::onClearFilterClicked);
}

void TransactionView::updateCategoryCombo(int type) {
    m_categoryCombo->clear();
    QList<Category> categories = m_categoryModel->getCategoriesByType(type);
    for (const Category& cat : categories) {
        m_categoryCombo->addItem(cat.name, cat.id);
    }
}

void TransactionView::onTypeChanged(int index) {
    int type = m_typeCombo->currentData().toInt();
    updateCategoryCombo(type);
}

void TransactionView::onAddClicked() {
    double amount = m_amountEdit->text().toDouble();
    if (amount <= 0) {
        QMessageBox::warning(this, QString::fromUtf8("警告"), QString::fromUtf8("请输入有效的金额"));
        return;
    }

    int categoryId = m_categoryCombo->currentData().toInt();
    int accountId = m_accountCombo->currentData().toInt();
    QDate date = m_dateEdit->date();
    QString note = m_noteEdit->text().trimmed();

    if (m_controller.addTransaction(categoryId, accountId, amount, date, note)) {
        m_amountEdit->clear();
        m_noteEdit->clear();
        QMessageBox::information(this, QString::fromUtf8("成功"), QString::fromUtf8("添加成功"));
    } else {
        QMessageBox::warning(this, QString::fromUtf8("失败"), QString::fromUtf8("添加失败"));
    }
}

void TransactionView::onEditClicked() {
    double amount = m_amountEdit->text().toDouble();
    if (amount <= 0) {
        QMessageBox::warning(this, QString::fromUtf8("警告"), QString::fromUtf8("请输入有效的金额"));
        return;
    }

    int categoryId = m_categoryCombo->currentData().toInt();
    int accountId = m_accountCombo->currentData().toInt();
    QDate date = m_dateEdit->date();
    QString note = m_noteEdit->text().trimmed();

    if (m_controller.updateTransaction(m_editId, categoryId, accountId, amount, date, note)) {
        m_amountEdit->clear();
        m_noteEdit->clear();
        m_editBtn->setEnabled(false);
        m_deleteBtn->setEnabled(false);
        m_editId = -1;
        QMessageBox::information(this, QString::fromUtf8("成功"), QString::fromUtf8("修改成功"));
    } else {
        QMessageBox::warning(this, QString::fromUtf8("失败"), QString::fromUtf8("修改失败"));
    }
}

void TransactionView::onDeleteClicked() {
    if (QMessageBox::question(this, QString::fromUtf8("确认"), QString::fromUtf8("确定要删除该记录吗？")) == QMessageBox::Yes) {
        if (m_controller.deleteTransaction(m_editId)) {
            m_amountEdit->clear();
            m_noteEdit->clear();
            m_editBtn->setEnabled(false);
            m_deleteBtn->setEnabled(false);
            m_editId = -1;
            QMessageBox::information(this, QString::fromUtf8("成功"), QString::fromUtf8("删除成功"));
        } else {
            QMessageBox::warning(this, QString::fromUtf8("失败"), QString::fromUtf8("删除失败"));
        }
    }
}

void TransactionView::onTableDoubleClicked(const QModelIndex& index) {
    if (index.isValid()) {
        m_editId = index.sibling(index.row(), 0).data(Qt::UserRole).toInt();
        QString categoryName = index.sibling(index.row(), 1).data().toString();
        QString accountName = index.sibling(index.row(), 2).data().toString();
        QString typeStr = index.sibling(index.row(), 3).data().toString();
        QString amountStr = index.sibling(index.row(), 4).data().toString();
        QString dateStr = index.sibling(index.row(), 0).data().toString();
        QString note = index.sibling(index.row(), 5).data().toString();

        m_typeCombo->setCurrentIndex(typeStr == QString::fromUtf8("收入") ? 1 : 0);
        onTypeChanged(m_typeCombo->currentIndex());
        
        int categoryIndex = m_categoryCombo->findText(categoryName);
        if (categoryIndex >= 0) {
            m_categoryCombo->setCurrentIndex(categoryIndex);
        }
        
        int accountIndex = m_accountCombo->findText(accountName);
        if (accountIndex >= 0) {
            m_accountCombo->setCurrentIndex(accountIndex);
        }
        
        m_amountEdit->setText(amountStr.mid(1));
        m_dateEdit->setDate(QDate::fromString(dateStr, "yyyy-MM-dd"));
        m_noteEdit->setText(note);
        
        m_editBtn->setEnabled(true);
        m_deleteBtn->setEnabled(true);
    }
}

void TransactionView::onFilterClicked() {
    int categoryId = m_filterCategoryCombo->currentData().toInt();
    int accountId = m_filterAccountCombo->currentData().toInt();
    QDate startDate = m_filterStartDate->date();
    QDate endDate = m_filterEndDate->date();

    if (categoryId >= 0) {
        m_controller.filterByCategory(categoryId);
    } else if (accountId >= 0) {
        m_controller.filterByAccount(accountId);
    } else if (startDate.isValid() && endDate.isValid()) {
        m_controller.filterByDateRange(startDate, endDate);
    }
}

void TransactionView::onClearFilterClicked() {
    m_filterCategoryCombo->setCurrentIndex(0);
    m_filterAccountCombo->setCurrentIndex(0);
    m_filterStartDate->setDate(QDate());
    m_filterEndDate->setDate(QDate());
    m_controller.clearFilters();
}

TransactionView::~TransactionView() {}

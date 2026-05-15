#include "TransactionView.h"
#include "../model/CategoryModel.h"
#include "../model/AccountModel.h"
#include "../controller/TransactionController.h"
#include "../model/Constants.h"
#include "../utils/CsvExporter.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTableView>
#include <QDateEdit>
#include <QMessageBox>
#include <QLabel>
#include <QFrame>
#include <QFileDialog>
#include <QHeaderView>
#include <QStyle>

TransactionView::TransactionView(QWidget* parent) : QWidget(parent), m_editId(-1) {
    m_categoryModel = new CategoryModel(this);
    m_accountModel = new AccountModel(this);
    setupUI();
}

void TransactionView::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(12);

    QHBoxLayout* summaryLayout = new QHBoxLayout();
    summaryLayout->setSpacing(16);

    QFrame* incomeCard = new QFrame();
    incomeCard->setObjectName("cardFrame");
    QVBoxLayout* incomeLayout = new QVBoxLayout(incomeCard);
    incomeLayout->setContentsMargins(20, 16, 20, 16);
    QLabel* incomeTitle = new QLabel(QStringLiteral("总收入"));
    incomeTitle->setObjectName("cardTitle");
    m_incomeLabel = new QLabel(QStringLiteral("0.00"));
    m_incomeLabel->setObjectName("incomeValue");
    incomeLayout->addWidget(incomeTitle);
    incomeLayout->addWidget(m_incomeLabel);

    QFrame* expenseCard = new QFrame();
    expenseCard->setObjectName("cardFrame");
    QVBoxLayout* expenseLayout = new QVBoxLayout(expenseCard);
    expenseLayout->setContentsMargins(20, 16, 20, 16);
    QLabel* expenseTitle = new QLabel(QStringLiteral("总支出"));
    expenseTitle->setObjectName("cardTitle");
    m_expenseLabel = new QLabel(QStringLiteral("0.00"));
    m_expenseLabel->setObjectName("expenseValue");
    expenseLayout->addWidget(expenseTitle);
    expenseLayout->addWidget(m_expenseLabel);

    QFrame* balanceCard = new QFrame();
    balanceCard->setObjectName("cardFrame");
    QVBoxLayout* balanceLayout = new QVBoxLayout(balanceCard);
    balanceLayout->setContentsMargins(20, 16, 20, 16);
    QLabel* balanceTitle = new QLabel(QStringLiteral("净结余"));
    balanceTitle->setObjectName("cardTitle");
    m_balanceLabel = new QLabel(QStringLiteral("0.00"));
    m_balanceLabel->setObjectName("cardValue");
    balanceLayout->addWidget(balanceTitle);
    balanceLayout->addWidget(m_balanceLabel);

    QFrame* assetsCard = new QFrame();
    assetsCard->setObjectName("cardFrame");
    QVBoxLayout* assetsLayout = new QVBoxLayout(assetsCard);
    assetsLayout->setContentsMargins(20, 16, 20, 16);
    QLabel* assetsTitle = new QLabel(QStringLiteral("总资产"));
    assetsTitle->setObjectName("cardTitle");
    m_assetsLabel = new QLabel(QStringLiteral("0.00"));
    m_assetsLabel->setObjectName("cardValue");
    assetsLayout->addWidget(assetsTitle);
    assetsLayout->addWidget(m_assetsLabel);

    summaryLayout->addWidget(incomeCard);
    summaryLayout->addWidget(expenseCard);
    summaryLayout->addWidget(balanceCard);
    summaryLayout->addWidget(assetsCard);

    QFrame* inputCard = new QFrame();
    inputCard->setObjectName("cardFrame");
    QVBoxLayout* inputCardLayout = new QVBoxLayout(inputCard);
    inputCardLayout->setContentsMargins(16, 12, 16, 12);
    inputCardLayout->setSpacing(8);

    QHBoxLayout* topRow = new QHBoxLayout();
    topRow->setSpacing(8);

    m_toggleCategoryBtn = new QPushButton(QStringLiteral("选择分类 ▼"));
    m_toggleCategoryBtn->setObjectName("categoryToggleBtn");
    m_toggleCategoryBtn->setFixedHeight(36);
    m_toggleCategoryBtn->setMinimumWidth(120);

    m_selectedCategoryLabel = new QLabel(QStringLiteral("未选择分类"));
    m_selectedCategoryLabel->setObjectName("selectedCategoryLabel");

    m_accountCombo = new QComboBox();
    QList<Account> accounts = m_accountModel->getAllAccounts();
    for (const Account& acc : accounts) {
        m_accountCombo->addItem(acc.name, acc.id);
    }
    m_accountCombo->setMinimumWidth(100);
    m_accountCombo->setFixedHeight(36);

    m_amountEdit = new QLineEdit();
    m_amountEdit->setPlaceholderText(QStringLiteral("金额"));
    m_amountEdit->setFixedWidth(100);
    m_amountEdit->setFixedHeight(36);

    m_dateEdit = new QDateEdit(QDate::currentDate());
    m_dateEdit->setDisplayFormat("yyyy-MM-dd");
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setFixedWidth(130);
    m_dateEdit->setFixedHeight(36);

    m_noteEdit = new QLineEdit();
    m_noteEdit->setPlaceholderText(QStringLiteral("备注"));
    m_noteEdit->setFixedHeight(36);

    m_addBtn = new QPushButton(QStringLiteral("添加"));
    m_addBtn->setFixedHeight(36);
    m_editBtn = new QPushButton(QStringLiteral("修改"));
    m_editBtn->setObjectName("secondaryBtn");
    m_editBtn->setEnabled(false);
    m_editBtn->setFixedHeight(36);
    m_deleteBtn = new QPushButton(QStringLiteral("删除"));
    m_deleteBtn->setObjectName("dangerBtn");
    m_deleteBtn->setEnabled(false);
    m_deleteBtn->setFixedHeight(36);

    topRow->addWidget(m_toggleCategoryBtn);
    topRow->addWidget(m_selectedCategoryLabel, 1);
    topRow->addWidget(m_accountCombo);
    topRow->addWidget(m_amountEdit);
    topRow->addWidget(m_dateEdit);
    topRow->addWidget(m_noteEdit, 1);
    topRow->addWidget(m_addBtn);
    topRow->addWidget(m_editBtn);
    topRow->addWidget(m_deleteBtn);

    inputCardLayout->addLayout(topRow);

    m_categoryGrid = new CategoryGridWidget();
    m_categoryGrid->setCategoryModel(m_categoryModel);
    m_categoryGrid->setVisible(false);
    m_categoryGrid->setMinimumHeight(200);
    m_categoryGrid->setMaximumHeight(280);
    m_categoryGrid->setObjectName("categoryGridPanel");

    inputCardLayout->addWidget(m_categoryGrid);

    QFrame* filterCard = new QFrame();
    filterCard->setObjectName("cardFrame");
    QVBoxLayout* filterCardLayout = new QVBoxLayout(filterCard);
    filterCardLayout->setContentsMargins(16, 12, 16, 12);
    filterCardLayout->setSpacing(8);

    QLabel* filterSectionLabel = new QLabel(QStringLiteral("筛选"));
    filterSectionLabel->setObjectName("sectionLabel");

    QHBoxLayout* filterLayout = new QHBoxLayout();
    filterLayout->setSpacing(8);

    m_filterCategoryCombo = new QComboBox();
    m_filterCategoryCombo->addItem(QStringLiteral("全部分类"), -1);
    QList<Category> allCategories = m_categoryModel->getCategoriesByType(0);
    allCategories += m_categoryModel->getCategoriesByType(1);
    allCategories += m_categoryModel->getCategoriesByType(2);
    for (const Category& cat : allCategories) {
        QString displayName = m_categoryModel->getFullCategoryName(cat.id);
        m_filterCategoryCombo->addItem(displayName, cat.id);
    }
    m_filterCategoryCombo->setMinimumWidth(140);
    m_filterCategoryCombo->setFixedHeight(32);

    m_filterAccountCombo = new QComboBox();
    m_filterAccountCombo->addItem(QStringLiteral("全部账户"), -1);
    QList<Account> filterAccounts = m_accountModel->getAllAccounts();
    for (const Account& acc : filterAccounts) {
        m_filterAccountCombo->addItem(acc.name, acc.id);
    }
    m_filterAccountCombo->setMinimumWidth(120);
    m_filterAccountCombo->setFixedHeight(32);

    m_filterStartDate = new QDateEdit();
    m_filterStartDate->setDisplayFormat("yyyy-MM-dd");
    m_filterStartDate->setCalendarPopup(true);
    m_filterStartDate->setFixedWidth(130);
    m_filterStartDate->setFixedHeight(32);

    m_filterEndDate = new QDateEdit();
    m_filterEndDate->setDisplayFormat("yyyy-MM-dd");
    m_filterEndDate->setCalendarPopup(true);
    m_filterEndDate->setFixedWidth(130);
    m_filterEndDate->setFixedHeight(32);

    m_filterBtn = new QPushButton(QStringLiteral("筛选"));
    m_filterBtn->setFixedHeight(32);
    m_clearFilterBtn = new QPushButton(QStringLiteral("清除"));
    m_clearFilterBtn->setObjectName("secondaryBtn");
    m_clearFilterBtn->setFixedHeight(32);
    m_importBtn = new QPushButton(QStringLiteral("导入CSV"));
    m_importBtn->setObjectName("importBtn");
    m_importBtn->setFixedHeight(32);

    filterLayout->addWidget(m_filterCategoryCombo);
    filterLayout->addWidget(m_filterAccountCombo);
    filterLayout->addWidget(m_filterStartDate);
    filterLayout->addWidget(m_filterEndDate);
    filterLayout->addStretch();
    filterLayout->addWidget(m_filterBtn);
    filterLayout->addWidget(m_clearFilterBtn);
    filterLayout->addWidget(m_importBtn);

    filterCardLayout->addWidget(filterSectionLabel);
    filterCardLayout->addLayout(filterLayout);

    m_tableView = new QTableView();
    m_tableView->setModel(m_controller.getModel());
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->setShowGrid(false);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_tableView->setColumnWidth(0, 100);
    m_tableView->setColumnWidth(1, 120);
    m_tableView->setColumnWidth(2, 80);
    m_tableView->setColumnWidth(3, 60);
    m_tableView->setMinimumHeight(200);

    mainLayout->addLayout(summaryLayout);
    mainLayout->addWidget(inputCard);
    mainLayout->addWidget(filterCard);
    mainLayout->addWidget(m_tableView, 1);

    connect(m_addBtn, &QPushButton::clicked, this, &TransactionView::onAddClicked);
    connect(m_editBtn, &QPushButton::clicked, this, &TransactionView::onEditClicked);
    connect(m_deleteBtn, &QPushButton::clicked, this, &TransactionView::onDeleteClicked);
    connect(m_tableView, &QTableView::doubleClicked, this, &TransactionView::onTableDoubleClicked);
    connect(m_toggleCategoryBtn, &QPushButton::clicked, this, &TransactionView::onToggleCategoryPanel);
    connect(m_categoryGrid, &CategoryGridWidget::categorySelected, this, &TransactionView::onCategorySelected);
    connect(m_filterBtn, &QPushButton::clicked, this, &TransactionView::onFilterClicked);
    connect(m_clearFilterBtn, &QPushButton::clicked, this, &TransactionView::onClearFilterClicked);
    connect(m_importBtn, &QPushButton::clicked, this, &TransactionView::onImportClicked);

    updateSummary();
}

void TransactionView::updateSummary() {
    double income = m_controller.getTotalIncome();
    double expense = m_controller.getTotalExpense();
    double balance = income - expense;

    m_accountModel->refresh();
    double totalAssets = m_accountModel->getTotalAssets();

    m_incomeLabel->setText(QStringLiteral("¥ %1").arg(income, 0, 'f', 2));
    m_expenseLabel->setText(QStringLiteral("¥ %1").arg(expense, 0, 'f', 2));
    m_balanceLabel->setText(QStringLiteral("¥ %1").arg(balance, 0, 'f', 2));
    m_assetsLabel->setText(QStringLiteral("¥ %1").arg(totalAssets, 0, 'f', 2));
}

void TransactionView::refreshData() {
    m_controller.refresh();
    m_categoryModel->refresh();
    m_categoryGrid->refresh();
    updateSummary();
}

int TransactionView::getSelectedCategoryId() const {
    return m_categoryGrid->selectedCategoryId();
}

void TransactionView::onToggleCategoryPanel() {
    bool visible = !m_categoryGrid->isVisible();
    m_categoryGrid->setVisible(visible);
    if (visible) {
        m_toggleCategoryBtn->setText(QStringLiteral("选择分类 ▲"));
    } else {
        m_toggleCategoryBtn->setText(QStringLiteral("选择分类 ▼"));
    }
}

void TransactionView::onCategorySelected(int categoryId) {
    QString fullName = m_categoryModel->getFullCategoryName(categoryId);
    m_selectedCategoryLabel->setText(fullName);
    m_selectedCategoryLabel->setObjectName("categoryToggleBtn");
    style()->unpolish(m_selectedCategoryLabel);
    style()->polish(m_selectedCategoryLabel);
}

void TransactionView::onAddClicked() {
    double amount = m_amountEdit->text().toDouble();
    if (amount <= 0) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请输入有效的金额"));
        return;
    }

    int categoryId = getSelectedCategoryId();
    if (categoryId < 0) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请选择分类"));
        return;
    }

    Category cat = m_categoryModel->getCategoryById(categoryId);
    if (cat.type == static_cast<int>(CategoryType::TRANSFER) && cat.name == TransferCategory::TRANSFER_OUT) {
        amount = -amount;
    }

    int accountId = m_accountCombo->currentData().toInt();
    QDate date = m_dateEdit->date();
    QString note = m_noteEdit->text().trimmed();

    if (m_controller.addTransaction(categoryId, accountId, amount, date, note)) {
        m_amountEdit->clear();
        m_noteEdit->clear();
        updateSummary();
    } else {
        QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("添加失败"));
    }
}

void TransactionView::onEditClicked() {
    double amount = m_amountEdit->text().toDouble();
    if (amount <= 0) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请输入有效的金额"));
        return;
    }

    int categoryId = getSelectedCategoryId();
    Category cat = m_categoryModel->getCategoryById(categoryId);
    if (cat.type == static_cast<int>(CategoryType::TRANSFER) && cat.name == TransferCategory::TRANSFER_OUT) {
        amount = -amount;
    }

    int accountId = m_accountCombo->currentData().toInt();
    QDate date = m_dateEdit->date();
    QString note = m_noteEdit->text().trimmed();

    if (m_controller.updateTransaction(m_editId, categoryId, accountId, amount, date, note)) {
        m_amountEdit->clear();
        m_noteEdit->clear();
        m_editBtn->setEnabled(false);
        m_deleteBtn->setEnabled(false);
        m_editId = -1;
        updateSummary();
    } else {
        QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("修改失败"));
    }
}

void TransactionView::onDeleteClicked() {
    if (QMessageBox::question(this, QStringLiteral("确认"), QStringLiteral("确定要删除该记录吗？")) == QMessageBox::Yes) {
        bool success = false;
        TransactionModel* model = m_controller.getModel();
        for (int i = 0; i < model->rowCount(); ++i) {
            QModelIndex idx = model->index(i, 0);
            if (idx.data(Qt::UserRole).toInt() == m_editId) {
                int row = idx.row();
                Transaction t = model->getTransactions()[row];
                if (t.transferId > 0) {
                    success = m_controller.deleteTransactionWithTransferPair(m_editId);
                } else {
                    success = m_controller.deleteTransaction(m_editId);
                }
                break;
            }
        }
        if (!success) {
            success = m_controller.deleteTransaction(m_editId);
        }
        if (success) {
            m_amountEdit->clear();
            m_noteEdit->clear();
            m_editBtn->setEnabled(false);
            m_deleteBtn->setEnabled(false);
            m_editId = -1;
            updateSummary();
        } else {
            QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("删除失败"));
        }
    }
}

void TransactionView::onTableDoubleClicked(const QModelIndex& index) {
    if (!index.isValid()) return;

    m_editId = index.sibling(index.row(), 0).data(Qt::UserRole).toInt();
    QString categoryName = index.sibling(index.row(), 1).data().toString();
    QString accountName = index.sibling(index.row(), 2).data().toString();
    QString typeStr = index.sibling(index.row(), 3).data().toString();
    QString amountStr = index.sibling(index.row(), 4).data().toString();
    QString dateStr = index.sibling(index.row(), 0).data().toString();
    QString note = index.sibling(index.row(), 5).data().toString();

    int targetCategoryId = -1;

    for (int i = 0; i < m_controller.getModel()->rowCount(); ++i) {
        QModelIndex idx = m_controller.getModel()->index(i, 0);
        if (idx.data(Qt::UserRole).toInt() == m_editId) {
            targetCategoryId = idx.sibling(i, 1).data(Qt::UserRole).toInt();
            break;
        }
    }

    if (targetCategoryId < 0) {
        int searchType = 0;
        if (typeStr == QStringLiteral("收入")) searchType = static_cast<int>(CategoryType::INCOME);
        else if (typeStr == TransferCategory::TRANSFER) searchType = static_cast<int>(CategoryType::TRANSFER);
        for (const Category& c : m_categoryModel->getCategoriesByType(searchType)) {
            if (m_categoryModel->getFullCategoryName(c.id) == categoryName || c.name == categoryName) {
                targetCategoryId = c.id;
                break;
            }
        }
    }

    if (targetCategoryId >= 0) {
        m_categoryGrid->setSelectedCategory(targetCategoryId);
        onCategorySelected(targetCategoryId);
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
    updateSummary();
}

void TransactionView::onClearFilterClicked() {
    m_filterCategoryCombo->setCurrentIndex(0);
    m_filterAccountCombo->setCurrentIndex(0);
    m_filterStartDate->setDate(QDate());
    m_filterEndDate->setDate(QDate());
    m_controller.clearFilters();
    updateSummary();
}

void TransactionView::onImportClicked() {
    QString filePath = QFileDialog::getOpenFileName(this,
        QStringLiteral("选择CSV文件"), "", "CSV Files (*.csv)");
    if (filePath.isEmpty()) {
        return;
    }

    if (CsvExporter::importTransactions(filePath)) {
        m_controller.refresh();
        m_categoryModel->refresh();
        m_categoryGrid->refresh();
        updateSummary();
        QMessageBox::information(this, QStringLiteral("成功"), QStringLiteral("导入成功"));
    } else {
        QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("导入失败"));
    }
}

TransactionView::~TransactionView() {}

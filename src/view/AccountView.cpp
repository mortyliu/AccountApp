#include "AccountView.h"
#include "../model/AccountModel.h"
#include "../controller/AccountController.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>
#include <QMessageBox>
#include <QLabel>
#include <QFrame>
#include <QHeaderView>

AccountView::AccountView(QWidget* parent) : QWidget(parent), m_editId(-1) {
    setupUI();
}

void AccountView::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(12);

    QFrame* inputCard = new QFrame();
    inputCard->setObjectName("cardFrame");
    QVBoxLayout* inputCardLayout = new QVBoxLayout(inputCard);
    inputCardLayout->setContentsMargins(16, 12, 16, 12);
    inputCardLayout->setSpacing(8);

    QLabel* sectionLabel = new QLabel(QStringLiteral("新增账户"));
    sectionLabel->setObjectName("sectionLabel");

    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputLayout->setSpacing(8);

    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText(QStringLiteral("账户名称"));

    m_addBtn = new QPushButton(QStringLiteral("添加"));
    m_editBtn = new QPushButton(QStringLiteral("修改"));
    m_editBtn->setObjectName("secondaryBtn");
    m_editBtn->setEnabled(false);
    m_deleteBtn = new QPushButton(QStringLiteral("删除"));
    m_deleteBtn->setObjectName("dangerBtn");
    m_deleteBtn->setEnabled(false);

    inputLayout->addWidget(m_nameEdit, 1);
    inputLayout->addWidget(m_addBtn);
    inputLayout->addWidget(m_editBtn);
    inputLayout->addWidget(m_deleteBtn);

    inputCardLayout->addWidget(sectionLabel);
    inputCardLayout->addLayout(inputLayout);

    m_tableView = new QTableView();
    m_tableView->setModel(m_controller.getModel());
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->setShowGrid(false);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->setColumnWidth(0, 60);
    m_tableView->setColumnWidth(1, 120);
    m_tableView->setColumnWidth(2, 100);
    m_tableView->setColumnWidth(3, 100);
    m_tableView->setColumnWidth(4, 100);

    mainLayout->addWidget(inputCard);
    mainLayout->addWidget(m_tableView, 1);

    connect(m_addBtn, &QPushButton::clicked, this, &AccountView::onAddClicked);
    connect(m_editBtn, &QPushButton::clicked, this, &AccountView::onEditClicked);
    connect(m_deleteBtn, &QPushButton::clicked, this, &AccountView::onDeleteClicked);
    connect(m_tableView, &QTableView::doubleClicked, this, &AccountView::onTableDoubleClicked);
}

void AccountView::onAddClicked() {
    QString name = m_nameEdit->text().trimmed();

    if (name.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请输入账户名称"));
        return;
    }

    if (m_controller.addAccount(name)) {
        m_nameEdit->clear();
    } else {
        QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("添加失败"));
    }
}

void AccountView::onEditClicked() {
    QString name = m_nameEdit->text().trimmed();

    if (name.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请输入账户名称"));
        return;
    }

    if (m_controller.updateAccount(m_editId, name, "")) {
        m_nameEdit->clear();
        m_editBtn->setEnabled(false);
        m_deleteBtn->setEnabled(false);
        m_editId = -1;
    } else {
        QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("修改失败"));
    }
}

void AccountView::onDeleteClicked() {
    if (QMessageBox::question(this, QStringLiteral("确认"), QStringLiteral("确定要删除该账户吗？")) == QMessageBox::Yes) {
        if (m_controller.deleteAccount(m_editId)) {
            m_nameEdit->clear();
            m_editBtn->setEnabled(false);
            m_deleteBtn->setEnabled(false);
            m_editId = -1;
        } else {
            QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("删除失败，该账户可能正在被使用"));
        }
    }
}

void AccountView::onTableDoubleClicked(const QModelIndex& index) {
    if (index.isValid()) {
        m_editId = index.sibling(index.row(), 0).data().toInt();
        QString name = index.sibling(index.row(), 1).data().toString();

        m_nameEdit->setText(name);
        m_editBtn->setEnabled(true);
        m_deleteBtn->setEnabled(true);
    }
}

AccountView::~AccountView() {}

#include "CategoryView.h"
#include "../model/CategoryModel.h"
#include "../controller/CategoryController.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTableView>
#include <QMessageBox>

CategoryView::CategoryView(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void CategoryView::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* inputLayout = new QHBoxLayout();
    
    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText(QString::fromUtf8("分类名称"));
    
    m_typeCombo = new QComboBox();
    m_typeCombo->addItem(QString::fromUtf8("支出"), 0);
    m_typeCombo->addItem(QString::fromUtf8("收入"), 1);
    
    m_addBtn = new QPushButton(QString::fromUtf8("添加"));
    m_editBtn = new QPushButton(QString::fromUtf8("修改"));
    m_editBtn->setEnabled(false);
    m_deleteBtn = new QPushButton(QString::fromUtf8("删除"));
    m_deleteBtn->setEnabled(false);

    inputLayout->addWidget(m_nameEdit);
    inputLayout->addWidget(m_typeCombo);
    inputLayout->addWidget(m_addBtn);
    inputLayout->addWidget(m_editBtn);
    inputLayout->addWidget(m_deleteBtn);

    m_tableView = new QTableView();
    m_tableView->setModel(m_controller.getModel());
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setColumnWidth(0, 50);
    m_tableView->setColumnWidth(1, 150);
    m_tableView->setColumnWidth(2, 80);

    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(m_tableView);

    connect(m_addBtn, &QPushButton::clicked, this, &CategoryView::onAddClicked);
    connect(m_editBtn, &QPushButton::clicked, this, &CategoryView::onEditClicked);
    connect(m_deleteBtn, &QPushButton::clicked, this, &CategoryView::onDeleteClicked);
    connect(m_tableView, &QTableView::doubleClicked, this, &CategoryView::onTableDoubleClicked);
}

void CategoryView::onAddClicked() {
    QString name = m_nameEdit->text().trimmed();
    int type = m_typeCombo->currentData().toInt();

    if (name.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("警告"), QString::fromUtf8("请输入分类名称"));
        return;
    }

    if (m_controller.addCategory(name, type)) {
        m_nameEdit->clear();
        QMessageBox::information(this, QString::fromUtf8("成功"), QString::fromUtf8("添加成功"));
    } else {
        QMessageBox::warning(this, QString::fromUtf8("失败"), QString::fromUtf8("添加失败"));
    }
}

void CategoryView::onEditClicked() {
    QString name = m_nameEdit->text().trimmed();
    int type = m_typeCombo->currentData().toInt();

    if (name.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("警告"), QString::fromUtf8("请输入分类名称"));
        return;
    }

    if (m_controller.updateCategory(m_editId, name, type)) {
        m_nameEdit->clear();
        m_editBtn->setEnabled(false);
        m_deleteBtn->setEnabled(false);
        m_editId = -1;
        QMessageBox::information(this, QString::fromUtf8("成功"), QString::fromUtf8("修改成功"));
    } else {
        QMessageBox::warning(this, QString::fromUtf8("失败"), QString::fromUtf8("修改失败"));
    }
}

void CategoryView::onDeleteClicked() {
    if (QMessageBox::question(this, QString::fromUtf8("确认"), QString::fromUtf8("确定要删除该分类吗？")) == QMessageBox::Yes) {
        if (m_controller.deleteCategory(m_editId)) {
            m_nameEdit->clear();
            m_editBtn->setEnabled(false);
            m_deleteBtn->setEnabled(false);
            m_editId = -1;
            QMessageBox::information(this, QString::fromUtf8("成功"), QString::fromUtf8("删除成功"));
        } else {
            QMessageBox::warning(this, QString::fromUtf8("失败"), QString::fromUtf8("删除失败，该分类可能正在被使用"));
        }
    }
}

void CategoryView::onTableDoubleClicked(const QModelIndex& index) {
    if (index.isValid()) {
        m_editId = index.sibling(index.row(), 0).data().toInt();
        QString name = index.sibling(index.row(), 1).data().toString();
        QString type = index.sibling(index.row(), 2).data().toString();
        
        m_nameEdit->setText(name);
        m_typeCombo->setCurrentIndex(type == QString::fromUtf8("收入") ? 1 : 0);
        m_editBtn->setEnabled(true);
        m_deleteBtn->setEnabled(true);
    }
}

CategoryView::~CategoryView() {}

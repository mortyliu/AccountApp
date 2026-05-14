#include "CategoryView.h"
#include "IconSelectDialog.h"
#include "../model/CategoryModel.h"
#include "../model/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include <QFrame>
#include <QSvgRenderer>
#include <QPainter>
#include <QPixmap>

CategoryView::CategoryView(QWidget* parent) : QWidget(parent), m_editId(-1) {
    setupUI();
}

void CategoryView::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(12);

    m_categoryGrid = new CategoryGridWidget();
    m_categoryGrid->setCategoryModel(m_controller.getModel());
    m_categoryGrid->setFixedHeight(360);
    mainLayout->addWidget(m_categoryGrid, 1);

    QFrame* inputCard = new QFrame();
    inputCard->setObjectName("cardFrame");
    QVBoxLayout* inputCardLayout = new QVBoxLayout(inputCard);
    inputCardLayout->setContentsMargins(16, 12, 16, 12);
    inputCardLayout->setSpacing(8);

    QLabel* sectionLabel = new QLabel(QStringLiteral("编辑分类"));
    sectionLabel->setObjectName("sectionLabel");

    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputLayout->setSpacing(8);

    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText(QStringLiteral("分类名称"));
    m_nameEdit->setFixedHeight(36);

    m_typeCombo = new QComboBox();
    m_typeCombo->addItem(QStringLiteral("支出"), 0);
    m_typeCombo->addItem(QStringLiteral("收入"), 1);
    m_typeCombo->addItem(QStringLiteral("转账"), 2);
    m_typeCombo->setFixedWidth(90);
    m_typeCombo->setFixedHeight(36);

    m_parentCombo = new QComboBox();
    m_parentCombo->addItem(QStringLiteral("无（一级分类）"), -1);
    m_parentCombo->setMinimumWidth(140);
    m_parentCombo->setFixedHeight(36);

    m_iconPreviewLabel = new QLabel();
    m_iconPreviewLabel->setFixedSize(36, 36);
    m_iconPreviewLabel->setAlignment(Qt::AlignCenter);
    m_iconPreviewLabel->setObjectName("iconPreviewLabel");
    m_iconPreviewLabel->setText(QStringLiteral("📁"));

    m_iconSelectBtn = new QPushButton(QStringLiteral("选择图标"));
    m_iconSelectBtn->setObjectName("iconSelectBtn");
    m_iconSelectBtn->setFixedHeight(36);

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

    inputLayout->addWidget(m_nameEdit, 1);
    inputLayout->addWidget(m_typeCombo);
    inputLayout->addWidget(m_parentCombo);
    inputLayout->addWidget(m_iconPreviewLabel);
    inputLayout->addWidget(m_iconSelectBtn);
    inputLayout->addWidget(m_addBtn);
    inputLayout->addWidget(m_editBtn);
    inputLayout->addWidget(m_deleteBtn);

    inputCardLayout->addWidget(sectionLabel);
    inputCardLayout->addLayout(inputLayout);

    mainLayout->addWidget(inputCard);

    connect(m_addBtn, &QPushButton::clicked, this, &CategoryView::onAddClicked);
    connect(m_editBtn, &QPushButton::clicked, this, &CategoryView::onEditClicked);
    connect(m_deleteBtn, &QPushButton::clicked, this, &CategoryView::onDeleteClicked);
    connect(m_categoryGrid, &CategoryGridWidget::categorySelected, this, &CategoryView::onCategorySelected);
    connect(m_typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CategoryView::onTypeChanged);
    connect(m_iconSelectBtn, &QPushButton::clicked, this, &CategoryView::onIconSelectClicked);

    updateParentCombo();
}

void CategoryView::updateParentCombo() {
    m_parentCombo->clear();
    m_parentCombo->addItem(QStringLiteral("无（一级分类）"), -1);

    int type = m_typeCombo->currentData().toInt();
    CategoryModel* model = m_controller.getModel();
    QList<Category> topCategories = model->getTopLevelCategories(type);
    for (const Category& cat : topCategories) {
        if (cat.id != m_editId) {
            m_parentCombo->addItem(cat.name, cat.id);
        }
    }
}

void CategoryView::clearInput() {
    m_nameEdit->clear();
    m_editId = -1;
    m_currentIcon = "";
    m_iconPreviewLabel->setText(QStringLiteral("📁"));
    m_editBtn->setEnabled(false);
    m_deleteBtn->setEnabled(false);
    m_categoryGrid->clearSelection();
}

void CategoryView::onIconSelectClicked() {
    IconSelectDialog dialog(m_currentIcon, this);
    if (dialog.exec() == QDialog::Accepted) {
        m_currentIcon = dialog.selectedIcon();
        if (!m_currentIcon.isEmpty()) {
            QString iconPath = QStringLiteral(":/icons/") + m_currentIcon;
            QSvgRenderer renderer(iconPath);
            if (renderer.isValid()) {
                QPixmap pixmap(32, 32);
                pixmap.fill(Qt::transparent);
                QPainter painter(&pixmap);
                renderer.render(&painter, QRectF(0, 0, 32, 32));
                painter.end();
                m_iconPreviewLabel->setPixmap(pixmap);
            } else {
                m_iconPreviewLabel->setText(QStringLiteral("📁"));
            }
        } else {
            m_iconPreviewLabel->setText(QStringLiteral("📁"));
        }
    }
}

void CategoryView::onTypeChanged(int index) {
    Q_UNUSED(index);
    updateParentCombo();
}

void CategoryView::onCategorySelected(int categoryId) {
    if (categoryId < 0) return;

    m_editId = categoryId;
    CategoryModel* model = m_controller.getModel();
    Category cat = model->getCategoryById(categoryId);
    if (cat.id < 0) return;

    m_nameEdit->setText(cat.name);
    m_typeCombo->setCurrentIndex(cat.type);
    updateParentCombo();

    if (cat.parentId < 0) {
        m_parentCombo->setCurrentIndex(0);
    } else {
        int idx = m_parentCombo->findData(cat.parentId);
        if (idx >= 0) {
            m_parentCombo->setCurrentIndex(idx);
        }
    }

    m_currentIcon = cat.icon;
    if (!m_currentIcon.isEmpty()) {
        QString iconPath = QStringLiteral(":/icons/") + m_currentIcon;
        QSvgRenderer renderer(iconPath);
        if (renderer.isValid()) {
            QPixmap pixmap(32, 32);
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            renderer.render(&painter, QRectF(0, 0, 32, 32));
            painter.end();
            m_iconPreviewLabel->setPixmap(pixmap);
        } else {
            m_iconPreviewLabel->setText(QStringLiteral("📁"));
        }
    } else {
        m_iconPreviewLabel->setText(QStringLiteral("📁"));
    }

    m_editBtn->setEnabled(true);
    m_deleteBtn->setEnabled(true);
}

void CategoryView::onAddClicked() {
    QString name = m_nameEdit->text().trimmed();
    int type = m_typeCombo->currentData().toInt();
    int parentId = m_parentCombo->currentData().toInt();

    if (name.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请输入分类名称"));
        return;
    }

    if (m_controller.addCategory(name, type, parentId, m_currentIcon)) {
        m_nameEdit->clear();
        m_currentIcon = "";
        m_iconPreviewLabel->setText(QStringLiteral("📁"));
        updateParentCombo();
        m_categoryGrid->refresh();
    } else {
        QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("添加失败"));
    }
}

void CategoryView::onEditClicked() {
    QString name = m_nameEdit->text().trimmed();
    int type = m_typeCombo->currentData().toInt();
    int parentId = m_parentCombo->currentData().toInt();

    if (name.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请输入分类名称"));
        return;
    }

    if (parentId == m_editId) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("不能将自己设为上级分类"));
        return;
    }

    if (m_controller.updateCategory(m_editId, name, type, parentId, m_currentIcon)) {
        clearInput();
        updateParentCombo();
        m_categoryGrid->refresh();
    } else {
        QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("修改失败"));
    }
}

void CategoryView::onDeleteClicked() {
    if (DatabaseManager::instance().hasSubCategories(m_editId)) {
        QMessageBox::warning(this, QStringLiteral("无法删除"),
            QStringLiteral("该分类下还有二级分类，请先删除所有二级分类"));
        return;
    }

    if (DatabaseManager::instance().hasTransactions(m_editId)) {
        QMessageBox::warning(this, QStringLiteral("无法删除"),
            QStringLiteral("该分类下还有交易记录，无法删除"));
        return;
    }

    if (QMessageBox::question(this, QStringLiteral("确认"),
        QStringLiteral("确定要删除该分类吗？")) == QMessageBox::Yes) {
        if (m_controller.deleteCategory(m_editId)) {
            clearInput();
            updateParentCombo();
            m_categoryGrid->refresh();
        } else {
            QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("删除失败"));
        }
    }
}

CategoryView::~CategoryView() {}

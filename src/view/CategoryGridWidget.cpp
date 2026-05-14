#include "CategoryGridWidget.h"
#include <QMouseEvent>
#include <QEnterEvent>
#include <QPainter>
#include <QPixmap>
#include <QScrollArea>
#include <QScrollBar>
#include <QSvgRenderer>
#include <QStyle>

CategoryItem::CategoryItem(int categoryId, const QString& name, const QString& iconPath, QWidget* parent)
    : QFrame(parent), m_categoryId(categoryId) {
    setFixedSize(76, 80);
    setCursor(Qt::PointingHandCursor);
    setObjectName("categoryItem");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 6, 4, 4);
    layout->setSpacing(2);
    layout->setAlignment(Qt::AlignCenter);

    m_iconLabel = new QLabel();
    m_iconLabel->setFixedSize(36, 36);
    m_iconLabel->setAlignment(Qt::AlignCenter);

    bool iconLoaded = false;
    if (!iconPath.isEmpty()) {
        QSvgRenderer renderer(iconPath);
        if (renderer.isValid()) {
            QPixmap pixmap(32, 32);
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            renderer.render(&painter, QRectF(0, 0, 32, 32));
            painter.end();
            m_iconLabel->setPixmap(pixmap);
            iconLoaded = true;
        }
    }

    if (!iconLoaded) {
        m_iconLabel->setText(QStringLiteral("📁"));
        m_iconLabel->setStyleSheet(QStringLiteral("font-size:20px;"));
    }

    m_iconLabel->setObjectName("categoryIcon");

    m_nameLabel = new QLabel(name);
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setWordWrap(true);
    m_nameLabel->setObjectName("categoryName");

    layout->addWidget(m_iconLabel);
    layout->addWidget(m_nameLabel);
}

void CategoryItem::setSelected(bool selected) {
    m_selected = selected;
    if (selected) {
        setObjectName("categoryItemSelected");
    } else {
        setObjectName("categoryItem");
    }
    style()->unpolish(this);
    style()->polish(this);
}

void CategoryItem::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked(m_categoryId);
    }
    QFrame::mousePressEvent(event);
}

void CategoryItem::enterEvent(QEnterEvent* event) {
    if (!m_selected) {
        setStyleSheet(QStringLiteral("#categoryItem { border-radius: 4px; background-color: #F5F5F5; }"));
    }
    QFrame::enterEvent(event);
}

void CategoryItem::leaveEvent(QEvent* event) {
    if (!m_selected) {
        setStyleSheet(QStringLiteral("#categoryItem { border: none; border-radius: 4px; background-color: transparent; }"));
    }
    QFrame::leaveEvent(event);
}

CategoryGridWidget::CategoryGridWidget(QWidget* parent)
    : QWidget(parent), m_model(nullptr), m_currentType(0), m_selectedParentId(-1), m_finalSelectedId(-1) {
    setupUI();
}

void CategoryGridWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setupHeader();

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setObjectName("categoryScroll");
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget* scrollContent = new QWidget();
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setContentsMargins(16, 12, 16, 16);
    scrollLayout->setSpacing(8);

    m_gridContainer = new QWidget();
    m_gridLayout = new QGridLayout(m_gridContainer);
    m_gridLayout->setSpacing(8);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);

    m_subAreaFrame = new QFrame();
    m_subAreaFrame->setObjectName("subCategoryArea");
    m_subAreaFrame->setVisible(false);
    QVBoxLayout* subLayout = new QVBoxLayout(m_subAreaFrame);
    subLayout->setContentsMargins(16, 12, 16, 12);
    subLayout->setSpacing(8);

    m_subGridLayout = new QGridLayout();
    m_subGridLayout->setSpacing(8);
    m_subGridLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* subGridContainer = new QWidget();
    subGridContainer->setLayout(m_subGridLayout);
    subLayout->addWidget(subGridContainer);

    scrollLayout->addWidget(m_gridContainer);
    scrollLayout->addWidget(m_subAreaFrame);
    scrollLayout->addStretch();

    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea, 1);
}

void CategoryGridWidget::setupHeader() {
    m_headerFrame = new QFrame();
    m_headerFrame->setFixedHeight(48);
    m_headerFrame->setObjectName("categoryHeader");

    QHBoxLayout* headerLayout = new QHBoxLayout(m_headerFrame);
    headerLayout->setContentsMargins(12, 0, 12, 0);
    headerLayout->setSpacing(0);

    m_closeBtn = new QPushButton(QStringLiteral("×"));
    m_closeBtn->setFixedSize(32, 32);
    m_closeBtn->setObjectName("closeTabBtn");
    connect(m_closeBtn, &QPushButton::clicked, this, &CategoryGridWidget::onCloseClicked);

    m_tabLayout = new QHBoxLayout();
    m_tabLayout->setSpacing(0);

    QStringList tabNames = {QStringLiteral("支出"), QStringLiteral("收入"), QStringLiteral("转账")};
    for (int i = 0; i < tabNames.size(); ++i) {
        QPushButton* tab = new QPushButton(tabNames[i]);
        tab->setCheckable(true);
        tab->setObjectName("categoryTab");
        tab->setProperty("tabIndex", i);
        tab->setMinimumWidth(60);
        tab->setFixedHeight(44);
        connect(tab, &QPushButton::toggled, this, [this, i](bool checked) {
            if (checked) onTabChanged(i);
        });
        m_tabButtons.append(tab);
        m_tabLayout->addWidget(tab);
    }

    m_tabButtons[0]->blockSignals(true);
    m_tabButtons[0]->setChecked(true);
    m_tabButtons[0]->blockSignals(false);

    m_addBtn = new QPushButton(QStringLiteral("+"));
    m_addBtn->setFixedSize(32, 32);
    m_addBtn->setObjectName("addCategoryBtn");
    connect(m_addBtn, &QPushButton::clicked, this, &CategoryGridWidget::onAddClicked);

    headerLayout->addWidget(m_closeBtn);
    headerLayout->addStretch();
    headerLayout->addLayout(m_tabLayout);
    headerLayout->addStretch();
    headerLayout->addWidget(m_addBtn);
}

void CategoryGridWidget::setCategoryModel(CategoryModel* model) {
    m_model = model;
    refresh();
}

void CategoryGridWidget::refresh() {
    updateGrid();
}

void CategoryGridWidget::updateGrid() {
    while (m_gridLayout->count()) {
        QLayoutItem* item = m_gridLayout->takeAt(0);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    if (!m_model) {
        return;
    }

    QList<Category> categories = m_model->getTopLevelCategories(m_currentType);
    int col = 0;
    int row = 0;
    int maxCols = 6;

    for (const Category& cat : categories) {
        QString iconPath;
        if (!cat.icon.isEmpty()) {
            iconPath = QStringLiteral(":/icons/") + cat.icon;
        } else {
            iconPath = QStringLiteral(":/icons/") + getDefaultIcon(cat.name) + QStringLiteral(".svg");
        }
        CategoryItem* item = new CategoryItem(cat.id, cat.name, iconPath, this);
        connect(item, &CategoryItem::clicked, this, &CategoryGridWidget::onItemClicked);

        if (cat.id == m_selectedParentId) {
            item->setSelected(true);
        }

        m_gridLayout->addWidget(item, row, col);
        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
}

void CategoryGridWidget::showSubCategories(int parentId) {
    while (m_subGridLayout->count()) {
        QLayoutItem* item = m_subGridLayout->takeAt(0);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    QList<Category> subs = m_model->getSubCategories(parentId);
    if (subs.isEmpty()) {
        m_subAreaFrame->setVisible(false);
        m_finalSelectedId = parentId;
        return;
    }

    int col = 0;
    for (const Category& sub : subs) {
        QString iconPath;
        if (!sub.icon.isEmpty()) {
            iconPath = QStringLiteral(":/icons/") + sub.icon;
        } else {
            iconPath = QStringLiteral(":/icons/") + getDefaultIcon(sub.name) + QStringLiteral(".svg");
        }
        CategoryItem* item = new CategoryItem(sub.id, sub.name, iconPath, this);
        connect(item, &CategoryItem::clicked, this, [this, sub](int id) {
            m_finalSelectedId = id;
            emit categorySelected(id);
        });

        if (sub.id == m_finalSelectedId) {
            item->setSelected(true);
        }

        m_subGridLayout->addWidget(item, 0, col);
        col++;
    }

    m_subAreaFrame->setVisible(true);
}

void CategoryGridWidget::onTabChanged(int type) {
    for (int i = 0; i < m_tabButtons.size(); ++i) {
        m_tabButtons[i]->blockSignals(true);
        m_tabButtons[i]->setChecked(i == type);
        m_tabButtons[i]->blockSignals(false);
    }

    m_currentType = type;
    m_selectedParentId = -1;
    m_finalSelectedId = -1;
    m_subAreaFrame->setVisible(false);
    updateGrid();
}

void CategoryGridWidget::onItemClicked(int categoryId) {
    for (int i = 0; i < m_gridLayout->count(); ++i) {
        QLayoutItem* layoutItem = m_gridLayout->itemAt(i);
        if (layoutItem && layoutItem->widget()) {
            CategoryItem* item = static_cast<CategoryItem*>(layoutItem->widget());
            item->setSelected(item->categoryId() == categoryId);
        }
    }

    for (int i = 0; i < m_subGridLayout->count(); ++i) {
        QLayoutItem* layoutItem = m_subGridLayout->itemAt(i);
        if (layoutItem && layoutItem->widget()) {
            CategoryItem* item = static_cast<CategoryItem*>(layoutItem->widget());
            item->setSelected(false);
        }
    }

    m_selectedParentId = categoryId;
    showSubCategories(categoryId);
    emit categorySelected(categoryId);
}

void CategoryGridWidget::onCloseClicked() {
    hide();
}

void CategoryGridWidget::onAddClicked() {
    emit addCategoryRequested();
}

int CategoryGridWidget::selectedCategoryId() const {
    return m_finalSelectedId >= 0 ? m_finalSelectedId : m_selectedParentId;
}

void CategoryGridWidget::setSelectedCategory(int categoryId) {
    if (!m_model) return;

    Category cat = m_model->getCategoryById(categoryId);
    if (cat.id < 0) return;

    if (cat.type != m_currentType) {
        onTabChanged(cat.type);
    }

    if (cat.parentId >= 0) {
        m_selectedParentId = cat.parentId;
        m_finalSelectedId = categoryId;

        for (int i = 0; i < m_gridLayout->count(); ++i) {
            QLayoutItem* layoutItem = m_gridLayout->itemAt(i);
            if (layoutItem && layoutItem->widget()) {
                CategoryItem* item = static_cast<CategoryItem*>(layoutItem->widget());
                item->setSelected(item->categoryId() == cat.parentId);
            }
        }

        showSubCategories(cat.parentId);

        for (int i = 0; i < m_subGridLayout->count(); ++i) {
            QLayoutItem* layoutItem = m_subGridLayout->itemAt(i);
            if (layoutItem && layoutItem->widget()) {
                CategoryItem* item = static_cast<CategoryItem*>(layoutItem->widget());
                item->setSelected(item->categoryId() == categoryId);
            }
        }
    } else {
        m_selectedParentId = categoryId;
        m_finalSelectedId = -1;

        for (int i = 0; i < m_gridLayout->count(); ++i) {
            QLayoutItem* layoutItem = m_gridLayout->itemAt(i);
            if (layoutItem && layoutItem->widget()) {
                CategoryItem* item = static_cast<CategoryItem*>(layoutItem->widget());
                item->setSelected(item->categoryId() == categoryId);
            }
        }

        showSubCategories(categoryId);
    }
}

void CategoryGridWidget::clearSelection() {
    m_selectedParentId = -1;
    m_finalSelectedId = -1;
    m_subAreaFrame->setVisible(false);

    for (int i = 0; i < m_gridLayout->count(); ++i) {
        QLayoutItem* layoutItem = m_gridLayout->itemAt(i);
        if (layoutItem && layoutItem->widget()) {
            CategoryItem* item = static_cast<CategoryItem*>(layoutItem->widget());
            item->setSelected(false);
        }
    }
}

QString CategoryGridWidget::getDefaultIcon(const QString& categoryName) {
    static QMap<QString, QString> iconMap = {
        {QStringLiteral("转账"), QStringLiteral("transfer")},
        {QStringLiteral("餐饮"), QStringLiteral("cat_food")},
        {QStringLiteral("交通"), QStringLiteral("cat_transport")},
        {QStringLiteral("购物"), QStringLiteral("cat_shopping")},
        {QStringLiteral("娱乐"), QStringLiteral("cat_entertainment")},
        {QStringLiteral("医疗"), QStringLiteral("cat_medical")},
        {QStringLiteral("教育"), QStringLiteral("cat_education")},
        {QStringLiteral("住房"), QStringLiteral("cat_housing")},
        {QStringLiteral("日常"), QStringLiteral("cat_daily")},
        {QStringLiteral("其他支出"), QStringLiteral("cat_other_expense")},
        {QStringLiteral("工资"), QStringLiteral("cat_salary")},
        {QStringLiteral("奖金"), QStringLiteral("cat_bonus")},
        {QStringLiteral("投资收益"), QStringLiteral("cat_investment")},
        {QStringLiteral("其他收入"), QStringLiteral("cat_other_income")},
        {QStringLiteral("早餐"), QStringLiteral("cat_breakfast")},
        {QStringLiteral("午餐"), QStringLiteral("cat_lunch")},
        {QStringLiteral("晚餐"), QStringLiteral("cat_dinner")},
        {QStringLiteral("零食饮料"), QStringLiteral("cat_snack")},
        {QStringLiteral("公交地铁"), QStringLiteral("cat_subway")},
        {QStringLiteral("打车"), QStringLiteral("cat_taxi")},
        {QStringLiteral("加油"), QStringLiteral("cat_gasoline")},
        {QStringLiteral("衣物"), QStringLiteral("cat_clothing")},
        {QStringLiteral("数码"), QStringLiteral("cat_digital")},
        {QStringLiteral("日用"), QStringLiteral("cat_daily_goods")},
        {QStringLiteral("游戏"), QStringLiteral("cat_game")},
        {QStringLiteral("电影"), QStringLiteral("cat_movie")},
        {QStringLiteral("旅游"), QStringLiteral("cat_travel")},
        {QStringLiteral("门诊"), QStringLiteral("cat_clinic")},
        {QStringLiteral("药品"), QStringLiteral("cat_medicine")},
        {QStringLiteral("书籍"), QStringLiteral("cat_book")},
        {QStringLiteral("培训"), QStringLiteral("cat_training")},
        {QStringLiteral("房租"), QStringLiteral("cat_rent")},
        {QStringLiteral("物业"), QStringLiteral("cat_property")},
        {QStringLiteral("水电"), QStringLiteral("cat_utilities")},
        {QStringLiteral("话费"), QStringLiteral("cat_phone_bill")},
        {QStringLiteral("理发"), QStringLiteral("cat_haircut")},
    };

    return iconMap.value(categoryName, QStringLiteral("app"));
}

#include "IconSelectDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QSvgRenderer>
#include <QPainter>
#include <QPixmap>
#include <QFrame>
#include <QEvent>
#include <QStyle>

IconSelectDialog::IconSelectDialog(const QString& currentIcon, QWidget* parent)
    : QDialog(parent), m_selectedIcon(currentIcon) {
    setWindowTitle(QStringLiteral("选择图标"));
    setMinimumSize(480, 400);
    setupUI();
}

void IconSelectDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    QLabel* hintLabel = new QLabel(QStringLiteral("点击选择图标："));
    hintLabel->setObjectName("sectionLabel");

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget* gridContainer = new QWidget();
    m_gridLayout = new QGridLayout(gridContainer);
    m_gridLayout->setSpacing(8);
    m_gridLayout->setContentsMargins(8, 8, 8, 8);

    QStringList iconNames = {
        QStringLiteral("transfer.svg"),
        QStringLiteral("food.svg"),
        QStringLiteral("shopping.svg"),
        QStringLiteral("transport.svg"),
        QStringLiteral("salary.svg"),
        QStringLiteral("redpack.svg"),
        QStringLiteral("medical.svg"),
        QStringLiteral("education.svg"),
        QStringLiteral("entertainment.svg"),
        QStringLiteral("housing.svg"),
        QStringLiteral("water.svg"),
        QStringLiteral("phone.svg"),
        QStringLiteral("finance.svg"),
        QStringLiteral("cash.svg"),
        QStringLiteral("bank.svg"),
        QStringLiteral("insurance.svg"),
        QStringLiteral("travel.svg"),
        QStringLiteral("clothes.svg"),
        QStringLiteral("beauty.svg"),
        QStringLiteral("other.svg")
    };

    int col = 0;
    int row = 0;
    int maxCols = 5;

    for (const QString& iconName : iconNames) {
        QFrame* iconFrame = new QFrame();
        iconFrame->setFixedSize(72, 80);
        iconFrame->setObjectName("iconGridItem");
        iconFrame->setCursor(Qt::PointingHandCursor);

        QVBoxLayout* frameLayout = new QVBoxLayout(iconFrame);
        frameLayout->setContentsMargins(4, 4, 4, 4);
        frameLayout->setSpacing(2);
        frameLayout->setAlignment(Qt::AlignCenter);

        QLabel* iconLabel = new QLabel();
        iconLabel->setFixedSize(32, 32);
        iconLabel->setAlignment(Qt::AlignCenter);

        QString iconPath = QStringLiteral(":/icons/") + iconName;
        QSvgRenderer renderer(iconPath);
        if (renderer.isValid()) {
            QPixmap pixmap(32, 32);
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            renderer.render(&painter, QRectF(0, 0, 32, 32));
            painter.end();
            iconLabel->setPixmap(pixmap);
        }

        QString displayName = iconName;
        displayName.remove(".svg");
        QLabel* nameLabel = new QLabel(displayName);
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->setObjectName("categoryName");

        frameLayout->addWidget(iconLabel, 0, Qt::AlignCenter);
        frameLayout->addWidget(nameLabel, 0, Qt::AlignCenter);

        if (iconName == m_selectedIcon) {
            iconFrame->setObjectName("iconGridItemSelected");
            m_selectedIcon = iconName;
        }

        iconFrame->setProperty("iconName", iconName);
        iconFrame->installEventFilter(this);

        m_gridLayout->addWidget(iconFrame, row, col);
        m_iconWidgets.append(iconFrame);

        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }

    scrollArea->setWidget(gridContainer);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    QPushButton* cancelBtn = new QPushButton(QStringLiteral("取消"));
    cancelBtn->setObjectName("secondaryBtn");
    QPushButton* confirmBtn = new QPushButton(QStringLiteral("确认"));

    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(confirmBtn);

    mainLayout->addWidget(hintLabel);
    mainLayout->addWidget(scrollArea, 1);
    mainLayout->addLayout(btnLayout);

    connect(confirmBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

bool IconSelectDialog::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QFrame* frame = qobject_cast<QFrame*>(watched);
        if (frame && frame->property("iconName").isValid()) {
            QString iconName = frame->property("iconName").toString();
            onIconClicked(iconName);
            return true;
        }
    }
    return QDialog::eventFilter(watched, event);
}

void IconSelectDialog::onIconClicked(const QString& iconName) {
    for (QWidget* w : m_iconWidgets) {
        if (w->property("iconName").toString() == iconName) {
            w->setObjectName("iconGridItemSelected");
        } else {
            w->setObjectName("iconGridItem");
        }
        w->style()->unpolish(w);
        w->style()->polish(w);
    }
    m_selectedIcon = iconName;
}

QString IconSelectDialog::selectedIcon() const {
    return m_selectedIcon;
}

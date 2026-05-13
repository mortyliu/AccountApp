#include "MainWindow.h"
#include "../view/TransactionView.h"
#include "../view/CategoryView.h"
#include "../view/AccountView.h"
#include "../view/StatisticsView.h"
#include "../view/TransferView.h"
#include "../utils/CsvExporter.h"
#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QSpacerItem>
#include <QScrollArea>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    if (!DatabaseManager::instance().openDatabase()) {
        QMessageBox::critical(nullptr, QStringLiteral("错误"), QStringLiteral("无法打开数据库"));
        QApplication::exit(1);
    }

    setupUI();
    setupMenu();
}

QPushButton* MainWindow::createNavButton(const QString& text, int index) {
    QPushButton* btn = new QPushButton(text);
    btn->setObjectName("navBtn");
    btn->setCheckable(true);
    btn->setFlat(true);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setMinimumHeight(44);
    connect(btn, &QPushButton::clicked, this, [this, index]() { onNavClicked(index); });
    m_navButtons.append(btn);
    return btn;
}

void MainWindow::setupUI() {
    setWindowTitle(QStringLiteral("记账软件"));
    setMinimumSize(1000, 650);

    QWidget* centralWidget = new QWidget();
    centralWidget->setObjectName("centralWidget");
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QWidget* sidebar = new QWidget();
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(200);
    QVBoxLayout* sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);

    QLabel* title = new QLabel(QStringLiteral("AccountApp"));
    title->setObjectName("sidebarTitle");
    QLabel* subtitle = new QLabel(QStringLiteral("个人记账管理"));
    subtitle->setObjectName("sidebarSubtitle");

    sidebarLayout->addWidget(title);
    sidebarLayout->addWidget(subtitle);
    sidebarLayout->addSpacing(10);

    sidebarLayout->addWidget(createNavButton(QStringLiteral("  收支记录"), 0));
    sidebarLayout->addWidget(createNavButton(QStringLiteral("  分类管理"), 1));
    sidebarLayout->addWidget(createNavButton(QStringLiteral("  账户管理"), 2));
    sidebarLayout->addWidget(createNavButton(QStringLiteral("  账户转账"), 3));
    sidebarLayout->addWidget(createNavButton(QStringLiteral("  统计分析"), 4));

    sidebarLayout->addStretch();

    m_navButtons[0]->setChecked(true);

    QWidget* contentArea = new QWidget();
    contentArea->setObjectName("centralWidget");
    QVBoxLayout* contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(24, 20, 24, 20);
    contentLayout->setSpacing(16);

    m_pageTitle = new QLabel(QStringLiteral("收支记录"));
    m_pageTitle->setObjectName("pageTitle");

    m_stackedWidget = new QStackedWidget();

    m_transactionView = new TransactionView();
    m_categoryView = new CategoryView();
    m_accountView = new AccountView();
    m_transferView = new TransferView();
    m_statisticsView = new StatisticsView();

    m_stackedWidget->addWidget(m_transactionView);
    m_stackedWidget->addWidget(m_categoryView);
    m_stackedWidget->addWidget(m_accountView);
    m_stackedWidget->addWidget(m_transferView);
    m_stackedWidget->addWidget(m_statisticsView);

    contentLayout->addWidget(m_pageTitle);
    contentLayout->addWidget(m_stackedWidget, 1);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentArea, 1);

    setCentralWidget(centralWidget);
}

void MainWindow::setupMenu() {
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QMenu* fileMenu = menuBar->addMenu(QStringLiteral("文件"));

    QAction* exportAction = new QAction(QStringLiteral("导出数据"), this);
    QAction* importAction = new QAction(QStringLiteral("导入数据"), this);

    fileMenu->addAction(exportAction);
    fileMenu->addAction(importAction);

    QMenu* helpMenu = menuBar->addMenu(QStringLiteral("帮助"));
    QAction* aboutAction = new QAction(QStringLiteral("关于"), this);
    helpMenu->addAction(aboutAction);

    connect(exportAction, &QAction::triggered, this, &MainWindow::onExportData);
    connect(importAction, &QAction::triggered, this, &MainWindow::onImportData);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
}

void MainWindow::onNavClicked(int index) {
    for (int i = 0; i < m_navButtons.size(); ++i) {
        m_navButtons[i]->setChecked(i == index);
    }
    m_stackedWidget->setCurrentIndex(index);

    QStringList titles = {
        QStringLiteral("收支记录"),
        QStringLiteral("分类管理"),
        QStringLiteral("账户管理"),
        QStringLiteral("账户转账"),
        QStringLiteral("统计分析")
    };
    m_pageTitle->setText(titles.value(index));
}

void MainWindow::onExportData() {
    QString filePath = QFileDialog::getSaveFileName(this, QStringLiteral("导出数据"), "", QStringLiteral("CSV文件 (*.csv)"));
    if (!filePath.isEmpty()) {
        if (CsvExporter::exportTransactions(filePath)) {
            QMessageBox::information(this, QStringLiteral("成功"), QStringLiteral("数据导出成功"));
        } else {
            QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("数据导出失败"));
        }
    }
}

void MainWindow::onImportData() {
    QString filePath = QFileDialog::getOpenFileName(this, QStringLiteral("导入数据"), "", QStringLiteral("CSV文件 (*.csv)"));
    if (!filePath.isEmpty()) {
        if (CsvExporter::importTransactions(filePath)) {
            m_transactionView->refreshData();
            QMessageBox::information(this, QStringLiteral("成功"), QStringLiteral("数据导入成功"));
        } else {
            QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("数据导入失败"));
        }
    }
}

void MainWindow::onAbout() {
    QMessageBox::about(this, QStringLiteral("关于"), QStringLiteral("记账软件 v1.0\n\n一款简洁的个人记账工具，支持收支记录、分类管理、统计分析等功能。"));
}

MainWindow::~MainWindow() {}

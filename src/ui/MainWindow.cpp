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
#include <QTabWidget>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    if (!DatabaseManager::instance().openDatabase()) {
        QMessageBox::critical(nullptr, QStringLiteral("错误"), QStringLiteral("无法打开数据库"));
        QApplication::exit(1);
    }

    setupUI();
    setupMenu();
}

void MainWindow::setupUI() {
    setWindowTitle(QStringLiteral("记账软件"));
    setMinimumSize(800, 600);

    m_tabWidget = new QTabWidget(this);
    
    m_transactionView = new TransactionView();
    m_categoryView = new CategoryView();
    m_accountView = new AccountView();
    m_transferView = new TransferView();
    m_statisticsView = new StatisticsView();

    m_tabWidget->addTab(m_transactionView, QStringLiteral("收支记录"));
    m_tabWidget->addTab(m_categoryView, QStringLiteral("分类管理"));
    m_tabWidget->addTab(m_accountView, QStringLiteral("账户管理"));
    m_tabWidget->addTab(m_transferView, QStringLiteral("账户转账"));
    m_tabWidget->addTab(m_statisticsView, QStringLiteral("统计分析"));

    setCentralWidget(m_tabWidget);
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

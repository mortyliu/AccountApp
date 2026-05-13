#include "MainWindow.h"
#include "../view/TransactionView.h"
#include "../view/CategoryView.h"
#include "../view/AccountView.h"
#include "../view/StatisticsView.h"
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
        QMessageBox::critical(nullptr, QString::fromUtf8("错误"), QString::fromUtf8("无法打开数据库"));
        QApplication::exit(1);
    }

    setupUI();
    setupMenu();
}

void MainWindow::setupUI() {
    setWindowTitle(QString::fromUtf8("记账软件"));
    setMinimumSize(800, 600);

    m_tabWidget = new QTabWidget(this);
    
    m_transactionView = new TransactionView();
    m_categoryView = new CategoryView();
    m_accountView = new AccountView();
    m_statisticsView = new StatisticsView();

    m_tabWidget->addTab(m_transactionView, QString::fromUtf8("收支记录"));
    m_tabWidget->addTab(m_categoryView, QString::fromUtf8("分类管理"));
    m_tabWidget->addTab(m_accountView, QString::fromUtf8("账户管理"));
    m_tabWidget->addTab(m_statisticsView, QString::fromUtf8("统计分析"));

    setCentralWidget(m_tabWidget);
}

void MainWindow::setupMenu() {
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QMenu* fileMenu = menuBar->addMenu(QString::fromUtf8("文件"));
    
    QAction* exportAction = new QAction(QString::fromUtf8("导出数据"), this);
    QAction* importAction = new QAction(QString::fromUtf8("导入数据"), this);
    
    fileMenu->addAction(exportAction);
    fileMenu->addAction(importAction);

    QMenu* helpMenu = menuBar->addMenu(QString::fromUtf8("帮助"));
    QAction* aboutAction = new QAction(QString::fromUtf8("关于"), this);
    helpMenu->addAction(aboutAction);

    connect(exportAction, &QAction::triggered, this, &MainWindow::onExportData);
    connect(importAction, &QAction::triggered, this, &MainWindow::onImportData);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
}

void MainWindow::onExportData() {
    QString filePath = QFileDialog::getSaveFileName(this, QString::fromUtf8("导出数据"), "", QString::fromUtf8("CSV文件 (*.csv)"));
    if (!filePath.isEmpty()) {
        if (CsvExporter::exportTransactions(filePath)) {
            QMessageBox::information(this, QString::fromUtf8("成功"), QString::fromUtf8("数据导出成功"));
        } else {
            QMessageBox::warning(this, QString::fromUtf8("失败"), QString::fromUtf8("数据导出失败"));
        }
    }
}

void MainWindow::onImportData() {
    QString filePath = QFileDialog::getOpenFileName(this, QString::fromUtf8("导入数据"), "", QString::fromUtf8("CSV文件 (*.csv)"));
    if (!filePath.isEmpty()) {
        if (CsvExporter::importTransactions(filePath)) {
            QMessageBox::information(this, QString::fromUtf8("成功"), QString::fromUtf8("数据导入成功"));
        } else {
            QMessageBox::warning(this, QString::fromUtf8("失败"), QString::fromUtf8("数据导入失败"));
        }
    }
}

void MainWindow::onAbout() {
    QMessageBox::about(this, QString::fromUtf8("关于"), QString::fromUtf8("记账软件 v1.0\n\n一款简洁的个人记账工具，支持收支记录、分类管理、统计分析等功能。"));
}

MainWindow::~MainWindow() {}


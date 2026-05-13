#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QMenuBar>
#include <QAction>
#include "../view/CategoryView.h"
#include "../view/AccountView.h"
#include "../view/TransactionView.h"
#include "../view/StatisticsView.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onExportData();
    void onImportData();
    void onAbout();

private:
    void setupUI();
    void setupMenu();

    QTabWidget* m_tabWidget;
    CategoryView* m_categoryView;
    AccountView* m_accountView;
    TransactionView* m_transactionView;
    StatisticsView* m_statisticsView;
};

#endif // MAINWINDOW_H

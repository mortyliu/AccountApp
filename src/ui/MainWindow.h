#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QList>
#include "../view/CategoryView.h"
#include "../view/AccountView.h"
#include "../view/TransactionView.h"
#include "../view/StatisticsView.h"
#include "../view/TransferView.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onNavClicked(int index);
    void onExportData();
    void onImportData();
    void onAbout();

private:
    void setupUI();
    void setupMenu();
    void setupSidebar();
    QPushButton* createNavButton(const QString& text, int index);

    QStackedWidget* m_stackedWidget;
    QList<QPushButton*> m_navButtons;

    CategoryView* m_categoryView;
    AccountView* m_accountView;
    TransactionView* m_transactionView;
    TransferView* m_transferView;
    StatisticsView* m_statisticsView;

    QLabel* m_pageTitle;
};

#endif // MAINWINDOW_H

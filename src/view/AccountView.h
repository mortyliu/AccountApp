#ifndef ACCOUNTVIEW_H
#define ACCOUNTVIEW_H

#include <QWidget>
#include <QTableView>
#include <QLineEdit>
#include <QPushButton>
#include "../controller/AccountController.h"

class AccountView : public QWidget {
    Q_OBJECT
public:
    explicit AccountView(QWidget* parent = nullptr);
    ~AccountView();

private slots:
    void onAddClicked();
    void onEditClicked();
    void onDeleteClicked();
    void onTableDoubleClicked(const QModelIndex& index);

private:
    void setupUI();

    AccountController m_controller;
    QTableView* m_tableView;
    QLineEdit* m_nameEdit;
    QPushButton* m_addBtn;
    QPushButton* m_editBtn;
    QPushButton* m_deleteBtn;

    int m_editId;
};

#endif // ACCOUNTVIEW_H

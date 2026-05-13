#ifndef TRANSFERVIEW_H
#define TRANSFERVIEW_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QDateEdit>

class TransferView : public QWidget {
    Q_OBJECT
public:
    TransferView(QWidget* parent = nullptr);
    ~TransferView();

private slots:
    void onTransferClicked();

private:
    void setupUI();
    void loadAccounts();

    QComboBox* m_fromAccountCombo;
    QComboBox* m_toAccountCombo;
    QLineEdit* m_amountEdit;
    QDateEdit* m_dateEdit;
    QLineEdit* m_noteEdit;
    QPushButton* m_transferBtn;
};

#endif

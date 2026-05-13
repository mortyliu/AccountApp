#include "TransferView.h"
#include "../controller/TransactionController.h"
#include "../model/DatabaseManager.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QSqlQuery>

TransferView::TransferView(QWidget* parent) : QWidget(parent) {
    setupUI();
    loadAccounts();
}

TransferView::~TransferView() {}

void TransferView::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* inputLayout = new QHBoxLayout();

    m_fromAccountCombo = new QComboBox();
    m_toAccountCombo = new QComboBox();

    m_amountEdit = new QLineEdit();
    m_amountEdit->setPlaceholderText(QString::fromUtf8("金额"));
    m_amountEdit->setValidator(new QDoubleValidator(0.01, 9999999.99, 2, this));

    m_dateEdit = new QDateEdit(QDate::currentDate());
    m_dateEdit->setDisplayFormat("yyyy-MM-dd");
    m_dateEdit->setCalendarPopup(true);

    m_noteEdit = new QLineEdit();
    m_noteEdit->setPlaceholderText(QString::fromUtf8("备注"));

    m_transferBtn = new QPushButton(QString::fromUtf8("确认转账"));

    inputLayout->addWidget(new QLabel(QString::fromUtf8("账户1:")));
    inputLayout->addWidget(m_fromAccountCombo);
    inputLayout->addWidget(new QLabel(QString::fromUtf8("→")));
    inputLayout->addWidget(new QLabel(QString::fromUtf8("账户2:")));
    inputLayout->addWidget(m_toAccountCombo);
    inputLayout->addWidget(m_amountEdit);
    inputLayout->addWidget(m_dateEdit);
    inputLayout->addWidget(m_noteEdit);
    inputLayout->addWidget(m_transferBtn);

    mainLayout->addLayout(inputLayout);

    setLayout(mainLayout);
}

void TransferView::loadAccounts() {
    m_fromAccountCombo->clear();
    m_toAccountCombo->clear();

    QSqlQuery query = DatabaseManager::instance().getAllAccounts();
    while (query.next()) {
        int id = query.value(0).toInt();
        QString name = query.value(1).toString();
        m_fromAccountCombo->addItem(name, id);
        m_toAccountCombo->addItem(name, id);
    }
}

void TransferView::onTransferClicked() {
    bool ok;
    double amount = m_amountEdit->text().toDouble(&ok);
    if (!ok || amount <= 0) {
        QMessageBox::warning(this, QString::fromUtf8("警告"), QString::fromUtf8("请输入有效的金额"));
        return;
    }

    int fromAccountId = m_fromAccountCombo->currentData().toInt();
    int toAccountId = m_toAccountCombo->currentData().toInt();

    if (fromAccountId == toAccountId) {
        QMessageBox::warning(this, QString::fromUtf8("警告"), QString::fromUtf8("请选择不同的账户"));
        return;
    }

    QString note = m_noteEdit->text().trimmed();
    if (note.isEmpty()) {
        note = QString::fromUtf8("转账");
    }

    bool success = TransactionController::transfer(fromAccountId, toAccountId, amount, m_dateEdit->date(), note);

    if (success) {
        QMessageBox::information(this, QString::fromUtf8("成功"), QString::fromUtf8("转账成功"));
        m_amountEdit->clear();
        m_noteEdit->clear();
    } else {
        QMessageBox::warning(this, QString::fromUtf8("失败"), QString::fromUtf8("转账失败，转出账户余额不足"));
    }
}

#include "TransferView.h"
#include "../controller/TransactionController.h"
#include "../model/DatabaseManager.h"
#include "../model/Constants.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QSqlQuery>
#include <QFrame>

TransferView::TransferView(QWidget* parent) : QWidget(parent) {
    setupUI();
    loadAccounts();
}

TransferView::~TransferView() {}

void TransferView::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(12);

    QFrame* transferCard = new QFrame();
    transferCard->setObjectName("cardFrame");
    QVBoxLayout* cardLayout = new QVBoxLayout(transferCard);
    cardLayout->setContentsMargins(24, 20, 24, 24);
    cardLayout->setSpacing(16);

    QLabel* sectionLabel = new QLabel(QStringLiteral("账户转账"));
    sectionLabel->setObjectName("sectionLabel");

    QGridLayout* formLayout = new QGridLayout();
    formLayout->setSpacing(12);
    formLayout->setColumnMinimumWidth(0, 80);

    QLabel* fromLabel = new QLabel(QStringLiteral("转出账户"));
    m_fromAccountCombo = new QComboBox();
    m_fromAccountCombo->setMinimumWidth(200);

    QLabel* toLabel = new QLabel(QStringLiteral("转入账户"));
    m_toAccountCombo = new QComboBox();
    m_toAccountCombo->setMinimumWidth(200);

    QLabel* amountLabel = new QLabel(QStringLiteral("转账金额"));
    m_amountEdit = new QLineEdit();
    m_amountEdit->setPlaceholderText(QStringLiteral("请输入金额"));
    m_amountEdit->setValidator(new QDoubleValidator(0.01, 9999999.99, 2, this));

    QLabel* dateLabel = new QLabel(QStringLiteral("转账日期"));
    m_dateEdit = new QDateEdit(QDate::currentDate());
    m_dateEdit->setDisplayFormat("yyyy-MM-dd");
    m_dateEdit->setCalendarPopup(true);

    QLabel* noteLabel = new QLabel(QStringLiteral("备注"));
    m_noteEdit = new QLineEdit();
    m_noteEdit->setPlaceholderText(QStringLiteral("可选备注"));

    formLayout->addWidget(fromLabel, 0, 0);
    formLayout->addWidget(m_fromAccountCombo, 0, 1);
    formLayout->addWidget(toLabel, 1, 0);
    formLayout->addWidget(m_toAccountCombo, 1, 1);
    formLayout->addWidget(amountLabel, 2, 0);
    formLayout->addWidget(m_amountEdit, 2, 1);
    formLayout->addWidget(dateLabel, 3, 0);
    formLayout->addWidget(m_dateEdit, 3, 1);
    formLayout->addWidget(noteLabel, 4, 0);
    formLayout->addWidget(m_noteEdit, 4, 1);

    m_transferBtn = new QPushButton(QStringLiteral("确认转账"));
    m_transferBtn->setMinimumHeight(40);

    cardLayout->addWidget(sectionLabel);
    cardLayout->addLayout(formLayout);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(m_transferBtn, 0, Qt::AlignLeft);

    mainLayout->addWidget(transferCard);
    mainLayout->addStretch();

    connect(m_transferBtn, &QPushButton::clicked, this, &TransferView::onTransferClicked);
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
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请输入有效的金额"));
        return;
    }

    int fromAccountId = m_fromAccountCombo->currentData().toInt();
    int toAccountId = m_toAccountCombo->currentData().toInt();

    if (fromAccountId == toAccountId) {
        QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请选择不同的账户"));
        return;
    }

    QString note = m_noteEdit->text().trimmed();
    if (note.isEmpty()) {
        note = TransferCategory::TRANSFER;
    }

    bool success = TransactionController::transfer(fromAccountId, toAccountId, amount, m_dateEdit->date(), note);

    if (success) {
        QMessageBox::information(this, QStringLiteral("成功"), QStringLiteral("转账成功"));
        m_amountEdit->clear();
        m_noteEdit->clear();
    } else {
        QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("转账失败，转出账户余额不足"));
    }
}

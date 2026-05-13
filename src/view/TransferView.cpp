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
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    QLabel* titleLabel = new QLabel(u8"账户转账");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QWidget* accountWidget = new QWidget();
    QHBoxLayout* accountLayout = new QHBoxLayout(accountWidget);
    accountLayout->setSpacing(20);

    QWidget* fromAccountWidget = new QWidget();
    QVBoxLayout* fromLayout = new QVBoxLayout(fromAccountWidget);
    QLabel* fromLabel = new QLabel(u8"账户1 (转出)");
    m_fromAccountCombo = new QComboBox();
    fromLayout->addWidget(fromLabel);
    fromLayout->addWidget(m_fromAccountCombo);
    accountLayout->addWidget(fromAccountWidget);

    QLabel* arrowLabel = new QLabel(u8"→");
    arrowLabel->setFont(QFont("Arial", 20, QFont::Bold));
    arrowLabel->setAlignment(Qt::AlignCenter);
    accountLayout->addWidget(arrowLabel);

    QWidget* toAccountWidget = new QWidget();
    QVBoxLayout* toLayout = new QVBoxLayout(toAccountWidget);
    QLabel* toLabel = new QLabel(u8"账户2 (转入)");
    m_toAccountCombo = new QComboBox();
    toLayout->addWidget(toLabel);
    toLayout->addWidget(m_toAccountCombo);
    accountLayout->addWidget(toAccountWidget);

    mainLayout->addWidget(accountWidget);

    QWidget* amountWidget = new QWidget();
    QHBoxLayout* amountLayout = new QHBoxLayout(amountWidget);
    amountLayout->setSpacing(10);

    QLabel* amountLabel = new QLabel(u8"转账金额");
    m_amountEdit = new QLineEdit();
    m_amountEdit->setPlaceholderText(u8"请输入金额");
    m_amountEdit->setValidator(new QDoubleValidator(0.01, 9999999.99, 2, this));

    amountLayout->addWidget(amountLabel);
    amountLayout->addWidget(m_amountEdit);
    mainLayout->addWidget(amountWidget);

    QWidget* dateWidget = new QWidget();
    QHBoxLayout* dateLayout = new QHBoxLayout(dateWidget);
    dateLayout->setSpacing(10);

    dateLayout->addWidget(new QLabel(u8"日期"));
    m_dateEdit = new QDateEdit(QDate::currentDate());
    m_dateEdit->setDisplayFormat("yyyy-MM-dd");
    m_dateEdit->setCalendarPopup(true);
    dateLayout->addWidget(m_dateEdit);
    mainLayout->addWidget(dateWidget);

    QWidget* noteWidget = new QWidget();
    QHBoxLayout* noteLayout = new QHBoxLayout(noteWidget);
    noteLayout->setSpacing(10);

    QLabel* noteLabel = new QLabel(u8"备注");
    m_noteEdit = new QLineEdit();
    m_noteEdit->setPlaceholderText(u8"可选");

    noteLayout->addWidget(noteLabel);
    noteLayout->addWidget(m_noteEdit);
    mainLayout->addWidget(noteWidget);

    m_transferBtn = new QPushButton(u8"确认转账");
    m_transferBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 10px 20px; border: none; border-radius: 5px; font-size: 14px; } QPushButton:hover { background-color: #45a049; }");
    connect(m_transferBtn, &QPushButton::clicked, this, &TransferView::onTransferClicked);
    mainLayout->addWidget(m_transferBtn, 0, Qt::AlignCenter);

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
        QMessageBox::warning(this, u8"警告", u8"请输入有效的金额");
        return;
    }

    int fromAccountId = m_fromAccountCombo->currentData().toInt();
    int toAccountId = m_toAccountCombo->currentData().toInt();

    if (fromAccountId == toAccountId) {
        QMessageBox::warning(this, u8"警告", u8"请选择不同的账户");
        return;
    }

    QString note = m_noteEdit->text().trimmed();
    if (note.isEmpty()) {
        note = u8"转账";
    }

    bool success = TransactionController::transfer(fromAccountId, toAccountId, amount, m_dateEdit->date(), note);

    if (success) {
        QMessageBox::information(this, u8"成功", u8"转账成功");
        m_amountEdit->clear();
        m_noteEdit->clear();
    } else {
        QMessageBox::warning(this, u8"失败", u8"转账失败，转出账户余额不足");
    }
}

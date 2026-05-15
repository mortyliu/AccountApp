#ifndef CONSTANTS_H
#define CONSTANTS_H

enum class CategoryType {
    EXPENSE = 0,
    INCOME = 1,
    TRANSFER = 2
};

namespace TransferCategory {
    const QString TRANSFER = QStringLiteral("转账");
    const QString TRANSFER_IN = QStringLiteral("转入");
    const QString TRANSFER_OUT = QStringLiteral("转出");
}

#endif // CONSTANTS_H
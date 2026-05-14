#ifndef ICONSELECTDIALOG_H
#define ICONSELECTDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QVector>
#include <QString>

class IconSelectDialog : public QDialog {
    Q_OBJECT
public:
    explicit IconSelectDialog(const QString& currentIcon, QWidget* parent = nullptr);

    QString selectedIcon() const;

private slots:
    void onIconClicked(const QString& iconName);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void setupUI();

    QString m_selectedIcon;
    QGridLayout* m_gridLayout;
    QVector<QWidget*> m_iconWidgets;
};

#endif

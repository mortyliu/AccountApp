#include "StatisticsView.h"
#include "../controller/StatisticsController.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QDateEdit>
#include <QChart>
#include <QChartView>
#include <QPieSeries>
#include <QLineSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QLabel>
#include <QFrame>

StatisticsView::StatisticsView(QWidget* parent) : QWidget(parent), m_controller(nullptr) {
    m_controller = new StatisticsController(this);
    setupUI();
}

void StatisticsView::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(12);

    QFrame* controlCard = new QFrame();
    controlCard->setObjectName("cardFrame");
    QVBoxLayout* controlCardLayout = new QVBoxLayout(controlCard);
    controlCardLayout->setContentsMargins(16, 12, 16, 12);
    controlCardLayout->setSpacing(8);

    QLabel* sectionLabel = new QLabel(QStringLiteral("图表设置"));
    sectionLabel->setObjectName("sectionLabel");

    QHBoxLayout* controlLayout = new QHBoxLayout();
    controlLayout->setSpacing(8);

    m_typeCombo = new QComboBox();
    m_typeCombo->addItem(QStringLiteral("支出"), 0);
    m_typeCombo->addItem(QStringLiteral("收入"), 1);
    m_typeCombo->addItem(QStringLiteral("转账"), 2);

    m_chartTypeCombo = new QComboBox();
    m_chartTypeCombo->addItem(QStringLiteral("饼图"), 0);
    m_chartTypeCombo->addItem(QStringLiteral("折线图"), 1);

    m_startDate = new QDateEdit(QDate::currentDate().addMonths(-1));
    m_startDate->setDisplayFormat("yyyy-MM-dd");
    m_startDate->setCalendarPopup(true);

    m_endDate = new QDateEdit(QDate::currentDate());
    m_endDate->setDisplayFormat("yyyy-MM-dd");
    m_endDate->setCalendarPopup(true);

    m_yearCombo = new QComboBox();
    int currentYear = QDate::currentDate().year();
    for (int i = currentYear - 5; i <= currentYear; ++i) {
        m_yearCombo->addItem(QString::number(i), i);
    }
    m_yearCombo->setCurrentIndex(5);

    m_refreshBtn = new QPushButton(QStringLiteral("刷新"));

    controlLayout->addWidget(m_typeCombo);
    controlLayout->addWidget(m_chartTypeCombo);
    controlLayout->addWidget(m_startDate);
    controlLayout->addWidget(m_endDate);
    controlLayout->addWidget(m_yearCombo);
    controlLayout->addStretch();
    controlLayout->addWidget(m_refreshBtn);

    controlCardLayout->addWidget(sectionLabel);
    controlCardLayout->addLayout(controlLayout);

    QFrame* chartCard = new QFrame();
    chartCard->setObjectName("cardFrame");
    QVBoxLayout* chartCardLayout = new QVBoxLayout(chartCard);
    chartCardLayout->setContentsMargins(4, 4, 4, 4);

    m_chart = new QChart();
    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    chartCardLayout->addWidget(m_chartView);

    mainLayout->addWidget(controlCard);
    mainLayout->addWidget(chartCard, 1);

    connect(m_refreshBtn, &QPushButton::clicked, this, &StatisticsView::onRefreshClicked);
    connect(m_chartTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &StatisticsView::onChartTypeChanged);

    onRefreshClicked();
}

void StatisticsView::updatePieChart() {
    m_chart->removeAllSeries();

    int type = m_typeCombo->currentData().toInt();
    QDate start = m_startDate->date();
    QDate end = m_endDate->date();

    QMap<QString, double> stats = m_controller->getCategoryStatistics(start, end, type);

    m_pieSeries = new QPieSeries();
    for (auto it = stats.begin(); it != stats.end(); ++it) {
        m_pieSeries->append(it.key(), it.value());
    }

    m_pieSeries->setLabelsVisible();

    m_chart->addSeries(m_pieSeries);
    QString titleStr;
    if (type == 1) titleStr = QStringLiteral("收入分类统计");
    else if (type == 2) titleStr = QStringLiteral("转账分类统计");
    else titleStr = QStringLiteral("支出分类统计");
    m_chart->setTitle(titleStr);
    m_chart->legend()->setAlignment(Qt::AlignBottom);
}

void StatisticsView::updateLineChart() {
    m_chart->removeAllSeries();
    QList<QAbstractAxis*> axes = m_chart->axes();
    for (QAbstractAxis* axis : axes) {
        m_chart->removeAxis(axis);
    }

    int type = m_typeCombo->currentData().toInt();
    int year = m_yearCombo->currentData().toInt();

    QMap<int, double> stats = m_controller->getMonthlyStatistics(year, type);

    m_lineSeries = new QLineSeries();
    QStringList categories;

    for (int month = 1; month <= 12; ++month) {
        categories << QString::number(month) + QStringLiteral("月");
        m_lineSeries->append(month, stats.value(month, 0));
    }

    m_chart->addSeries(m_lineSeries);

    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    m_chart->addAxis(axisX, Qt::AlignBottom);
    m_lineSeries->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setLabelFormat("%.0f");
    m_chart->addAxis(axisY, Qt::AlignLeft);
    m_lineSeries->attachAxis(axisY);

    QString lineTitle;
    if (type == 1) lineTitle = QString("%1").arg(year) + QStringLiteral("年收入趋势");
    else if (type == 2) lineTitle = QString("%1").arg(year) + QStringLiteral("年转账趋势");
    else lineTitle = QString("%1").arg(year) + QStringLiteral("年支出趋势");
    m_chart->setTitle(lineTitle);
    m_chart->legend()->setVisible(false);
}

void StatisticsView::onRefreshClicked() {
    if (m_chartTypeCombo->currentData().toInt() == 0) {
        updatePieChart();
    } else {
        updateLineChart();
    }
}

void StatisticsView::onChartTypeChanged(int index) {
    onRefreshClicked();
}

StatisticsView::~StatisticsView() {}

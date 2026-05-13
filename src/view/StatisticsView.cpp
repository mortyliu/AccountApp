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

StatisticsView::StatisticsView(QWidget* parent) : QWidget(parent), m_controller(nullptr) {
    m_controller = new StatisticsController(this);
    setupUI();
}

void StatisticsView::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* controlLayout = new QHBoxLayout();
    
    m_typeCombo = new QComboBox();
    m_typeCombo->addItem(QString::fromUtf8("支出"), 0);
    m_typeCombo->addItem(QString::fromUtf8("收入"), 1);
    
    m_chartTypeCombo = new QComboBox();
    m_chartTypeCombo->addItem(QString::fromUtf8("饼图"), 0);
    m_chartTypeCombo->addItem(QString::fromUtf8("折线图"), 1);
    
    m_startDate = new QDateEdit(QDate::currentDate().addMonths(-1));
    m_startDate->setDisplayFormat("yyyy-MM-dd");
    
    m_endDate = new QDateEdit(QDate::currentDate());
    m_endDate->setDisplayFormat("yyyy-MM-dd");
    
    m_yearCombo = new QComboBox();
    int currentYear = QDate::currentDate().year();
    for (int i = currentYear - 5; i <= currentYear; ++i) {
        m_yearCombo->addItem(QString::number(i), i);
    }
    m_yearCombo->setCurrentIndex(5);
    
    m_refreshBtn = new QPushButton(QString::fromUtf8("刷新"));

    controlLayout->addWidget(new QLabel(QString::fromUtf8("类型:")));
    controlLayout->addWidget(m_typeCombo);
    controlLayout->addWidget(new QLabel(QString::fromUtf8("图表:")));
    controlLayout->addWidget(m_chartTypeCombo);
    controlLayout->addWidget(new QLabel(QString::fromUtf8("日期范围:")));
    controlLayout->addWidget(m_startDate);
    controlLayout->addWidget(m_endDate);
    controlLayout->addWidget(new QLabel(QString::fromUtf8("年份:")));
    controlLayout->addWidget(m_yearCombo);
    controlLayout->addWidget(m_refreshBtn);

    m_chart = new QChart();
    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(m_chartView);

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
    m_chart->setTitle(type == 1 ? QString::fromUtf8("收入分类统计") : QString::fromUtf8("支出分类统计"));
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
        categories << QString::number(month) + QString::fromUtf8("月");
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
    
    m_chart->setTitle(type == 1 ? QString("%1").arg(year) + QString::fromUtf8("年收入趋势") : QString("%1").arg(year) + QString::fromUtf8("年支出趋势"));
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

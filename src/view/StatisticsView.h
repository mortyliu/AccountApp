#ifndef STATISTICSVIEW_H
#define STATISTICSVIEW_H

#include <QWidget>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QChartView>
#include <QPieSeries>
#include <QLineSeries>
#include <QChart>
#include "../controller/StatisticsController.h"

class StatisticsView : public QWidget {
    Q_OBJECT
public:
    explicit StatisticsView(QWidget* parent = nullptr);
    ~StatisticsView();

private slots:
    void onRefreshClicked();
    void onChartTypeChanged(int index);

private:
    void setupUI();
    void updatePieChart();
    void updateLineChart();

    StatisticsController* m_controller;
    
    QComboBox* m_typeCombo;
    QComboBox* m_chartTypeCombo;
    QDateEdit* m_startDate;
    QDateEdit* m_endDate;
    QComboBox* m_yearCombo;
    QPushButton* m_refreshBtn;
    
    QChartView* m_chartView;
    QPieSeries* m_pieSeries;
    QLineSeries* m_lineSeries;
    QChart* m_chart;
};

#endif // STATISTICSVIEW_H

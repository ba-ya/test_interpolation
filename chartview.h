#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QChart>
#include <QChartView>
#include <QGraphicsSimpleTextItem>
#include <QLineSeries>
#include <QScatterSeries>
#include <QCategoryAxis>
#include <QGraphicsLayout>
#include <QScatterSeries>

#if (QT_VERSION <= QT_VERSION_CHECK(6,0,0))
QT_CHARTS_USE_NAMESPACE
#endif

class ChartView : public QChartView
{
    Q_OBJECT

public:
    ChartView(QWidget* parent)
        : QChartView(parent)
    {
        setMouseTracking(true);
        setRenderHint(QPainter::Antialiasing);
        setRubberBand(QChartView::NoRubberBand);
        init_charts();
    }

    ~ChartView() {}

public:
    void update_x_axis(int cnt_tick, double start, double end)
    {
        x_start = start;
        x_end = end;
        xtick_count = cnt_tick;
        on_x_update();
    }

    void update_y_axis(int cnt_tick, double start, double end)
    {
        y_start = start;
        y_end = end;
        ytick_count = cnt_tick;
        on_y_update();
    }

    void clear_series() {
        m_series->clear();
        m_scatter->clear();
    }

public slots:
    void recv_points(std::shared_ptr<std::vector<QPointF>> a, bool show_scatter = true)
    {
        QList<QPointF> b(a->begin(), a->end());
        m_series->replace(b);
        if (show_scatter) {
            m_scatter->replace(b);
        } else {
            m_scatter->clear();
        }

        // range
        x_end = a->size() - 1;
        on_x_update();

    }

private:
    void on_x_update()
    {
        for (auto &a : xLabels) {
            xAxis->remove(a);
        }
        xLabels.clear();
        xAxis->setRange(x_start, x_end);
        for (int i = 0; i < xtick_count; i++) {
            auto x = x_start + (x_end - x_start) / (xtick_count - 1) * i;
            auto a = QString::number(x);
            xAxis->append(a, x);
            xLabels.push_back(a);
        }
    }

    void on_y_update()
    {
        for (auto &a : yLabels) {
            yAxis->remove(a);
        }
        yLabels.clear();
        yAxis->setRange(y_start, y_end);
        for (int i = 0; i < ytick_count; i++) {
            auto y = y_start + (y_end - y_start) / (ytick_count - 1) * i;
            auto a = QString::number(y) + "%";
            yAxis->append(a, y);
        }
    }

    void init_charts()
    {
        m_chart = new QChart();
        m_chart->legend()->hide();
        m_chart->layout()->setContentsMargins(0, 0, 0, 0);
        setChart(m_chart);

        xAxis = new QCategoryAxis();
        xAxis->setLabelsPosition(QCategoryAxis::AxisLabelsPosition::AxisLabelsPositionOnValue);
        on_x_update();

        yAxis = new QCategoryAxis();
        yAxis->setLabelsPosition(QCategoryAxis::AxisLabelsPosition::AxisLabelsPositionOnValue);
        on_y_update();

        m_chart->addAxis(xAxis, Qt::AlignBottom);
        m_chart->addAxis(yAxis, Qt::AlignLeft);

        m_series = new QLineSeries();
        m_chart->addSeries(m_series);
        m_series->attachAxis(xAxis);
        m_series->attachAxis(yAxis);

        m_scatter = new QScatterSeries();
        m_chart->addSeries(m_scatter);
        m_scatter->attachAxis(xAxis);
        m_scatter->attachAxis(yAxis);
    }

private:
    QChart* m_chart;
    QLineSeries* m_series;
    QScatterSeries *m_scatter;
    QCategoryAxis* xAxis;
    QCategoryAxis* yAxis;

    double x_start = 0;
    double x_end = 50;
    int xtick_count = 5;
    double y_start = 0;
    double y_end = 100;
    int ytick_count = 5;

    std::vector<QString> xLabels;
    std::vector<QString> yLabels;

};

#endif // CHARTVIEW_H

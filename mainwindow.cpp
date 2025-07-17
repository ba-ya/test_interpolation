#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    // show
    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // edit
    ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // row
    auto cnt_row = CntType;
    ui->table->setRowCount(cnt_row);
    ui->table->setColumnCount(1);
    for (int i = 0; i < CntType; ++i) {
        ui->table->setItem(i, 0, new QTableWidgetItem(get_type_name(i)));
    }
    connect(ui->table, &QTableWidget::itemDoubleClicked, this, [this](QTableWidgetItem *item) {
        ui->lineEdit_name->setText(item->text());
        do_something(item->text());
    });
}

QString MainWindow::get_type_name(int type)
{
    switch(type) {
    case TCG_Old: return "TCG_Old";
    case TCG_New: return "TCG_New";
    default: return "Unkown";
    }
}

void MainWindow::do_something(QString type_name)
{
    auto points = 1024;
    std::vector<double> deps = {300, 500, 700, 900};
    std::vector<double> gains = {10, 75, 20, 95};
    if (type_name == get_type_name(TCG_Old)) {
        tcg_old_version(deps, gains, points);
    } else if (type_name == get_type_name(TCG_New)) {
        tcg_linear_version(deps, gains, points);
    }
}

void MainWindow::tcg_old_version(std::vector<double> deps, std::vector<double> gains, int cnt_out)
{
    std::vector<QPointF> points_1;
    std::vector<QPointF> points_2;

    fill_zero(deps, gains);
    auto cnt = gains.size();

    for (int k = 0; k < cnt - 1; k++) {
        auto delta_points = deps[k + 1] - deps[k];
        auto delta_gain = gains[k + 1] - gains[k];
        double inc_gain = delta_gain / delta_points;
        double curr_gain = gains[k];

        auto init_value = static_cast<int>(deps[k]);
        auto end_value = static_cast<int>(deps[k + 1]);
        for (int j = init_value; j < end_value; j++) {
            points_1.push_back(QPointF(j, gains[k]));
            points_2.push_back(QPointF(j, curr_gain));
            curr_gain += inc_gain;
        }
    }
    auto init_value = static_cast<int>(deps[cnt - 1]);
    for (int j = init_value; j < cnt_out; j++) {
        points_1.push_back(QPointF(j, gains[cnt - 1]));
        points_2.push_back(QPointF(j, gains[cnt - 1]));
    }
    // debug_vector(points_2);
    ui->chart_1->recv_points(std::make_shared<std::vector<QPointF>>(points_1), false);
    ui->chart_2->recv_points(std::make_shared<std::vector<QPointF>>(points_2), false);
}

void MainWindow::tcg_linear_version(std::vector<double> deps, std::vector<double> gains, int cnt_out)
{
    fill_zero(deps, gains);
    std::vector<QPointF> result;
    // make step = 1
    auto cnt_interpolate = deps.back() - deps.front() + 1;
    auto tmp = linear_interpolate(deps, gains, cnt_interpolate);
    result.insert(result.end(), tmp.begin(), tmp.end());
    for (int i = deps.back() + 1; i < cnt_out; ++i) {
        result.push_back(QPointF(i, gains.back()));
    }
    // debug_vector(result);
    ui->chart_2->recv_points(std::make_shared<std::vector<QPointF>>(result), false);
}

std::vector<QPointF> MainWindow::linear_interpolate(std::vector<double> x, std::vector<double> y, int cnt_out)
{
    std::vector<QPointF> result;
    if (x.size() != y.size() || x.size() < 2 || cnt_out <= 0)
        return result;

    double x_min = x.front();
    double x_max = x.back();

    double step = (x_max - x_min) / (cnt_out - 1);

    for (int i = 0; i < cnt_out; ++i) {
        double xi = x_min + i * step;

        // 找到第一个比xi大的数
        auto upper = std::upper_bound(x.begin(), x.end(), xi);
        if (upper == x.end()) {
            result.push_back(QPointF(xi, y.back()));
            continue;
        }
        // 找到左边界的索引值
        int idx = std::distance(x.begin(), upper) - 1;
        if (idx < 0) idx = 0;

        double x0 = x[idx];
        double x1 = x[idx + 1];
        double y0 = y[idx];
        double y1 = y[idx + 1];

        // 线性插值公式
        double yi = y0 + (y1 - y0) * (xi - x0) / (x1 - x0);

        result.push_back(QPointF(xi, yi));
    }
    return result;
}

void MainWindow::fill_zero(std::vector<double> &x, std::vector<double> &y)
{
    if (x.front() <= 0) {
        return;
    }
    x.insert(x.begin(), 0);
    y.insert(y.begin(), 0);
}

void MainWindow::debug_vector(std::vector<QPointF> &vec)
{
    for (int i = 0; i < vec.size(); ++i) {
        qDebug() << i << vec[i];
    }
}

void MainWindow::on_btn_clear_released()
{
    ui->chart_1->clear_series();
    ui->chart_2->clear_series();
}


#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <random>

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
    QStringList funcs = {
        "tcg_old_version",
        "tcg_linear_version",
        "ndt_view_dscan",
        "smp_linkcl",
    };
    // row
    auto cnt_row = funcs.size();
    ui->table->setRowCount(cnt_row);
    ui->table->setColumnCount(1);
    for (int i = 0; i < cnt_row; ++i) {
        ui->table->setItem(i, 0, new QTableWidgetItem(funcs[i]));
    }
    connect(ui->table, &QTableWidget::itemDoubleClicked, this, [this](QTableWidgetItem *item) {
        ui->lineEdit_name->setText(item->text());
        do_something(item->text());
    });
}

void MainWindow::do_something(QString type_name)
{
    auto points = 100;
    std::vector<double> deps = {30, 50, 70, 90};
    std::vector<double> gains = {10, 75, 20, 95};
    if (type_name == "tcg_old_version") {
        tcg_old_version(deps, gains, points);
    } else if (type_name == "tcg_linear_version") {
        tcg_linear_version(deps, gains, points);
    } else if (type_name == "ndt_view_dscan") {
        ndt_view_dscan();
    } else if (type_name == "smp_linkcl") {
        smp_linkcl();
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
    bool is_scatt = ui->cb_scatter->isChecked();
    ui->chart_1->recv_points(std::make_shared<std::vector<QPointF>>(points_1), is_scatt);
    ui->chart_2->recv_points(std::make_shared<std::vector<QPointF>>(points_2), is_scatt);
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
    bool is_scatt = ui->cb_scatter->isChecked();
    ui->chart_2->recv_points(std::make_shared<std::vector<QPointF>>(result), is_scatt);
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

void MainWindow::ndt_view_dscan()
{
    auto cnt_in = 700;
    auto min = 10, max = 75;
    std::vector<int16_t> data_in = get_data_in(min, max, cnt_in, false);
    auto points_1 = to_points(data_in);

    auto cnt_out = 120;
    auto range = 100;
    auto data_out = on_beam_interpolate(0, range, data_in, 0, range, cnt_out - 1);
    auto points_2 = to_points(data_out);

    bool is_scatt = ui->cb_scatter->isChecked();
    ui->chart_1->recv_points(std::make_shared<std::vector<QPointF>>(points_1), is_scatt);
    ui->chart_2->recv_points(std::make_shared<std::vector<QPointF>>(points_2), is_scatt);
}

std::vector<int16_t> MainWindow::on_beam_interpolate(
    double start_in, double end_in, const std::vector<int16_t> &data_in,
    double start_out, double end_out, int cnt_out
    )
{
    std::vector<int16_t> data_out;
    if (data_in.empty()) {
        return data_out;
    }
    if (cnt_out <= 0) {
        return data_out;
    }
    data_out.reserve(cnt_out + 1);
    for (auto i = 0; i < cnt_out + 1; i++) {
        auto x = start_out + (end_out - start_out) * i / cnt_out;
        if (x < start_in || x > end_in) {
            data_out.push_back(0);
            continue;
        }
        auto d = (end_in - start_in) / (data_in.size() - 1);
        int id_1 = (x - start_in) / d;
        int id_2 = id_1 + 1;
        if (id_2 >= static_cast<int>(data_in.size())) {
            data_out.push_back(data_in.back());
            continue;
        }
        auto x1 = start_in + id_1 * d;
        auto x2 = start_in + (id_1 + 1) * d;
        // 标准公式的变形, 加权形式
        // (x - x1) / (x2 - x1) * y1 + (x2 - x) / (x2 - x1) * y2
        auto a = (x - x2) / (x1 - x2) * data_in[id_1] + (x - x1) / (x2 - x1) * data_in[id_2];
        data_out.push_back(a);
    }
    return data_out;
}

void MainWindow::smp_linkcl()
{
    auto cnt_in = 700;
    auto min = 10, max = 75;
    std::vector<int16_t> data_in = get_data_in(min, max, cnt_in);
    auto points_1 = to_points(data_in);

    auto cnt_out = 120;
    double range = 100, wave_speed = 5920;
    int compress = 1;
    range = wave_speed / 1000 * (compress * 10 / 1000.0) * cnt_in;
    auto data_out = on_data_interpolate(range, wave_speed, compress, cnt_out, data_in, cnt_in);
    auto points_2 = to_points(data_out);

    bool is_scatt = ui->cb_scatter->isChecked();
    ui->chart_1->recv_points(std::make_shared<std::vector<QPointF>>(points_1), is_scatt);
    ui->chart_2->recv_points(std::make_shared<std::vector<QPointF>>(points_2), is_scatt);
}

std::vector<int16_t> MainWindow::on_data_interpolate(
    double domain, double wave_speed, int compress,
    int cnt_out,
    std::vector<int16_t> data, int cnt_in)
{
    int delta = 10; // usb, 10ns
    // 实际范围内有多少点，对应插值前的数据：data
    // wave_speed(m/s) -> mm/ms -> mm/ms * (1ms / 1000us) -> mm/us
    // delta(ns) -> ns * 0.001us/ns
    // ohter words, mm/ms->mm/us(小单位->大单位, /1000)
    // ohter words, ns->us(小单位->大单位, /1000)
    auto a = wave_speed / 1000 * (compress * delta / 1000.0); // mm, 2个点的间隔
    auto b = domain / (cnt_out - 1);
    qDebug() << "domain: " << domain << "cnt_out: " << cnt_out << ", cnt_in: " << cnt_in << ", a: " << a << ", b:" << b;

    std::vector<int16_t> buffer;
    buffer.resize(cnt_out);
    for (auto i = 0; i < cnt_out; i++)
    {
        // 线性插值
        auto pos = i * b;
        auto index_low = static_cast<int>(pos / a);
        auto index_high = index_low + 1;

        if (index_low >= cnt_in - 1)
        {
            buffer[i] = data[cnt_in - 1];
        }
        else
        {
            auto x = pos;
            auto x1 = index_low * a;
            auto x2 = index_high * a;
            buffer[i] = (x - x2) / (x1 - x2) * data[index_low] + (x - x1) / (x2 - x1) * data[index_high];
        }
    }
    return buffer;
}

std::vector<int16_t> MainWindow::get_data_in(int min, int max, int cnt_in, bool limit_range)
{
    std::vector<int16_t> data_in;

    // random
    std::random_device rd;                      // 用于种子
    std::mt19937 gen(rd());                     // Mersenne Twister 随机数引擎
    std::normal_distribution<> dist(50, 40);    // 正态分布 均值50，标准差40, 即范围为(50-40, 50+40)

    for (int i = 0; i < cnt_in; ++i) {
        int16_t y;
        if (limit_range) {
            // 拒绝采样
            do {
                y = dist(gen);
            } while (y < min || y > max);
        } else {
            y = dist(gen);
        }
        data_in.push_back(y);
    }
    return data_in;
}

void MainWindow::debug_vector(std::vector<QPointF> &vec)
{
    for (int i = 0; i < vec.size(); ++i) {
        qDebug() << i << vec[i];
    }
}

std::vector<QPointF> MainWindow::to_points(const std::vector<int16_t> &vec)
{
    std::vector<QPointF> points;
    for (int i = 0; i < vec.size(); ++i) {
        points.push_back(QPointF(i, vec[i]));
    }
    return points;
}

void MainWindow::on_btn_clear_released()
{
    ui->chart_1->clear_series();
    ui->chart_2->clear_series();
}


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
    if (type_name == get_type_name(TCG_Old)) {
        tcg_old_version();
    } else if (type_name == get_type_name(TCG_New)) {
        //...
    }
}

void MainWindow::tcg_old_version()
{
    qDebug() << "--" << __FUNCTION__;
    std::vector<QPointF> points_1;
    std::vector<QPointF> points_2;
    /// old version
    const auto focus_laws = 1;
    auto beampoints = 110;

    std::vector<double> deps = {0, 25, 50, 75, 100};
    std::vector<double> gains = {0, 10, 15, 20, 15};
    auto cnt = gains.size();

    for (int i = 0; i < focus_laws; i++) {
        for (int k = 0; k < cnt - 1; k++) {
            auto delta_points = deps[k + 1] - deps[k];
            auto delta_gain = gains[k + 1] - gains[k];
            double inc_gain = delta_gain / delta_points;
            double curr_gain = gains[k];

            auto init_value = static_cast<int>(deps[k]);
            auto end_value = static_cast<int>(deps[k + 1]);
            for (int j = init_value; j < end_value; j++) {
                curr_gain += inc_gain;
                points_1.push_back(QPointF(j, gains[k]));
                points_2.push_back(QPointF(j, curr_gain));
            }
        }
    }
    ui->chart_1->recv_points(std::make_shared<std::vector<QPointF>>(points_1));
    ui->chart_2->recv_points(std::make_shared<std::vector<QPointF>>(points_2));
}


void MainWindow::on_btn_clear_released()
{
    ui->chart_1->clear_series();
    ui->chart_2->clear_series();
}


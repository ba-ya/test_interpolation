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
    /// old version
    const auto focus_laws = 64;
    auto beampoints = 1024;

    std::vector<double> deps = {0, 20, 100, 500};
    std::vector<double> gains = {0, 10, 15, 20};
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
                auto value = static_cast<uint16_t>(512 * pow(10, curr_gain / 20.0));
            }
            if (i == 0) {
                qDebug().nospace() << "[" << init_value << "," << end_value << "), end_g: " << curr_gain;
            }
        }
    }
}


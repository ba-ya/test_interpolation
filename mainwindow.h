#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void init();

    enum Types{
        TCG_Old,
        TCG_New,

        CntType,
    };
    QString get_type_name(int type);

private slots:
    void on_btn_clear_released();

private:
    void do_something(QString type_name);
    void tcg_old_version(std::vector<double> deps, std::vector<double> gains, int cnt_out);
    void tcg_linear_version(std::vector<double> deps, std::vector<double> gains, int cnt_out);
    // only fit monotonical incremental
    std::vector<QPointF> linear_interpolate(std::vector<double> x, std::vector<double> y, int cnt_out);
    // adj
    void fill_zero(std::vector<double> &x, std::vector<double> &y);
    // debug
    void debug_vector(std::vector<QPointF> &vec);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

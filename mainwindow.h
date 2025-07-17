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
        Ndt_View_Dscan,
        Smp_Linkcl,

        CntType,
    };
    QString get_type_name(int type);

private slots:
    void on_btn_clear_released();

private:
    void do_something(QString type_name);
    /// ----------------tcg
    void tcg_old_version(std::vector<double> deps, std::vector<double> gains, int cnt_out);
    void tcg_linear_version(std::vector<double> deps, std::vector<double> gains, int cnt_out);
    // only fit monotonical incremental
    std::vector<QPointF> linear_interpolate(std::vector<double> x, std::vector<double> y, int cnt_out);
    // adj
    void fill_zero(std::vector<double> &x, std::vector<double> &y);
    /// ----------------ndt view dscan
    void ndt_view_dscan();
    std::vector<int16_t> on_beam_interpolate(
        double start_in, double end_in, const std::vector<int16_t> &data_in,
        double start_out, double end_out, int cnt_out
        );
    /// ----------------smp linkcl
    void smp_linkcl();
    std::vector<int16_t> on_data_interpolate(
        double domain, double wave_speed, int compress,
        int cnt_out,
        std::vector<int16_t> data, int cnt_in);
    /// ----------------other
    // data_in
    std::vector<int16_t> get_data_in(int min, int max, int cnt_in, bool limit_range = true);
    // prepare points for chart
    std::vector<QPointF> to_points(const std::vector<int16_t> &vec);
    // debug
    void debug_vector(std::vector<QPointF> &vec);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

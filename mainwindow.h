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

private:
    void do_something(QString type_name);
    void tcg_old_version();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QLabel *GetHumiLabel();
    QLabel *GetTempLabel();



private slots:
    void on_pushButton_clicked();
private:
    Ui::MainWindow *ui;
    QLabel *labelHumi;
    QLabel *labelTemp;
};
#endif // MAINWINDOW_H

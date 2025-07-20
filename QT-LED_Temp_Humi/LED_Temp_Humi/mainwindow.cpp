#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rpc_client.h"

#include <QDebug>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    labelHumi = this->findChild<QLabel*>("label");
    labelTemp = this->findChild<QLabel*>("label_2");
}

QLabel *MainWindow::GetHumiLabel()
{
    return labelHumi;
}
QLabel *MainWindow::GetTempLabel()
{
    return labelTemp;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    //static int status = 1;
    /*
    int status;
    int fd = open("/dev/100ask_led", O_RDWR);
    read(fd, &status, sizeof(status));
    if(status)
        qDebug()<<"LED clicked on";
    else
        qDebug()<<"LED clicked off";
    */
    /* 2. control LED */
    //led_control(status);
    //led_drv_control(status);
    //status = !status;
    rpc_led_control();
    //close(fd);

    //status = !status;
}

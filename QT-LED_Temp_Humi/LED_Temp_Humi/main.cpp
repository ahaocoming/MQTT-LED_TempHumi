#include "mainwindow.h"
#include "rpc_client.h"

#include <QApplication>
#include "dht11_thread.h"

int main(int argc, char *argv[])
{
    QLabel *labelHumi;
    QLabel *labelTemp;
    /* 1. init LED */
    //led_init();
    //led_drv_init();
    RPC_Client_Init();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    labelHumi = w.GetHumiLabel();
    labelTemp = w.GetTempLabel();

    /* 3. create dht11 thread */

    DHT11Thread thread;
    thread.SetLabels(labelHumi, labelTemp);
    thread.start(); // 启动线程


    return a.exec();
}

#include <QThread>
#include <QDebug>
#include "dht11_thread.h"
#include "dht11.h"
#include "rpc_client.h"
//2. rewrite run()
void DHT11Thread::run(){

    char humi;
    char temp;

    char buf[20];

    //dht11_init();


    while(1) {
        if(rpc_dht11_read(&humi, &temp) == 0)
        {
            /* display on qt */
            sprintf(buf, "%d%%", humi);
            labelHumi->setText(buf);
            sprintf(buf, "%d", temp);
            labelTemp->setText(buf);

            sleep(1); // 模拟耗时操作
        }

    }
}

void DHT11Thread::SetLabels(QLabel *labelHumi, QLabel *labelTemp)
{
    this->labelHumi = labelHumi;
    this->labelTemp = labelTemp;
}

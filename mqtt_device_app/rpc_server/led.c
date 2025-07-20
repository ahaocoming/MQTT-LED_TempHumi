#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
//#include <QDebug>

// use led's drive to achieve

static int fd;
static int led_state;
void *led_thread(void *arg)
{
    char buf[2];
    while(1)
    {
        if(read(fd, buf, 2) == 2)
        {
	    led_state = buf[1];
            sleep(2);
	    //printf("led_server_state:%d\n",led_state);
        }

    }
}

void led_init(void)
{
    pthread_t tid2;
    fd = open("/dev/100ask_led", O_RDWR);
    if(fd < 0)
    {
        //qDebug()<<"open /dev/100ask_led failed";
    }
    else
    {
        int ret = pthread_create(&tid2,NULL,led_thread,NULL);//创建线程
    }
}


void led_control(int on)
{
    char buf[2];
    buf[0] = 0;
    //fd = open("/dev/100ask_led", O_RDWR);
    if(on)
    {
        buf[1] = 0;
        write(fd, buf, 2);
    }
    else
    {
        buf[1] = 1;
        write(fd, buf, 2);
    }
    //close(fd);


}


int led_read(void)
{
    return led_state;
}

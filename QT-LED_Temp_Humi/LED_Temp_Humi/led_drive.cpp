#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <QDebug>

// use led's drive to achieve

static int fd;


void led_drv_init(void)
{
    fd = open("/dev/100ask_led", O_RDWR);
    if(fd < 0)
    {
        qDebug()<<"open /dev/100ask_led failed";
    }
}


void led_drv_control(int on)
{
    char buf[2];
    buf[0] = 0;

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



}

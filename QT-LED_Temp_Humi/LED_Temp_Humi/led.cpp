#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <QDebug>

// only use system command to achive
void led_init(void)
{
    /*
     *  echo 131 > /sys/class/gpio/export
     *  echo out > /sys/class/gpio/gpio131/direction
    */
    int fd;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if(fd < 0)
    {
        qDebug()<<"open /sys/class/gpio/export err";
        return;
    }

    write(fd, "131\n", 4);

    close(fd);

    fd = open("/sys/class/gpio/gpio131/direction", O_WRONLY);
    if(fd < 0)
    {
        qDebug()<<"open /sys/class/gpio/gpio131/direction err";
        return;
    }

    write(fd, "out\n", 4);

    close(fd);


}

void led_control(int on)
{
   /*
    *  echo 0 > /sys/class/gpio/gpio131/value     低电平亮起
    *  echo 1 > /sys/class/gpio/gpio131/value      高电平熄灭
    */
    static int fd = -1;

    if(fd == -1) //for no need open fd every time
    {
        fd = open("/sys/class/gpio/gpio131/value", O_RDWR);
    }
    if(fd < 0)
    {
        qDebug()<<"open /sys/class/gpio/gpio131/value err";
        return;
    }
    if(on)
    {
        write(fd, "0\n", 2);
    }
    else
    {
        write(fd, "1\n", 2);
    }
}





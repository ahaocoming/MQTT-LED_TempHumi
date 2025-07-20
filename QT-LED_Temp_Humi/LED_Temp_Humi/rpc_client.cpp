#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "rpc_client.h"
#include "cJSON.h"
#include <QDebug>
static int g_iSocketClient;

int rpc_led_control(void)
{
    char buf[100];
    int iLen;
    int ret = -1;
    char buffer[2];
    int iSocketClient = g_iSocketClient;

    static int status = 0;
    int fd = open("/dev/100ask_led", O_RDWR);
    if(fd < 0)
    {
        qDebug()<<"open /dev/100ask_led failed";
    }
    read(fd, buffer, 2);
    status = buffer[1];
    if(status) // 0 is on, 1 is off
        qDebug()<<"LED clicked off" << status;
    else
        qDebug()<<"LED clicked on" << status;
    status = !status;
    close(fd);
    sprintf(buf, "{\"method\": \"led_control\", \"params\": [%d], \"id\": \"2\" }", !status);
    qDebug()<<"LED now:" << status;
    iLen = send(iSocketClient, buf, strlen(buf), 0);
    if (iLen == strlen(buf))
    {
        while (1) 
        {
            iLen = read(iSocketClient, buf, sizeof(buf));
            buf[iLen] = 0;
            if (iLen == 1 && (buf[0] == '\r' || buf[0] == '\n'))
                continue;
            else
                break;
        } 
        
        if (iLen > 0)
        {
            cJSON *root = cJSON_Parse(buf);
            cJSON *result = cJSON_GetObjectItem(root, "result");
            ret = result->valueint;
            cJSON_Delete(root);
            return ret;
        }
        else
        {
            printf("read rpc reply err : %d\n", iLen);
            return -1;
        }
    }
    else
    {
        printf("send rpc request err : %d, %s\n", iLen, strerror(errno));
        return -1;
    }
}

int rpc_dht11_read(char *humi, char *temp)
{
    char buf[300];
    int iLen;

    int iSocketClient = g_iSocketClient;

    sprintf(buf, "{\"method\": \"dht11_read\"," \
                   "\"params\":[0], \"id\": \"2\" }");                    
    iLen = send(iSocketClient, buf, strlen(buf), 0);
    if (iLen ==  strlen(buf))
    {
        while (1) 
        {
            iLen = read(iSocketClient, buf, sizeof(buf));
            buf[iLen] = 0;
            if (iLen == 1 && (buf[0] == '\r' || buf[0] == '\n'))
                continue;
            else
                break;
        } 
        
        if (iLen > 0)
        {
            cJSON *root = cJSON_Parse(buf);
            cJSON *result = cJSON_GetObjectItem(root, "result");
            if (result)
            {
                cJSON * a = cJSON_GetArrayItem(result,0);
                cJSON * b = cJSON_GetArrayItem(result,1);
                *humi = a->valueint;
                *temp = b->valueint;

    
                cJSON_Delete(root);
                return 0;
            }
            else
            {
                cJSON_Delete(root);
                return -1;
            }
        }
        else
        {
            printf("read rpc reply err : %d\n", iLen);
            return -1;
        }
    }
    else
    {
        printf("send rpc request err : %d, %s\n", iLen, strerror(errno));
        return -1;
    }
}

/*-----------------------------------*/



int RPC_Client_Init(void) 
{
    int iSocketClient;
    struct sockaddr_in tSocketServerAddr;
    int iRet;

    iSocketClient = socket(AF_INET, SOCK_STREAM, 0);

    tSocketServerAddr.sin_family      = AF_INET;
    tSocketServerAddr.sin_port        = htons(PORT);  /* host to net, short */
    //tSocketServerAddr.sin_addr.s_addr = INADDR_ANY;
    inet_aton("127.0.0.1", &tSocketServerAddr.sin_addr);
    memset(tSocketServerAddr.sin_zero, 0, 8);


    iRet = connect(iSocketClient, (const struct sockaddr *)&tSocketServerAddr, sizeof(struct sockaddr));	
    if (-1 == iRet)
    {
        printf("connect error!\n");
        return -1;
    }

    g_iSocketClient = iSocketClient;

    return iSocketClient;    
}







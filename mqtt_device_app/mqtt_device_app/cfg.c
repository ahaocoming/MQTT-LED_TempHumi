#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include "cJSON.h"
#include "cfg.h"

/* 配置文件的内容格式：
{  “URI”：“mqtt://bj-2-mqtt.iot-api.com:1883”，
    “clientid”:"0m1g1fus",
    "username":"x78dthxeouzchlrs",
    "password":"nZAatc9raQ",
    "ProductID":"0m1g1fus"，
    "DeviceName":“home_control”

   
}
*/


int read_cfg(char *URI, char * clientid, char *username, char *password, char *pubtopic, char *subtopic)
{
    char buf[1000];
    cJSON *ptTemp;
    int fd = open(CFG_FILE, O_RDONLY);

    if(fd < 0)
        return -1;
    
    int ret = read(fd, buf, sizeof(buf));
    if (ret <= 0)
        return -1;
    
    cJSON *root = cJSON_Parse(buf);

    ptTemp = cJSON_GetObjectItem(root, "URI");
    if(ptTemp)
    {
        strcpy(URI, ptTemp->valuestring);
    }
    ptTemp = cJSON_GetObjectItem(root, "clientid");
    if(ptTemp)
    {
        strcpy(clientid, ptTemp->valuestring);
    }
    ptTemp = cJSON_GetObjectItem(root, "username");
    if(ptTemp)
    {
        strcpy(username, ptTemp->valuestring);
    }
    ptTemp = cJSON_GetObjectItem(root, "password");
    if(ptTemp)
    {
        strcpy(password, ptTemp->valuestring);
    }
    ptTemp = cJSON_GetObjectItem(root, "pubtopic");
    if(ptTemp)
    {
        strcpy(pubtopic, ptTemp->valuestring);
    }
    ptTemp = cJSON_GetObjectItem(root, "subtopic");
    if(ptTemp)
    {
        strcpy(subtopic, ptTemp->valuestring);
    }

    cJSON_Delete(root);
    return 0;
}
#include <jsonrpc-c.h>
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
#include "rpc_server.h"
#include "led.h"
#include "dht11.h"

static struct jrpc_server my_server;


/* 参数: {"params":[0|1]} */
cJSON * server_led_control(jrpc_context * ctx, cJSON * params, cJSON *id) {

    cJSON * status = cJSON_GetArrayItem(params,0);
    led_control(status->valueint);

    return cJSON_CreateNumber(status->valueint);
}
/* 参数: {"params":null} */
cJSON * server_led_read(jrpc_context * ctx, cJSON * params, cJSON *id) {

    return cJSON_CreateNumber(led_read());
}
/* 参数: {"params":null} */
cJSON * server_dht11_read(jrpc_context * ctx, cJSON * params, cJSON *id) {
    int array[2];
    array[0] = array[1] = 0;
    // dht11_read(&humi,&temp)

    while(dht11_read((char *)&array[0], (char *)&array[1]) != 0);

    return cJSON_CreateIntArray(array, 2);
}


int RPC_Server_Init(void) 
{
    int err;
    
    err = jrpc_server_init(&my_server, PORT);
    if (err)
    {
        printf("jrpc_server_init err : %d\n", err);
    }
    
    jrpc_register_procedure(&my_server, server_led_control, "led_control", NULL );
    jrpc_register_procedure(&my_server, server_led_read, "led_read", NULL );
    jrpc_register_procedure(&my_server, server_dht11_read, "dht11_read", NULL );

    jrpc_server_run(&my_server);
    jrpc_server_destroy(&my_server);

    return 0;
}


int main(int argc, char **argv)
{ 
    led_init();
    dht11_init();
    RPC_Server_Init();

    return 0;
}



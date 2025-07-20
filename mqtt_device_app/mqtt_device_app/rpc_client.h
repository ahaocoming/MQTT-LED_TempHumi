#ifndef _RPC_CLIENT_H
#define _RPC_CLIENT_H

#define PORT 1234

int rpc_led_control(int on);
int rpc_dht11_read(char *humi, char *temp);
int rpc_led_read(void);
int RPC_Client_Init(void);

#endif


/*******************************************************************************
 * Copyright (c) 2012, 2023 IBM Corp., Ian Craggs
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution. 
 *
 * The Eclipse Public License is available at 
 *   https://www.eclipse.org/legal/epl-2.0/
 * and the Eclipse Distribution License is available at 
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial contribution
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
#include "rpc_client.h"
#include "cfg.h"
#include "cJSON.h"

#include "unistd.h" //sleep()函数

#define ADDRESS     "tcp://192.168.5.10:1883"
#define CLIENTID    "0m1g1fus"
#define TOPIC_SUBSCRIBE     "attributes/push"
#define TOPIC_PUBLISH       "attributes"
#define QOS         0
#define TIMEOUT     10000L

#define USER_NAME "100ask"
#define PASSWORD  "100ask"

int led_state;

volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: %.*s\n", message->payloadlen, (char*)message->payload);

/* 根据接收到的消息控制LED */
/* 消息格式: {"LED":0} */
    cJSON *root = cJSON_Parse((char*)message->payload);
    cJSON *LED = cJSON_GetObjectItem(root, "LED");
    if(LED)
    {
       led_state = rpc_led_control(LED->valueint);
       printf("led_state:%d",led_state);      
    }
    cJSON_Delete(root);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    if (cause)
    	printf("     cause: %s\n", cause);
}

int main(int argc, char* argv[])
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    char URI[1000];
    char clientid[1000];
    char username[1000];
    char password[1000];
    char pubtopic[1000];
    char subtopic[1000];

    char address[1000];

    char pub_topic[1000];//$thing/up/property/{ProductID}/{DeviceName}
    char sub_topic[1000];//$thing/down/property/{ProductID}/{DeviceName}

   /* 读配置文件 /etc/ThingsCloud.cfg 得到URI、clientID、username、passWD、ProductID、DeviceName*/
    if( 0 != read_cfg(URI, clientid, username, password, pubtopic, subtopic) )
    {
        printf("read cfg err\n");
        return -1;
    }
    sprintf(address,"%s", URI);

    sprintf(pub_topic,"%s", pubtopic);
    sprintf(sub_topic,"%s", subtopic);

   /* INIT RPC: connect RPC Server */
    if(RPC_Client_Init() == -1)
    {
        printf("RPC_Client_Init err\n");
        return -1;
    }
   
    if ((rc = MQTTClient_create(&client, address, clientid,
        MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to create client, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto exit;
    }

    if ((rc = MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to set callbacks, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username = username;
    conn_opts.password = password;
    while(1)
    {
        if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
        {
            printf("Failed to connect, return code %d\n", rc);
            rc = EXIT_FAILURE;
            //goto destroy_exit;
        }
        else
        {
            break;
        }
    }

    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", TOPIC_SUBSCRIBE, CLIENTID, QOS);
    if ((rc = MQTTClient_subscribe(client, sub_topic, QOS)) != MQTTCLIENT_SUCCESS)
    {
        printf("%s", pubtopic);
        printf("%s", subtopic);
    	printf("Failed to subscribe, return code %d\n", rc);
    	rc = EXIT_FAILURE;
    }
    else
    {
    	int ch;
        int cnt = 0;
        MQTTClient_message pubmsg = MQTTClient_message_initializer;
        char buf[1000];
        MQTTClient_deliveryToken token;
        
    	while (1)
    	{
            /* rpc_dht11_read */
            char humi, temp;
            while (rpc_dht11_read(&humi, &temp) != 0);
            led_state = rpc_led_read();
            
            sprintf(buf, "{ \
                            \"temp_value\":%d,\
                            \"humi_value\":%d,\
                            \"LED\":%d\
                          }", temp, humi,!led_state);
            printf("buf:%s\n", buf);
            pubmsg.payload = buf;
            pubmsg.payloadlen = (int)strlen(buf);
            pubmsg.qos = QOS;
            pubmsg.retained = 0;

            /* 发布消息 */


            if ((rc = MQTTClient_publishMessage(client, pub_topic, &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
            {
                 printf("Failed to publish message, return code %d\n", rc);
                 continue;
            }
            
            rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
            printf("Message with delivery token %d delivered\n", token);

            sleep(5);

    	} 

        if ((rc = MQTTClient_unsubscribe(client, sub_topic)) != MQTTCLIENT_SUCCESS)
        {
        	printf("Failed to unsubscribe, return code %d\n", rc);
        	rc = EXIT_FAILURE;
        }
    }

    if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS)
    {
    	printf("Failed to disconnect, return code %d\n", rc);
    	rc = EXIT_FAILURE;
    }
destroy_exit:
    MQTTClient_destroy(&client);
exit:
    return rc;
}

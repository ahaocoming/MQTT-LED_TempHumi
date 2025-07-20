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

#define ADDRESS     "tcp://192.168.5.10:1883"
#define CLIENTID    "ExampleClientSub"
#define TOPIC_SUBSCRIBE     "/iot/down"
#define TOPIC_PUBLISH       "/iot/up"
#define QOS         0
#define TIMEOUT     10000L

#define USER_NAME "100ask"
#define PASSWORD  "100ask"

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
    char broker[100];

    if (argc !=2 )
    {
	    printf("Usage: %s <broker_ip>\n", argv[0]);
	    return -1;
    }

    sprintf(broker, "tcp://%s:1883", argv[1]);

   /* 读配置文件 /etc/wechat.cfg 得到URL、clientID、username、passWD、productID、DeviceName*/

   /* INIT RPC: connect RPC Server */


    if ((rc = MQTTClient_create(&client, broker, CLIENTID,
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
    conn_opts.username = USER_NAME;
    conn_opts.password = PASSWORD;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", TOPIC_SUBSCRIBE, CLIENTID, QOS);
    if ((rc = MQTTClient_subscribe(client, TOPIC_SUBSCRIBE, QOS)) != MQTTCLIENT_SUCCESS)
    {
    	printf("Failed to subscribe, return code %d\n", rc);
    	rc = EXIT_FAILURE;
    }
    else
    {
    	int ch;
        int cnt = 0;
        MQTTClient_message pubmsg = MQTTClient_message_initializer;
        char buf[100];
        MQTTClient_deliveryToken token;
        
    	while (1)
    	{
            /* rpc_dht11_read */

            sprintf(buf, "publish %d", cnt++);
            pubmsg.payload = buf;
            pubmsg.payloadlen = (int)strlen(buf);
            pubmsg.qos = QOS;
            pubmsg.retained = 0;

            /* 发布消息 */


            if ((rc = MQTTClient_publishMessage(client, TOPIC_PUBLISH, &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
            {
                 printf("Failed to publish message, return code %d\n", rc);
                 continue;
            }
            
            rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
            printf("Message with delivery token %d delivered\n", token);                        
    	} 

        if ((rc = MQTTClient_unsubscribe(client, TOPIC_SUBSCRIBE)) != MQTTCLIENT_SUCCESS)
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

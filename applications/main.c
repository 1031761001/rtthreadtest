/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-08     RT-Thread    first version
 */
#include <arpa/inet.h>
#include <netdev.h>
#include <rtthread.h>
#include "paho_mqtt.h"

/* 连接MQTT的参数 */
#define MQTT_Uri    "tcp://39.106.25.57:1883"   // MQTT服务器的地址和端口号
#define ClientId    "yutangv01"                // ClientId需要唯一
#define UserName    "yutang"                    // 用户名
#define PassWord    "xuuuu194264"                    // 用户名对应的密码


/* WiFi信息 */

#define ssid "yutang"
#define wifi_password "438438438"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

static void mqtt_main(void);
static int mqtt_thread_entry(void);


int main(void)
{
    int count = 1;
    struct netdev *dev = RT_NULL;
    rt_wlan_connect((char *)ssid,(char *)wifi_password);
    rt_thread_mdelay(1000);
    rt_thread_t thread = rt_thread_create("mqtt", mqtt_thread_entry, RT_NULL, 4096, 25, 10);

    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }

    while (count++)
    {
       // LOG_D("Hello RT-Thread!");
        rt_thread_mdelay(1000);
    }
    
    return RT_EOK;
}


static int mqtt_thread_entry(void)
{
    struct netdev *dev = RT_NULL;
    while (1)
    {
        dev = netdev_get_first_by_flags(NETDEV_FLAG_INTERNET_UP);
        if (dev == RT_NULL)
        {
            LOG_D("wait netdev internet up...");
            rt_thread_mdelay(500);
        }
        else
        {
            LOG_D("local ip is:%d.%d.%d.%d", ((( dev->ip_addr.addr) >> 0) & 0xFF), ((( dev->ip_addr.addr) >> 8) & 0xFF),
                    ((( dev->ip_addr.addr) >> 16) & 0xFF), ((( dev->ip_addr.addr) >> 24) & 0xFF));
            break;
        }

    }

    LOG_D("mqtt_main begin...");
    mqtt_main();
}

/* 收到订阅的"Bear_Pi"主题的消息时的回调函数*/
static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data);
/* 默认的订阅回调函数，如果有订阅的 Topic 没有设置回调函数，则使用该默认回调函数 */
static void mqtt_sub_default_callback(MQTTClient *c, MessageData *msg_data);
/* 连接成功回调函数 */
static void mqtt_connect_callback(MQTTClient *c);
/* 上线回调函数 */
static void mqtt_online_callback(MQTTClient *c);

    /* 定义一个MQTT客户端结构体 */
    static MQTTClient client;

static void mqtt_main(void)
{


        /* 对MQTT客户端结构体变量进行配置 */
    client.isconnected = 0;
    client.uri = MQTT_Uri;

    /* 配置MQTT的连接参数 */
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;
    memcpy(&client.condata, &condata, sizeof(condata));
    client.condata.clientID.cstring = ClientId;
    client.condata.keepAliveInterval = 30;
    client.condata.cleansession = 1;
    client.condata.username.cstring = (char*)UserName;
    client.condata.password.cstring = (char*)PassWord;


      /* 为mqtt申请内存 */
    client.buf_size = client.readbuf_size = 1024;
    client.buf = rt_calloc(1, client.buf_size);
    client.readbuf = rt_calloc(1, client.readbuf_size);
    if (!(client.buf && client.readbuf))
    {
        rt_kprintf("no memory for MQTT client buffer!\r\n");
        return -1;
    }

        /* 设置回调函数  */
    client.connect_callback = mqtt_connect_callback;
    client.online_callback = mqtt_online_callback;
    /* client.offline_callback = mqtt_offline_callback; */

    /* 订阅一个主题，并设置其回调函数 */
    client.messageHandlers[0].topicFilter = rt_strdup("F407V01");
    client.messageHandlers[0].callback = mqtt_sub_callback;
    client.messageHandlers[0].qos = QOS1;

    /* 设置默认的回调函数 */
    client.defaultMessageHandler = mqtt_sub_default_callback;


    /* 启动 mqtt client */
    paho_mqtt_start(&client);

    while(1)
    {
        /* LOG_D("MQTT..."); */
        rt_thread_mdelay(1500);
    }
}

char i;
/* 收到订阅的"Bear_Pi"主题的消息时的回调函数*/
static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data)
{
    rt_pin_mode(1,0);
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    rt_kprintf("Receive topic: %.*s, message data:\r\n", msg_data->topicName->lenstring.len, msg_data->topicName->lenstring.data);
    rt_kprintf("%.*s\r\n", msg_data->message->payloadlen, (char *)msg_data->message->payload);
    
        if(*(char *)(msg_data->message->payload) == '1' )
        rt_pin_write(1,0);
    else
        rt_pin_write(1,1);

}

/* 默认的订阅回调函数，如果有订阅的 Topic 没有设置回调函数，则使用该默认回调函数 */
static void mqtt_sub_default_callback(MQTTClient *c, MessageData *msg_data)
{
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    rt_kprintf("Receive topic: %.*s, message data:\r\n", msg_data->topicName->lenstring.len, msg_data->topicName->lenstring.data);
    rt_kprintf("%.*s\r\n", msg_data->message->payloadlen, (char *)msg_data->message->payload);
}

/* 连接成功回调函数 */
static void mqtt_connect_callback(MQTTClient *c)
{
    rt_kprintf("mqtt connect success! \r\n");
}

/* 上线回调函数 */
static void mqtt_online_callback(MQTTClient *c)
{
    rt_kprintf("mqtt online \r\n");
    paho_mqtt_publish(&client, QOS1, "F407V01", "Hello, I am BearPi with RT_Thread.");
}


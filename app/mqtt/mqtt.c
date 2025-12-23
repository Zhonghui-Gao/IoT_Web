#include "mqtt.h"
#include <MQTTClient.h>
#include <log.h>
#include <stdlib.h>
#include <assert.h>

//客户端结构体指针
static MQTTClient client;
//链接配置结构
static MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
//标记MQTT消息的值，检查消息是否发送成功
static MQTTClient_deliveryToken token;
//回调函数
static int (*recv_callback)(char *, int);

static void mqtt_connectionLost(void *context, char *cause)
{
    assert(context == NULL);
    log_fatal("MQTT connection lost: %s", cause);
    exit(EXIT_FAILURE);
}

static int mqtt_messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    assert(context == NULL);
    log_trace("Message arrived\n\ttopic: %.*s\n\tpayload: %.*s",
              topicLen, topicName,
              (int)message->payloadlen, (char *)message->payload);

    int result = recv_callback((char *)message->payload, (int)message->payloadlen);
    return result == 0 ? 1 : 0;
}

static void mqtt_deliveryComplete(void *context, MQTTClient_deliveryToken dt)
{
    assert(context == NULL);
    log_trace("Message delivery complete for token: %d", dt);
}

int mqtt_init()
{
    //初始化函数
    int rc;
    if ((rc = MQTTClient_create(&client, MQTT_SERVER, CLIENT_ID,
                                MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS) {
        log_error("Failed to create MQTT client, return code %d", rc);
        goto EXIT;
    }
    //设置回调函数
    rc = MQTTClient_setCallbacks(client, NULL, mqtt_connectionLost, mqtt_messageArrived, mqtt_deliveryComplete);
    if (rc != MQTTCLIENT_SUCCESS) {
        log_error("Failed to set MQTT callbacks, return code %d", rc);
        goto DESTROY_EXIT;
    }

    //配置连接参数
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    rc = MQTTClient_connect(client, &conn_opts);
    if (rc != MQTTCLIENT_SUCCESS) {
        log_error("Failed to connect to MQTT broker, return code %d", rc);
        goto DESTROY_EXIT;
    }

    rc = MQTTClient_subscribe(client, PULL_TOPIC, QOS);

    if (rc != MQTTCLIENT_SUCCESS) {
        log_error("Failed to subscribe to topic %s, return code %d", PULL_TOPIC, rc);
        goto DISCONNECT_EXIT;
    }

    log_info("MQTTClient initialized success");
    return 0;

DISCONNECT_EXIT:
    MQTTClient_disconnect(client, 10000);
DESTROY_EXIT:
    MQTTClient_destroy(&client);
EXIT:
    return -1;
}

void mqtt_registerRecvCallback(int (*callback)(char *, int))
{
    recv_callback = callback;
}

int mqtt_send(char *json_str, int len)
{
    MQTTClient_message message = MQTTClient_message_initializer;
    message.payload = json_str;
    message.payloadlen = len;
    message.qos = QOS;
    message.retained = 0;

    int rc = MQTTClient_publishMessage(client, PUSH_TOPIC, &message, &token);
    if (rc != MQTTCLIENT_SUCCESS) {
        log_error("Failed to publish message, return code %d", rc);
        return -1;
    } else {
        log_trace("MQTTClient_publishMessage success");
        return 0;
    }
}

void mqtt_close()
{
    log_info("MQTT Client closing...");
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
}
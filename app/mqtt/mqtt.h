#pragma once

#if !defined(__APP_MQTT_H__)
#define __APP_MQTT_H__

#define MQTT_SERVER "tcp://127.0.0.1:1883"
#define CLIENT_ID "gao-ubuntu-68f72895e04b"
#define PULL_TOPIC "PULL_TOPIC"
#define PUSH_TOPIC "PUSH_TOPIC"

/**
 * @brief QOS level
 *
 * @return  0:至多交付一次，1:至少交付一次，2：仅交付一次
 */
#define QOS 0

/**
 * @brief MQTT初始化
 *
 * @return int 0:成功，-1:失败
 */
int mqtt_init();

/**
 * @brief MQTT注册接收回调函数
 *
 * @param recv_callback 接收回调函数
 */
void mqtt_registerRecvCallback(int (*recv_callback)(char *, int));

/**
 * @brief MQTT发送数据
 *
 * @param json_str json字符串
 * @param len json字符串长度
 */
int mqtt_send(char *json_str, int len);

/**
 * @brief MQTT关闭
 *
 */
void mqtt_close();


#endif // !define(__APP_MQTT_H__)
#pragma once

#if !defined(__APP_MESSAGE_H__)
#define __APP_MESSAGE_H__

typedef enum ConnectionTypeEnum {
    Connection_Type_NONE = 0,
    Connection_Type_LORA = 1,
    Connection_Type_BLE_MESH = 2,
    Connection_Type_IIC = 3
} ConnectionType;

// message：type id_len data_len id + data
typedef struct MessageStruct {
    ConnectionType connection_type;  //连接类型
    unsigned char *payload;         //消息体
    int id_len;                     //设备ID长度
    int data_len;                   //数据长度
} Message;

/**
 * @brief 初始化消息
 *
 * @param message 消息指针
 * @param binary 二进制数据
 * @param len 二进制数据长度
 * @return int 0 成功，-1 失败
 */
int message_initByBinary(Message *message, void *binary, int len);

/**
 * @brief 初始化消息
 *
 * @param message  消息指针
 * @param json_str json字符串
 * @param len      json字符串长度
 * @return int     0 成功，-1 失败
 */
int message_initByJson(Message *message, char *json_str, int len);

/**
 * @brief      保存消息
 *
 * @param message    消息指针
 * @param binary     二进制数据
 * @param len         二进制数据长度
 * @return int        成功返回实际保存长度，-1 失败
 */
int message_saveBinary(Message *message, void *binary, int len);

/**
 * @brief      保存消息
 *
 * @param message    消息指针
 * @param json_str    json字符串
 * @param len         json字符串长度
 * @return int         0 成功，-1 失败
 */
int message_saveJson(Message *message, char *json_str, int len);

void message_free(Message *message);

#endif
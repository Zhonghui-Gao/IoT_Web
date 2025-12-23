#pragma once

#if !defined(__APP_BUFFER_H__)
#define __APP_BUFFER_H__

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>

#define LOG 1

typedef struct BUFFERSTRUCT {
    void *ptr;              //缓存区指针
    int size;            //缓存区长度
    int start;           //数据起始位
    int len;             //数据长度
    pthread_mutex_t mutex;   //互斥锁
} Buffer;

/**
 * @brief 初始化Buffer
 *
 * @param buffer buffer对象指针
 * @param size buffer长度
 * @return int 0成功 -1失败
 */
int buffer_init(Buffer *buffer, int size);

/**
 * @brief 从Buffer中读取数据
 *
 * @param buffer buffer对象指针
 * @param buf 读出数据的缓存区指针
 * @param len 缓存区长度
 * @return size_t
 */
int buffer_read(Buffer *buffer, void *buf, int len);

/**
 * @brief 向Buffer中写入数据
 *
 * @param buffer buffer对象指针
 * @param buf 要写入的数据指针
 * @param len 要写入的长度
 * @return size_t
 */
int buffer_write(Buffer *buffer, void *buf, int len);

/**
 * @brief 查询环形缓冲区当前可读数据量，线程安全
 *
 * @param buffer buffer指针
 */
int buffer_available(Buffer *buffer);

/**
 * @brief 销毁Buffer
 *
 * @param buffer buffer指针
 */
void buffer_free(Buffer *buffer);

#endif
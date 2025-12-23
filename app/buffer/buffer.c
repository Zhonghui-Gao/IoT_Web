#include "buffer.h"
#include <string.h>
#include <stdlib.h>
#include <log.h>
#include <cjson/cJSON.h>

// static pthread_mutex_t lock_initializer = PTHREAD_MUTEX_INITIALIZER;

int buffer_init(Buffer *buffer, int size)
{
    // Buffer *buffer = malloc(sizeof(Buffer));
    buffer->ptr = malloc(size);
    if (!buffer->ptr) {
#ifdef LOG
        log_warn("not enough  memory for buffer %p", buffer);
        return -1;
#else
        return -1;
#endif
    }

    buffer->size = size;
    buffer->start = 0;
    buffer->len = 0;
    pthread_mutex_init(&buffer->mutex, NULL); //³õÊ¼»¯Ëø
    // memcpy(&buffer->mutex, &lock_initializer, sizeof(pthread_mutex_t));

#ifdef LOG
    log_debug("Buffer:%p created", buffer);
    return 0;
#else
    return 0;
#endif
}

int buffer_read(Buffer *buffer, void *buf, int len)
{
    if (!buffer || !buf) {
        log_warn("Buffer or buf not valid");
        return -1;
    }

    pthread_mutex_lock(&buffer->mutex);

    if (len > buffer->len)   len = buffer->len;

    if (len == 0) {
        pthread_mutex_unlock(&buffer->mutex);
        return 0;
    }

    if (buffer->start + len <= buffer->size) {
        memcpy(buf, buffer->ptr + buffer->start, len);
        buffer->start += len;
    } else {
        int first_len = buffer->size - buffer->start;
        memcpy(buf, buffer->ptr + buffer->start, first_len);
        memcpy(buf + first_len, buffer->ptr, len - first_len);
        buffer->start = len - first_len;
    }

    buffer->len -= len;

    pthread_mutex_unlock(&buffer->mutex);
    log_trace("Buffer status after read: start: %d, len: %d", buffer->start, buffer->len);
    return len;
}

int buffer_write(Buffer *buffer, void *buf, int len)
{
    if (!buffer || !buf) {
        log_warn("Buffer or buf not valid");
        return -1;
    }

    pthread_mutex_lock(&buffer->mutex);
    if (len > buffer->size - buffer->len) {
        pthread_mutex_unlock(&buffer->mutex);
        log_warn("Buffer not enough: %d", buffer);
        return -1;
    }

    int write_offset = buffer->start + buffer->len;

    if (write_offset > buffer->size) {
        write_offset -= buffer->size;
    }

    if (write_offset + len <= buffer->size) {
        memcpy(buffer->ptr + write_offset, buf, len);
    } else {
        int first_len = buffer->size - write_offset;
        memcpy(buffer->ptr + write_offset, buf, first_len);
        memcpy(buffer->ptr, buf + first_len, len - first_len);
    }

    buffer->len += len;
    pthread_mutex_unlock(&buffer->mutex);
    log_trace("Buffer status after write: start %d, len %d", buffer->start, buffer->len);
    return 0;
}

int buffer_available(Buffer *buffer)
{
    pthread_mutex_lock(&buffer->mutex);
    int len = buffer->len;
    pthread_mutex_unlock(&buffer->mutex);
    return len;
}

void buffer_free(Buffer *buffer)
{
    pthread_mutex_destroy(&buffer->mutex);
    free(buffer->ptr);
    free(buffer);
}
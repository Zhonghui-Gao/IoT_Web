#include "message/message.h"
#include <log.h>
#include <assert.h>
#include <string.h>

// 添加一个辅助函数来打印十六进制数组
static void print_hex_array(const char *label, const unsigned char *data, int len)
{
    char buffer[256];
    int pos = 0;

    pos += sprintf(buffer + pos, "%s: ", label);
    for (int i = 0; i < len; i++) {
        pos += sprintf(buffer + pos, "%02X", data[i]);
    }
    log_info("%s", buffer);
}

int main(void)
{
    unsigned char data[] = {0x01, 0x02, 0x04, 0x00, 0x01, 0xEA, 0xAC, 0x22, 0x88};
    unsigned char result[9];
    char json_str[1024];
    Message message;

    // 初始化消息
    int ret = message_initByBinary(&message, data, sizeof(data));
    if (ret != 0) {
        log_error("Failed to init message from binary");
        return -1;
    }

    // 保存为JSON
    ret = message_saveJson(&message, json_str, sizeof(json_str));
    if (ret != 0) {
        log_error("Failed to save message to JSON");
        message_free(&message);
        return -1;
    }

    log_info("%s", json_str);
    print_hex_array("Original data", data, sizeof(data));

    Message message2;
    // 从JSON初始化新消息
    ret = message_initByJson(&message2, json_str, strlen(json_str));  // 使用strlen而不是sizeof
    if (ret != 0) {
        log_error("Failed to init message from JSON");
        message_free(&message);
        return -1;
    }

    // 保存为二进制
    ret = message_saveBinary(&message2, result, sizeof(result));
    if (ret <= 0) {
        log_error("Failed to save message to binary");
        message_free(&message);
        message_free(&message2);
        return -1;
    }

    print_hex_array("Result data", result, sizeof(result));

    // 修复断言：使用memcmp比较内存内容，而不是memcpy的返回值
    assert(memcmp(data, result, sizeof(data)) == 0);

    message_free(&message);
    message_free(&message2);

    log_info("Test passed!");
    return 0;
}
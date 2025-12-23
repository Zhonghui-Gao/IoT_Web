#include "mqtt/mqtt.h"
#include <assert.h>
#include <string.h>
#include <unistd.h>

static int recv_flag = 0;

int recvCallback(char *str, int len)
{
    recv_flag = 1;
    assert(memcmp(str, "Hello MQTT", len) == 0);
    return 0;
}

int main(void)
{
    mqtt_init();

    mqtt_registerRecvCallback(recvCallback);

    mqtt_send("Hello MQTT", strlen("Hello MQTT"));

    //等待接收回调
    int wait_time = 0;
    while (!recv_flag && wait_time < 10) {
        sleep(1);
        wait_time++;
    }

    mqtt_close();

    return 0;
}
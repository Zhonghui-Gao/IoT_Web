#include "message.h"
#include <string.h>
#include <ctype.h>
#include <log.h>
#include <cjson/cJSON.h>
#include <stdlib.h>

// ÷∂Øfree
static char *bin_to_str(unsigned char *binary, int len)
{
    char *hex_str = (char *)malloc(len * 2 + 1);
    if (!hex_str) {
        log_warn("Not enough memory");
        return NULL;
    }

    for (int i = 0; i < len; i++) {
        sprintf(hex_str + i * 2, "%02X", binary[i]);
    }

    hex_str[len * 2] = '\0';
    return hex_str;
}

static int str_to_bin(char *hex_str, unsigned char *binary, int len)
{
    if (strlen(hex_str) % 2 != 0) {
        log_warn("Hex string is not valid (odd length)");
        return -1;
    }

    if (len < (int)(strlen(hex_str) / 2)) {
        log_warn("Buffer length is not enough, needed %d but got %d", (int)(strlen(hex_str) / 2), len);
        return -1;
    }

    len = strlen(hex_str) / 2 ;
    for (int i = 0; i < len; i++) {
        char hex_str_high = hex_str[i * 2];
        char hex_str_low  = hex_str[i * 2 + 1];

        if (!isxdigit(hex_str_high) || !isxdigit(hex_str_low)) {
            log_warn("Invalid hex character in input string");
            return -1;
        }

        unsigned char high_val = (hex_str_high >= '0' && hex_str_high <= '9') ? hex_str_high - '0' :
                                 (hex_str_high >= 'a' && hex_str_high <= 'f') ? hex_str_high - 'a' + 10 :
                                 (hex_str_high >= 'A' && hex_str_high <= 'F') ? hex_str_high - 'A' + 10 : 0;

        unsigned char low_val =  (hex_str_low >= '0' && hex_str_low <= '9') ? hex_str_low - '0' :
                                 (hex_str_low >= 'a' && hex_str_low <= 'f') ? hex_str_low - 'a' + 10 :
                                 (hex_str_low >= 'A' && hex_str_low <= 'F') ? hex_str_low - 'A' + 10 : 0;

        binary[i] = (high_val << 4 ) | low_val;
    }
    return len;
}

int message_initByBinary(Message *message, void *binary, int len)
{
    memset(message, 0, sizeof(Message));

    //type id data payload payload = type + id + data
    memcpy(&message->connection_type, binary, 1);
    memcpy(&message->id_len, binary + 1, 1);
    memcpy(&message->data_len, binary + 2, 1);

    if (len != message->id_len + message->data_len + 3) {
        log_warn("Message is not valid");
        return -1;
    }

    message->payload = malloc(message->id_len + message->data_len);

    if (!message->payload) {
        log_warn("Not enough for message");
        return -1;
    }

    memcpy(message->payload, binary + 3, message->id_len + message->data_len);
    return 0;
}

int message_initByJson(Message *message, char *json_str, int len)
{
    cJSON *json_object = cJSON_ParseWithLength(json_str, len);
    //
    cJSON *connection_type = cJSON_GetObjectItem(json_object, "connection_type");
    message->connection_type = connection_type->valueint;

    cJSON *id = cJSON_GetObjectItem(json_object, "id");
    if (strlen(id->valuestring) % 2 != 0) {
        log_warn("Message is not valid");
        return -1;
    }
    message->id_len = strlen(id->valuestring) / 2;

    cJSON *data = cJSON_GetObjectItem(json_object, "data");
    if (strlen(data->valuestring) % 2 != 0) {
        log_warn("Message is not valid");
        return -1;
    }
    message->data_len = strlen(data->valuestring) / 2;

    message->payload = malloc(message->data_len + message->id_len);
    if (!message->payload) {
        log_warn("Not enough memory for message");
        return -1;
    }

    if (str_to_bin(id->valuestring, message->payload, message->id_len) < 0) {
        log_info("Convertion failed");
        free(message->payload);
        return -1;
    }

    if (str_to_bin(data->valuestring, message->payload + message->id_len, message->data_len) < 0) {
        log_info("Convertion failed");
        free(message->payload);
        return -1;
    }

    cJSON_Delete(json_object);
    return 0;
}

int message_saveBinary(Message *message, void *binary, int len)
{
    int total_len = 3 + message->id_len + message->data_len;

    if (len < total_len) {
        log_warn("Buffer is not enough for message");
        return -1;
    }

    memcpy(binary, &message->connection_type, 1);
    memcpy(binary + 1, &message->id_len, 1);
    memcpy(binary + 2, &message->data_len, 1);
    memcpy(binary + 3, message->payload, message->id_len + message->data_len);

    return total_len;
}

int message_saveJson(Message *message, char *json_str, int len)
{
    cJSON *json_object = cJSON_CreateObject();

    cJSON_AddNumberToObject(json_object, "connection_type", message->connection_type);

    cJSON_AddStringToObject(json_object, "id", bin_to_str(message->payload, message->id_len));
    cJSON_AddStringToObject(json_object, "data", bin_to_str(message->payload + message->id_len, message->data_len));

    char *str = cJSON_PrintUnformatted(json_object);
    if (len < (int)strlen(str) - 1) {
        log_warn("Buffer not enough for message");
        return -1;
    }

    strcpy(json_str, str);
    cJSON_free(str);
    cJSON_Delete(json_object);
    return 0;
}

void message_free(Message *message)
{
    if (message->payload) {
        free(message->payload);
        message->payload = NULL;
    }
}
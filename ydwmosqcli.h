#ifndef YDW_MOSQ_CLI_H
#define YDW_MOSQ_CLI_H

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <mosquitto.h>
extern int debugLevel;
#define TOPIC_TEST1 "/teltonika/#"
#define TOPIC_QOS 0
enum{
    MSG_ERROR,
    MSG_WARNING,
    MSG_INFO,
    MSG_DEBUG,
    MSG_MSGDUMP,
    MSG_EXCESSIVE,
};

#define dbg_printf(level, ...)                      \
do                                                  \
{                                                   \
    if (debugLevel >= level)                         \
    {                                               \
        syslog(LOG_WARNING, __VA_ARGS__);           \
    }                                               \
}                                                   \
while (0)
//printf(__VA_ARGS__);
void myclient_message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);
void myclient_subscribe_callback(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos);
void myclient_log_callback(struct mosquitto *mosq, void *obj, int level, const char *str);
void myclient_connect_callback(struct mosquitto *mosq, void *obj, int result);
void myclient_disconnect_callback(struct mosquitto *mosq, void *obj, int result);
#endif
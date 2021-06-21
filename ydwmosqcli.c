#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <getopt.h>
#include <limits.h>
#include <locale.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "json.h"
#include "yodiwo.h"
#include "config.h"

#include "log.h"
#include "external.h"
#include <mosquitto.h>
#include <liblog.h>
#include "ydwmosqcli.h"
#include "ydwjsoncli.h"
#include "ydwucicli.h"
void myclient_message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    if (message->payloadlen) {

                 log_message(NAME, L_NOTICE,"Subject received: %s\r\n", message->topic);
                 log_message(NAME, L_NOTICE,"Received message: %s, message length: %d\r\n", message->payload, message->payloadlen);
				 printf("Subject received: %s\r\n", message->topic);
                 json_decoder(message);
                 config_init();
                 
    }
    
    return;
}

void myclient_subscribe_callback(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos)
{
    log_message(NAME, L_NOTICE,"Subscribed (mid: %d): %d\r\n", mid, granted_qos[0]);
	printf( "Start sub ok\n");
}

void myclient_log_callback(struct mosquitto *mosq, void *obj, int level, const char *str)
{
    log_message(NAME, L_NOTICE, "%s\r\n", str);
}

void myclient_connect_callback(struct mosquitto *mosq, void *obj, int result)
{
    if(!result) {
        log_message(NAME, L_NOTICE, "connect ok\n");
        mosquitto_subscribe(mosq, NULL, TOPIC_TEST1, TOPIC_QOS);
    }
}

void myclient_disconnect_callback(struct mosquitto *mosq, void *obj, int result)
{
    log_message(NAME, L_NOTICE, "connect lost\r\n");
}

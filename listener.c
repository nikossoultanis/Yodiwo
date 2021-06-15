#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include "MQTTClient.h"
//#include "log.h"
#define ADDRESS     "ssl://api.yodiwo.com:8883"
#define CLIENTID    "NikosClient"
#define TOPIC       "/teltonika/1111275868/config_request/"
#define PAYLOAD     "Sending trial message"
#define QOS         0
#define TIMEOUT     10000L

volatile MQTTClient_deliveryToken deliveredtoken;
MQTTClient client;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    char buffer [1024];
    char* payloadptr;

    //All json fields that we need
    struct json_object *parsed_json;
    struct json_object *id;
    struct json_object *addSchedule;
    struct json_object *schedule;
    struct json_object *schedule_items;
    struct json_object *period;
    struct json_object *scheduleKey;
    struct json_object *disabled;
    struct json_object *modbus_array;
    struct json_object *slaveId;
    struct json_object *type;
    struct json_object *startReg;
    struct json_object *value;
    struct json_object *mulValues;
    struct json_object *numBytes;
    struct json_object *write;

    int i;
    size_t schedule_len;

    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: \n");
    payloadptr = message->payload;
    //save the incoming message to local buffer
    for(i=0; i<message->payloadlen; i++)
    {
        buffer[i] = *payloadptr++;
        putchar(buffer[i]);
    }

    parsed_json = json_tokener_parse(buffer);
    //validating the incoming json format
    if(json_object_object_get_ex(parsed_json, "Id", &id))
        ;
    else
        return -1; // return and wait the next incoming message
    if(json_object_object_get_ex(parsed_json, "AddSchedule", &addSchedule))
        ;
    else
        return -1;
    if(json_object_object_get_ex(addSchedule, "Schedules", &schedule))
        ;
    else
        return -1;

    schedule_len = json_object_array_length(schedule);
    int mul_values_counter;

    // iterate through all the Schedule array
    for(i=0; i<schedule_len;i++)
    {
        json_object * jvalue;
        jvalue = json_object_array_get_idx(schedule,i);

        json_object_object_get_ex(jvalue, "ScheduleKey", &scheduleKey);
        json_object_object_get_ex(jvalue, "Period", &period);
        json_object_object_get_ex(jvalue, "Disabled", &disabled);

        json_object_object_get_ex(jvalue, "ModbusMasterCmd", &modbus_array);
        json_object_object_get_ex(modbus_array, "SlaveId", &slaveId);
        json_object_object_get_ex(modbus_array, "Type", &type);
        json_object_object_get_ex(modbus_array, "StartReg", &startReg);
        json_object_object_get_ex(modbus_array, "Value", &value);
        json_object_object_get_ex(modbus_array, "MulValues", &mulValues);
        json_object_object_get_ex(modbus_array, "NumBytes", &numBytes);
        json_object_object_get_ex(modbus_array, "Write", &write);

        printf("\nThe ID is: %d", json_object_get_int(id));
        printf("\nSchedule Key is: %s", json_object_get_string(scheduleKey));
        printf("\nPeriod is: %d", json_object_get_int(period));
        printf("\nDisabled is: %s", json_object_get_string(disabled));
        printf("\nSlave Id is: %d", json_object_get_int(slaveId));
        printf("\nType is: %d", json_object_get_int(type));
        printf("\nStartReg: %d", json_object_get_int(startReg));
        printf("\nValue is: %d", json_object_get_int(value));
        for(mul_values_counter=0; mul_values_counter<json_object_array_length(mulValues); mul_values_counter++)
        {
            printf("\nMulValues[%d] = %d", mul_values_counter,json_object_get_int(json_object_array_get_idx(mulValues, mul_values_counter)));
        }
        if(json_object_array_length(mulValues) == 0)
        {
            printf("\nMulValues = null");
        }
            printf("\nNumBytes is: %d", json_object_get_int(numBytes));
            printf("\nWrite is: %s", json_object_get_string(write));
            printf("\n");
    }
    MQTTClient_deliveryToken token;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;

    pubmsg.payload = "message reply message reply message reply"; // needs to be the JSON reply
    pubmsg.payloadlen = strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    deliveredtoken = 0;
    MQTTClient_publishMessage(client, "/teltonika/1111275868/config_response/", &pubmsg, &token);

    putchar('\n');
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    
    return 1;
}
void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}
int main(int argc, char* argv[])
{
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;


    // MQTTClient_deliveryToken token;
    int rc;
    int ch;
    if (MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL) == MQTTCLIENT_SUCCESS)
    {
        printf("Client is created with Address: %s \n", ADDRESS);
        printf("Client ID: %s \n", CLIENTID);
    }
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username = "1111275868";
    conn_opts.password = "test123!!!";
    conn_opts.ssl = &ssl_opts;

    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
    printf("Passed the Callbacks \n");

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);

    // pubmsg.payload = PAYLOAD;
    // pubmsg.payloadlen = strlen(PAYLOAD);
    // pubmsg.qos = QOS;
    // pubmsg.retained = 0;
    // deliveredtoken = 0;
    //log_message("123", 2, "hhe");
    // MQTTClient_publishMessage(client, "/teltonika/1111275868/config_response/", &pubmsg, &token);
    // printf("Waiting for publication of %s\n"
    //     "on topic %s for client with ClientID: %s\n",
    //     PAYLOAD, TOPIC, CLIENTID);

    MQTTClient_subscribe(client, TOPIC, QOS);
    do
    {
        ch = getchar();
    } while(ch!='Q' && ch != 'q');

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);

    return rc;
}
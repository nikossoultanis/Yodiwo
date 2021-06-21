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
#include <mosquitto.h>
#include "log.h"
#include "external.h"
#include <liblog.h>
#include "ydwjsoncli.h"

void json_decoder(const struct mosquitto_message *message)
{
    size_t schedule_len;
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
    int mul_values_counter;
    int i;
    log_message(NAME, L_NOTICE,"Starting the message parsing \n\r");
    printf("Starting the parsing \n\r");
	parsed_json = json_tokener_parse(message->payload);
    printf("Parsed the message payload\n\r");
    log_message(NAME, L_NOTICE,"Finished the parsing \n\r");

    if(json_object_object_get_ex(parsed_json, "Id", &id))
        ; // do nothing and continue to the next check
    else
    {
        printf("Incorrect format. Aborting\n\r");
        log_message(NAME, L_NOTICE,"JSON format is incorrect. Aborting \n\r");
        return;
    } // return and wait the next incoming message
    if(json_object_object_get_ex(parsed_json, "AddSchedule", &addSchedule))
        ;
    else
    {
        printf("Incorrect format. Aborting\n\r");
        log_message(NAME, L_NOTICE,"JSON format is incorrect. Aborting \n\r");
        return;
    }
    if(json_object_object_get_ex(addSchedule, "Schedules", &schedule))
        ;
    else
    {
        printf("Incorrect format. Aborting\n\r");
        log_message(NAME, L_NOTICE,"JSON format is incorrect. Aborting \n\r");
        return;
    }
    schedule_len = json_object_array_length(schedule);

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
    free(scheduleKey);
    free(period);
    free(disabled);
    free(modbus_array);
    free(slaveId);
    free(type);
    free(startReg);
    free(value);
    free(mulValues);
    free(numBytes);
    free(write);
}

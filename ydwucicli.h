#ifndef _YODIWO_CONFIG_H__
#define _YODIWO_CONFIG_H__

#include <uci.h>
#include <time.h>

#include "yodiwo.h"

struct rs485_package
{
    int *id;
    // struct json_object *addSchedule;
    // struct json_object *schedule;
    // struct json_object *schedule_items;
    int *period;
    int *scheduleKey;
    bool *disabled;
    // struct json_object *modbus_array;
    int *slaveId;
    int *type;
    int *startReg;
    int *value;
    // struct json_object *mulValues;
    int *numBytes;
    bool *write;
};
#endif
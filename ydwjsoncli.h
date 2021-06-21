#ifndef YDW_JSON_CLI_H
#define YDW_JSON_CLI_H

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <mosquitto.h>
#include "json.h"



//printf(__VA_ARGS__);
void json_decoder(const struct mosquitto_message *message);
#endif

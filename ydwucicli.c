#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "config.h"
#include "log.h"
#include "yodiwo.h"
#include <uci.h>
static bool first_run = true;
static struct uci_context *uci_ctx;
static struct uci_package *uci_rs485;
static struct uci_package *uci_yodiwo;

//static struct rs485_pakage *uci_commands;
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

struct rs485_package *rs_config;
static int config_init_local(void)
{
	struct uci_section *s;
	struct uci_element *e1;

	uci_foreach_element(&uci_yodiwo->sections, e1) {
		s = uci_to_section(e1);
		if (strcmp(s->type, "local") == 0) {
			config_free_local();

			config->local->logging_level = DEFAULT_LOGGING_LEVEL;
			uci_foreach_element(&s->options, e1) {
				if (!strcmp((uci_to_option(e1))->e.name, "interface")) {
					config->local->interface = strdup(uci_to_option(e1)->v.string);
					DD("yodiwo.@local[0].interface=%s\n", config->local->interface);
					continue;
				}

				if (!strcmp((uci_to_option(e1))->e.name, "port")) {
					if (!atoi((uci_to_option(e1))->v.string)) {
						D("in section local port has invalid value...\n");
						return -1;
					}
					config->local->port = strdup(uci_to_option(e1)->v.string);
					DD("yodiwo.@local[0].port=%s\n", config->local->port);
					continue;
				}

				if (!strcmp((uci_to_option(e1))->e.name, "username")) {
					config->local->username = strdup(uci_to_option(e1)->v.string);
					DD("yodiwo.@local[0].username=%s\n", config->local->username);
					continue;
				}

				if (!strcmp((uci_to_option(e1))->e.name, "password")) {
					config->local->password = strdup(uci_to_option(e1)->v.string);
					DD("yodiwo.@local[0].password=%s\n", config->local->password);
					continue;
				}

				if (!strcmp((uci_to_option(e1))->e.name, "ubus_socket")) {
					config->local->ubus_socket = strdup(uci_to_option(e1)->v.string);
					DD("yodiwo.@local[0].ubus_socket=%s\n", config->local->ubus_socket);
					continue;
				}
				
				if (!strcmp((uci_to_option(e1))->e.name, "logging_level")) {
					char *c;
					int log_level = atoi((uci_to_option(e1))->v.string);					 
					asprintf(&c, "%d", log_level);
					if (!strcmp(c, uci_to_option(e1)->v.string)) 
						config->local->logging_level = log_level;						
					free(c);
					DD("yodiwo.@local[0].logging_level=%d\n", config->local->logging_level);
					continue;
				}
			}

			if (!config->local->interface) {
				D("in local you must define interface\n");
				return -1;
			}

			if (!config->local->port) {
				D("in local you must define port\n");
				return -1;
			}

			if (!config->local->ubus_socket) {
				D("in local you must define ubus_socket\n");
				return -1;
			}

			return 0;
		}
	}
	D("uci section local not found...\n");
	return -1;
}

static struct uci_package *
config_init_package(const char *c)
{
	if (first_run) {
		rs_config = calloc(1, sizeof(struct rs485_package));
		if (!rs_config)
        {
            goto error;
        }
	}
	if (!uci_ctx) {
		uci_ctx = uci_alloc_context();
		if (!uci_ctx) goto error;
	} else {
		if (uci_rs485) {
			uci_unload(uci_ctx, uci_rs485);
			uci_rs485 = NULL;
		}
	}
	if (uci_load(uci_ctx, c, &uci_rs485)) {
		uci_free_context(uci_ctx);
		uci_ctx = NULL;
        printf("Returning NULL \n\r");
		return NULL;
	}
	return uci_rs485;

error:
    printf("went to error \n\r");
	//config_exit();
	return NULL;
}

static inline void config_free_ctx(void)
{
	if (uci_ctx) {
		if (uci_rs485) {
			uci_unload(uci_ctx, uci_rs485);
			uci_rs485 = NULL;
		}
		uci_free_context(uci_ctx);
		uci_ctx = NULL;
	}
}

void config_load_package(void)
{
	uci_rs485 = config_init_package("rs485");
    if (!uci_rs485) goto error;
	//if () goto error;

	//backup_check_yodiwo_url();
	//cwmp_periodic_inform_init();

	first_run = false;
	//config_free_ctx();

	//cwmp_update_value_change();
	return;

error:
	log_message(NAME, L_CRIT, "configuration (re)loading failed, exit daemon\n");
	D("configuration (re)loading failed\n"); 
	exit(EXIT_FAILURE);
}


void config_init(void)
{
    //uci_rs485 = config_init_package("rs485");
    config_init_local();
    // printf("test if its empty. \n\r");
    // if(uci_list_empty(&uci_rs485->sections))
    // {
    //     printf("Empty list. \n\r");
    // }else
    //     printf("not empty \n\r");
    return;
}
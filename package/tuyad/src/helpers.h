#ifndef HELPERS_H
#define HELPERS_H

#include <uci.h>

#include "cJSON.h"
#include "cpu.h"
#include "network_interface.h"
#include "system.h"
#include "error_codes.h"
#include <syslog.h>

int help_int_add_to_json(cJSON *root, char *str, int value);
int help_str_add_to_json(cJSON *root, char *str, char *value);
char *get_data_json(struct ubus_context *ctx);
int config_set_send_now_false(void);

#endif
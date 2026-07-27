#ifndef CONFIG_H
#define CONFIG_H

#include <uci.h>
#include <string.h>
#include "error_codes.h"

extern char g_cfg_product_id[17];
extern char g_cfg_device_id[23];
extern char g_cfg_device_secret[17];
extern int g_cfg_interval;
extern int g_cfg_send_now;

int config_load_uci();

#endif
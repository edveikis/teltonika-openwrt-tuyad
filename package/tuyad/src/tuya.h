#ifndef TUYA_H
#define TUYA_H

#include <stdio.h>

#include "cJSON.h"
#include "error_codes.h"
#include "tuya_log.h"
#include "tuya_error_code.h"
#include "system_interface.h"
#include "mqtt_client_interface.h"
#include "tuyalink_core.h"
#include "user_input.h"
#include "file_reader.h"
#include <syslog.h>

extern tuya_mqtt_context_t client_instance;

void tuya_on_connected(tuya_mqtt_context_t* context, void* user_data);
void tuya_on_disconnect(tuya_mqtt_context_t* context, void* user_data);
void tuya_on_messages(tuya_mqtt_context_t* context, void* user_data, const tuyalink_message_t* msg);

#endif
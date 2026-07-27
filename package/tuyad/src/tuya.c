#include "tuya.h"

int validate_action(const char *action, cJSON *msg_data) 
{
    cJSON* action_code = cJSON_GetObjectItem(msg_data, "actionCode");
    
    if (action_code && cJSON_IsString(action_code) && strcmp(action_code->valuestring, action) == 0) {
        return APP_SUCCESS;
    }
    
    return APP_FAILURE;
}

int save_log(cJSON *msg_data)
{
    cJSON* params   = cJSON_GetObjectItem(msg_data, "inputParams");
    cJSON* log_text = cJSON_GetObjectItem(params, "text");

    if (log_text && log_text->valuestring) {
        if (fr_append("/tmp/tuya_action.log", log_text->valuestring) == APP_SUCCESS) {
            return APP_SUCCESS;
        }
    }

    return APP_FAILURE;
}

tuya_mqtt_context_t client_instance;

void tuya_on_connected(tuya_mqtt_context_t* context, void* user_data) {}

void tuya_on_disconnect(tuya_mqtt_context_t* context, void* user_data) {}

void tuya_on_messages(tuya_mqtt_context_t* context, void* user_data, const tuyalink_message_t* msg) 
{
    if (msg->type != THING_TYPE_ACTION_EXECUTE) {
        return;
    }

    cJSON* root = cJSON_Parse(msg->data_string);
    if (!root) {
        return;
    }

    if (validate_action("save_log", root) == APP_SUCCESS) {
        if (save_log(root) != APP_SUCCESS) {
            syslog(LOG_ERR, "Failed to complete save_log action");
        }
    }

    cJSON_Delete(root);
}
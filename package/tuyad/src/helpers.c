#include "helpers.h"

int help_int_add_to_json(cJSON *root, char *str, int value)
{
    cJSON *new_obj = cJSON_CreateObject();
    if (new_obj == NULL) {
        return APP_FAILURE;
    }

    cJSON_AddNumberToObject(new_obj, "value", value);
    cJSON_AddItemToObject(root, str, new_obj);

    return APP_SUCCESS;
}

int help_str_add_to_json(cJSON *root, char *str, char *value)
{
    cJSON *new_obj = cJSON_CreateObject();
    if (new_obj == NULL) {
        return APP_FAILURE;
    }

    cJSON_AddStringToObject(new_obj, "value", value);
    cJSON_AddItemToObject(root, str, new_obj);

    return APP_SUCCESS;
}


char *get_data_json(struct ubus_context *ctx)
{
    cJSON *root = cJSON_CreateObject();

    double cpu_load = cpu_usage(100); // sampling time: 100ms
    if (cpu_load == NO_VALUE) {
        syslog(LOG_ERR, "Failed to get CPU usage");
    }
    else {
        if (help_int_add_to_json(root, "cpu_usage", (int)cpu_load) == APP_FAILURE) {
            syslog(LOG_ERR, "Failed to add cpu_usage to root json");
        }
    }

    struct Info info = { 0 };
	int res = system_get_info(ctx, &info);
    if (res == APP_FAILURE) {
        syslog(LOG_ERR, "Failed to get data from the UBUS");
    }
    else {
        if (help_int_add_to_json(root, "total_ram", info.mem_total) == APP_FAILURE) {
            syslog(LOG_ERR, "Failed to add total_ram to root json");
        }
        if (help_int_add_to_json(root, "free_ram", info.mem_free) == APP_FAILURE) {
            syslog(LOG_ERR, "Failed to add free_ram to root json");
        }
        if (help_int_add_to_json(root, "uptime", info.uptime) == APP_FAILURE) {
            syslog(LOG_ERR, "Failed to add uptime to root json");
        }
    }

    if (ni_get_info(ctx) != APP_SUCCESS) {
        cJSON_Delete(root); 
        return NULL;
    }

    char *net_json = ni_to_json();
    if (net_json == NULL) {
        syslog(LOG_ERR, "Failed to convert network interface data to json");
    }
    else {
        if (help_str_add_to_json(root, "net_interfaces", net_json) == APP_FAILURE) {
            syslog(LOG_ERR, "Failed to add net_interfaces to root json");
        }
    }

    char *data = cJSON_PrintUnformatted(root);

    cJSON_Delete(root);
    free(net_json);

    return data;
}

int config_set_send_now_false(void)
{
    struct uci_context *ctx = uci_alloc_context();
    if (!ctx) {
        return APP_FAILURE;
    }

    struct uci_package *pkg = NULL;
    if (uci_load(ctx, "tuyad", &pkg) != UCI_OK) {
        uci_free_context(ctx);
        return APP_FAILURE;
    }

    struct uci_section *s = uci_lookup_section(ctx, pkg, "main");
    if (!s) {
        uci_unload(ctx, pkg);
        uci_free_context(ctx);
        return APP_FAILURE;
    }

    struct uci_ptr ptr = {
        .p = pkg,
        .s = s,
        .option = "send_now",
        .value = "0",
    };

    if (uci_set(ctx, &ptr) != UCI_OK) {
        uci_unload(ctx, pkg);
        uci_free_context(ctx);
        return APP_FAILURE;
    }

    if (uci_commit(ctx, &pkg, false) != UCI_OK) {
        uci_unload(ctx, pkg);
        uci_free_context(ctx);
        return APP_FAILURE;
    }

    uci_unload(ctx, pkg);
    uci_free_context(ctx);

    return APP_SUCCESS;
}
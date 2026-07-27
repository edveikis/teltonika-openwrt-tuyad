#include "config.h"

char g_cfg_product_id[17] = {0};
char g_cfg_device_id[23] = {0};
char g_cfg_device_secret[17] = {0};

int g_cfg_interval = 0;
int g_cfg_send_now = 0;

static void load_string_option(struct uci_context *uci, struct uci_section *sec, const char *option, char *dst, size_t dst_size)
{
    const char *val = uci_lookup_option_string(uci, sec, option);
    if (val) {
        strncpy(dst, val, dst_size - 1);
        dst[dst_size - 1] = '\0';
    }
}

static void load_int_option(struct uci_context *uci, struct uci_section *sec, const char *option, int *dst)
{
    const char *val = uci_lookup_option_string(uci, sec, option);
    if (val) {
        *dst = atoi(val);
    }
}

int config_load_uci(void)
{
    struct uci_context *uci = uci_alloc_context();
    if (!uci) {
        return APP_FAILURE;
    }

    struct uci_package *pkg = NULL;
    if (uci_load(uci, "tuyad", &pkg) != UCI_OK) {
        uci_free_context(uci);
        return APP_FAILURE;
    }

    struct uci_section *sec = uci_lookup_section(uci, pkg, "main");
    if (!sec) {
        uci_unload(uci, pkg);
        uci_free_context(uci);
        return APP_FAILURE;
    }

    load_string_option(uci, sec, "dev_id", g_cfg_device_id, sizeof(g_cfg_device_id));
    load_string_option(uci, sec, "dev_secret", g_cfg_device_secret, sizeof(g_cfg_device_secret));
    load_string_option(uci, sec, "product_id", g_cfg_product_id, sizeof(g_cfg_product_id));

    load_int_option(uci, sec, "interval", &g_cfg_interval);
    load_int_option(uci, sec, "send_now", &g_cfg_send_now);
   
    uci_unload(uci, pkg);
    uci_free_context(uci);

    return APP_SUCCESS;
}
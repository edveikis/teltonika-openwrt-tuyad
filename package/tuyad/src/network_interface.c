#include "network_interface.h"

struct Interface *net = NULL;
int size = 0;

int ni_get_names_uci()
{
    struct uci_context *uci = uci_alloc_context();
    struct uci_package *p = NULL;
    struct uci_element *e;
    struct Interface *i;
    int count = 0;

    if (uci == NULL) {
        return APP_FAILURE;
    }

    if (uci_load(uci, "network", &p) != UCI_OK) {
        uci_free_context(uci);
        return APP_FAILURE;
    }

    i = malloc(sizeof(struct Interface) * 20);
    if (i == NULL) {
        uci_unload(uci, p);
        uci_free_context(uci);
        return APP_FAILURE;
    }

    uci_foreach_element(&p->sections, e) {
        if (count >= 20) {
            break;
        } 

        struct uci_section *sec = uci_to_section(e);

        if (strcmp(sec->type, "interface") != 0) {
            continue;
        }
        
        if (strcmp(sec->e.name, "loopback") == 0) {
            continue;
        }

        if (count >= 20) {
            break;
        }

        strncpy(i[count].name, sec->e.name, 127);
        i[count].name[127] = '\0';

        ++count;
    }

    uci_unload(uci, p);
    uci_free_context(uci);

    free(net);
    net = i;
    size = count;

    return APP_SUCCESS;
}

enum {
    ATTR_DEVICE,
    ATTR_IPV4_ADDRESS,
    __ATTR_MAX
};

enum { 
    ADDR_ADDRESS, 
    ADDR_MASK, 
    __ADDR_MAX 
};

static const struct blobmsg_policy policy[__ATTR_MAX] = {
    [ATTR_DEVICE] = { "device", BLOBMSG_TYPE_STRING },
    [ATTR_IPV4_ADDRESS] = { "ipv4-address", BLOBMSG_TYPE_ARRAY },
};

static const struct blobmsg_policy apol[__ADDR_MAX] = {
    [ADDR_ADDRESS] = { "address", BLOBMSG_TYPE_STRING },
    [ADDR_MASK]    = { "mask",    BLOBMSG_TYPE_INT32 },
};

static void ni_status_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
    struct Interface *itf = (struct Interface*)req->priv;
    struct blob_attr *tb[__ATTR_MAX];
    struct blob_attr *cur;
    int rem;

    blobmsg_parse(policy, __ATTR_MAX, tb, blob_data(msg), blob_len(msg));

    if (!tb[ATTR_DEVICE])
        return;

    if (!tb[ATTR_IPV4_ADDRESS])
        return;

    blobmsg_for_each_attr(cur, tb[ATTR_IPV4_ADDRESS], rem) {
        struct blob_attr *atb[__ADDR_MAX];

        blobmsg_parse(apol, __ADDR_MAX, atb, blobmsg_data(cur), blobmsg_data_len(cur));

        if (atb[ADDR_ADDRESS] && atb[ADDR_MASK]) {
            const char *ip = blobmsg_get_string(atb[ADDR_ADDRESS]);
            int mask = blobmsg_get_u32(atb[ADDR_MASK]);

            strncpy(itf->ip_address, ip, 15);
            itf->ip_address[15] = '\0';
            itf->mask_tmp = mask;
        }
    }

	const char *dev = blobmsg_get_string(tb[ATTR_DEVICE]);
    strncpy(itf->device, dev, 15);
    itf->device[15] = '\0';
}

int ni_get_status(struct ubus_context *ctx, struct Interface *out, char *name)
{
    uint32_t dev_id;
	struct Interface info = { 0 };
    char full_name[64];

    int n = snprintf(full_name, sizeof(full_name), "network.interface.%s", name);
    if (n < 0 || (size_t)n >= sizeof(full_name)) {
        return APP_FAILURE;
    }

	if (ubus_lookup_id(ctx, full_name, &dev_id) || ubus_invoke(ctx, dev_id, "status", NULL, ni_status_cb, &info, 3000)) {
		return APP_FAILURE;
	}

	*out = info;
	return APP_SUCCESS;
}

enum {
    DEV_STATS,
    __DEV_MAX
};

static const struct blobmsg_policy dev_policy[__DEV_MAX] = {
    [DEV_STATS] = { "statistics", BLOBMSG_TYPE_TABLE },
};

enum {
    RX_BYTES,
    TX_BYTES,
    __STAT_MAX
};

static const struct blobmsg_policy stat_policy[__STAT_MAX] = {
    [RX_BYTES] = { "rx_bytes", BLOBMSG_TYPE_INT64 },
    [TX_BYTES] = { "tx_bytes", BLOBMSG_TYPE_INT64 },
};

static void ni_device_stats_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
    struct Interface *itf = (struct Interface*)req->priv;
    struct blob_attr *tb[__DEV_MAX];
    struct blob_attr *stb[__STAT_MAX];

    blobmsg_parse(dev_policy, __DEV_MAX, tb, blob_data(msg), blob_len(msg));

    if (!tb[DEV_STATS])
        return;

    blobmsg_parse(stat_policy, __STAT_MAX, stb, blobmsg_data(tb[DEV_STATS]), blobmsg_data_len(tb[DEV_STATS]));

    if (stb[RX_BYTES])
        itf->rx = blobmsg_get_u64(stb[RX_BYTES]);

    if (stb[TX_BYTES])
        itf->tx = blobmsg_get_u64(stb[TX_BYTES]);
}

int ni_get_device_stats(struct ubus_context *ctx, struct Interface *out)
{
    uint32_t dev_id;
    struct blob_buf b = {0};

    if (ubus_lookup_id(ctx, "network.device", &dev_id)) {
        return APP_FAILURE;
    }

    blob_buf_init(&b, 0);
    blobmsg_add_string(&b, "name", out->device); // specify name, we dont need everything

    if (ubus_invoke(ctx, dev_id, "status", b.head, ni_device_stats_cb, out, 3000)) {
        blob_buf_free(&b);
        return APP_FAILURE;
    }

    blob_buf_free(&b);
    return APP_SUCCESS;
}

// TODO: DOC it, move to helper?
void mask_to_netmask(int prefix, char *out, size_t out_size)
{
    uint32_t mask;

    if (prefix <= 0) {
        mask = 0;
    } 
    else if (prefix >= 32) {
        mask = 0xFFFFFFFFu;
    } 
    else {
        mask = ~((1u << (32 - prefix)) - 1);
    }

    snprintf(out, out_size, "%u.%u.%u.%u",
              (mask >> 24) & 0xFF,
              (mask >> 16) & 0xFF,
              (mask >> 8)  & 0xFF,
              mask & 0xFF);
}

int ni_get_info(struct ubus_context *ctx)
{
    if  (ni_get_names_uci() != APP_SUCCESS) {
        return APP_FAILURE;
    }

    for (int i = 0; i < size; ++i) {
        
        struct Interface iff = {0};
        if (ni_get_status(ctx, &iff, net[i].name) != APP_SUCCESS) {
            continue;
        }

        if (ni_get_device_stats(ctx, &iff) != APP_SUCCESS) {
            continue;
        }

        strncpy(net[i].device, iff.device, 128);
        net[i].device[127] = '\0';

        strncpy(net[i].ip_address, iff.ip_address, 15);
        net[i].ip_address[15] = '\0';

        net[i].mask_tmp = iff.mask_tmp;
        mask_to_netmask(net[i].mask_tmp, net[i].netmask, sizeof(net[i].netmask));

        net[i].tx = iff.tx;
        net[i].rx = iff.rx;
    }

    return APP_SUCCESS;
}

char *ni_to_json()
{
    cJSON *root = cJSON_CreateArray();
    if (root == NULL) {
        return NULL;
    }

    for (int i = 0; i < size; ++i) {
        cJSON *iface = cJSON_CreateObject();
        if (iface == NULL) {
            continue;
        }

        cJSON_AddStringToObject(iface, "name", net[i].name);
        cJSON_AddStringToObject(iface, "ip", net[i].ip_address);
        cJSON_AddStringToObject(iface, "netmask", net[i].netmask);
        cJSON_AddNumberToObject(iface, "tx_bytes", net[i].tx);
        cJSON_AddNumberToObject(iface, "rx_bytes", net[i].rx);

        cJSON_AddItemToArray(root, iface);
    }

    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    return json_str;
}

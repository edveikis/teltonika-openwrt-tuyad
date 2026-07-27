#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

#include <stdlib.h>
#include <string.h>
#include <uci.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>

#include "error_codes.h"
#include "cJSON.h"

struct Interface {
    char name[128];
    char device[128];
    char ip_address[16];
    char netmask[16];
    uint64_t tx;
    uint64_t rx;
    int mask_tmp;
};

extern struct Interface *net;
extern int size;

int ni_get_info(struct ubus_context *ctx);
char *ni_to_json();

#endif
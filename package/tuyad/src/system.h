#ifndef SYSTEM_H
#define SYSTEM_H

#include <libubox/blobmsg_json.h>
#include <libubus.h>

#include "error_codes.h"

struct Info {
	int mem_total;
	int mem_free;
    int uptime;
};

enum {
	TOTAL_MEMORY,
	FREE_MEMORY,
	__MEMORY_MAX,
};

enum {
	MEMORY_DATA,
    INFO_UPTIME,
	__INFO_MAX,
};

extern struct blobmsg_policy memory_policy[__MEMORY_MAX];
extern struct blobmsg_policy info_policy[__INFO_MAX];

int system_get_info(struct ubus_context *ctx, struct Info *out);

#endif
#include "system.h"

struct blobmsg_policy memory_policy[__MEMORY_MAX] = {
	[TOTAL_MEMORY]	  = { .name = "total", .type = BLOBMSG_TYPE_INT64 },
	[FREE_MEMORY]	  = { .name = "free", .type = BLOBMSG_TYPE_INT64 },
};

struct blobmsg_policy info_policy[__INFO_MAX] = {
	[MEMORY_DATA] = { .name = "memory", .type = BLOBMSG_TYPE_TABLE },
	[INFO_UPTIME] = { .name = "uptime", .type = BLOBMSG_TYPE_INT32 },
};

void system_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	struct Info *info = (struct Info *)req->priv;
	struct blob_attr *tb[__INFO_MAX];
	struct blob_attr *memory[__MEMORY_MAX];

	blobmsg_parse(info_policy, __INFO_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[INFO_UPTIME]) {
		return;
	}
	
	if (!tb[MEMORY_DATA]) {
		return;
	}
	
	blobmsg_parse(
		memory_policy, 
		__MEMORY_MAX, 
		memory, 
        blobmsg_data(tb[MEMORY_DATA]), 
		blobmsg_data_len(tb[MEMORY_DATA])
	);
		
	if (!memory[TOTAL_MEMORY] || !memory[FREE_MEMORY]) {
		return;
	}
	
	info->mem_total = blobmsg_get_u64(memory[TOTAL_MEMORY]);
	info->mem_free = blobmsg_get_u64(memory[FREE_MEMORY]);
	info->uptime = blobmsg_get_u32(tb[INFO_UPTIME]);
}

int system_get_info(struct ubus_context *ctx, struct Info *out)
{
	uint32_t id;
	struct Info info = { 0 };

	if (ubus_lookup_id(ctx, "system", &id) ||
	    ubus_invoke(ctx, id, "info", NULL, system_cb, &info, 3000)) {
		return APP_FAILURE;
	}

	*out = info;
	return APP_SUCCESS;
}
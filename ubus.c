/*
 * Copyright (C) 2022 John Crispin <john@phrozen.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <libubus.h>

#include "db.h"

static struct ubus_auto_conn conn;
static struct blob_buf b;

enum device_add_attr {
	DEVICE_ADD_SERIAL,
	DEVICE_ADD_COMPAT,
	DEVICE_ADD_MAX,
};

static const struct blobmsg_policy device_add_policy[DEVICE_ADD_MAX] = {
	[DEVICE_ADD_SERIAL]	= { "serial", BLOBMSG_TYPE_STRING },
	[DEVICE_ADD_COMPAT]	= { "compatible", BLOBMSG_TYPE_STRING },
};

static int
ubus_device_add(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[DEVICE_ADD_MAX];

	blobmsg_parse(device_add_policy, DEVICE_ADD_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[DEVICE_ADD_SERIAL] || !tb[DEVICE_ADD_COMPAT])
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (device_add(blobmsg_get_string(tb[DEVICE_ADD_SERIAL]),
		       blobmsg_get_string(tb[DEVICE_ADD_COMPAT])))
		return UBUS_STATUS_INVALID_ARGUMENT;

	return UBUS_STATUS_OK;
}

enum device_remove_attr {
	DEVICE_REMOVE_SERIAL,
	DEVICE_REMOVE_MAX,
};

static const struct blobmsg_policy device_remove_policy[DEVICE_REMOVE_MAX] = {
	[DEVICE_REMOVE_SERIAL]= { "serial", BLOBMSG_TYPE_STRING },
};

static int
ubus_device_remove(struct ubus_context *ctx, struct ubus_object *obj,
		   struct ubus_request_data *req, const char *method,
		   struct blob_attr *msg)
{
	struct blob_attr *tb[DEVICE_REMOVE_MAX];

	blobmsg_parse(device_remove_policy, DEVICE_REMOVE_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[DEVICE_REMOVE_SERIAL])
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (device_remove(blobmsg_get_string(tb[DEVICE_ADD_SERIAL])))
		return UBUS_STATUS_INVALID_ARGUMENT;

	return UBUS_STATUS_OK;
}

static int
ubus_device_list(struct ubus_context *ctx, struct ubus_object *obj,
		 struct ubus_request_data *req, const char *method,
		 struct blob_attr *msg)
{
	if (device_list(&b))
		return UBUS_STATUS_INVALID_ARGUMENT;

	ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;
}

enum state_add_attr {
	STATE_ADD_SERIAL,
	STATE_ADD_BLOB,
	STATE_ADD_MAX,
};

static const struct blobmsg_policy state_add_policy[STATE_ADD_MAX] = {
	[STATE_ADD_SERIAL]	= { "serial", BLOBMSG_TYPE_STRING },
	[STATE_ADD_BLOB]	= { "state", BLOBMSG_TYPE_TABLE },
};

static int
ubus_state_add(struct ubus_context *ctx, struct ubus_object *obj,
	       struct ubus_request_data *req, const char *method,
	       struct blob_attr *msg)
{
	struct blob_attr *tb[STATE_ADD_MAX];

	blobmsg_parse(state_add_policy, STATE_ADD_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[STATE_ADD_SERIAL] || !tb[STATE_ADD_BLOB])
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (state_add(blobmsg_get_string(tb[DEVICE_ADD_SERIAL]),
		      tb[STATE_ADD_BLOB]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	return UBUS_STATUS_OK;
}

enum state_list_attr {
	STATE_LIST_SERIAL,
	STATE_LIST_ROWS,
	STATE_LIST_MAX,
};

static const struct blobmsg_policy state_list_policy[STATE_ADD_MAX] = {
	[STATE_LIST_SERIAL]	= { "serial", BLOBMSG_TYPE_STRING },
	[STATE_LIST_ROWS]	= { "rows", BLOBMSG_TYPE_INT32 },
};

static int
ubus_state_list(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[STATE_LIST_MAX];

	blobmsg_parse(state_list_policy, STATE_LIST_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[STATE_LIST_SERIAL] || !tb[STATE_LIST_ROWS])
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (state_list(&b, blobmsg_get_string(tb[STATE_LIST_SERIAL]),
		       blobmsg_get_u32(tb[STATE_LIST_ROWS])))
		return UBUS_STATUS_INVALID_ARGUMENT;

	ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;
}

enum health_add_attr {
	HEALTH_ADD_SERIAL,
	HEALTH_ADD_BLOB,
	HEALTH_ADD_MAX,
};

static const struct blobmsg_policy health_add_policy[HEALTH_ADD_MAX] = {
	[HEALTH_ADD_SERIAL]	= { "serial", BLOBMSG_TYPE_STRING },
	[HEALTH_ADD_BLOB]	= { "health", BLOBMSG_TYPE_TABLE },
};

static int
ubus_health_add(struct ubus_context *ctx, struct ubus_object *obj,
	       struct ubus_request_data *req, const char *method,
	       struct blob_attr *msg)
{
	struct blob_attr *tb[HEALTH_ADD_MAX];

	blobmsg_parse(health_add_policy, HEALTH_ADD_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[HEALTH_ADD_SERIAL] || !tb[HEALTH_ADD_BLOB])
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (health_add(blobmsg_get_string(tb[DEVICE_ADD_SERIAL]),
		      tb[HEALTH_ADD_BLOB]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	return UBUS_STATUS_OK;
}

enum health_list_attr {
	HEALTH_LIST_SERIAL,
	HEALTH_LIST_ROWS,
	HEALTH_LIST_MAX,
};

static const struct blobmsg_policy health_list_policy[HEALTH_ADD_MAX] = {
	[HEALTH_LIST_SERIAL]	= { "serial", BLOBMSG_TYPE_STRING },
	[HEALTH_LIST_ROWS]	= { "rows", BLOBMSG_TYPE_INT32 },
};

static int
ubus_health_list(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[HEALTH_LIST_MAX];

	blobmsg_parse(health_list_policy, HEALTH_LIST_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[HEALTH_LIST_SERIAL] || !tb[HEALTH_LIST_ROWS])
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (health_list(&b, blobmsg_get_string(tb[HEALTH_LIST_SERIAL]),
		       blobmsg_get_u32(tb[HEALTH_LIST_ROWS])))
		return UBUS_STATUS_INVALID_ARGUMENT;

	ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;
}

enum event_add_attr {
	EVENT_ADD_TYPE,
	EVENT_ADD_SERIAL,
	EVENT_ADD_CLIENT,
	EVENT_ADD_EVENT,
	EVENT_ADD_MAX,
};

static const struct blobmsg_policy event_add_policy[EVENT_ADD_MAX] = {
	[EVENT_ADD_TYPE]	= { "type", BLOBMSG_TYPE_STRING },
	[EVENT_ADD_SERIAL]	= { "serial", BLOBMSG_TYPE_STRING },
	[EVENT_ADD_CLIENT]	= { "client", BLOBMSG_TYPE_STRING },
	[EVENT_ADD_EVENT]	= { "event", BLOBMSG_TYPE_STRING },
};

static int
ubus_event_add(struct ubus_context *ctx, struct ubus_object *obj,
	       struct ubus_request_data *req, const char *method,
	       struct blob_attr *msg)
{
	struct blob_attr *tb[EVENT_ADD_MAX];
	char *serial = NULL, *client = NULL;

	blobmsg_parse(event_add_policy, EVENT_ADD_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[EVENT_ADD_TYPE] || !tb[EVENT_ADD_EVENT])
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (tb[EVENT_ADD_SERIAL])
		serial = blobmsg_get_string(tb[EVENT_ADD_SERIAL]);

	if (tb[EVENT_ADD_CLIENT])
		client = blobmsg_get_string(tb[EVENT_ADD_CLIENT]);

	if (event_add(blobmsg_get_string(tb[EVENT_ADD_TYPE]),
		      serial, client,
		      blobmsg_get_string(tb[EVENT_ADD_EVENT])))
		return UBUS_STATUS_INVALID_ARGUMENT;

	return UBUS_STATUS_OK;
}

enum event_list_attr {
	EVENT_LIST_TYPE,
	EVENT_LIST_SERIAL,
	EVENT_LIST_CLIENT,
	EVENT_LIST_ROWS,
	EVENT_LIST_MAX,
};

static const struct blobmsg_policy event_list_policy[EVENT_LIST_MAX] = {
	[EVENT_LIST_TYPE]	= { "type", BLOBMSG_TYPE_STRING },
	[EVENT_LIST_SERIAL]	= { "serial", BLOBMSG_TYPE_STRING },
	[EVENT_LIST_CLIENT]	= { "client", BLOBMSG_TYPE_STRING },
	[EVENT_LIST_ROWS]	= { "rows", BLOBMSG_TYPE_INT32 },
};

static int
ubus_event_list(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[EVENT_LIST_MAX];
	char *type = NULL, *serial = NULL, *client = NULL;

	blobmsg_parse(event_list_policy, EVENT_LIST_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[EVENT_LIST_ROWS])
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (tb[EVENT_LIST_TYPE])
		type = blobmsg_get_string(tb[EVENT_LIST_TYPE]);

	if (tb[EVENT_LIST_SERIAL])
		serial = blobmsg_get_string(tb[EVENT_LIST_SERIAL]);

	if (tb[EVENT_LIST_CLIENT])
		client = blobmsg_get_string(tb[EVENT_LIST_CLIENT]);

	if (event_list(&b, type, serial, client,
		       blobmsg_get_u32(tb[EVENT_LIST_ROWS])))
		return UBUS_STATUS_INVALID_ARGUMENT;

	ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;
}

static const struct ubus_method urender_methods[] = {
	UBUS_METHOD("device_add", ubus_device_add, device_add_policy),
	UBUS_METHOD("device_remove", ubus_device_remove, device_remove_policy),
	UBUS_METHOD_NOARG("device_list", ubus_device_list),
	UBUS_METHOD("state_add", ubus_state_add, state_add_policy),
	UBUS_METHOD("state_list", ubus_state_list, state_list_policy),
	UBUS_METHOD("health_add", ubus_health_add, health_add_policy),
	UBUS_METHOD("health_list", ubus_health_list, health_list_policy),
	UBUS_METHOD("event_add", ubus_event_add, event_add_policy),
	UBUS_METHOD("event_list", ubus_event_list, event_list_policy),
};

static struct ubus_object_type urender_object_type =
	UBUS_OBJECT_TYPE("urender", urender_methods);

static struct ubus_object urender_object = {
	.name = "urender-db",
	.type = &urender_object_type,
	.methods = urender_methods,
	.n_methods = ARRAY_SIZE(urender_methods),
};

static void
ubus_connect_handler(struct ubus_context *ctx)
{
	int ret;

	ret = ubus_add_object(ctx, &urender_object);
	if (ret)
		fprintf(stderr, "Failed to add object: %s\n", ubus_strerror(ret));
}

void
ubus_startup(void)
{
	conn.cb = ubus_connect_handler;
	ubus_auto_connect(&conn);
}

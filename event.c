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

#include <time.h>

#include "db.h"

int
event_add(char *type, char *serial, char *client, char *event)
{
	char *sql = "INSERT INTO event (type, serial, client, event, timestamp) VALUES(@type, @serial, @client, @event, @timestamp)";
	sqlite3_stmt *stmt;
	int rc;

	db_prepare(rc, stmt, sql);

	db_bind_text(stmt, "@type", type);
	db_bind_text(stmt, "@serial", serial);
	db_bind_text(stmt, "@client", client);
	db_bind_text(stmt, "@event", event);
	db_bind_int64(stmt, "@timestamp", time(NULL));

	return db_insert(stmt);
}

static int
event_list_cb(struct blob_buf *b, sqlite3_stmt *stmt)
{
	void *c = blobmsg_open_array(b, NULL);
	int i;

	blobmsg_add_u64(b, NULL, sqlite3_column_int(stmt, 0));
	for (i = 1; i < 5; i++) {
		const char *val = sqlite3_column_text(stmt, i);

		if (val)
			blobmsg_add_string(b, NULL, val);
		else
			blobmsg_add_u8(b, NULL, 0);
	}
	blobmsg_close_array(b, c);

	return 0;
}

int
event_list(struct blob_buf *b, char *type, char *serial, char *client, int rows)
{
	char *sql_all = "SELECT timestamp, type, event, serial, client FROM event %s%s%sORDER by timestamp DESC LIMIT @rows;";
	char *sql = sql_all;
	sqlite3_stmt *stmt;
	char sql_buf[256];
	int rc;

	if (type || serial || client) {
		char *key = NULL, *val = NULL;

		if (type) {
			key = "type";
			val = type;
		} else if (serial) {
			key = "serial";
			val = serial;
		} else if (client) {
			key = "client";
			val = client;
		}
		sql = sql_buf;

		/* @key = @val will always bind as WHERE 'key' = 'value' breaking the where conditional when using the bind API */
		snprintf(sql_buf, sizeof(sql_buf),
			       "SELECT timestamp, type, event, serial, client FROM event WHERE %s = '%s' ORDER by timestamp DESC LIMIT @rows;",
			       key, val);
	}

	db_prepare(rc, stmt, sql);

	db_bind_int64(stmt, "@rows", rows);

	return db_select(stmt, b, event_list_cb);
}

int
event_remove_serial(char *serial)
{
	char *sql = "DELETE FROM event WHERE serial = @serial";
	sqlite3_stmt *stmt = NULL;
	int rc;

	db_prepare(rc, stmt, sql);

	db_bind_text(stmt, "@serial", serial);

	return db_delete(stmt);
}

int
event_purge(int timestamp)
{
	char *sql = "DELETE FROM event WHERE timestamp < @timestamp";
	sqlite3_stmt *stmt = NULL;
	int rc;

	db_prepare(rc, stmt, sql);

	db_bind_int64(stmt, "@timestamp", timestamp);

	return db_delete(stmt);
}

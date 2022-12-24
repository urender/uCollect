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
health_add(char *serial, struct blob_attr *b)
{
	char *sql = "INSERT INTO health (serial, health, timestamp) VALUES(@serial, @health, @timestamp)";
	sqlite3_stmt *stmt;
	int rc;

	db_prepare(rc, stmt, sql);

	db_bind_text(stmt, "@serial", serial);
	db_bind_blob(stmt, "@health", b);
	db_bind_int64(stmt, "@timestamp", time(NULL));

	return db_insert(stmt);
}

static int
health_list_cb(struct blob_buf *b, sqlite3_stmt *stmt)
{
	void *c = blobmsg_open_array(b, NULL);

	blobmsg_add_u64(b, NULL, sqlite3_column_int(stmt, 0));
	blobmsg_add_field(b, BLOBMSG_TYPE_TABLE, NULL,
			  sqlite3_column_blob(stmt, 1),
			  sqlite3_column_bytes(stmt, 1));
	blobmsg_close_array(b, c);

	return 0;
}

int
health_list(struct blob_buf *b, char *serial, int rows)
{
	char *sql = "SELECT timestamp, health FROM health WHERE serial = @serial ORDER by timestamp DESC LIMIT @rows;";
	sqlite3_stmt *stmt;
	int rc;

	db_prepare(rc, stmt, sql);

	db_bind_text(stmt, "@serial", serial);
	db_bind_int64(stmt, "@rows", rows);

	return db_select(stmt, b, health_list_cb);
}

int
health_remove_serial(char *serial)
{
	char *sql = "DELETE FROM health WHERE serial = @serial";
	sqlite3_stmt *stmt = NULL;
	int rc;

	db_prepare(rc, stmt, sql);

	db_bind_text(stmt, "@serial", serial);

	return db_delete(stmt);
}

int health_purge(int timestamp)
{
	char *sql = "DELETE FROM health WHERE timestamp < @timestamp";
	sqlite3_stmt *stmt = NULL;
	int rc;

	db_prepare(rc, stmt, sql);

	db_bind_int64(stmt, "@timestamp", timestamp);

	return db_delete(stmt);
}

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
device_add(char *serial, char *compat)
{
	static char *sql = "INSERT INTO device (serial, compatible, created, modified) VALUES(@serial, @compat, @created, @modified)";
	sqlite3_stmt *stmt = NULL;
	int rc;

	db_prepare(rc, stmt, sql);

	db_bind_text(stmt, "@serial", serial);
	db_bind_text(stmt, "@compat", compat);
	db_bind_int64(stmt, "@created", time(NULL));
	db_bind_int64(stmt, "@modified", time(NULL));

	return db_insert(stmt);
}

int
device_remove(char *serial)
{
	char *sql = "DELETE FROM device WHERE serial = @serial";
	sqlite3_stmt *stmt = NULL;
	int rc;

	state_remove_serial(serial);
	health_remove_serial(serial);
	event_remove_serial(serial);

	db_prepare(rc, stmt, sql);

	db_bind_text(stmt, "@serial", serial);

	return db_delete(stmt);
}

static int
device_list_cb(struct blob_buf *b, sqlite3_stmt *stmt)
{
	void *c = blobmsg_open_table(b, NULL);

	blobmsg_add_string(b, "serial", sqlite3_column_text(stmt, 0));
	blobmsg_add_string(b, "compatible", sqlite3_column_text(stmt, 1));
	blobmsg_add_u64(b, "created", sqlite3_column_int(stmt, 2));
	blobmsg_add_u64(b, "modified", sqlite3_column_int(stmt, 3));
	blobmsg_close_array(b, c);

	return 0;
}

int
device_list(struct blob_buf *b)
{
	char *sql = "SELECT serial, compatible, created, modified FROM device ORDER BY serial;";
	sqlite3_stmt *stmt;
	int rc;

	db_prepare(rc, stmt, sql);

	return db_select(stmt, b, device_list_cb);
}

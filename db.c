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

#include "db.h"

sqlite3 *db;

#define TABLE_DEVICE							\
	"CREATE TABLE IF NOT EXISTS device ("				\
	"serial		VARCHAR(30) UNIQUE PRIMARY KEY NOT NULL,"	\
	"compatible    	VARCHAR(32) NOT NULL,"				\
	"created	BIGINT NOT NULL,"				\
	"modified	BIGINT NOT NULL"				\
	")"

#define INDEX_DEVICE	"CREATE INDEX IF NOT EXISTS device_index ON device(serial)"

#define TABLE_STATE							\
	"CREATE TABLE IF NOT EXISTS state ("				\
	"serial		VARCHAR(30) NOT NULL,"				\
	"state		BLOB NOT NULL,"					\
	"timestamp	BIGINT NOT NULL,"				\
	"FOREIGN KEY(serial) REFERENCES device(serial)"			\
	")"

#define INDEX_STATE	"CREATE INDEX IF NOT EXISTS state_index ON state(serial)"

#define TABLE_HEALTH							\
	"CREATE TABLE IF NOT EXISTS health ("				\
	"serial		VARCHAR(30) NOT NULL,"				\
	"health		BLOB NOT NULL,"					\
	"timestamp	BIGINT NOT NULL,"				\
	"FOREIGN KEY(serial) REFERENCES device(serial)"			\
	")"

#define INDEX_HEALTH	"CREATE INDEX IF NOT EXISTS health_index ON health(serial)"

#define TABLE_EVENT							\
	"CREATE TABLE IF NOT EXISTS event ("				\
	"type		VARCHAR(30) NOT NULL,"				\
	"serial		VARCHAR(30),"					\
	"client		VARCHAR(64),"					\
	"event		TEXT,"						\
	"timestamp	BIGINT NOT NULL,"				\
	"FOREIGN KEY(serial) REFERENCES device(serial)"			\
	")"

#define INDEX_EVENT_TYPE	"CREATE INDEX IF NOT EXISTS event_type_index ON event(type)"
#define INDEX_EVENT_SERIAL	"CREATE INDEX IF NOT EXISTS event_serial_index ON event(serial)"

#define FOREIGN_KEYS	"PRAGMA foreign_keys = ON"

static char *db_commands[] = {
	TABLE_DEVICE,
	INDEX_DEVICE,

	TABLE_STATE,
	INDEX_STATE,

	TABLE_HEALTH,
	INDEX_HEALTH,

	TABLE_EVENT,
	INDEX_EVENT_TYPE,
	INDEX_EVENT_SERIAL,

	FOREIGN_KEYS,
	NULL
};

static int
db_create_db(void)
{
	char **cmd = db_commands;

	while (*cmd) {
		int ret = db_exec(*cmd);
		cmd++;
		if (ret)
			return ret;
	}

	return 0;
}

void
db_stop(void)
{
	sqlite3_close(db);
}

int
db_start(void)
{
	int rc = sqlite3_open("urender.db", &db);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return rc;
	}

	rc = db_create_db();
	if (rc)
		db_stop();

	return rc;
}

void
db_purge(int timestamp)
{
	health_purge(timestamp);
	state_purge(timestamp);
	event_purge(timestamp);
}

int
db_select(sqlite3_stmt *stmt, struct blob_buf *b, int (*cb)(struct blob_buf *b, sqlite3_stmt *stmt))
{
	void *c;

	blob_buf_init(b, 0);

	c = blobmsg_open_array(b, "rows");
	while (sqlite3_step(stmt) == SQLITE_ROW)
		cb(b, stmt);
	blobmsg_close_array(b, c);

	sqlite3_finalize(stmt);

	return 0;
}

int
__db_bind_text(sqlite3_stmt *stmt, char *id, char *value, const char *func, const int line)
{
	int idx = sqlite3_bind_parameter_index(stmt, id);
	int rc;

	if (value)
		rc = sqlite3_bind_text(stmt, idx, value, strlen(value), NULL);
	else
		rc = sqlite3_bind_null(stmt, idx);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error (%s:%d): (%d) - %s\n", func, line, rc, sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}

	return 0;
}

int
__db_bind_int64(sqlite3_stmt *stmt, char *id, uint64_t value, const char *func, const int line)
{
	int idx = sqlite3_bind_parameter_index(stmt, id);
	int rc = sqlite3_bind_int64(stmt, idx, value);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error (%s:%d): (%d) - %s\n", func, line, rc, sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}

	return 0;
}

int
__db_bind_blob(sqlite3_stmt *stmt, char *id, struct blob_attr *attr, const char *func, const int line)
{
	int idx = sqlite3_bind_parameter_index(stmt, id);
	int rc = sqlite3_bind_blob(stmt, idx, blobmsg_data(attr), blob_pad_len(attr), SQLITE_STATIC);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error (%s:%d): (%d) - %s\n", func, line, rc, sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}

	return 0;
}

int
__db_simple(sqlite3_stmt *stmt, const char *func, const int line)
{
	int rc = sqlite3_step(stmt);

	if (rc != SQLITE_DONE)
		fprintf(stderr, "SQL error (%s:%d): (%d) - %s\n", func, line, rc, sqlite3_errmsg(db));
	sqlite3_finalize(stmt);

	return rc != SQLITE_DONE;
}

int
__db_exec(char *sql, const char *func, const int line)
{
	char *err_msg = 0;
	int rc;

	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error (%s:%d): %s\n", func, line, err_msg);
		sqlite3_free(err_msg);
	}

	return rc;
}


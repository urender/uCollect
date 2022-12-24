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

#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <libubox/blobmsg.h>

extern void ubus_startup(void);

extern sqlite3 *db;
extern int db_start(void);
extern void db_stop(void);
extern void db_purge(int timestamp);
extern int db_select(sqlite3_stmt *stmt, struct blob_buf *b, int (*cb)(struct blob_buf *b, sqlite3_stmt *stmt));

/* making this a causes a SQLITE_MISUSE ?! */
#define db_prepare(rc, stmt, sql)										\
	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);								\
	if (rc != SQLITE_OK) {											\
		fprintf(stderr, "SQL error (%s:%d): (%d) - %s\n", __func__, __LINE__, rc, sqlite3_errmsg(db));	\
		return -1;											\
	}													\

extern int __db_bind_text(sqlite3_stmt *stmt, char *id, char *value, const char *func, const int line);
#define db_bind_text(x, y, z)					\
	if (__db_bind_text(x, y, z, __func__, __LINE__))	\
		return -1;

extern int __db_bind_int64(sqlite3_stmt *stmt, char *id, uint64_t value, const char *func, const int line);
#define db_bind_int64(x, y, z)					\
	if (__db_bind_int64(x, y, z, __func__, __LINE__))	\
		return -1;

extern int __db_bind_blob(sqlite3_stmt *stmt, char *id, struct blob_attr *attr, const char *func, const int line);
#define db_bind_blob(x, y, z)					\
	if (__db_bind_blob(x, y, z, __func__, __LINE__))	\
		return -1;

extern int __db_simple(sqlite3_stmt *stmt, const char *func, const int line);
#define db_insert(x) __db_simple(x, __func__, __LINE__)
#define db_delete(x) __db_simple(x, __func__, __LINE__)

extern int __db_exec(char *sql, const char *func, const int line);
#define db_exec(x) __db_exec(x, __func__, __LINE__)

typedef int (*sqlite3_callback)(void *b, int, char**, char**);

extern int device_add(char *serial, char *compat);
extern int device_remove(char *serial);
extern int device_list(struct blob_buf *b);

extern int state_add(char *serial, struct blob_attr *b);
extern int state_list(struct blob_buf *b, char *serial, int rows);
extern int state_remove_serial(char *serial);
extern int state_purge(int timestamp);

extern int health_add(char *serial, struct blob_attr *b);
extern int health_list(struct blob_buf *b, char *serial, int rows);
extern int health_remove_serial(char *serial);
extern int health_purge(int timestamp);

extern int event_add(char *type, char *serial, char *client, char *event);
extern int event_list(struct blob_buf *b, char *type, char *serial, char *client, int rows);
extern int event_remove_serial(char *serial);
extern int event_purge(int timestamp);


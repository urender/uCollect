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

#include <libubox/uloop.h>

#include "db.h"

int main(int argc, char **argv)
{
	uloop_init();

	ubus_startup();

	db_start();

/*	device_add("1122334455", "abc");
	device_add("1122334456", "abc");
	device_add("1122334457", "abc");

	event_add("admin", NULL, "192.168.178.1", "New login on admin panel");
	event_add("device", "1122334455", NULL, "Connected");
	event_add("system", NULL, NULL, "Startup");
*/
	uloop_run();

	uloop_done();
	db_stop();

	return 0;
}

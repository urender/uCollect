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

#include <uci.h>
#include <uci_blob.h>

#include "db.h"

static void
config_load_global(struct uci_section *s)
{
	enum {
		GLOBAL_ATTR_PATH,
		__GLOBAL_ATTR_MAX,
	};

	static const struct blobmsg_policy global_attrs[__GLOBAL_ATTR_MAX] = {
		[GLOBAL_ATTR_PATH] = { .name = "path", .type = BLOBMSG_TYPE_STRING },
	};

	const struct uci_blob_param_list global_attr_list = {
		.n_params = __GLOBAL_ATTR_MAX,
		.params = global_attrs,
	};

	struct blob_attr *tb[__GLOBAL_ATTR_MAX] = { 0 };

	blob_buf_init(&b, 0);
	uci_to_blob(&b, s, &global_attr_list);
	blobmsg_parse(global_attrs, __GLOBAL_ATTR_MAX, tb, blob_data(b.head), blob_len(b.head));

	if (tb[GLOBAL_ATTR_PATH])
		config.db_path = blobmsg_get_string(tb[GLOBAL_ATTR_PATH]);

}

void
config_load(void)
{
	struct uci_context *uci = uci_alloc_context();
        struct uci_package *package = NULL;

	if (!uci_load(uci, "uStore", &package)) {
		struct uci_element *e;

		uci_foreach_element(&package->sections, e) {
			struct uci_section *s = uci_to_section(e);

			if (!strcmp(s->type, "global"))
				config_load_global(s);
		}
	} else {
		ulog(LOG_ERR, "failed to load UCI\n");
		exit(EXIT_FAILURE);
	}

	uci_unload(uci, package);
	uci_free_context(uci);
}

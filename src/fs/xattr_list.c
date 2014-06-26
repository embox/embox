/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    20.11.2013
 */

#include <errno.h>
#include <string.h>
#include <mem/misc/pool.h>

#include <fs/xattr_list.h>

#define XATTR_LIST_QUANTITY 32
#define XATTR_ENT_QUANTITY 64

POOL_DEF(xattr_list_pool, struct xattr_list, XATTR_LIST_QUANTITY);
POOL_DEF(xattr_ent_pool, struct xattr_ent, XATTR_ENT_QUANTITY);

void xattr_list_init(struct xattr_list *xlst) {

	dlist_init(&xlst->xl_head);
}

struct xattr_list *xattr_list_create(void) {
	struct xattr_list *xlst;

	xlst = pool_alloc(&xattr_list_pool);

	xattr_list_init(xlst);

	return xlst;
}

void xattr_list_delete(struct xattr_list *xlst) {

	pool_free(&xattr_list_pool, xlst);
}

static void xattr_ent_value_update(struct xattr_ent *xent, const char *value,
		size_t vlen) {

	assert(vlen <= XATTR_MAX_VSIZE);

	memcpy(xent->xe_val, value, vlen);
	xent->xe_len = vlen;
}

static struct xattr_ent *xattr_ent_create(struct dlist_head *add_next,
		const char *name, const char *value, size_t vlen) {
	struct xattr_ent *xent;

	assert(strlen(name) < XATTR_MAX_NSIZE && vlen <= XATTR_MAX_VSIZE);

	xent = pool_alloc(&xattr_ent_pool);

	dlist_head_init(&xent->xe_lnk);

	strcpy(xent->xe_name, name);
	xattr_ent_value_update(xent, value, vlen);

	dlist_add_next(&xent->xe_lnk, add_next);

	return xent;
}

static void xattr_ent_delete(struct xattr_ent *xent) {

	dlist_del(&xent->xe_lnk);
	pool_free(&xattr_ent_pool, xent);
}

static struct xattr_ent *xattr_find(struct xattr_list *xlnk, const char *name,
		struct dlist_head **add_next_p) {
	struct xattr_ent *xent, *xent_out;
	struct dlist_head *add_next;

	add_next = &xlnk->xl_head;
	xent_out = NULL;

	dlist_foreach_entry(xent, &xlnk->xl_head, xe_lnk) {
		int d;

		d = strcmp(xent->xe_name, name);

		if (d == 0) {
			xent_out = xent;
			break;
		}

		if (d > 0) {
			break;
		}

		add_next = &xent->xe_lnk;
	}

	if (add_next_p) {
		*add_next_p = add_next;
	}

	return xent_out;
}

int getxattr_generic(struct xattr_list *xlst, const char *name,
		char *value, size_t len) {
	struct xattr_ent *xent;

	if (strlen(name) >= XATTR_MAX_NSIZE || len > XATTR_MAX_VSIZE) {
		return -ERANGE;
	}

	xent = xattr_find(xlst, name, NULL);
	if (!xent) {
		return -ENOENT;
	}

	if (len < xent->xe_len) {
		return -ERANGE;
	}

	memcpy(value, xent->xe_val, xent->xe_len);

	return xent->xe_len;
}

int setxattr_generic(struct xattr_list *xlst, const char *name,
			const char *value, size_t len, int flags) {
	struct xattr_ent *xent;
	struct dlist_head *add_next;

	if (strlen(name) > XATTR_MAX_NSIZE || len > XATTR_MAX_VSIZE) {
		return -ERANGE;
	}

	xent = xattr_find(xlst, name, &add_next);

	if (xent) {

		if (flags == XATTR_CREATE) {
			return -EEXIST;
		}

		xattr_ent_value_update(xent, value, len);
		return 0;
	}

	if (flags == XATTR_REPLACE) {
		return -ENOENT;
	}

	xent = xattr_ent_create(add_next, name, value, len);
	if (!xent) {
		return -ENOMEM;
	}

	return 0;
}

int listxattr_generic(struct xattr_list *xlst, char *list, size_t len) {
	struct xattr_ent *xent;
	char *plist;
	size_t last;

	plist = list;
	last = len;

	dlist_foreach_entry(xent, &xlst->xl_head, xe_lnk) {
		int attr_len;

		attr_len = strlen(xent->xe_name);
		if (last <= attr_len) {
			return -ERANGE;
		}

		memcpy(plist, xent->xe_name, attr_len + 1);
		plist += attr_len + 1;

		last -= xent->xe_len + 1;
	}

	return plist - list;
}

int removexattr_generic(struct xattr_list *xlst, const char *name) {
	struct xattr_ent *xent;

	xent = xattr_find(xlst, name, NULL);
	if (!xent) {
		return -ENOENT;
	}

	xattr_ent_delete(xent);

	return 0;
}

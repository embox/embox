/**
 * @file
 * @brief SNMP MIB operations
 *
 * @date 9.11.2012
 * @author Alexander Kalmuk
 */

#include <net/snmp.h>
#include <mem/objalloc.h>
#include <errno.h>
#include <string.h>
#include <framework/mod/options.h>

OBJALLOC_DEF(mib_obj_pool, struct mib_obj, OPTION_GET(NUMBER,max_obj_count));

struct mib_obj mib_root;

static mib_obj_t getchild_by_id(mib_obj_t obj, int id);

int mib_obj_link(mib_obj_t obj, mib_obj_t parent) {
	assert(obj != NULL);
	assert(parent != NULL);

	dlist_head_init(&obj->parent_link);
	dlist_add_prev(&obj->parent_link, &parent->children);

	return ENOERR;
}

mib_obj_t mib_obj_alloc(void) {
	mib_obj_t obj = objalloc(&mib_obj_pool);

	if (!obj) {
		return NULL;
	}

	dlist_init(&obj->children);

	return ENOERR;
}

#if 0
void mib_obj_unlink(mib_obj_t obj) {
	assert(obj != NULL);
	dlist_del(&obj->parent_link);
}
#endif

mib_obj_t mib_obj_getbyoid(const char *oid) {
	mib_obj_t cur = &mib_root;
	int *str = (int *)oid;
	size_t len = strlen(oid);

	if (len % sizeof(int) != 0) {
		return NULL;
	}

	while (len > 0) {
		int id = *str;

		cur = getchild_by_id(cur, id);
		if (!cur) {
			break;
		}
		str++;
		len -= sizeof(int);
	}

	return NULL;
}

static mib_obj_t getchild_by_id(mib_obj_t obj, int id) {
	mib_obj_t cur, nxt;

	dlist_foreach_entry(cur, nxt, &obj->children, parent_link) {
		if (cur->id == id) {
			break;
		}
	}

	return (&cur->parent_link == &obj->children ? NULL : obj);
}

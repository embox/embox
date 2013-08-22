/*
 * @file
 *
 * @date Mar 5, 2013
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <util/dlist.h>
#include <sys/stat.h>
#include <mem/misc/pool.h>

#include <fs/file_operation.h>
#include <fs/vfs.h>

struct tty_repo_item {
	struct dlist_head list;
	void *dev;
	const struct kfile_operations *file_ops;
	const char *name;
};

static DLIST_DEFINE(tty_repo);

POOL_DEF(tty_pool, struct tty_repo_item, 0x4);

int tty_register(const char *name, void *dev, const struct kfile_operations *file_ops) {
	struct tty_repo_item *item;
	if(NULL == (item = pool_alloc(&tty_pool))) {
		return -ENOMEM;
	}
	item->dev = dev;
	item->file_ops = file_ops;
	item->name = name;

	dlist_add_next(dlist_head_init(&item->list), &tty_repo);

	return ENOERR;
}

int tty_unregister(const char *name) {
	return ENOERR;
}

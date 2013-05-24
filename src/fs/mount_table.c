/**
 * @file
 *
 * @date May 17, 2013
 * @author: Anton Bondarev
 */

#include <string.h>
#include <errno.h>


#include <util/dlist.h>
#include <mem/misc/pool.h>

#include <fs/node.h>
#include <fs/vfs.h>


struct mount_descriptor {
	struct node *dir_node;
	struct dlist_head mount_link;
};

static DLIST_DEFINE(mount_filesystem_list);

POOL_DEF(mount_desc_pool, struct mount_descriptor, 8);

int mount_table_check(struct node *dir_node) {
	struct mount_descriptor *desc, *tmp;
	char full_path[PATH_MAX];
	char dir_path[PATH_MAX];

	if(dir_node == NULL) {
		return -EINVAL;
	}

	if(0 != vfs_get_path_by_node(dir_node, full_path)) {
		return -EINVAL; /* wrong directory name*/
	}

	dlist_foreach_entry(desc, tmp, &mount_filesystem_list, mount_link) {
		if(0 != vfs_get_path_by_node(desc->dir_node, dir_path)) {
			continue;
		}

		if(0 == strcmp(dir_path, full_path)) {
			return -EBUSY;
		}
	}
	return ENOERR;
}

int mount_table_add(struct node *dir_node) {
	int res;
	struct mount_descriptor *mdesc;

	if(dir_node == NULL) {
		return -EINVAL;
	}

	if(ENOERR != (res = mount_table_check(dir_node))) {
		return res;
	}

	if(NULL == (mdesc = pool_alloc(&mount_desc_pool))) {
		return -ENOMEM;
	}

	dlist_add_next(dlist_head_init(&mdesc->mount_link), &mount_filesystem_list);

	return ENOERR;
}

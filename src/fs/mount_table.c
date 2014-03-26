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
#include <fs/mount.h>


static DLIST_DEFINE(mount_filesystem_list);

POOL_DEF(mount_desc_pool, struct mount_descriptor, 8);

int mount_table_check(struct node *dir_node) {
	struct mount_descriptor *desc, *tmp;
	char full_path[PATH_MAX];

	if(dir_node == NULL) {
		return -EINVAL;
	}

	if(0 != vfs_get_path_by_node(dir_node, full_path)) {
		return -EINVAL; /* wrong directory name*/
	}

	dlist_foreach_entry(desc, tmp, &mount_filesystem_list, mnt_link) {
		if (dir_node == desc->dir_node) {
			return -EBUSY;
		}
	}
	return ENOERR;
}

struct mount_descriptor *mount_table_find(struct node *dir_node) {
	struct mount_descriptor *desc, *tmp;
//	char full_path[PATH_MAX];

	if(dir_node == NULL) {
		return NULL;
	}

//	if(0 != vfs_get_path_by_node(dir_node, full_path)) {
//		return NULL; /* wrong directory name*/
//	}

	dlist_foreach_entry(desc, tmp, &mount_filesystem_list, mnt_link) {
		if (dir_node == desc->dir_node) {
			return desc;
		}
	}
	return NULL;
}

struct mount_descriptor *mount_table_find_by_root(struct node *root_node) {
	struct mount_descriptor *desc, *tmp;
//	char full_path[PATH_MAX];

	if(root_node == NULL) {
		return NULL;
	}

//	if(0 != vfs_get_path_by_node(root_node, full_path)) {
//		return NULL; /* wrong directory name*/
//	}

	dlist_foreach_entry(desc, tmp, &mount_filesystem_list, mnt_link) {
		if (root_node == desc->mnt_root) {
			return desc;
		}
	}
	return NULL;
}

struct mount_descriptor *mount_table_get_desc(struct node *dir_node) {
	struct node *parent = dir_node;

	while (0 != strcmp(parent->name, "/")) {
		parent = vfs_get_parent(parent);
	}

	return mount_table_find(parent);
}

int mount_table_add(struct node *dir_node, struct node *root) {
	struct mount_descriptor *mdesc, *parent_mdesc;

	if(dir_node == NULL) {
		return -EINVAL;
	}

	if(NULL == (mdesc = pool_alloc(&mount_desc_pool))) {
		return -ENOMEM;
	}

	mdesc->dir_node = dir_node;
	mdesc->dir_node->mounted++;
	mdesc->mnt_root = root;
	parent_mdesc = mount_table_get_desc(dir_node);
	mdesc->mnt_parent = parent_mdesc == NULL ? mdesc : parent_mdesc;


	dlist_add_next(dlist_head_init(&mdesc->mnt_link), &mount_filesystem_list);

	return ENOERR;
}

int mount_table_del(struct node *dir_node) {
	struct mount_descriptor *mdesc;

	if(dir_node == NULL) {
		return -EINVAL;
	}

	if(dlist_empty(&mount_filesystem_list)) {
		return -EINVAL;
	}

	if(NULL == (mdesc = mount_table_find(dir_node))) {
		return -EINVAL;
	}

	mdesc->dir_node->mounted--;
	vfs_del_leaf(mdesc->mnt_root);

	dlist_del(&mdesc->mnt_link);

	pool_free(&mount_desc_pool, mdesc);

	return ENOERR;
}

struct dlist_head *mount_table(void) {
	return &mount_filesystem_list;
}

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

#include <mem/misc/pool.h>

POOL_DEF(mount_desc_pool, struct mount_descriptor, 16);

static struct mount_descriptor *mnt_root = NULL;

struct mount_descriptor *mount_table_get_child(struct mount_descriptor *parent, struct node *mnt_point) {
	struct mount_descriptor *desc_next, *desc;

	if (parent->mnt_point == mnt_point ) {
		assert(parent->mnt_parent == parent);
		return parent;
	}

	dlist_foreach_entry(desc, desc_next, &parent->mnt_mounts, mnt_child) {
		if (desc->mnt_point == mnt_point) {
			return desc;
		}
	}

	return NULL;
}

int mount_table_add(struct mount_descriptor *parent, struct node *mnt_point, struct path *root) {
	struct mount_descriptor *mdesc;

	assert(parent != NULL || (parent == NULL && mnt_root == NULL));

	if(mnt_point == NULL) {
		return -EINVAL;
	}

	if(NULL == (mdesc = pool_alloc(&mount_desc_pool))) {
		return -ENOMEM;
	}

	mdesc->mnt_point = mnt_point;
	mdesc->mnt_point->mounted++;
	mdesc->mnt_root = root->node;

	dlist_init(&mdesc->mnt_mounts);
	dlist_head_init(&mdesc->mnt_child);

	if (parent == NULL) {
		mdesc->mnt_parent = mdesc;
		mnt_root = mdesc;
	} else {
		mdesc->mnt_parent = parent;
		dlist_add_next(&mdesc->mnt_child, &parent->mnt_mounts);
	}

	root->mnt_desc = mdesc;

	return ENOERR;
}

int mount_table_del(struct node *dir_node, struct mount_descriptor *mdesc) {

	if(dir_node == NULL || mdesc == NULL) {
		return -EINVAL;
	}

	mdesc->mnt_point->mounted--;
	vfs_del_leaf(mdesc->mnt_root);

	dlist_del(&mdesc->mnt_mounts);
	dlist_del(&mdesc->mnt_child);

	pool_free(&mount_desc_pool, mdesc);

	return ENOERR;
}

struct mount_descriptor *mount_table(void) {
	return mnt_root;
}

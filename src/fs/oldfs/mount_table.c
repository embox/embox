/**
 * @file
 *
 * @date May 17, 2013
 * @author: Anton Bondarev
 * @author: Vita Loginova
 */

#include <string.h>
#include <errno.h>


#include <lib/libds/dlist.h>
#include <mem/misc/pool.h>

#include <fs/inode.h>
#include <fs/vfs.h>
#include <fs/mount.h>

#include <mem/misc/pool.h>

POOL_DEF(mount_desc_pool, struct mount_descriptor, OPTION_GET(NUMBER,mount_desc_quantity));

static struct mount_descriptor *mnt_root = NULL;

struct mount_descriptor *mount_table_get_child(struct mount_descriptor *parent, struct inode *mnt_point) {
	struct mount_descriptor *desc;

	if (parent->mnt_point == mnt_point ) {
		assert(parent->mnt_parent == parent);
		return parent;
	}

	dlist_foreach_entry(desc, &parent->mnt_mounts, mnt_child) {
		if (desc->mnt_point == mnt_point) {
			return desc;
		}
	}

	return NULL;
}

struct mount_descriptor *mount_table_add(struct path *mnt_point_path,
		struct mount_descriptor *mnt_desc,
		struct inode *root, const char *dev) {
	struct mount_descriptor *mdesc;

	assert(mnt_desc != NULL ||
			(mnt_desc == NULL && mnt_root == NULL));

	if (mnt_point_path->node == NULL) {
		return NULL;
	}

	if (mnt_root != NULL && mnt_point_path->node ==
			mnt_desc->mnt_root) {
		return NULL;
	}

	if (NULL == (mdesc = pool_alloc(&mount_desc_pool))) {
		return NULL;
	}

	mdesc->mnt_point = mnt_point_path->node;
	mdesc->mnt_point->mounted++;
	mdesc->mnt_root = root;

	/* XXX mount root should preserve mode, uid/gid */
	root->i_mode = mnt_point_path->node->i_mode;
	root->i_owner_id = mnt_point_path->node->i_owner_id;
	root->i_group_id = mnt_point_path->node->i_group_id;

	dlist_init(&mdesc->mnt_mounts);
	dlist_head_init(&mdesc->mnt_child);

	if (mnt_desc == NULL) {
		mdesc->mnt_parent = mdesc;
		mnt_root = mdesc;
	} else {
		mdesc->mnt_parent = mnt_desc;
		dlist_add_next(&mdesc->mnt_child, &mnt_desc->mnt_mounts);
	}

	if (dev != NULL) {
		strncpy(mdesc->mnt_dev, dev, MOUNT_DESC_STRINFO_LEN);
	} else {
		mdesc->mnt_dev[0] = '\0';
	}

	mdesc->mnt_dev[MOUNT_DESC_STRINFO_LEN - 1] = '\0';

	return mdesc;
}

int mount_table_del(struct mount_descriptor *mdesc) {

	if(mdesc == NULL) {
		return -EINVAL;
	}

	if (mnt_root == mdesc) {
		mnt_root = NULL;
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

/**
 * @file dentry.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 10.01.2020
 */

#include <fs/dentry.h>
#include <fs/inode.h>
#include <fs/perm.h>
#include <fs/vfs.h>
#include <drivers/block_dev.h>

int dvfs_lookup(const char *path, struct lookup *lookup) {
	struct path node_path;
	int res = fs_perm_lookup(path, NULL, &node_path);

	if (res != 0) {
		return res;
	}

	lookup->item = node_path.node->i_dentry;

	return 0;
}

static struct mount_descriptor *mount_desc_walk(struct mount_descriptor *desc, struct inode *node) {
	struct mount_descriptor *i, *res;
	if (desc == NULL) {
		return NULL;
	}

	if (desc->mnt_point == node) {
		return desc;
	}

	if (desc->mnt_root == node) {
		return desc;
	}

	dlist_foreach_entry(i, &desc->mnt_mounts, mnt_child) {
		res = mount_desc_walk(i, node);
		if (res != NULL) {
			return res;
		}
	}

	return NULL;
}

struct mount_descriptor *mount_desc_by_inode(struct inode *node) {
	struct mount_descriptor *mount_list = mount_table();

	if (mount_list == NULL) {
		return NULL;
	}

	return mount_desc_walk(mount_list, node);
}

int dentry_full_path(struct dentry *dentry, char *buf) {
	struct path path = {
		.node     = dentry->d_inode,
		.mnt_desc = mount_desc_by_inode(dentry->d_inode),
	};

	vfs_get_path_by_node(&path, buf);

	return 0;
}

struct dentry *dvfs_root(void) {
	struct inode *root = vfs_get_root();
	return root->i_dentry;
}

int dentry_ref_inc(struct dentry *dentry) {
	assert(dentry);
	return ++dentry->usage_count;
}

int dentry_ref_dec(struct dentry *dentry) {
	assert(dentry);
	if (dentry->usage_count <= 0) {
		return 0;
	}
	return --dentry->usage_count;
}

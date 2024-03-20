/**
 * @brief
 *
 * @date 02.01.24
 * @author Aleksey Zhmulin
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include <lib/libds/dlist.h>
#include <vfs/core.h>

void vfs_mount_point_get_first_bind(const struct inode *mount_point,
    struct inode *lookup) {
	memcpy(lookup, mount_point, sizeof(struct inode));

	while (vfs_mount_point_get_prev_bind(lookup, lookup)) {}
}

void vfs_mount_point_get_first(const struct inode *mount_point,
    struct inode *lookup) {
	memcpy(lookup, mount_point, sizeof(struct inode));

	while (vfs_mount_point_get_prev(lookup, lookup)) {}
}

void vfs_mount_point_get_last(const struct inode *mount_point,
    struct inode *lookup) {
	memcpy(lookup, mount_point, sizeof(struct inode));

	while (vfs_mount_point_get_next(lookup, lookup)) {}
}

bool vfs_mount_point_get_next(const struct inode *mount_point,
    struct inode *lookup) {
	struct super_block *sb;

	assert(!vfs_inode_is_bad(mount_point));

	dlist_foreach_entry(sb, &mount_point->sb->mount_list, list_item) {
		if (mount_point->ino == sb->mount_point.ino) {
			lookup->ino = VFS_MPT_INO;
			lookup->sb = sb;
			return true;
		}
	}

	return false;
}

bool vfs_mount_point_get_prev(const struct inode *mount_point,
    struct inode *lookup) {
	assert(!vfs_inode_is_bad(mount_point));

	if (!vfs_inode_is_mount_point(mount_point)
	    || vfs_inode_is_root(mount_point)) {
		return false;
	}

	memcpy(lookup, &mount_point->sb->mount_point, sizeof(struct inode));

	return true;
}

bool vfs_mount_point_get_prev_bind(const struct inode *mount_point,
    struct inode *lookup) {
	assert(!vfs_inode_is_bad(mount_point));

	if (!mount_point->sb->bind_mount) {
		return false;
	}

	return vfs_mount_point_get_prev(mount_point, lookup);
}

void vfs_mount_point_add_next(const struct inode *mount_point,
    struct super_block *sb) {
	struct inode mpt;

	vfs_mount_point_get_last(mount_point, &mpt);

	dlist_add_next(&sb->list_item, &mpt.sb->mount_list);
	memcpy(&sb->mount_point, &mpt, sizeof(struct inode));
}

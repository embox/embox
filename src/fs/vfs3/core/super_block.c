/**
 * @brief
 *
 * @date 02.01.24
 * @author Aleksey Zhmulin
 */

#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <kernel/spinlock.h>
#include <lib/libds/dlist.h>
#include <util/err.h>
#include <vfs/core.h>

void vfs_super_block_init(struct super_block *sb,
    const struct vfs_driver *drv) {
	struct inode mpt;

	sb->drv = drv;
	sb->usage_count = 0;
	sb->bind_mount = false;
	sb->mount_point.ino = VFS_BAD_INO;
	sb->mount_point.sb = NULL;

	dlist_init(&sb->list_item);
	dlist_init(&sb->mount_list);
	spin_init(&sb->lock, __SPIN_UNLOCKED);

	mpt.ino = VFS_MPT_INO;
	mpt.sb = sb;

	vfs_inode_init(&mpt, S_IRWXA | S_IFDIR);
}

void vfs_super_block_deinit(struct super_block *sb) {
	dlist_del(&sb->list_item);
}

struct super_block *vfs_super_block_mount(const char *name,
    struct block_dev *bdev, bool bind_mount) {
	const struct vfs_driver *drv;
	struct super_block *sb;

	drv = vfs_driver_find(name);
	if (!drv) {
		return err2ptr(ENOENT);
	}

	sb = drv->ops.mount(bdev);
	if (ptr2err(sb)) {
		return sb;
	}

	vfs_super_block_init(sb, drv);
	sb->bind_mount = bind_mount;

	return sb;
}

int vfs_super_block_unmount(struct super_block *sb) {
	if (!dlist_empty(&sb->mount_list) || (sb->usage_count != 0)) {
		return -EBUSY;
	}

	return sb->drv->ops.umount(sb);
}

/**
 * @brief cdevfs driver
 *
 * @date 02.01.24
 * @author Aleksey Zhmulin
 */

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <drivers/char_dev.h>
#include <framework/mod/options.h>
#include <lib/libds/dlist.h>
#include <util/err.h>
#include <util/log.h>
#include <vfs/core.h>

struct idesc;
struct block_dev;

static bool mounted = false;
static struct super_block cdevfs_sb;

static struct idesc *cdevfs_open(const struct inode *inode, int oflag) {
	struct char_dev *cdev;

	cdev = (struct char_dev *)(uintptr_t)inode->ino;

	return char_dev_open(cdev, oflag);
}

static void cdevfs_load_info(const struct inode *inode,
    struct inode_info *info) {
	memset(info, 0, sizeof(struct inode_info));
	info->mode = S_IRWXA | S_IFCHR;
}

static void cdevfs_save_info(const struct inode *inode,
    const struct inode_info *info) {
}

static int cdevfs_readdir(const struct inode *dir_inode, struct inode *pos,
    struct dirent *dirent) {
	struct char_dev *cdev;

	if (vfs_inode_is_bad(pos)) {
		cdev = NULL;
	}
	else {
		cdev = (struct char_dev *)(uintptr_t)pos->ino;
	}

	cdev = char_dev_iterate(cdev);

	if (!cdev) {
		return -ENOENT;
	}

	pos->ino = (ino_t)(uintptr_t)cdev;

	dirent->d_ino = 0;
	strcpy(dirent->d_name, cdev->name);

	return 0;
}

static int cdevfs_lookup(const struct inode *dir_inode, const char *name,
    struct inode *lookup) {
	struct char_dev *cdev;

	if ((cdev = char_dev_find(name))) {
		lookup->ino = (ino_t)(uintptr_t)cdev;
		lookup->sb = &cdevfs_sb;
		return 0;
	}

	return -ENOENT;
}

static struct super_block *cdevfs_mount(struct block_dev *bdev) {
	if (mounted) {
		return err2ptr(EBUSY);
	}

	mounted = true;

	return &cdevfs_sb;
}

static int cdevfs_umount(struct super_block *sb) {
	assert(mounted);

	mounted = false;

	vfs_super_block_deinit(sb);

	return 0;
}

static const struct vfs_driver cdevfs_driver = {
    .name = "cdevfs",
    .ops = ((struct vfs_driver_ops){
        .mount = cdevfs_mount,
        .umount = cdevfs_umount,
        .open = cdevfs_open,
        .load_info = cdevfs_load_info,
        .save_info = cdevfs_save_info,
        .readdir = cdevfs_readdir,
        .lookup = cdevfs_lookup,
    }),
};

VFS_DRIVER_DEF(cdevfs_driver);

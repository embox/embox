/**
 * @file
 *
 * @date Oct 24, 2013
 * @author: Anton Bondarev
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <fs/hlpr_path.h>
#include <fs/perm.h>
#include <security/security.h>
#include <fs/vfs.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/path.h>
#include <fs/fs_driver.h>
#include <fs/file_desc.h>
#include <sys/time.h>
#include <utime.h>
#include <drivers/block_dev.h>

int ktruncate(struct inode *node, off_t length) {
	int ret;

	if (S_ISDIR(node->i_mode)) {
		SET_ERRNO(EISDIR);
		return -1;
	}

	if (0 > (ret = fs_perm_check(node, S_IWOTH))) {
		SET_ERRNO(-ret);
		return -1;
	}

	if ((!node->i_sb->sb_iops) || (!node->i_sb->sb_iops->ino_truncate)) {
		//SET_ERRNO(ENOTSUP);
		//SET_ERRNO(EPERM); it may mean that file it's not possible to modify
		return 0;
	}

	if (0 > (ret = node->i_sb->sb_iops->ino_truncate(node, length))) {
		SET_ERRNO(-ret);
		return -1;
	}

	return ret;
}

int kfile_change_stat(struct inode *node, const struct utimbuf *times) {
	struct timeval now;

	if (node == NULL) {
		return -ENOENT;
	}

	if (times == NULL) {
		if (gettimeofday(&now, NULL) == -1) {
			return -EINVAL;
		}
		inode_ctime_set(node, now.tv_sec);
		return 0;
	}

	inode_ctime_set(node, times->actime);
	inode_mtime_set(node, times->modtime);

	return 0;
}

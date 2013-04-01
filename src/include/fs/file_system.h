/**
 * @file
 * @brief
 *
 * @date 26.11.2012
 * @author Andrey Gazukin
 */

#ifndef FILE_SYSTEM_H_
#define FILE_SYSTEM_H_

#include <util/dlist.h>
#include <sys/stat.h>
#include <limits.h>

struct filesystem {
	struct dlist_head link;
	struct fs_driver *drv;    			/* file system driver */
	struct block_dev *bdev;   			/* block device, where is this file system */
	void             *fsi;    			/* extended information */
	void             *rootdir_prev_fs;	/* root directory previous file system info */
	void             *rootdir_prev_fi;	/* root directory previous file info */
	char mntfrom[PATH_MAX];
	char mntto[PATH_MAX];

	const struct kfile_operations *file_op;
};


extern struct filesystem *filesystem_alloc(const char *fs_driver);
extern void filesystem_free(struct filesystem *fs);
extern int filesystem_get_mount_list(char *buff);

#endif /* FILE_SYSTEM_H_ */

/**
 * @file
 * @brief
 *
 * @date 26.11.2012
 * @author Andrey Gazukin
 */

#ifndef FILE_SYSTEM_H_
#define FILE_SYSTEM_H_

struct filesystem {
	struct fs_driver *drv;    /* file system driver */
	struct block_dev *bdev;   /* block device, where is this file system */
	void             *fsi;    /* extended information */

	const struct kfile_operations *file_op;
};


extern struct filesystem *filesystem_alloc(const char *fs_driver);
extern void filesystem_free(struct filesystem *fs);

#endif /* FILE_SYSTEM_H_ */

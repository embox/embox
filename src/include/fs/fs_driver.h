/**
 * @file
 *
 * @date 05.10.10
 * @author Nikolay Korotky
 */

#ifndef FS_DRV_H_
#define FS_DRV_H_

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include <util/array.h>

struct inode;
struct block_dev;
struct super_block;

struct fsop_desc {
	int (*format)(struct block_dev *bdev, void *priv);
	int (*mount)(struct super_block *sb, struct inode *dest);
	int (*create_node)(struct inode *parent_node, struct inode *new_node);
	int (*delete_node)(struct inode *node);

	/* TODO: consider following to accept nas * as first arg (Anton Kozlov) */
	int (*getxattr)(struct inode *node, const char *name,
			char *value, size_t len);
	int (*setxattr)(struct inode *node, const char *name,
			const char *value, size_t len, int flags);
	int (*listxattr)(struct inode *node, char *list, size_t len);

	int (*truncate)(struct inode *node, off_t length);
	int (*umount)(struct inode *dir);
};

struct file_operations;

/**
 * Structure of file system driver.
 * We can mount some file system with name of FS which has been registered in
 * our system.
 */
struct fs_driver {
	const char                   *name;
	int (*fill_sb)(struct super_block *sb, const char *source);
	const struct file_operations *file_op;
	const struct fsop_desc       *fsop;
};

#define DECLARE_FILE_SYSTEM_DRIVER(fs_driver_)      \
	ARRAY_SPREAD_DECLARE(const struct fs_driver *const, \
			fs_drivers_registry);                \
	ARRAY_SPREAD_ADD(fs_drivers_registry, \
			&fs_driver_)

extern const struct fs_driver *fs_driver_find(const char *name);

#endif /* FS_DRV_H_ */

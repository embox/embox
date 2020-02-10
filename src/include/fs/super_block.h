/**
 * @file
 * @brief
 *
 * @date 26.11.2012
 * @author Andrey Gazukin
 */

#ifndef SUPER_BLOCK_H_
#define SUPER_BLOCK_H_

struct block_dev;
struct file_operations;
struct fs_driver;
struct inode_operations;
struct super_block_operations;

/* Stubs */
struct super_block_operations { };

struct super_block {
	const struct fs_driver *fs_drv;
	struct block_dev       *bdev;
	void                   *sb_data;

	struct super_block_operations *sb_ops;
	struct inode_operations       *sb_iops;
	const struct file_operations  *sb_fops;
};

extern struct super_block *super_block_alloc(const char *fs_driver, const char *source);
extern void super_block_free(struct super_block *sb);

#endif /* SUPER_BLOCK_H_ */

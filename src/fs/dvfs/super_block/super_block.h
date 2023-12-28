/**
 * @file
 * @brief
 *
 * @date 26.11.2012
 * @author Andrey Gazukin
 */

#ifndef FS_DVFS_SUPER_BLOCK_H_
#define FS_DVFS_SUPER_BLOCK_H_

struct block_dev;
struct file_operations;
struct fs_driver;
struct inode_operations;
struct dlist_head;
struct inode;
struct super_block;
struct idesc;
struct lookup;

struct super_block_operations {
	int (*destroy_inode)(struct inode *inode);
	int (*umount_begin)(struct super_block *sb);
	struct idesc *(*open_idesc)(struct lookup *l, int __oflag);
};

struct super_block {
	const struct fs_driver *fs_drv;
	struct block_dev *bdev;
	struct dlist_head *inode_list;

	struct super_block_operations *sb_ops;
	struct inode_operations *sb_iops;
	const struct file_operations *sb_fops;

	struct inode *sb_root;
	void *sb_data;
};

extern struct super_block *super_block_alloc(const char *fs_driver,
    const char *source);
extern int super_block_free(struct super_block *sb);

#endif /* FS_DVFS_SUPER_BLOCK_H_ */

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
struct dlist_head;
struct inode;
struct super_block;
struct idesc;
struct lookup;

struct super_block_operations {
	int           (*destroy_inode)(struct inode *inode);
	int           (*umount_begin)(struct super_block *sb);
	struct idesc *(*open_idesc)(struct lookup *l, int __oflag);
};

struct super_block {
	const struct fs_driver *fs_drv;
	struct block_dev       *bdev;
	struct dlist_head      *inode_list;

	struct super_block_operations *sb_ops;
	struct inode_operations       *sb_iops;
	const struct file_operations  *sb_fops;

	struct inode           *sb_root;
	void                   *sb_data;

	/* Set by kumount() once it has established that no
	 * descriptor is open on this volume, and checked when a descriptor is
	 * created. Counting the descriptors and tearing the volume down are two
	 * operations; on four cores another thread opens a file between them, and
	 * then closes it onto a superblock that no longer exists. The seal makes
	 * the count mean something for longer than the instant it was taken. */
	int                     sb_unmounting;
};

extern struct super_block *super_block_alloc(const char *fs_driver, const char *source);
extern int super_block_free(struct super_block *sb);

/* Seals the volume against new descriptors if none are open on it. Answers 0
 * when the seal is on, or the number of open descriptors when it is not. */
extern int file_desc_seal_sb(struct super_block *sb);

#endif /* SUPER_BLOCK_H_ */

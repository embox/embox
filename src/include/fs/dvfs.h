/* @author Denis Deryugin
 * @date 17 Mar 2015
 *
 * Dumb VFS
 */

#ifndef _DVFS_H_
#define _DVFS_H_

#include <fs/file_system.h>
#include <util/dlist.h>

/*****************
 New VFS prototype
 *****************/

#define DENTRY_NAME_LEN 16
#define FS_NAME_LEN     16

struct super_block;
struct inode;
struct dentry;
struct dumb_fs_driver;

struct super_block {
	struct dumb_fs_driver *fs_drv; /* Assume that all FS have single driver */
	struct block_dev *bdev;

	struct dentry     *root;
	struct dlist_head *inode_list;

	struct super_block_operations *sb_ops;
	struct inode_operations	      *sb_iops;
	struct dentry_operations      *sb_dops;
	struct file_operations        *sb_fops;

	void *sb_data;
};

struct super_block_operations {
	struct inode *(*alloc_inode)(struct super_block *sb);
	int           (*destroy_inode)(struct inode *inode);
	int           (*write_inode)(struct inode *inode);
	int           (*umount_begin)(struct super_block *sb);
};

struct inode {
	int    i_no;
	int    start_pos; /* location on disk */
	size_t length;

	struct dentry *i_dentry;
	struct super_block *i_sb;
	struct inode_operations	*i_ops;

	void *i_data;
};

struct inode_operations {
	struct inode *(*create)(struct dentry *d_new, struct dentry *d_dir, int mode);
	struct inode *(*lookup)(char const *name, struct dentry const *dir);
	int           (*mkdir)(struct dentry *d_new, struct dentry *d_parent);
	int           (*rmdir)(struct dentry *dir);
	int           (*truncate)(struct inode *inode, size_t len);
	int           (*pathname)(struct inode *inode, char *buf);
};

struct dentry {
	char name[DENTRY_NAME_LEN];

	struct inode *d_inode;
	struct super_block *d_sb;

	struct dentry     *parent;
	struct dlist_head *next;     /* Next element in this directory */
	struct dlist_head *children; /* Subelements of directory */

	struct dentry_operations *d_ops;
};

struct dentry_operations {

};

struct file {
	struct dentry *f_dentry;
	struct inode  *f_inode;

	off_t pos;

	struct file_operations *f_ops;
};

struct file_operations {
	int    (*open)(struct inode *node, struct file *desc);
	int    (*close)(struct file *desc);
	size_t (*read)(struct file *desc, void *buf, size_t size);
	size_t (*write)(struct file *desc, void *buf, size_t size);
	int    (*ioctl)(struct file *desc, int request, ...);
};

struct dumb_fs_driver {
	const char name[FS_NAME_LEN];
};

#define DFS_CREAT 0x0001
extern struct inode  *dvfs_alloc_inode(struct super_block *sb);
extern int            dvfs_destroy_inode(struct inode *inode);

extern struct file   *dvfs_alloc_file(void);
extern int            dvfs_destroy_file(struct file *desc);

extern struct dentry *dvfs_alloc_dentry(void);
extern int            dvfs_destroy_dentry(struct dentry *dentry);

struct lookup;
extern struct dentry *dvfs_root(void);
int dvfs_lookup(const char *path, struct lookup *lookup);

extern int dvfs_open(const char *path, struct file *desc, int mode);
extern int dvfs_write(struct file *desc, char *buf, int count);
extern int dvfs_read(struct file *desc, char *buf, int count);

#endif

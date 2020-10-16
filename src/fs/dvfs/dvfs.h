/* @author Denis Deryugin
 * @date 17 Mar 2015
 *
 * Dumb VFS
 */

#ifndef _DVFS_H_
#define _DVFS_H_

#include <config/embox/fs/dvfs/core.h>
#include <framework/mod/options.h>
#define DENTRY_NAME_LEN   OPTION_MODULE_GET(embox__fs__dvfs__core, NUMBER, dentry_name_len)
#define DVFS_MAX_PATH_LEN OPTION_MODULE_GET(embox__fs__dvfs__core, NUMBER, max_path_len)

#include <drivers/block_dev.h>
#include <fs/idesc.h>
#include <fs/file_desc.h>
#include <util/dlist.h>
#include <fs/file_operation.h>
#include <fs/fs_driver.h>
#include <fs/inode_operation.h>
#include <fs/dir_context.h>
#include <fs/inode.h>
#include <fs/dentry.h>
#include <fs/super_block.h>
#include <sys/stat.h>

/*****************
 New VFS prototype
 *****************/
#define DVFS_PATH_FULL     0x001
#define DVFS_PATH_FS       0x002
#define DVFS_NAME          0x004

#define DVFS_CHILD_VIRTUAL 0x02000000
#define DVFS_MOUNT_POINT   0x04000000
#define DVFS_NO_LSEEK      0x08000000
#define DVFS_DISCONNECTED  0x10000000

struct dentry;
struct file_desc;
struct inode;
struct super_block;
struct lookup;
struct inode_operations;
struct dir_ctx;

struct super_block_operations {
	struct inode *(*alloc_inode)(struct super_block *sb);
	int           (*destroy_inode)(struct inode *inode);
	int           (*write_inode)(struct inode *inode);
	int           (*umount_begin)(struct super_block *sb);
	struct idesc *(*open_idesc)(struct lookup *l, int __oflag);
};

struct auto_mount {
	char mount_path[DVFS_MAX_PATH_LEN];
	struct fs_driver *fs_driver;
};

struct dvfsmnt {
	struct dvfsmnt *mnt_parent;
	struct dentry  *mnt_mountpoint;
	struct dentry  *mnt_root;
	struct super_block *mnt_sb;

	struct dlist_head children;
	struct dlist_head children_lnk;

	int flags;
};

extern struct dentry *dvfs_alloc_dentry(void);
extern int            dvfs_destroy_dentry(struct dentry *dentry);
extern int            dvfs_fs_dentry_try_free(struct super_block *sb);

extern struct dvfsmnt *dvfs_alloc_mnt(void);
extern int             dvfs_destroy_mnt(struct dvfsmnt *mnt);

extern struct dentry *dvfs_root(void);
extern int dvfs_remove(const char *path);
struct idesc *dvfs_file_open_idesc(struct lookup *lookup, int __oflag);
extern int dvfs_close(struct file_desc *desc);
extern int dvfs_write(struct file_desc *desc, char *buf, int count);
extern int dvfs_read(struct file_desc *desc, char *buf, int count);
extern int dvfs_fstat(struct file_desc *desc, struct stat *sb);
extern int dvfs_iterate(struct lookup *lookup, struct dir_ctx *ctx);
extern int dvfs_create_new(const char *name, struct lookup *lookup, int flags);
extern int dvfs_rename(struct dentry *from, struct dentry *to);

/* dcache-related stuff */
extern struct dentry *dvfs_cache_lookup(const char *path, struct dentry *base);
extern struct dentry *dvfs_cache_get(char *path, struct lookup *lookup);
extern int dvfs_cache_del(struct dentry *dentry);
extern int dvfs_cache_add(struct dentry *dentry);

extern struct block_dev *bdev_by_path(const char *source);
extern int dvfs_mount(const char *dev, const char *dest, const char *fstype, int flags);
extern int dvfs_umount(struct dentry *d);

extern int dentry_fill(struct super_block *, struct inode *,
                       struct dentry *d, struct dentry *parent);
extern void dentry_upd_flags(struct dentry *dentry);
extern int dentry_full_path(struct dentry *dentry, char *buf);
extern int dentry_ref_inc(struct dentry *dentry);
extern int dentry_ref_dec(struct dentry *dentry);
extern int dentry_disconnect(struct dentry *dentry);
extern int dentry_reconnect(struct dentry *parent, const char *name);

#endif

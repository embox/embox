/**
 * @file
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 10.01.2020
 */

#ifndef FS_DENTRY_H_
#define FS_DENTRY_H_

struct inode;

struct dentry {
	int usage_count;

	struct inode *d_inode;

	struct dentry     *parent;

	int flags;
};

struct lookup {
	struct dentry *item;
	struct dentry *parent;
};

extern int dvfs_lookup(const char *path, struct lookup *lookup);
extern int dvfs_pathname(struct inode *inode, char *buf, int flags);
extern struct dentry *dvfs_root(void);
extern int dentry_full_path(struct dentry *dentry, char *buf);
extern int dentry_ref_inc(struct dentry *dentry);
extern int dentry_ref_dec(struct dentry *dentry);
extern struct mount_descriptor *mount_desc_by_inode(struct inode *node);

#endif /* FS_DENTRY_H_ */

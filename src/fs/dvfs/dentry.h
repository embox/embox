/**
 * @file
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 10.01.2020
 */

#ifndef FS_DENTRY_H_
#define FS_DENTRY_H_

#include <limits.h>

#include <lib/libds/dlist.h>

struct inode;
struct super_block;
struct dentry;
struct lookup;

struct dentry {
	char name[NAME_MAX];

	int flags;
	int usage_count;

	struct inode *d_inode;
	struct super_block *d_sb;

	struct dentry     *parent;
	struct dlist_head children; /* Sub-elements of directory */
	struct dlist_head children_lnk;

	struct dlist_head d_lnk;   /* List for all dentries in system */
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

#endif /* FS_DENTRY_H_ */

/* @author Denis Deryugin
 * @date 17 Mar 2015
 *
 * Dumb VFS
 */

#ifndef _DVFS_H_
#define _DVFS_H_

#include <config/embox/fs/dvfs/core.h>
#include <framework/mod/options.h>

#include <kernel/task/resource/idesc.h>
#include <fs/file_desc.h>
#include <lib/libds/dlist.h>
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
/* The name is gone from the tree (unlink, rename over it) but somebody still
 * holds the dentry. It is off its parent's list, keeps its reference on the
 * parent, and is freed by whoever drops the last reference. */
#define DVFS_DYING         0x20000000

struct dentry;
struct file_desc;
struct inode;
struct super_block;
struct lookup;
struct inode_operations;
struct dir_ctx;
struct block_dev;

/* LOCKING
 *
 * dvfs_lock() guards the name tree: the list of all dentries, children lists,
 * usage_count, the dentry flags, d_covered, the root and every task's working
 * directory; reclaim and mount/umount run under it. It is a recursive
 * sleeping mutex: driver inode and superblock ops run under it and sleep.
 *
 * Order: dvfs_lock -> driver lock (fat_lock) -> block device
 *                  -> idesc table lock, pool spinlocks
 * A driver never calls back into the VFS. Never take dvfs_lock with a driver
 * lock held, nor from interrupt context. File data (kread/kwrite/lseek) is
 * not under it: a descriptor only pins its dentry.
 *
 * REFERENCES
 *
 * usage_count changes only under the lock. A dentry is held by each child
 * linked under it, each descriptor and DIR on it, each task whose working
 * directory it is, a mount covering it, and a lookup's result. An
 * unreferenced dentry stays as cache, reclaimed as a leaf under the lock when
 * a pool runs out.
 */
extern void dvfs_lock(void);
extern void dvfs_unlock(void);
/* Counts (and names, once) a place that reached the tree without the lock. */
extern void dvfs_lock_assert(const char *where);

extern unsigned long vfs_tree_contended;  /* dvfs_lock() had to wait */
extern unsigned long dvfs_unlocked_tree;  /* dvfs_lock_assert() misses */
extern unsigned long dvfs_reclaimed;      /* cached dentries reclaimed */
extern unsigned long dvfs_umount_busy;    /* umounts refused as busy */

extern struct dentry *dvfs_alloc_dentry(void);
extern int            dvfs_destroy_dentry(struct dentry *dentry);
extern int            dvfs_fs_dentry_try_free(struct super_block *sb);

extern struct dentry *dvfs_root(void);
extern int dvfs_remove(const char *path);
struct idesc *dvfs_file_open_idesc(struct lookup *lookup, int __oflag);

/* Resolve PATH starting at BASE (NULL: the root for an absolute path, the
 * task's working directory for a relative one). Called with dvfs_lock held.
 *
 * On 0:   lookup->parent is referenced; lookup->item is referenced, or NULL
 *         when the last component does not exist -- then LAST, if given
 *         (NAME_MAX bytes), gets that component's name.
 * On error nothing is referenced: -ENOENT (an earlier component is
 *         missing), -ENOTDIR, -ENAMETOOLONG, -ENOMEM.
 *
 * Release with dvfs_lookup_put(). */
extern int dvfs_lookup_at(struct dentry *base, const char *path,
    struct lookup *lookup, char *last);
extern void dvfs_lookup_put(struct lookup *lookup);

extern int dvfs_iterate(struct lookup *lookup, struct dir_ctx *ctx);
extern int dvfs_create_new(const char *name, struct lookup *lookup, int flags);

/* Whole operations: each resolves its path and acts in one hold of the lock,
 * as dvfs_remove() does. 0 or -errno. */
extern int dvfs_unlink(const char *path);  /* not a directory */
extern int dvfs_rmdir(const char *path);   /* an empty directory */
extern int dvfs_rename(const char *from, const char *to);

/* The name of an open dentry goes away: off the parent's list, DYING, freed
 * by the last reference. Under the lock. */
extern void dentry_unlink_dying(struct dentry *d);

/* dcache-related stuff */
extern struct dentry *dvfs_cache_lookup(const char *path, struct dentry *base);
extern struct dentry *dvfs_cache_get(char *path, struct lookup *lookup);
extern int dvfs_cache_del(struct dentry *dentry);
extern int dvfs_cache_add(struct dentry *dentry);

extern struct block_dev *bdev_by_path(const char *source);
extern int dvfs_mount(const char *dev, const char *dest, const char *fstype, int flags);
extern int dvfs_umount(struct dentry *d);
extern int dvfs_umount_path(const char *path);

extern int dentry_fill(struct super_block *, struct inode *,
                       struct dentry *d, struct dentry *parent);
extern void dentry_upd_flags(struct dentry *dentry);
extern int dentry_full_path(struct dentry *dentry, char *buf);
extern int dentry_ref_inc(struct dentry *dentry);
extern int dentry_ref_dec(struct dentry *dentry);

#endif

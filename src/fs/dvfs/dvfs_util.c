/* @file
 * @brief  DVFS allocators, fillers and default handlers
 * @author Denis Deryugin
 * @date   8 Apr 2014
 */

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include <fs/dvfs.h>
#include <framework/mod/options.h>
#include <kernel/sched/current.h>
#include <kernel/thread/sync/mutex.h>
#include <mem/misc/pool.h>
#include <lib/libds/dlist.h>
#include <util/atomic_rmw.h>
#include <util/log.h>

#define INODE_POOL_SIZE OPTION_GET(NUMBER, inode_pool_size)
#define DENTRY_POOL_SIZE OPTION_GET(NUMBER, dentry_pool_size)
#define FILE_POOL_SIZE OPTION_GET(NUMBER, file_pool_size)
#define POISON_FREE OPTION_GET(BOOLEAN, poison_free)

POOL_DEF(inode_pool, struct inode, INODE_POOL_SIZE);
POOL_DEF(dentry_pool, struct dentry, DENTRY_POOL_SIZE);
POOL_DEF(file_pool, struct file_desc, FILE_POOL_SIZE);

/* The tree lock; see LOCKING in dvfs.h. Recursive: paths through the drivers
 * come back into the tree (a reclaim inside an allocation inside a lookup, a
 * put inside a destroy). */
static struct mutex dvfs_tree_mutex = RMUTEX_INIT_STATIC;

/* Before the first thread the mutex cannot be taken and nobody races;
 * task_vfs_init makes the root then, so the lock only counts its depth. */
static int dvfs_lock_early;

unsigned long vfs_tree_contended;
unsigned long dvfs_unlocked_tree;
unsigned long dvfs_reclaimed;
unsigned long dvfs_umount_busy;
unsigned long inode_free_deferred;
unsigned long inode_ref_refused;

static unsigned int dvfs_inode_gen;

void dvfs_lock(void) {
	if (schedee_get_current() == NULL) {
		dvfs_lock_early++;
		return;
	}
	if (0 != mutex_trylock(&dvfs_tree_mutex)) {
		atomic_rmw_add_fetch(&vfs_tree_contended, 1, __ATOMIC_RELAXED);
		mutex_lock(&dvfs_tree_mutex);
	}
}

void dvfs_unlock(void) {
	if (dvfs_lock_early > 0) {
		dvfs_lock_early--;
		return;
	}
	mutex_unlock(&dvfs_tree_mutex);
}

void dvfs_lock_assert(const char *where) {
	struct schedee *self = schedee_get_current();

	if (dvfs_lock_early > 0 || (self && dvfs_tree_mutex.holder == self)) {
		return;
	}
	if (dvfs_unlocked_tree == 0) {
		log_error("UNLOCKED TREE: %s reached the name tree without dvfs_lock",
		    where);
	}
	dvfs_unlocked_tree++;
}

#define FREE_DENTRY_ANY    0
#define FREE_DENTRY_INODE  1
static int dvfs_free_dentry(int with_ino);
static void dentry_destroy_locked(struct dentry *dentry);

/* Default FS-nondependent operations */
/* @brief Alloc inode and set it superblock and inode_operations
 * @param sb Superblock of related file system
 *
 * @return Pointer to the new inode
 * @retval NULL inode could not be allocated
 */
struct inode *dvfs_alloc_inode(struct super_block *sb) {
	struct inode *inode;
	if (!sb)
		return NULL;

	dvfs_lock_assert("dvfs_alloc_inode");

	inode = pool_alloc(&inode_pool);
	if (!inode) {
		if (!dvfs_free_dentry(FREE_DENTRY_INODE)) {
			inode = pool_alloc(&inode_pool);
		}
		if (!inode) {
			return NULL;
		}
	}

	*inode = (struct inode) {
		.i_no = -1,
		.i_sb = sb,
		.i_ops = sb->sb_iops,
		.i_gen = ++dvfs_inode_gen,
	};

	return inode;
}

/* @brief Remove inode from inode_pool
 * @param inode inode to be removed from pool
 *
 * @retval 0 Ok
 */
int dvfs_default_destroy_inode(struct inode *inode) {
	if (POISON_FREE) {
		memset(inode, 0x5a, sizeof(*inode));
	}
	pool_free(&inode_pool, inode);
	return 0;
}

/* @brief Try to resolve pathname according to the dentry
 * @param inode The inode which is to be path-resolved
 * @param buf   Buffer for the path
 * @param flags Used to figure out pathname format, see dvfs_pathname doc
 *
 * @retval 0 Ok
 */
int dvfs_default_pathname(struct inode *inode, char *buf, int flags) {
	assert(inode);
	if (inode->i_dentry)
		strcpy(buf, inode_name(inode));
	else
		strcpy(buf, "empty");

	return 0;
}

extern int dvfs_default_destroy_inode(struct inode *);
/* @brief Try to destroy the inode
 * @param inode Pointer to the inode to be destroyed
 *
 * @return Negative error code
 * @retval 0 Ok
 */
int dvfs_destroy_inode(struct inode *inode) {
	assert(inode);

	if (inode->i_dentry && inode->i_dentry->d_inode == inode)
		inode->i_dentry->d_inode = NULL;

	if (inode->i_sb && inode->i_sb->sb_ops &&
	    inode->i_sb->sb_ops->destroy_inode)
		inode->i_sb->sb_ops->destroy_inode(inode);
	return dvfs_default_destroy_inode(inode);
}

/**
* @brief Double-linked list of all dentries
*/
DLIST_DEFINE(dentry_dlist);

static struct dentry *global_root = NULL;

/* Reclaim may take D only if it is in the tree (a dentry being built has no
 * parent yet), unheld, childless and not kept by the tree's structure. */
static int dentry_reclaimable(struct dentry *d) {
	return d->parent != NULL
	    && d->usage_count == 0
	    && dlist_empty(&d->children)
	    && d != global_root
	    && d->d_covered == NULL
	    && !(d->flags & (DVFS_MOUNT_POINT | DVFS_DYING));
}

/**
 * @brief Free a cached dentry: nobody holds it and nothing is under it.
 *
 * @param mode  FREE_DENTRY_ANY	   Find any dentry to delete
 *              FREE_DENTRY_INODE  Find dentry with inodes
 *
 * @return 0      if succeeded
 *         -EBUSY if no free dentry found
 *
 * Runs inside an allocation, possibly in the middle of a path walk; the walk
 * holds every dentry it stands on (see dvfs_path_walk()), so none is taken.
 */
static int dvfs_free_dentry(int mode) {
	struct dentry *dentry;

	dvfs_lock_assert("dvfs_free_dentry");

	dlist_foreach_entry(dentry, &dentry_dlist, d_lnk) {
		if (mode == FREE_DENTRY_INODE && dentry->d_inode == NULL)
			continue;

		if (dentry_reclaimable(dentry)) {
			dentry_destroy_locked(dentry);
			atomic_rmw_add_fetch(&dvfs_reclaimed, 1, __ATOMIC_RELAXED);
			return 0;
		}
	}

	return -EBUSY;
}

/* @brief Get new dentry from pool
 *
 * @return Pointer to the new dentry
 * @retval NULL Pool is full
 */
struct dentry *dvfs_alloc_dentry(void) {
	struct dentry *dentry;

	dvfs_lock_assert("dvfs_alloc_dentry");

	dentry = pool_alloc(&dentry_pool);
	if (!dentry) {
		if (!dvfs_free_dentry(FREE_DENTRY_ANY)) {
			dentry = pool_alloc(&dentry_pool);
		}
		if (!dentry) {
			return NULL;
		}
	}

	memset(dentry, 0, sizeof(struct dentry));

	dlist_head_init(&dentry->d_lnk);
	dlist_add_next(&dentry->d_lnk, &dentry_dlist);
	dlist_init(&dentry->children);
	dlist_head_init(&dentry->children_lnk);

	return dentry;
}

extern int dvfs_cache_del(struct dentry *dentry);

/* Frees an unheld dentry and the reference its place kept on the parent */
static void dentry_destroy_locked(struct dentry *dentry) {
	struct dentry *parent;

	assert(dentry->usage_count == 0);
	assert(dentry != global_root);

	if (dentry->d_inode) {
		dvfs_destroy_inode(dentry->d_inode);
	}

	parent = dentry->parent;
	if (parent && parent != dentry) {
		/* A dying dentry is already off the list; dlist_del_init() of it
		 * changes nothing. */
		dlist_del_init(&dentry->children_lnk);
		dvfs_cache_del(dentry);
	}

	dlist_del(&dentry->d_lnk);

	if (POISON_FREE) {
		memset(dentry, 0x5a, sizeof(*dentry));
	}
	pool_free(&dentry_pool, dentry);

	if (parent && parent != dentry) {
		dentry_ref_dec(parent);
	}
}

/**
 * @brief Remove dentry from pool
 *
 * @retval 0      Freed
 * @retval -EBUSY Somebody holds it
 */
int dvfs_destroy_dentry(struct dentry *dentry) {
	int res = -EBUSY;

	dvfs_lock();
	log_debug("destroy %p", dentry);
	assert(dentry->usage_count >= 0);
	if (dentry->usage_count == 0 && dentry != global_root) {
		dentry_destroy_locked(dentry);
		res = 0;
	}
	dvfs_unlock();

	return res;
}

/*
 * @brief Try to free a single dentry from given FS
 *
 * @retval Number of freed dentries
 */
int dvfs_fs_dentry_try_free(struct super_block *sb) {
	struct dentry *dentry;

	assert(sb);
	dvfs_lock_assert("dvfs_fs_dentry_try_free");

	dlist_foreach_entry(dentry, &dentry_dlist, d_lnk) {
		if (sb == dentry->d_sb && dentry_reclaimable(dentry)) {
			dentry_destroy_locked(dentry);
			atomic_rmw_add_fetch(&dvfs_reclaimed, 1, __ATOMIC_RELAXED);
			return 1;
		}
	}

	return 0;
}

/**
 * @brief Update dentry flags according to it's inode content
 *
 * @param dentry Pointer to dentry to be updated
 */
void dentry_upd_flags(struct dentry *dentry) {
	if (dentry->d_inode) {
		dentry->flags |= dentry->d_inode->i_mode & (S_IFMT | S_IRWXA);
	}
}

/* @brief Get new file descriptor from pool
 *
 * @return Pointer to the new file descriptor
 * @retval NULL Pool is full
 */
struct file_desc *dvfs_alloc_file(void) {
	return pool_alloc(&file_pool);
}

/* @brief Remove file descriptor from pool
 */
int dvfs_destroy_file(struct file_desc *desc) {
	pool_free(&file_pool, desc);
	return 0;
}

/* Fillers */
/* @brief Fills inode fields according to existing dentry and SB
 * @param inode      The structure to be filled
 * @param superblock The superblock of FS of the inode
 * @param dentry     The dentry related to inode
 */
int inode_fill(struct super_block *sb, struct inode *inode,
                      struct dentry *dentry) {
	inode->i_dentry = dentry;
	inode->i_sb     = sb;
	inode->i_ops    = sb ? sb->sb_iops : NULL;
	/* Other fields are left without changes on purpose */

	return 0;
}
/* @brief Fills dentry fields according to existing superblok,
 *        inode and prent dentry
 * @param superblock The superblock of FS of the dentry
 * @param inode      The inode related to dentry
 * @param dentry     The structure to be filled
 * @param parent     The parent of the denrty
 */
int dentry_fill(struct super_block *sb, struct inode *inode,
                      struct dentry *dentry, struct dentry *parent) {
	dvfs_lock_assert("dentry_fill");

	dentry->d_inode     = inode;
	dentry->d_sb        = sb;
	dentry->parent      = parent;

	if (inode) {
		inode->i_dentry = dentry;
	}

	dlist_init(&dentry->children);
	dlist_head_init(&dentry->children_lnk);

	if (parent) {
		dlist_add_prev(&dentry->children_lnk, &parent->children);
		dentry_ref_inc(parent);
	}
	return 0;
}

int dentry_ref_inc(struct dentry *dentry) {
	int res;

	assert(dentry);
	dvfs_lock();
	log_debug("dentry inc %p %s (%d->%d)",
			dentry, dentry->name, dentry->usage_count,
			dentry->usage_count + 1);
	res = ++dentry->usage_count;
	dvfs_unlock();

	return res;
}

/* Drops one reference. The last one frees a DYING dentry; a cached one stays
 * for reclaim. Returns the count left. */
int dentry_ref_dec(struct dentry *dentry) {
	int res;

	assert(dentry);
	dvfs_lock();
	log_debug("dentry dec %p %s (%d->%d)",
			dentry, dentry->name, dentry->usage_count,
			dentry->usage_count - 1);
	assert(dentry->usage_count > 0);
	res = --dentry->usage_count;
	if (res == 0 && (dentry->flags & DVFS_DYING)) {
		if (dentry->d_inode && dentry->d_inode->i_dying) {
			/* An unlinked file, freed by its last holder */
			atomic_rmw_add_fetch(&inode_free_deferred, 1, __ATOMIC_RELAXED);
		}
		dentry_destroy_locked(dentry);
	}
	dvfs_unlock();

	return res;
}

void dentry_unlink_dying(struct dentry *d) {
	dvfs_lock_assert("dentry_unlink_dying");
	assert(d->usage_count > 0);

	d->flags |= DVFS_DYING;
	if (d->d_inode) {
		d->d_inode->i_dying = 1;
	}
	dvfs_cache_del(d);
	/* Off the list, so no lookup finds the name again; the reference on the
	 * parent stays until the dentry is freed. */
	dlist_del_init(&d->children_lnk);
}

/* Root-related stuff */
extern struct super_block *rootfs_sb(void);

/* @brief Create the global root and give it the root superblock once mounted
 *
 * The root is never replaced, since every working directory, lookup and mount
 * holds it; a second mount on "/" is refused in dvfs_mount().
 */
int dvfs_update_root(void) {
	struct super_block *sb;
	struct inode *inode;

	dvfs_lock();

	if (global_root == NULL) {
		global_root = dvfs_alloc_dentry();
		assert(global_root);
		global_root->usage_count = 1;
		global_root->parent = global_root;
		strcpy(global_root->name, "/");
		global_root->flags = S_IFDIR | VFS_DIR_VIRTUAL | DVFS_MOUNT_POINT;
	}

	sb = rootfs_sb();

	if (sb != NULL && global_root->d_sb == NULL) {
		inode = sb->sb_root;
		assert(inode);

		global_root->d_sb = sb;
		global_root->d_inode = inode;
		inode->i_dentry = global_root;
	}

	dvfs_unlock();

	return 0;
}

/* @brief Global root getter
 * @return Pointer to the global VFS root
 */
struct dentry *dvfs_root(void) {
	if (!global_root) {
		dvfs_update_root();
	}

	return global_root;
}

/**
* @brief Check if element with given name presents as a subelement
*        of the folder in RAM.
*
* @param parent
* @param name
*
* @return Pointer to dentry if found or NULL if not
*/
struct dentry *local_lookup(struct dentry *parent, char *name) {
	struct dentry *d;
	struct dlist_head *l;

	dvfs_lock_assert("local_lookup");

	dlist_foreach(l, &parent->children) {
		if (l == &parent->children)
			continue;
		d = mcast_out(l, struct dentry, children_lnk);

		if (POISON_FREE) {
			assert(d->flags != 0x5a5a5a5a);
		}

		if (!strcmp(d->name, name)) {
			return d;
		}
	}

	return NULL;
}

/**
 * @file
 * @brief pool of nodes
 *
 * @date 06.10.10
 * @author Nikolay Korotky
 */

#ifndef FS_NODE_H_
#define FS_NODE_H_

#include <fcntl.h>
//#include <sys/stat.h>
#include <limits.h>

#include <lib/libds/slist.h>
#include <lib/libds/tree.h>

#include <kernel/thread/sync/mutex.h>

#include <fs/super_block.h>

struct inode_operations;
struct super_block;
struct dentry;

struct flock_shared {
	struct thread *holder;
	struct dlist_head flock_link;
};

struct node_flock {
	struct mutex      exlock;
	long              shlock_count;
	struct dlist_head shlock_holders;
	spinlock_t        flock_guard;
};

struct inode {
	int      i_no;
	size_t        i_size;
	unsigned int  i_ctime; /* time of last status change */
	unsigned int  i_mtime;

	mode_t                i_mode;/* discrete access mode Read-Write-Execution */
	uid_t                 i_owner_id;/* owner user ID */
	gid_t                 i_group_id;/* owner group ID */

	struct dentry        *i_dentry;
	struct super_block      *i_sb;
	struct inode_operations *i_ops;

	void                 *i_privdata;

	unsigned int i_nlink;

	/* i_nlink counts names; i_ref counts users of the pointer.
	 * Between finding a path and using it, another core can unlink the
	 * inode. i_ref protects against this; i_dying marks an inode that
	 * left the name tree but still has users. Both are accessed under
	 * vfs.c's tree lock. */
	int i_ref;
	int i_dying;

	/* Generation number for this pool slot allocation.
	 * A descriptor holds a pointer, and a freed-and-reused pointer is
	 * still valid -- to somebody else's file. The generation catches this:
	 * whoever holds this inode records the number it saw, and a mismatch
	 * means the slot changed hands. */
	unsigned int i_gen;

	struct slist_link dirent_link;

	/* node name (use vfs_get_path_by_node() for get full path*/
	char                  name[NAME_MAX + 1];

	int                   mounted; /* is mount point*/

	struct node_flock     flock;

	/* service data structure for enabling tree operation */
	struct tree_link      tree_link;
};

extern struct inode *inode_new(struct super_block *sb);
extern void inode_del(struct inode *node);

/**
 * @param name Non-empty string.
 * @param name_len (optional) how many bytes to take from name.
 *    If zero, the name must be a null-terminated string.
 */
extern struct inode *inode_alloc(struct super_block *sb);

/* inode_ref() returns 0 if the inode is dying (caller must not use it).
 * Every successful reference is matched by exactly one inode_unref().
 * Both live in vfs.c, next to the lock that orders them against vfs_del_leaf(). */
extern int inode_ref(struct inode *node);
extern void inode_unref(struct inode *node);

/* "The tree is done with this inode." Frees it if nobody holds it, marks it
 * dying if somebody does -- the last inode_unref() frees it then. This is
 * the ONLY way an inode that was ever reachable may be given up. */
extern void inode_release(struct inode *node);

/* Hands the filesystem back whatever it hung off this inode, now. Needed
 * before the superblock goes, because a deferred inode outlives the unlink. */
extern void inode_detach_fs(struct inode *node);

/* Frees postponed because somebody still held the inode, and references
 * refused because it was already dying. Both are evidence that the window
 * this counting exists to close is a real one. */
extern unsigned long inode_free_deferred;
extern unsigned long inode_ref_refused;

/* Uses of a descriptor whose inode is not the one it opened, and uses of one
 * whose inode has left the tree. Both must be zero; a non-zero value names a
 * silent corruption that would otherwise be found by its consequences. */
extern unsigned long fdesc_stale_gen;
extern unsigned long fdesc_dead_inode;

/* Walks that arrived at an inode after it left the name tree. */
extern unsigned long vfs_walk_dying;

extern void inode_free(struct inode *node);
extern void *inode_priv(const struct inode *node);
extern void inode_priv_set(struct inode *node, void *priv);
extern size_t inode_size(const struct inode *node);
extern void inode_size_set(struct inode *node, size_t sz);
extern unsigned inode_ctime(const struct inode *node);
extern void inode_ctime_set(struct inode *node, unsigned ctime);
extern unsigned inode_mtime(const struct inode *node);
extern void inode_mtime_set(struct inode *node, unsigned mtime);
extern char *inode_name(struct inode *node);
extern char *inode_name_set(struct inode *node, const char *name);
#if 0
static inline int node_is_directory(struct inode *node) {
	return S_ISDIR(node->i_mode);
}
#endif
#endif /* FS_NODE_H_ */

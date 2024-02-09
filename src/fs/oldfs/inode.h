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
#include <sys/stat.h>
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
extern struct inode *node_alloc(const char *name);

extern void node_free(struct inode *node);
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

static inline int node_is_directory(struct inode *node) {
	return S_ISDIR(node->i_mode);
}

#endif /* FS_NODE_H_ */

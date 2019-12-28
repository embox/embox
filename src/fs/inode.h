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

#include <util/slist.h>
#include <util/tree.h>

#include <kernel/thread/sync/mutex.h>

#include <fs/file_system.h>

struct nas;

struct inode_operations;
struct super_block;

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
	unsigned int i_nlink;
	struct slist_link dirent_link;

	/* node name (use vfs_get_path_by_node() for get full path*/
	char                  name[NAME_MAX + 1];

	mode_t                mode;/* discrete access mode Read-Write-Execution */
	uid_t                 uid;/* owner user ID */
	gid_t                 gid;/* owner group ID */

	/* node attribute structure (extended information about node)*/
	struct nas            *nas;
	struct inode_operations *i_ops;

	int                   mounted; /* is mount point*/

	struct node_flock     flock;

	/* service data structure for enabling tree operation */
	struct tree_link      tree_link;
};

struct node_info {
	size_t        size;
	unsigned int  ctime; /* time of last status change */
	unsigned int  mtime;
};

struct node_fi {
	struct node_info ni;
	void  *privdata;
};

struct nas {
	struct inode          *node;
	struct filesystem    *fs;
	struct node_fi       *fi;
};

extern struct inode *inode_new(struct super_block *sb);
extern void inode_del(struct inode *node);

/**
 * @param name Non-empty string.
 * @param name_len (optional) how many bytes to take from name.
 *    If zero, the name must be a null-terminated string.
 */
extern struct inode *node_alloc(const char *name, size_t name_len);

extern void node_free(struct inode *node);

static inline int node_is_directory(struct inode *node) {
	return S_ISDIR(node->mode);
}

#endif /* FS_NODE_H_ */

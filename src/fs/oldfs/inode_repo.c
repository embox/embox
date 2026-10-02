/**
 * @file
 * @brief pool of nodes
 *
 * @date 06.10.10
 * @author Nikolay Korotky
 * @author Eldar Abusalimov
 */

#include <string.h>
#include <limits.h>
#include <errno.h>

#include <mem/misc/pool.h>

#include <util/math.h>

#include <fs/inode.h>
#include <fs/dentry.h>
#include <fs/vfs.h>

#include <framework/mod/options.h>

#define MAX_NODE_QUANTITY OPTION_GET(NUMBER,fnode_quantity)

POOL_DEF(node_pool, struct inode, MAX_NODE_QUANTITY);
POOL_DEF(dentry_pool, struct dentry, MAX_NODE_QUANTITY);

static inline int flock_init(struct inode *node) {
	/* flock initialization */
	mutex_init(&node->flock.exlock);
	node->flock.shlock_count = 0;
	dlist_init(&node->flock.shlock_holders);
	spin_init(&node->flock.flock_guard, __SPIN_UNLOCKED);

	return ENOERR;
}

struct inode *inode_new(struct super_block *sb) {
	struct inode *node;
	struct dentry *dentry;

	node = pool_alloc(&node_pool);
	if (!node) {
		return NULL;
	}
	dentry = pool_alloc(&dentry_pool);
	if (!dentry) {
		pool_free(&node_pool, &node_pool);
		return NULL;
	}

	memset(node, 0, sizeof(struct inode));

	node->i_dentry = dentry;
	dentry->d_inode = node;

	node->i_sb = sb;

	tree_link_init(&node->tree_link);

	node->mounted = 0;

	flock_init(node);

	node->i_nlink = 0;
	slist_link_init(&node->dirent_link);

	return node;
}

void inode_del(struct inode *node) {
	node->i_nlink --;
	if (0 == node->i_nlink) {
		vfs_del_leaf(node);
	}
}

struct inode *inode_alloc(struct super_block *sb) {
	struct inode *node;

	node = inode_new(sb);
	if (!node) {
		return NULL;
	}

	/* it's for permanent linked inode to file tree */
	node->i_nlink++;

	return node;
}

void inode_free(struct inode *node) {
	if (node->i_dentry) {
		pool_free(&dentry_pool, node->i_dentry);
	}
	pool_free(&node_pool, node);
}

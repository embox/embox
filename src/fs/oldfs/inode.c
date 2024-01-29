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

#include <fs/inode.h>
#include <fs/dentry.h>
#include <fs/vfs.h>

#include <framework/mod/options.h>

#define MAX_NODE_QUANTITY OPTION_GET(NUMBER,fnode_quantity)

struct node_tuple {
	struct inode node;
	struct nas nas;
	struct node_fi fi;
};

POOL_DEF(node_pool, struct node_tuple, MAX_NODE_QUANTITY);
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
	struct node_tuple *nt;
	struct inode *node;
	struct dentry *dentry;
	struct nas *nas;

	nt = pool_alloc(&node_pool);
	if (!nt) {
		return NULL;
	}
	dentry = pool_alloc(&dentry_pool);
	if (!dentry) {
		pool_free(&node_pool, nt);
		return NULL;
	}

	memset(nt, 0, sizeof(struct node_tuple));

	node = &nt->node;
	nas = &nt->nas;

	node->i_dentry = dentry;
	dentry->d_inode = node;

	node->nas = nas;
	nas->node = node;
	node->i_sb = sb;

	nas->fi = &nt->fi;

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

struct inode *node_alloc(const char *name, size_t name_len) {
	struct inode *node;
	char *node_name;

	if (!name_len) {
		name_len = strlen(name);
	}

	if (name_len > NAME_MAX) {
		return NULL;
	}

	node = inode_new(NULL);
	if (!node) {
		return NULL;
	}

	node_name = inode_name(node);
	strncpy(node_name, name, name_len);
	node_name[name_len] = '\0';

	/* it's for permanent linked inode to file tree */
	node->i_nlink++;

	return node;
}

void node_free(struct inode *node) {
	if (node->i_dentry) {
		pool_free(&dentry_pool, node->i_dentry);
	}
	pool_free(&node_pool, member_cast_out(node, struct node_tuple, node));
}

void *inode_priv(const struct inode *node) {
	assert(node);

	return node->nas->fi->privdata;
}

void inode_priv_set(struct inode *node, void *priv) {
	assert(node);

	node->nas->fi->privdata = priv;
}

size_t inode_size(const struct inode *node) {
	return node->nas->fi->ni.size;
}

void inode_size_set(struct inode *node, size_t sz) {
	node->nas->fi->ni.size = sz;
}

unsigned inode_ctime(const struct inode *node) {
	return node->i_ctime;
}

void inode_ctime_set(struct inode *node, unsigned ctime) {
	node->i_ctime = ctime;
}

unsigned inode_mtime(const struct inode *node) {
	return node->i_mtime;
}

void inode_mtime_set(struct inode *node, unsigned mtime) {
	node->i_mtime = mtime;
}

char *inode_name(struct inode *node) {
	return node->name;
}

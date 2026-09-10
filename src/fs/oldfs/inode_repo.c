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
#include <fs/super_block.h>
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

static unsigned int inode_gen_next;

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

	/* Monotonic, so that a pool slot handed out again is
	 * never mistaken for the same inode. Wrapping takes 2^32 allocations, and
	 * a wrap only costs one collision, not a class of them. */
	node->i_gen = ++inode_gen_next;

	node->i_dentry = dentry;
	dentry->d_inode = node;

	node->i_sb = sb;

	tree_link_init(&node->tree_link);

	node->mounted = 0;

	flock_init(node);

	node->i_nlink = 0;
	node->i_ref = 0;
	node->i_dying = 0;
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

void inode_detach_fs(struct inode *node) {
	/* An inode may now outlive the unlink that took it out
	 * of the tree -- a descriptor or a directory walk can still be standing on
	 * it. What it must NOT outlive is its filesystem: umount frees the
	 * superblock, and inode_free() would then ask a freed superblock for its
	 * destroy_inode(). So on the umount path the private data is handed back
	 * here, while the superblock is still there, and the inode goes on holding
	 * nothing. Safe twice, and safe on an inode that never had any. */
	/* See file_desc_alloc(). Same reason: another core
	 * may be clearing i_sb, and a guard that reads it twice can dereference
	 * the NULL it just checked. */
	{
		struct super_block *sb = node->i_sb;

		if (sb && sb->sb_ops && sb->sb_ops->destroy_inode) {
			sb->sb_ops->destroy_inode(node);
		}
	}
	node->i_sb = NULL;
}

void inode_free(struct inode *node) {
	/* Hand the filesystem back whatever it hung off this inode
	 * before the inode itself goes on the pool. oldfs never called
	 * destroy_inode, and kmount() walks the whole volume and makes an inode
	 * per file, so every mount leaked one driver pool entry per file until
	 * creates started failing with ENOMEM. Every destroy_inode in the tree
	 * tolerates a null private pointer and clears it afterwards, so this is
	 * safe on an inode that never had one and safe twice. */
	/* Read i_sb once, as in inode_detach_fs() above. */
	{
		struct super_block *sb = node->i_sb;

		if (sb && sb->sb_ops && sb->sb_ops->destroy_inode) {
			sb->sb_ops->destroy_inode(node);
		}
	}
	if (node->i_dentry) {
		pool_free(&dentry_pool, node->i_dentry);
	}
	/* Poison, so that a reference released on an inode that is
	 * already gone can say "already freed" rather than "never referenced".
	 * A fresh inode from the pool has i_ref 0; this one has -1. */
	node->i_ref = -1;
	node->i_dying = -1;
	pool_free(&node_pool, node);
}

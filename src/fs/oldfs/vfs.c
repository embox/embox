/**
 * @file
 * @brief Describes tree of VFS (Virtual Filesystem Switch).
 * @details An abstraction layer on top of a more concrete file systems.
 *
 * @date 12.10.10
 * @author Anton Bondarev
 * @author Eldar Abusalimov
 * @author Vita Loginova
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include <fs/dentry.h>
#include <fs/hlpr_path.h>
#include <fs/inode.h>
#include <fs/vfs.h>
#include <kernel/printk.h>
#include <fs/super_block.h>
#include <fs/mount.h>

#include <limits.h>

#include <hal/ipl.h>
#include <kernel/spinlock.h>
#include <util/atomic_rmw.h>

#define ROOT_MODE 0775

#define ERR_CHILD_NOT_FOUND 1
#define ERR_CHILD_MOUNTED   2

static struct inode *__vfs_get_parent(struct inode *child);
static int __vfs_subtree_lookup_existing(struct inode *parent,
		const char *str_path, const char **p_end_existent, struct inode **child);
static struct inode * __vfs_subtree_create(struct inode *parent, const char *path,
		mode_t mode, int intermediate);
static struct inode *__vfs_subtree_create_child(struct inode *parent, const char *name,
		size_t len, mode_t mode);

struct lookup_tuple {
	const char *name;
	size_t len;
};

/* The namespace tree's child lists are walked by every
 * path lookup and rewritten by every create, unlink and mount. Nothing
 * serialised the two, and `affinity.smp(default_mask=1)` -- the fence that
 * keeps ordinary threads on the boot core -- is what has been standing in for
 * a lock here.
 *
 * The structure this protects is the CHILD LIST, and only that. A parent
 * pointer is one word: a reader can see a stale one, never a torn one, and
 * following a stale parent is the same wrong answer it was on one core. A
 * child list being walked while another core unlinks from it is a different
 * kind of wrong, and it is the kind that ends in an address that is not a
 * pointer.
 *
 * Interrupts masked rather than preemption disabled, and nothing that can
 * block runs inside: tree_lookup_child() and the tree_children_* walks are
 * pointer arithmetic, inode_free() reaches the filesystem's destroy_inode()
 * and is called after the unlink, and the ".." case hands off to
 * __vfs_get_parent(), which asks the mount table and takes ITS lock -- so it
 * is answered before this one is taken and the two never nest.
 *
 * What this does NOT do is make a lookup and the use of its result one
 * operation. A path resolved on one core can be unlinked by another before
 * the caller does anything with it; that is a reference-counting question and
 * it is not this. */
static spinlock_t vfs_tree_lock = SPIN_STATIC_UNLOCKED;

/* Times a walk had to wait: two cores inside the namespace tree at once.
 * Zero after a run says the run proved nothing about concurrency here. */
unsigned long vfs_tree_contended;

static ipl_t vfs_tree_enter(void) {
	ipl_t ipl = ipl_save();

	if (!__spin_trylock(&vfs_tree_lock)) {
		atomic_add_fetch(&vfs_tree_contended, 1, __ATOMIC_RELAXED);
		__spin_lock(&vfs_tree_lock);
	}

	return ipl;
}

static void vfs_tree_leave(ipl_t ipl) {
	__spin_unlock(&vfs_tree_lock);
	ipl_restore(ipl);
}

static int vfs_lookup_cmp(struct tree_link *link, void *data) {
	struct lookup_tuple *lookup = data;
	struct inode *node = tree_element(link, struct inode, tree_link);
	const char *name = inode_name(node);
	return !(strncmp(name, lookup->name, lookup->len) || name[lookup->len]);
}

/*================================================================================
 ====================== Functions that are working with path ======================
 ==================================================================================*/

int vfs_get_pathbynode_tilln(struct path *node, struct path *parent, char *path,
		size_t plen) {
	char *p;
	size_t ll = plen - 1;

	if (plen <= 0) {
		return -ERANGE;
	}

	p = path + ll;
	*p = '\0';

	while (node->node != parent->node && node->node != NULL) {
		size_t nnlen;
		char *node_name;

		if_root_follow_up(node);

		node_name = inode_name(node->node);
		/* Some root nodes are marked with "/" name which leads
		 * to pathes like "///dev", so we want to avoid it  */
		if (strcmp(node_name, "/") && node_name[0] != '\0') {
			nnlen = strlen(node_name);

			if (nnlen + 1 > ll) {
				return -ERANGE;
			}
#ifdef __GNUC__
#if __GNUC__ > 7
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
#endif
			p = strncpy(p - nnlen, node_name, nnlen);
#ifdef __GNUC__
#if __GNUC__ > 7
#pragma GCC diagnostic pop
#endif
#endif
			*--p = '/';
			ll -= nnlen + 1;
		}

		vfs_get_parent(node, node);
	}

	memmove(path, p, plen - ll);

	if (node->node != parent->node) {
		return 1;
	}

	return 0;
}

void vfs_lookup_childn(struct path *parent, const char *name, size_t len,
		struct path *child) {
	assert(parent);
	assert(child);

	if_mounted_follow_down(parent);
	*child = *parent;

	child->node = vfs_subtree_lookup_childn(parent->node, name, len);
}

static void __vfs_lookup_existing(const struct path *parent,
		const char *str_path, const char **p_end_existent, struct path *path) {
	struct inode *node;
	size_t len = 0;

	assert(parent && str_path);
	*path = *parent;

	while ((str_path = path_next(str_path, &len))) {
		if_mounted_follow_down(path);
		if (-ERR_CHILD_MOUNTED
				!= __vfs_subtree_lookup_existing(path->node, str_path,
						p_end_existent, &node)) {
			path->node = node;
			break;
		}

		str_path = *p_end_existent;
		path->node = node;
	}

	return;
}

void vfs_lookup_child(struct path *parent, const char *name, struct path *child) {
	vfs_lookup_childn(parent, name, strlen(name), child);
}

int vfs_lookup(const char *str_path, struct path *path) {
	struct path parent;

	vfs_get_root_path(&parent);

	if_mounted_follow_down(&parent);

	__vfs_lookup_existing(&parent, str_path, &str_path, path);

	if (path_next(str_path, NULL)) {
		/* Have unresolved fragments in path. */
		return -1;
	}

	return 0;
}

void vfs_create_child(struct path *parent, const char *name, mode_t mode,
		struct path *child) {
	assert(parent);
	assertf(mode & S_IFMT, "Must provide a type of node, see S_IFXXX");

	if_mounted_follow_down(parent);

	child->node = vfs_subtree_create_child(parent->node, name, mode);
}

static int __vfs_create(struct path *parent, const char *path, mode_t mode,
		int intermediate, struct path *child) {
	assert(parent);
	assert(child);

	*child = *parent;

	__vfs_lookup_existing(child, path, &path, child);
	if_mounted_follow_down(child);

	child->node = __vfs_subtree_create(child->node, path, mode, intermediate);

	return child->node ? 0 : -1;
}

int vfs_create(struct path *parent, const char *path, mode_t mode,
		struct path *child) {
	return __vfs_create(parent, path, mode, 0, child);
}

int vfs_create_intermediate(struct path *parent, const char *path, mode_t mode,
		struct path *child) {
	return __vfs_create(parent, path, mode, 1, child);
}

int vfs_get_child_next(struct path *parent_path, struct inode *child_prev, struct path *child_next) {
	*child_next = *parent_path;
	if_mounted_follow_down(child_next);

	child_next->node = vfs_subtree_get_child_next(child_next->node, child_prev);

	return child_next->node ? 0 : -1;
}

void vfs_get_parent(struct path *child_path, struct path *parent_path) {
	*parent_path = *child_path;
	if_root_follow_up(parent_path);
	parent_path->node = __vfs_get_parent(parent_path->node);
	parent_path->mnt_desc = mount_desc_by_inode(parent_path->node);
}

void vfs_get_root_path(struct path *path) {
	path->node = vfs_get_root();
	path->mnt_desc = mount_table();
}

void vfs_get_leaf_path(struct path *path) {
	char *leaf_name;

	if ((NULL == (leaf_name = getenv("PWD")))
			|| (0 != vfs_lookup(leaf_name, path))) {
		vfs_get_root_path(path);
	}
}

void if_mounted_follow_down(struct path *path) {
	if (path->node->mounted) {
		path->mnt_desc = mount_table_get_child(path->mnt_desc, path->node);

		assert(path->mnt_desc);

		path->node = path->mnt_desc->mnt_root;
	}
}

/**
 * @brief Check if given node is a root of some nested FS, and if so,
 * then replace it with the mountpoint node of parent FS
 */
void if_root_follow_up(struct path *path) {
	assert(path);
	assert(path->node);

	if (path->mnt_desc != NULL) {
		if (path->node == path->mnt_desc->mnt_root) {
			path->node = path->mnt_desc->mnt_point;
			path->mnt_desc = path->mnt_desc->mnt_parent;
		}
	}
}

/*================================================================================
 ====================== Functions that are working with node ======================
 ==================================================================================*/

static struct inode *__vfs_get_parent(struct inode *child) {
	struct mount_descriptor *mdesc = mount_desc_by_inode(child);

	/* If child is root of some FS then we return
	 * parent of the mount point from the parent FS */
	if (mdesc && mdesc->mnt_root == child) {
		return __vfs_get_parent(mdesc->mnt_point);
	}

	return tree_element(child->tree_link.par, struct inode, tree_link);
}

static int __vfs_subtree_lookup_existing(struct inode *parent,
		const char *str_path, const char **p_end_existent,
		struct inode **child_ptr) {
	size_t len = 0;
	int res = 0;
	struct inode *child = *child_ptr;

	assert(parent && str_path);

	while ((str_path = path_next(str_path, &len))) {
		child = vfs_subtree_lookup_childn(parent, str_path, len);
		if (!child) {
			res = -ERR_CHILD_NOT_FOUND;
			goto out;
		}

		str_path += len;
		parent = child;

		if (child->mounted) {
			res = -ERR_CHILD_MOUNTED;
			goto out;
		}
	}

	out: if (p_end_existent) {
		*p_end_existent = str_path;
	}

	*child_ptr = parent;
	return res;
}

static struct inode *__vfs_subtree_create(struct inode *parent, const char *path,
		mode_t mode, int intermediate) {
	struct inode *child = NULL;
	size_t len;
	struct inode **tmp_parent;

	assert(parent);

	tmp_parent = &parent;

	__vfs_subtree_lookup_existing(*tmp_parent, path, &path, tmp_parent);

	path = path_next(path, &len);

	/* Here path points to the first non-existent fragment, if any. */

	if (intermediate) {
		const char *next_path;
		size_t next_len;

		if (!path) {
			/* Node already exist, set mode. */
			//XXX wtf?? parent->mode = mode;
			return *tmp_parent;
		}

		while ((next_path = path_next(path + len, &next_len))) {
			child = __vfs_subtree_create_child(*tmp_parent, path, len, S_IFDIR);

			if (!child) {
				return NULL;
			}

			tmp_parent = &child;
			path = next_path;
			len = next_len;
		}
	} else if (!path || path_next(path + len, NULL)) {
		/* Node already exists or missing intermediate node. */
		return NULL;
	}

	return vfs_subtree_create_child(*tmp_parent, path, mode);
}

static struct inode *__vfs_subtree_create_child(struct inode *parent, const char *name,
		size_t len, mode_t mode) {
	struct inode *child = NULL;

	assert(parent);

	child = inode_alloc(parent->i_sb);
	if (child) {
		inode_name_set(child, name);
		child->i_mode = mode;
		child->i_dentry->flags = mode;
		child->i_owner_id = getuid();
		child->i_group_id = getgid();

		child->i_sb = parent->i_sb;

		vfs_add_leaf(child, parent);
	}

	return child;
}

struct inode *vfs_subtree_create_child(struct inode *parent, const char *name,
		mode_t mode) {
	return __vfs_subtree_create_child(parent, name, strlen(name), mode);
}

struct inode *vfs_subtree_lookup_childn(struct inode *parent, const char *name,
		size_t len) {
	struct lookup_tuple lookup = { .name = name, .len = len };
	struct tree_link *tlink;
	struct inode *ret;
	ipl_t ipl;

	assert(parent);

	/* Answered before the lock is taken: it asks the mount table, which has
	 * a lock of its own, and one at a time is how that stays true. */
	if (path_is_double_dot(name))
		return (ret = __vfs_get_parent(parent)) ? ret : parent;

	ipl = vfs_tree_enter();
	if (parent->i_dying) {
		/* As above -- a name under an inode that has left the tree resolves to
		 * nothing, rather than to whatever its child list looks like halfway
		 * through being taken apart. */
		vfs_walk_dying++;
		tlink = NULL;
	}
	else {
		tlink = tree_lookup_child(&(parent->tree_link), vfs_lookup_cmp,
		    &lookup);
	}
	vfs_tree_leave(ipl);

	return tlink ? tree_element(tlink, struct inode, tree_link) : NULL;
}

struct inode *vfs_subtree_lookup_child(struct inode *parent, const char *name) {
	return vfs_subtree_lookup_childn(parent, name, strlen(name));
}

struct inode *vfs_subtree_lookup(struct inode *parent, const char *str_path) {
	struct inode *node;

	assert(parent);

	__vfs_subtree_lookup_existing(parent, str_path, &str_path, &node);

	if (path_next(str_path, NULL)) {
		/* Have unresolved fragments in path. */
		return NULL;
	}

	return node;
}

struct inode *vfs_subtree_get_child_next(struct inode *parent, struct inode *prev_child) {
	struct tree_link *chld_link;
	struct inode *ret = NULL;
	ipl_t ipl;

	assert(parent);

	ipl = vfs_tree_enter();

	if (parent->i_dying) {
		/* It has no children any more; whatever it had has been unlinked with
		 * it. Walking its list means walking a list somebody is dismantling.
		 * */
		vfs_walk_dying++;
		goto out;
	}

	if (!prev_child) {
		chld_link = tree_children_begin(&parent->tree_link);
	}
	else {
		if (prev_child->i_dying) {
			/* The cursor has been unlinked, so its neighbours are whatever the
			 * unlink left behind. The walk has lost its place; ending it is
			 * the answer that touches nothing. A caller that holds a reference
			 * (readdir does) still has a valid pointer to read this from --
			 * one that does not is the open window this counter is here to
			 * size. */
			vfs_walk_dying++;
			goto out;
		}
		chld_link = tree_children_next(&prev_child->tree_link);
		if (tree_children_end(&parent->tree_link) == chld_link) {
			goto out;
		}
	}

	ret = tree_element(chld_link, struct inode, tree_link);

out:
	vfs_tree_leave(ipl);

	return ret;
}

struct inode *vfs_subtree_create(struct inode *parent, const char *path,
		mode_t mode) {
	return __vfs_subtree_create(parent, path, mode, 0);
}

struct inode *vfs_subtree_create_intermediate(struct inode *parent,
		const char *path, mode_t mode) {
	return __vfs_subtree_create(parent, path, mode, 1);
}

struct inode *vfs_get_leaf(void) {
	struct path leaf;

	vfs_get_leaf_path(&leaf);

	return leaf.node;
}

/* Reference counting for inodes held across a walk; see fs/oldfs/inode.h. */
unsigned long inode_free_deferred;
unsigned long inode_ref_refused;

/* Walks that arrived at an inode after it left the tree.
 * The caller of a lookup holds a raw pointer across the tree lock, and umount
 * takes every inode of a volume out from under it. An inode still allocated
 * can say so; one already freed cannot, which is why this is a measurement
 * and not a guarantee. */
unsigned long vfs_walk_dying;

int inode_ref(struct inode *node) {
	ipl_t ipl;
	int live;

	if (node == NULL) {
		return 0;
	}

	ipl = vfs_tree_enter();
	live = !node->i_dying;
	if (live) {
		node->i_ref++;
	}
	else {
		inode_ref_refused++;
	}
	vfs_tree_leave(ipl);

	return live;
}

void inode_unref(struct inode *node) {
	ipl_t ipl;
	int last;

	if (node == NULL) {
		return;
	}

	ipl = vfs_tree_enter();
	if (node->i_ref <= 0) {
		vfs_tree_leave(ipl);
		printk("inode_unref(%p): i_ref %d, i_dying %d, sb %p, name '%s', "
		       "from %p -- %s\n",
		    node, node->i_ref, node->i_dying, node->i_sb, node->name,
		    __builtin_return_address(0),
		    node->i_ref == -1 ? "ALREADY FREED" : "NEVER REFERENCED");
		return;
	}
	last = (0 == --node->i_ref) && node->i_dying;
	vfs_tree_leave(ipl);

	/* Outside the lock, for the same reason vfs_del_leaf() frees outside it:
	 * inode_free() reaches the filesystem's destroy_inode(), and on this
	 * board that is a FAT driver behind a mutex. */
	if (last) {
		inode_free(node);
	}
}

void inode_release(struct inode *node) {
	ipl_t ipl;
	int held;

	if (node == NULL) {
		return;
	}

	ipl = vfs_tree_enter();
	/* From here no new reference may be taken on it. Whether it can be freed
	 * depends on who is still standing on it, and this is the only place where
	 * that question and the giving-up are one operation. */
	node->i_dying = 1;
	held = node->i_ref > 0;
	if (held) {
		inode_free_deferred++;
	}
	vfs_tree_leave(ipl);

	/* Deferred inodes keep their filesystem: a descriptor may still be
	 * reading through one, and handing the driver's private data back early
	 * puts that data in the next file's hands. Giving it back is the UMOUNT
	 * path's job, where the superblock itself is going -- see kumount(). */
	if (!held) {
		inode_free(node);
	}
}

int vfs_del_leaf(struct inode *node) {
	ipl_t ipl;
	int rc;
	int held = 0;

	assert(node);

	ipl = vfs_tree_enter();
	rc = tree_unlink_link(&(node->tree_link));
	if (rc) {
		/* Same decision as inode_release(), taken here so that it and the
		 * unlink are one operation. */
		node->i_dying = 1;
		held = node->i_ref > 0;
		if (held) {
			inode_free_deferred++;
		}
	}
	vfs_tree_leave(ipl);

	/* Outside the lock. inode_free() hands the inode back to the filesystem's
	 * destroy_inode(), which for FAT is a driver behind a mutex. It
	 * is already unlinked, so no walk can reach it while that happens. */
	if (rc && !held) {
		inode_free(node);
	}
	return rc;
}

static struct inode *vfs_init_root(struct inode *node) {
	inode_name_set(node, "/");
	node->i_mode = S_IFDIR | ROOT_MODE;
	node->i_dentry->flags = S_IFDIR | ROOT_MODE;

	return node;
}

static struct inode *root_node;

struct inode *vfs_get_root(void) {
	if (!root_node) {
		struct inode *node;
		node = inode_alloc(NULL);
		assert(node);
		root_node = vfs_init_root(node);
	}

	return root_node;
}

struct inode *vfs_set_root(struct inode *node) {
	/* FIXME */
	//assert(!root_node);
	if (root_node) {
		inode_free(root_node);
	}

	root_node = vfs_init_root(node);

	return root_node;
}

int vfs_add_leaf(struct inode *child, struct inode *parent) {
	ipl_t ipl;

	ipl = vfs_tree_enter();
	tree_add_link(&(parent->tree_link), &(child->tree_link));
	vfs_tree_leave(ipl);

	return 0;
}

struct inode *vfs_subtree_get_parent(struct inode *node) {
	return __vfs_get_parent(node);
}

int vfs_get_relative_path(struct inode *node, char *path, size_t path_len) {
	struct inode *prev = NULL;
	char *p;
	size_t ll = path_len - 1;

	assert(path_len > 0);

	p = path + ll;
	*p = '\0';

	while (node != prev && node != NULL) {
		size_t nnlen;

		nnlen = strlen(inode_name(node));

		if (nnlen + 1 > ll) {
			return -ERANGE;
		}
#ifdef __GNUC__
#if __GNUC__ > 7
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif
#endif
		p = strncpy(p - nnlen, inode_name(node), nnlen);
#ifdef __GNUC__
#if __GNUC__ > 7
#pragma GCC diagnostic pop
#endif
#endif
		*--p = '/';
		ll -= nnlen + 1;

		prev = node;
		node = __vfs_get_parent(node);

		if (prev && node && prev->i_sb != node->i_sb) {
			break;
		}
	}

	assert(path_len >= ll);

	memmove(path, p, path_len - ll);

	if (node != prev) {
		return 1;
	}

	return 0;
}

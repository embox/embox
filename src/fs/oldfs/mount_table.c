/**
 * @file
 *
 * @date May 17, 2013
 * @author: Anton Bondarev
 * @author: Vita Loginova
 */

#include <string.h>
#include <errno.h>


#include <hal/ipl.h>
#include <kernel/spinlock.h>
#include <lib/libds/dlist.h>
#include <mem/misc/pool.h>

#include <fs/inode.h>
#include <fs/vfs.h>
#include <fs/mount.h>

#include <mem/misc/pool.h>
#include <util/atomic_rmw.h>

POOL_DEF(mount_desc_pool, struct mount_descriptor, OPTION_GET(NUMBER,mount_desc_quantity));

static struct mount_descriptor *mnt_root = NULL;

/* The mount tree is walked by every path lookup and rewritten by mount/umount.
 * This spinlock serialises the two. Interrupts masked; nothing in the locked
 * sections blocks. pool_alloc/pool_free and vfs_del_leaf() run outside the
 * lock because they take their own locks. */
static spinlock_t mount_table_lock = SPIN_STATIC_UNLOCKED;

/* Times a caller had to wait for the lock: two cores inside the mount tree at
 * once. Zero after a run means the run proved nothing about concurrency here. */
unsigned long mount_table_contended;

static ipl_t mount_table_enter(void) {
	ipl_t ipl = ipl_save();

	if (!__spin_trylock(&mount_table_lock)) {
		atomic_add_fetch(&mount_table_contended, 1, __ATOMIC_RELAXED);
		__spin_lock(&mount_table_lock);
	}

	return ipl;
}

static void mount_table_leave(ipl_t ipl) {
	__spin_unlock(&mount_table_lock);
	ipl_restore(ipl);
}

struct mount_descriptor *mount_table_get_child(struct mount_descriptor *parent, struct inode *mnt_point) {
	struct mount_descriptor *desc;
	struct mount_descriptor *found = NULL;
	ipl_t ipl;

	/* A path with no mount descriptor has no children either. */
	if (parent == NULL) {
		return NULL;
	}

	ipl = mount_table_enter();

	if (parent->mnt_point == mnt_point) {
		/* This used to assert that a descriptor mounted at the inode being
		 * asked about must be the root one -- true of a tree nobody is
		 * changing, and not an invariant on four cores. The answer does not
		 * depend on it: whoever is mounted at this inode is the child being
		 * asked for. */
		found = parent;
	}
	else {
		dlist_foreach_entry(desc, &parent->mnt_mounts, mnt_child) {
			if (desc->mnt_point == mnt_point) {
				found = desc;
				break;
			}
		}
	}

	mount_table_leave(ipl);

	return found;
}

struct mount_descriptor *mount_table_add(struct path *mnt_point_path,
		struct mount_descriptor *mnt_desc,
		struct inode *root, const char *dev) {
	struct mount_descriptor *mdesc;
	ipl_t ipl;

	assert(mnt_desc != NULL ||
			(mnt_desc == NULL && mnt_root == NULL));

	if (mnt_point_path->node == NULL) {
		return NULL;
	}

	if (mnt_root != NULL && mnt_point_path->node ==
			mnt_desc->mnt_root) {
		return NULL;
	}

	/* Allocated before the lock is taken, because pool_alloc() takes the
	 * pool's own. Nothing can see this descriptor until it is linked. */
	if (NULL == (mdesc = pool_alloc(&mount_desc_pool))) {
		return NULL;
	}

	mdesc->mnt_point = mnt_point_path->node;
	mdesc->mnt_root = root;

	/* XXX mount root should preserve mode, uid/gid */
	root->i_mode = mnt_point_path->node->i_mode;
	root->i_owner_id = mnt_point_path->node->i_owner_id;
	root->i_group_id = mnt_point_path->node->i_group_id;

	dlist_init(&mdesc->mnt_mounts);
	dlist_head_init(&mdesc->mnt_child);

	if (dev != NULL) {
		strncpy(mdesc->mnt_dev, dev, MOUNT_DESC_STRINFO_LEN);
	} else {
		mdesc->mnt_dev[0] = '\0';
	}

	mdesc->mnt_dev[MOUNT_DESC_STRINFO_LEN - 1] = '\0';

	ipl = mount_table_enter();

	/* The two checks above read mnt_root without the lock, which was fine when
	 * there was one core. Re-ask now: another mount may have become the root
	 * while this one was allocating. */
	if ((mnt_desc == NULL) && (mnt_root != NULL)) {
		mount_table_leave(ipl);
		pool_free(&mount_desc_pool, mdesc);
		return NULL;
	}

	/* Read without the lock by if_mounted_follow_down(). A stale zero there
	 * means one lookup does not follow a mount that is being made; a stale
	 * non-zero means it calls mount_table_get_child(), which takes the lock
	 * and answers NULL. Both are transient wrong answers, neither is a walk
	 * over a list somebody is unlinking from, and that is the difference this
	 * lock exists to make. */
	mdesc->mnt_point->mounted++;

	if (mnt_desc == NULL) {
		mdesc->mnt_parent = mdesc;
		mnt_root = mdesc;
	} else {
		mdesc->mnt_parent = mnt_desc;
		dlist_add_next(&mdesc->mnt_child, &mnt_desc->mnt_mounts);
	}

	mount_table_leave(ipl);

	return mdesc;
}

int mount_table_del(struct mount_descriptor *mdesc) {
	struct inode *mnt_root_node;
	ipl_t ipl;

	if(mdesc == NULL) {
		return -EINVAL;
	}

	ipl = mount_table_enter();

	if (mnt_root == mdesc) {
		mnt_root = NULL;
	}

	mdesc->mnt_point->mounted--;
	mnt_root_node = mdesc->mnt_root;

	dlist_del(&mdesc->mnt_mounts);
	dlist_del(&mdesc->mnt_child);

	mount_table_leave(ipl);

	/* Outside the lock, because vfs_del_leaf() reaches the filesystem's
	 * destroy_inode() and that takes a mutex. The descriptor is already
	 * unlinked, so no lookup can arrive at it while this runs -- the same
	 * reasoning as idesc_table_del() closing after it has cleared the slot. */
	vfs_del_leaf(mnt_root_node);

	pool_free(&mount_desc_pool, mdesc);

	return ENOERR;
}

struct mount_descriptor *mount_table(void) {
	struct mount_descriptor *root;
	ipl_t ipl;

	ipl = mount_table_enter();
	root = mnt_root;
	mount_table_leave(ipl);

	return root;
}

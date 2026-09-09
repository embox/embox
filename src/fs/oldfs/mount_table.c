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

/* Serialises path lookups against mount/umount. IRQs masked; nothing inside
 * blocks. pool_alloc/pool_free and vfs_del_leaf() run outside the lock
 * because they take their own locks. Without SMP masked IRQs alone do. */
#if defined(SMP)
static spinlock_t mount_table_lock = SPIN_STATIC_UNLOCKED;
#endif

/* Times a caller waited for the lock. Zero after a test run means no
 * contention was exercised. */
unsigned long mount_table_contended;

static ipl_t mount_table_enter(void) {
	ipl_t ipl = ipl_save();

#if defined(SMP)
	if (!__spin_trylock(&mount_table_lock)) {
		atomic_rmw_add_fetch(&mount_table_contended, 1, __ATOMIC_RELAXED);
		__spin_lock(&mount_table_lock);
	}
#endif

	return ipl;
}

static void mount_table_leave(ipl_t ipl) {
#if defined(SMP)
	__spin_unlock(&mount_table_lock);
#endif
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
		/* Whatever is mounted at this inode is the child, root or not. */
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

	/* Before the lock: pool_alloc() takes the pool's own lock. */
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

	/* The checks above read mnt_root unlocked; another mount may have become
	 * the root meanwhile. */
	if ((mnt_desc == NULL) && (mnt_root != NULL)) {
		mount_table_leave(ipl);
		pool_free(&mount_desc_pool, mdesc);
		return NULL;
	}

	/* if_mounted_follow_down() reads this without the lock. A stale value only
	 * makes one lookup miss a new mount or re-check it under the lock; it never
	 * leads to an unlocked walk of the mount lists. */
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

	/* Outside the lock: vfs_del_leaf() reaches destroy_inode(), which takes a
	 * mutex. The descriptor is already unlinked, so no lookup can reach it. */
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

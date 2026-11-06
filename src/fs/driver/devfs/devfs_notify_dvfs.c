/**
 * @file devfs_notify_dvfs.c
 * @brief A device that goes away takes its name in /dev with it.
 */
#include <stddef.h>

#include <drivers/dev_module.h>
#include <fs/dentry.h>
#include <fs/dvfs.h>
#include <fs/inode.h>
#include <lib/libds/dlist.h>

/* Every dentry that exists, linked by d_lnk. */
extern struct dlist_head dentry_dlist;

void devfs_notify_new_module(struct dev_module *devmod) {
	/* Nothing to do: the next lookup misses the tree and devfs_lookup() finds
	 * the device. */
}

/* The one dentry naming this device module, or NULL. Under the tree lock. */
static struct dentry *dentry_of_module(struct dev_module *devmod) {
	struct dentry *d;

	dlist_foreach_entry(d, &dentry_dlist, d_lnk) {
		if (d->d_inode != NULL && inode_priv(d->d_inode) == devmod) {
			return d;
		}
	}
	return NULL;
}

void devfs_notify_del_module(struct dev_module *devmod) {
	struct dentry *d;

	if (devmod == NULL) {
		return;
	}

	dvfs_lock();

	/* Rescanned, not iterated: freeing a dentry may free its parent. Each turn
	 * frees a dentry or clears its match, so the loop ends. */
	while ((d = dentry_of_module(devmod)) != NULL) {
		if (d->usage_count > 0) {
			/* Held (an open descriptor, a mount): the name goes now and the
			 * dentry lives until released, as with unlink(). */
			inode_priv_set(d->d_inode, NULL);
			dentry_unlink_dying(d);
		}
		else if (0 != dvfs_destroy_dentry(d)) {
			/* Refused with usage_count 0 (the root, or the pool disagrees):
			 * clearing the pointer is all that is left. */
			inode_priv_set(d->d_inode, NULL);
		}
	}

	dvfs_unlock();
}

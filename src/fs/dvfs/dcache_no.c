/**
 * @file
 * @brief Handle cache lookup just by iterating vfs tree
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-06-09
 */

#include <string.h>
#include <fs/dvfs.h>

struct dentry *dvfs_cache_lookup(const char *path, struct dentry *base) {
	return NULL;
}

struct dentry *dvfs_cache_get(char *path, struct lookup *lookup) {
	struct dentry *d;
	struct dlist_head *l;
	dlist_foreach(l, &lookup->parent->children) {
		if (l == &lookup->parent->children)
			continue;
		d = mcast_out(l, struct dentry, children_lnk);

		if (d != lookup->item && !strcmp(d->name, lookup->item->name)) {
			dvfs_destroy_dentry(lookup->item);
			return d;
		}
	}

	return NULL;
}

int dvfs_cache_del(struct dentry *dentry) {
	return 0;
}

int dvfs_cache_add(struct dentry *dentry) {
	return 0;
}

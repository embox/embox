/**
 * @file
 * @brief Handle cache lookup just by iterating vfs tree
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-06-09
 */

#include <fs/dvfs.h>

struct dentry *dvfs_cache_lookup(const char *path, struct dentry *base) {
	return NULL;
}

struct dentry *dvfs_cache_get(char *path) {
	return NULL;
}

int dvfs_cache_del(struct dentry *dentry) {
	return 0;
}

int dvfs_cache_add(struct dentry *dentry) {
	return 0;
}

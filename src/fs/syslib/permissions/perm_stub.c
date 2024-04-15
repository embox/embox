/**
 * @file
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-09-28
 */

#include <fs/perm.h>

/* This is stubs to make system compile w/o old vfs */

int fs_perm_mask(struct inode *node) {
	return 0777;
}

int fs_perm_check(struct inode *node, int fd_flags) {
	return 0;
}

int fs_perm_lookup(const char *path, const char **pathlast,
		struct path *nodelast) {
	return 0;
}

int fs_perm_lookup_relative(const char *path, const char **pathlast,
		struct path *nodelast) {
	return 0;
}

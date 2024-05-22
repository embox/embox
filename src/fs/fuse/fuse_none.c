/**
 * @file
 *
 * @date 23.11.2015
 * @author: Anton Bondarev
 */
#include <stddef.h>
#include <errno.h>

#include <fs/fuse_module.h>

struct fuse_module *fuse_module_lookup(const char *fuse_type) {
	return NULL;
}

int fuse_module_mount(struct fuse_module *fm,
		const char *dev, const char *dest) {
	return -ENOSUPP;
}

int fuse_module_umount(struct fuse_module *fm) {
	return -ENOSUPP;
}


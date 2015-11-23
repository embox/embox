/**
 * @file
 *
 * @data 23.11.2015
 * @author: Anton Bondarev
 */
#include <stddef.h>
#include <errno.h>
#include <string.h>

#include <util/array.h>

#include <fs/fuse_module.h>

ARRAY_SPREAD_DEF(struct fuse_module *, fuse_module_repo);

struct fuse_module *fuse_module_lookup(char *fuse_type) {
	struct fuse_module *fm;
	array_spread_foreach(fm, fuse_module_repo) {
		if (0 == strcmp(fuse_type, fm->fuse_module_name)) {
			return fm;
		}
	}
	return NULL;
}

int fuse_module_mount(struct fuse_module *fm, char *dev, char *dest) {
	return -ENOSUPP;
}

int fuse_module_umount(struct fuse_module *fm) {
	return -ENOSUPP;
}


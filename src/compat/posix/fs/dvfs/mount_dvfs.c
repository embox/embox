/**
 * @brief
 *
 * @date 15.03.24
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <posix_errno.h>
#include <sys/mount.h>

#include <fs/dvfs.h>
#include <fs/fuse_module.h>

int mount(const char *source, const char *target, const char *filesystemtype,
    unsigned long mountflags, const void *data) {
	struct fuse_module *fm;
	int err;

	fm = fuse_module_lookup(filesystemtype);
	if (fm) {
		return fuse_module_mount(fm, source, target);
	}

	err = dvfs_mount(source, target, filesystemtype, mountflags);
	if (err) {
		return SET_ERRNO(-err);
	}

	return 0;
}

int umount(const char *target) {
	int err;

	err = dvfs_umount_path(target);
	if (err) {
		return SET_ERRNO(-err);
	}

	return 0;
}

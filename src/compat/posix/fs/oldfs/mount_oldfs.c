/**
 * @brief
 *
 * @date 15.03.24
 * @author Aleksey Zhmulin
 */

#include <sys/mount.h>

#include <fs/kfsop.h>

int mount(const char *source, const char *target, const char *filesystemtype,
    unsigned long mountflags, const void *data) {
	return kmount(source, target, filesystemtype);
}

int umount(const char *target) {
	return kumount(target);
}

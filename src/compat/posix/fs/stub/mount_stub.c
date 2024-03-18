/**
 * @brief
 *
 * @date 15.03.24
 * @author Aleksey Zhmulin
 */

#include <errno.h>

#include <util/log.h>

int mount(const char *source, const char *target, const char *filesystemtype,
    unsigned long mountflags, const void *data) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

int umount(const char *target) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

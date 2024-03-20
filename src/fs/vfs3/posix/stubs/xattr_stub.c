/**
 * @brief
 *
 * @date 12.02.24
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <stddef.h>
#include <sys/xattr.h>

#include <util/log.h>

int getxattr(const char *path, const char *name, char *value, size_t size) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

int setxattr(const char *path, const char *name, const char *value, size_t size,
    int flags) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

int listxattr(const char *path, char *list, size_t size) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

int fsetxattr(int fd, const char *name, const char *value, size_t size,
    int flags) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

int fgetxattr(int fd, const char *name, void *value, size_t size) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

int flistxattr(int fd, char *list, size_t size) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

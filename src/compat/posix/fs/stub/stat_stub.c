/**
 * @brief
 *
 * @date 12.02.24
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <sys/stat.h>

#include <util/log.h>

int stat(const char *path, struct stat *buf) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

int lstat(const char *path, struct stat *buf) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

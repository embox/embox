/**
 * @brief
 *
 * @date 12.02.24
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <fcntl.h>

#include <util/log.h>

int openat(int dirfd, const char *path, int __oflag, ...) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

/**
 * @brief
 *
 * @date 03.12.25
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <fcntl.h>

#include <util/log.h>

int openat(int fildes, const char *path, int __oflag, ...) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

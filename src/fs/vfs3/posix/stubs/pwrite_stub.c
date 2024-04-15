/**
 * @brief
 *
 * @date 12.02.24
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <stddef.h>
#include <sys/types.h>
#include <unistd.h>

#include <util/log.h>

ssize_t pwrite(int fd, const void *buf, size_t nbyte, off_t offset) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

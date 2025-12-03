/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 03.12.25
 */

#include <errno.h>
#include <unistd.h>

#include <util/log.h>

ssize_t readlink(const char *path, char *buf, size_t bufsize) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

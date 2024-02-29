/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 10.11.23
 */

#include <errno.h>
#include <utime.h>

#include <util/log.h>

int utime(const char *path, const struct utimbuf *times) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

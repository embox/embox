/**
 * @brief
 *
 * @date 12.02.24
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <sys/file.h>

#include <util/log.h>

int flock(int fd, int operation) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

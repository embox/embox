/**
 * @brief
 *
 * @date 12.02.24
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <unistd.h>

#include <util/log.h>

int fsync(int fd) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

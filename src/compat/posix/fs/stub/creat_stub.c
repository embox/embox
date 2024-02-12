/**
 * @brief
 *
 * @date 12.02.24
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>

#include <util/log.h>

int creat(const char *pathname, mode_t mode) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

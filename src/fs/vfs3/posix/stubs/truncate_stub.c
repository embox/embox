/**
 * @brief
 *
 * @date 12.02.24
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <util/log.h>

int truncate(const char *path, off_t length) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

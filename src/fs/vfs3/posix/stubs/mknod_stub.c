/**
 * @brief
 *
 * @date 12.02.24
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <util/log.h>

int mknod(const char *pathname, mode_t mode, dev_t dev) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

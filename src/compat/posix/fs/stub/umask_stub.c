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

mode_t umask(mode_t mode) {
	log_warning(">>> %s", __func__);
	return mode;
}

mode_t umask_modify(mode_t newmode) {
	log_warning(">>> %s", __func__);
	return newmode;
}

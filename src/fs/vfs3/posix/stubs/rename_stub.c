/**
 * @brief
 *
 * @date 12.02.24
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <stdio.h>

#include <util/log.h>

int rename(const char *src_name, const char *dst_name) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

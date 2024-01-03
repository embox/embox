/**
 * @brief
 *
 * @date 12.02.24
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <stddef.h>
#include <unistd.h>

#include <util/log.h>

char *realpath(const char *file_name, char *resolved_name) {
	log_warning(">>> %s", __func__);
	return NULL;
}

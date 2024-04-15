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

char *getcwd(char *buff, size_t size) {
	log_warning(">>> %s", __func__);
	return NULL;
}

/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 31.03.13
 */

#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <linux/limits.h>
#include <unistd.h>
#include <util/array.h>

char current_dir[PATH_MAX] = "/"; /* FIXME */

char * getcwd(char *buff, size_t size) {
	if ((buff == NULL) || (size == 0)) {
		SET_ERRNO(EINVAL);
		return NULL;
	}

	if (strlen(&current_dir[0]) + 1 >= size) {
		SET_ERRNO(ERANGE);
		return NULL;
	}

	strcpy(buff, &current_dir[0]);

	return buff;
}

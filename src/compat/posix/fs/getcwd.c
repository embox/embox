/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 31.03.13
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char * getcwd(char *buff, size_t size) {
	const char *dir;

	if ((buff == NULL) || (size == 0)) {
		SET_ERRNO(EINVAL);
		return NULL;
	}

	dir = getenv("PWD");
	if (dir == NULL) { /* TODO remove this */
		setenv("PWD", "/", 0);
		dir = "/";
	}
	assert(dir != NULL); /* $PWD must be set */

	if (strlen(dir) + 1 >= size) {
		SET_ERRNO(ERANGE);
		return NULL;
	}

	strncpy(buff, dir, size);

	return buff;
}

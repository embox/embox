/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 31.03.13
 */

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

int chdir(const char *path) {
	if ((path == NULL) || (*path == '\0')
			|| (*path != '/')) {
		SET_ERRNO(ENOENT);
		return -1;
	}

	if (strlen(path) >= PATH_MAX - 1) {
		SET_ERRNO(ENAMETOOLONG);
		return -1;
	}

	if (-1 == setenv("PWD", path, 1)) {
		assert(errno == ENOMEM); /* it is the only possible error */
		SET_ERRNO(ENAMETOOLONG);
		return -1;
	}

	return 0;
}

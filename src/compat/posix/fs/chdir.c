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

extern char current_dir[PATH_MAX];

int chdir(const char *path) {
	if ((path == NULL) || (*path == '\0')
			|| (*path != '/')) {
		SET_ERRNO(ENOENT);
		return -1;
	}

	if (strlen(path) + 1 >= ARRAY_SIZE(current_dir)) {
		SET_ERRNO(ENAMETOOLONG);
		return -1;
	}

	strcpy(&current_dir[0], path);

	return 0;
}

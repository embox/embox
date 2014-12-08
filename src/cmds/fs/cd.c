/**
 * @file
 * @brief
 *
 * @date 01.04.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <framework/mod/options.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <util/array.h>

#define MODOPS_HOME_DIR OPTION_STRING_GET(home_dir)

int main(int argc, char *argv[]) {
	char *dir, buff[PATH_MAX];

	if (argc == 1) {
		dir = MODOPS_HOME_DIR;
	}
	else if (argv[1][0] == '/') {
		dir = argv[1];
	}
	else {
		dir = getcwd(&buff[0], ARRAY_SIZE(buff));
		if (dir == NULL) {
			return -errno;
		}
		if (0 == strcmp(dir, "/")) {
			snprintf(&buff[0], ARRAY_SIZE(buff), "/%s", argv[1]);
		}
		else {
			snprintf(&buff[0], ARRAY_SIZE(buff), "%s/%s", dir, argv[1]);
		}
	}

	if (-1 == chdir(dir)) {
		return -errno;
	}

	return 0;
}

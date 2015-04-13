/**
 * @file
 * @brief
 *
 * @date 28.02.13
 * @author Ilia Vaprol
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
	int i, res;
	char *equal;

	for (i = 1; i < argc; ++i) {
		equal = strchr(argv[i], '=');
		if (!equal) continue;

		if (*(equal + 1)) {
			res = putenv(argv[i]);
			if (res == -1) return -errno;
		}
		else {
			*equal = '\0';
			res = unsetenv(argv[i]);
			if (res == -1) return -errno;
		}
	}

	return 0;
}

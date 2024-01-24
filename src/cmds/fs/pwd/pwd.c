/**
 * @file
 * @brief
 *
 * @date 01.04.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <lib/libds/array.h>

int main(int argc, char *argv[]) {
	char buff[PATH_MAX];

	if (NULL == getcwd(&buff[0], ARRAY_SIZE(buff))) {
		return -errno;
	}

	printf("%s\n", &buff[0]);

	return 0;
}

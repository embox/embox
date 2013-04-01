/**
 * @file
 * @brief
 *
 * @date 01.04.13
 * @author Ilia Vaprol
 */

#include <embox/cmd.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <util/array.h>

EMBOX_CMD(exec);

static int exec(int argc, char *argv[]) {
	char buff[PATH_MAX];

	if (NULL == getcwd(&buff[0], ARRAY_SIZE(buff))) {
		return -errno;
	}

	printf("%s\n", &buff[0]);

	return 0;
}

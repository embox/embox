/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.11.2014
 */

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "libleddrv.h"

int main(int argc, char *argv[]) {
	const char *action = argv[1];

	if (action) {
		if (0 == strcmp(action, "init")) {
			return leddrv_init();
		} else if (0 == strcmp(action, "set")) {
			return leddrv_set(atoi(argv[2]));
		} else if (0 == strcmp(action, "clr")) {
			return leddrv_clr(atoi(argv[2]));
		}
	}

	return -EINVAL;
}

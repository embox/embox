/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    09.06.2012
 */

#include <embox/unit.h>
#include <diag/diag_device.h>

#include <fcntl.h>
#include <unistd.h>

EMBOX_UNIT_INIT(diag_env_init);

int diag_env_init(void) {
	int fd = open("/dev/diag", O_RDWR);

	if (fd != 0) {
		close(fd);
		return 1;
	}

	/* Duplicating diag device for stdout */
	if (1 != (fd = dup(0))) {
		close(0);
		close(fd);
		return 1;
	}

	if (2 != (fd = dup(0))) {
		close(0);
		close(1);
		close(fd);
		return 1;
	}

	return 0;
}


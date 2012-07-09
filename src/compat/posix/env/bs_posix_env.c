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

static int check_valid(int fd, int reference_fd) {
	if (fd == reference_fd) {
		return 0;
	}

	for (int i = 0; i < reference_fd; i++) {
		close(i);
	}

	if (fd < 0) {
		return fd;
	}

	close(fd);
	return -1;
}


int diag_env_init(void) {
	int fd = open("/dev/diag", O_RDWR);
	int res = 0;

	if ((res = check_valid(fd, 0)) != 0) {
		return res;
	}

	for (int i = 1; i <= 2; i++) {
		fd = dup(0);
		if ((res = check_valid(fd, i)) != 0) {
			return res;
		}
	}

	return 0;
}

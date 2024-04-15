/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 05.10.23
 */
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

static void stty_print_usage(void) {
	printf("Usage: stty [-F DEVICE] [SETTING]...\n");
}

static int stty_print_speed(int fd) {
	struct termios2 termios2;
	speed_t speed;

	if (-1 == ioctl(fd, TCGETS2, &termios2)) {
		return -errno;
	}

	/* Get custom baud rate (standard baud rates are not yet supported) */
	speed = termios2.c_ospeed;

	printf("%u\n", speed);

	return 0;
}

static int stty_set_speed(int fd, speed_t speed) {
	struct termios2 termios2;

	if (-1 == ioctl(fd, TCGETS2, &termios2)) {
		return -errno;
	}

	termios2.c_cflag &= ~CBAUD; /* Remove current baud rate */
	termios2.c_cflag |= BOTHER; /* Allow custom baud rate */

	termios2.c_ispeed = speed; /* Set the input baud rate */
	termios2.c_ospeed = speed; /* Set the output baud */

	if (-1 == ioctl(fd, TCSETS2, &termios2)) {
		return -errno;
	}

	return 0;
}

int main(int argc, char **argv) {
	speed_t speed;
	int opt;
	int err;
	int fd;

	err = 0;

	if (argc < 2) {
		err = -EINVAL;
		goto out;
	}

	if (-1 == (opt = getopt(argc, argv, "hF"))) {
		err = -EINVAL;
		goto out;
	}

	switch (opt) {
	case 'h':
		stty_print_usage();
		break;

	case 'F':
		if (optind + 1 >= argc) {
			err = -EINVAL;
			goto out;
		}

		if (-1 == (fd = open(argv[optind], O_RDONLY))) {
			err = -errno;
			goto out;
		}

		optind++;
		if (!strcmp(argv[optind], "speed")) {
			err = stty_print_speed(fd);
		}
		else if (isdigit(argv[optind][0])) {
			speed = strtoul(argv[optind], NULL, 10);
			err = stty_set_speed(fd, speed);
		}

		close(fd);
		break;

	default:
		printf("stty: invalid option -- '%c'\n", optopt);
		err = -EINVAL;
		break;
	}

out:
	if (err) {
		stty_print_usage();
	}

	return err;
}

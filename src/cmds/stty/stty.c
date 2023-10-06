/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 05.10.23
 */
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

static void stty_print_usage(void) {
	printf("Usage: stty [-F DEVICE] [SETTING]...\n");
}

static int stty_print_speed(int fd) {
	struct termios t;
	int err;

	err = 0;

	if (-1 == tcgetattr(fd, &t)) {
		err = -errno;
		goto out;
	}

	printf("%u\n", t.c_ospeed);

out:
	return err;
}

static int stty_set_speed(int fd, speed_t speed) {
	struct termios t;
	int err;

	err = 0;

	if (-1 == tcgetattr(fd, &t)) {
		err = -errno;
		goto out;
	}

	t.c_ospeed = speed;
	t.c_ispeed = speed;

	if (-1 == tcsetattr(fd, TCSANOW, &t)) {
		err = -errno;
		goto out;
	}

out:
	return err;
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

	if (-1 != (opt = getopt(argc, argv, "hF"))) {
		switch (opt) {
		case 'h':
			stty_print_usage();
			break;

		case 'F':
			if (optind + 1 >= argc) {
				err = -EINVAL;
				goto out;
			}

			if (-1 == (fd = open(argv[optind], O_RDWR))) {
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

			if (err) {
				goto out;
			}
			break;

		default:
			printf("stty: invalid option -- '%c'\n", optopt);
			err = -EINVAL;
			goto out;
		}
	}

out:
	if (err) {
		stty_print_usage();
	}

	return err;
}

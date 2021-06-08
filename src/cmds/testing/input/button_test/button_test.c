/**
 * @file
 * @brief Touchscreen test - draws touches, print position and other events
 *
 * @date   15.04.2020
 * @author Alexander Kalmuk
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include <drivers/input/input_dev.h>


static int button_handler(int fd) {
	struct input_event ev;
	int type;

	while (1) {
		if (read(fd, &ev, sizeof ev) <= 0) {
			continue;
		}

		type = ev.type;

		switch (type) {
		case USER_BUTTON_PRESSED:
			printf("button pressed\n");
			break;
		case USER_BUTTON_UNPRESSED:
			printf("button unpressed\n");
			break;
		default:
			printf("event (type=%d, value=%d)\n", ev.type, ev.value);
			break;
		}

	}

	return 0;
}

static void print_usage(const char *cmd) {
	printf("Usage: %s [-h] <button>\n", cmd);
}

int main(int argc, char **argv) {
	int opt, fd;

	if (argc < 2) {
		print_usage(argv[0]);
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch (opt) {
		case 'h':
			print_usage(argv[0]);
			/* FALLTHROUGH */
		default:
			return 0;
		}
	}

	fd = open(argv[argc - 1], O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "Cannot open touchscreen \"%s\"\n", argv[argc - 1]);
		return -1;
	}


	/* Testing for infinite time. */
	button_handler(fd);

	return 0;
}

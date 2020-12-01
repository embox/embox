/**
 * @file
 * @brief Keyboard test - prints keyboards events to serial port
 *
 * @date 08.04.2020
 * @author Alexander Kalmuk
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>
#include <drivers/input/input_dev.h>
#include <drivers/input/keymap.h>

static int keyboard_handle(int fd) {
	struct input_event ev;
	unsigned char code[4];
	int event_nr = 0;

	while (1) {
		if (read(fd, &ev, sizeof ev) <= 0) {
			continue;
		}

		printf("event (id=%d, type=0x%x, value=0x%x)\n",
			event_nr++, ev.type, ev.value);
		printf("Key %s\n", ev.type & KBD_KEY_PRESSED ? "pressed" : "released");
		if (1 == keymap_to_ascii(&ev, code)) {
			if (isprint(code[0])) {
				printf("Code = %c\n", code[0]);
			}
		}
		printf("\n");
	}

	return 0;
}

static void print_usage(const char *cmd) {
	printf("Usage: %s [-h] <keyboard>\n", cmd);
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
		fprintf(stderr, "Cannot open keyboard \"%s\"\n", argv[argc - 1]);
		return -1;
	}

	/* Testing for infinite time. */
	keyboard_handle(fd);

	return 0;
}

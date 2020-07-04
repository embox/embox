/**
 * @file
 *
 * @brief Show list of input devices
 *
 * @date 15.04.2020
 * @author Alexander Kalmuk
 */
#include <stdio.h>
#include <unistd.h>
#include <drivers/input/input_dev.h>

static void print_usage(const char *cmd) {
	printf("Usage: %s\n", cmd);
}

static char *type_to_str(enum input_dev_type type) {
	switch (type) {
	case INPUT_DEV_KBD:
		return "keyboard";
	case INPUT_DEV_MOUSE:
		return "mouse";
	case INPUT_DEV_APB:
		return "apb";
	case INPUT_DEV_TOUCHSCREEN:
		return "touchscreen";
	default:
		return "unknown";
	}
}

int main(int argc, char **argv) {
	struct input_dev *dev = NULL;
	int opt;

	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch (opt) {
		case 'h':
			/* FALLTHROUGH */
		default:
			print_usage(argv[0]);
			return 0;
		}
	}

	printf("        Name        Type\n");
	while ((dev = input_dev_iterate(dev))) {
		printf("%12s%12s\n", dev->name, type_to_str(dev->type));
	}

	return 0;
}

/**
 * @file
 *
 * @date Oct 29, 2012
 * @author: Anton Bondarev
 *          Roman Kurbatov
 *          - options processing
 *          - rebooting the system
 */

#include <stdio.h>
#include <string.h>

#include <hal/platform.h>

static void print_usage(void) {
	printf("Usage:"
	       "\n"
	       "shutdown -p"
	       "\n"
	       "\t"
	       "turn off the system"
	       "\n"
	       "shutdown -r"
	       "\n"
	       "\t"
	       "reboot the system"
	       "\n");
}

int main(int argc, char **argv) {
	if (argc != 2) {
		print_usage();
		return 0;
	}

	if (strcmp(argv[1], "-p") == 0) {
		platform_shutdown(SHUTDOWN_MODE_HALT);
	}
	else if (strcmp(argv[1], "-r") == 0) {
		platform_shutdown(SHUTDOWN_MODE_REBOOT);
	}
	else {
		print_usage();
	}

	return 0;
}

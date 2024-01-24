/**
 * @file
 * @brief Print existing in system clock sources
 *
 * @date 26.11.2011
 * @author Alina Kramar
 */

#include <errno.h>
#include <lib/libds/dlist.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <kernel/time/clock_source.h>

static void print_usage(void) {
	printf("Usage: clock [-i] [-h]\n");
}

static int print_info(struct clock_source *cs, int num) {
	printf("%02d: name: ", num);
	cs->name ? printf("%s\n", cs->name) : printf("%s", "no name");
	if (cs->counter_device) {
		printf("counter_device frequency: %u\n", cs->counter_device->cycle_hz);
	}
	if (cs->event_device) {
		printf("event_device frequency: %u\n", cs->event_device->event_hz);
	}
	printf("\n");
	return 0;
}

static int clock_source_info(void) {
	extern struct clock_source *kernel_clock_source;
	struct clock_source *cs;
	int k = 0;

	dlist_foreach_entry(cs, &kernel_clock_source->lnk, lnk) {
		print_info(cs, ++k);
	}

	return 0;
}

int main(int argc, char **argv) {
	int opt;

	if (argc <= 1) {
		print_usage();
		return -EINVAL;
	}


	while (-1 != (opt = getopt(argc, argv, "hi"))) {
		printf("\n");
		switch (opt) {
		case '?':
			printf("Invalid command line option\n");
			/* FALLTHROUGH */
		case 'h':
			print_usage();
			break;
		case 'i':
			clock_source_info();
			break;
		default:
			break;
		}
	}

	return 0;
}

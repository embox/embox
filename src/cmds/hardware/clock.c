/**
 * @file
 * @brief Print existing in system clock sources
 *
 * @date 26.11.2011
 * @author Alina Kramar
 */

#include <embox/cmd.h>

#include <errno.h>
#include <util/array.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <kernel/time/clock_source.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: clock [-i] [-h]\n");
}

static int print_info(const struct clock_source *cs, int num) {
	printf("%02d:\n", num);
	cs->name ? printf("%s\n", cs->name) : printf("%s", "no name");
	printf("flags: %04d\n", cs->flags);
	if (cs->counter_device)
		printf("counter_device resolution: %d\n", cs->counter_device->resolution);
	if (cs->event_device)
		printf("event_device resolution: %d\n\n", cs->event_device->resolution);
	return 0;
}

extern const struct clock_source * __clock_sources[];

static int clock_source_info(void) {
	const struct clock_source *cs;
	int k = 0;

	array_foreach(cs, __clock_sources, ARRAY_SPREAD_SIZE(__clock_sources)) {
		print_info(cs, ++k);
	}

	return 0;
}

static int exec(int argc, char **argv) {
	int opt;

	if (argc <= 1) {
		print_usage();
		return -EINVAL;
	}

	getopt_init();

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

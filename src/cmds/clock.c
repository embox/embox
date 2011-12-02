/**
 * @file
 * @brief Print existing in system clock sources
 *
 * @date 26.11.2011
 * @author TODO Alina Kramar
 */

#include <embox/cmd.h>

#include <lib/list.h>

#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <kernel/clock_source.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: clock [-i] [-h]\n");
}

static int print_info(struct clock_source* c_src, int num) {
	printf("%02d:\n", num);
	printf("    flags: %04d\n", c_src->flags);
	printf("    precision: %04d\n", c_src->precision);
	return 0;
}

static int clock_source_info(void) {
	struct clock_source_head* ptr;
	int k = 0;

	list_for_each_entry(ptr, &clock_source_list, lnk) {
		print_info(ptr->clock_source, k);
		k++;
	}

	return 0;
}

static int exec(int argc, char **argv) {
	int opt;

	if (argc <= 1) {
		print_usage();
		return -1;
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

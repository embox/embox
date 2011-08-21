/**
 * @file
 *
 * @brief
 *
 * @date 20.08.2011
 * @author Anton Bondarev
 *
 */
#include <stdio.h>
#include <embox/cmd.h>
#include <getopt.h>
#include <errno.h>
#include <framework/example/api.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: example [h] [<example_name>]");
}

static void print_examples(void) {
	const struct example *example;
	int i = 0;

	example_foreach(example) {
		printf("%3d. %s\n", ++i, example_name(example));
	}
	printf("\nTotal examples: %d\n", i);
}

static int exec(int argc, char **argv) {
	int opt;
	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch (opt) {
		case '?':
		case 'h':
			print_usage();
			return ENOERR;
		default:
			return -EINVAL;
		}
	};

	print_examples();

	return ENOERR;
}

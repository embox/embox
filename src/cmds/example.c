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
		printf("%3d. %s.%s\n", ++i, example_path(example), example_name(example));
	}
	printf("\nTotal examples: %d\n", i);
}

static int exec(int argc, char **argv) {
	int opt;
	const struct example *example;

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
	if (argc > 1) {
		example = example_lookup(argv[1]);
		example_exec(example, argc - 1, argv + 1);
		return 0;
	}

	print_examples();

	return ENOERR;
}

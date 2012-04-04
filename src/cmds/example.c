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
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <framework/example/api.h>
#include <embox/cmd.h>


EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: example [-h] [-n <number> | <example_name>]");
}

static void print_examples(void) {
	const struct example *example;
	int i = 0;

	example_foreach(example) {
		if(0 == strcmp("", example_path(example))) {
			printf("%3d. %s\n", ++i, example_name(example));
		} else {
			printf("%3d. %s.%s\n", ++i, example_path(example), example_name(example));
		}
	}
	printf("\nTotal examples: %d\n", i);
}

static int exec(int argc, char **argv) {
	int opt, number, args_enum;
	const struct example *example;

	number = 0;
	args_enum = 1;
	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "n:h"))) {
		switch (opt) {
		case '?':
		case 'h':
			print_usage();
			return ENOERR;
		case 'n':
			if (sscanf(optarg, "%d", &number) != 1) {
				printf("Invalid a number of example\n");
				return -EINVAL;
			}
			args_enum = 2;
			break;
		default:
			return -EINVAL;
		}
	}
	if (argc > 1) {
		if (number == 0) {
			example = example_lookup(argv[1]);
			if (example == NULL) {
				printf("Example \"%s\" not found\n", argv[1]);
			}
		}
		else {
			example_foreach(example) {
				if (--number == 0) {
					break;
				}
			}
			if (number != 0) {
				printf("Invalid a number of example\n");
				return -EINVAL;
			}
		}
		example_exec(example, argc - args_enum, argv + args_enum);
		return 0;
	}

	print_examples();

	return ENOERR;
}

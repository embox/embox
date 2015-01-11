/**
 * @file
 * @brief Example runner
 *
 * @date 20.08.2011
 * @author Anton Bondarev
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <framework/example/api.h>

static void print_usage(const char *cmd) {
	printf("Usage: %s [-h] [-n <number> | <example_name>]", cmd);
}

static void print_examples(void) {
	const struct example *example;
	int i = 0;

	example_foreach(example) {
		if (0 == strcmp("", example_path(example))) {
			printf("%3d. %s\n", ++i, example_name(example));
		} else {
			printf("%3d. %s.%s\n", ++i,
				example_path(example), example_name(example));
		}
	}
	printf("\nTotal examples: %d\n", i);
}

int main(int argc, char **argv) {
	int opt, number;
	const struct example *example;

	if (argc == 1) {
		print_examples();
		return ENOERR;
	}

	number = -1;
	example = NULL;
	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "n:h"))) {
		switch (opt) {
		case 'n':
			if (sscanf(optarg, "%d", &number) != 1) {
				printf("Invalid a number of example\n");
				return -EINVAL;
			}
			break;
		case 'h':
			print_usage(argv[0]);
			return ENOERR;
		default:
			if (number == -1 && optind <= 1) {
				return -EINVAL;
			}
			break;
		}
	}

	if (number == -1) {
		example = example_lookup(argv[optind]);
		if (example == NULL) {
			printf("Example \"%s\" not found\n", argv[optind]);
			return -ENOENT;
		}
	} else {
		example_foreach(example) {
			if (--number == 0) {
				break;
			}
		}
		if (number != 0) {
			printf("Invalid a number of example\n");
			return -EINVAL;
		}

		optind --;
		argv[optind] = (char *)example_name(example);
	}

	return example_exec(example, argc - optind, argv + optind);
}

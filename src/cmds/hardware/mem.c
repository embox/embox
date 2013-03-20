/**
 * @file
 * @brief read from memory
 *
 * @date 13.02.09
 * @author Alexey Fomin
 */

#include <embox/cmd.h>

#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

EMBOX_CMD(exec);

#define DEFAULT_LENGTH 0x4

static void print_usage(void) {
	printf("Usage: mem [-h] -a addr [-n bytes]\n");
}

static int parse_option(char *optarg, int opt, long unsigned int *number) {
	char *endptr;

	if ((optarg == NULL) || (*optarg == '\0')) {
		printf("wmem -%c: option expected\n", opt);
		print_usage();
		return -EINVAL;
	}

	*number = strtoul(optarg, &endptr, 0);
	if (*endptr != '\0') {
		printf("wmem -%c: invalid option: %s\n", opt, optarg);
		print_usage();
		return -EINVAL;
	}

	return 0;
}

static int exec(int argc, char **argv) {
	bool a_flag = false;
	int opt, i, ret;
	unsigned long int *address;
	size_t length = DEFAULT_LENGTH;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "a:n:h"))) {
		switch (opt) {
		case 'a':
			ret = parse_option(optarg, opt, (unsigned long int *) &address);
			if (ret != 0) {
				return ret;
			}
			a_flag = true;
			break;

		case 'n':
			ret = parse_option(optarg, opt, (unsigned long int *) &length);
			if (ret != 0) {
				return ret;
			}
			break;

		case '?':
		case 'h':
			print_usage();
			/* FALLTHROUGH */
		default:
			return 0;
		}
	}

	if (!a_flag) {
		printf("mem: address required\n");
		print_usage();
		return -EINVAL;
	}

	/*address = (unsigned int *) ((int) address & ~(sizeof(address) - 1));*/
	/*length = (length + sizeof(address) - 1) / sizeof(address);*/
	i = 0;
	while (length--) {
		if (i-- == 0) {
			i = 3;
			printf("\n0x%08x:\t", (unsigned int) address);
		}
		printf("0x%08lx  ", *address++);
	}
	printf("\n");

	return 0;
}

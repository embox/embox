/**
 * @file
 * @brief Writes memory word at the specified address.
 *
 * @date 20.02.09
 * @author Alexey Fomin
 *          - Initial implementation
 * @author Eldar Abusalimov
 *          - Reviewing and refactoring
 */

#include <embox/cmd.h>

#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: wmem [-h] -a addr -v value\n");
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
	bool a_flag = false, v_flag = false;
	int opt, ret;
	volatile unsigned int *address;
	unsigned int value;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "a:v:h"))) {
		switch (opt) {
		case 'a':
			ret = parse_option(optarg, opt, (unsigned long int *) &address);
			if (ret != 0) {
				return ret;
			}
			a_flag = true;
			break;

		case 'v':
			ret = parse_option(optarg, opt, (unsigned long int *) &value);
			if (ret != 0) {
				return ret;
			}
			v_flag = true;
			break;

		case '?':
		case 'h':
			print_usage();
			return 0;
		default:
			return 0;
		}
	}

	if (!a_flag || !v_flag) {
		printf("wmem: both -a and -v options required\n");
		print_usage();
		return -EINVAL;
	}

	*address = value;

	return 0;
}

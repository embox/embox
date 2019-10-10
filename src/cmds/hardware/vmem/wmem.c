/**
 * @file
 * @brief Writes memory word at the specified address.
 *
 * @date 20.02.09
 * @author Alexey Fomin
 *          - Initial implementation
 * @author Eldar Abusalimov
 *          - Reviewing and refactoring
 * @author Anton Kozlov
 *          - access_type added
 */

#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

enum access_type {
	MEM_AT_CHAR,
	MEM_AT_SHORT,
	MEM_AT_LONG,
};

static void print_usage(void) {
	printf("Usage: wmem [-h] [-l -s -c] -a addr -v value\n");
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

int main(int argc, char **argv) {
	bool a_flag = false, v_flag = false;
	int opt, ret;
	volatile unsigned int *address;
	unsigned int value;
	enum access_type at = MEM_AT_LONG;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "a:v:hlsc"))) {
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
		case 'l':
			at = MEM_AT_LONG;
			break;
		case 's':
			at = MEM_AT_SHORT;
			break;
		case 'c':
			at = MEM_AT_CHAR;
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

	if ((int) address & ((1 << at) - 1)) {
		printf("address is not aligned to selected mem access\n");
		return -EINVAL;
	}

	switch (at) {
	case MEM_AT_LONG:
		*(unsigned long *) address = value;
		break;
	case MEM_AT_SHORT:
		*(unsigned short *) address = value;
		break;
	case MEM_AT_CHAR:
		*(unsigned char *) address = value;
		break;
	}

	return 0;
}

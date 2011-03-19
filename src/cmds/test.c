/**
 * @file
 * @brief Works with Embox test subsystem.
 *
 * @date 17.11.09
 * @author Alexey Fomin
 *         - Initial implementation
 * @author Eldar Abusalimov
 *         - Rewriting some parts from scratch
 */

#include <embox/cmd.h>

#include <getopt.h>
#include <stdio.h>

#include <test/framework.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: test [-h] [-n <test_num>] [-t <test_name, string>] [-i]\n");
}

static void print_tests(void) {
	const struct test_suite *test;
	int i = 0;

	test_foreach(test) {
		printf("%3d. %s\n", ++i, test_name(test));
	}
	printf("\nTotal tests: %d\n", i);
}

static const struct test_suite *get_test_by_nr(int nr) {
	const struct test_suite *test;
	int i = 0;

	if (nr <= 0) {
		printf("Invalid test number: %d\n", nr);
		return NULL;
	}

	test_foreach(test) {
		if (++i == nr) {
			return test;
		}
	}

	printf("Invalid test number: %d (total tests: %d)\n", nr, i);
	return NULL;
}

static int exec(int argc, char **argv) {
	const struct test_suite *test = NULL;
	int test_nr = -1;
	int opt;
	/* TODO it must be agreed with shell maximum command length */
	char test_name[100] = { 0 };

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "hn:t:i"))) {
		switch (opt) {
		case 'n':
			if ((optarg == NULL) || (!sscanf(optarg, "%d", &test_nr))) {
				printf("test -n: number expected\n");
				print_usage();
				return -1;
			}
			break;
		case 't':
			if ((optarg == NULL) || (!sscanf(optarg, "%s", test_name))) {
				printf("test -t: test name expected\n");
				print_usage();
				return -1;
			}
			break;
		case '?':
		case 'h':
			print_usage();
			/* FALLTHROUGH */
		default:
			return 0;
		}
	};

	if (test_nr != -1) {
		if (NULL == (test = get_test_by_nr(test_nr))) {
			return -1;
		}
	}
	if (*test_name != 0) {
		if (NULL == (test = test_lookup(test_name))) {
			printf("Can't find test named \"%s\"\n", test_name);
			return -1;
		}
	}

	if (NULL == test) {
		print_tests();
		return 0;
	}

	return test_suite_run(test);
}

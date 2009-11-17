/**
 * \file expresstest.c
 * \date Nov 17, 2009
 * \author afomin
 */

#include "shell_command.h"
#include "express_tests.h"

#define COMMAND_NAME     "expresstest"
#define COMMAND_DESC_MSG "works with express test subsystem"
#define HELP_MSG         "Usage: expresstest [-h] [-n <test_num> [-i]]"
static const char *man_page =
#include "expresstest_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page)
;

static void print_express_tests() {
	extern express_test_descriptor_t *__express_tests_start,
			*__express_tests_end;
	express_test_descriptor_t ** p_test = &__express_tests_start;
	int i, total = (int) (&__express_tests_end - &__express_tests_start);

	TRACE("\nTotal tests: %d\n\n", total);

	for (i = 0; i < total; i++, p_test++) {
		TRACE("%3d) ", i);
		if (NULL == (*p_test)) {
			TRACE("BROKEN\n");
			continue;
		}
		if (NULL == ((*p_test)->name)) {
			TRACE("NONAME ");
		}
		TRACE("%10s - ", (*p_test)->name);
		if (NULL == ((*p_test)->exec)) {
			TRACE("BROKEN\n");
			continue;
		}
	}
}

static express_test_descriptor_t *get_express_test(int test_num) {
	extern express_test_descriptor_t *__express_tests_start,
			*__express_tests_end;
	express_test_descriptor_t **p_test = &__express_tests_start;
	int total = (int) (&__express_tests_end - &__express_tests_start);
	if (test_num < 0 || test_num > total - 1) {
		return NULL;
	}
	p_test += test_num;
	return *p_test;
}
static int exec(int argsc, char **argsv) {
	BOOL run_info_func = FALSE;
	express_test_descriptor_t *p_test = NULL;
	int test_num = -1;
	int nextOption;
	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "hn:i");
		switch (nextOption) {
		case 'h':
			show_man_page();
			return 0;
		case 'n':
			if ((optarg == NULL) || (!sscanf(optarg, "%d", &test_num))) {
				LOG_ERROR("%s: -n: test number expected.\n", COMMAND_NAME);
				show_help();
				return -1;
			}
		case 'i':
			if (test_num == -1) {
				LOG_ERROR("%s: -i: missing -n before -i.\n", COMMAND_NAME);
			}
			run_info_func = TRUE;
			return 0;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != nextOption);

	if (test_num == -1) {
		print_express_tests();
		return 0;
	}

	p_test = get_express_test(test_num);
	if (run_info_func) {
		if (p_test->info_func == NULL) {
			LOG_ERROR("No info func available for that test\n");
			return -1;
		}
		p_test->info_func();
		return 0;
	}

	if (p_test->exec == NULL) {
		LOG_ERROR("Unable to execute express test\n");
		return -1;
	}
	p_test->exec();
	return 0;
}

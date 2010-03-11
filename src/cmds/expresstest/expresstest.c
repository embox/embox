/**
 * @file
 * @brief Works with express test subsystem.
 *
 * @date 17.11.2009
 * @author Alexey Fomin
 */
#include <shell_command.h>
#include <embox/test.h>

#define COMMAND_NAME     "expr"
#define COMMAND_DESC_MSG "works with express test subsystem"
#define HELP_MSG         "Usage: expresstest [-h] [-n <test_num> [-i]]"
static const char *man_page =
#include "expresstest_help.inc"
;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page)
;

static inline char to_upper(char c) {
	if ((c <= 'z') && (c >= 'a')) {
		return c + 'A' - 'a';
	} else {
		return c;
	}
}
static int is_ignored_symbol(char c) {
	return (c == '_');
}

/**
 * Compares input_name and test_name
 * Ignores special symbols in test_name
 * But all symbols in input_name are significant
 *
 * e.g.  cpucontext == cpu_context
 *      cpu_context == cpu_context
 *  but cpu_context != cpucontext
 */
static int is_test_name(const char *input_name, const char *test_name) {
	while(*test_name) {
		if (to_upper(*test_name) == to_upper(*input_name)) {
			test_name++;
			input_name++;
			continue;
		}
		if (is_ignored_symbol(*test_name)) {
			test_name++;
			continue;
		}
		return 0;
	}
	return (*input_name == 0);
}

#define DEFAULT_NAME_STR "NONAME"
#define DEFAULT_SHORT_NAME_STR "?"
#define BROKEN_DESCRIPTOR_STR "BROKEN"
#define ON_BOOT_TEST_STR "ONBOOT"
#define MANUAL_TEST_STR "MANUAL"
#define NO_INFO_ERROR_STR "No info func available for that test"
#define WRONG_TEST_NUMBER_ERROR_STR "Wrong test number entered"
#define WRONG_TEST_NAME_ERROR_STR "test with specified name not found"

#define PASSED_CODE 0
#define FAILED_STR "FAILED"
#define PASSED_STR "PASSED"

static void print_express_tests(void) {
	extern express_test_descriptor_t *__express_tests_start,
			*__express_tests_end;
	express_test_descriptor_t ** p_test = &__express_tests_start;
	int i, total = (int) (&__express_tests_end - &__express_tests_start);

	TRACE("\nTotal tests: %d\n\n", total);

	for (i = 0; i < total; i++, p_test++) {
		TRACE("%3d) ", i);
		if (NULL == (*p_test)) {
			TRACE(BROKEN_DESCRIPTOR_STR"\n");
			continue;
		}

		if (NULL != ((*p_test)->name)) {
			TRACE("%10s ", (*p_test)->name);
		} else {
			TRACE(DEFAULT_NAME_STR" ");
		}
		if (NULL != ((*p_test)->name)) {
			TRACE("(%s) - ", (*p_test)->name);
		} else {
			TRACE(DEFAULT_SHORT_NAME_STR" ");
		}
		if (NULL == ((*p_test)->exec)) {
			TRACE(BROKEN_DESCRIPTOR_STR"\n");
			continue;
		}
		if ((*p_test)->execute_on_boot) {
			TRACE(ON_BOOT_TEST_STR"\n");
		} else {
			TRACE(MANUAL_TEST_STR"\n");
		}
	}
}

static express_test_descriptor_t *get_express_test(int test_num) {
	extern express_test_descriptor_t *__express_tests_start,
			*__express_tests_end;
	express_test_descriptor_t **p_test = &__express_tests_start;
	int total = (int) (&__express_tests_end - &__express_tests_start);

	if (test_num < 0 || test_num > total - 1) {
		TRACE("total: %d\ncurrent: %d\n\n\n", total, test_num);
		return NULL;
	}
	p_test += test_num;
	return *p_test;
}

static express_test_descriptor_t *get_express_test_by_name(char *name) {
	extern express_test_descriptor_t *__express_tests_start,
			*__express_tests_end;
	express_test_descriptor_t **p_test = &__express_tests_start;
	int total = (int) (&__express_tests_end - &__express_tests_start);
	int i;

	for (i = 0; i < total; i++, p_test++) {
		if (NULL == (*p_test)) {
			/* Broken descriptor */
			continue;
		}
		if (NULL == ((*p_test)->name)) {
			/* Broken test name */
			continue;
		}
		if (is_test_name(name, (*p_test)->name)) {
			return *p_test;
		}
	}

	return 0;
}

static int exec(int argsc, char **argsv) {
	bool run_info_func = false;
	express_test_descriptor_t *p_test = NULL;
	int test_num = -1, result;
	int nextOption;
	/* TODO it must be agreed with shell maximum command length */
	char test_name[100] = { 0 };

	getopt_init();
	do {
		nextOption = getopt(argsc, argsv, "hn:t:i");
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
			break;
		case 't':
			if ((optarg == NULL) || (!sscanf(optarg, "%s", test_name))) {
				LOG_ERROR("%s: -t: test name expected.\n", COMMAND_NAME);
				show_help();
				return -1;
			}
			break;
		case 'i':
			if ((test_num == -1) && (*test_name == 0)) {
				LOG_ERROR("%s: -i: missing -n or -t before -i.\n", COMMAND_NAME);
			}
			run_info_func = true;
			break;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != nextOption);
	if (test_num == -1 && (*test_name == 0)) {
		print_express_tests();
		return 0;
	}
	if (test_num == -1) {
		p_test = get_express_test_by_name(test_name);
		if (p_test == NULL) {
			TRACE("%s: "WRONG_TEST_NAME_ERROR_STR"\n", test_name);
			return -1;
		}
	} else {
		p_test = get_express_test(test_num);
		if (p_test == NULL) {
			TRACE(WRONG_TEST_NUMBER_ERROR_STR"\n");
			return -1;
		}
	}
	if (run_info_func) {
		if (p_test->info_func == NULL) {
			LOG_ERROR(NO_INFO_ERROR_STR"\n");
			return -1;
		}
		p_test->info_func();
		return 0;
	}
	if (p_test->exec == NULL) {
		LOG_ERROR("Unable to execute express test\n");
		return -1;
	}
	TRACE("Testing %s ... ", p_test->name == NULL ?
			DEFAULT_NAME_STR : p_test->name);
	result = p_test->exec(0, NULL);
	if (result == PASSED_CODE) {
		TRACE(PASSED_STR"\n");
	} else {
		TRACE(FAILED_STR"\n");
	}

	return 0;
}

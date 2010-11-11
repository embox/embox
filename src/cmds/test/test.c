/**
 * @file
 * @brief Works with express test subsystem.
 *
 * @date 17.11.2009
 * @author Alexey Fomin
 *         - Initial implementation
 * @author Eldar Abusalimov
 *         - Rewriting some parts from scratch
 */

#include <ctype.h>
#include <stdbool.h>

#include <shell_command.h>
#include <test/framework.h>
// XXX for struct mod. -- Eldar
#include <mod/core.h>

#define COMMAND_NAME     "test"
#define COMMAND_DESC_MSG "works with express test subsystem"
#define HELP_MSG \
	"Usage: test [-h] [-n <test_num>] [-t <test_name, string>] [-i]"
static const char *man_page =
#include "test_help.inc"
;

#define DEFAULT_NAME_STR "NONAME"

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

static void print_tests(void) {
	struct test *test;
	int i = 0;

	test_foreach(test) {
		TRACE("%3d. ", ++i);

		if (NULL != test->mod->name) {
			TRACE("%s\n", test->mod->name);
		} else {
			TRACE(DEFAULT_NAME_STR);
		}
	}
	TRACE("\nTotal tests: %d\n", i);
}

static bool is_ignored_symbol(char c) {
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
static bool is_test_name(const char *input_name, const char *test_name) {
	if (NULL == input_name || NULL == test_name) {
		return false;
	}
	while (*test_name) {
		if (tolower(*test_name) == tolower(*input_name)) {
			test_name++;
			input_name++;
			continue;
		}
		if (is_ignored_symbol(*test_name)) {
			test_name++;
			continue;
		}
		return false;
	}
	return (*input_name == 0);
}

static struct test *get_test_by_name(char *name) {
	struct test *test;

	test_foreach(test) {
		if (is_test_name(name, test->mod->name)) {
			return test;
		}
	}

	return NULL;
}

static struct test *get_test(int nr) {
	struct test *test = NULL;
	int i = 0;

	if (nr <= 0) {
		TRACE("Invalid test number: %d\n", nr);
		return NULL;
	}

	test_foreach(test) {
		if(++i == nr) {
			break;
		}
	}

	if (NULL == test) {
		TRACE("Invalid test number: %d (total tests: %d)\n", nr, i);
	}
	return test;
}

static int exec(int argc, char **argv) {
	bool run_info_func = false;
	struct test *test = NULL;
	int test_nr = -1;
	int next_option;
	/* TODO it must be agreed with shell maximum command length */
	char test_name[100] = { 0 };

	getopt_init();
	do {
		switch (next_option = getopt(argc, argv, "hn:t:i")) {
		case 'h':
			show_man_page();
			return 0;
		case 'n':
			if ((optarg == NULL) || (!sscanf(optarg, "%d", &test_nr))) {
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
			run_info_func = true;
			break;
		case -1:
			break;
		default:
			return 0;
		}
	} while (-1 != next_option);

	if (test_nr == -1 && *test_name == 0) {
		if (run_info_func) {
			LOG_ERROR("%s: -i: missing -n or -t.\n", COMMAND_NAME);
		}
		print_tests();
		return 0;
	}
	if (test_nr == -1) {
		if (NULL == (test = get_test_by_name(test_name))) {
			return -1;
		}
	} else {
		if (NULL == (test = get_test(test_nr))) {
			return -1;
		}
	}
	if (run_info_func) {
		LOG_ERROR("Not yet implemented\n");
		return 0;
	}

	test_invoke(test);

	return 0;
}

/**
 * \file express_tests.c
 *
 * \date Dec 4, 2008
 *
 * \author Anton Bondarev
 * \author Eldar Abusalimov
 */
#include "conio/conio.h"
#include "common.h"
#include "express_tests.h"

int express_tests_execute() {
	extern EXPRESS_TEST_DESCRIPTOR *__express_tests_start, *__express_tests_end;
	EXPRESS_TEST_DESCRIPTOR ** p_test = &__express_tests_start;
	int i, total = (int) (&__express_tests_end - &__express_tests_start);
	int passed = 0, failed = 0;

	TRACE("\nRunning express tests subsystem (total tests: %d)\n\n", total);

	for (i = 0; i < total; i++, p_test++) {
		if (NULL == (*p_test)) {
			LOG_ERROR("Missing express test descriptor\n");
			continue;
		}
		if (NULL == ((*p_test)->name)) {
			LOG_ERROR("Broken express test descriptor: can't find test name\n");
			continue;
		}
		if (NULL == ((*p_test)->exec)) {
			LOG_ERROR("Broken express test descriptor: can't find exec function for test %s\n", (*p_test)->name);
			continue;
		}

		TRACE("Testing %s ... ", (*p_test)->name);
		// TODO magic constants
		int result = sys_exec_start((*p_test)->exec);
		if (result == -1) {
			TRACE("FAILED\n");
			failed++;
		} else if (result == -2) {
			TRACE("INTERRUPTED\n");
			failed++;
		} else if (result == -3) {
			TRACE("UNABLE TO START\n");
			failed++;
		} else {
			TRACE("PASSED\n");
			passed++;
		}

	}

	TRACE("\nPassed: %d\n", passed);
	TRACE("Failed: %d\n", failed);

	return (passed == total) ? 0 : -1;

}

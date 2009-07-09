/**
 * \file express_tests.c
 *
 * \date Dec 4, 2008
 * \author anton
 */
#include "types.h"
#include "common.h"
#include "express_tests.h"

int express_tests_execute() {
	extern EXPRESS_TEST_DESCRIPTOR *__express_tests_start, *__express_tests_end;
	EXPRESS_TEST_DESCRIPTOR ** p_test = &__express_tests_start;
	int total = (int) ((WORD) (&__express_tests_end)
			- (WORD) (&__express_tests_start))
			/ sizeof(EXPRESS_TEST_DESCRIPTOR*);
	int passed = 0, failed = 0;

	TRACE("\nRunning express tests subsystem (total tests: %d)\n\n", total);

	do {
		if (NULL == (*p_test)) {
			LOG_ERROR("Missing express test descriptor\n");
			continue;
		}
		if (NULL == ((*p_test)->name)) {
			LOG_ERROR("Broken express test descriptor. Can't find test name\n");
			continue;
		}
		if (NULL == ((*p_test)->exec)) {
			LOG_ERROR("Broken express test descriptor. Can't find exec function for test %s\n", (*p_test)->name);
			continue;
		}

		TRACE("Testing %s ... ", (*p_test)->name);
		if (-1 != (*p_test)->exec()) {
			TRACE("PASSED\n");
			passed++;
		} else {
			TRACE("FAILED\n");
			failed++;
		}

	} while (++p_test != &__express_tests_end);

	TRACE("\nPassed: %d\n", passed);
	TRACE("Failed: %d\n", failed);

	return (passed + failed == total) ? 0 : -1;

}

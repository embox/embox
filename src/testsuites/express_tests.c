/**
 * \file express_tests.c
 *
 * \date Dec 4, 2008
 *
 * \author Anton Bondarev
 * \author Eldar Abusalimov
 * \author Alexey Fomin
 */
#include <common.h>
#include <express_tests.h>
#include <kernel/init.h>
#include <kernel/sys.h>

int express_tests_execute_all( void ) {
	extern int *__express_tests_result;
	extern express_test_descriptor_t *__express_tests_start, *__express_tests_end;
	express_test_descriptor_t ** p_test = &__express_tests_start;
	int i, total = (int) (&__express_tests_end - &__express_tests_start);
	int passed = 0, failed = 0, skipped = 0, result;

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
		if (!(*p_test)->execute_on_boot)
		{
			TRACE("SKIPPED\n");
			skipped++;
			continue;
		}
		/* TODO magic constants */
#ifndef SIMULATION_TRG
		result = sys_exec_start((*p_test)->exec, 0, NULL);
#else
		result = (*p_test)->exec(0, NULL);
#endif

		/* writing test results to special section */
		__express_tests_result[i] = result;

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

	TRACE("\nSkipped: %d\n", skipped);
	TRACE("Passed: %d\n", passed);
	TRACE("Failed: %d\n", failed);

	return (failed == 0) ? 0 : -1;
}

int express_tests_execute( int level ) {
	extern int *__express_tests_result;
	extern express_test_descriptor_t *__express_tests_start, *__express_tests_end;
	express_test_descriptor_t ** p_test = &__express_tests_start;
	int i, total = (int) (&__express_tests_end - &__express_tests_start);
	int passed = 0, failed = 0, skipped = 0, result;

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
		if ((*p_test)->level != level) {
			continue;
		}

		TRACE("Testing %s ... ", (*p_test)->name);
		if (!(*p_test)->execute_on_boot)
		{
			TRACE("SKIPPED\n");
			skipped++;
			continue;
		}

		/* Executing express test */
#ifndef SIMULATION_TRG
		result = sys_exec_start((*p_test)->exec, 0, NULL);
#else
		result = (*p_test)->exec(0, NULL);
#endif

		/* writing test results to special section */
		__express_tests_result[i] = result;

		if (result == EXPRESS_TESTS_PASSED_RETCODE) {
			TRACE("PASSED\n");
		} else if (result == EXPRESS_TESTS_FAILED_RETCODE) {
			TRACE("FAILED\n");
			failed++;
		} else if (result == EXPRESS_TESTS_INTERRUPTED_RETCODE) {
			TRACE("INTERRUPTED\n");
			failed++;
		} else if (result == EXPRESS_TESTS_UNABLE_TO_START_RETCODE) {
			TRACE("UNABLE TO START\n");
			failed++;
		} else {
			TRACE(" Unknown retcode: %i (counting FAILED)\n", result);
			failed++;
		}
	}

	if (passed + failed == 0) {
		/* No tests to execute on this level
		 * Nothing to write in this case */
		return 0;
	}

	TRACE("\nTests results on level %d:\n", level);
	TRACE("\tSkipped: %d\n", skipped);
	TRACE("\t Passed: %d\n", passed);
	TRACE("\t Failed: %d\n", failed);

	return (failed == 0) ? 0 : -1;
}

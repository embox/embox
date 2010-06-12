/**
 * @file
 * @brief TODO
 *
 * @date 11.06.2010
 * @author Eldar Abusalimov
 */

#ifndef TEST_TYPES_H_
#define TEST_TYPES_H_

/**
 * Each test implements this interface.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */
typedef int(*test_run_t)(void);

struct test_private;

struct test {
	/** Internal data. */
	struct test_private *private;
	/** The test itself. */
	test_run_t run;
	/** Test name (defaults to the corresponding mod name). */
	const char *name;
};

struct test_private {
	int result;
};

#endif /* TEST_TYPES_H_ */

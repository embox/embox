/**
 * @file
 * @brief TODO
 *
 * @date 11.06.2010
 * @author Eldar Abusalimov
 */

#ifndef IMPL_TEST_TYPES_H_
#define IMPL_TEST_TYPES_H_

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
	/** The corresponding mod. */
	const struct mod *mod;
};

struct test_private {
	int result;
};

#endif /* IMPL_TEST_TYPES_H_ */

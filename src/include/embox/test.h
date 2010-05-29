/**
 * @file
 * @brief EMBOX testing framework.
 * @details Test is a simple function which examines some parts of hardware or
 * software. Test can be either passed or failed (framework uses return value
 * of test function to determine the test result).
 * Each test resides in a single mod which is usually defined in
 * @code embox.test @endcode or @code <platform>.test @endcode packages.
 *
 * @sa EMBOX_TEST() simple macro used to declare a new test
 *
 * @date Jul 4, 2009
 * @author Anton Bondarev, Alexey Fomin
 *         - Initial implementation
 * @author Eldar Abusalimov
 *         - Rewriting from scratch, adapting for usage with mod DI framework
 */

#ifndef EMBOX_TEST_H_
#define EMBOX_TEST_H_

#include <stddef.h>
#include <stdbool.h>
#include <embox/kernel.h>
#include <embox/mod.h>

#define EMBOX_TEST(_run) \
	extern const char MOD_SELF_NAME[]; \
	EMBOX_TEST_DETAILS(_run, MOD_SELF_NAME, NULL)

#define EMBOX_TEST_DETAILS(_run, _name, _info) \
	static const struct test __test__; \
	MOD_SELF_API_TAGGED_DEF(&__test__, &__test_mod_ops, test); \
	static int _run(void); \
	static int __test_run__(void) __attribute__ ((alias(#_run))); \
	static struct test_private __test_private__; \
	static const struct test __test__ = { \
			.private = &__test_private__, \
			.run = _run, \
			.info = _info, \
			.name = _name \
		}

#define EMBOX_TEST_EXPORT(symbol) \
	int __test_symbol__##symbol(void) __attribute__ ((alias("__test_run__")))

#define EMBOX_TEST_IMPORT(symbol) \
	static int symbol(void) __attribute__ ((alias("__test_symbol__"#symbol)))

#define test_pass() (0)

#define test_fail(_reason) test_fail_data(_reason, NULL)

#define TEST_ASSERT(cond) \
	do { \
		typeof(cond) __cond = (cond); \
		if (!__cond) \
			return test_fail_data("Condition check failed: " #cond, __cond); \
	} while(0)

#define test_fail_data(_reason, _data) __extension__ ({ \
		__TEST_FAILURE_INFO_DEF(__test_failure_info); \
		__TEST_FAILURE_DEF(__test_failure, &__test_failure_info); \
		__test_failure_info.reason = (_reason); \
		__test_failure_info.data = (void *) (_data); \
		(int) &__test_failure; \
	})

#define __TEST_FAILURE_INFO_DEF(s_var) \
		static struct test_failure_info s_var

#define __TEST_FAILURE_DEF(s_var, _info) \
		static const struct test_failure s_var = { \
				.func = __func__, \
				.file = __FILE__, \
				.line = __LINE__, \
				.info = _info, \
		}

#define test_foreach(t) \
		__test_foreach_guard(t, __LINE__)
#define __test_foreach_guard(t, guard) \
		__test_foreach_guard_expand(t, guard)
#define __test_foreach_guard_expand(t, guard) \
		__test_foreach(t, __test_iterator_##guard)
#define __test_foreach(t, i) \
		for(struct test_iterator i##_alloc, *i = test_get_all(&i##_alloc); \
			(t = test_iterator_has_next(i) ? test_iterator_next(i) : NULL);)

extern const struct mod_ops __test_mod_ops;

/**
 * Each test implements this interface.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on fail (if the test provides @link #test_info_t
 * detailed info @endlink function)
 */
typedef int(*test_run_t)(void);

/**
 * @param result
 */
typedef void (*test_info_t)(int result);

struct test_private;

struct test {
	/** Internal data. */
	struct test_private *private;
	/** The test itself. */
	test_run_t run;
	/** (optional) Shows detailed results of the last test. */
	test_info_t info;
	/** Test name (defaults to the corresponding mod name). */
	const char *name;
};

struct test_private {
	int result;
};

struct test_failure_info;

struct test_failure {
	const char *func;
	const char *file;
	const int line;
	struct test_failure_info *info;
};

struct test_failure_info {
	const char *reason;
	void *data;
};

struct test_iterator {
	struct mod_iterator mod_iterator;
};

extern int test_invoke(struct test *test);

extern struct test_iterator *test_get_all(struct test_iterator *iterator);

extern struct test *test_iterator_next(struct test_iterator *iterator);

extern bool test_iterator_has_next(struct test_iterator *iterator);

#endif /* EMBOX_TEST_H_ */

/**
 * @file
 * @brief TODO
 *
 * @date 11.06.2010
 * @author Eldar Abusalimov
 */

#ifndef TEST_TOOLS_H_
#define TEST_TOOLS_H_

#define test_pass() (0)

#define test_fail(_reason) test_fail_data(_reason, NULL)

#define TEST_ASSERT(cond) \
	do { \
		typeof(cond) __cond = (cond); \
		if (!__cond) \
			return test_fail_data("Condition check failed: " #cond, __cond); \
	} while (0)

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
		.info = _info,    \
	}

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


#endif /* TEST_TOOLS_H_ */

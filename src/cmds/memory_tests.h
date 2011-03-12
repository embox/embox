/**
 * @file
 * @date Jul 29, 2009
 * @author Alexey Fomin
 * @author Daria Teplykh
 */

#ifndef MEMORY_TESTS_H_
#define MEMORY_TESTS_H_

#define MEMTEST_RETCODE_PASSED  0
#define MEMTEST_RETCODE_FAILED -1

typedef struct memtest_err {
	const char *test_name;
	uint32_t addr;
	uint32_t received_value;
	uint32_t expected_value;
} memtest_err_t;

typedef int (*TEST_MEM_FUNC)(uint32_t *addr, long int amount, uint32_t template,
		memtest_err_t *last_err);

typedef struct memtest_desc {
	const char *test_name;
	TEST_MEM_FUNC func;
}memory_test_t;

TEST_MEM_FUNC *get_memtest_func(const char *test_name);

#endif /* MEMORY_TESTS_H_ */

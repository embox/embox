/**
 * @file memory_tests.h
 * @date Jul 29, 2009
 * @author afomin
 * @details
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

typedef struct memtest_desc {
	const char *test_name;
	int (*func)(uint32_t *addr, size_t block_size, uint32_t template,
			memtest_err_t *s_err);
}memory_test_t;

memory_test_t memtest_array[] ={
		{"runzero", memory_test_walking_zero},
		{"runone", memory_test_walking_one},
		{"address", memory_test_address},
		{"chess", memory_test_chess},
		{"quick", memory_test_quick},
		{"loop", memory_test_loop}
};

int memory_test_walking_one(uint32_t *base_addr, long int amount);
int memory_test_walking_zero(uint32_t *base_addr, long int amount);
int memory_test_address(uint32_t *base_addr, long int amount);
int memory_test_chess(uint32_t *base_addr, long int amount);
int memory_test_quick(uint32_t *base_addr, long int amount);

int memory_test_loop(uint32_t *addr, long int counter);

#endif /* MEMORY_TESTS_H_ */

/**
 * \file memory_tests.h
 * \date Jul 29, 2009
 * \author afomin
 * \details
 */

#ifndef MEMORY_TESTS_H_
#define MEMORY_TESTS_H_

int memory_test_run1(uint32_t *base_addr, long int amount);
int memory_test_run0(uint32_t *base_addr, long int amount);
int memory_test_address(uint32_t *base_addr, long int amount);
int memory_test_chess(uint32_t *base_addr, long int amount);
int memory_test_quick(uint32_t *base_addr, long int amount);


int memory_test_loop(uint32_t *addr, long int counter);

#endif /* MEMORY_TESTS_H_ */

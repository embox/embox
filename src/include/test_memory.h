/**
 * @file
 *
 * @date 29.07.2009
 * @author Alexey Fomin
 */
#ifndef TEST_MEMORY_H_
#define TEST_MEMORY_H_

#include <types.h>

int memory_test_run1(uint32_t *base_addr, long int amount);
int memory_test_run0(uint32_t *base_addr, long int amount);
int memory_test_address(uint32_t *base_addr, long int amount);
int memory_test_chess(uint32_t *base_addr, long int amount);
int memory_test_quick(uint32_t *base_addr, long int amount);
int memory_test_loop(uint32_t *addr, long int counter);

#endif /* TEST_MEMORY_H_ */

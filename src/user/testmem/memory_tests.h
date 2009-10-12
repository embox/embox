/**
 * \file memory_tests.h
 * \date Jul 29, 2009
 * \author afomin
 * \details
 */

#ifndef MEMORY_TESTS_H_
#define MEMORY_TESTS_H_

void memory_test_run1(WORD *base_addr, long int amount);
void memory_test_run0(WORD *base_addr, long int amount);
void memory_test_address(WORD *base_addr, long int amount);
void memory_test_chess(WORD *base_addr, long int amount);
void memory_test_quick(WORD *base_addr, long int amount);


void memory_test_loop(WORD *addr, long int counter);

#endif /* MEMORY_TESTS_H_ */

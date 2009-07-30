/**
 * \file memory_tests.c
 * \date Jul 29, 2009
 * \author afomin
 * \details
 */

#include "types.h"
#include "common.h"
#include "string.h"
#include "memory_tests.h"

inline static print_error(WORD *addr, WORD expected_value) {
	TRACE("FAILED! at addr 0x%8x value 0x%8x (0x%8x expected)\n", addr, *addr,
			expected_value);
}
void memory_test_run1(WORD *base_addr, long int amount) {
	WORD *addr, end_addr = base_addr + amount;
	WORD value;

	value = 0x1;
	while (value != 0) {
		// Writing
		for (addr = base_addr; addr < end_addr; addr++) {
			*addr = value;
		}
		// Checking
		for (addr = base_addr; addr < end_addr; addr++) {
			if (*addr != value) {
				print_error(addr, value);
				return;
			}
		}
		value <<= 1;
	}
	return;
}

void memory_test_run0(WORD *base_addr, long int amount) {
	WORD *addr, end_addr = base_addr + amount;
	WORD value;

	value = 0x1;
	while (value != 0) {
		// Writing
		for (addr = base_addr; addr < end_addr; addr++) {
			*addr = ~value;
		}
		// Checking
		for (addr = base_addr; addr < end_addr; addr++) {
			if (*addr != ~value) {
				print_error(addr, ~value);
				return;
			}
		}
		value <<= 1;
	}
	return;
}

void memory_test_loop(WORD *addr, long int counter) {
	WORD value = 0x55555555;

	// Infinite loop case
	if (amount == 0) {
		while (TRUE) {
			*addr = value;
			if (*addr != value) {
				print_error(addr, value);
				return;
			}
			value = ~value;
		}
	}

	// // Finite loop case
	while (counter--) {
		*addr = value;
		if (*addr != value) {
			print_error(addr, value);
			return;
		}
		value = ~value;
	}
}


/**
 * \file testmem.c
 * \date Jul 24, 2009
 * \author afomin
 * \details
 */

#include "conio/conio.h"
#include "asm/cache.h"
#include "common.h"
#include "shell.h"
#include "testmem.h"
#include "memory_tests.h"

static char testmem_keys[] = {
#include "testmem_keys.inc"
		};

static void testmem_print_help(void) {
	printf(
#include "testmem_help.inc"
	);
}

typedef void TEST_MEM_FUNC(WORD *addr, long int amount);

int testmem_shell_handler(int argsc, char **argsv) {
	SHELL_KEY keys[MAX_SHELL_KEYS];
	int i;
	char *key_value;
	TEST_MEM_FUNC *test_mem_func;
	WORD *address = (WORD *) 0x70000000L;
	long int amount = 1000000L;
	int keys_amount = parse_arg("testmem", argsc, argsv, testmem_keys,
			sizeof(testmem_keys), keys);

	if (keys_amount <= 0) {
		testmem_print_help();
		return -1;
	}

	if (get_key('h', keys, keys_amount, &key_value)) {
		testmem_print_help();
		return 0;
	}

	// Key -t for test type
	// If not presented - setting default type
	// !! for "loop" different default value
	// !!   for amount (in this case amount is
	// !!   a counter of loop)
	if (get_key('t', keys, keys_amount, &key_value)) {
		if (strcmp(key_value, "runzero") == 0) {
			test_mem_func = &memory_test_run0;
		} else if (strcmp(key_value, "runone") == 0) {
			test_mem_func = &memory_test_run1;
		} else if (strcmp(key_value, "loop") == 0) {
			amount = 0;
			test_mem_func = &memory_test_loop;
		} else if (strcmp(key_value, "address") == 0) {
			test_mem_func = &memory_test_address;
		} else if (strcmp(key_value, "chess") == 0) {
					test_mem_func = &memory_test_chess;
		} else {
			LOG_ERROR("testmem: %s: no such test.\n", key_value);
			testmem_print_help();
			return -1;
		}
	} else {
		LOG_ERROR("testmem: test name expected.\n");
		testmem_print_help();
		return -1;
	}

	// Key -a for address
	// Can be in hex and in decimal
	if (get_key('a', keys, keys_amount, &key_value)) {
		if ((key_value == NULL) || // addr empty
				((!sscanf(key_value, "0x%x", &address)) // addr not in hex
						&& (!sscanf(key_value, "%d", (int *) &address)))) { // addr not in decimal
			LOG_ERROR("testmem: -a: address value in hex or in decimal expected.\n");
			testmem_print_help();
			return -1;
		}
		// TODO remove next
		TRACE("Address: 0x%08x\n", address);
	}

	// Key -n for number of
	//   - times, starting loop in "loop" test
	//   - bytes testing in other case
	// The same as address can be either in hex or decimal
	if (get_key('n', keys, keys_amount, &key_value)) {
		if (key_value == NULL) { // amount empty
			LOG_ERROR("testmem: -n: amount value in hex or in decimal expected.\n");
			testmem_print_help();
			return -1;
		}
		if (!sscanf(key_value, "0x%x", &amount)) { // amount not in hex
			if (!sscanf(key_value, "%d", (int *) &amount)) { // amount not in decimal
				LOG_ERROR("testmem: -n: amount value in hex or in decimal expected.\n");
				testmem_print_help();
				return -1;
			}
		}
	}

	cache_data_disable();
	TRACE("Before starting: address: 0x%08x, amount: 0x%08x\n", address, amount);
	(*test_mem_func)(address, amount);
	cache_data_enable();
	return 0;
}

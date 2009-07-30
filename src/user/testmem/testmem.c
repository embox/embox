/**
 * \file testmem.c
 * \date Jul 24, 2009
 * \author afomin
 * \details
 */

#include "types.h"
#include "conio.h"
#include "common.h"
#include "shell.h"
#include "testmem.h"

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
		mem_print_help();
		return 0;
	}

	// Key -t for test type
	// If not presented - setting default type
	// !! for "loop" different default value
	// !!   for amount (in this case amount is
	// !!   a counter of loop)
	if (get_key('t', keys, keys_amount, &key_value)) {
		switch(key_value) {
		case "runzero":
			test_mem_func = &test_memory_run0;
			break;
		case "runone":
			test_mem_func = &test_memory_run1;
			break;
		case "loop":
			amount = 0;
			test_mem_func = &test_memory_loop;
			break;
		default:
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
	}

	// Key -n for number of
	//   - times, starting loop in "loop" test
	//   - bytes testing in other case
	// The same as address can be either in hex or decimal
	if (get_key('n', keys, keys_amount, &key_value)) {
		if ((key_value == NULL) || // amount empty
				((!sscanf(key_value, "0x%x", &address)) // amount not in hex
				&& (!sscanf(key_value, "%d", (int *) &address)))) { // amount not in decimal
			LOG_ERROR("testmem: -n: amount value in hex or in decimal expected.\n");
			mem_print_help();
			return -1;
		}
	}

	cache_disable();
	(*test_mem_func)(address, amount);
	cache_enable();
	return 0;
}

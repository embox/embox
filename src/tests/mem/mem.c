/*
 * mem.c
 *
 *  Created on: 13.02.2009
 *      Author: Alexey Fomin
 */

#include "shell.h"
//#include "conio.h"
#include "types.h"
#include "mem.h"

char mem_keys[] = {
#include "mem_keys.inc"
		};

void print_memory(WORD *addr, long int amount) {
	int i = 0, j;
	addr = (WORD *) ((WORD) addr & 0xFFFFFFFC);
	while (i < amount) {
		printf("0x%08x:\t", (int) (addr + i));
		for (j = 0; j < 4; j++) {
			printf("%08x\t", *(addr + i));
			i++;
		}
		printf("\n");
	}

}

void test_memory(WORD *addr, long int amount) {
	int i = 0, percent_complited = 0;
	WORD old_value;

	printf("Memory test .");
	addr = (WORD *) ((WORD) addr & 0xFFFFFFFC);
	while (i++ < amount) {
		old_value = *addr;
		*addr = 0x12345678;
		if (0x12345678 != *addr) {
			printf(" FALSE!\nAt address: 0x%x", addr);
			return;
		}
		*addr = old_value;
		addr++;
		while ((i * 100 > amount * percent_complited)) {
			if (percent_complited++ % 2) {
				printf(".");
			}
		}
	}
	printf("OK\n");
}

typedef void TEST_MEM_FUNC(WORD *addr, long int amount);

int mem_shell_handler(int argsc, char **argsv) {
	SHELL_KEY keys[MAX_SHELL_KEYS];
	int i;
	char *key_value;
	TEST_MEM_FUNC *test_mem_func;
	WORD *address = (WORD *) 0x70000000L;
	long int amount = 1000000L;
	int keys_amount = parse_arg("mem", argsc, argsv, mem_keys,
			sizeof(mem_keys), keys);

	if (keys_amount < 0) {
		printf(
#include "mem_help.inc"
		);
		return -1;
	}
	if (get_key('h', keys, keys_amount, &key_value)) {
		printf(
#include "mem_help.inc"
		);
		return 0;
	}

	if (get_key('t', keys, keys_amount, &key_value)) {
		// In test mode

		address = (WORD *) 0x70000000L;
		amount = 1000000L;
		test_mem_func = &test_memory;
	} else {
		// In read memory mode

		address = 0x0;
		amount = 50;
		test_mem_func = &print_memory;
	}


	if (get_key('a', keys, keys_amount, &key_value)) {
		if ((key_value != NULL) && (!sscanf(key_value, "0x%x", &address))
				&& (!sscanf(key_value, "%d", (int *) &address))) {
			printf("ERROR: mem: hex value expected.\n");
			printf(
#include "mem_help.inc"
			);
		}
	}

	if (get_key('c', keys, keys_amount, &key_value)) {
		if ((key_value != NULL) && (!sscanf(key_value, "%d", &amount))) {
			printf("ERROR: mem: integer value expected.\n");
			printf(
#include "mem_help.inc"
			);
		}
	}

	(*test_mem_func)(address, amount);
	return 0;
}


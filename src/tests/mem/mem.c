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

void print_memory(WORD *addr, int amount) {
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

int mem_shell_handler(int argsc, char **argsv) {
	SHELL_KEY keys[MAX_SHELL_KEYS];
	int i;
	char *key_value;
	int keys_amount;
	WORD *address = 0x0;
	int amount = 50;

	keys_amount = parse_arg("mem", argsc, argsv, mem_keys, sizeof(mem_keys),
			keys);

	if (keys_amount < 0) {
		printf(
#include "mem_help.inc"
		);
		return -1;
	}
	if (get_key('a', keys, keys_amount, &key_value)) {
		if ((key_value != NULL) && (!sscanf(key_value, "0x%x", &address))
				&& (!sscanf(key_value, "%x", &address))) {
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
	printf("Addr: %x\n", address);
	print_memory(address, amount);
}


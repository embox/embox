/*
 * mem.c
 *
 *  Created on: 13.02.2009
 *      Author: Alexey Fomin
 */

#include "shell.h"
#include "mem.h"

char mem_keys[] = {
#include "mem.inc"
		};

void print_memory(int *addr, int amount) {
	int i = 0, j;
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
	SHELL_KEY arguments[MAX_SHELL_KEYS];
	int i;
	char *key_value;
	int keys_amount;

	keys_amount = parse_arg("mem", argsc, argsv, mem_keys, sizeof(mem_keys), arguments);


	if (keys_amount < 0) {
		return -1;
	}

	if (get_key('d', arguments, sizeof(arguments), &key_value)) {
		print_memory(0, 50);
	}
}

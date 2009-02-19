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
		printf("0x%08x:\t", (int)(addr+i));
		for (j = 0; j < 4; j++) {
			printf("%08x\t", *(addr+i));
			i++;
		}
		printf("\n");
	}

}

int mem_shell_handler(int argsc, char **argsv) {
	SHELL_KEY arguments[MAX_SHELL_KEYS];
	int keys_amount = parse_arg(argsc, argsv, arguments);
	int i;
	char *key_value;

	if (!check_keys(arguments, keys_amount, mem_keys, sizeof(mem_keys))) {
		printf("ERROR: mem: incorrect key entered! See help.");
		return -1;
	}

	//	for (i = 0; i < keys_amount; i++) {
//		printf("\t%c - %s\n", arguments[i].name, arguments[i].value);
//	}

	if (get_key('d', arguments, sizeof(arguments), &key_value)) {
		print_memory(0, 50);
	}
}


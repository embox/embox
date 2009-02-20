/*
 * wmem.c
 *
 *  Created on: 20.02.2009
 *      Author: Alexey Fomin
 */

#include "shell.h"
//#include "conio.h"
#include "types.h"
#include "mem.h"

char wmem_keys[] = {
#include "wmem_keys.inc"
		};

static const WORD test_constant_value = 0x12345;

int wmem_shell_handler(int argsc, char **argsv) {
	SHELL_KEY keys[MAX_SHELL_KEYS];
	int i;
	char *key_value;
	int keys_amount;
	WORD *address;
	WORD new_value;
// DEBUG!!!!!
	printf("%x - %x\n", &test_constant_value, test_constant_value);

	keys_amount = parse_arg("wmem", argsc, argsv, wmem_keys, sizeof(wmem_keys),
			keys);

	if (keys_amount < 0) {
		return -1;
	}

	if ((get_key('a', keys, keys_amount, &key_value))) {
		if ((key_value != NULL) && (!sscanf(key_value, "0x%x", &address))
				&& (!sscanf(key_value, "%x", &address))) {
			printf("ERROR: wmem: hex value expected.\n");
			return -1;
		}

	} else {
		printf("ERROR: wmem: -a key expected!\n");
		return -1;
	}

	if (get_key('v', keys, keys_amount, &key_value)) {
		if ((key_value != NULL) && (!sscanf(key_value, "%x", &new_value))) {
			printf("ERROR: wmem: hex value expected.\n");
			return -1;
		}
	} else {
		printf("ERROR: wmem: -v key expected!\n");
	}

	if (address != address & 0xFFFFFFFC){
		printf("ERROR: wmem: address in wrong format (last 2 bits must be 0)\n");
		return -1;
	}
	*((WORD *) address) = new_value;
// DEBUG!!!!!
	printf("%x - %x\n", &test_constant_value, test_constant_value);
}

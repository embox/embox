/**
 * \file wmem.c
 *
 * \date 20.02.2009
 * \author Alexey Fomin
 */

#include "shell.h"
#include "conio/conio.h"
#include "asm/types.h"
#include "mem.h"

char wmem_keys[] = {
#include "wmem_keys.inc"
		};

void wmem_print_help(void) {
	printf(
#include "wmem_help.inc"
			);
}
int wmem_shell_handler(int argsc, char **argsv) {
	SHELL_KEY keys[MAX_SHELL_KEYS];
	int i;
	char *key_value;
	int keys_amount;
	WORD *address;
	WORD new_value;

	keys_amount = parse_arg("wmem", argsc, argsv, wmem_keys, sizeof(wmem_keys),
			keys);

	if (keys_amount <= 0) {
		wmem_print_help();
		return -1;
	}

	if (keys_amount == 0) {
			wmem_print_help();
			return 0;
		}

	if ((get_key('h', keys, keys_amount, &key_value))) {
		wmem_print_help();
		return 0;
	}

	if ((get_key('a', keys, keys_amount, &key_value))) {
		if ((key_value != NULL) && (!sscanf(key_value, "0x%x", &address))
				&& (!sscanf(key_value, "%d", (int *) &address))) {
			LOG_ERROR("wmem: hex value expected.\n");
			wmem_print_help();
			return -1;
		}

	} else {
		LOG_ERROR("wmem: -a key expected!\n");
		wmem_print_help();
		return -1;
	}

	if (get_key('v', keys, keys_amount, &key_value)) {
		if ((key_value != NULL) && (!sscanf(key_value, "0x%x", &new_value))
				&& (!sscanf(key_value, "%d", (int *) &new_value))) {
			LOG_ERROR("wmem: hex value expected.\n");
			wmem_print_help();
			return -1;
		}
	} else {
		LOG_ERROR("wmem: -v key expected!\n");
	}

	if (address != address & 0xFFFFFFFC) {
		LOG_ERROR("wmem: address in wrong format (last 2 bits must be 0)\n");
		wmem_print_help();
		return -1;
	}
	*((WORD *) address) = new_value;

}

/**
 * \file getopt.c
 * \date 23.09.09
 * \author Sikmir
 */
#include "conio/getopt.h"
#include "common.h"
#include "conio/conio.h"

int parse_arg(const char *handler_name, int argsc, char **argsv,
		char *available_keys, int amount_of_available_keys, SHELL_KEY *keys) {
	int i, j, args_count;

	i = 0;
	args_count = 0;

	while (i < argsc) {
		if (*argsv[i] != '-') {
			LOG_ERROR("%s: wrong condition found. Arguments not in format: -<key> [<value>]\n",
					handler_name);

			return -1;
		}

		if (args_count >= MAX_SHELL_KEYS) {
			LOG_ERROR("%s: wrong key entered. See help.\n", handler_name);
			return -2;
		}
		// Get key name.
		// Second symbol. Ex: -h -x (-hello == -h)
		keys[args_count].name = *(*(argsv + i) + 1);
		keys[args_count].value = 0x0;

		// Now working with value if one exists
		i++;
		if (i < argsc && **(argsv + i) != '-') {
			keys[args_count].value = *(argsv + i);
			i++;
		}
		args_count++;
	}

	//return args_count;

	for (i = 0; i < args_count; i++) {
		for (j = 0; j < amount_of_available_keys; j++) {
			if (keys[i].name == available_keys[j]) {
				break;
			}
		}
		if (j >= amount_of_available_keys) {
			LOG_ERROR("%s: incorrect key entered! See help.\n",
					handler_name);
			return -3;
		}
	}
	return args_count;
}

/*int check_keys(SHELL_KEY *keys, int amount, char *available_keys,
        int amount_of_available_keys) {
        int i, j;

        for (i = 0; i < amount; i++) {
                for (j = 0; j < amount_of_available_keys; j++) {
                        if (keys[i].name == available_keys[j]) {
                                break;
                        }
                }
                if (j >= amount_of_available_keys) {
                        return FALSE;
                }
        }
        return TRUE;
}*/

int get_key(char key, SHELL_KEY *keys, int keys_amount, char **value) {
        int i;
        for (i = 0; i < keys_amount; i++) {
                if (keys[i].name == key) {
                	*value = keys[i].value;
                        return TRUE;
                }
        }
        return FALSE;
}

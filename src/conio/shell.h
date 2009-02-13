/*
 * shell.h
 *
 *  Created on: 02.02.2009
 *      Author: Alexey Fomin
 */

#ifndef SHELL_H_
#define SHELL_H_

#define MAX_SHELL_KEYS 5

typedef int (*PSHELL_HANDLER)(int argsc, char **argsv);

typedef struct {
	char name;
	char *value;
} SHELL_KEY;

void shell_start();
int shell_find_commands(char *cmdline, char **proposals);

// Compare keys entered with available keys
// returns TRUE if all keys presented are available, FALSE otherwise
int check_keys(SHELL_KEY *keys, int amount, char *available_keys, int amount_of_available_keys);

// Compare keys with available
// returns TRUE if all keys presented are available, FALSE otherwise
int check_keys(SHELL_KEY *keys, int amount, char *available_keys, int amount_of_available_keys);

#endif /* SHELL_H_ */

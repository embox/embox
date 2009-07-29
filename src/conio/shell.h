/**
 * \file shell.h
 *
 * \date 02.02.2009
 * \author Alexey Fomin
 */

#ifndef SHELL_H_
#define SHELL_H_

#include "common.h"

#include "users.inc"

#define MAX_SHELL_KEYS 12
#include "cpu_context.h"

CPU_CONTEXT shell_proc_state;

typedef int (*PSHELL_HANDLER)(int argsc, char **argsv);

typedef struct {
	char name;
	char *value;
} SHELL_KEY;

typedef struct {
	char *name;
	char *description;
	PSHELL_HANDLER phandler;
} SHELL_HANDLER_DESCR;



void shell_start();
void insert_shell_handler(char* name, char* descr, PSHELL_HANDLER func);
/**
 * parse arguments array on keys-value structures and entered amount of entered keys otherwise (if everything's OK)
 * @return  -1 wrong condition found. Arguments not in format: -<key> [<value>]
 * @return  -2 too many keys entered
 * @return -3 wrong key
 * @return 0 if (if everything's OK)
 */
int parse_arg(const char *handler_name, int argsc, char **argsv,
		char *available_keys, int amount_of_available_keys, SHELL_KEY *keys);

/**
 * @return  pointer to first element in shell command list description
 */
SHELL_HANDLER_DESCR *shell_get_command_list();

/**
 * @return  quantity of shell commands
 */
int shell_size_command_list();

#endif /* SHELL_H_ */

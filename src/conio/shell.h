/*
 * shell.h
 *
 *  Created on: 02.02.2009
 *      Author: Alexey Fomin
 */

#ifndef SHELL_H_
#define SHELL_H_

#include "mem.h"
#include "wmem.h"
#include "lspnp.h"
#include "help.h"

#define MAX_SHELL_KEYS 5
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

static SHELL_HANDLER_DESCR shell_handlers[] = {
#include "shell.inc"
		};

void shell_start();
int shell_find_commands(char *cmdline, char **proposals);

// parse arguments array on keys-value structures
// RETURNS:
// -1 wrong condition found. Arguments not in format: -<key> [<value>]
// -2 too many keys entered
// -3 wrong key entered
// amount of entered keys otherwise (if everything's OK)
int parse_arg(const char *handler_name, int argsc, char **argsv,
		char *available_keys,	int amount_of_available_keys, SHELL_KEY *keys);

#endif /* SHELL_H_ */

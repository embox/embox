/**
 * @file
 *
 * @date 02.02.2009
 * @author Alexey Fomin
 */
#ifndef SHELL_H_
#define SHELL_H_

#include <embox/kernel.h>

typedef int (*PSHELL_HANDLER)(int argsc, char **argsv);

typedef struct {
	char *name;
	char *description;
	PSHELL_HANDLER phandler;
} SHELL_HANDLER_DESCR;


#if 0
void shell_start(void);
#endif
void insert_shell_handler(char* name, char* descr, PSHELL_HANDLER func);

/**
 * @return  pointer to first element in shell command list description
 */
SHELL_HANDLER_DESCR *shell_get_command_list(void);

/**
 * @return  quantity of shell commands
 */
int shell_size_command_list(void);

#endif /* SHELL_H_ */

/**
 * @file
 *
 * @date 13.09.09
 * @author Anton Bondarev
 */
#ifndef USER_HANDLER_H_
#define USER_HANDLER_H_

#include <shell.h>
#include <getopt.h>
#include <types.h>
#include <embox/kernel.h>

#define SHELL_COMMAND_NAME_MAX_LEN 0x20

typedef struct _SHELL_COMMAND_DESCRIPTOR {
	const char *name;
	int (*exec)(int argsc, char **argsv);
	const char *description;
	const char *help_message;
	const char **man_page;
} SHELL_COMMAND_DESCRIPTOR;

#define DECLARE_SHELL_COMMAND(name, exec, desc_msg, help_msg, man_page) \
    static int exec(int argsc, char **argsv); \
    static const SHELL_COMMAND_DESCRIPTOR _descriptor \
		__attribute__ ((used, section(".shell_commands"))) \
		= { name, exec, desc_msg, help_msg, &man_page }

#define show_help() printf(HELP_MSG)

#define show_man_page() printf(man_page)

/**
 * start exec shell command with pointed descriptor
 */
int shell_command_exec(SHELL_COMMAND_DESCRIPTOR *descriptor, int argsc,
		char **argsv);
#if 0
SHELL_COMMAND_DESCRIPTOR *shell_command_find_descriptor(char *name);
#endif
/**
 * iterate functions
 */
SHELL_COMMAND_DESCRIPTOR *shell_command_descriptor_find_first(char *start_str,
		int length);

SHELL_COMMAND_DESCRIPTOR *shell_command_descriptor_find_next(
		SHELL_COMMAND_DESCRIPTOR * cur_desc, char *start_str, int length);

#endif /*USER_HANDLER_H_*/

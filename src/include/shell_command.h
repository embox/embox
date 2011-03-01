/**
 * @file
 *
 * @date 13.09.09
 * @author Anton Bondarev
 */
#ifndef COMMAND_FRAMEWORK_H_
#define COMMAND_FRAMEWORK_H_

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

#define show_help() printf(HELP_MSG "\n")

#define show_man_page() printf(man_page)

/**
 * start exec shell command with pointed descriptor
 */
extern int shell_command_exec(SHELL_COMMAND_DESCRIPTOR *descriptor, int argsc,
		char **argsv);

/**
 * iterate functions
 */
extern SHELL_COMMAND_DESCRIPTOR *shell_command_descriptor_find_first(char *start_str,
		int length);

/**
 * find descriptor for shell command with pointed name each command have to
 * register own descriptor in system, by needs macros DECLARE_SHELL_COMMAND
 * that macros put pinter on registering descriptor to section ".shell_command".
 * And we can find this descriptor when we need to use it.
 *
 * @param command name
 * @return command descriptor if command was found in image
 * @return NULL another way
 */
extern SHELL_COMMAND_DESCRIPTOR *shell_command_descriptor_find_next(
		SHELL_COMMAND_DESCRIPTOR * cur_desc, char *start_str, int length);

#endif /*COMMAND_FRAMEWORK_H_*/

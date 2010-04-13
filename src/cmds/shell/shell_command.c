/**
 * @file
 *
 * @date 13 august 2009
 * @author Anton Bondarev
 * @details Realized interface for working with user command
 *          which can be call from shell
 */

#include <shell_command.h>
#include <string.h>

int shell_command_exec(SHELL_COMMAND_DESCRIPTOR *descriptor, int argsc,
		char **argsv) {
	if ((NULL != descriptor) && (NULL != descriptor->exec)) {
		return job_exec(descriptor->exec, argsc, argsv);
	}
	LOG_ERROR("Error shell command: wrong command descriptor\n");
	return -1;
}

SHELL_COMMAND_DESCRIPTOR *shell_command_descriptor_find_first(char *search_str,
		int length) {
	extern SHELL_COMMAND_DESCRIPTOR __shell_commands_start, __shell_commands_end;
	SHELL_COMMAND_DESCRIPTOR *p_desc;
	for (p_desc = &__shell_commands_start; p_desc < &__shell_commands_end; p_desc++) {
		if (NULL == (p_desc->name)) {
			LOG_ERROR("Broken shell command descriptor: can't find command name\n");
			continue;
		}
		if (length == -1) {
			if (0 == strcmp(search_str, p_desc->name)) {
				return p_desc;
			}
		} else {
			if (0 == strncmp(search_str, p_desc->name, length)) {
				return p_desc;
			}
		}
	}
	return NULL;
}

/**
 * find descriptor for shell command with pointed name
 * each command have to register own descriptor in system, by needs macros DECLARE_SHELL_COMMAND
 * that macros put pinter on registering descriptor to section ".shell_command".
 * And we can find this descriptor when we need to use it.
 *
 * @param command name
 * @return command descriptor if command was found in image
 * @return NULL another way
 */
SHELL_COMMAND_DESCRIPTOR *shell_command_descriptor_find_next(
		SHELL_COMMAND_DESCRIPTOR *cur_desc, char *search_str, int length) {
	extern SHELL_COMMAND_DESCRIPTOR __shell_commands_end;
	SHELL_COMMAND_DESCRIPTOR *p_desc;
	for (p_desc = cur_desc + 1; p_desc < &__shell_commands_end; p_desc++) {
		if (NULL == (p_desc->name)) {
			LOG_ERROR("Broken shell command descriptor: can't find command name\n");
			continue;
		}
		if (length == -1) {
			if (0 == strcmp(search_str, p_desc->name)) {
				return p_desc;
			}
		} else {
			if (0 == strncmp(search_str, p_desc->name, length)) {
				return p_desc;
			}
		}
	}
	return NULL;
}

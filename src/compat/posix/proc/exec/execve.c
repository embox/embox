/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.06.2014
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

static inline int _exec_switch_env(char *const envp[]) {
	int err;

	if (!envp) {
		return 0;
	}

	clearenv();

	while (*envp != NULL) {
		err = putenv(*envp);
		if (err) {
			return err;
		}
		envp++;
	}

	return 0;
}

int execve(const char *path, char *const argv[], char *const envp[]) {
	int err;

	err = _exec_switch_env(envp);
	if (err) {
		return err;
	}

	return execv(path, argv);
}

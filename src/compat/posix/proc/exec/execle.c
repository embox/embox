/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 01.05.23
 */

#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <unistd.h>

int execle(const char *path, const char *arg, ...) {
	char *buf[ARG_MAX];
	char **envp;
	int i;
	va_list args;

	*buf = (char *)arg;
	i = 0;

	va_start(args, arg);
	while (buf[i] != NULL) {
		if (++i == ARG_MAX) {
			return -E2BIG;
		}
		buf[i] = va_arg(args, char *);
	}
	envp = va_arg(args, char **);
	va_end(args);

	return execve(path, buf, envp);
}

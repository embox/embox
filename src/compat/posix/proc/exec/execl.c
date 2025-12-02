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
#include <sys/cdefs.h>
#include <unistd.h>

int execl(const char *path, const char *arg, ...) {
	char *buf[ARG_MAX];
	int i;
	va_list args;

	*buf = (char *)arg;
	i = 0;

	va_start(args, arg);
	while (buf[i] != NULL) {
		if (++i == ARG_MAX) {
			return -E2BIG;
		}
		buf[i] = (char *)va_arg(args, const char *);
	}
	va_end(args);

	return execv(path, buf);
}

__strong_alias(execlp, execl);

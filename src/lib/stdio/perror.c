/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    29.10.2012
 */

#include <sys/types.h>
#include <unistd.h>
#include <posix_errno.h>
#include <stdio.h>
#include <string.h>

void perror(const char *s) {
	char *err;

	if (s != NULL && *s != 0) {
		write(STDERR_FILENO, s, strlen(s));
		write(STDERR_FILENO, ": ", 2);
	}

	err =  strerror(errno);
	write(STDERR_FILENO, err, strlen(err));

	write(STDERR_FILENO, "\n", 1);
}

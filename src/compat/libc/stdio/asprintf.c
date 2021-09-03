/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    21.07.2014
 */

#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#define ASPRINTF_LEN 1024

int vasprintf(char **strp, const char *fmt, va_list ap) {
	char *str;
	*strp = NULL;

	str = malloc(ASPRINTF_LEN);
	if (str) {
		int ret;

		ret = vsnprintf(str, ASPRINTF_LEN, fmt, ap);

		*strp = str;
		if (ret >= ASPRINTF_LEN) {
			str[ret - 1] = '\n';
		}
		return ret;
	}
	return -ENOMEM;
}

int asprintf(char **strp, const char *fmt, ...) {
	int ret;
	va_list ap;

	va_start(ap, fmt);
	ret = vasprintf(strp, fmt, ap);
	va_end(ap);

	return ret;
}

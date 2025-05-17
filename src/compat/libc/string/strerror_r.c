/**
 * @file
 * @brief Implementation of #strerror_r() function.
 *
 * @date 11.05.25
 * @author Dmitry Pilyuk
 */

#include <errno.h>
#include <stddef.h>
#include <string.h>

int strerror_r(int errnum, char *strerrbuf, size_t buflen) {
	const char *err_msg;

	err_msg = strerror(errnum);

	if (strlen(err_msg) >= buflen) {
		return SET_ERRNO(ERANGE);
	}

	strcpy(strerrbuf, err_msg);

	return 0;
}

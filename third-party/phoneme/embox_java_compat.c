/**
 * @file
 * @brief
 *
 * @date 7.01.13
 * @author Alexander Kalmuk
 */

#include "embox_java_compat.h"
#include <errno.h>

/* TODO */
javacall_result emboxErrno2javaErrno(int embox_errno) {
	switch (errno) {
	case ENOERR:
		return JAVACALL_OK;
	case EBADF:
		return JAVACALL_BAD_FILE_NAME;
	case ENOMEM:
		return JAVACALL_OUT_OF_MEMORY;
	}

	return JAVACALL_OK;
}


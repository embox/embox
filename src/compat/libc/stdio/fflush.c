/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    24.11.2014
 */

#include <fcntl.h>
#include "file_struct.h"

#include <errno.h>
#include <stdio.h>

extern int libc_ob_forceflush(FILE *file);

int fflush(FILE *stream) {
	if (stream == NULL) {
		// If the argument is NULL we must flush all open output
		// streams. For now, just fail and do not attempt to
		// dereference NULL pointer.
		errno = ENOSYS;
		return EOF;
	}

	if (stream->flags & O_WRONLY) {
		libc_ob_forceflush(stream);
	}

	return 0;
}

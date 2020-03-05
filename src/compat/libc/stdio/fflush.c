/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    24.11.2014
 */

#include <fcntl.h>
#include "file_struct.h"

#include <stdio.h>

extern int libc_ob_forceflush(FILE *file);

int fflush(FILE *stream) {

	if (stream->flags & O_WRONLY) {
		libc_ob_forceflush(stream);
	}

	return 0;
}

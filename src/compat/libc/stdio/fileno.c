/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    21.11.2014
 */

#include <assert.h>
#include "file_struct.h"

#include <stdio.h>

int fileno(FILE *stream) {
	assert(stream != NULL);
	return stream->fd;
}

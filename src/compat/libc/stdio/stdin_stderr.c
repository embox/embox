/**
 * @file
 *
 * @date 06.08.09
 * @author Anton Bondarev
 */

#include <unistd.h>
#include "file_struct.h"

#include <stdio.h>
#include <fcntl.h>

/* stdin */
static FILE stdin_struct = {
	.fd = STDIN_FILENO,
	.flags = O_RDONLY,
};
FILE *stdin = &stdin_struct;

/* stderr */
static FILE stderr_struct = {
	.fd = STDERR_FILENO,
	.flags = O_WRONLY,
};
FILE *stderr = &stderr_struct;

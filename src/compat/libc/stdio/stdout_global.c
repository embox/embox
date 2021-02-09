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

/* stdout */
static char stdout_obuf[16];
static FILE stdout_struct = {
	.fd = STDOUT_FILENO,
	.flags = O_WRONLY,
	.buftype = _IOLBF,
	.obuf = stdout_obuf,
	.obuf_sz = sizeof(stdout_obuf),
};
FILE *stdout = &stdout_struct;

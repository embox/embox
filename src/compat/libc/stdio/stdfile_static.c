/**
 * @file
 *
 * @date 06.08.09
 * @author Anton Bondarev
 */

#include <unistd.h>
#include "file_struct.h"

#include <stdio.h>

static FILE stdin_struct = {
	.fd = STDIN_FILENO,
};
static FILE stdout_struct = {
	.fd = STDOUT_FILENO,
};
static FILE stderr_struct = {
	.fd = STDERR_FILENO,
};

FILE *stdin = &stdin_struct;
FILE *stdout = &stdout_struct;
FILE *stderr = &stderr_struct;


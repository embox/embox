/**
 * @file
 *
 * @date Jun 6, 2014
 * @author: Anton Bondarev
 */

#include <framework/mod/options.h>
#include <mem/misc/pool.h>
#include "file_struct.h"

#include <stdio.h>
#include <string.h>

#define FILE_QUANTITY OPTION_GET(NUMBER,file_quantity)

POOL_DEF(file_pool, FILE, FILE_QUANTITY);

FILE *stdio_file_alloc(int fd) {
	FILE *file = pool_alloc(&file_pool);

	if (!file) {
		return NULL;
	}

	memset(file, 0, sizeof(FILE));
	file->fd = fd;

	return file;
}

void stdio_file_free(FILE *file) {
	if ((file != stdin) && (file != stdout)	&& (file != stderr)) {
		pool_free(&file_pool, file);
	}
}

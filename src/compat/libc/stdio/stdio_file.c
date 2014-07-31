/**
 * @file
 *
 * @date Jun 6, 2014
 * @author: Anton Bondarev
 */

#include <stdio.h>

#include <mem/misc/pool.h>

#include <framework/mod/options.h>

#define FILE_QUANTITY OPTION_GET(NUMBER,file_quantity)


POOL_DEF(file_pool, FILE, FILE_QUANTITY);

FILE *stdio_file_alloc(int fd) {
	FILE *file = pool_alloc(&file_pool);

	if (!file) {
		return NULL;
	}

	file->fd = fd;
	file->has_ungetc = 0;
	file->readfn = NULL;
	return file;
}

void stdio_file_free(FILE *file) {
	if ((file != stdin) && (file != stdout)	&& (file != stderr)) {
		pool_free(&file_pool, file);
	}
}

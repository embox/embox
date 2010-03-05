/**
 * @file
 *
 * @date 30.06.2009
 * @author Anton Bondarev
 */
#ifndef FILE_H_
#define FILE_H_

#include <types.h>

typedef void FILE;

FILE *fopen (const char *file_name, char *mode);

size_t fwrite (const void *buf, size_t size, size_t count, FILE *file);

size_t fread (const void *buf, size_t size, size_t count, FILE *file);

void fclose (FILE *file);

#endif /*FILE_H_*/

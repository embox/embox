#ifndef FILE_H_
#define FILE_H_

#include "types.h"

void *fopen (const char *file_name, char *mode);

size_t fwrite (const void *buf, size_t size, size_t count, void *file);

size_t fread (const void *buf, size_t size, size_t count, void *file);

void fclose (void *file);

#endif /*FILE_H_*/

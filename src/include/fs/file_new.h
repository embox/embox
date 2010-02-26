/**
 * @file
 *
 * @date 28.08.2009
 * @author Roman Evstifeev
 */
#ifndef FILE_H_
#define FILE_H_

#include <file_interface.h>

typedef int FDESC;

#define FDESC_INVALID (-1)

// open flags
#define O_CREAT 1<<0
// TODO:
//#define O_TRUNC 1<<1

FDESC open (const char *file_path, int flags);

size_t write (FDESC file_desc, const void *buf, size_t nbytes);

size_t read (FDESC file_desc, void *buf, size_t nbytes);

bool fclose (FDESC file_desc);

int lseek(FDESC file_desc, long offset, int whence) ;

size_t list_dir (const char *path, FLIST_ITEM* out_flist, int max_nitems);

bool remove (const char* file_path);

bool fsync(const char* file_path);

#endif /*FILE_H_*/

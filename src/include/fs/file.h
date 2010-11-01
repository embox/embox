/**
 * @file
 *
 * @date 11.10.2010
 * @author Anton Bondarev
 */

#ifndef FILE_H_
#define FILE_H_

#include <types.h>

typedef struct file {
	const char                    *f_path;
	const struct file_operations  *f_op;
	spinlock_t                     f_lock;
	unsigned int                   f_mode;
	unsigned int                   f_pos;
} file_t;

typedef void  *(*FILEOP_OPEN)(const char *file_name, const char *mode);
typedef int    (*FILEOP_CLOSE)(void * file);
typedef size_t (*FILEOP_READ)(void *buf, size_t size, size_t count, void *file);
typedef size_t (*FILEOP_WRITE)(const void *buf, size_t size, size_t count, void *file);
typedef int    (*FILEOP_FSEEK)(void *file, long offset, int whence);
typedef int    (*FILEOP_IOCTRL)(void *file, int request, va_list args);

typedef struct file_operations {
	FILEOP_OPEN  fopen;
	FILEOP_CLOSE fclose;
	FILEOP_READ  fread;
	FILEOP_WRITE fwrite;
	FILEOP_FSEEK fseek;
	FILEOP_IOCTRL ioctrl;
} file_operations_t;

#endif /* FILE_H_ */

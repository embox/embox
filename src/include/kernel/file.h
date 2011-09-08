/**
 * @file
 * @brief Kernel file abstraction
 *
 * @date 06.09.11
 * @author Anton Kozlov
 */

#ifndef KERNEL_FILE_H_
#define KERNEL_FILE_H_

#include <stdarg.h>
#include <stdio.h> /* FILE */

typedef void  *(*FILEOP_OPEN)(const char *file_name, const char *mode);
typedef int    (*FILEOP_CLOSE)(void * file);
typedef size_t (*FILEOP_READ)(void *buf, size_t size, size_t count, void *file);
typedef size_t (*FILEOP_WRITE)(const void *buf, size_t size, size_t count, void *file);
typedef int    (*FILEOP_FSEEK)(void *file, long offset, int whence);
typedef int    (*FILEOP_IOCTL)(void *file, int request, va_list args);

typedef struct file_operations {
	FILEOP_OPEN  fopen;
	FILEOP_CLOSE fclose;
	FILEOP_READ  fread;
	FILEOP_WRITE fwrite;
	FILEOP_FSEEK fseek;
	FILEOP_IOCTL ioctl;
} file_operations_t;

extern int reopen(int fd, FILE *file);

extern int open(const char *path, const char *mode);

extern int file_close(int fd);

extern ssize_t write(int fd, const void *buf, size_t nbyte);

extern ssize_t read(int fd, void *buf, size_t nbyte);
#endif /* KERNEL_FILE_H_ */

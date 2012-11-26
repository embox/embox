/**
 * @file
 * @brief Kernel file abstraction
 *
 * @date 06.09.11
 * @author Anton Kozlov
 */

#ifndef KERNEL_FILE_H_
#define KERNEL_FILE_H_

#include <types.h>
#include <stdarg.h>

struct file_desc;
struct node;

typedef void  *(*FILEOP_OPEN)(struct file_desc *desc,  int flag);
typedef int    (*FILEOP_CLOSE)(struct file_desc *desc);
typedef size_t (*FILEOP_READ)(void *buf, size_t size, size_t count, void *file);
typedef size_t (*FILEOP_WRITE)(const void *buf, size_t size, size_t count, void *file);
typedef int    (*FILEOP_FSEEK)(void *file, long offset, int whence);
typedef int    (*FILEOP_IOCTL)(void *file, int request, va_list args);
typedef int    (*FILEOP_FSTAT)(void *file, void *buff);

typedef struct file_operations {
	FILEOP_OPEN  fopen;
	FILEOP_CLOSE fclose;
	FILEOP_READ  fread;
	FILEOP_WRITE fwrite;
	FILEOP_FSEEK fseek;
	FILEOP_IOCTL ioctl;
	FILEOP_FSTAT fstat;
} file_operations_t;

#endif /* KERNEL_FILE_H_ */

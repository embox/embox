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
#include <fs/node.h>

struct file_desc;

typedef void  *(*FILEOP_OPEN)(struct file_desc *desc,  const char *mode);
typedef int    (*FILEOP_CLOSE)(struct file_desc *desc);
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

typedef struct file_create_param {
	void  *node;
	void  *parents_node;
	char   path[CONFIG_MAX_LENGTH_FILE_NAME];
} file_create_param_t;

extern int nip_tail(char *head, char *tail);
extern int increase_tail(char *head, char *tail);

#endif /* KERNEL_FILE_H_ */

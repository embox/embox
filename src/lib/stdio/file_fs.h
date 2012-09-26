/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.09.2012
 */

#ifndef LIB_STDIO_FILE_FS_H_
#define LIB_STDIO_FILE_FS_H_

#include <stddef.h>
#include <stdarg.h>
#include <fs/core.h>

#include <framework/mod/options.h>
#include <module/embox/fs/file_desc.h>

#define __FILE_QUANTITY OPTION_MODULE_GET(embox__fs__file_desc,NUMBER,fdesc_quantity)

struct file_struct_int {
	struct file_desc *desc;
};

extern struct file_desc stdin_int_struct;
extern struct file_desc stdout_int_struct;
extern struct file_desc stderr_int_struct;

#define INIT_STDIO(_nm) { .desc = & _nm ## _int_struct }
#define INIT_STDIN INIT_STDIO(stdin)
#define INIT_STDOUT INIT_STDIO(stdout)
#define INIT_STDERR INIT_STDIO(stderr)

static inline int __libc_open(const char *path, const char *mode, struct file_struct_int *file) {
	file->desc = kopen(path, mode);
	return (file->desc == NULL ? -1 : 0);
}

static inline size_t __libc_write(const void *buf, size_t size, size_t count, struct file_struct_int *file) {
	return kwrite(buf, size, count, file->desc);
}

static inline size_t __libc_read(void *buf, size_t size, size_t count, struct file_struct_int *file) {
	return kread(buf, size, count, file->desc);
}

static inline int __libc_close(struct file_struct_int *file) {
	return kclose(file->desc);
}

static inline int __libc_lseek(struct file_struct_int *file, long int offset, int origin) {
	return kseek(file->desc, offset, origin);
}

static inline int __libc_ioctl(struct file_struct_int *file, int request, va_list args) {
	return kioctl(file->desc, request, args);
}

#endif /* LIB_STDIO_FILE_FS_H_ */

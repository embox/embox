/**
 * @file
 *
 * @date 11.10.10
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
typedef int    (*FILEOP_IOCTL)(void *file, int request, va_list args);

typedef struct file_operations {
	FILEOP_OPEN  fopen;
	FILEOP_CLOSE fclose;
	FILEOP_READ  fread;
	FILEOP_WRITE fwrite;
	FILEOP_FSEEK fseek;
	FILEOP_IOCTL ioctl;
} file_operations_t;

typedef struct stat {
	int       st_dev;     /* ID of device containing file */
	int       st_ino;     /* inode number */
	int       st_mode;    /* protection */
	int       st_nlink;   /* number of hard links */
	int       st_uid;     /* user ID of owner */
	int       st_gid;     /* group ID of owner */
	int       st_rdev;    /* device ID (if special file) */
	size_t    st_size;    /* total size, in bytes */
	size_t    st_blksize; /* blocksize for file system I/O */
	int       st_blocks;  /* number of 512B blocks allocated */
	unsigned  st_atime;   /* time of last access */
	unsigned  st_mtime;   /* time of last modification */
	unsigned  st_ctime;   /* time of last status change */
} stat_t;

#endif /* FILE_H_ */

/**
 * @file
 *
 * @date 11.10.10
 * @author Anton Bondarev
 */

#ifndef FS_FILE_H_
#define FS_FILE_H_

#include <types.h>
#include <stdio.h>
#include <kernel/file.h>

typedef struct file {
	const char                    *f_path;
	const struct file_operations  *f_op;
	spinlock_t                     f_lock;
	unsigned int                   f_mode;
	unsigned int                   f_pos;
} file_t;

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

typedef struct lsof_map {
	struct list_head *next;
	struct list_head *prev;
	const char        path[CONFIG_MAX_LENGTH_FILE_NAME];
	FILE             *fd;
} lsof_map_t;

//extern void lsof_map_init(void);

#endif /* FS_FILE_H_ */

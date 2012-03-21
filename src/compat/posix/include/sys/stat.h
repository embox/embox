/**
 * @file
 *
 * @brief
 *
 * @date 07.09.2011
 * @author Anton Bondarev
 */

#ifndef SYS_STAT_H_
#define SYS_STAT_H_

#include <sys/types.h>

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

typedef unsigned int mode_t;

/**
 * Get file status (size, mode, mtime and so on)
 */
extern int fstat(const char *path, struct stat *buf);

extern int mkdir (const char *path, mode_t __mode);

extern int    chmod(const char *, mode_t);
extern int    fchmod(int, mode_t);
extern int    lstat(const char *, struct stat *);
extern int    mkfifo(const char *, mode_t);
extern int    mknod(const char *, mode_t, dev_t);
extern int    stat(const char *, struct stat *);
extern mode_t umask(mode_t);

#endif /* SYS_STAT_H_ */

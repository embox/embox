/**
 * @file
 *
 * @brief
 *
 * @date 07.09.2011
 * @author Anton Bondarev
 */

#ifndef FCNTL_H_
#define FCNTL_H_

#include <sys/stat.h>
#include <sys/cdefs.h>

#include <sys/stat.h>
#include <stdarg.h>

__BEGIN_DECLS

/* int open (const char *path, int __oflag);
 * int open (const char *path, int __oflag, mode_t mode);
 */
extern int open(const char *path, int __oflag, ...);

/*
 * shall be equivalent to: open(path, O_WRONLY|O_CREAT|O_TRUNC, mode)
 */
extern int creat(const char *pathname, mode_t mode);

extern int fcntl(int fd, int cmd, ...);

/* _ATFILE_SOURCE */
extern int openat(int dirfd, const char *pathname, int flags, ...);

/* fcntl commands */
#define F_GETFD            0
#define F_SETFD            1
#define F_GETPIPE_SZ       2
#define F_SETPIPE_SZ       3

#define F_GETLK            4 /* Get record locking information. */
#define F_SETLK            5 /* Set record locking information. */
#define F_SETLKW           6 /* Set record locking information; wait if blocked. */

/* Values for l_type used for record locking with fcntl() (the following values are unique) are as follows: */
#define F_RDLCK            7 /* Shared or read lock. */
#define F_UNLCK            8 /* Unlock. */
#define F_WRLCK            9 /* Exclusive or write lock. */

#define F_GETFL            10
#define F_SETFL            11
#define F_DUPFD            12
#define F_DUPFD_CLOEXEC    13

/* fcntl flags */
#define O_ACCESS_MASK      (O_RDONLY|O_WRONLY|O_RDWR)
#define O_ACCMODE          O_ACCESS_MASK
#define O_RDONLY           0x0000  /* Open for reading only */
#define O_WRONLY           0x0001  /* Open for writing only */
#define O_RDWR             0x0002  /* Open for reading and writing */
#define O_SPECIAL          0x0004  /* Open for special access */
#define O_APPEND           0x0008  /* Writes done at EOF */

#define O_CREAT            0x0100  /* Create and open file */
#define O_TRUNC            0x0200  /* Truncate file */
#define O_EXCL             0x0400  /* Open only if file doesn't already exist */
#define O_DIRECT           0x0800  /* Do not use cache for reads and writes */
#define O_NONBLOCK         0x1000  /* Non-blocking mode */
/* Write I/O operations on the file descriptor shall complete
 * as defined by synchronized I/O file integrity completion. */
#define O_SYNC             0x04000000

#define O_NOCTTY		   0x10000 	/*  */
#define O_NDELAY		   0x01000	/*	*/

#define O_DIRECTORY        0x2000  /* Fail if not a directory. */

/* Open directory for search only.
 * The result is unspecified if this flag is applied to a non-directory file.
 */
#define O_SEARCH           0x4000
#define O_PATH             O_SEARCH

/* If path names a symbolic link, fail and set errno to [ELOOP] */
#define O_NOFOLLOW         0x8000

/* file descriptor flags */
#define FD_CLOEXEC         0x0010
/* TODO not POSIX */
#define O_CLOEXEC          FD_CLOEXEC


struct flock {
	short  l_type;   /* Type of lock; F_RDLCK, F_WRLCK, F_UNLCK. */
	short  l_whence; /* Flag for starting offset. */
	off_t  l_start;  /* Relative offset in bytes. */
	off_t  l_len;    /* Size; if 0 then until EOF. */
	pid_t  l_pid;    /* Process ID of the process holding the lock; returned with F_GETLK. */
};

__END_DECLS

#endif /* FCNTL_H_ */

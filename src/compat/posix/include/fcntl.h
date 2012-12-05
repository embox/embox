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

/* int open (const char *path, int __oflag);
 * int open (const char *path, int __oflag, mode_t mode);
 */
extern int open(const char *path, int __oflag, ...);

extern int close(int file);

/*
 * shall be equivalent to: open(path, O_WRONLY|O_CREAT|O_TRUNC, mode)
 */
extern int creat(const char *pathname, mode_t mode);

extern int fcntl(int fd, int cmd, ...);

/* fcntl commands */
#define F_GETFD            0
#define F_SETFD            1
#define F_GETPIPE_SZ       2
#define F_SETPIPE_SZ       3

#define F_GETFL     F_GETFD
#define F_SETFL     F_SETFD

/* fcntl flags */
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

#endif /* FCNTL_H_ */

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
extern int open (const char *path, int __oflag, ...);

extern int close(int file);

extern int creat(const char *pathname, mode_t mode);

#define O_RDONLY           0
#define O_WRONLY           1
#define O_RDWR             2

#define O_CREAT        00100
#define O_TRUNC        01000

#endif /* FCNTL_H_ */

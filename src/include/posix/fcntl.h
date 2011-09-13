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

#if 0
extern int open (const char *path, int __oflag, ...);
#else
extern int open(const char *path, const char *mode);
#endif

extern int close(int file);

#define O_RDONLY           0
#define O_WRONLY           1
#define O_RDWR             2

#define O_CREAT        00100
#define O_TRUNC        01000

#endif /* FCNTL_H_ */

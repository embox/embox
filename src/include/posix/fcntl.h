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

#endif /* FCNTL_H_ */

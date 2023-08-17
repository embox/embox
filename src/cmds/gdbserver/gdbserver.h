/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 14.08.23
 */
#ifndef GDBSERVER_H_
#define GDBSERVER_H_

#include <sys/types.h>

extern void gdbserver_start(int fd, void *entry);
extern void gdbserver_stop(void);

extern ssize_t gdbserver_read(int fd, char *dst, size_t nbyte);
extern ssize_t gdbserver_write(int fd, const char *src, size_t nbyte);
extern int gdbserver_prepare_connection(const char *arg);

#endif /* GDBSERVER_H_ */

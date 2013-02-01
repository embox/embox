/**
 * @file
 * @brief POSIX Standard: 2.10 Symbolic Constants.
 * @details This file defines miscellaneous symbolic constants and types,
 *        and declares miscellaneous function. Now only sleep functions.
 *
 * @date 24.11.09
 * @author Anton Bondarev
 */

#ifndef UNISTD_H_
#define UNISTD_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

/* Standard file descriptors. */
#define STDIN_FILENO    0   /* Standard input. */
#define STDOUT_FILENO   1   /* Standard output. */
#define STDERR_FILENO   2   /* Standard error output. */

/* Values for the WHENCE argument to lseek. */
#define SEEK_SET        0   /* Seek from beginning of file. */
#define SEEK_CUR        1   /* Seek from current position. */
#define SEEK_END        2   /* Seek from end of file. */

/**
 * suspend execution for an interval of time
 * @param seconds interval in seconds
 * @return 0 if time has elapsed if was error then return -1
 */
extern int sleep(unsigned int seconds);

/**
 * suspend execution for an interval of time
 * @param useconds interval in nanoseconds,microseconds,milliseconds
 * @return 0 if time has elapsed if was error then return -1
 */
extern int nsleep(useconds_t nseconds);
extern int usleep(useconds_t useconds);
extern int msleep(useconds_t mseconds);

extern ssize_t write(int fd, const void *buf, size_t nbyte);

extern ssize_t read(int fd, void *buf, size_t nbyte);

extern int lseek(int fd, long int offset, int origin);

extern long int ltell(int fd);

extern int fsync(int);

extern pid_t fork(void);

extern int execv(const char *path, char *const argv[]);
extern int execve(const char *filename, char *const argv[], char *const envp[]);

extern int unlink(const char *pathname);

extern int rmdir(const char *pathname);

static inline pid_t getpid(void) {
	return -1;
}

extern int isatty(int fd);

extern int dup(int flides);
extern int dup2(int flides, int flides2);

extern int pipe(int pipefd[2]/*, int flags: O_NONBLOCK */);
extern void _exit(int status);

__END_DECLS

#endif /* UNISTD_H_ */

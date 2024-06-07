/**
 * @file
 * @brief POSIX Standard: 2.10 Symbolic Constants.
 * @details This file defines miscellaneous symbolic constants and types,
 *        and declares miscellaneous function. Now only sleep functions.
 *
 * @date 24.11.09
 * @author Anton Bondarev
 */

#ifndef COMPAT_POSIX_UNISTD_H_
#define COMPAT_POSIX_UNISTD_H_

#include <compiler.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/cdefs.h>

#include <sys/uio.h>

#include <posix_environ.h>

/*
 * The following symbolic constants are defined for sysconf():
 */
#define _SC_PAGESIZE          1
#define _SC_PAGE_SIZE         _SC_PAGESIZE
#define _SC_CLK_TCK           2
#define _SC_GETPW_R_SIZE_MAX  3
#define _SC_ATEXIT_MAX        4
/*not posix */
#define _SC_NPROCESSORS_ONLN  103
#define _SC_NPROCESSORS_CONF  _SC_NPROCESSORS_ONLN
#define _SC_PHYS_PAGES        104

#define _SC_OPEN_MAX          20

/*
_SC_2_C_BIND
_SC_2_C_DEV
_SC_2_C_VERSION
_SC_2_FORT_DEV
_SC_2_FORT_RUN
_SC_2_LOCALEDEF
_SC_2_SW_DEV
_SC_2_UPE
_SC_2_VERSION
_SC_ARG_MAX
_SC_AIO_LISTIO_MAX
_SC_AIO_MAX
_SC_AIO_PRIO_DELTA_MAX
_SC_ASYNCHRONOUS_IO
_SC_BC_BASE_MAX
_SC_BC_DIM_MAX
_SC_BC_SCALE_MAX
_SC_BC_STRING_MAX
_SC_CHILD_MAX
_SC_COLL_WEIGHTS_MAX
_SC_DELAYTIMER_MAX
_SC_EXPR_NEST_MAX
_SC_FSYNC
_SC_GETGR_R_SIZE_MAX
_SC_GETPW_R_SIZE_MAX
_SC_IOV_MAX
_SC_JOB_CONTROL
_SC_LINE_MAX
_SC_LOGIN_NAME_MAX
_SC_MAPPED_FILES
_SC_MEMLOCK
_SC_MEMLOCK_RANGE
_SC_MEMORY_PROTECTION
_SC_MESSAGE_PASSING
_SC_MQ_OPEN_MAX
_SC_MQ_PRIO_MAX
_SC_NGROUPS_MAX
_SC_OPEN_MAX
_SC_PAGE_SIZE
_SC_PASS_MAX (LEGACY)
_SC_PRIORITIZED_IO
_SC_PRIORITY_SCHEDULING
_SC_RE_DUP_MAX


_SC_REALTIME_SIGNALS
_SC_RTSIG_MAX
_SC_SAVED_IDS
_SC_SEMAPHORES
_SC_SEM_NSEMS_MAX
_SC_SEM_VALUE_MAX
_SC_SHARED_MEMORY_OBJECTS
_SC_SIGQUEUE_MAX
_SC_STREAM_MAX
_SC_SYNCHRONIZED_IO
_SC_THREADS
_SC_THREAD_ATTR_STACKADDR
_SC_THREAD_ATTR_STACKSIZE
_SC_THREAD_DESTRUCTOR_ITERATIONS
_SC_THREAD_KEYS_MAX
_SC_THREAD_PRIORITY_SCHEDULING
_SC_THREAD_PRIO_INHERIT
_SC_THREAD_PRIO_PROTECT
_SC_THREAD_PROCESS_SHARED
_SC_THREAD_SAFE_FUNCTIONS
_SC_THREAD_STACK_MIN
_SC_THREAD_THREADS_MAX
_SC_TIMERS
_SC_TIMER_MAX
_SC_TTY_NAME_MAX
_SC_TZNAME_MAX
_SC_VERSION
_SC_XOPEN_VERSION
_SC_XOPEN_CRYPT
_SC_XOPEN_ENH_I18N
_SC_XOPEN_SHM
_SC_XOPEN_UNIX
_SC_XOPEN_XCU_VERSION
_SC_XOPEN_LEGACY
_SC_XOPEN_REALTIME
_SC_XOPEN_REALTIME_THREADS
_SC_XBS5_ILP32_OFF32
_SC_XBS5_ILP32_OFFBIG
_SC_XBS5_LP64_OFF64
_SC_XBS5_LPBIG_OFFBIG
*/



/* Standard file descriptors. */
#define STDIN_FILENO    0   /* Standard input. */
#define STDOUT_FILENO   1   /* Standard output. */
#define STDERR_FILENO   2   /* Standard error output. */

/* Values for the WHENCE argument to lseek. */
#define SEEK_SET        0   /* Seek from beginning of file. */
#define SEEK_CUR        1   /* Seek from current position. */
#define SEEK_END        2   /* Seek from end of file. */


__BEGIN_DECLS
/**
 * suspend execution for an interval of time
 * @param seconds interval in seconds
 * @return 0 if time has elapsed if was error then return -1
 */
extern int sleep(unsigned int seconds);

/**
 * suspend execution for an interval of time
 * @param useconds interval in microseconds
 * @return 0 if time has elapsed if was error then return -1
 */
extern int usleep(useconds_t useconds);

extern ssize_t write(int fd, const void *buf, size_t nbyte);

extern ssize_t read(int fd, void *buf, size_t nbyte);

extern ssize_t pwrite(int fd, const void *buf, size_t nbyte, off_t offset);

extern ssize_t pread(int fildes, void *buf, size_t nbyte, off_t offset);

extern ssize_t writev(int fd, const struct iovec *iov, int iovcnt);

extern ssize_t readv(int fd, const struct iovec *iov, int iovcnt);

extern off_t lseek(int fd, off_t offset, int origin);

extern int close(int fd);

extern int fsync(int);

extern pid_t fork(void);
extern pid_t vfork(void);

extern int daemon(int nochdir , int noclose); 

extern int nice(int incr);

extern int execv(const char *path, char *const argv[]);
extern int execve(const char *filename, char *const argv[], char *const envp[]);
extern int execvp(const char *file, char *const argv[]);
extern int execl(const char *path, const char *arg, ...);

// extern int link(const char *oldpath, const char *newpath);
// extern ssize_t readlink(const char *path, char *buf,
// 		       size_t bufsize);
extern ssize_t readlinkat(int fd, const char *path,
		       char *buf, size_t bufsize);

extern int unlink(const char *pathname);

extern int rmdir(const char *pathname);

extern pid_t getpid(void);

extern int isatty(int fd);

extern int dup(int flides);
extern int dup2(int flides, int flides2);

extern int pipe(int pipefd[2]);
extern int pipe2(int pipefd[2], int flags);

extern void _NORETURN _exit (int status);

extern uid_t getuid(void);
extern uid_t geteuid(void);

extern uid_t getgid(void);
extern uid_t getegid(void);

extern int setreuid(uid_t ruid, uid_t euid);
extern int setuid(uid_t uid);
extern int seteuid(uid_t uid);

extern int setregid(gid_t rgid, gid_t egid);
extern int setgid(gid_t gid);
extern int setegid(gid_t gid);

/**
 * @brief Change length of regular file
 *
 * @param path Path of the regular file
 * @param length New length
 *
 * @return Negative error code or 0 if succeed
 *
 * @note Currently unsupported errors:
 *    EACCESS
 *    EFAULT
 *    EINTR
 *    EIO
 *    ENAMETOOLONG
 *    ENOTDIR
 *    EROFS
 **/
extern int truncate(const char *path, off_t length);
extern int ftruncate(int fd, off_t length);

extern char * getcwd(char *buff, size_t size);
extern int chdir(const char *path);

extern int getpagesize(void);

extern long int sysconf(int name);

#define R_OK 4
#define W_OK 2
#define X_OK 1
#define F_OK 0
static inline int access(const char *path, int amode) {
	(void)path; (void)amode;
	return 0;
}

extern void swab(const void *bfrom, void *bto, ssize_t n);

/**
 * @param argc is the number of arguments on cmdline
 * @param argv is the pointer to array of cmdline arguments
 * @param opts is the string of all valid options
 * each char case must be given; options taking an arg are followed by = ':'
 */
extern int getopt(int argc, char *const argv[], const char *opts);

extern char *optarg; /**< argument to optopt */
extern int optind;   /**< last touched cmdline argument */
extern int optopt;   /**< last returned option */
extern int opterr;   /**< flag:error message on unrecognzed options */

#ifndef environ
/**
 * FIXME environ MUST have follow declaration:
 * but then how to do it different for all tasks
 */
extern char **environ;
#endif

#define PASS_MAX 32
extern char *getpass(const char *prompt);

/**
 * @brief Non posix extension of deprecated @a getpass that not use static buffers and
 * therefore thread-safe.
 *
 * @param prompt Prompt to be printed to user
 * @param buf Buffer to store user's input
 * @param buflen @a buf length
 *
 * @return
 * 	NULL on error
 * 	pointer to user entered password
 */
extern char *getpass_r(const char *prompt, char *buf, size_t buflen);

extern int gethostname(char *name, size_t len);

extern int chown(const char *path, uid_t owner, gid_t group);


/*******************************************
 * stubs
 *******************************************/
static inline void sync(void) {
}


extern unsigned alarm(unsigned seconds);

__END_DECLS

#endif /* COMPAT_POSIX_UNISTD_H_ */

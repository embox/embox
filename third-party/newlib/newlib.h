/**
 * @date May 3, 2010
 * @author Aleksandr Kirov
 */

#include <errno.h>
//#include <sys/stat.h>
#undef errno
extern int errno;

/**
 *The examples in this chapter include this treatment of errno.
 *
 */

/**
 * Exit a program without cleaning up files. If your system doesn't provide
 *  this, it is best to avoid linking with subroutines
 *   that require it (exit, system).
 */
_exit

/**
 * Close a file. Minimal implementation:
 */
int close(int file);

/**
 * environ - A pointer to a list of environment variables and their values.
 *  For a minimal environment, this empty list is adequate:
 */
char *__env[1] = { 0 };
char **environ = __env;

/**
 * Transfer control to a new process. Minimal implementation
 * (for a system without processes):
 */
int execve(char *name, char **argv, char **env);

/**
 * Create a new process. Minimal implementation
 * (for a system without processes):
 */
int fork(void);

/**
 * Status of an open file. For consistency with other minimal
 * implementations in these examples, all files are regarded
 * as character special devices. The sys/stat.h header file
 * required is distributed in the include subdirectory for
 * this C library.
 */
int fstat(int file, struct stat *st);

/**
 * Process-ID; this is sometimes used to generate strings unlikely
 * to conflict with other processes. Minimal implementation,
 * for a system without processes:
 */
int getpid(void);

/**
 * Query whether output stream is a terminal.
 * For consistency with the other minimal implementations,
 * which only support output to stdout,
 * this minimal implementation is suggested:
 */
int isatty(int file);

/**
 * Kill - Send a signal. Minimal implementation:
 */
int kill(int pid, int sig);

/**
 * link - Establish a new name for an existing file. Minimal implementation:
 */
int link(char *old, char *new);

/**
 * lseek - Set position in a file. Minimal implementation:
 */
int lseek(int file, int ptr, int dir);

/**
 * open - Open a file. Minimal implementation:
 */
int open(const char *name, int flags, int mode);

/**
 * read - Read from a file. Minimal implementation:
 */
int read(int file, char *ptr, int len);

/**
 * sbrk - Increase program data space. As malloc and related
 * functions depend on this, it is useful to have a working
 * implementation. The following suffices for a standalone
 * system; it exploits the symbol _end automatically defined by the GNU linker.
 */
caddr_t sbrk(int incr);

/**
 * stat - Status of a file (by name). Minimal implementation:
 */
int stat(char *file, struct stat *st);

/**
 * times - Timing information for current process. Minimal implementation:
 */
int times(struct tms *buf);

/**
 * unlink - Remove a file's directory entry. Minimal implementation:
 */
int unlink(char *name);

/**
 * Wait for a child process. Minimal implementation:
 */
int wait(int *status);

/**
 * write to a file. libc subroutines will use this system routine
 * for output to all files, including stdout—so if you need to generate
 * any output, for example to a serial port for debugging, you should
 * make your minimal write capable of doing this. The following minimal
 * implementation is an incomplete example; it relies on a outbyte subroutine
 * (not shown; typically, you must write this in assembler from examples
 *  provided by your hardware manufacturer) to actually perform the output.
 *
 */
int write(int file, char *ptr, int len);


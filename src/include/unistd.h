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

#define _SC_TIMER_MAX 20 /**<system timers quantity */

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
 * @param useconds interval in microseconds
 * @return 0 if time has elapsed if was error then return -1
 */
extern int usleep(useconds_t useconds);

#endif /* UNISTD_H_ */

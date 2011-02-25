/**
 * @file
 * @brief This file defines miscellaneous symbolic constants and types,
 *        and declares miscellaneous function. Now only sleep functions.
 *
 * @date 24.11.2009
 * @author Anton Bondarev
 */
#ifndef UNISTD_H_
#define UNISTD_H_

#include <sys/types.h>

#define _SC_TIMER_MAX 4 /**<system timers quantity */

/**
 * suspend execution for an interval of time
 * @param seconds interval in seconds
 * @return 0 if time has elapsed if was error then return -1
 */
extern unsigned int sleep(unsigned int seconds);

/**
 * suspend execution for an interval of time
 * @param useconds interval in microseconds
 * @return 0 if time has elapsed if was error then return -1
 */
extern int usleep(useconds_t useconds);

#endif /* UNISTD_H_ */

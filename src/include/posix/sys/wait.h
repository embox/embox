/**
 * @file
 *
 * @brief
 *
 * @date 13.09.2011
 * @author Anton Bondarev
 */

#ifndef SYS_WAIT_H_
#define SYS_WAIT_H_

#include <sys/types.h>

extern pid_t waitpid(pid_t pid, int *status, int options);

#endif /* SYS_WAIT_H_ */

/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    09.06.2014
 */

#ifndef POSIX_FS_GETUMASK_H_
#define POSIX_FS_GETUMASK_H_

#include <sys/types.h>

extern mode_t getumask(void);

extern mode_t umask_modify(mode_t newmode);

#endif /* POSIX_FS_GETUMASK_H_ */

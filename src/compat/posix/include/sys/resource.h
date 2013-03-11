/**
 * @file
 * @brief
 *
 * @date 11.03.13
 * @author Ilia Vaprol
 */

#ifndef COMPAT_POSIX_SYS_RESOURCE_H_
#define COMPAT_POSIX_SYS_RESOURCE_H_

#include <sys/types.h>

#define PRIO_PROCESS 0
#define PRIO_PGRP    1
#define PRIO_USER    2

int getpriority(int which, id_t who);
int setpriority(int which, id_t who, int value);

#endif /* COMPAT_POSIX_SYS_RESOURCE_H_ */

/**
 * @file
 * @brief
 *
 * @date 25.07.2013
 * @author Andrey Gazukin
 */

#ifndef COMPAT_LINUX_INCLUDE_SYS_WATCHDOG_H_
#define COMPAT_LINUX_INCLUDE_SYS_WATCHDOG_H_

#include <sys/ioctl.h>

#define	WATCHDOG_IOCTL_BASE	'W'

#define	WDIOC_SETTIMEOUT        _IOWR(WATCHDOG_IOCTL_BASE, 6, int)
#define	WDIOC_GETTIMEOUT        _IOR(WATCHDOG_IOCTL_BASE, 7, int)

#endif /* COMPAT_LINUX_INCLUDE_SYS_WATCHDOG_H_ */

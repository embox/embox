/**
 * @file
 *
 * @data 16.12.2017
 * @author: Anton Bondarev
 */

#ifndef COMPAT_POSIX_SYS_PARAM_H_
#define COMPAT_POSIX_SYS_PARAM_H_

#include <hal/clock.h>

# undef HZ
# define HZ		(clock_freq())	/* Internal kernel timer frequency */
# define USER_HZ	100		/* some user interfaces are */

#endif /* COMPAT_POSIX_SYS_PARAM_H_ */

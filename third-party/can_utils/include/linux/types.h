/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 11.06.26
 */

#ifndef COMPAT_LINUX_STDDEF_H_
#define COMPAT_LINUX_STDDEF_H_

#include_next <linux/types.h>
#include <sys/types.h>

typedef clockid_t __kernel_clockid_t;

#endif /* COMPAT_LINUX_STDDEF_H_ */

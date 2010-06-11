/**
 * @file
 * @brief contains some often-used function prototypes etc
 *
 * @date 25.03.2010
 */

#ifndef EMBOX_KERNEL_H_
#define EMBOX_KERNEL_H_

#ifdef __EMBOX__
#include <err.h>
#else
#include <linux/types.h>
#endif

#define ARRAY_SIZE(arr)                 (sizeof(arr) / sizeof((arr)[0]))
/* deprecated */
#define array_len(array)                ARRAY_SIZE(array)

#endif /* EMBOX_KERNEL_H_ */

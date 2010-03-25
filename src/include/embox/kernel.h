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

#define array_len(array)		(sizeof(array) / sizeof(array[0]))

#endif /* EMBOX_KERNEL_H_ */

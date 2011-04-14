/**
 * @file
 * @brief contains some often-used function prototypes etc
 *
 * @date 13.03.2010
 * @author Eldar Abusalimov
 */

#ifndef EMBOX_KERNEL_H_
#define EMBOX_KERNEL_H_

/**
 * @mainpage
 *
 * This manual documents the Embox C API.
 *
 * There are several other places to look for Embox information, such
 * as the wiki; those can be found at <a href="http://code.google.com/p/embox">
 * the Embox website</a>.
 */

#ifdef __EMBOX__
#include <err.h>
#else
#include <linux/types.h>
#endif

#include <util/array.h>

#endif /* EMBOX_KERNEL_H_ */

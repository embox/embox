/**
 * @file
 * @brief
 *
 * @date 03.11.10
 * @author Anton Bondarev
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include <stddef.h>

#define MAX_CDEV_QUANTITY 0

#ifdef __GNUC__
/* Avoid warning for unused parameters with ((unused)) attribute */
#define CHAR_DEV_DEF(chname, idesc_op, priv)                                 \
	__attribute__((unused)) static struct {                                  \
		void *i, *p;                                                         \
	} unused##__LINE__ = {                                                   \
	    .i = (void *)idesc_op,                                               \
	    .p = priv,                                                           \
	};                                                                       \
	const struct dev_module *const MACRO_CONCAT(__char_device_registry_ptr_, \
	    chname) = NULL

#else
#define CHAR_DEV_DEF(chname, idesc_op, priv)                                 \
	const struct dev_module *const MACRO_CONCAT(__char_device_registry_ptr_, \
	    chname) = NULL

#endif /* __GNUC__ */

#endif /* DEVICE_H_ */

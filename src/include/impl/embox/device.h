/**
 * @file
 *
 * @brief Implements EMBOX_DEVICE macros for registering devices as files.
 *
 * @date 03.11.2010
 * @author Anton Bondarev
 */

#ifndef IMPL_DEVICE_H_
#define IMPL_DEVICE_H_

#include <util/array.h>

extern const device_module_t __device_registry[];

#define __EMBOX_DEVICE(name, file_op) \
	ARRAY_DIFFUSE_ADD(__device_registry, {name, file_op})


#endif /* IMPL_DEVICE_H_ */

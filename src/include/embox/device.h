/**
 * @file
 * @brief
 *
 * @date 03.11.10
 * @author Anton Bondarev
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include <fs/file.h>

#include <util/array.h>

typedef struct device_module {
	const char * name;
	const struct file_operations *fops;
} device_module_t;

extern const device_module_t __device_registry[];

#define EMBOX_DEVICE(name, file_op) \
	ARRAY_SPREAD_ADD(__device_registry, {name, file_op})

#endif /* DEVICE_H_ */

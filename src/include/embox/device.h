/**
 * @file
 * @brief
 *
 * @date 03.11.2010
 * @author Anton Bondarev
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include <fs/file.h>

typedef struct device_module {
	const char * name;
	const struct file_operations *fops;
} device_module_t;

#include __impl(embox/device.h)

#define EMBOX_DEVICE(name, file_op) __EMBOX_DEVICE(name, file_op)

#endif /* DEVICE_H_ */

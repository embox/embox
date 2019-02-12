/**
 * @file
 * @brief
 *
 * @date 03.11.10
 * @author Anton Bondarev
 */

#ifndef DEVICE_H_
#define DEVICE_H_

struct kfile_operations;

typedef int (* dev_module_init_ft)(void);
struct dev_module {
	const char * name;
	const struct kfile_operations *fops;
	const dev_module_init_ft init;
};

#include <util/array.h>
#define CHAR_DEV_DEF(name, file_op, idesc_op, init_func) \
	ARRAY_SPREAD_DECLARE(const struct dev_module, __device_registry); \
	ARRAY_SPREAD_ADD(__device_registry, {name, file_op, init_func})

extern int char_dev_init_all(void);
extern int char_dev_register(const char *name,
		const struct kfile_operations *ops);

#endif /* DEVICE_H_ */

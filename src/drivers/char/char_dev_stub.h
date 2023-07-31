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
#include <kernel/task/resource/idesc.h>

#define MAX_CDEV_QUANTITY 0

struct dev_module;

#ifdef __GNUC__
	/* Avoid warning for unused parameters with ((unused)) attribute */
	#define CHAR_DEV_DEF(chname, open_fn, idesc_op, priv) \
		__attribute__((unused)) static struct { \
			void *o, *c, *i, *p; \
		} unused##__LINE__ = { \
			.o = open_fn, .i = (void *)idesc_op, .p = priv \
		}; \
		const struct dev_module *const MACRO_CONCAT(__char_device_registry_ptr_, chname) = NULL

#else
	#define CHAR_DEV_DEF(chname, open_fn, idesc_op, priv) \
			const struct dev_module *const MACRO_CONCAT(__char_device_registry_ptr_, chname) = NULL

#endif /* __GNUC__ */

struct dev_module;
struct idesc;

struct idesc_dev {
	struct idesc idesc;
	struct dev_module *dev;
};

extern int char_dev_init_all(void);
extern int char_dev_register(const struct dev_module *cdev);
extern int char_dev_unregister(struct dev_module *cdev);
extern int char_dev_idesc_fstat(struct idesc *idesc, void *buff);
extern struct idesc *char_dev_idesc_create(struct dev_module *cdev);

static inline struct dev_module *idesc_to_dev_module(struct idesc *desc) {
	return NULL;
}

extern struct idesc *char_dev_default_open(struct dev_module *cdev, void *priv);
extern void char_dev_default_close(struct idesc *idesc);

#endif /* DEVICE_H_ */

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

struct idesc;
struct dev_module;

#ifdef __GNUC__
	/* Avoid warning for unused parameters with ((unused)) attribute */
	#define CHAR_DEV_DEF(chname, open_fn, close_fn, idesc_op, priv) \
		__attribute__((unused)) static struct { \
			void *o, *c, *i, *p; \
		} unused##__LINE__ = { \
			.o = open_fn, .c = close_fn, .i = idesc_op, .p = priv \
		};
#else
	#define CHAR_DEV_DEF(chname, open_fn, close_fn, idesc_op, priv)
#endif /* __GNUC__ */

extern int char_dev_init_all(void);
extern int char_dev_register(const struct dev_module *cdev);
extern int char_dev_idesc_fstat(struct idesc *idesc, void *buff);
extern int cdev_idesc_alloc(struct dev_module *cdev);
extern struct idesc *char_dev_idesc_create(struct dev_module *cdev);

static inline struct dev_module *idesc_to_dev_module(struct idesc *desc) {
	return NULL;
}

#endif /* DEVICE_H_ */

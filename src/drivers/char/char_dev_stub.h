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

#define CHAR_DEV_DEF(chname, open_fn, close_fn, idesc_op, priv)

extern int char_dev_init_all(void);
extern int char_dev_register(const struct dev_module *cdev);
extern int char_dev_idesc_fstat(struct idesc *idesc, void *buff);
extern int cdev_idesc_alloc(struct dev_module *cdev);
extern struct idesc *char_dev_idesc_create(struct dev_module *cdev);

static inline struct dev_module *idesc_to_dev_module(struct idesc *desc) {
	return NULL;
}

#endif /* DEVICE_H_ */

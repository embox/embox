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

struct idesc;
struct dev_module;

extern int char_dev_init_all(void);
extern int char_dev_register(const struct dev_module *cdev);
extern int char_dev_unregister(struct dev_module *cdev);
extern int char_dev_default_fstat(struct idesc *idesc, void *buff);
extern int char_dev_default_open(struct idesc *idesc, void *source);
extern void char_dev_default_close(struct idesc *idesc);

static inline struct dev_module *idesc_to_dev_module(struct idesc *desc) {
	return NULL;
}

#endif /* DEVICE_H_ */

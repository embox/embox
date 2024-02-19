/**
 * @file
 * @brief Declarations for char devices
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-05
 */

#ifndef CHAR_DEV_DVFS_H_
#define CHAR_DEV_DVFS_H_

#include <drivers/device.h>
#include <kernel/task/resource/idesc.h>

#include <module/embox/device/char_dev_api.h>

extern int char_dev_init_all(void);
extern int char_dev_register(struct dev_module *cdev);
extern int char_dev_unregister(struct dev_module *cdev);
extern int char_dev_default_fstat(struct idesc *idesc, void *buff);
extern int char_dev_default_open(struct idesc *idesc, void *source);
extern void char_dev_default_close(struct idesc *idesc);

extern const struct idesc_ops *char_dev_get_default_ops(void);
extern struct dev_module *idesc_to_dev_module(struct idesc *desc);

#endif /* CHAR_DEV_DVFS_H_ */

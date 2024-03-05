/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 19.02.24
 */

#ifndef DEVICE_CHAR_DEV_H_
#define DEVICE_CHAR_DEV_H_

#include <limits.h>

#include <drivers/device.h>
#include <kernel/task/resource/idesc.h>
#include <lib/libds/array.h>
#include <lib/libds/dlist.h>

#include <config/embox/device/char_dev.h>

#define CHAR_DEV_INIT(_self, _name, _ops)         \
	((struct char_dev){                           \
	    .list_item = DLIST_INIT(_self.list_item), \
	    .usage_count = 0,                         \
	    .ops = _ops,                              \
	    .name = _name,                            \
	})

#define CHAR_DEV_REGISTER(_cdev_ptr)                                          \
	ARRAY_SPREAD_DECLARE(struct char_dev *const, __char_dev_static_registry); \
	ARRAY_SPREAD_ADD(__char_dev_static_registry, _cdev_ptr)

struct char_dev;

struct char_dev_ops {
	void (*close)(struct char_dev *cdev);
	int (*open)(struct char_dev *cdev, struct idesc *idesc);
	int (*status)(struct char_dev *cdev, int mask);
	int (*ioctl)(struct char_dev *cdev, int request, void *data);
	ssize_t (*read)(struct char_dev *cdev, void *buf, size_t nbyte);
	ssize_t (*write)(struct char_dev *cdev, const void *buf, size_t nbyte);
	void *(*direct_access)(struct char_dev *cdev, off_t off, size_t len);
};

struct char_dev {
	const struct char_dev_ops *ops;
	struct dlist_head list_item;
	int usage_count;
	char name[NAME_MAX];
};

struct char_dev_idesc {
	struct idesc idesc;
	struct char_dev *cdev;
};

extern struct dlist_head *const __char_dev_registry;

extern struct char_dev *char_dev_iterate(struct char_dev *cdev);
extern struct char_dev *char_dev_find(const char *name);
extern int char_dev_rename(struct char_dev *cdev, const char *new_name);
extern int char_dev_register(struct char_dev *cdev);
extern int char_dev_unregister(struct char_dev *cdev);
extern void char_dev_init(struct char_dev *cdev, const char *name,
    const struct char_dev_ops *ops);

extern struct idesc *char_dev_open(struct char_dev *cdev, int oflag);

#define char_dev_foreach(cdev_ptr) \
	dlist_foreach_entry(cdev_ptr, __char_dev_registry, list_item)

#endif /* DEVICE_CHAR_DEV_H_ */

/**
 * @file
 * @brief
 *
 * @date 20.01.13
 * @author Alexander Kalmuk
 */

#ifndef EMBOX_INPUT_DEVICE_H_
#define EMBOX_INPUT_DEVICE_H_

#include <util/dlist.h>

struct input_event {
	/* e.g. KEY_PRESSED. Device-dependent */
	unsigned char type;
	/* e.g. REL_X for mouse, or pressed key for keyboard */
	int value;
};

struct input_dev {
	const char *name;
	int (*open)(struct input_dev *);
	void (*close)(struct input_dev *);
	struct dlist_head handler_link;
	struct dlist_head input_dev_link;
};

struct input_handler {
	unsigned int id;
	struct dlist_head input_dev_link;
	int (*store_event)(struct input_handler *, struct input_event );
	void *storage;
};

extern void input_dev_register(struct input_dev *dev);
extern void input_dev_inject_event(struct input_dev *dev, struct input_event e);
extern struct input_handler *input_dev_get_handler(struct input_dev *dev,
		unsigned int id);

#endif /* EMBOX_INPUT_DEVICE_H_ */

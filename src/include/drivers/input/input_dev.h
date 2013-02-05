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
	int type;
	/* e.g. REL_X for mouse, or pressed key for keyboard */
	int value;
};

/*describe input device instance */
struct input_dev {
	const char *name; /* registered name /dev/input/<name> */
	int irq;
	struct dlist_head handler_link; /* subscribers */
	struct dlist_head input_dev_link; /* global device list */


	/* file operations */
	int (*open)(struct input_dev *);
	void (*close)(struct input_dev *);
};


struct input_handler {
	unsigned int id; /* subscriber ID */
	struct dlist_head input_dev_link; /* link to subscribers */

	int (*store_event)(struct input_handler *, struct input_event );

	void *storage;
};

extern void input_dev_register(struct input_dev *dev);

extern void input_dev_inject_event(struct input_dev *dev, struct input_event e);

extern struct input_handler *input_dev_get_handler(struct input_dev *dev,
		unsigned int id);

#endif /* EMBOX_INPUT_DEVICE_H_ */

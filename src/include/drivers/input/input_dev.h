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
#include <util/ring_buff.h>
#include <kernel/thread/event.h>

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
	int (*getc)(void);
	struct dlist_head subscribers; /* subscribers */

	struct dlist_head global_indev_list; /* global device list */


	/* file operations without file system*/
	int (*open)(struct input_dev *);
	void (*close)(struct input_dev *);
};


struct input_subscriber {
	unsigned int id; /* subscriber ID */
	struct dlist_head subscribers; /* link to subscribers */

	//int (*store_event)(struct input_subscriber *, struct input_event );

	struct ring_buff rbuff; /* structure of ring buffer it required ring_buff_init */
	char inbuff[0x20];
	struct event rx_happend;
};

extern void input_dev_register(struct input_dev *dev);

#if 0
extern void input_dev_inject_event(struct input_dev *dev, struct input_event e);

extern struct input_subscriber *input_dev_find_subscriber(struct input_dev *dev,
		unsigned int id);
#endif

#endif /* EMBOX_INPUT_DEVICE_H_ */

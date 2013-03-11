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

enum input_dev_type {
	INPUT_DEV_KBD,
	INPUT_DEV_MOUSE,
	INPUT_DEV_APB
};

struct input_dev;

typedef int indev_event_cb_t(struct input_dev *indev);
typedef int indev_event_get_t(struct input_dev *dev, struct input_event *ev);

/*describe input device instance */
struct input_dev {
	const char *name; /* registered name /dev/input/<name> */
	enum input_dev_type type;
	int proto;
	int irq;

	struct dlist_head global_indev_list; /* global device list */
	struct dlist_head post_link;

	indev_event_cb_t *indev_event;
	indev_event_get_t *indev_get;
#if 0
	int (*getc)(void);



	/* file operations without file system*/
	int (*open)(struct input_dev *);
	void (*close)(struct input_dev *);
#endif
};

#if 0
struct input_subscriber {
	unsigned int id; /* subscriber ID */
	struct dlist_head subscribers; /* link to subscribers */

	//int (*store_event)(struct input_subscriber *, struct input_event );

	struct ring_buff rbuff; /* structure of ring buffer it required ring_buff_init */
	char inbuff[0x20];
	struct event rx_happend;
};
#endif
extern int input_dev_register(struct input_dev *dev);

extern int input_dev_event(struct input_dev *dev, struct input_event *ev);

extern struct input_dev *input_dev_lookup(char *name);

extern int input_dev_open(struct input_dev *dev, indev_event_cb_t *event);
extern int input_dev_close(struct input_dev *dev);
#if 0
extern void input_dev_inject_event(struct input_dev *dev, struct input_event e);

extern struct input_subscriber *input_dev_find_subscriber(struct input_dev *dev,
		unsigned int id);
#endif

#endif /* EMBOX_INPUT_DEVICE_H_ */

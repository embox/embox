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
#include <kernel/event.h>

#define INPUT_DEV_EVENT_QUEUE_LEN 16

enum input_dev_type {
	INPUT_DEV_KBD,
	INPUT_DEV_MOUSE,
	INPUT_DEV_APB,
};

struct input_event {
	enum input_dev_type devtype;
	/* e.g. KEY_PRESSED. Device-dependent */
	int type;
	/* e.g. REL_X for mouse, or pressed key for keyboard */
	int value;
};

struct input_dev;

typedef int indev_event_cb_t(struct input_dev *indev);
typedef int indev_event_get_t(struct input_dev *dev, struct input_event *ev);

/*describe input device instance */
struct input_dev {
	const char *name; /* registered name /dev/input/<name> */
	indev_event_get_t *event_get;
	enum input_dev_type type;
	int proto;
	int irq;
	void *data;

	indev_event_cb_t *event_cb;
	struct dlist_head global_indev_list; /* global device list */
	struct dlist_head post_link;

	struct ring_buff rbuf;
	struct input_event event_buf[INPUT_DEV_EVENT_QUEUE_LEN];
	struct input_event *curprocessd;

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

extern struct input_dev *input_dev_lookup(const char *name);

extern int input_dev_open(struct input_dev *dev, indev_event_cb_t *event);
extern int input_dev_close(struct input_dev *dev);
#if 0
extern void input_dev_inject_event(struct input_dev *dev, struct input_event e);

extern struct input_subscriber *input_dev_find_subscriber(struct input_dev *dev,
		unsigned int id);
#endif

#endif /* EMBOX_INPUT_DEVICE_H_ */

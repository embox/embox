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

#define INPUT_DEV_EVENT_QUEUE_LEN 16

struct input_dev;

typedef int indev_event_cb_t(struct input_dev *indev);

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

struct input_dev_ops {
	int (*start)(struct input_dev *dev);
	int (*stop)(struct input_dev *dev);
	int (*event_get)(struct input_dev *dev, struct input_event *ev);
};

/*describe input device instance */
struct input_dev {
	const struct input_dev_ops *ops;
	const char *name; /* registered name /dev/input/<name> */
	enum input_dev_type type;
	int proto;
	int irq;
	void *data;

	indev_event_cb_t *event_cb; /* callback on event. NULL is valid and
				       means input dev isn't opened */
	struct dlist_head global_indev_list; /* global device list */
	struct dlist_head post_link; /* link in to process queue */

	struct ring_buff rbuf;
	struct input_event event_buf[INPUT_DEV_EVENT_QUEUE_LEN];
	struct input_event *curprocessd; /* pointer to allocated but not valid
					    input event (@a event_get
					    may fail) */

};

/**
 * @brief Register new input device
 *
 * @param dev
 *
 * @return 0 on sucess
 */
extern int input_dev_register(struct input_dev *dev);

/**
 * @brief Notify that input device has new event to get
 *
 * @param dev
 *
 * @return 0 on success
 */
extern int input_dev_input(struct input_dev *dev);

/**
 * @brief Get pending event from input dev and store it in provided place.
 *
 * @param dev
 * @param ev
 *
 * @return 0 on success
 */
extern int input_dev_event(struct input_dev *dev, struct input_event *ev);

/**
 * @brief Lookup input device by name
 *
 * @param name
 *
 * @return Pointer to input device or NULL
 */
extern struct input_dev *input_dev_lookup(const char *name);

/**
 * @brief Open input device, optionally trigger event callback on new events
 * avaible.
 *
 * @param dev
 * @param event
 *
 * @return 0 on success
 */
extern int input_dev_open(struct input_dev *dev, indev_event_cb_t *event);

/**
 * @brief Close input device, callback will not be called (if any)
 *
 * @param dev
 *
 * @return
 */
extern int input_dev_close(struct input_dev *dev);

#endif /* EMBOX_INPUT_DEVICE_H_ */

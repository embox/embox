/**
 * @file
 * @brief
 *
 * @date 20.01.13
 * @author Alexander Kalmuk
 */

#ifndef EMBOX_INPUT_DEVICE_H_
#define EMBOX_INPUT_DEVICE_H_

#include <lib/libds/dlist.h>
#include <lib/libds/ring_buff.h>

#include <framework/mod/options.h>
#include <module/embox/driver/input/core.h>

#include <drivers/input/input_codes.h>

#define INPUT_DEV_EVENT_QUEUE_LEN \
    OPTION_MODULE_GET(embox__driver__input__core, NUMBER, event_queue_len)

struct input_dev;

typedef int indev_event_cb_t(struct input_dev *indev);

enum input_dev_type {
	INPUT_DEV_KBD,
	INPUT_DEV_MOUSE,
	INPUT_DEV_APB,
	INPUT_DEV_TOUCHSCREEN,
	INPUT_DEV_BUTTON,
};

struct input_event {
	enum input_dev_type devtype;
	/* e.g. KBD_KEY_PRESSED. Device-dependent */
	int type;
	/* e.g. REL_X for mouse, or pressed key for keyboard */
	int value;
};

struct input_dev_ops {
	int (*start)(struct input_dev *dev);
	int (*stop)(struct input_dev *dev);
};

/*describe input device instance */
struct input_dev {
	const struct input_dev_ops *ops;
	const char *name; /* registered name /dev/input/<name> */
	enum input_dev_type type;

	void *data;

	int flags;
#define INPUT_DEV_OPENED (1 << 0)

	indev_event_cb_t *event_cb; /* user callback on event */
	struct dlist_head dev_link; /* global device list */
	struct dlist_head post_link; /* link in to process queue */

	struct ring_buff rbuf;
	struct input_event event_buf[INPUT_DEV_EVENT_QUEUE_LEN];
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
extern void input_dev_report_event(struct input_dev *dev, struct input_event *ev);

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

extern struct input_dev *input_dev_iterate(struct input_dev * dev);

static inline int input_dev_bytes_count(struct input_dev *dev) {
	return ring_buff_get_cnt(&dev->rbuf);
}

extern int input_dev_private_register(struct input_dev *inpdev);
extern int input_dev_private_notify(struct input_dev *inpdev,
                                    struct input_event *ev);

#endif /* EMBOX_INPUT_DEVICE_H_ */

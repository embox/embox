/**
 * @file
 *
 * @date 20.01.13
 * @author Alexander Kalmuk
 */

#include <types.h>
#include <drivers/input/input_dev.h>

static DLIST_DEFINE(__input_devices);

void input_dev_register(struct input_dev *dev) {
	assert(dev != NULL);
	dlist_head_init(&dev->input_dev_link);
	dlist_add_prev(&dev->input_dev_link, &__input_devices);
	dlist_init(&dev->handler_link);
}

struct input_handler *input_dev_get_handler(struct input_dev *dev,
		unsigned int handler_id) {
	struct input_handler *cur, *nxt;

	assert(dev != NULL);

	dlist_foreach_entry(cur, nxt, &dev->handler_link, input_dev_link) {
		if (cur->id == handler_id) {
			return cur;
		}
	}

	return NULL;
}

void input_dev_inject_event(struct input_dev *dev, struct input_event event) {
	struct input_handler *cur, *nxt;

	assert(dev != NULL);

	dlist_foreach_entry(cur, nxt, &dev->handler_link, input_dev_link) {
		if (cur->store_event) {
			cur->store_event(cur, event);
		}
	}
}

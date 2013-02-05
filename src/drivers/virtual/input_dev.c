/**
 * @file
 *
 * @date 20.01.13
 * @author Alexander Kalmuk
 */

#include <types.h>
#include <kernel/irq.h>
#include <drivers/input/input_dev.h>





//void input_dev_inject_event(struct input_dev *dev, struct input_event event) {
//	struct input_subscriber *cur, *nxt;
//
//	assert(dev != NULL);
//
//	dlist_foreach_entry(cur, nxt, &dev->subscribers, subscribers) {
//		if (cur->store_event) {
//			cur->store_event(cur, event);
//		}
//	}
//}

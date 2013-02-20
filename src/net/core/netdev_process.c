/**
 * @file
 * @brief Network device processing routines.
 *
 * @date 27.07.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <errno.h>
#include <mem/misc/pool.h>
#include <net/if.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <stdlib.h>
#include <string.h>
#include <hal/ipl.h>
#include <linux/list.h>

static LIST_HEAD(rx_dev_queue);

void netdev_rx_queued(struct net_device *dev) {
	ipl_t sp;

	assert(dev != NULL);

	sp = ipl_save();

	assert(((dev->rx_dev_link.next == NULL) && (dev->rx_dev_link.prev == NULL))
			|| ((dev->rx_dev_link.next != NULL) && (dev->rx_dev_link.prev != NULL)));

	if (dev->rx_dev_link.next == NULL) {
		list_add_tail(&dev->rx_dev_link, &rx_dev_queue);
	}

	ipl_restore(sp);
}

void netdev_rx_dequeued(struct net_device *dev) {
	ipl_t sp;

	assert(dev != NULL);

	sp = ipl_save();

	list_del(&dev->rx_dev_link);

	ipl_restore(sp);
}

void netdev_rx_processing(void) {
	struct net_device *dev, *save;

	list_for_each_entry_safe(dev, save, &rx_dev_queue, rx_dev_link) {
		dev->poll(dev);
		netdev_rx_dequeued(dev);
	}
}

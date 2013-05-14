/**
 * @file
 * @brief Network device processing routines
 *
 * @date 27.07.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <hal/ipl.h>
#include <net/netdevice.h>
#include <stddef.h>
#include <util/list.h>

static LIST_DEF(netdev_rx_list);

void netdev_rx_queued(struct net_device *dev) {
	ipl_t sp;

	assert(dev != NULL);

	sp = ipl_save();
	{
		if (list_alone_link(&dev->rx_lnk)) {
			list_add_last_link(&dev->rx_lnk, &netdev_rx_list);
		}
	}
	ipl_restore(sp);
}

void netdev_rx_dequeued(struct net_device *dev) {
	ipl_t sp;

	assert(dev != NULL);

	sp = ipl_save();
	{
		assert(!list_alone_link(&dev->rx_lnk));
		list_unlink_link(&dev->rx_lnk);
	}
	ipl_restore(sp);
}

void netdev_rx_processing(void) {
	struct net_device *dev;

	list_foreach(dev, &netdev_rx_list, rx_lnk) {
		assert(dev->poll != NULL);
		dev->poll(dev);
		netdev_rx_dequeued(dev);
	}
}

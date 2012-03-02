/**
 * @file
 * @brief
 *
 * @date 27.10.11
 * @author Anton Kozlov
 */

#include <errno.h>
#include <mem/misc/pool.h>
#include <net/if.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/interrupt.h>

#include <embox/unit.h>

extern net_device_t *get_dev_by_idx(int num); /* TODO delete it */

EMBOX_UNIT_INIT(unit_init);

static void net_rx_action(struct softirq_action *action) {
	size_t i;
	net_device_t *dev;

	//TODO it will be better use list of active device and cache for them
	for (i = 0; i < CONFIG_NET_DEVICES_QUANTITY; i++) {
		dev = get_dev_by_idx(i);
		if ((NULL != dev) && (NULL != dev->poll)) {
			dev->poll(dev);
		}
	}
}

static int unit_init(void) {
	open_softirq(NET_RX_SOFTIRQ, net_rx_action, NULL);
	return 0;
}

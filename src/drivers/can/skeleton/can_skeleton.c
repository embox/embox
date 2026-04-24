/**
 * @file
 *
 * @author Anton Bondarev
 * @date 14.04.26
 */

#include <util/log.h>

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <kernel/irq.h>

#include <net/inetdevice.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/skbuff.h>

#include <net/can_netdevice.h>

#include <net/util/show_packet.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(can_skeleton_init);

static int can_skeleton_xmit(struct net_device *dev, struct sk_buff *skb) {
	uint8_t *skb_data;

	skb_data = skb_data_cast_in(skb->data);
	show_packet(skb_data, skb->len, "can tx");

	return 0;
}

#if 0

static irq_return_t can_skeleton_irq_handler(unsigned int irq_num, void *dev_id) {
	return IRQ_HANDLED;
}
#endif

static int can_skeleton_start(struct net_device *dev) {

	return 0;
}

static int can_skeleton_stop(struct net_device *dev) {
	return 0;
}

static const struct net_driver can_skeleton_drv_ops = {
    .xmit = can_skeleton_xmit,
    .start = can_skeleton_start,
    .stop = can_skeleton_stop,
};

static int can_skeleton_init(void) {
	struct net_device *netdev;


	// TODO: canbus_alloc()
	netdev = etherdev_alloc(0);
	if (netdev == NULL) {
		return -ENOMEM;
	}

	netdev->drv_ops = &can_skeleton_drv_ops;

#if 0
	int err;
	err = irq_attach(netdev->irq, can_skeleton_irq_handler, 0, NULL, "netdev");
	if (err < 0) {
		return err;
	}
#endif

	cannetdev_register(netdev);

	return 0;
}

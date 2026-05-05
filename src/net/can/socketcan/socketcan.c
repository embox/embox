/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.04.26
 */

#include <errno.h>
#include <stdint.h>

#include <embox/unit.h>
#include <kernel/irq.h>
#include <lib/libds/array_spread.h>
#include <net/can.h>
#include <net/netdevice.h>
#include <net/skbuff.h>

#include "can_netdevice.h"

int socketcan_start(struct net_device *dev) {
	return 0;
}

static int socketcan_stop(struct net_device *dev) {
	return 0;
}

static int socketcan_send(struct net_device *dev, struct sk_buff *skb) {
	struct can_controller *can;
	struct can_frame *frame;
	int err;

	can = (struct can_controller *)dev->base_addr;
	frame = (struct can_frame *)skb_data_cast_in(skb->data);

	err = can->ops->send(can, frame);

	skb_free(skb);

	return err;
}

static const struct net_driver socketcan_ops = {
    .start = socketcan_start,
    .stop = socketcan_stop,
    .xmit = socketcan_send,
};

extern int canif_rx(void *data);

static irq_return_t socketcan_irq_handler(unsigned int irq_num, void *dev_id) {
	struct can_controller *can;
	struct net_device *dev;
	struct can_frame *frame;
	struct sk_buff *skb;
	int err;

	dev = (struct net_device *)dev_id;
	can = (struct can_controller *)dev->base_addr;

	while (can->ops->hasrx(can)) {
		skb = skb_alloc(sizeof(struct can_frame));
		skb->dev = dev;

		frame = (struct can_frame *)skb_data_cast_in(skb->data);

		err = can->ops->receive(can, frame);
		if (err) {
			skb_free(skb);
		}
		else {
			canif_rx(skb);
		}
	}

	can->ops->eoi(can);

	return IRQ_HANDLED;
}

EMBOX_UNIT_INIT(socketcan_init);

ARRAY_SPREAD_DEF(struct can_controller *const, __can_socketcan_registry);

static int socketcan_init(void) {
	struct can_controller *can;
	struct net_device *netdev;
	int err;

	array_spread_foreach(can, __can_socketcan_registry) {
		netdev = can_netdev_alloc();
		if (netdev == NULL) {
			return -ENOMEM;
		}

		netdev->base_addr = (uintptr_t)can;
		netdev->irq = can->irq;
		netdev->drv_ops = &socketcan_ops;

		err = irq_attach(netdev->irq, socketcan_irq_handler, 0, netdev, NULL);
		if (err) {
			return err;
		}

		err = can->ops->start(can);
		if (err) {
			return err;
		}

#if 0
		err = netdev_register(netdev);
		if (err) {
			return err;
		}
#else
		can_netdev_register(netdev);
#endif
	}

	return 0;
}

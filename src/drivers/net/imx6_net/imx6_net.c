/**
 * @file imx6_net.c
 * @brief iMX6 MAC-NET driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-05-11
 */

#include <errno.h>

#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/skbuff.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(imx6_net_init);
static int imx6_net_xmit(struct net_device *dev, struct sk_buff *skb) {
	skb_free(skb);
	return 0;
}

static int imx6_net_open(struct net_device *dev) {
	return 0;
}

static int imx6_net_set_macaddr(struct net_device *dev, const void *addr) {
	return 0;
}

static const struct net_driver imx6_net_drv_ops = {
	.xmit = imx6_net_xmit,
	.start = imx6_net_open,
	.set_macaddr = imx6_net_set_macaddr
};

static int imx6_net_init(void) {
	struct net_device *nic;

	if (NULL == (nic = etherdev_alloc(0))) {
                return -ENOMEM;
        }

	nic->drv_ops = &imx6_net_drv_ops;

	return inetdev_register_dev(nic);;
}

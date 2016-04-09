/**
 * @file
 *
 * @data 09.04.2016
 * @author: Anton Bondarev
 */
#include <net/skbuff.h>
#include <net/netdevice.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(lan91c111_init);
static int lan91c111_xmit(struct net_device *dev, struct sk_buff *skb) {
	skb_free(skb);
	return 0;
}

static int lan91c111_open(struct net_device *dev) {
	return 0;
}

static int lan91c111_set_macaddr(struct net_device *dev, const void *addr) {
	return 0;
}

static const struct net_driver lan91c111_drv_ops = {
	.xmit = lan91c111_xmit,
	.start = lan91c111_open,
	.set_macaddr = lan91c111_set_macaddr
};

static int lan91c111_init(void) {
	return 0;
}

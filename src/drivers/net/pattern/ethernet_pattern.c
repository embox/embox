/**
 * @file
 *
 * @date 09.04.2016
 * @author: Anton Bondarev
 */
#include <net/skbuff.h>
#include <net/netdevice.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(ethernet_pattern_init);
static int ethernet_pattern_xmit(struct net_device *dev, struct sk_buff *skb) {
	skb_free(skb);
	return 0;
}

static int ethernet_pattern_open(struct net_device *dev) {
	return 0;
}

static int ethernet_pattern_set_macaddr(struct net_device *dev, const void *addr) {
	return 0;
}

static const struct net_driver ethernet_pattern_drv_ops = {
	.xmit = ethernet_pattern_xmit,
	.start = ethernet_pattern_open,
	.set_macaddr = ethernet_pattern_set_macaddr
};

static int ethernet_pattern_init(void) {
	return 0;
}

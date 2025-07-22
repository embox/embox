/**
 * @file 
 * @brief ISM43362-M3G-L44-SPI Inventek eS-WiFi module library
 * @author Daria Pechenova
 * @version
 * @date 18.07.2025
 */

 #include <util/log.h>

#include <errno.h>
#include <unistd.h>

#include <net/l2/ethernet.h>
#include <net/l0/net_entry.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>

#include <libs/ism43362.h>

#include <embox/unit.h>
#include <framework/mod/options.h>

EMBOX_UNIT_INIT(eswifi_init);

static int eswifi_xmit(struct net_device *dev, struct sk_buff *skb) {
	return 0;
}

static int eswifi_open(struct net_device *dev) {
	return 0;
}

static int eswifi_set_macaddr(struct net_device *dev, const void *addr) {
	return 0;
}

static const struct net_driver eswifi_drv_ops = {
	.xmit = eswifi_xmit,
	.start = eswifi_open,

	.set_macaddr = eswifi_set_macaddr,
};

static int eswifi_init(void) {
	struct net_device *nic;

	nic = etherdev_alloc(0);
	if (NULL == nic) {
		return -ENOMEM;
	}

	nic->drv_ops = &eswifi_drv_ops;

	return inetdev_register_dev(nic);
}

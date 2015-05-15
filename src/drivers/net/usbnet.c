/**
 * @file
 *
 * @author  Alex Kalmuk
 * @date    05.05.2015
 */

#include <assert.h>
#include <errno.h>

#include <util/math.h>

#include <net/l2/ethernet.h>
#include <net/l3/arp.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>
#include <net/l0/net_entry.h>
#include <drivers/usb/usb_driver.h>
#include <drivers/usb/usb_dev_desc.h>
#include <drivers/usb/usb_cdc.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(usbnet_init);

static struct usb_driver usbnet_driver;
static const struct net_driver usbnet_drv_ops;

struct usbnet_priv {
	struct usb_dev *usbdev;
};

static int usbnet_init(void) {
	return usb_driver_register(&usbnet_driver);
}

static int usbnet_probe(struct usb_driver *drv, struct usb_dev *dev,
		void **data) {
	struct net_device *nic;
	struct usbnet_priv *nic_priv;

	nic = (struct net_device *) etherdev_alloc(sizeof *nic_priv);
	if (nic == NULL) {
		return -ENOMEM;
	}
	nic->drv_ops = &usbnet_drv_ops;
	nic_priv = netdev_priv(nic, struct usbnet_priv);
	nic_priv->usbdev = dev;

	return inetdev_register_dev(nic);
}

static void usbnet_disconnect(struct usb_dev *dev, void *data) {

}

static struct usb_device_id usbnet_id_table[] = {
	{ 0x0525, 0xa4a2 },
	{ },
};

static void usbnet_send_notify_hnd(struct usb_request *req, void *arg) {
}

static void usb_net_bulk_send(struct usb_dev *dev, struct sk_buff *skb) {
	struct usb_endp *endp;
	size_t i = 0;
	int len;

	endp = dev->endpoints[2];

	for (i = skb->len, len = min(skb->len, endp->max_packet_size);
			i != 0; i -= len) {
		usb_endp_bulk(endp, usbnet_send_notify_hnd,
				skb->mac.raw + (skb->len - i), len);
	}

	if (len == endp->max_packet_size) {
		usb_endp_bulk(endp, usbnet_send_notify_hnd, skb->mac.raw + i, 0);
	}
}

static int usbnet_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct usbnet_priv *nic_priv;
	//struct usb_class_cdc *cdc;

	nic_priv = netdev_priv(dev, struct usbnet_priv);
	//cdc = usb2cdcdata(nic_priv->usbdev);

	// Fill endpoints

	usb_net_bulk_send(nic_priv->usbdev, skb);

	return 0;
}

static struct usb_driver usbnet_driver = {
	.probe = usbnet_probe,
	.disconnect = usbnet_disconnect,
	.id_table = usbnet_id_table,
};

static const struct net_driver usbnet_drv_ops = {
	.xmit = usbnet_xmit,
#if 0
	.start = usbnet_open,
	.stop = usbnet_stop,
	.set_macaddr = usbnet_set_mac_address
#endif
};

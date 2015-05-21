/**
 * @file
 *
 * @author  Alex Kalmuk
 * @date    05.05.2015
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

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
#include <mem/sysmalloc.h>
#include <embox/unit.h>

#include <kernel/time/timer.h>

#include <kernel/printk.h>

EMBOX_UNIT_INIT(usbnet_init);

#define USBNET_TIMER_FREQ 10

static struct usb_driver usbnet_driver;
static const struct net_driver usbnet_drv_ops;

static void usbnet_timer_handler(struct sys_timer *tmr, void *param);
static void usbnet_rcv_notify(struct usb_request *req, void *arg);
static int usbnet_init(void);
static int usbnet_probe(struct usb_driver *drv, struct usb_dev *dev,
		void **data);
static void usbnet_disconnect(struct usb_dev *dev, void *data);
static void usb_net_bulk_send(struct usb_dev *dev, struct sk_buff *skb);
static int usbnet_xmit(struct net_device *dev, struct sk_buff *skb);

struct usbnet_priv {
	struct usb_dev *usbdev;
	char *data;
	char *pdata;
	struct sys_timer timer;
};

static int usbnet_init(void) {
	return usb_driver_register(&usbnet_driver);
}

static void usbnet_rcv_notify(struct usb_request *req, void *arg) {
	struct usb_endp *in_endp;
	struct usb_dev *dev = req->endp->dev;
	struct usb_class_cdc *cdc = usb2cdcdata(dev);
	struct net_device *nic = (struct net_device *) cdc->privdata;
	struct usbnet_priv *nic_priv = (struct usbnet_priv *) nic->priv;

	printk(">>>>> usbnet_timer_handler");
	in_endp = nic_priv->usbdev->endpoints[2];

	switch(req->req_stat) {
	case USB_REQ_NOERR:
		memcpy(nic_priv->pdata, req->buf, req->len);
		nic_priv->pdata += req->len;
		break;
	default:
		return;
	}

	/* End of packet */
	if (req->len < in_endp->max_packet_size) {
		struct sk_buff *skb;
		size_t len;

		len = nic_priv->pdata - (char *) nic_priv->data;
		if ((skb = skb_alloc(len))) {
			memcpy(skb->mac.raw, nic_priv->data, len);
			skb->dev = nic;
			netif_rx(skb);
		} else {
			printk("usbnet: skbuff allocation failed\n");
		}

		/* Prepare for copying new packet */
		nic_priv->pdata = nic_priv->data;
	}

	timer_init_msec(&nic_priv->timer, TIMER_ONESHOT,
	            USBNET_TIMER_FREQ, usbnet_timer_handler, nic_priv);
}

static void usbnet_timer_handler(struct sys_timer *tmr, void *param) {
	struct usb_endp *in_endp;
	struct usbnet_priv *nic_priv = (struct usbnet_priv *) param;

	in_endp = nic_priv->usbdev->endpoints[2];

	usb_endp_bulk(in_endp, usbnet_rcv_notify, nic_priv->pdata,
			in_endp->max_packet_size);
}

static int usbnet_probe(struct usb_driver *drv, struct usb_dev *dev,
		void **data) {
	struct net_device *nic;
	struct usbnet_priv *nic_priv;
	struct usb_class_cdc *cdc = usb2cdcdata(dev);
	int res;

	nic = (struct net_device *) etherdev_alloc(sizeof *nic_priv);
	if (nic == NULL) {
		return -ENOMEM;
	}
	nic->drv_ops = &usbnet_drv_ops;
	nic_priv = netdev_priv(nic, struct usbnet_priv);
	nic_priv->usbdev = dev;
	nic_priv->data = nic_priv->pdata = sysmalloc(ETH_FRAME_LEN);

	cdc->privdata = (void *) nic;

	res = inetdev_register_dev(nic);
	if (res < 0) {
		return res;
	}

    res = timer_init_msec(&nic_priv->timer, TIMER_ONESHOT,
            USBNET_TIMER_FREQ, usbnet_timer_handler, nic_priv);

    return res;
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

	endp = dev->endpoints[3];

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

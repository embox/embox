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
#include <util/log.h>

#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>
#include <net/l0/net_entry.h>
#include <net/l0/net_entry.h>
#include <drivers/usb/usb.h>
#include <drivers/usb/usb_driver.h>
#include <mem/sysmalloc.h>
#include <embox/unit.h>

#include <kernel/time/timer.h>

EMBOX_UNIT_INIT(usbnet_init);

#define USBNET_TIMER_FREQ 1

#define USBNET_IN_ENDP   1
#define USBNET_OUT_ENDP  2

static struct usb_driver usbnet_driver;
static const struct net_driver usbnet_drv_ops;

static void usbnet_timer_handler(struct sys_timer *tmr, void *param);

struct usbnet_priv {
	struct usb_interface *usbdev;
	/* Storage for incoming packet */
	char *data;
	/* Current position in packet */
	char *pdata;
	/* Timer for polling */
	struct sys_timer timer;

	struct net_device *nic;
};

static int usbnet_init(void) {
	return usb_driver_register(&usbnet_driver);
}

static void usbnet_rcv_notify(struct usb_request *req, void *arg) {
	struct usbnet_priv *nic_priv;
	struct net_device *nic;
	struct usb_endp *in_endp;

	nic_priv = (struct usbnet_priv *) req->endp->dev->current_config->usb_iface[2]->driver_data;
	assert(nic_priv);
	nic = nic_priv->nic;
	assert(nic);

	in_endp = nic_priv->usbdev->endpoints[USBNET_IN_ENDP];
	assert(in_endp);

	switch(req->req_stat) {
	case USB_REQ_NOERR:
		memcpy(nic_priv->pdata, req->buf, req->len);
		nic_priv->pdata += req->len;
		break;
	default:
		log_error("req error (req->req_stat=%d)\n", req->req_stat);
		return;
	}

	/* End of packet */
	if (req->actual_len < req->len) {
		struct sk_buff *skb;
		size_t len;

		len = nic_priv->pdata - nic_priv->data;
		skb = skb_alloc(len);
		if (!skb) {
			log_error("usbnet: skbuff allocation failed");
		} else {
			memcpy(skb->mac.raw, nic_priv->data, len);
			skb->dev = nic;
			netif_rx(skb);
		}

		/* Prepare for copying new packet */
		nic_priv->pdata = nic_priv->data;

		sys_timer_init_start_msec(&nic_priv->timer, SYS_TIMER_ONESHOT,
				USBNET_TIMER_FREQ, usbnet_timer_handler, nic_priv);
	} else {
		/* Receive the next part of the packet */
		usb_endp_bulk(in_endp, usbnet_rcv_notify, NULL, nic_priv->pdata,
				in_endp->max_packet_size);
	}
}

static void usbnet_timer_handler(struct sys_timer *tmr, void *param) {
	struct usb_endp *in_endp;
	struct usbnet_priv *nic_priv = (struct usbnet_priv *) param;

	in_endp = nic_priv->usbdev->endpoints[USBNET_IN_ENDP];
	assert(in_endp);

	usb_endp_bulk(in_endp, usbnet_rcv_notify, NULL, nic_priv->pdata,
			in_endp->max_packet_size);
}

static int usbnet_probe(struct usb_interface *dev) {
	struct net_device *nic;
	struct usbnet_priv *nic_priv;
	int res;
	struct usb_interface *data_iface;

//	if (dev == dev->usb_dev->current_config->usb_iface[0]) {
//		return 0; /*control interface */
//	}

	data_iface = dev->usb_dev->current_config->usb_iface[2];

	assert(data_iface);

	/* Enable in/out DATA interface */
	if (usb_set_iface(dev->usb_dev, 1, 1) < 0) {
		res = -EINVAL;
		goto out_ret;
	}

	nic = (struct net_device *) etherdev_alloc(sizeof *nic_priv);
	if (!nic) {
		res = -ENOMEM;
		goto out_ret;
	}
	nic->drv_ops = &usbnet_drv_ops;
	nic_priv = netdev_priv(nic);
	nic_priv->usbdev = data_iface;
	nic_priv->nic = nic;
	nic_priv->data = nic_priv->pdata = sysmalloc(ETH_FRAME_LEN);
	if (!nic_priv->data) {
		res = -ENOMEM;
		goto out_free_nic;
	}
	data_iface->driver_data = nic_priv;

	res = inetdev_register_dev(nic);
	if (res < 0) {
		goto out_free_data;
	}

	res = sys_timer_init_start_msec(&nic_priv->timer, SYS_TIMER_ONESHOT,
		USBNET_TIMER_FREQ, usbnet_timer_handler, nic_priv);
	if (res < 0) {
		goto out_free_data;
	}

	return 0;

out_free_data:
	sysfree(nic_priv->data);
out_free_nic:
	etherdev_free(nic);
out_ret:
	return res;
}

static void usbnet_disconnect(struct usb_interface *dev, void *data) {
}

static struct usb_device_id usbnet_id_table[] = {
	{ 2 /* CDC class */, 0x0525, 0xa4a2 },
	{ },
};

static void usbnet_send_notify_hnd(struct usb_request *req, void *arg) {
	if (arg) {
		skb_free((struct sk_buff *)arg);
	}
}

static int usbnet_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct usb_endp *endp;
	struct usbnet_priv *priv = netdev_priv(dev);

	assert(priv);
	assert(priv->usbdev);

	endp = priv->usbdev->endpoints[USBNET_OUT_ENDP];
	assert(endp);

	log_debug("skb->len = %d\n", skb->len);

	if (usb_endp_bulk(endp, usbnet_send_notify_hnd, skb, skb->mac.raw, skb->len) != 0) {
		log_error("usb_endp_bulk failed");
		return -1;
	}

	if (skb->len % endp->max_packet_size == 0) {
		if (usb_endp_bulk(endp, usbnet_send_notify_hnd, NULL, NULL, 0) != 0) {
			log_error("usb_endp_bulk zero len packet failed");
			return -1;
		}
	}

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

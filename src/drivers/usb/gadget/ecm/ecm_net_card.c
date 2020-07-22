/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    22.07.2020
 */

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <util/log.h>
#include <arpa/inet.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>
#include <net/l0/net_entry.h>
#include <drivers/usb/usb_defines.h>
#include <drivers/usb/usb_desc.h>
#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/gadget/gadget.h>

#define ECM_RX_BUF_SZ    1024

static struct net_device *ecm_card_nic;

static struct usb_gadget_ep *bulk_tx;
static struct usb_gadget_ep *bulk_rx;

static struct usb_gadget_request req_rx;
static struct usb_gadget_request req_tx;

static uint8_t ecm_rx_buf[ECM_RX_BUF_SZ];
static size_t ecm_rx_len;

static int ecm_card_xmit(struct net_device *dev, struct sk_buff *skb) {
	memcpy(req_tx.buf, skb->mac.raw, skb->len);
	req_tx.len = skb->len;

	usb_gadget_ep_queue(bulk_tx, &req_tx);

	skb_free(skb);

	return 0;
}

/* TODO The packets should be assemblied on UDC level, not here at ECM. */
static int ecm_rx_complete(struct usb_gadget_ep *ep,
	                    struct usb_gadget_request *req) {
	struct sk_buff *skb;

	memcpy(ecm_rx_buf + ecm_rx_len, req->buf, req->actual_len);
	ecm_rx_len += req->actual_len;

	if (req->actual_len == ep->desc->w_max_packet_size) {
		/* Packet is not completed yet. */
		usb_gadget_ep_queue(ep, req);
		return 0;
	}

	/* Packet is completed */

	assert(ecm_rx_len <= sizeof req->buf);

	skb = skb_alloc(ecm_rx_len);

	if (!skb) {
		log_error("skbuff allocation failed");
	} else {
		memcpy(skb->mac.raw, ecm_rx_buf, ecm_rx_len);
		skb->dev = ecm_card_nic;
		netif_rx(skb);
	}

	/* Wait for the next packet. */
	ecm_rx_len = 0;

	usb_gadget_ep_queue(ep, req);

	return 0;
}

static int ecm_card_start(struct net_device *dev) {
	return 0;
}

static int ecm_card_stop(struct net_device *dev) {
	return 0;
}

static const struct net_driver ecm_card_drv_ops = {
	.start       = ecm_card_start,
	.stop        = ecm_card_stop,
	.xmit        = ecm_card_xmit,
};

int ecm_card_init(struct usb_gadget_ep *tx, struct usb_gadget_ep *rx) {
	bulk_tx = tx;
	bulk_rx = rx;

	req_rx.complete = ecm_rx_complete;
	req_rx.len = sizeof req_rx.buf;
	usb_gadget_ep_queue(bulk_rx, &req_rx);

	req_tx.len = sizeof req_tx.buf;

	if (NULL == (ecm_card_nic = etherdev_alloc(0))) {
		return -ENOMEM;
	}

	ecm_card_nic->drv_ops = &ecm_card_drv_ops;

	return inetdev_register_dev(ecm_card_nic);
}

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
static const uint8_t ecm_card_mac[ETH_ALEN] = {
	0x00, 0x02, 0x02, 0x03, 0x00, 0x00
};

static const uint32_t ecm_tx_max_wait_trials = 1000000U;

static struct usb_gadget_ep *bulk_tx;
static struct usb_gadget_ep *bulk_rx;

static struct usb_gadget_request req_rx;
static struct usb_gadget_request req_tx;
static volatile int ecm_tx_busy;

static uint8_t ecm_rx_buf[ECM_RX_BUF_SZ];
static size_t ecm_rx_len;

static int ecm_card_xmit(struct net_device *dev, struct sk_buff *skb) {
	uint32_t trials = ecm_tx_max_wait_trials;
	int ret;

	while (ecm_tx_busy && trials > 0) {
		trials--;
	}

	if (trials == 0) {
		return -EBUSY;
	}

	memcpy(req_tx.buf, skb->mac.raw, skb->len);
	req_tx.len = skb->len;

	ecm_tx_busy = 1;
	ret = usb_gadget_ep_queue(bulk_tx, &req_tx);
	if (ret != 0) {
		ecm_tx_busy = 0;
		return ret;
	}

	skb_free(skb);

	return 0;
}

static int ecm_tx_complete(struct usb_gadget_ep *ep, struct usb_gadget_request *req) {
	ecm_tx_busy = 0;

	return 0;
}

/* TODO The packets should be assemblied on UDC level, not here at ECM. */
static int ecm_rx_complete(struct usb_gadget_ep *ep,
	                    struct usb_gadget_request *req) {
	struct sk_buff *skb;

	if (req->actual_len > sizeof(ecm_rx_buf) - ecm_rx_len) {
		log_error("ECM RX packet is too large");
		ecm_rx_len = 0;
		usb_gadget_ep_queue(ep, req);
		return 0;
	}

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
	int ret;

	bulk_tx = tx;
	bulk_rx = rx;

	req_rx.complete = ecm_rx_complete;
	req_rx.len = sizeof req_rx.buf;
	usb_gadget_ep_queue(bulk_rx, &req_rx);

	ecm_tx_busy = 0;
	req_tx.complete = ecm_tx_complete;
	req_tx.len = sizeof req_tx.buf;

	if (NULL == (ecm_card_nic = etherdev_alloc(0))) {
		return -ENOMEM;
	}

	ecm_card_nic->drv_ops = &ecm_card_drv_ops;

	ret = netdev_set_macaddr(ecm_card_nic, ecm_card_mac);
	if (ret != 0) {
		return ret;
	}

	return inetdev_register_dev(ecm_card_nic);
}

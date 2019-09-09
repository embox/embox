/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.10.2013
 */

#include <string.h>
#include <stddef.h>

#include <mem/misc/pool.h>
#include <util/dlist.h>
#include <drivers/usb/usb.h>

POOL_DEF(usb_hcds, struct usb_hcd, USB_MAX_HCD);
POOL_DEF(usb_hubs, struct usb_hub, USB_MAX_HUB);
POOL_DEF(usb_devs, struct usb_dev, USB_MAX_ENDP);
POOL_DEF(usb_endps, struct usb_endp, USB_MAX_ENDP);
POOL_DEF(usb_requests, struct usb_request, USB_MAX_REQ);

struct usb_hcd *usb_hcd_alloc(struct usb_hcd_ops *ops, void *args) {
	struct usb_hcd *hcd = pool_alloc(&usb_hcds);

	if (!hcd) {
		return NULL;
	}

	hcd->ops = ops;
	index_init(&hcd->enumerator, 1, USB_HC_MAX_DEV, &hcd->idx_data);

	dlist_head_init(&hcd->lnk);
	usb_queue_init(&hcd->reset_queue);

	if (ops->hcd_hci_alloc) {
		hcd->hci_specific = ops->hcd_hci_alloc(hcd, args);
		if (!hcd->hci_specific) {
			pool_free(&usb_hcds, hcd);
			return NULL;
		}
	}

	return hcd;
}

void usb_hcd_free(struct usb_hcd *hcd) {

	if (hcd->ops->hcd_hci_free) {
		hcd->ops->hcd_hci_free(hcd, hcd->hci_specific);
	}

	pool_free(&usb_hcds, hcd);
}

struct usb_hub *usb_hub_alloc(struct usb_hcd *hcd, usb_hub_port_t port_n) {
	struct usb_hub *hub = pool_alloc(&usb_hubs);

	hub->hcd = hcd;
	hub->port_n = port_n;

	for (int i = 0; i < port_n; i++) {
		usb_hub_port_init(&hub->ports[i], hub, i);
	}

	return hub;
}

void usb_hub_free(struct usb_hub *hub) {
	pool_free(&usb_hubs, hub);
}

static void usb_dev_free(struct usb_dev *dev) {
	int i;

	for (i = 0; i < USB_DEV_MAX_ENDP; i++) {
		struct usb_endp *endp = dev->endpoints[i];
		if (endp) {
			dev->endpoints[i] = NULL;
			usb_endp_free(endp);
		}
	}

	index_free(&dev->hcd->enumerator, dev->idx);
	pool_free(&usb_devs, dev);
}

struct usb_dev *usb_dev_alloc(struct usb_hcd *hcd) {
	struct usb_dev *dev = pool_alloc(&usb_devs);
	size_t idx;

	if (!dev) {
		return NULL;
	}

	idx = index_alloc(&hcd->enumerator, INDEX_MIN);
	assert(idx != INDEX_NONE);
	assert(idx < USB_HC_MAX_DEV);

	memset(dev, 0, sizeof(struct usb_dev));

	dev->hcd = hcd;
	dev->idx = idx;
	dev->bus_idx = 0;

	if (!usb_endp_alloc(dev, &usb_desc_endp_control_default)) {
		usb_dev_free(dev);
		return NULL;
	}

	dlist_head_init(&dev->dev_link);

	return dev;
}

void usb_dev_use_dec(struct usb_dev *dev) {
	if (!--dev->use_count) {
		usb_dev_free(dev);
	}
}

void usb_dev_use_inc(struct usb_dev *dev) {
	++dev->use_count;

}

struct usb_endp *usb_endp_alloc(struct usb_dev *dev,
		const struct usb_desc_endpoint *endp_desc) {
	struct usb_endp *endp = pool_alloc(&usb_endps);
	struct usb_hcd *hcd = dev->hcd;
	int endp_num;

	if (!endp) {
		return NULL;
	}

	endp->dev = dev;

	usb_endp_fill_from_desc(endp, endp_desc);

	usb_queue_init(&endp->req_queue);

	for (endp_num = 0; endp_num < USB_DEV_MAX_ENDP; endp_num++) {
		struct usb_endp *dendp = dev->endpoints[endp_num];
		if (!dendp) {
			break;
		}

		assert(dendp->address != endp->address ||
				dendp->direction != endp->direction);
	}

	assert(endp_num < USB_DEV_MAX_ENDP);

	dev->endpoints[endp_num] = endp;

	if (hcd->ops->endp_hci_alloc) {
		endp->hci_specific = hcd->ops->endp_hci_alloc(endp);
		if (!hcd->hci_specific) {
			pool_free(&usb_endps, endp);
			return NULL;
		}
	}

	return endp;
}

void usb_endp_free(struct usb_endp *endp) {
	struct usb_hcd *hcd = endp->dev->hcd;

	if (hcd->ops->endp_hci_free) {
		hcd->ops->endp_hci_free(endp, endp->hci_specific);
	}

	pool_free(&usb_endps, endp);
}


static struct usb_request *usb_request_alloc(struct usb_endp *endp) {
	struct usb_hcd *hcd = endp->dev->hcd;
	struct usb_request *req;

	req = pool_alloc(&usb_requests);
	req->endp = endp;

	usb_queue_link_init(&req->req_link);

	if (hcd->ops->req_hci_alloc) {
		req->hci_specific = hcd->ops->req_hci_alloc(req);
		if (!req->hci_specific) {
			pool_free(&usb_requests, req);
			return NULL;
		}
	}
	return req;
}

struct usb_request *usb_endp_request_alloc(struct usb_endp *endp,
		usb_request_notify_hnd_t notify_hnd, void *arg, unsigned token,
		void *buf, size_t len) {
	struct usb_request *req;

	req = usb_request_alloc(endp);
	assert(req);

	req->endp = endp;
	req->token = token;
	req->buf = buf;
	req->len = len;
	req->notify_hnd = notify_hnd;
	req->hnd_data = arg;

	return req;
}

void usb_request_free(struct usb_request *req) {
	struct usb_hcd *hcd = req->endp->dev->hcd;

	if (hcd->ops->req_hci_free) {
		hcd->ops->req_hci_free(req, hcd->hci_specific);
	}

	pool_free(&usb_requests, req);
}


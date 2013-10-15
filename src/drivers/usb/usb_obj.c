/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.10.2013
 */

#include <string.h>
#include <drivers/usb.h>
#include <mem/misc/pool.h>
#include <stddef.h>

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

	if (ops->hcd_hci_alloc) {
		hcd->hci_specific = ops->hcd_hci_alloc(hcd, args);
		if (!hcd->hci_specific) {
			/* XXX mem leak */
			return NULL;
		}
	}

	return hcd;
}

void usb_hcd_free(struct usb_hcd *hcd) {

	if (hcd->ops->hcd_hci_free) {
		hcd->ops->hcd_hci_free(hcd);
	}

	pool_free(&usb_hcds, hcd);
}

static void usb_hub_port_init(struct usb_hub_port *port, struct usb_hub *hub,
	       	usb_hub_port_t i) {

	port->hub = hub;
	port->idx = i;
	port->status = port->changed = 0;
	port->dev = NULL;
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

	if (!usb_endp_alloc(dev, 0, &usb_desc_endp_control_default)) {
		usb_dev_free(dev);
		return NULL;
	}

	return dev;
}

void usb_dev_free(struct usb_dev *dev) {
	pool_free(&usb_devs, dev);
	index_free(&dev->hcd->enumerator, dev->idx);
}

struct usb_endp *usb_endp_alloc(struct usb_dev *dev, unsigned int num,
		const struct usb_desc_endpoint *endp_desc) {
	struct usb_endp *endp = pool_alloc(&usb_endps);
	struct usb_hcd *hcd = dev->hcd;

	if (!endp) {
		return NULL;
	}

	endp->dev = dev;
	endp->num = num;

	usb_endp_fill_from_desc(endp, endp_desc);

	assert(dev->endpoints[num] == NULL);
	dev->endpoints[num] = endp;

	if (hcd->ops->endp_hci_alloc) {
		endp->hci_specific = hcd->ops->endp_hci_alloc(endp);
		if (!hcd->hci_specific) {
			/* XXX mem leak */
			return NULL;
		}
	}

	return endp;
}

void usb_endp_free(struct usb_endp *endp) {
	struct usb_hcd *hcd = endp->dev->hcd;

	if (hcd->ops->endp_hci_free) {
		hcd->ops->endp_hci_free(endp);
	}
}

extern struct usb_request *usb_request_alloc(struct usb_endp *endp) {
	struct usb_hcd *hcd = endp->dev->hcd;
	struct usb_request *req;

	assert(usb_endp_type(endp) == USB_COMM_CONTROL);

	req = pool_alloc(&usb_requests);
	req->endp = endp;

	if (hcd->ops->req_hci_alloc) {
		req->hci_specific = hcd->ops->req_hci_alloc(req);
		if (!req->hci_specific) {
			/* XXX mem leak */
			return NULL;
		}
	}
	return req;
}

void usb_request_free(struct usb_request *req) {
	struct usb_hcd *hcd = req->endp->dev->hcd;

	if (hcd->ops->req_hci_free) {
		hcd->ops->req_hci_free(req);
	}

	pool_free(&usb_requests, req);
}


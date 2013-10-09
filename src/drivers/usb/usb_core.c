/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.10.2013
 */

#include <errno.h>
#include <string.h>
#include <drivers/usb.h>
#include <util/member.h>
#include <util/dlist.h>
#include <mem/misc/pool.h>
#include <kernel/time/ktime.h>
#include <kernel/printk.h>

static DLIST_DEFINE(usb_hcds);

static void usb_request_build(struct usb_request *req, uint8_t req_type, uint8_t request,
			uint16_t value, uint16_t index, uint16_t count,
			void *data) {
	char *rbuf = (char *) &req->ctrl_header;

	assert(req->endp->type == USB_COMM_CONTROL);

	rbuf[0] = req_type;
	rbuf[1] = request;
	rbuf[2] = value & 0xff;
	rbuf[3] = value >> 8;
	rbuf[4] = index & 0xff;
	rbuf[5] = index >> 8;
	rbuf[6] = count & 0xff;
	rbuf[7] = count >> 8;

	req->buf = data;
	req->len = count;
}

static void usb_endp_request(struct usb_endp *endp, uint8_t req_type,
		uint8_t request, uint16_t value, uint16_t index,
		uint16_t count, void *data) {
	struct usb_hcd *hcd = endp->dev->hcd;
	struct usb_request *req;

	req = usb_request_alloc(endp);
	assert(req, "%s: allocating usb request for ", __func__);

	assert(sizeof(struct usb_desc_device) == 18);
	usb_request_build(req, req_type, request, value, index,
			count, data);
	hcd->ops->control_request(endp, req);
}

static inline void usb_dev_set_state(struct usb_dev *dev, enum usb_dev_state state) {
	dev->state = state;
}

static void usb_dev_notify(struct usb_dev *dev, usb_dev_event_t event_type,
		union usb_dev_event event) {
	struct usb_endp *ctrl_endp;

	ctrl_endp = dev->endpoints[0];

	if (event_type == USB_DEV_EVENT_PORT) {
		assert(event.port == dev->port);
	}

	switch(dev->state) {
	case USB_DEV_CONNECTED:
		if (event.port->status & USB_HUB_PORT_POWER) {
			usb_dev_set_state(dev, USB_DEV_POWERED);
		}

		break;
	case USB_DEV_POWERED:
		assert(event_type == USB_DEV_EVENT_PORT);
		if (event.port->changed & USB_HUB_PORT_RESET) {
			usb_dev_set_state(dev, USB_DEV_DEFAULT);

			usb_endp_request(ctrl_endp, USB_DEV_REQ_TYPE_WR | USB_DEV_REQ_TYPE_STD
					| USB_DEV_REQ_TYPE_DEV, USB_DEV_REQ_SET_ADDR, dev->idx,
					0, 0, NULL);
		}
		break;
	case USB_DEV_DEFAULT:
		if (event_type == USB_DEV_EVENT_REQUEST) {
			dev->bus_idx = dev->idx;
			usb_dev_set_state(dev, USB_DEV_ADDRESS);

			usb_endp_request(ctrl_endp, USB_DEV_REQ_TYPE_RD | USB_DEV_REQ_TYPE_STD
				| USB_DEV_REQ_TYPE_DEV, USB_DEV_REQ_GET_DESC, USB_DESC_TYPE_DEV,
				0, sizeof(struct usb_desc_device), &dev->dev_desc);
		}

		break;
	case USB_DEV_ADDRESS:
		printk("usb: found dev 0x%04x:0x%04x\n", dev->dev_desc.id_vendor,
				dev->dev_desc.id_product);
		break;
	default:
		break;
	}
}

static inline void usb_dev_notify_port(struct usb_dev *dev) {
	union usb_dev_event ev = { .port = dev->port };

	usb_dev_notify(dev, USB_DEV_EVENT_PORT, ev);
}

static inline void usb_dev_notify_request(struct usb_dev *dev, struct usb_request *req) {
	union usb_dev_event ev = { .req = req };

	usb_dev_notify(dev, USB_DEV_EVENT_REQUEST, ev);
}

void usb_request_complete(struct usb_request *req) {

	if (req->req_stat != USB_REQ_NOERR) {
		printk("usb_request %p: failed\n", req);
		usb_request_free(req);
	}

	usb_dev_notify_request(req->endp->dev, req);
	usb_request_free(req);
}

static void usb_hub_ctrl(struct usb_hub_port *port, enum usb_hub_request request,
		unsigned short value) {
	struct usb_hcd *hcd = port->hub->hcd;

	assert(hcd->root_hub == port->hub, "%s: only root hub is supported", __func__);

	hcd->ops->rhub_ctrl(port, request, value);
}

static void usb_port_state_changed(struct usb_hub_port *port, usb_hub_state_t state, char val) {

	if (state & USB_HUB_PORT_CONNECT) {
		assert(port->dev == NULL);
		assert(val, "%s: cannot handle disconnect", __func__);

		usb_hub_ctrl(port, USB_HUB_REQ_PORT_SET,
			USB_HUB_PORT_POWER | USB_HUB_PORT_ENABLE | USB_HUB_PORT_RESET);

		usb_port_device_bind(port, usb_dev_alloc(port->hub->hcd));
		usb_dev_notify_port(port->dev);
	}

	if (state & USB_HUB_PORT_RESET) {
		usb_dev_notify_port(port->dev);
	}
}

int usb_rh_nofity(struct usb_hcd *hcd) {
	struct usb_hub *rh = hcd->root_hub;

	for (int i = 0; i < rh->port_n; i++) {
		unsigned short port_ch = rh->ports[i].changed;
		if (port_ch & USB_HUB_PORT_CONNECT) {
			usb_port_state_changed(&rh->ports[i], USB_HUB_PORT_CONNECT,
					rh->ports[i].status & USB_HUB_PORT_CONNECT ? 1 : 0);
		}

		if (port_ch & USB_HUB_PORT_RESET) {
			usb_port_state_changed(&rh->ports[i], USB_HUB_PORT_RESET, 1);
		}
	}

	return 0;
}

int usb_hcd_register(struct usb_hcd *hcd) {
	int ret;

	dlist_add_next(&hcd->lnk, &usb_hcds);

	assert(hcd->ops->hcd_start);
	assert(hcd->ops->hcd_stop);
	assert(hcd->ops->rhub_ctrl);
	assert(hcd->ops->control_request);

	if ((ret = hcd->ops->hcd_start(hcd))) {
		return ret;
	}

	return 0;
}

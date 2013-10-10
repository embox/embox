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
#include <kernel/panic.h>

static DLIST_DEFINE(usb_hcds);

const struct usb_desc_endpoint usb_desc_endp_control_default = {
	.b_lenght = sizeof(struct usb_desc_endpoint),
	.b_desc_type = USB_DESC_TYPE_ENDPOINT,
	.b_endpoint_address = 0,
	.bm_attributes = 0,
	.w_max_packet_size = 8,
	.b_interval = 0,
};

static void usb_request_build(struct usb_request *req, uint8_t req_type,
		uint8_t request, uint16_t value, uint16_t index, uint16_t count,
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

static inline void usb_dev_set_state(struct usb_dev *dev,
		enum usb_dev_state state) {
	dev->state = state;
}

static int usb_dev_configuration_check(struct usb_dev *dev) {
	return 1;
}

static struct usb_desc_getconf_data *usb_dev_getconf_alloc(struct usb_dev *dev) {
	return dev->getconf_data = &dev->tgetconf_data;
}

static void usb_dev_getconf_free(struct usb_dev *dev) {
	dev->getconf_data = NULL;
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

			usb_endp_request(ctrl_endp, USB_DEV_REQ_TYPE_WR
					| USB_DEV_REQ_TYPE_STD
					| USB_DEV_REQ_TYPE_DEV,
				USB_DEV_REQ_SET_ADDR, dev->idx,
				0, 0, NULL);
		}
		break;
	case USB_DEV_DEFAULT:
		assert(event_type == USB_DEV_EVENT_REQUEST);
		if (event_type == USB_DEV_EVENT_REQUEST) {
			dev->bus_idx = dev->idx;
			usb_dev_set_state(dev, USB_DEV_ADDRESS);

			usb_endp_request(ctrl_endp, USB_DEV_REQ_TYPE_RD
					| USB_DEV_REQ_TYPE_STD
					| USB_DEV_REQ_TYPE_DEV,
				USB_DEV_REQ_GET_DESC, USB_DESC_TYPE_DEV << 8,
				0, sizeof(struct usb_desc_device),
				&dev->dev_desc);

		}

		break;
	case USB_DEV_ADDRESS:
		assert(event_type == USB_DEV_EVENT_REQUEST);
		if (event_type != USB_DEV_EVENT_REQUEST) {
			break;
		}

		if (event.req->buf == (void *) &dev->dev_desc) {
			dev->c_config = 1;

			if (NULL == usb_dev_getconf_alloc(dev)) {
				panic("%s: failed to allocate device's "
						"getconf_data\n", __func__);
			}

			usb_endp_request(ctrl_endp, USB_DEV_REQ_TYPE_RD
				| USB_DEV_REQ_TYPE_STD
				| USB_DEV_REQ_TYPE_DEV, USB_DEV_REQ_GET_DESC,
				USB_DESC_TYPE_CONFIG << 8,
				dev->c_config,
				sizeof(struct usb_desc_configuration) +
					sizeof(struct usb_desc_interface),
				dev->getconf_data);
		} else if (event.req->buf == (void *) dev->getconf_data) {
			if (!usb_dev_configuration_check(dev)) {
				int last_config_n = dev->dev_desc.i_num_configurations;
				if (dev->c_config >= last_config_n) {
					panic("%s: cannot find appropriate "
							"configuration", __func__);
				}

				dev->c_config += 1;
				usb_endp_request(ctrl_endp, USB_DEV_REQ_TYPE_RD
							| USB_DEV_REQ_TYPE_STD
							| USB_DEV_REQ_TYPE_DEV,
						USB_DEV_REQ_GET_DESC,
						USB_DESC_TYPE_CONFIG << 8 ,
						dev->c_config,
						sizeof(struct usb_desc_configuration) +
							sizeof(struct usb_desc_interface),
						dev->getconf_data);
			} else {
				dev->c_interface = 0;
				usb_endp_request(ctrl_endp, USB_DEV_REQ_TYPE_WR
						| USB_DEV_REQ_TYPE_STD
						| USB_DEV_REQ_TYPE_DEV,
					USB_DEV_REQ_SET_CONF,
					dev->getconf_data->config_desc.b_configuration_value,
					0, 0, NULL);

			}
		} else if (event.req->buf == NULL) {
			switch (event.req->ctrl_header.b_request) {
			case USB_DEV_REQ_SET_CONF:
				usb_dev_set_state(dev, USB_DEV_CONFIGURED);
				dev->endp_n = 1 + dev->getconf_data->interface_desc.b_num_endpoints;
				usb_endp_request(ctrl_endp, USB_DEV_REQ_TYPE_RD
						| USB_DEV_REQ_TYPE_STD
						| USB_DEV_REQ_TYPE_DEV,
					USB_DEV_REQ_GET_DESC,
					USB_DESC_TYPE_CONFIG << 8,
					dev->c_config,
					sizeof(struct usb_desc_configuration) +
						sizeof(struct usb_desc_interface) +
						dev->endp_n * sizeof(struct usb_desc_endpoint),
					dev->getconf_data);
				break;
			default:
				break;
			}
		} else {
			panic("%s: strange request is done\n", __func__);
		}

		break;
	case USB_DEV_CONFIGURED:
		if (event_type == USB_DEV_EVENT_REQUEST
				&& event.req->buf == (void *) dev->getconf_data) {

			for (int i = 1; i < dev->endp_n; i++) {
				struct usb_desc_endpoint *endp_desc =
					&dev->getconf_data->endp_descs[i - 1];

				if (NULL == usb_endp_alloc(dev, i, endp_desc)) {
					panic("%s: failed to alloc endpoint\n",
						       	__func__);
				}
			}

			usb_dev_getconf_free(dev);
		}

		break;
	default:
		break;
	}
}

static inline void usb_dev_notify_port(struct usb_dev *dev) {
	union usb_dev_event ev = { .port = dev->port };

	usb_dev_notify(dev, USB_DEV_EVENT_PORT, ev);
}

static inline void usb_dev_notify_request(struct usb_dev *dev,
		struct usb_request *req) {
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

	assert(hcd->root_hub == port->hub,
			"%s: only root hub is supported", __func__);

	hcd->ops->rhub_ctrl(port, request, value);
}

static void usb_port_state_changed(struct usb_hub_port *port,
		usb_hub_state_t state, char val) {

	if (state & USB_HUB_PORT_CONNECT) {
		assert(port->dev == NULL);
		assert(val, "%s: cannot handle disconnect", __func__);

		usb_hub_ctrl(port, USB_HUB_REQ_PORT_SET,
			USB_HUB_PORT_POWER | USB_HUB_PORT_ENABLE
			| USB_HUB_PORT_RESET);

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
			usb_port_state_changed(&rh->ports[i],
					USB_HUB_PORT_CONNECT,
					rh->ports[i].status & USB_HUB_PORT_CONNECT ? 1 : 0);
		}

		if (port_ch & USB_HUB_PORT_RESET) {
			usb_port_state_changed(&rh->ports[i],
					USB_HUB_PORT_RESET, 1);
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

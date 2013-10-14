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

static inline void usb_dev_notify_posted(struct usb_dev *dev);
static inline void usb_dev_notify_port(struct usb_dev *dev);
static int usb_dev_post(struct usb_dev *dev, unsigned int ms,
		usb_dev_notify_hnd_t notify_hnd);

static void usb_dev_wait(struct usb_dev *dev, usb_dev_notify_hnd_t notify_hnd) {
	assert(dev->notify_hnd == NULL);

	dev->notify_hnd = notify_hnd;
}

static void usb_hub_ctrl(struct usb_hub_port *port, enum usb_hub_request request,
		unsigned short value);

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

static void usb_endp_request(struct usb_endp *endp, usb_request_notify_hnd_t notify_hnd,
		uint8_t req_type, uint8_t request, uint16_t value, uint16_t index,
		uint16_t count, void *data) {
	struct usb_hcd *hcd = endp->dev->hcd;
	struct usb_request *req;

	req = usb_request_alloc(endp);
	assert(req, "%s: allocating usb request failed", __func__);

	usb_request_build(req, req_type, request, value, index,
			count, data);
	req->notify_hnd = notify_hnd;
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

static void usb_dev_request_hnd_set_addr(struct usb_request *req);
static void usb_dev_request_hnd_dev_desc(struct usb_request *req);
static void usb_dev_request_hnd_conf_header(struct usb_request *req);
static void usb_dev_request_hnd_set_conf(struct usb_request *req);
static void usb_dev_request_hnd_get_conf(struct usb_request *req);

static void usb_dev_request_hnd_set_addr(struct usb_request *req) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_endp *ctrl_endp;

	ctrl_endp = dev->endpoints[0];

	dev->bus_idx = dev->idx;
	usb_dev_set_state(dev, USB_DEV_ADDRESS);

	usb_endp_request(ctrl_endp, usb_dev_request_hnd_dev_desc,
		USB_DEV_REQ_TYPE_RD
			| USB_DEV_REQ_TYPE_STD
			| USB_DEV_REQ_TYPE_DEV,
		USB_DEV_REQ_GET_DESC, USB_DESC_TYPE_DEV << 8,
		0, sizeof(struct usb_desc_device),
		&dev->dev_desc);
}

static void usb_dev_request_hnd_dev_desc(struct usb_request *req) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_endp *ctrl_endp;

	ctrl_endp = dev->endpoints[0];

	dev->c_config = 1;

	if (NULL == usb_dev_getconf_alloc(dev)) {
		panic("%s: failed to allocate device's "
				"getconf_data\n", __func__);
	}

	usb_endp_request(ctrl_endp, usb_dev_request_hnd_conf_header,
		USB_DEV_REQ_TYPE_RD
		| USB_DEV_REQ_TYPE_STD
		| USB_DEV_REQ_TYPE_DEV, USB_DEV_REQ_GET_DESC,
		USB_DESC_TYPE_CONFIG << 8,
		dev->c_config,
		sizeof(struct usb_desc_configuration) +
			sizeof(struct usb_desc_interface),
		dev->getconf_data);
}

static void usb_dev_request_hnd_conf_header(struct usb_request *req) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_endp *ctrl_endp;

	ctrl_endp = dev->endpoints[0];

	if (!usb_dev_configuration_check(dev)) {
		int last_config_n = dev->dev_desc.i_num_configurations;
		if (dev->c_config >= last_config_n) {
			panic("%s: cannot find appropriate "
					"configuration", __func__);
		}

		dev->c_config += 1;
		usb_endp_request(ctrl_endp, usb_dev_request_hnd_conf_header,
			USB_DEV_REQ_TYPE_RD
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
		usb_endp_request(ctrl_endp, usb_dev_request_hnd_set_conf,
			USB_DEV_REQ_TYPE_WR
				| USB_DEV_REQ_TYPE_STD
				| USB_DEV_REQ_TYPE_DEV,
			USB_DEV_REQ_SET_CONF,
			dev->getconf_data->config_desc.b_configuration_value,
			0, 0, NULL);

	}
}

static void usb_dev_request_hnd_set_conf(struct usb_request *req) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_endp *ctrl_endp;

	ctrl_endp = dev->endpoints[0];

	assert(req->ctrl_header.b_request == USB_DEV_REQ_SET_CONF);

	usb_dev_set_state(dev, USB_DEV_CONFIGURED);
	dev->endp_n = 1 + dev->getconf_data->interface_desc.b_num_endpoints;
	usb_endp_request(ctrl_endp, usb_dev_request_hnd_get_conf,
		USB_DEV_REQ_TYPE_RD
			| USB_DEV_REQ_TYPE_STD
			| USB_DEV_REQ_TYPE_DEV,
		USB_DEV_REQ_GET_DESC,
		USB_DESC_TYPE_CONFIG << 8,
		dev->c_config,
		sizeof(struct usb_desc_configuration) +
			sizeof(struct usb_desc_interface) +
			dev->endp_n * sizeof(struct usb_desc_endpoint),
		dev->getconf_data);

}

static void usb_dev_request_hnd_get_conf(struct usb_request *req) {
	struct usb_dev *dev = req->endp->dev;

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

static void usb_dev_notify_connected_delay(struct usb_dev *dev, enum usb_dev_event_type event_type);
static void usb_dev_notify_connected(struct usb_dev *dev, enum usb_dev_event_type event_type);
static void usb_dev_notify_reset_awaiting(struct usb_dev *dev, enum usb_dev_event_type event_type);
static void usb_dev_notify_reseted_delay(struct usb_dev *dev, enum usb_dev_event_type event_type);

static void usb_dev_notify_connected(struct usb_dev *dev, enum usb_dev_event_type event_type) {
	assert(event_type == USB_DEV_EVENT_PORT);

	if (dev->port->status & USB_HUB_PORT_POWER) {
		usb_dev_post(dev, 100, usb_dev_notify_connected_delay);
	}
}

static void usb_dev_notify_connected_delay(struct usb_dev *dev, enum usb_dev_event_type event_type) {

	if (event_type == USB_DEV_EVENT_POSTED) {
		usb_dev_set_state(dev, USB_DEV_POWERED);
		usb_dev_wait(dev, usb_dev_notify_reset_awaiting);
		usb_hub_ctrl(dev->port, USB_HUB_REQ_PORT_SET, USB_HUB_PORT_RESET);
	} else if (event_type == USB_DEV_EVENT_PORT) {
		/* handle dither */
		printk("%s: port event: status=%08x\n", __func__, dev->port->status);
	}
}

static void usb_dev_notify_reset_awaiting(struct usb_dev *dev, enum usb_dev_event_type event_type) {

	assert(event_type == USB_DEV_EVENT_PORT);

	if (event_type == USB_DEV_EVENT_PORT) {
		if (dev->port->changed & USB_HUB_PORT_RESET) {
			usb_dev_set_state(dev, USB_DEV_DEFAULT);
/*FIXME*/
#if 0
			usb_dev_post(dev, 10, usb_dev_notify_reseted_delay);
#else
			usb_dev_wait(dev, usb_dev_notify_reseted_delay);
			usb_dev_notify_posted(dev);
#endif
		}
	}
}

static void usb_dev_notify_reseted_delay(struct usb_dev *dev, enum usb_dev_event_type event_type) {

	assert(event_type == USB_DEV_EVENT_POSTED);

	usb_endp_request(dev->endpoints[0], usb_dev_request_hnd_set_addr,
		USB_DEV_REQ_TYPE_WR
			| USB_DEV_REQ_TYPE_STD
			| USB_DEV_REQ_TYPE_DEV,
		USB_DEV_REQ_SET_ADDR, dev->idx,
		0, 0, NULL);
}

static inline void usb_dev_notify(struct usb_dev *dev, enum usb_dev_event_type type) {
	usb_dev_notify_hnd_t notify_hnd = dev->notify_hnd;

	assert(notify_hnd);

	dev->notify_hnd = NULL;

	notify_hnd(dev, type);
}

static inline void usb_dev_notify_port(struct usb_dev *dev) {

	usb_dev_notify(dev, USB_DEV_EVENT_PORT);
}

static inline void usb_dev_notify_posted(struct usb_dev *dev) {

	usb_dev_notify(dev, USB_DEV_EVENT_POSTED);
}

static void usb_dev_posted_handle(struct sys_timer *timer, void *param) {
	struct usb_dev *dev = param;

	usb_dev_notify_posted(dev);
}

static int usb_dev_post(struct usb_dev *dev, unsigned int ms,
		usb_dev_notify_hnd_t notify_hnd) {

	usb_dev_wait(dev, notify_hnd);
	return timer_init(&dev->post_timer, TIMER_ONESHOT, ms, usb_dev_posted_handle,
			dev);
}

static void __attribute__((used)) usb_dev_post_cancel(struct usb_dev *dev) {

	timer_close(&dev->post_timer);
	dev->notify_hnd = NULL;
}

void usb_request_complete(struct usb_request *req) {

	if (req->req_stat != USB_REQ_NOERR) {
		printk("usb_request %p: failed\n", req);
	}

	assert(req->notify_hnd);
	req->notify_hnd(req);
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
		struct usb_dev *dev;

		assert(port->dev == NULL);
		assert(val, "%s: cannot handle disconnect", __func__);

		usb_hub_ctrl(port, USB_HUB_REQ_PORT_SET,
			USB_HUB_PORT_POWER | USB_HUB_PORT_ENABLE);

		dev = usb_dev_alloc(port->hub->hcd);
		usb_port_device_bind(port, dev);
		usb_dev_wait(dev, usb_dev_notify_connected);
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

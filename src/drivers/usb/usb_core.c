/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.10.2013
 */

#include <errno.h>
#include <string.h>
#include <util/member.h>
#include <util/dlist.h>
#include <mem/misc/pool.h>
#include <kernel/irq_lock.h>
#include <kernel/time/ktime.h>
#include <kernel/printk.h>
#include <kernel/panic.h>
#include <drivers/usb/usb_driver.h>

#include <drivers/usb/usb.h>

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

}

static int usb_endp_do_req(struct usb_endp *endp) {
	struct usb_queue_link *l;
	struct usb_request *req;
	struct usb_hcd *hcd;

	l = usb_queue_peek(&endp->req_queue);
	if (!l) {
		return 0;
	}

	req = member_cast_out(l, struct usb_request, req_link);

	hcd = req->endp->dev->hcd;
	hcd->ops->request(req);

	return 0;
}

int usb_endp_request(struct usb_endp *endp, struct usb_request *req) {
	bool endp_busy;

	endp_busy = usb_queue_add(&endp->req_queue, &req->req_link);
	if (!endp_busy) {
		return usb_endp_do_req(endp);
	}

	return 0;
}

void usb_request_complete(struct usb_request *req) {
	struct usb_endp *endp = req->endp;

	if (req->req_stat != USB_REQ_NOERR) {
		printk("usb_request %p: failed\n", req);
	}

	if (req->notify_hnd) {
		req->notify_hnd(req, req->hnd_data);
	}

	usb_queue_done(&endp->req_queue, &req->req_link);
	usb_request_free(req);

	usb_endp_do_req(endp);
}

static unsigned short usb_endp_dir_token_map(struct usb_endp *endp) {
	switch (endp->direction) {
	case USB_DIRECTION_IN:
		return USB_TOKEN_IN;
	case USB_DIRECTION_OUT:
		return USB_TOKEN_OUT;
	default:
		break;
	}
	panic("%s: can't map unidirection endpoint to token\n", __func__);
}

int usb_endp_interrupt(struct usb_endp *endp, usb_request_notify_hnd_t notify_hnd,
		void *buf, size_t len) {
	struct usb_request *req;

	assert(usb_endp_type(endp) == USB_COMM_INTERRUPT);

	req = usb_endp_request_alloc(endp, notify_hnd, NULL,
			usb_endp_dir_token_map(endp), buf, len);

	return usb_endp_request(endp, req);
}

int usb_endp_control(struct usb_endp *endp, usb_request_notify_hnd_t notify_hnd,
		uint8_t req_type, uint8_t request, uint16_t value, uint16_t index,
		uint16_t count, void *data) {
	struct usb_request *rstp, *rdt = NULL, *rstt;
	unsigned short dtoken, dntoken;

	if (req_type & USB_DEV_REQ_TYPE_RD) {
		dtoken = USB_TOKEN_IN;
		dntoken = USB_TOKEN_OUT;
	} else {
		dtoken = USB_TOKEN_OUT;
		dntoken = USB_TOKEN_IN;
	}

	assert(usb_endp_type(endp) == USB_COMM_CONTROL);

	rstp = usb_endp_request_alloc(endp, NULL, NULL, USB_TOKEN_SETUP | USB_TOKEN_OUT,
			NULL, sizeof(struct usb_control_header));
	rstp->buf = (void *) &rstp->ctrl_header;
	usb_request_build(rstp, req_type, request, value, index,
			count, data);

	if (count) {
		rdt = usb_endp_request_alloc(endp, NULL, NULL, dtoken, data, count);
		if (!rdt) {
			goto out1;
		}
	}

	rstt = usb_endp_request_alloc(endp, notify_hnd, NULL, USB_TOKEN_STATUS | dntoken,
		       	NULL, 0);
	if (!rstt) {
		goto out2;
	}

	usb_endp_request(endp, rstp);
	if (count) {
		usb_endp_request(endp, rdt);
	}
	usb_endp_request(endp, rstt);

	return 0;

out2:
	if (count) {
		usb_request_free(rdt);
	}
out1:
	usb_request_free(rstp);

	return -ENOMEM;
}

int usb_endp_bulk(struct usb_endp *endp, usb_request_notify_hnd_t notify_hnd,
		void *buf, size_t len) {
	struct usb_request *req;

	assert(usb_endp_type(endp) == USB_COMM_BULK);

	req = usb_endp_request_alloc(endp, notify_hnd, NULL,
			usb_endp_dir_token_map(endp), buf, len);

	return usb_endp_request(endp, req);
}

static inline void usb_dev_set_state(struct usb_dev *dev,
		enum usb_dev_state state) {
	dev->state = state;
}

static int usb_dev_configuration_check(struct usb_dev *dev) {
	return 1;
	/*return usb_class_supported(dev);*/
}

static struct usb_desc_getconf_data *usb_dev_getconf_alloc(struct usb_dev *dev) {
	return dev->getconf_data = &dev->tgetconf_data;
}

static __attribute__((used)) void usb_dev_getconf_free(struct usb_dev *dev) {
	dev->getconf_data = NULL;
}

static void usb_dev_request_hnd_set_addr(struct usb_request *req, void *arg);
static void usb_dev_request_hnd_dev_desc(struct usb_request *req, void *arg);
static void usb_dev_request_hnd_conf_header(struct usb_request *req, void *arg);
static void usb_dev_request_hnd_set_conf(struct usb_request *req, void *arg);

void usb_dev_addr_assign(struct usb_dev *dev) {

	usb_endp_control(dev->endpoints[0], usb_dev_request_hnd_set_addr,
		USB_DEV_REQ_TYPE_WR
			| USB_DEV_REQ_TYPE_STD
			| USB_DEV_REQ_TYPE_DEV,
		USB_DEV_REQ_SET_ADDR, dev->idx,
		0, 0, NULL);
}

static __attribute__((used)) void usb_dev_request_hnd_set_addr(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_endp *ctrl_endp;

	ctrl_endp = dev->endpoints[0];

	dev->bus_idx = dev->idx;
	usb_dev_set_state(dev, USB_DEV_ADDRESS);
	usb_dev_addr_assigned(dev);

	usb_endp_control(ctrl_endp, usb_dev_request_hnd_dev_desc,
		USB_DEV_REQ_TYPE_RD
			| USB_DEV_REQ_TYPE_STD
			| USB_DEV_REQ_TYPE_DEV,
		USB_DEV_REQ_GET_DESC, USB_DESC_TYPE_DEV << 8,
		0, sizeof(struct usb_desc_device),
		&dev->dev_desc);
}

static void usb_dev_request_hnd_dev_desc(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_endp *ctrl_endp;

	ctrl_endp = dev->endpoints[0];

	dev->c_config = 0;

	if (NULL == usb_dev_getconf_alloc(dev)) {
		panic("%s: failed to allocate device's "
				"getconf_data\n", __func__);
	}

	printk("usb_core: found vendor=%04x product=%04x; initializing\n",
			dev->dev_desc.id_vendor, dev->dev_desc.id_product);

	usb_endp_control(ctrl_endp, usb_dev_request_hnd_conf_header,
		USB_DEV_REQ_TYPE_RD
		| USB_DEV_REQ_TYPE_STD
		| USB_DEV_REQ_TYPE_DEV, USB_DEV_REQ_GET_DESC,
		USB_DESC_TYPE_CONFIG << 8,
		dev->c_config,
		sizeof(struct usb_desc_configuration) +
			sizeof(struct usb_desc_interface),
		dev->getconf_data);
}

static void usb_dev_request_hnd_conf_header(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_endp *ctrl_endp;

	ctrl_endp = dev->endpoints[0];

	dev->interface_desc = &dev->getconf_data->interface_desc;

	if (!usb_dev_configuration_check(dev)) {
		int last_config_n = dev->dev_desc.i_num_configurations;
		if (dev->c_config >= last_config_n) {
			panic("%s: cannot find appropriate "
					"configuration", __func__);
		}

		dev->c_config += 1;
		usb_endp_control(ctrl_endp, usb_dev_request_hnd_conf_header,
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
		usb_endp_control(ctrl_endp, usb_dev_request_hnd_set_conf,
			USB_DEV_REQ_TYPE_WR
				| USB_DEV_REQ_TYPE_STD
				| USB_DEV_REQ_TYPE_DEV,
			USB_DEV_REQ_SET_CONF,
			dev->getconf_data->config_desc.b_configuration_value,
			0, 0, NULL);

	}
}

static void usb_dev_request_hnd_set_conf(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;

	usb_dev_set_state(dev, USB_DEV_CONFIGURED);

	dev->endp_n = 1 + dev->interface_desc->b_num_endpoints;

	usb_dev_configured(dev);
}

void usb_hub_ctrl(struct usb_hub_port *port, enum usb_hub_request request,
		unsigned short value) {
	struct usb_hcd *hcd = port->hub->hcd;

	assert(hcd->root_hub == port->hub,
			"%s: only root hub is supported", __func__);

	hcd->ops->rhub_ctrl(port, request, value);
}

static void usb_port_state_changed(struct usb_hub_port *port,
		usb_hub_state_t state, char val) {

	if (state & USB_HUB_PORT_CONNECT) {

		usb_hub_ctrl(port, USB_HUB_REQ_PORT_SET,
			USB_HUB_PORT_POWER | USB_HUB_PORT_ENABLE);

		if (val) {
			usb_port_attached(port);
		} else {
			usb_port_detached(port);
		}
	}

#if 0
	if (state & USB_HUB_PORT_RESET) {
		usb_dev_notify_port(port->dev);
	}
#endif
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
	assert(hcd->ops->request);

	if ((ret = hcd->ops->hcd_start(hcd))) {
		return ret;
	}

	return 0;
}


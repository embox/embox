/**
 * @file
 * @brief
 *    For more details about configurations and interfaces see
 *    usb specification 1.1 chapter 9.
 *
 * @author  Alex Kalmuk
 * @date    05.04.2014
 */

#include <mem/misc/pool.h>
#include <mem/sysmalloc.h>
#include <kernel/printk.h>
#include <kernel/panic.h>
#include <embox/unit.h>
#include <drivers/usb/usb_cdc.h>
#include <drivers/usb/usb_driver.h>
#include <kernel/printk.h>
#include <stdlib.h>
#include <string.h>

EMBOX_UNIT_INIT(usb_cdc_init);

POOL_DEF(cdc_classes, struct usb_class_cdc, USB_CDC_MAX_DEVS);

/* FIXME store all static data in pool */

/* Storage for device configuration*/
static char cdc_conf[16];

#define INTERFACE_DESC_OFFSET          (sizeof (struct usb_desc_configuration))
#define FUNC_DESC_OFFSET               (INTERFACE_DESC_OFFSET + \
                                           sizeof (struct usb_desc_interface))
#define CS_INTERFACE                   0x24

/* Configuration */
static void cdc_get_conf_len_hnd(struct usb_request *req, void *arg);
static void cdc_get_conf(struct usb_dev *dev, struct usb_class_cdc *cdc,
		size_t idx, size_t len, usb_request_notify_hnd_t cb);
static void cdc_get_conf_content_hnd(struct usb_request *req, void *arg);
static void cdc_get_conf_hnd(struct usb_request *req, void *arg);
static void cdc_next_conf(struct usb_dev *dev);

/* Interface */
void cdc_set_interface(struct usb_dev *dev, size_t iface,
		size_t alternated_cfg, usb_request_notify_hnd_t cb);
struct usb_desc_interface *cdc_get_interface(void *conf, size_t index);
static void cdc_set_interface_hnd(struct usb_request *req, void *arg);

/* Endpoint */
static size_t cdc_skip_func_descs(void *start);
static size_t cdc_skip_endpoints(void *start);

/* usb-net specific */
static void usb_net_hook(struct usb_dev *dev, struct usb_class_cdc *cdc);
static int is_rndis(struct usb_desc_interface *desc);

/**
 * Skip all functional descriptors.
 * Return size of all functional descriptors.
 * @see ecm120.pdf (Subclass Specification for
 *  Ethernet Control Model Devices) Section 5
 * XXX skip is a default action now, but it is possible to redefine this behavior
 */
static size_t cdc_skip_func_descs(void *start) {
	uint8_t *cur, *func_desc_start;

	func_desc_start = start;
	cur = func_desc_start;

	while (1) {
		uint8_t len, type;

		type = *(cur + 1);
		if (type == CS_INTERFACE) {
			len = *cur;
			cur += len;
		} else {
			break;
		}
	}

	return (cur - func_desc_start);
}

/*
 * @see ecm120.pdf (Subclass Specification for
 *  Ethernet Control Model Devices) Section 5
 */
static size_t cdc_skip_endpoints(void *start) {
	uint8_t *cur, *func_desc_start;

	func_desc_start = start;
	cur = func_desc_start;

	while (1) {
		uint8_t len, type;

		type = *(cur + 1);
		if (type == USB_DESC_TYPE_ENDPOINT) {
			len = *cur;
			cur += len;
		} else {
			break;
		}
	}

	return (cur - func_desc_start);
}

struct usb_desc_interface *cdc_get_interface(void *conf, size_t index) {
	size_t cur = 0;
	char *curp = conf + INTERFACE_DESC_OFFSET;

	while (cur != index) {
		curp += sizeof (struct usb_desc_interface);
		curp += cdc_skip_func_descs(curp);
		curp += cdc_skip_endpoints(curp);
		cur++;
	}

	return (struct usb_desc_interface *) curp;
}

/*
 * TODO It is a toy function to get usb_net interface properly. In future it must be moved into
 * separate file.
 */
static void usb_net_hook(struct usb_dev *dev, struct usb_class_cdc *cdc) {
	struct usb_desc_endpoint *ep;
	struct usb_desc_interface *data_iface;
	size_t i;

	/* Get the second interface (Data Interface) for usb-net*/
	data_iface = cdc_get_interface(cdc->getconf, 2);
	/* Fill in the IN and OUT endpoints */
	for (i = 0; i < 2; i ++) {
		ep = (struct usb_desc_endpoint *) ((char *)data_iface +
				sizeof (struct usb_desc_interface) + i * sizeof (struct usb_desc_endpoint));
		assert(usb_endp_alloc(dev, ep));
	}
}

static void cdc_get_conf_hnd(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_class_cdc *cdc = usb2cdcdata(dev);
	size_t func_desc_len;
	struct usb_desc_interface *iface;

	iface = cdc_get_interface(cdc->getconf, 0);

	/* Check if the first interface is one of Microsoft's pet,
	 * and if so then try to skip it. */
	if (is_rndis(iface)) {
		sysfree(cdc->getconf);
		cdc_next_conf(dev);
		return;
	}

	func_desc_len = cdc_skip_func_descs(cdc->getconf + INTERFACE_DESC_OFFSET +
			sizeof (struct usb_desc_interface));

	usb_dev_generic_fill_iface(dev, cdc->getconf + INTERFACE_DESC_OFFSET);
	usb_dev_generic_fill_endps(dev, cdc->getconf + FUNC_DESC_OFFSET + func_desc_len);

	/* XXX */
	usb_net_hook(dev, cdc);

	usb_driver_handle(req->endp->dev);

	/* TODO free resources */

	usb_class_start_handle(dev);
}

static void cdc_get_conf_len_hnd(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;

	usb_endp_control(dev->endpoints[0], cdc_get_conf_content_hnd, NULL,
		USB_DEV_REQ_TYPE_RD
			| USB_DEV_REQ_TYPE_STD
			| USB_DEV_REQ_TYPE_DEV,
		USB_DEV_REQ_GET_DESC,
		(USB_DESC_TYPE_CONFIG << 8) | 1,
		dev->c_config,
		9,
		cdc_conf);
}

static void cdc_get_conf(struct usb_dev *dev, struct usb_class_cdc *cdc,
		size_t idx, size_t len, usb_request_notify_hnd_t cb) {
	usb_endp_control(dev->endpoints[0], cb, NULL,
		USB_DEV_REQ_TYPE_RD
			| USB_DEV_REQ_TYPE_STD
			| USB_DEV_REQ_TYPE_DEV,
		USB_DEV_REQ_GET_DESC,
		idx,
		dev->c_config,
		len,
		cdc->getconf);
}

static void cdc_get_conf_content_hnd(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_class_cdc *cdc = usb2cdcdata(dev);
	struct usb_desc_configuration *c;

	c = (struct usb_desc_configuration*) cdc_conf;

	cdc->getconf = sysmalloc(c->w_total_length);

	cdc_get_conf(dev, cdc, (USB_DESC_TYPE_CONFIG << 8) | 1,
			c->w_total_length, cdc_set_interface_hnd);
}

void cdc_set_interface(struct usb_dev *dev, size_t iface,
		size_t alternated_cfg, usb_request_notify_hnd_t cb) {
	/* Set CDC Data Interface */
	usb_endp_control(dev->endpoints[0], cb, NULL,
			USB_DEV_REQ_TYPE_WR
			| USB_DEV_REQ_TYPE_STD
			| USB_DEV_REQ_TYPE_IFC,
		0xB,
		iface,
		alternated_cfg,
		0,
		NULL);
}

static void cdc_set_interface_hnd(struct usb_request *req, void *arg) {
	cdc_set_interface(req->endp->dev, 1, 1, cdc_get_conf_hnd);
}

static int is_rndis(struct usb_desc_interface *desc) {
	return desc->b_interface_class == 2 /* USB_CLASS_COMM */
			&& desc->b_interface_subclass == 2
			&& desc->b_interface_protocol == 0xff;
}

static void cdc_next_conf(struct usb_dev *dev) {
	size_t nconfigs;
	struct usb_class_cdc *cdc = usb2cdcdata(dev);

	nconfigs = dev->dev_desc.i_num_configurations;

	if (cdc->current_conf >= nconfigs) {
		return;
	}

	usb_endp_control(dev->endpoints[0], cdc_get_conf_len_hnd, NULL,
		USB_DEV_REQ_TYPE_WR
			| USB_DEV_REQ_TYPE_STD
			| USB_DEV_REQ_TYPE_DEV,
		USB_DEV_REQ_SET_CONF,
		++cdc->current_conf,
		0,
		0,
		NULL);
}

static int usb_class_cdc_get_conf(struct usb_class *cls, struct usb_dev *dev) {
	cdc_next_conf(dev);
	return 0;
}

static void *usb_class_cdc_alloc(struct usb_class *cls, struct usb_dev *dev) {
	struct usb_class_cdc *cdc;

	cdc = pool_alloc(&cdc_classes);
	if (cdc) {
		cdc->current_conf = 0; /* Default configuration is 0 */
	}

	return cdc;
}

static void usb_class_cdc_free(struct usb_class *cls, struct usb_dev *dev, void *spec) {
	pool_free(&cdc_classes, spec);
}

static struct usb_class usb_class_cdc = {
	.usb_class = USB_CLASS_CDC,
	.class_alloc = usb_class_cdc_alloc,
	.class_free = usb_class_cdc_free,
	.get_conf = usb_class_cdc_get_conf,
	.get_conf_hnd = cdc_get_conf_hnd,
};

static int usb_cdc_init(void) {
	return usb_class_register(&usb_class_cdc);
}

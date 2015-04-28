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
#include <kernel/printk.h>
#include <stdlib.h>
#include <string.h>

EMBOX_UNIT_INIT(usb_cdc_init);

POOL_DEF(cdc_classes, struct usb_class_cdc, USB_CDC_MAX_DEVS);

struct usb_desc_configuration config_desc;
struct usb_desc_interface interface_desc;

/* FIXME store all static data in pool */

/* Storage for device configuration*/
static char cdc_conf[16];
static char *cdc_sendbuf;

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

/* Interface */
static void cdc_set_interface(struct usb_dev *dev, size_t iface,
		size_t alternated_cfg, usb_request_notify_hnd_t cb);
void *cdc_get_interface(void *conf, size_t index);
static void cdc_set_interface_hnd(struct usb_request *req, void *arg);

/* Endpoint */
static size_t cdc_skip_func_descs(void *start);
static size_t cdc_skip_endpoints(void *start);

/* usb-net specific */
static void usb_net_hook(struct usb_dev *dev, struct usb_class_cdc *cdc);
static void usb_net_bulk_send(struct usb_dev *dev);
static void usb_net_send_notify_hnd(struct usb_request *req, void *arg);

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

void *cdc_get_interface(void *conf, size_t index) {
	size_t cur = 0;
	char *curp = conf + INTERFACE_DESC_OFFSET;

	while (cur != index) {
		curp += sizeof (struct usb_desc_interface);
		curp += cdc_skip_func_descs(curp);
		curp += cdc_skip_endpoints(curp);
		cur++;
	}

	return curp;
}

/*
 * TODO It is a toy function to get usb_net interface properly. In future it must be moved into
 * separate file.
 */
static void usb_net_hook(struct usb_dev *dev, struct usb_class_cdc *cdc) {
	struct usb_desc_endpoint *ep;
	struct usb_endp *data_ep;
	struct usb_desc_interface *data_iface;

	/* Get the second interface (Data Interface) for usb-net*/
	data_iface = cdc_get_interface(cdc->getconf, 2);
	ep = (struct usb_desc_endpoint *) ((char *)data_iface +
			sizeof (struct usb_desc_interface) + 1 * sizeof (struct usb_desc_endpoint));
	data_ep = usb_endp_alloc(dev, ep);
	assert(data_ep);
}

static void usb_net_send_notify_hnd(struct usb_request *req, void *arg) {
	printk("\nSENT!\n");
	sysfree(cdc_sendbuf);
}

/*
 * Send 64 bytes of raw data
 */
static void usb_net_bulk_send(struct usb_dev *dev) {
	struct usb_endp *intr_endp;

	cdc_sendbuf = sysmalloc(64);

	memset(cdc_sendbuf, 0xe4, 64);

	intr_endp = dev->endpoints[2];

	usb_endp_bulk(intr_endp, usb_net_send_notify_hnd, cdc_sendbuf,
			intr_endp->max_packet_size - 4);
}

static void cdc_get_conf_hnd(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_class_cdc *cdc = usb2cdcdata(dev);
	size_t func_desc_len;

	printk("%s: in\n", __func__);

	func_desc_len = cdc_skip_func_descs(cdc->getconf + INTERFACE_DESC_OFFSET +
			sizeof (struct usb_desc_interface));

	usb_dev_generic_fill_iface(dev, cdc->getconf + INTERFACE_DESC_OFFSET);
	usb_dev_generic_fill_endps(dev, cdc->getconf + FUNC_DESC_OFFSET + func_desc_len);

	/* XXX */
	usb_net_hook(dev, cdc);

	usb_net_bulk_send(dev);

	/* TODO free resources */

	usb_class_start_handle(dev);

	printk("%s: out\n", __func__);
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
			c->w_total_length, cdc_get_conf_hnd);
}

static void cdc_set_interface(struct usb_dev *dev, size_t iface,
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
	cdc_set_interface(req->endp->dev, 1, 1, cdc_get_conf_len_hnd);
}

static int usb_class_cdc_get_conf(struct usb_class *cls, struct usb_dev *dev) {
	usb_endp_control(dev->endpoints[0], cdc_set_interface_hnd, NULL,
		USB_DEV_REQ_TYPE_WR
			| USB_DEV_REQ_TYPE_STD
			| USB_DEV_REQ_TYPE_DEV,
		USB_DEV_REQ_SET_CONF,
		1,
		0,
		0,
		NULL);

	return 0;
}

static void *usb_class_cdc_alloc(struct usb_class *cls, struct usb_dev *dev) {
	return pool_alloc(&cdc_classes);
}

static void usb_class_cdc_free(struct usb_class *cls, struct usb_dev *dev, void *spec) {
	pool_free(&cdc_classes, spec);
}

static struct usb_class usb_class_cdc = {
	.usb_class = USB_CLASS_CDC,
#if 1
	.class_alloc = usb_class_cdc_alloc,
	.class_free = usb_class_cdc_free,
#endif
	.get_conf = usb_class_cdc_get_conf,
	.get_conf_hnd = cdc_get_conf_hnd,
};

static int usb_cdc_init(void) {
	return usb_class_register(&usb_class_cdc);
}

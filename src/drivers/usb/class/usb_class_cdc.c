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

#include <util/log.h>

EMBOX_UNIT_INIT(usb_cdc_init);

POOL_DEF(cdc_classes, struct usb_class_cdc, USB_CDC_MAX_DEVS);

/* Storage for device configuration*/
static char cdc_conf[16];

#define INTERFACE_DESC_OFFSET          (sizeof (struct usb_desc_configuration))
#define CS_INTERFACE                   0x24

/* Configuration */
static void cdc_get_conf_len_hnd(struct usb_request *req, void *arg);
static void cdc_get_conf(struct usb_dev *dev, struct usb_class_cdc *cdc,
		size_t idx, size_t len, usb_request_notify_hnd_t cb);
static void cdc_get_conf_content_hnd(struct usb_request *req, void *arg);
static void cdc_get_conf_hnd(struct usb_request *req, void *arg);
static void cdc_next_conf(struct usb_dev *dev);

/* Endpoint */
static void cdc_get_endpoints(struct usb_dev *dev, struct usb_class_cdc *cdc);

/* Misc */
static size_t cdc_skip_type(uint8_t *start, uint8_t skip_type);
static int is_rndis(struct usb_desc_interface *desc);


/**
 * Skip all functional descriptors or all endpoints.
 * Return size of all skipped elements.
 * @see ecm120.pdf (Subclass Specification for
 *  Ethernet Control Model Devices) Section 5
 */
static size_t cdc_skip_type(uint8_t *start, uint8_t skip_type) {
	uint8_t *cur = start;

	while (cur[1] == skip_type) {
		uint8_t len = *cur;
		cur += len;
	}

	return (cur - start);
}


struct usb_desc_interface *cdc_get_interface(
		struct usb_desc_configuration *conf, size_t index) {
	size_t cur = 0;
	char *curp = (char *) conf + INTERFACE_DESC_OFFSET;

	while (cur != index) {
		curp += sizeof (struct usb_desc_interface);
		curp += cdc_skip_type((uint8_t*) curp, CS_INTERFACE);
		curp += cdc_skip_type((uint8_t*) curp, USB_DESC_TYPE_ENDPOINT);
		cur++;
	}

	return (struct usb_desc_interface *) curp;
}

static void cdc_get_endpoints(struct usb_dev *dev, struct usb_class_cdc *cdc) {
	struct usb_desc_endpoint *ep;
	struct usb_desc_interface *iface;
	char *endpoints;
	struct usb_desc_configuration *conf;
	size_t i, j;

	conf = (struct usb_desc_configuration *) cdc->getconf;

	/* TODO conf->b_num_interfaces + 1 must be
	 * conf->b_num_interfaces + alternated settings */
	for (i = 0; i < conf->b_num_interfaces + 1; i++) {
		iface = cdc_get_interface((struct usb_desc_configuration *) cdc->getconf, i);
		endpoints = (char *) iface + sizeof (struct usb_desc_interface);
		endpoints += cdc_skip_type((uint8_t*)endpoints, CS_INTERFACE);

		for (j = 0; j < iface->b_num_endpoints; j++) {
			ep = (struct usb_desc_endpoint *) endpoints + j;
			if (0 == usb_endp_alloc(dev, ep)) {
				panic("usb_enp_alloc() failed");
			}
		}
	}
}

static void cdc_get_conf_hnd(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_class_cdc *cdc = usb2cdcdata(dev);
	struct usb_desc_interface *iface;

	iface = cdc_get_interface((struct usb_desc_configuration *) cdc->getconf, 0);

	/* Check if the first interface is one of Microsoft's pet,
	 * and if so then try to skip it. */
	if (is_rndis(iface)) {
		printk("%s\n", "CDC: RNDIS detected");
		sysfree(cdc->getconf);
		cdc_next_conf(dev);
		return;
	}

	cdc_get_endpoints(dev, cdc);

	usb_driver_handle(req->endp->dev);

	usb_class_start_handle(dev);
}

static void cdc_get_conf_len_hnd(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_class_cdc *cdc = usb2cdcdata(dev);

	usb_endp_control(dev->endpoints[0], cdc_get_conf_content_hnd, NULL,
		USB_DEV_REQ_TYPE_RD
			| USB_DEV_REQ_TYPE_STD
			| USB_DEV_REQ_TYPE_DEV,
		USB_DEV_REQ_GET_DESC,
		(USB_DESC_TYPE_CONFIG << 8) | cdc->current_conf,
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
	if (!cdc->getconf) {
		printk("%s\n", "CDC: sysmalloc failed on storage for cdc->getconf allocation");
		return;
	}

	cdc_get_conf(dev, cdc,
		(USB_DESC_TYPE_CONFIG << 8) | cdc->current_conf,
		c->w_total_length, cdc_get_conf_hnd);
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

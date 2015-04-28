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

EMBOX_UNIT_INIT(usb_cdc_init);

POOL_DEF(cdc_classes, struct usb_class_cdc, USB_CDC_MAX_DEVS);

struct usb_desc_configuration config_desc;
struct usb_desc_interface interface_desc;

/* FIXME store all static data in pool */

/* Storage for device configuration*/
static char cdc_conf[16];

#define FUNCTIONAL_DESCS_TOTAL_SIZE    (3 * 5 + 4)
#define CONFIG_DESC_OFFSET             0
#define INTERFACE_DESC_OFFSET          (sizeof (struct usb_desc_configuration))
#define ENDPOINT_DESC_OFFSET           \
	(INTERFACE_DESC_OFFSET + sizeof (struct usb_desc_interface) + \
			FUNCTIONAL_DESCS_TOTAL_SIZE)

static void cdc_get_conf_len(struct usb_request *req, void *arg);
static void cdc_get_conf_by_idx(struct usb_dev *dev, struct usb_class_cdc *cdc,
		size_t idx, size_t len, usb_request_notify_hnd_t cb);
static void cdc_get_conf_content(struct usb_request *req, void *arg);

static void usb_class_cdc_get_conf_hnd(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_class_cdc *cdc = usb2cdcdata(dev);

	printk("%s: in\n", __func__);

	usb_dev_generic_fill_iface(dev, cdc->getconf + INTERFACE_DESC_OFFSET);
	usb_dev_generic_fill_endps(dev, cdc->getconf + ENDPOINT_DESC_OFFSET);

	//pool_free(&hid_getconfs, hid->getconf);
	sysfree(cdc->getconf);
	cdc->getconf = NULL;

	usb_class_start_handle(dev);

	printk("%s: out\n", __func__);
}

static void cdc_get_conf_len(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;

	usb_endp_control(dev->endpoints[0], cdc_get_conf_content, NULL,
		USB_DEV_REQ_TYPE_RD
			| USB_DEV_REQ_TYPE_STD
			| USB_DEV_REQ_TYPE_DEV,
		USB_DEV_REQ_GET_DESC,
		(USB_DESC_TYPE_CONFIG << 8) | 1,
		dev->c_config,
		9,
		cdc_conf);
}

static void cdc_get_conf_by_idx(struct usb_dev *dev, struct usb_class_cdc *cdc,
		size_t idx, size_t len, usb_request_notify_hnd_t cb) {
	usb_endp_control(dev->endpoints[0], usb_class_cdc_get_conf_hnd, NULL,
		USB_DEV_REQ_TYPE_RD
			| USB_DEV_REQ_TYPE_STD
			| USB_DEV_REQ_TYPE_DEV,
		USB_DEV_REQ_GET_DESC,
		idx,
		dev->c_config,
		len,
		cdc->getconf);
}

static void cdc_get_conf_content(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_class_cdc *cdc = usb2cdcdata(dev);
	struct usb_desc_configuration *c;

	c = (struct usb_desc_configuration*) cdc_conf;

	cdc->getconf = sysmalloc(c->w_total_length);

	cdc_get_conf_by_idx(dev, cdc, (USB_DESC_TYPE_CONFIG << 8) | 1,
			c->w_total_length, usb_class_cdc_get_conf_hnd);
}

static int usb_class_cdc_get_conf(struct usb_class *cls, struct usb_dev *dev) {
	usb_endp_control(dev->endpoints[0], cdc_get_conf_len, NULL,
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
	.get_conf_hnd = usb_class_cdc_get_conf_hnd,
#if 0
	.class_handle = usb_class_hid_handle,
	.class_release = usb_class_hid_release,
#endif
};

static int usb_cdc_init(void) {
	return usb_class_register(&usb_class_cdc);
}

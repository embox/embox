/**
 * @file
 * @brief
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
#include <prom/prom_printf.h>
#include <stdlib.h>

EMBOX_UNIT_INIT(usb_cdc_init);

POOL_DEF(cdc_classes, struct usb_class_cdc, USB_CDC_MAX_DEVS);

#if 0
static uint32_t line_coding;

static void get_line_coding_conf_hnd(struct usb_request *req, void *arg) {
}

static void get_line_coding(struct usb_dev *dev) {
	line_coding = 18;

	usb_endp_control(dev->endpoints[0], get_line_coding_conf_hnd, NULL,
			0x21,
			0x20,
			0,
		dev->c_config,
		sizeof line_coding,
		&line_coding);

	line_coding = 0;

	usb_endp_control(dev->endpoints[0], get_line_coding_conf_hnd, NULL,
			0xA1,
			0x21,
			0,
		dev->c_config,
		sizeof line_coding,
		&line_coding);

	prom_printf("line_coding - %d\n", line_coding);
}
#endif

struct usb_desc_configuration config_desc;
struct usb_desc_interface interface_desc;

#define FUNCTIONAL_DESCS_TOTAL_SIZE    (3 * 5 + 4)
#define CONFIG_DESC_OFFSET             0
#define INTERFACE_DESC_OFFSET          (sizeof (struct usb_desc_configuration))
#define ENDPOINT_DESC_OFFSET           \
	(INTERFACE_DESC_OFFSET + sizeof (struct usb_desc_interface) + \
			FUNCTIONAL_DESCS_TOTAL_SIZE)

static void usb_class_cdc_get_conf_hnd(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_class_cdc *cdc = usb2cdcdata(dev);

	prom_printf("%s: in\n", __func__);

	usb_dev_generic_fill_iface(dev, cdc->getconf + INTERFACE_DESC_OFFSET);
	usb_dev_generic_fill_endps(dev, cdc->getconf + ENDPOINT_DESC_OFFSET);

	//pool_free(&hid_getconfs, hid->getconf);
	sysfree(cdc->getconf);
	cdc->getconf = NULL;

	get_line_coding(dev);

	usb_class_start_handle(dev);

	prom_printf("%s: out\n", __func__);
}

static int usb_class_cdc_get_conf(struct usb_class *cls, struct usb_dev *dev) {
	size_t conf_size;
	struct usb_class_cdc *cdc = usb2cdcdata(dev);

	conf_size = sizeof(struct usb_desc_configuration)
					+ sizeof(struct usb_desc_interface)
					+ FUNCTIONAL_DESCS_TOTAL_SIZE
					+ (dev->endp_n - 1) * sizeof(struct usb_desc_endpoint);

	cdc->getconf = sysmalloc(conf_size);

	usb_endp_control(dev->endpoints[0], cls->get_conf_hnd, NULL,
		USB_DEV_REQ_TYPE_RD
			| USB_DEV_REQ_TYPE_STD
			| USB_DEV_REQ_TYPE_DEV,
		USB_DEV_REQ_GET_DESC,
		USB_DESC_TYPE_CONFIG << 8,
		dev->c_config,
		conf_size,
		cdc->getconf);

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

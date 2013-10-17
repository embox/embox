/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    16.10.2013
 */

#include <mem/misc/pool.h>
#include <kernel/printk.h>
#include <kernel/panic.h>
#include <embox/unit.h>
#include <drivers/usb.h>

EMBOX_UNIT_INIT(usb_hid_init);

#define USB_HID_MAX_INPUT_LEN 8

#define USB_HID_MAX_DEVS         2
#define USB_HID_MAX_GETCONFS     2

#define USB_CLASS_HID 		 3

#define USB_CLASS_HID_SUB	 0
#define USB_CLASS_HID_SUB_BOOT	 1

#define USB_CLASS_HID_PROT_NONE  0
#define USB_CLASS_HID_PROT_KBD   1
#define USB_CLASS_HID_PROT_MOUSE 2

struct usb_desc_hid {
	uint8_t b_lenght;
	uint8_t b_desc_type;
	uint16_t bcd_hid;
	uint8_t b_country_code;
	uint8_t b_num_descriptors;
	uint8_t b_descriptor_type;
	uint16_t w_descriptor_len;
} __attribute__((packed));

struct usb_hid_getconf {
	struct usb_desc_configuration config_desc;
	struct usb_desc_interface interface_desc;
	struct usb_desc_hid hid_desc;
	struct usb_desc_endpoint endp_descs[USB_DEV_MAX_ENDP];
} __attribute__((packed));

struct usb_class_hid {
	struct usb_hid_getconf *getconf;
	unsigned char input_data[USB_HID_MAX_INPUT_LEN];
};

struct usb_class_hid *usb2hiddata(struct usb_dev *dev) {
	return dev->class_specific;
}

POOL_DEF(hid_getconfs, struct usb_hid_getconf, USB_HID_MAX_GETCONFS);
POOL_DEF(hid_classes, struct usb_class_hid, USB_HID_MAX_DEVS);

#if 1 /* mouse */
static void usb_mouse_notify(struct usb_request *req) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_class_hid *hid = usb2hiddata(dev);

	printk("\n\nusb_mouse: received=");
	for (int i = 0; i < USB_HID_MAX_INPUT_LEN; i++) {
		printk(" %.2x", hid->input_data[i]);
	}
	printk("\n");

	usb_endp_interrupt(dev->endpoints[1], usb_mouse_notify,
			hid->input_data, dev->endpoints[1]->max_packet_size);
}

static int usb_handle_mouse(struct usb_dev *dev) {
	struct usb_class_hid *hid = usb2hiddata(dev);

	assert(dev->endpoints[1]->max_packet_size <= USB_HID_MAX_INPUT_LEN);

	usb_endp_interrupt(dev->endpoints[1], usb_mouse_notify,
			hid->input_data, dev->endpoints[1]->max_packet_size);

	return 0;
}
#endif

static void usb_hid_request_hnd_get_conf(struct usb_request *req) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_class_hid *hid = usb2hiddata(dev);

	usb_dev_generic_fill_endps(dev, hid->getconf->endp_descs);

	pool_free(&hid_getconfs, hid->getconf);
	hid->getconf = NULL;

	/* let's open mouse right here to see debug info! */
	usb_handle_mouse(dev);
}

static void *usb_class_hid_alloc(struct usb_class *cls, struct usb_dev *dev) {
	return pool_alloc(&hid_classes);
}

static void usb_class_hid_free(struct usb_class *cls, struct usb_dev *dev, void *spec) {
	pool_free(&hid_classes, spec);
}

static int usb_class_hid_get_conf(struct usb_class *cls, struct usb_dev *dev) {
	struct usb_class_hid *hid = usb2hiddata(dev);

	hid->getconf = pool_alloc(&hid_getconfs);

	usb_endp_control(dev->endpoints[0], usb_hid_request_hnd_get_conf,
		USB_DEV_REQ_TYPE_RD
			| USB_DEV_REQ_TYPE_STD
			| USB_DEV_REQ_TYPE_DEV,
		USB_DEV_REQ_GET_DESC,
		USB_DESC_TYPE_CONFIG << 8,
		dev->c_config,
		sizeof(struct usb_desc_configuration)
			+ sizeof(struct usb_desc_interface)
			+ sizeof(struct usb_desc_hid)
			+ (dev->endp_n - 1) * sizeof(struct usb_desc_endpoint),
		hid->getconf);

	return 0;
}

static struct usb_class usb_class_hid = {
	.usb_class = USB_CLASS_HID,
	.class_alloc = usb_class_hid_alloc,
	.class_free = usb_class_hid_free,
	.get_conf = usb_class_hid_get_conf,
};

static int usb_hid_init(void) {

	return usb_class_register(&usb_class_hid);
}

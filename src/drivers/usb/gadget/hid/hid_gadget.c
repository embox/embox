/**
 * @file
 * @brief
 *
 * @author Anton Bondarev
 * @date   02.05.2026
 */

#include <util/log.h>

#include <assert.h>

#include <lib/libds/array.h>
#include <embox/unit.h>

#include <drivers/usb/usb_defines.h>
#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/gadget/gadget.h>

EMBOX_UNIT_INIT(hid_gadget_init);

#define DEVICE_DESC_STR_MANUFACTURER        1
#define DEVICE_DESC_STR_PRODUCT             2
#define DEVICE_DESC_STR_SERIALNUMBER        3
#define DEVICE_DESC_STR_CONFIGURATION       4

#define DEVICE_VID    0xdead
#define DEVICE_PID    0x1234

static struct usb_gadget device_config = {
	.config_desc = {
		.b_length                = sizeof(struct  usb_desc_configuration),
		.b_desc_type             = USB_DESC_TYPE_CONFIG,
		.w_total_length          = 0, /* DYNAMIC */
		.b_num_interfaces        = 0, /* DYNAMIC */
		.b_configuration_value   = 1,
		.i_configuration         = DEVICE_DESC_STR_CONFIGURATION,
		.bm_attributes           = USB_CFG_ATT_ONE | USB_CFG_ATT_SELFPOWER |
									USB_CFG_ATT_WAKEUP,
		.b_max_power             = 0x30,
	},
};

static struct usb_gadget_composite usb_gadget = {
	.device_desc = {
		.b_length               = sizeof(struct usb_desc_device),
		.b_desc_type            = USB_DESC_TYPE_DEV,
		.bcd_usb                = 0x0200,
		.b_dev_class            = 0, /* Each interface specifies it’s own class code */
		.b_dev_subclass         = 0,
		.b_dev_protocol         = 0,
		.b_max_packet_size      = OPTION_GET(NUMBER, max_packet_size),
		.id_vendor              = DEVICE_PID,
		.id_product             = DEVICE_PID,
		.bcd_device             = 0,
		.i_manufacter           = DEVICE_DESC_STR_MANUFACTURER,
		.i_product              = DEVICE_DESC_STR_PRODUCT,
		.i_serial_number        = DEVICE_DESC_STR_SERIALNUMBER,
		.b_num_configurations   = 1,
	},
	.strings = {
		[DEVICE_DESC_STR_MANUFACTURER]      = "Embox",
		[DEVICE_DESC_STR_PRODUCT]           = "Embox USB HID",
		[DEVICE_DESC_STR_SERIALNUMBER]      = "0123456789",
		[DEVICE_DESC_STR_CONFIGURATION]     = "HID",
	},
	.configs = {
		&device_config,
	},
};

static int hid_gadget_init(void) {
	usb_gadget_register(&usb_gadget);

	if (usb_gadget_add_function(usb_gadget.configs[0], "hid") != 0) {
		log_error("acm interface insertion failed");
		return -1;
	}

	usb_gadget_udc_start(usb_gadget.ep0.udc);

	return 0;
}

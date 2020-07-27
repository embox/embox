/**
 * @file
 * @brief
 *
 * @author  Erick Cafferata
 * @date    20.07.2020
 */

#include <assert.h>
#include <string.h>
#include <util/log.h>
#include <util/array.h>
#include <embox/unit.h>

#include <drivers/usb/usb_defines.h>
#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/gadget/gadget.h>

EMBOX_UNIT_INIT(acm_gadget_init);

#define ACM_STR_MANUFACTURER        1
#define ACM_STR_PRODUCT             2
#define ACM_STR_SERIALNUMBER        3
#define ACM_STR_CONFIGURATION       4

#define ACM_VID    0xdead
#define ACM_PID    0xbeaf

static struct usb_gadget acm_gadget = {
	.device_desc = {
		.b_length               = sizeof(struct usb_desc_device),
		.b_desc_type            = USB_DESC_TYPE_DEV,
		.bcd_usb                = 0x0200,
		.b_dev_class            = 0, /* Each interface specifies it’s own class code */
		.b_dev_subclass         = 0,
		.b_dev_protocol         = 0,
		.b_max_packet_size      = 8,
		.id_vendor              = ACM_VID,
		.id_product             = ACM_PID,
		.bcd_device             = 0,
		.i_manufacter           = ACM_STR_MANUFACTURER,
		.i_product              = ACM_STR_PRODUCT,
		.i_serial_number        = ACM_STR_SERIALNUMBER,
		.b_num_configurations   = 1,
	},
	.config_desc = {
		.b_length                = sizeof(struct  usb_desc_configuration),
		.b_desc_type             = USB_DESC_TYPE_CONFIG,
		.w_total_length          = 0, /* DYNAMIC */
		.b_num_interfaces        = 0, /* DYNAMIC */
		.b_configuration_value   = 1,
		.i_configuration         = ACM_STR_CONFIGURATION,
		.bm_attributes           = USB_CFG_ATT_ONE | USB_CFG_ATT_SELFPOWER |
			USB_CFG_ATT_WAKEUP,
		.b_max_power             = 0x30,
	},
	.strings = {
		[ACM_STR_MANUFACTURER]      = "Embox",
		[ACM_STR_PRODUCT]           = "Embox USB CDC-ACM",
		[ACM_STR_SERIALNUMBER]      = "0123456789",
		[ACM_STR_CONFIGURATION]     = "ACM",
	},
};

static int acm_gadget_init(void) {
	usb_gadget_register(&acm_gadget);

	if (usb_gadget_add_function(&acm_gadget, "acm") != 0) {
		log_error("acm interface insertion failed");
		return -1;
	}

	usb_gadget_udc_start(acm_gadget.ep0.udc);

	return 0;
}

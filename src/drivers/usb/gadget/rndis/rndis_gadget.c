/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    25.06.2020
 */

#include <assert.h>
#include <string.h>
#include <util/log.h>
#include <util/array.h>
#include <embox/unit.h>

#include <drivers/usb/usb_defines.h>
#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/gadget/gadget.h>

EMBOX_UNIT_INIT(rndis_gadget_init);

#define RNDIS_VID    0xdead
#define RNDIS_PID    0xbeaf

#define RNDIS_STR_MANUFACTURER        1
#define RNDIS_STR_PRODUCT             2
#define RNDIS_STR_SERIALNUMBER        3
#define RNDIS_STR_CONFIGURATION       4

static struct usb_gadget rndis_gadget = {
	.device_desc = {
		.b_length               = sizeof(struct usb_desc_device),
		.b_desc_type            = USB_DESC_TYPE_DEV,
		.bcd_usb                = 0x0200,
		.b_dev_class            = 0, /* Each interface specifies it’s own class code */
		.b_dev_subclass         = 0,
		.b_dev_protocol         = 0,
		.b_max_packet_size      = 8,
		.id_vendor              = RNDIS_VID,
		.id_product             = RNDIS_PID,
		.bcd_device             = 0,
		.i_manufacter           = RNDIS_STR_MANUFACTURER,
		.i_product              = RNDIS_STR_PRODUCT,
		.i_serial_number        = RNDIS_STR_SERIALNUMBER,
		.b_num_configurations   = 1,
	},
	.config_desc = {
		.b_length                = sizeof(struct  usb_desc_configuration),
		.b_desc_type             = USB_DESC_TYPE_CONFIG,
		.w_total_length          = 0, /* DYNAMIC */
		.b_num_interfaces        = 0, /* DYNAMIC */
		.b_configuration_value   = 1,
		.i_configuration         = RNDIS_STR_CONFIGURATION,
		.bm_attributes           = USB_CFG_ATT_ONE | USB_CFG_ATT_SELFPOWER |
		                           USB_CFG_ATT_WAKEUP,
		.b_max_power             = 0x30,
	},
	.strings = {
		[RNDIS_STR_MANUFACTURER]  = "Embox",
		[RNDIS_STR_PRODUCT]       = "Embox USB RNDIS",
		[RNDIS_STR_SERIALNUMBER]  = "0123456789",
		[RNDIS_STR_CONFIGURATION] = "RNDIS",
	},
};

static int rndis_gadget_init(void) {
	usb_gadget_register(&rndis_gadget);

	if (usb_gadget_add_function(&rndis_gadget, "rndis") != 0) {
		log_error("RNDIS interface insertion failed");
		return -1;
	}

	usb_gadget_udc_start(rndis_gadget.ep0.udc);

	return 0;
}

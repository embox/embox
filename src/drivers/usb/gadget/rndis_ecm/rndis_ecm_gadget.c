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
#include <lib/libds/array.h>
#include <embox/unit.h>

#include <drivers/usb/usb_defines.h>
#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/gadget/gadget.h>

#include <drivers/usb/function/f_rndis_idx.h>
#include <drivers/usb/function/f_ecm_idx.h>

EMBOX_UNIT_INIT(rndis_ecm_gadget_init);

#define RNDIS_VID    0xdead
#define RNDIS_PID    0xbeaf

#define STR_MANUFACTURER        1
#define STR_PRODUCT             2
#define STR_SERIALNUMBER        3

static struct usb_gadget rndis_config = {
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
};

static struct usb_gadget ecm_config = {
	.config_desc = {
		.b_length                = sizeof(struct  usb_desc_configuration),
		.b_desc_type             = USB_DESC_TYPE_CONFIG,
		.w_total_length          = 0, /* DYNAMIC */
		.b_num_interfaces        = 0, /* DYNAMIC */
		.b_configuration_value   = 2,
		.i_configuration         = ECM_STR_CONFIGURATION,
		.bm_attributes           = USB_CFG_ATT_ONE | USB_CFG_ATT_SELFPOWER |
		                           USB_CFG_ATT_WAKEUP,
		.b_max_power             = 0x30,
	},
};

static struct usb_gadget_composite rndis_ecm_gadget = {
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
		.i_manufacter           = STR_MANUFACTURER,
		.i_product              = STR_PRODUCT,
		.i_serial_number        = STR_SERIALNUMBER,
		.b_num_configurations   = 2,
	},
	.strings = {
		[STR_MANUFACTURER]  = "Embox",
		[STR_PRODUCT]       = "Embox USB RNDIS",
		[STR_SERIALNUMBER]  = "0123456789",
		[RNDIS_STR_CONFIGURATION] = "RNDIS",

		[ECM_STR_CONFIGURATION]     = "ECM",
		[ECM_STR_CONTROL_INTERFACE] = "ECM Control Interface",
		[ECM_STR_DATA_INTERFACE]    = "ECM Data Interface",
		[ECM_STR_ETHADDR]           = "aabbccddee00",
	},
	.configs = {
		&rndis_config,
		&ecm_config,
	},
};

static int rndis_ecm_gadget_init(void) {
	usb_gadget_register(&rndis_ecm_gadget);

	if (usb_gadget_add_function(rndis_ecm_gadget.configs[0], "rndis") != 0) {
		log_error("RNDIS interface insertion failed");
		return -1;
	}

	if (usb_gadget_add_function(rndis_ecm_gadget.configs[1], "ecm") != 0) {
		log_error("ECM interface insertion failed");
		return -1;
	}

	usb_gadget_udc_start(rndis_ecm_gadget.ep0.udc);

	return 0;
}

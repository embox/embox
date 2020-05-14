/*
 * usb_example.c
 *
 *  Created on: May 8, 2020
 *      Author: anton
 */
#include <config/custom_config_qspi.h>

#include <hw_cpm.h>
#include <hw_usb.h>

#include <assert.h>
#include <string.h>
#include <util/log.h>
#include <util/array.h>
#include <kernel/irq.h>
#include <embox/unit.h>
#include <third_party/dialog/da14695/usb_da1469x.h>

#include <drivers/usb/gadget/rndis_desc.h>

EMBOX_UNIT_INIT(usb_example_init);

#define RNDIS_EP_BULK_TX 1
#define RNDIS_EP_BULK_RX 2
#define RNDIS_EP_INT_TX  3
#define RNDIS_EP_COUNT   4

#define EP0_BUFFER_SIZE 1024

static uint8_t ep0_buffer[EP0_BUFFER_SIZE];

static hw_usb_device_framework_ctrl_req_t usb_setup;

static const struct usb_desc_endpoint int_ep_desc = {
	.b_length            = sizeof (struct usb_desc_endpoint),
	.b_desc_type         = USB_DESC_TYPE_ENDPOINT,
	.b_endpoint_address  = 0x80 | RNDIS_EP_INT_TX, // in
	.bm_attributes       = USB_DESC_ENDP_TYPE_INTR,
	.w_max_packet_size   = 8,
	.b_interval          = 1,
};

static const struct usb_desc_endpoint bulk_ep_tx_desc = {
	.b_length            = sizeof (struct usb_desc_endpoint),
	.b_desc_type         = USB_DESC_TYPE_ENDPOINT,
	.b_endpoint_address  = 0x80 | RNDIS_EP_BULK_TX, // in
	.bm_attributes       = USB_DESC_ENDP_TYPE_BULK,
	.w_max_packet_size   = 64,
	.b_interval          = 0,
};

static const struct usb_desc_endpoint bulk_ep_rx_desc = {
	.b_length            = sizeof (struct usb_desc_endpoint),
	.b_desc_type         = USB_DESC_TYPE_ENDPOINT,
	.b_endpoint_address  = 0x0 | RNDIS_EP_BULK_RX, // out
	.bm_attributes       = USB_DESC_ENDP_TYPE_BULK,
	.w_max_packet_size   = 64,
	.b_interval          = 0,
};

static void usb_send_control_msg(uint8_t *data, uint8_t size) {
	assert(size <= EP0_BUFFER_SIZE);
	
	log_debug("size = %d", size);
	
	memcpy(ep0_buffer, data, size);
	hw_usb_ep_tx_start(0, ep0_buffer, size);
}

static void usb_req_get_status(void) {
	uint16_t status = 0;
	int ep_nr;

	switch (usb_setup.request_type & HW_USB_DEVICE_FRAMEWORK_RECIP_MASK) {
	case HW_USB_DEVICE_FRAMEWORK_RECIP_DEVICE:
		usb_send_control_msg((uint8_t *) &status, 2);
		break;
	case HW_USB_DEVICE_FRAMEWORK_RECIP_ENDPOINT:
		ep_nr = usb_setup.index & ~HW_USB_DEVICE_FRAMEWORK_DIR_IN;

		if (ep_nr >= RNDIS_EP_COUNT) {
			hw_usb_ep0_stall();
			break;
		}
		
		if (hw_usb_ep_is_stalled(ep_nr)) {
			status = 1;
		}
		usb_send_control_msg((uint8_t *) &status, 2);
		break;
	default:
		log_error("Unsupported RECIP 0x%x",
			usb_setup.request_type & HW_USB_DEVICE_FRAMEWORK_RECIP_MASK);
		hw_usb_ep0_stall();
		break;
	}
}

static void usb_req_set_address(void) {
	log_debug("addr=0x%x", usb_setup.value);
	hw_usb_bus_address(usb_setup.value);
	hw_usb_ep_tx_start(0, NULL, 0);
}

static void usb_send_config(void) {
	struct usb_desc_common_header *descs[] = {
		(struct usb_desc_common_header *) &rndis_config_desc,
		(struct usb_desc_common_header *) &iad_descriptor,
		(struct usb_desc_common_header *) &rndis_interface0_desc,
		(struct usb_desc_common_header *) &int_ep_desc,
		(struct usb_desc_common_header *) &interface0_functional_desc[0],
		(struct usb_desc_common_header *) &interface0_functional_desc[1],
		(struct usb_desc_common_header *) &interface0_functional_desc[2],
		(struct usb_desc_common_header *) &interface0_functional_desc[3],
		(struct usb_desc_common_header *) &rndis_interface1_desc,
		(struct usb_desc_common_header *) &bulk_ep_tx_desc,
		(struct usb_desc_common_header *) &bulk_ep_rx_desc,
	};
	int i, size = 0;
	uint8_t *buf = ep0_buffer;

	for (i = 0; i < ARRAY_SIZE(descs); i++) {
		memcpy(buf, descs[i], descs[i]->b_length);
		buf += descs[i]->b_length;
		size += descs[i]->b_length;
	}
	((struct usb_desc_configuration *) ep0_buffer)->w_total_length = size;

	hw_usb_ep_tx_start(0, ep0_buffer, MIN(size, usb_setup.length));
}

static void usb_req_get_descriptor(void) {
	switch (usb_setup.value >> 8) {
	case HW_USB_DEVICE_FRAMEWORK_DT_DEVICE:
		log_debug("HW_USB_DEVICE_FRAMEWORK_DT_DEVICE");
		usb_send_control_msg((uint8_t *) &rndis_device_desc,
					MIN(sizeof(rndis_device_desc), usb_setup.length));
		break;
	case HW_USB_DEVICE_FRAMEWORK_DT_CONFIG:
		log_debug("HW_USB_DEVICE_FRAMEWORK_DT_CONFIG");
		usb_send_config();
		break;
	default:
		hw_usb_ep0_stall();
		break;
	}
}

static void usb_handle_standard_req(void) {
	switch (usb_setup.request) {
	case HW_USB_DEVICE_FRAMEWORK_REQ_GET_STATUS:
		usb_req_get_status();
		break;
	case HW_USB_DEVICE_FRAMEWORK_REQ_SET_ADDRESS:
		usb_req_set_address();
		break;
	case HW_USB_DEVICE_FRAMEWORK_REQ_GET_DESCRIPTOR:
		usb_req_get_descriptor();
		break;
	case HW_USB_DEVICE_FRAMEWORK_REQ_GET_CONFIGURATION:
		/* TODO usb_req_get_configuration(); */
		break;
	case HW_USB_DEVICE_FRAMEWORK_REQ_SET_CONFIGURATION:
		/* TODO usb_req_set_configuration(); */
		break;
	default:
		log_error("Not implemented req 0x%x", usb_setup.request);
		hw_usb_ep0_stall();
		break;
	}
}

bool hw_usb_ep_rx_done(uint8_t ep_nr, uint8_t *buffer, uint16_t size) {
	if (ep_nr == 0) {
		if (size == 0) {
			return true;
		}

		if (buffer == (uint8_t *) &usb_setup) {
			log_debug("Usb setup packet:");
			log_debug("request_type=0x%02x, request=0x%02x, value=0x%04x, index=0x%04x, length=0x%04x",
				usb_setup.request_type, usb_setup.request, usb_setup.value, usb_setup.index, usb_setup.length);
			log_debug("");
		}
		
		switch (usb_setup.request_type & HW_USB_DEVICE_FRAMEWORK_TYPE_MASK) {
		case HW_USB_DEVICE_FRAMEWORK_TYPE_STANDARD:
			usb_handle_standard_req();
			break;
		default:
			log_error("Unknown request");
			break;
		}
	}

	return false;
}

void hw_usb_ep_tx_done(uint8_t ep_nr, uint8_t *buffer) {
	if (ep_nr == 0) {
		hw_usb_ep_rx_enable(0);
		return;
	}
}
#if !defined(dg_configDEVICE)
void hw_usb_bus_event(usb_bus_event_type event)
#else
void hw_usb_bus_event(HW_USB_BUS_EVENT_TYPE event)
#endif
{
	switch (event) {
	case UBE_RESET:
		log_debug("Reset event");
		hw_usb_bus_address(0);
		hw_usb_ep_rx_enable(0);
		break;
	default:
		log_error("Unknown event");
		break;
	}
}

void hw_usb_bus_frame(uint16_t frame_nr) {
	(void) frame_nr;
}

void hw_usb_ep_nak(uint8_t ep_nr) {
	if (ep_nr == 0) {
		hw_usb_ep_disable(0, false);
		hw_usb_ep_rx_enable(0);
	}
}

uint8_t *hw_usb_ep_get_rx_buffer(uint8_t ep_nr, bool is_setup, uint16_t *buffer_size) {
	if (ep_nr == 0) {
		if (is_setup) {
			*buffer_size = sizeof(usb_setup);
			return (uint8_t *) &usb_setup;
		} else {
			hw_usb_ep0_stall();
			*buffer_size = 0;
			return NULL;
		}
	}
	return NULL;
}

bool hw_usb_ep_rx_read_by_driver(uint8_t ep_nr) {
	return false;
}

void sys_usb_ext_hook_begin_enumeration(void) {
	hw_usb_ep_configure(0, true, NULL);
	/* TODO We do not configure other endpoints because they are unused currently. */
}

static int usb_example_init(void) {
	usb_da1469x_init();

	return 0;
}

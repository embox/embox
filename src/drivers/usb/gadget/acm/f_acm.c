/**
 * @file
 * @brief
 *
 * @author  Erick Cafferata
 * @date    20.07.2020
 */

#include <stdint.h>
#include <string.h>
#include <embox/unit.h>
#include <util/log.h>
#include <drivers/usb/usb_defines.h>
#include <drivers/usb/usb_desc.h>
#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/gadget/gadget.h>

EMBOX_UNIT_INIT(acm_init);

/* TODO These defines should be moved to some CDC part. */
#define USB_CDC_SUBCLASS_ACM        0x02
#define USB_CDC_PROTOCOL_ATV250     0x01

#define USB_CDC_HEADER_TYPE         0x00
#define USB_CDC_CALL_TYPE           0x01
#define USB_CDC_ACM_TYPE            0x02
#define USB_CDC_UNION_TYPE          0x06

static int acm_probe(struct usb_gadget *gadget);

static struct usb_desc_endpoint int_ep_desc = {
	.b_length            = sizeof (struct usb_desc_endpoint),
	.b_desc_type         = USB_DESC_TYPE_ENDPOINT,
	.bm_attributes       = USB_DESC_ENDP_TYPE_INTR,
	.w_max_packet_size   = 16, /* 8 byte header + data */
	.b_interval          = 1,
};

static struct usb_desc_endpoint bulk_ep_tx_desc = {
	.b_length            = sizeof (struct usb_desc_endpoint),
	.b_desc_type         = USB_DESC_TYPE_ENDPOINT,
	.bm_attributes       = USB_DESC_ENDP_TYPE_BULK,
	.w_max_packet_size   = 64,
	.b_interval          = 0,
};

static struct usb_desc_endpoint bulk_ep_rx_desc = {
	.b_length            = sizeof (struct usb_desc_endpoint),
	.b_desc_type         = USB_DESC_TYPE_ENDPOINT,
	.bm_attributes       = USB_DESC_ENDP_TYPE_BULK,
	.w_max_packet_size   = 64,
	.b_interval          = 0,
};

static struct usb_desc_interface acm_control_intf_desc = {
	.b_length                = sizeof(struct usb_desc_interface),
	.b_desc_type             = USB_DESC_TYPE_INTERFACE,
	.b_interface_number      = 0, /* DYNAMIC */
	.b_alternate_setting     = 0,
	.b_num_endpoints         = 1,
	.b_interface_class       = USB_CLASS_COMM,
	.b_interface_subclass    = USB_CDC_SUBCLASS_ACM,
	.b_interface_protocol    = USB_CDC_PROTOCOL_ATV250,
	.i_interface             = 0,
};

static const uint8_t cdc_header_fdesc[] = {
	0x05,                       /*  u8    bLength */
	USB_DT_CS_INTERFACE,        /*  u8    bDescriptorType */
	USB_CDC_HEADER_TYPE,        /*  u8    bDescriptorSubType */
	0x10, 0x01,                 /*  le16  bcdCDC */
};

static const uint8_t cdc_call_fdesc[] = {
	0x05,                /* bLength */
	USB_DT_CS_INTERFACE, /* bDescriptorType */
	USB_CDC_CALL_TYPE,   /* bDescriptorSubType*/
	0x00,                /* bmCapabilities */
	0x01,                /* bDataInterface */
};

static const uint8_t cdc_acm_fdesc[] = {
	0x04,                /* bLength */
	USB_DT_CS_INTERFACE, /* bDescriptorType */
	USB_CDC_ACM_TYPE,    /* bDescriptorSubType */
	0x02,                /* bmCapabilities */
};

static const uint8_t cdc_union_fdesc[] = {
	0x05,                       /*  u8    bLength */
	USB_DT_CS_INTERFACE,        /*  u8    bDescriptorType */
	USB_CDC_UNION_TYPE,         /*  u8    bDescriptorSubType */
	0x00,                       /*  u8    bMasterInterface0 */
	0x01,                       /*  u8    bSlaveInterface0 */
};


static struct usb_desc_interface acm_data_intf_desc = {
	.b_length                = sizeof(struct usb_desc_interface),
	.b_desc_type             = USB_DESC_TYPE_INTERFACE,
	.b_interface_number      = 0, /* DYNAMIC */
	.b_alternate_setting     = 0,
	.b_num_endpoints         = 2,
	.b_interface_class       = USB_CLASS_CDC_DATA,
	.b_interface_subclass    = 0,
	.b_interface_protocol    = 0,
	.i_interface             = 0,
};

static const struct usb_desc_common_header *acm_descs[] = {
	/* Control interface */
	(struct usb_desc_common_header *) &acm_control_intf_desc,
	(struct usb_desc_common_header *) &cdc_header_fdesc,
	(struct usb_desc_common_header *) &cdc_call_fdesc,
	(struct usb_desc_common_header *) &cdc_acm_fdesc,
	(struct usb_desc_common_header *) &cdc_union_fdesc,
	(struct usb_desc_common_header *) &int_ep_desc,

	/* Data interface alt 1 */
	(struct usb_desc_common_header *) &acm_data_intf_desc,
	(struct usb_desc_common_header *) &bulk_ep_tx_desc,
	(struct usb_desc_common_header *) &bulk_ep_rx_desc,
	NULL,
};

static struct usb_gadget_ep intr = {
	.dir = USB_DIR_IN,
	.desc = &int_ep_desc,
};

static struct usb_gadget_ep bulk_tx = {
	.dir = USB_DIR_IN,
	.desc = &bulk_ep_tx_desc,
};

static struct usb_gadget_ep bulk_rx = {
	.dir = USB_DIR_OUT,
	.desc = &bulk_ep_rx_desc,
};

static int acm_setup(struct usb_gadget_function *f,
		const struct usb_control_header *ctrl, uint8_t *buffer) {
	struct usb_gadget *gadget = f->gadget;
	struct usb_gadget_request *req = &gadget->composite->req;

	if ((ctrl->bm_request_type & USB_REQ_TYPE_MASK) != USB_REQ_TYPE_STANDARD) {
		log_error("Not a standard request");
		return -1;
	}

	switch (ctrl->b_request) {
	case USB_REQ_SET_INTERFACE:
		/* Send zero-length reply */
		req->len = 0;
		usb_gadget_ep_queue(&gadget->composite->ep0, req);
		return 0;
	default:
		log_error("Not supported standard request");
		break;
	}

	return -1;
}

extern int acm_tty_init(struct usb_gadget_ep *tx, struct usb_gadget_ep *rx);

static int acm_enumerate(struct usb_gadget_function *f) {
	acm_tty_init(&bulk_tx, &bulk_rx);

	usb_gadget_ep_enable(&bulk_tx);
	usb_gadget_ep_enable(&bulk_rx);
	usb_gadget_ep_enable(&intr);

	return 0;
}

static struct usb_gadget_function acm_func = {
	.name = "acm",
	.descs = acm_descs,
	.probe = acm_probe,
	.setup = acm_setup,
	.enumerate = acm_enumerate,
};

static int acm_probe(struct usb_gadget *gadget) {
	acm_control_intf_desc.b_interface_number =
		usb_gadget_add_interface(gadget, &acm_func);

	acm_data_intf_desc.b_interface_number =
		usb_gadget_add_interface(gadget, &acm_func);

	/* FIXME */
	intr.udc    = gadget->composite->ep0.udc;
	bulk_tx.udc = gadget->composite->ep0.udc;
	bulk_rx.udc = gadget->composite->ep0.udc;

	usb_gadget_ep_configure(gadget, &bulk_tx);
	usb_gadget_ep_configure(gadget, &bulk_rx);
	usb_gadget_ep_configure(gadget, &intr);

	return 0;
}

static int acm_init(void) {
	usb_gadget_register_function(&acm_func);

	return 0;
}

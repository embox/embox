/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    22.07.2020
 */

#include <stdint.h>
#include <string.h>

#include <drivers/usb/gadget/gadget.h>
#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/usb_defines.h>
#include <drivers/usb/usb_desc.h>
#include <embox/unit.h>
#include <net/l2/ethernet.h>
#include <util/log.h>

/* TODO Remove. It's only used for i_interface, iMACAddress, etc. defines. */
#include <drivers/usb/function/f_ecm_idx.h>

EMBOX_UNIT_INIT(ecm_init);

/* TODO These defines should be moved to some CDC part. */
#define USB_CDC_SUBCLASS_ETHERNET 0x06

#define USB_CDC_HEADER_TYPE   0x00
#define USB_CDC_UNION_TYPE    0x06
#define USB_CDC_ETHERNET_TYPE 0x0f

static int ecm_probe(struct usb_gadget *gadget);

/* IAD descriptor */
static const uint8_t iad_descriptor[8] = {
    0x08,                      /*    bLength */
    0x0B,                      /*    bDescriptorType */
    0x00,                      /*    bFirstInterface */
    0x02,                      /*    bInterfaceCount */
    USB_CLASS_COMM,            /*    bFunctionClass  */
    USB_CDC_SUBCLASS_ETHERNET, /*    bFunctionSubClass */
    0x00,                      /*    bFunctionProtocol */
    ECM_STR_CONFIGURATION      /*    iFunction   "RNDIS" */
};

static struct usb_desc_endpoint int_ep_desc = {
    .b_length = sizeof(struct usb_desc_endpoint),
    .b_desc_type = USB_DESC_TYPE_ENDPOINT,
    .bm_attributes = USB_DESC_ENDP_TYPE_INTR,
    .w_max_packet_size = 16, /* 8 byte header + data */
    .b_interval = 1,
};

static struct usb_desc_endpoint bulk_ep_tx_desc = {
    .b_length = sizeof(struct usb_desc_endpoint),
    .b_desc_type = USB_DESC_TYPE_ENDPOINT,
    .bm_attributes = USB_DESC_ENDP_TYPE_BULK,
    .w_max_packet_size = 64,
    .b_interval = 0,
};

static struct usb_desc_endpoint bulk_ep_rx_desc = {
    .b_length = sizeof(struct usb_desc_endpoint),
    .b_desc_type = USB_DESC_TYPE_ENDPOINT,
    .bm_attributes = USB_DESC_ENDP_TYPE_BULK,
    .w_max_packet_size = 64,
    .b_interval = 0,
};

static struct usb_desc_interface ecm_control_intf_desc = {
    .b_length = sizeof(struct usb_desc_interface),
    .b_desc_type = USB_DESC_TYPE_INTERFACE,
    .b_interface_number = 0, /* DYNAMIC */
    .b_alternate_setting = 0,
    .b_num_endpoints = 1,
    .b_interface_class = USB_CLASS_COMM,
    .b_interface_subclass = USB_CDC_SUBCLASS_ETHERNET,
    .b_interface_protocol = 0,
    .i_interface = ECM_STR_CONTROL_INTERFACE,
};

static const uint8_t ecm_header_desc[] = {
    0x05,                /*  u8    bLength */
    USB_DT_CS_INTERFACE, /*  u8    bDescriptorType */
    USB_CDC_HEADER_TYPE, /*  u8    bDescriptorSubType */
    0x10,
    0x01, /*  le16  bcdCDC */
};

static const uint8_t ecm_union_desc[] = {
    0x05,                /*  u8    bLength */
    USB_DT_CS_INTERFACE, /*  u8    bDescriptorType */
    USB_CDC_UNION_TYPE,  /*  u8    bDescriptorSubType */
    0x00,                /*  u8    bMasterInterface0 */
    0x01,                /*  u8    bSlaveInterface0 */
};

static const uint8_t ecm_eth_desc[] = {
    0x0d,                  /*  u8    bLength */
    USB_DT_CS_INTERFACE,   /*  u8    bDescriptorType */
    USB_CDC_ETHERNET_TYPE, /*  u8    bDescriptorSubType */
    ECM_STR_ETHADDR,       /*  u8    iMACAddress */
    0x00,
    0x00,
    0x00,
    0x00, /*  le32  bmEthernetStatistics */
    ETH_FRAME_LEN & 0xff,
    ETH_FRAME_LEN >> 8, /*  le16  wMaxSegmentSize */
    0x00,
    0x00, /*  le16  wNumberMCFilters */
    0x00, /*  u8    bNumberPowerFilters */
};

static struct usb_desc_interface ecm_data_intf_nop_desc = {
    .b_length = sizeof(struct usb_desc_interface),
    .b_desc_type = USB_DESC_TYPE_INTERFACE,
    .b_interface_number = 0, /* DYNAMIC */
    .b_alternate_setting = 0,
    .b_num_endpoints = 0,
    .b_interface_class = USB_CLASS_CDC_DATA,
    .b_interface_subclass = 0,
    .b_interface_protocol = 0,
    .i_interface = 0,
};

static struct usb_desc_interface ecm_data_intf_desc = {
    .b_length = sizeof(struct usb_desc_interface),
    .b_desc_type = USB_DESC_TYPE_INTERFACE,
    .b_interface_number = 0, /* DYNAMIC */
    .b_alternate_setting = 1,
    .b_num_endpoints = 2,
    .b_interface_class = USB_CLASS_CDC_DATA,
    .b_interface_subclass = 0,
    .b_interface_protocol = 0,
    .i_interface = ECM_STR_DATA_INTERFACE,
};

static const struct usb_desc_common_header *ecm_descs[] = {
    (struct usb_desc_common_header *)&iad_descriptor,
    /* Control interface */
    (struct usb_desc_common_header *)&ecm_control_intf_desc,
    (struct usb_desc_common_header *)&ecm_header_desc,
    (struct usb_desc_common_header *)&ecm_union_desc,
    (struct usb_desc_common_header *)&ecm_eth_desc,
    (struct usb_desc_common_header *)&int_ep_desc,

    /* Data interface alt 0 */
    (struct usb_desc_common_header *)&ecm_data_intf_nop_desc,

    /* Data interface alt 1 */
    (struct usb_desc_common_header *)&ecm_data_intf_desc,
    (struct usb_desc_common_header *)&bulk_ep_tx_desc,
    (struct usb_desc_common_header *)&bulk_ep_rx_desc,
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

/* CDC ECM class-specific requests */
#define CDC_ECM_SET_ETHERNET_PACKET_FILTER 0x43
#define CDC_ECM_GET_ETHERNET_STATISTIC     0x44

static int ecm_setup(struct usb_gadget_function *f,
    const struct usb_control_header *ctrl, uint8_t *buffer) {
	struct usb_gadget *gadget = f->gadget;
	struct usb_gadget_request *req = &gadget->composite->req;

	log_debug("EP0: bm=%02x bReq=%02x wValue=%04x wIndex=%04x wLen=%04x",
	    ctrl->bm_request_type, ctrl->b_request, ctrl->w_value, ctrl->w_index,
	    ctrl->w_length);

	/* 1) STANDARD requests */
	if ((ctrl->bm_request_type & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_STANDARD) {
		switch (ctrl->b_request) {
		case USB_REQ_SET_INTERFACE: {
			/* alt setting in wValue, interface in wIndex */
			uint16_t alt = ctrl->w_value;
			uint16_t intf = ctrl->w_index;

			log_debug("SET_INTERFACE intf=%u alt=%u", intf, alt);

			req->len = 0;
			usb_gadget_ep_queue(&gadget->composite->ep0, req);
			return 0;
		}
		default:
			log_error("Unsupported STANDARD bReq=%02x", ctrl->b_request);
			return -1;
		}
	}

	/* 2) CLASS requests (CDC ECM) */
	if ((ctrl->bm_request_type & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS
	    && (ctrl->bm_request_type & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_IFACE) {
		switch (ctrl->b_request) {
		case CDC_ECM_SET_ETHERNET_PACKET_FILTER:
			log_debug("CDC: SET_ETHERNET_PACKET_FILTER val=%04x", ctrl->w_value);
			req->len = 0;
			usb_gadget_ep_queue(&gadget->composite->ep0, req);
			return 0;

		case CDC_ECM_GET_ETHERNET_STATISTIC:
			log_debug("CDC: GET_ETHERNET_STATISTIC selector=%04x", ctrl->w_value);
			memset(buffer, 0, 4);
			req->len = 4;
			usb_gadget_ep_queue(&gadget->composite->ep0, req);
			return 0;

		default:
			log_error("Unsupported CDC CLASS bReq=%02x", ctrl->b_request);
			return -1;
		}
	}

	log_error("Unsupported request type bm=%02x bReq=%02x",
	    ctrl->bm_request_type, ctrl->b_request);
	return -1;
}

extern int ecm_card_init(struct usb_gadget_ep *tx, struct usb_gadget_ep *rx);

static int ecm_enumerate(struct usb_gadget_function *f) {
	ecm_card_init(&bulk_tx, &bulk_rx);

	usb_gadget_ep_enable(&bulk_tx);
	usb_gadget_ep_enable(&bulk_rx);
	usb_gadget_ep_enable(&intr);

	return 0;
}

static struct usb_gadget_function ecm_func = {
    .name = "ecm",
    .descs = ecm_descs,
    .probe = ecm_probe,
    .setup = ecm_setup,
    .enumerate = ecm_enumerate,
};

static int ecm_probe(struct usb_gadget *gadget) {
	ecm_control_intf_desc.b_interface_number = usb_gadget_add_interface(gadget,
	    &ecm_func);

	ecm_data_intf_nop_desc.b_interface_number = usb_gadget_add_interface(gadget,
	    &ecm_func);

	ecm_data_intf_desc.b_interface_number = ecm_data_intf_nop_desc.b_interface_number;

	/* FIXME */
	intr.udc = gadget->composite->ep0.udc;
	bulk_tx.udc = gadget->composite->ep0.udc;
	bulk_rx.udc = gadget->composite->ep0.udc;

	usb_gadget_ep_configure(gadget, &bulk_tx);
	usb_gadget_ep_configure(gadget, &bulk_rx);
	usb_gadget_ep_configure(gadget, &intr);

	return 0;
}

static int ecm_init(void) {
	usb_gadget_register_function(&ecm_func);

	return 0;
}

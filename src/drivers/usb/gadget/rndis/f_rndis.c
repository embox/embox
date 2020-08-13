/**
 * @file
 *
 * @date May 12, 2020
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <string.h>
#include <embox/unit.h>
#include <util/log.h>
#include <drivers/usb/usb_defines.h>
#include <drivers/usb/usb_desc.h>
#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/gadget/gadget.h>

#include <drivers/usb/function/f_rndis_idx.h>

/* IAD descriptor */
static const uint8_t iad_descriptor[8] = {
		0x08, /*    bLength */
		0x0B, /*    bDescriptorType */
		0x00, /*    bFirstInterface */
		0x02, /*    bInterfaceCount */
		0x02, /*    bFunctionClass */
		0x02, /*    bFunctionSubClass */
		0xFF, /*    bFunctionProtocol */
		RNDIS_STR_CONFIGURATION /*    iFunction   "RNDIS" */
};

/* Interface 0 descriptor */
static struct usb_desc_interface rndis_interface0_desc = {
    sizeof(struct usb_desc_interface),
	USB_DESC_TYPE_INTERFACE,

    0x00, /* uint8  bInterfaceNumber; DYNAMIC */
    0x00,                                           // uint8  bAlternateSetting;
    1,                                              // uint8  bNumEndpoints;
    0x02,                                  			// uint8  bInterfaceClass: Wireless Controller;
    0x02,                                           // uint8  bInterfaceSubClass
    0xFF,                                           // uint8  bInterfaceProtocol
    0      											// uint8  iInterface;
};

static const uint8_t interface0_functional_desc[4][5] = {
		/* Header Functional Descriptor */
		[0] = { 0x05,                              // bFunctionLength
		        0x24,                              // bDescriptorType = CS Interface
		        0x00,                              // bDescriptorSubtype
		        0x10,                              // bcdCDC = 1.10
		        0x01 },                             // bcdCDC = 1.10

		/* Call Management Functional Descriptor */
		[1] = { 0x05,                              // bFunctionLength
		        0x24,                              // bDescriptorType = CS Interface
		        0x01,                              // bDescriptorSubtype = Call Management
		        0x00,                              // bmCapabilities
		        0x01 },                             // bDataInterface

		/* Abstract Control Management Functional Descriptor */
		[2] = { 0x04,                              // bFunctionLength
		        0x24,                              // bDescriptorType = CS Interface
		        0x02,                              // bDescriptorSubtype = Abstract Control Management
		    //  0x08,                              // bmCapabilities = Device supports the notification Network_Connection
		        0x00 },                             // bmCapabilities = Device supports the notification Network_Connection

		/* Union Functional Descriptor */
		[3] = { 0x05,                              // bFunctionLength
		        0x24,                              // bDescriptorType = CS Interface
		        0x06,                              // bDescriptorSubtype = Union
		        0x00,                              // bControlInterface = "RNDIS Communications Control"
		        0x01 },                             // bSubordinateInterface0 = "RNDIS Ethernet Data"
};

static struct usb_desc_interface rndis_interface1_desc = {
	    sizeof(struct usb_desc_interface),
		USB_DESC_TYPE_INTERFACE,

		0x00, /* uint8  bInterfaceNumber; DYNAMIC */
		0x00,                                           // uint8  bAlternateSetting;
		2,                                              // uint8  bNumEndpoints;
		0x0A,                                           // uint8  bInterfaceClass: CDC;
		0x00,                                           // uint8  bInterfaceSubClass;
		0x00,                                           // uint8  bInterfaceProtocol;
		0   											// uint8  iInterface;
};

static int rndis_probe(struct usb_gadget *gadget);

static struct usb_desc_endpoint int_ep_desc = {
	.b_length            = sizeof (struct usb_desc_endpoint),
	.b_desc_type         = USB_DESC_TYPE_ENDPOINT,
	.bm_attributes       = USB_DESC_ENDP_TYPE_INTR,
	.w_max_packet_size   = 8,
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

static const struct usb_desc_common_header *rndis_descs[] = {
	(struct usb_desc_common_header *) &iad_descriptor,
	(struct usb_desc_common_header *) &rndis_interface0_desc,
	(struct usb_desc_common_header *) &interface0_functional_desc[0],
	(struct usb_desc_common_header *) &interface0_functional_desc[1],
	(struct usb_desc_common_header *) &interface0_functional_desc[2],
	(struct usb_desc_common_header *) &interface0_functional_desc[3],
	(struct usb_desc_common_header *) &int_ep_desc,
	(struct usb_desc_common_header *) &rndis_interface1_desc,
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

static struct usb_gadget_function rndis_func = {
	.name = "rndis",
	.descs = rndis_descs,
	.probe = rndis_probe,
};

static int rndis_probe(struct usb_gadget *gadget) {
	rndis_interface0_desc.b_interface_number =
		usb_gadget_add_interface(gadget, &rndis_func);

	rndis_interface1_desc.b_interface_number =
		usb_gadget_add_interface(gadget, &rndis_func);

	/* FIXME */
	intr.udc    = gadget->composite->ep0.udc;
	bulk_tx.udc = gadget->composite->ep0.udc;
	bulk_rx.udc = gadget->composite->ep0.udc;

	usb_gadget_ep_configure(gadget, &bulk_tx);
	usb_gadget_ep_configure(gadget, &bulk_rx);
	usb_gadget_ep_configure(gadget, &intr);

	/* TODO Endpoints are not enabled. */

	return 0;
}

EMBOX_UNIT_INIT(rndis_init);

static int rndis_init(void) {
	usb_gadget_register_function(&rndis_func);
	return 0;
}

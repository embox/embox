/**
 * @file
 * @brief https://docs.zephyrproject.org/latest/samples/subsys/usb/testusb/README.html
 *
 * @author  Anton Bondarev
 * @date    02.05.2026
 */

#include <util/log.h>

#include <stdint.h>
#include <string.h>


#include <drivers/usb/usb_defines.h>
#include <drivers/usb/usb_desc.h>
#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/gadget/gadget.h>


#include <embox/unit.h>

EMBOX_UNIT_INIT(f_loopback_init);

#if 0
struct loopback_desc {
	struct usb_association_descriptor iad;
	struct usb_if_descriptor if0;
	struct usb_ep_descriptor if0_out_ep;
	struct usb_ep_descriptor if0_in_ep;
	struct usb_ep_descriptor if0_hs_out_ep;
	struct usb_ep_descriptor if0_hs_in_ep;
	struct usb_if_descriptor if1;
	struct usb_ep_descriptor if1_int_out_ep;
	struct usb_ep_descriptor if1_int_in_ep;
	struct usb_if_descriptor if2_0;
	struct usb_ep_descriptor if2_0_iso_in_ep;
	struct usb_ep_descriptor if2_0_iso_out_ep;
	struct usb_if_descriptor if2_1;
	struct usb_ep_descriptor if2_1_iso_in_ep;
	struct usb_ep_descriptor if2_1_iso_out_ep;
	struct usb_desc_header nil_desc;
};
#endif

static struct usb_desc_endpoint loopback_ep_desc = {
	.b_length            = sizeof (struct usb_desc_endpoint),
	.b_desc_type         = USB_DESC_TYPE_ENDPOINT,
	.bm_attributes       = USB_DESC_ENDP_TYPE_INTR,
	.w_max_packet_size   = 16,
	.b_interval          = 100,
};


static struct usb_desc_interface loopback_intf_desc = {
	.b_length                = sizeof(struct usb_desc_interface),
	.b_desc_type             = USB_DESC_TYPE_INTERFACE,
	.b_interface_number      = 0, /* DYNAMIC */
	.b_alternate_setting     = 0,
	.b_num_endpoints         = 1,
	.b_interface_class       = USB_CLASS_HID,
	.b_interface_subclass    = 0, /* 1 */
	.b_interface_protocol    = 0,
	.i_interface             = 0,
};

static const struct usb_desc_common_header *f_loopback_descs[] = {

	(struct usb_desc_common_header *) &loopback_intf_desc,
	(struct usb_desc_common_header *) &loopback_ep_desc,

	NULL,
};

static struct usb_gadget_ep loopback_ep = {
	.dir = USB_DIR_IN,
	.desc = &loopback_ep_desc,
};

static int f_loopback_setup(struct usb_gadget_function *f,
		const struct usb_control_header *ctrl, uint8_t *buffer) {
	struct usb_gadget *gadget = f->gadget;
	struct usb_gadget_request *req = &gadget->composite->req;

	log_debug("EP0: bm=%02x bReq=%02x wValue=%04x wIndex=%04x wLen=%04x",
	    ctrl->bm_request_type, ctrl->b_request, ctrl->w_value, ctrl->w_index,
	    ctrl->w_length);
	if ((ctrl->bm_request_type & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_STANDARD) {
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
	}
	/* 2) CLASS requests */

	log_error("Unsupported EP0: bm=%02x bReq=%02x wValue=%04x wIndex=%04x wLen=%04x",
	    ctrl->bm_request_type, ctrl->b_request, ctrl->w_value, ctrl->w_index,
	    ctrl->w_length);

	return -1;
}

static int f_loopback_enumerate(struct usb_gadget_function *f) {

	usb_gadget_ep_enable(&loopback_ep);

	return 0;
}


static int f_loopback_probe(struct usb_gadget *gadget) ;

static struct usb_gadget_function loopback_func = {
	.name = "loopback",
	.descs = f_loopback_descs,
	.probe = f_loopback_probe,
	.setup = f_loopback_setup,
	.enumerate = f_loopback_enumerate,
};

static int f_loopback_probe(struct usb_gadget *gadget) {

	return 0;
}

static int f_loopback_init(void) {
	usb_gadget_register_function(&loopback_func);

	return 0;
}

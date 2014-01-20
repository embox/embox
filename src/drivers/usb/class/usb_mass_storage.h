/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    20.01.2014
 */

#ifndef USB_CLASS_USB_MASS_STORAGE_H_
#define USB_CLASS_USB_MASS_STORAGE_H_

#define USB_CLASS_MASS        8
#define USB_MASS_SUBCL_SCSI   6

#define USB_MASS_PROTO_CBICI  0
#define USB_MASS_PROTO_CBINCI 1
#define USB_MASS_PROTO_BBB    80

#define USB_MASS_MAX_DEVS     2

#define USB_REQ_MASS_RESET    0xff

#include <drivers/usb/usb.h>

#define USB_CBW_CB_MAXLEN 16
struct usb_mscbw {
	uint32_t cbw_signature;
	uint32_t cbw_tag;
	uint32_t cbw_transfer_len;
	uint8_t  cbw_flags;
	uint8_t  cbw_lun;
	uint8_t  cbw_len;
	uint8_t  cbw_cb[USB_CBW_CB_MAXLEN];
} __attribute__((packed));

struct usb_mscsw {
	uint32_t csw_signature;
	uint32_t csw_tag;
	uint32_t csw_data_resude;
	uint8_t  csw_status;
} __attribute__((packed));

struct scsi_cmd_inquiry {
	uint8_t  sinq_opcode;
	uint8_t  sinq_flags;
	uint8_t  sinq_page_code;
	uint16_t sinq_alloc_length;
	uint8_t  sinq_control;
} __attribute__((packed));

struct scsi_data_inquiry {
	uint8_t data_buf[36];
} __attribute__((packed));

struct usb_mass_request_ctx {
	enum {
		USB_MASS_REQST_CBW,
		USB_MASS_REQST_DATA,
		USB_MASS_REQST_CSW,
	} req_state;

	enum usb_direction dir;
	void *buf;
	size_t len;

	struct usb_mscbw cbw;
	struct usb_mscsw csw;

	usb_request_notify_hnd_t holded_hnd;
};

struct usb_mass {
	char blkin, blkout;

	struct scsi_data_inquiry data_inquiry;

	struct usb_mass_request_ctx req_ctx;
};

static inline struct usb_mass *usb2massdata(struct usb_dev *dev) {
	return dev->class_specific;
}

extern void usb_scsi_handle(struct usb_dev *dev);

#endif /* USB_CLASS_USB_MASS_STORAGE_H_ */


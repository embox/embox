/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    20.01.2014
 */

#ifndef USB_CLASS_USB_MASS_STORAGE_H_
#define USB_CLASS_USB_MASS_STORAGE_H_

#include <drivers/usb/usb.h>

#define USB_CLASS_MASS        8
#define USB_MASS_SUBCL_SCSI   6

#define USB_MASS_PROTO_CBICI  0
#define USB_MASS_PROTO_CBINCI 1
#define USB_MASS_PROTO_BBB    80

#define USB_MASS_MAX_DEVS     2

#define USB_REQ_MASS_RESET    0xff
#define USB_REQ_MASS_MAXLUN   0xfe

#define USB_CBW_CB_MAXLEN 16

#include <drivers/scsi.h>

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
	struct scsi_dev scsi_dev;
	struct usb_dev *usb_dev;

	uint8_t maxlun;

	char blkin, blkout;
	struct usb_mass_request_ctx req_ctx;
};

static inline struct usb_mass *usb2massdata(struct usb_dev *dev) {
	return dev->driver_data;
}

extern int usb_ms_transfer(struct usb_dev *dev, void *ms_cmd,
		size_t ms_cmd_len, enum usb_direction dir, void *buf, size_t len,
	       	usb_request_notify_hnd_t notify_hnd);

#endif /* USB_CLASS_USB_MASS_STORAGE_H_ */


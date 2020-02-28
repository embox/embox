/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.10.2013
 */

#ifndef DRIVERS_USB_H_
#define DRIVERS_USB_H_

#include <stdint.h>

#include <kernel/thread/sync/mutex.h>
#include <kernel/sched/waitq.h>
#include <util/indexator.h>
#include <util/dlist.h>

#include <drivers/usb/usb_queue.h>
#include <drivers/usb/usb_desc.h>
#include <drivers/usb/usb_token.h>

#define USB_MAX_HCD 2
#define USB_MAX_DEV 32
#define USB_MAX_HUB 7
#define USB_MAX_ENDP (USB_MAX_DEV * 2)
#define USB_MAX_REQ USB_MAX_DEV
#define USB_REQ_STD_LEN 64

#define USB_REQ_HEADER_LEN 8
#define USB_DEV_MAX_ENDP 32
#define USB_RH_MAX_PORT 15
#define USB_HC_MAX_DEV 127

struct usb_hcd;
struct usb_hub;
struct usb_dev;
struct usb_endp;
struct usb_request;

typedef void (*usb_request_notify_hnd_t)(struct usb_request *req, void *arg);

#define USB_DIR_OUT                 0x00
#define USB_DIR_IN                  0x80

#define USB_REQ_TYPE_STANDARD       0x00
#define USB_REQ_TYPE_CLASS          0x20
#define USB_REQ_TYPE_VENDOR         0x40

#define USB_REQ_RECIP_DEVICE        0x00
#define USB_REQ_RECIP_IFACE         0x01
#define USB_REQ_RECIP_ENDP          0x02
#define USB_REQ_RECIP_OTHER         0x03

#define USB_REQ_GET_STATUS          0x00
#define USB_REQ_CLEAR_FEATURE       0x01
#define USB_REQ_SET_FEATURE         0x03
#define USB_REQ_SET_ADDRESS         0x05
#define USB_REQ_GET_DESCRIPTOR      0x06
#define USB_REQ_SET_CONFIG          0x09

#define USB_PORT_FEATURE_CONNECTION      0
#define USB_PORT_FEATURE_ENABLE          1
#define USB_PORT_FEATURE_RESET           4
#define USB_PORT_FEATURE_POWER           8
#define USB_PORT_FEATURE_C_CONNECTION    16
#define USB_PORT_FEATURE_C_ENABLE        17
#define USB_PORT_FEATURE_C_RESET         20

#define USB_PORT_STAT_CONNECTION    0x0001
#define USB_PORT_STAT_ENABLE        0x0002
#define USB_PORT_STAT_SUSPEND       0x0004
#define USB_PORT_STAT_OVERCURRENT   0x0008
#define USB_PORT_STAT_RESET         0x0010

#define USB_CLASS_HUB  0x9
#define USB_DT_HUB     (USB_REQ_TYPE_CLASS | USB_CLASS_HUB)

/* Hub request types */
#define USB_RT_HUB  (USB_REQ_TYPE_CLASS | USB_REQ_RECIP_DEVICE)
#define USB_RT_PORT (USB_REQ_TYPE_CLASS | USB_REQ_RECIP_OTHER)

/* Class requests from the USB 2.0 hub spec, table 11-15 */
#define HUB_CLASS_REQ(dir, type, request) ((((dir) | (type)) << 8) | (request))

#define USB_CLEAR_HUB_FEATURE     HUB_CLASS_REQ(USB_DIR_OUT, USB_RT_HUB, USB_REQ_CLEAR_FEATURE)
#define USB_CLEAR_PORT_FEATURE    HUB_CLASS_REQ(USB_DIR_OUT, USB_RT_PORT, USB_REQ_CLEAR_FEATURE)
#define USB_GET_HUB_DESCRIPTOR    HUB_CLASS_REQ(USB_DIR_IN, USB_RT_HUB, USB_REQ_GET_DESCRIPTOR)
#define USB_GET_HUB_STATUS        HUB_CLASS_REQ(USB_DIR_IN, USB_RT_HUB, USB_REQ_GET_STATUS)
#define USB_GET_PORT_STATUS       HUB_CLASS_REQ(USB_DIR_IN, USB_RT_PORT, USB_REQ_GET_STATUS)
#define USB_SET_HUB_FEATURE       HUB_CLASS_REQ(USB_DIR_OUT, USB_RT_HUB, USB_REQ_SET_FEATURE)
#define USB_SET_PORT_FEATURE      HUB_CLASS_REQ(USB_DIR_OUT, USB_RT_PORT, USB_REQ_SET_FEATURE)

#define HUB_PORT_STATUS     0

enum usb_request_status {
	USB_REQ_NOERR,
	USB_REQ_STALL,
	USB_REQ_UNDERRUN,
	USB_REQ_INTERR,
};

enum usb_comm_type {
	USB_COMM_CONTROL,
	USB_COMM_INTERRUPT,
	USB_COMM_BULK,
	USB_COMM_ISOCHRON,
};

enum usb_direction {
	USB_DIRECTION_OUT,
	USB_DIRECTION_IN,
	USB_DIRECTION_BI,
};

enum usb_speed {
    USB_SPEED_HIGH = 0,
    USB_SPEED_FULL = 1,
    USB_SPEED_LOW  = 2,
};

struct usb_hcd_ops {
	void *(*hcd_hci_alloc)(struct usb_hcd *hcd, void *args);
	void (*hcd_hci_free)(struct usb_hcd *hcd, void *spec);
	void *(*endp_hci_alloc)(struct usb_endp *endp);
	void (*endp_hci_free)(struct usb_endp *endp, void *spec);
	void *(*req_hci_alloc)(struct usb_request *req);
	void (*req_hci_free)(struct usb_request *req, void *spec);

	int (*hcd_start)(struct usb_hcd *hcd);
	int (*hcd_stop)(struct usb_hcd *hcd);

	int (*root_hub_control)(struct usb_request *req);

	int (*request)(struct usb_request *req);
};

struct usb_endp {
	struct usb_dev *dev;
	unsigned char address;
	enum usb_direction direction;
	enum usb_comm_type type;
	unsigned short max_packet_size;
	unsigned char interval;

	struct usb_queue req_queue;

	void *hci_specific;
};

struct usb_dev {
	unsigned short bus_idx;
	unsigned short addr;
	struct dlist_head dev_link;
	struct usb_dev *parent;

	struct usb_hcd *hcd;
	unsigned char endp_n;
	struct usb_endp *endpoints[USB_DEV_MAX_ENDP];

	void *config_buf;
	struct usb_desc_device dev_desc;
	struct usb_desc_interface iface_desc;

	void *driver_data;
	struct usb_driver *drv;

	enum usb_speed speed;
};

struct usb_hub {
	struct usb_dev *dev;
	struct dlist_head lnk;
	unsigned port_n;
	struct mutex mutex;
};

struct usb_hcd {
	struct usb_hcd_ops *ops;
	struct dlist_head lnk;
	struct usb_hub *root_hub;

	index_data_t idx_data[INDEX_DATA_LEN(USB_HC_MAX_DEV)];
	struct indexator enumerator;

	void *hci_specific;
};

struct usb_request {
	struct usb_endp *endp;
	usb_token_t token;
	char *buf;
	size_t len;
	size_t actual_len;
	enum usb_request_status req_stat;
	usb_request_notify_hnd_t notify_hnd;
	void *hnd_data;
	struct waitq wq;

	struct usb_queue_link req_link;

	void *hci_specific;
	struct usb_control_header ctrl_header;
	uint8_t buffer[1024];
};

static inline int is_root_hub(struct usb_dev *dev) {
	return dev->parent == NULL;
}

static inline enum usb_comm_type usb_endp_type(struct usb_endp *endp) {
	return endp->type;
}

extern struct usb_hcd *usb_hcd_alloc(struct usb_hcd_ops *ops, void *args);

extern void usb_hcd_free(struct usb_hcd *hcd);

extern int usb_hcd_register(struct usb_hcd *hcd);

extern void usb_request_complete(struct usb_request *req);

extern int usb_endp_control(struct usb_endp *endp,
		usb_request_notify_hnd_t notify_hnd, void *arg,
		uint8_t req_type, uint8_t request, uint16_t value, uint16_t index,
		uint16_t count, void *data);

extern int usb_endp_control_wait(struct usb_endp *endp,
		uint8_t req_type, uint8_t request, uint16_t value, uint16_t index,
		uint16_t count, void *data, int timeout);

extern int usb_endp_bulk(struct usb_endp *endp, usb_request_notify_hnd_t hnd,
		void *buf, size_t len);
extern int usb_endp_bulk_wait(struct usb_endp *endp, void *buf,
	    size_t len, int timeout);

extern int usb_endp_interrupt(struct usb_endp *endp,
		usb_request_notify_hnd_t notify_hnd,
		void *buf, size_t len);

extern struct usb_dev *usb_new_device(struct usb_dev *parent,
		struct usb_hcd *hcd, unsigned int port);

extern struct usb_dev *usb_dev_iterate(struct usb_dev *dev);

extern int usb_get_configuration(struct usb_dev *dev, unsigned int n);
extern int usb_set_configuration(struct usb_dev *dev, unsigned int n);

/* FIXME This function is more like workaround to initialize
 * control endpoint. */
extern int usb_get_ep0(struct usb_dev *dev);

#endif /* DRIVERS_USB_H_ */

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
#include <lib/libds/indexator.h>
#include <lib/libds/dlist.h>

#include <drivers/usb/usb_defines.h>
#include <drivers/usb/usb_queue.h>
#include <drivers/usb/usb_desc.h>
#include <drivers/usb/usb_token.h>

#include <framework/mod/options.h>
#include <module/embox/driver/usb/core.h>

#define USB_MAX_HCD \
    OPTION_MODULE_GET(embox__driver__usb__core, NUMBER, usb_max_hcd)

#define USB_HC_MAX_DEV \
    OPTION_MODULE_GET(embox__driver__usb__core, NUMBER, usb_max_hcd_devs)

#define USB_MAX_DEV \
    OPTION_MODULE_GET(embox__driver__usb__core, NUMBER, usb_max_dev)

#define USB_MAX_HUB \
    OPTION_MODULE_GET(embox__driver__usb__core, NUMBER, usb_max_hub)

#define USB_MAX_REQ \
    OPTION_MODULE_GET(embox__driver__usb__core, NUMBER, usb_max_req)

#define USB_DEV_MAX_ENDP \
    OPTION_MODULE_GET(embox__driver__usb__core, NUMBER, usb_dev_max_endp)

#define USB_DEV_MAX_INTERFACES \
    OPTION_MODULE_GET(embox__driver__usb__core, NUMBER, usb_dev_max_interfaces)

#define USB_DEV_MAX_CONFIG \
	2

#define USB_REQ_HEADER_LEN 8
#define USB_MAX_ENDP (USB_MAX_DEV * USB_DEV_MAX_ENDP)

struct usb_hcd;
struct usb_hub;
struct usb_dev;
struct usb_endp;
struct usb_request;
struct usb_interface;

typedef void (*usb_request_notify_hnd_t)(struct usb_request *req, void *arg);

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

	void *hci_specific;
};

struct usb_dev_config {
	void *config_buf;

	struct usb_interface *usb_iface[USB_DEV_MAX_INTERFACES];
	int usb_iface_num;
};

struct usb_dev {
	unsigned short bus_idx;
	unsigned short addr;
	struct dlist_head dev_link;
	struct usb_dev *parent;

	struct usb_hcd *hcd;

	struct usb_desc_device dev_desc;

	struct usb_dev_config usb_dev_configs[USB_DEV_MAX_CONFIG];
	struct usb_dev_config *current_config;

	struct usb_endp endp0;

	enum usb_speed speed;
};

struct usb_interface {
	struct usb_dev *usb_dev;

	struct usb_desc_interface *iface_desc[USB_DEV_MAX_INTERFACES];
	unsigned char endp_n;
	struct usb_endp *endpoints[USB_DEV_MAX_ENDP];

	void *driver_data;
	struct usb_driver *drv;
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

static inline struct usb_request *usb_link2req(struct usb_queue_link *ul) {
	return member_cast_out(ul, struct usb_request, req_link);
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
		void *arg, void *buf, size_t len);
extern int usb_endp_bulk_wait(struct usb_endp *endp, void *buf,
	    size_t len, int timeout);

extern int usb_endp_interrupt(struct usb_endp *endp,
		usb_request_notify_hnd_t notify_hnd,
		void *buf, size_t len);

extern struct usb_dev *usb_new_device(struct usb_dev *parent,
		struct usb_hcd *hcd, unsigned int port);

extern struct usb_dev *usb_dev_iterate(struct usb_dev *dev);

extern int usb_get_config_desc(struct usb_dev *dev, unsigned int n);
extern int usb_get_configuration(struct usb_dev *dev, unsigned int n, int len);
extern int usb_set_configuration(struct usb_dev *dev, unsigned int n);
extern void usb_free_configuration(struct usb_dev *dev);
extern int usb_set_iface(struct usb_dev *dev, int iface, int alt);

/* FIXME This function is more like workaround to initialize
 * control endpoint. */
extern int usb_get_ep0(struct usb_dev *dev);

#endif /* DRIVERS_USB_H_ */

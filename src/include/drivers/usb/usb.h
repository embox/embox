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
#include <kernel/time/timer.h>
#include <util/indexator.h>
#include <util/dlist.h>

#include <drivers/usb/usb_queue.h>
#include <drivers/usb/usb_desc.h>
#include <drivers/usb/usb_token.h>

#define USB_RESET_HIGH_DELAY_MS 20

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

typedef unsigned short usb_hub_state_t;
typedef unsigned short usb_hub_port_t;

struct usb_hcd;
struct usb_hub;
struct usb_hub_port;
struct usb_dev;
struct usb_endp;
struct usb_request;
enum usb_dev_event_type;

typedef void (*usb_dev_notify_hnd_t)(struct usb_dev *dev, enum usb_dev_event_type event_type);
typedef void (*usb_request_notify_hnd_t)(struct usb_request *req, void *arg);

#define USB_DEV_REQ_TYPE_WR             0x00
#define USB_DEV_REQ_TYPE_RD             0x80

#define USB_DEV_REQ_TYPE_STD            0x00
#define USB_DEV_REQ_TYPE_CLS            0x20
#define USB_DEV_REQ_TYPE_VND            0x40

#define USB_DEV_REQ_TYPE_DEV            0x00
#define USB_DEV_REQ_TYPE_IFC            0x01
#define USB_DEV_REQ_TYPE_ENP            0x02
#define USB_DEV_REQ_TYPE_OTH            0x03

#define USB_DEV_REQ_GET_STAT            0x00
#define USB_DEV_REQ_SET_ADDR            0x05
#define USB_DEV_REQ_GET_DESC            0x06
#define USB_DEV_REQ_SET_CONF            0x09

#define USB_HUB_PORT_CONNECT            0x0001
#define USB_HUB_PORT_ENABLE             0x0002
#define USB_HUB_PORT_SUSPEND            0x0004
#define USB_HUB_PORT_OVERRUN            0x0008
#define USB_HUB_PORT_RESET              0x0010
#define USB_HUB_PORT_POWER              0x0020
#define USB_HUB_PORT_TIMEOUT            0x0040

enum __usb_hub_request {
    USB_HUB_REQ_GET_STATUS       = 0,
    USB_HUB_REQ_CLEAR_FEATURE    = 1,
    USB_HUB_REQ_SET_FEATURE      = 3,
    USB_HUB_REQ_GET_DESCRIPTOR   = 6,
    USB_HUB_REQ_SET_DESCRIPTOR   = 7,
    USB_HUB_REQ_CLEAR_TT_BUFFER  = 8,
    USB_HUB_REQ_RESET_TT         = 9,
    USB_HUB_REQ_GET_TT_STATE     = 10,
    USB_HUB_REQ_STOP_TT          = 11,
};

enum usb_hub_request {
	USB_HUB_REQ_PORT_SET,
	USB_HUB_REQ_PORT_CLEAR,
};

enum usb_host_state {
	USB_HOST_IDLE,
	USB_HOST_ENUMERATING,
};

enum usb_dev_plug_state {
	USB_DEV_DEFAULT,
	USB_DEV_ADDRESSED,
	USB_DEV_CONFIGURED,
	USB_DEV_DETACHED,
};

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

enum usb_dev_event_type {
	USB_DEV_EVENT_PORT,
	USB_DEV_EVENT_POSTED,
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

	int (*rhub_ctrl)(struct usb_hub_port *port, enum usb_hub_request req,
			unsigned short value);

	int (*request)(struct usb_request *req);
};

extern void usb_hub_ctrl(struct usb_hub_port *port, enum usb_hub_request request,
		unsigned short value);

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

static inline void usb_endp_fill_from_desc(struct usb_endp *endp,
	       	const struct usb_desc_endpoint *desc) {

	endp->address = desc->b_endpoint_address & USB_DESC_ENDP_ADDR_MASK;

	switch (desc->b_endpoint_address & USB_DESC_ENDP_ADDR_DIR_MASK) {
	case USB_DESC_ENDP_ADDR_OUT:
		endp->direction = USB_DIRECTION_OUT;
		break;
	default:
	case USB_DESC_ENDP_ADDR_IN:
		endp->direction = USB_DIRECTION_IN;
		break;
	}

	switch (desc->bm_attributes & USB_DESC_ENDP_TYPE_MASK) {
	case USB_DESC_ENDP_TYPE_ISOCHR:
		endp->type = USB_COMM_ISOCHRON;
		break;
	case USB_DESC_ENDP_TYPE_BULK:
		endp->type = USB_COMM_BULK;
		break;
	case USB_DESC_ENDP_TYPE_INTR:
		endp->type = USB_COMM_INTERRUPT;
		break;
	default:
	case USB_DESC_ENDP_TYPE_CTRL:
		endp->type = USB_COMM_CONTROL;
		endp->direction = USB_DIRECTION_BI;
		break;
	}
	endp->max_packet_size = desc->w_max_packet_size;
	endp->interval = desc->b_interval;
}

struct usb_desc_getconf_data {
	struct usb_desc_configuration config_desc;
	struct usb_desc_interface interface_desc;
	struct usb_desc_endpoint endp_descs[USB_DEV_MAX_ENDP];
} __attribute__((packed));

struct usb_dev {
	enum usb_dev_plug_state plug_state;
	unsigned int use_count;

	unsigned short idx; /**< index allocated for device */
	unsigned short bus_idx; /**<  index of device on bus. On `reseted' is 0,
				   after `addressed' is idx */
	struct dlist_head dev_link;

	struct usb_hcd *hcd;
	struct usb_hub_port *port;
	unsigned char endp_n;
	struct usb_endp *endpoints[USB_DEV_MAX_ENDP];
	unsigned char c_config;
	unsigned char c_interface;

	struct usb_desc_device dev_desc;
	struct usb_desc_interface iface_desc;
	struct usb_desc_getconf_data *getconf_data;
	struct usb_desc_getconf_data tgetconf_data;

	struct usb_desc_interface *interface_desc;

	void *class_specific;

	struct usb_driver *drv;
	void *drv_data;

	enum usb_speed speed;
};

static inline struct usb_desc_device *usb_dev_get_desc(struct usb_dev *dev) {
	return &dev->dev_desc;
}

typedef void (*usb_hub_port_state_t)(struct usb_hub_port *port);

struct usb_hub_status {
    union {
        uint16_t w_hub_status;
        struct {
            uint16_t local_power : 1;
            uint16_t overcurrent : 1;
            uint16_t wHubStatus_reserved : 14;
        };
    };
    union {
        uint16_t w_hub_change;
        struct {
            uint16_t local_power_changed : 1;
            uint16_t overcurrent_changed : 1;
            uint16_t wHubChange_reserved : 14;
        };
    };
} __attribute__((packed));

struct usb_port_status {
    union {
        uint16_t port_status;
        struct {
            uint16_t connected : 1;
            uint16_t enabled : 1;
            uint16_t suspended : 1;
            uint16_t overcurrent : 1;
            uint16_t reset : 1;
            uint16_t wPortStatus_reserved1 : 3;
            uint16_t powered : 1;
            uint16_t low_speed_attached : 1;
            uint16_t high_speed_attached : 1;
            uint16_t test_mode : 1;
            uint16_t indicator_control : 1;
            uint16_t wPortStatus_reserved2 : 3;
        };
    };
    union {
        uint16_t port_change;
        struct {
            uint16_t connected_changed : 1;
            uint16_t enabled_changed : 1;
            uint16_t suspended_changed : 1;
            uint16_t overcurrent_changed : 1;
            uint16_t reset_changed : 1;
            uint16_t wPortChange_reserved : 11;
        };
    };
} __attribute__((packed));

struct usb_hub_port {
	struct usb_hub *hub;
	usb_hub_port_state_t state;
	struct sys_timer post_timer;

	int idx;
	usb_hub_state_t status;
	usb_hub_state_t changed;

	struct usb_queue_link reset_link;

	struct usb_dev *dev;
};

struct usb_hub {
	struct usb_hcd *hcd;
	usb_hub_port_t port_n;
	struct usb_hub_port ports[USB_RH_MAX_PORT];
};

static inline void usb_port_device_bind(struct usb_hub_port *port,
	       	struct usb_dev *dev) {
	port->dev = dev;
	dev->port = port;
}

static inline void usb_port_device_unbind(struct usb_hub_port *port,
	       	struct usb_dev *dev) {
	port->dev = NULL;
	dev->port = NULL;
}

struct usb_hcd {
	enum usb_host_state state;
	struct usb_hcd_ops *ops;
	struct dlist_head lnk;
	struct usb_hub *root_hub;

	index_data_t idx_data[INDEX_DATA_LEN(USB_HC_MAX_DEV)];
	struct indexator enumerator;
	struct usb_queue reset_queue;

	void *hci_specific;
};

struct usb_request {
	struct usb_endp *endp;
	usb_token_t token;
	char *buf;
	size_t len;
	enum usb_request_status req_stat;
	usb_request_notify_hnd_t notify_hnd;
	void *hnd_data;

	struct usb_queue_link req_link;

	void *hci_specific;
	struct usb_control_header ctrl_header;
};

static inline enum usb_comm_type usb_endp_type(struct usb_endp *endp) {
	return endp->type;
}

extern const struct usb_desc_endpoint usb_desc_endp_control_default;

extern int usb_hcd_register(struct usb_hcd *hcd);

extern int usb_rh_nofity(struct usb_hcd *hcd);

extern void usb_request_complete(struct usb_request *req);

extern int usb_endp_interrupt(struct usb_endp *endp, usb_request_notify_hnd_t notify_hnd,
		void *buf, size_t len);

extern int usb_endp_control(struct usb_endp *endp, usb_request_notify_hnd_t notify_hnd, void *arg,
		uint8_t req_type, uint8_t request, uint16_t value, uint16_t index,
		uint16_t count, void *data);

extern int usb_endp_bulk(struct usb_endp *endp, usb_request_notify_hnd_t hnd,
		void *buf, size_t len);

extern void usb_dev_configure(struct usb_dev *dev);
extern void usb_dev_addr_assign(struct usb_dev *dev);
extern void usb_dev_request_delete(struct usb_dev *dev);

extern int usb_whitelist_check(struct usb_dev *dev);
extern void usb_whitelist_accepts(struct usb_dev *dev);
extern void usb_whitelist_rejects(struct usb_dev *dev);

/* user interface */
extern int usb_endp_request(struct usb_endp *endp, struct usb_request *req);

/* pnp */
extern struct usb_dev *usb_dev_iterate(struct usb_dev *dev);
extern void usb_dev_configured(struct usb_dev *dev);
extern void usb_dev_addr_settled(struct usb_dev *dev);

extern void usb_port_reset_done(struct usb_hub_port *port);
extern void usb_dev_addr_assigned(struct usb_dev *dev);
extern void usb_port_notify(struct usb_hub_port *port);

extern int usb_port_reset_unlock(struct usb_hub_port *port);
extern void usb_dev_disconnect(struct usb_hub_port *port);

/* port */
extern void usb_hub_port_init(struct usb_hub_port *port, struct usb_hub *hub,
	       	usb_hub_port_t i);
extern int usb_port_address_setle_wait(struct usb_hub_port *port, int ms);
/* obj */
extern struct usb_hcd *usb_hcd_alloc(struct usb_hcd_ops *ops, void *args);
extern void usb_hcd_free(struct usb_hcd *hcd);

extern struct usb_hub *usb_hub_alloc(struct usb_hcd *hcd, usb_hub_port_t port_n);
extern void usb_hub_free(struct usb_hub *hub);

extern struct usb_dev *usb_dev_alloc(struct usb_hcd *hcd);
extern void usb_dev_use_dec(struct usb_dev *dev);
extern void usb_dev_use_inc(struct usb_dev *dev);

extern struct usb_endp *usb_endp_alloc(struct usb_dev *dev,
		const struct usb_desc_endpoint *endp_desc);
extern void usb_endp_free(struct usb_endp *endp);

extern struct usb_request *usb_endp_request_alloc(struct usb_endp *endp,
		usb_request_notify_hnd_t notify_hnd, void *arg, unsigned token,
		void *buf, size_t len);

extern void usb_request_free(struct usb_request *req);

/* class */

typedef unsigned char usb_class_t;

struct usb_class {
	struct dlist_head lnk;
	usb_class_t usb_class;
	int (*get_conf)(struct usb_class *cls, struct usb_dev *dev);
	void (*get_conf_hnd)(struct usb_request *req, void *arg);
	void *(*class_alloc)(struct usb_class *cls, struct usb_dev *dev);
	void (*class_free)(struct usb_class *cls, struct usb_dev *dev, void *spec);
	void (*class_handle)(struct usb_class *cls, struct usb_dev *dev);
	void (*class_release)(struct usb_class *cls, struct usb_dev *dev);
};

static inline usb_class_t usb_dev_class(struct usb_dev *dev) {
	usb_class_t dev_class = usb_dev_get_desc(dev)->b_dev_class;

	if (dev_class) {
		return dev_class;
	}

	return dev->interface_desc->b_interface_class;
}

extern int usb_class_supported(struct usb_dev *dev);
extern int usb_class_handle(struct usb_dev *dev);
extern void usb_class_release(struct usb_dev *dev);

extern int usb_dev_generic_fill_iface(struct usb_dev *dev, struct usb_desc_interface *idesc);
extern int usb_dev_generic_fill_endps(struct usb_dev *dev, struct usb_desc_endpoint endp_descs[]);

extern int usb_class_generic_get_conf(struct usb_class *class, struct usb_dev *dev);
extern void usb_class_generic_get_conf_hnd(struct usb_request *req, void *arg);
extern void usb_class_released(struct usb_dev *dev);
extern void usb_class_start_handle(struct usb_dev *dev);

extern int usb_class_register(struct usb_class *cls);
#endif /* DRIVERS_USB_H_ */


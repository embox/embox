/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    09.12.2019
 */

#include <unistd.h>
#include <mem/misc/pool.h>
#include <embox/unit.h>
#include <util/dlist.h>
#include <util/log.h>
#include <util/err.h>
#include <kernel/thread.h>
#include <drivers/usb/usb_driver.h>
#include <drivers/usb/usb.h>

#define USB_HUBS_HANDLE_INTERVAL (1000 * 1000) /* 1 sec */
#define USB_HUB_PORT_STS_TIMEOUT 1000
#define USB_CTRL_GET_TIMEOUT     1000

EMBOX_UNIT_INIT(usb_hub_driver_init);

POOL_DEF(usb_hubs, struct usb_hub, USB_MAX_HUB);
POOL_DEF(usb_devs, struct usb_dev, USB_MAX_DEV);

static DLIST_DEFINE(usb_hubs_list);
static DLIST_DEFINE(usb_devs_list);
static struct thread *usb_hubs_thread;

static int usb_hub_port_init(struct usb_hub *hub, struct usb_dev *dev,
		unsigned int port_nr);

static struct usb_hub *usb_dev_to_hub(struct usb_dev *dev) {
	return (struct usb_hub *)dev->driver_data;
}

/* TODO May be to use usb bus instead of hcd and
 * get hcd from bus? */
struct usb_dev *usb_new_device(struct usb_dev *parent,
		struct usb_hcd *hcd, unsigned int port) {
	struct usb_dev *dev;
	struct usb_hub *hub;

	dev = pool_alloc(&usb_devs);
	if (!dev) {
		log_error("%s: pool_alloc failed\n", __func__);
		return NULL;
	}
	memset(dev, 0, sizeof(struct usb_dev));
	dlist_head_init(&dev->dev_link);
	dlist_add_next(&dev->dev_link, &usb_devs_list);

	dev->hcd = hcd;
	dev->parent = parent;

	/* Fill control endpoint. */
	if (usb_get_ep0(dev) < 0) {
		goto out_err;
	}

	if (parent) { /* It's not a Root Hub */
		hub = usb_dev_to_hub(parent);
		assert(hub);
		/* Here we reset device and set address. */
		if (usb_hub_port_init(hub, dev, port) < 0) {
			goto out_err;
		}
		/* Fill device configuration. */
		if (usb_get_configuration(dev, 0) < 0) {
			goto out_err;
		}
	}

	/* Ok, now we can make USB driver specific stuff. */
	if (usb_driver_probe(dev) < 0) {
		goto out_err;
	}

	return dev;

out_err:
	dlist_del(&dev->dev_link);
	pool_free(&usb_devs, dev);
	return NULL;
}

struct usb_dev *usb_dev_iterate(struct usb_dev *dev) {
	struct dlist_head *dev_link;

	dev_link = dev ? dev->dev_link.next : usb_devs_list.next;
	if (dev_link == &usb_devs_list) {
		return NULL;
	}
	return member_cast_out(dev_link, struct usb_dev, dev_link);
}

static int usb_hub_get_descriptor(struct usb_dev *dev,
		struct usb_desc_hub *desc) {
	int ret;

	ret = usb_endp_control_wait(dev->endpoints[0],
		USB_DIR_IN | USB_RT_HUB,
		USB_REQ_GET_DESCRIPTOR, USB_DT_HUB << 8,
		0, 7, desc, USB_CTRL_GET_TIMEOUT);
	if (ret) {
		log_error("%s: failed with %d", ret);
		return ret;
	}
	return 0;
}

static int usb_hub_port_get_status(struct usb_hub *hub,
		unsigned int port, uint16_t *status, uint16_t *change) {
	int ret;
	uint16_t portstatus[2];

	ret = usb_endp_control_wait(hub->dev->endpoints[0],
		USB_DIR_IN | USB_RT_PORT, USB_REQ_GET_STATUS,
		HUB_PORT_STATUS,
		port + 1, 4, &portstatus, USB_HUB_PORT_STS_TIMEOUT);
	if (ret) {
		log_error("%s: failed with %d", ret);
		return ret;
	}

	*status = portstatus[0];
	*change = portstatus[1];

	return 0;
}

static int usb_hub_clear_port_feature(struct usb_hub *hub,
		unsigned int port, int feature) {
	return usb_endp_control_wait(hub->dev->endpoints[0],
		USB_RT_PORT, USB_REQ_CLEAR_FEATURE, feature,
		port + 1, 0, NULL, 1000);
}

static int usb_hub_set_port_feature(struct usb_hub *hub,
		unsigned int port, int feature) {
	return usb_endp_control_wait(hub->dev->endpoints[0],
		USB_RT_PORT, USB_REQ_SET_FEATURE, feature,
		port + 1, 0, NULL, 1000);
}

static int usb_hub_port_set_power(struct usb_hub *hub, unsigned int port,
		bool on) {
	int ret;

	if (on) {
		ret = usb_hub_set_port_feature(hub, port, USB_PORT_FEATURE_POWER);
	} else {
		ret = usb_hub_clear_port_feature(hub, port, USB_PORT_FEATURE_POWER);
	}
	if (ret < 0) {
		return ret;
	}
	usleep(100 * 1000); /* 100 ms */
	return 0;
}

static int usb_hub_port_reset(struct usb_hub *hub, unsigned int port) {
	int ret;
	int timeout = 100;

	ret = usb_hub_set_port_feature(hub, port, USB_PORT_FEATURE_RESET);
	if (ret) {
		return -1;
	}

	/* Wait until reset done. */
	ret = -1;
	while (timeout--) {
		uint16_t port_status, port_change;

		usb_hub_port_get_status(hub, port, &port_status, &port_change);
		if (port_change & USB_PORT_STAT_RESET) {
			ret = 0;
			break;
		}
		usleep(1000); /* 1 ms */
	}

	return ret;
}

/* This function initializes device at a given port. */
static int usb_hub_port_device_init(struct usb_hub *hub, struct usb_dev *dev) {
	int ret;
	uint32_t addr;

	ret = usb_endp_control_wait(dev->endpoints[0],
		USB_DIR_IN | USB_REQ_TYPE_STANDARD | USB_REQ_RECIP_DEVICE,
		USB_REQ_GET_DESCRIPTOR,
		USB_DESC_TYPE_DEV << 8,
		0, sizeof(struct usb_desc_device), &dev->dev_desc, 1000);
	if (ret < 0) {
		log_error("%s: GET_DESC failed\n", __func__);
		return ret;
	}

	addr = index_alloc(&dev->hcd->enumerator, INDEX_MIN);
	assert(addr != INDEX_NONE);
	assert(addr < USB_HC_MAX_DEV);

	/* Set device address */
	ret = usb_endp_control_wait(dev->endpoints[0],
		USB_DIR_OUT | USB_REQ_TYPE_STANDARD | USB_REQ_RECIP_DEVICE,
		USB_REQ_SET_ADDRESS, addr,
		0, 0, NULL, 1000);
	if (ret < 0) {
		log_error("%s: SET_ADDR failed\n", __func__);
		return ret;
	}
	dev->addr = addr;

	return 0;
}

static int usb_hub_port_init(struct usb_hub *hub, struct usb_dev *dev,
		unsigned int port) {
	int ret;

	ret = usb_hub_port_set_power(hub, port, true);
	if (ret < 0) {
		log_error("usb_hub_port_set_power failed\n");
		return ret;
	}
	ret = usb_hub_port_reset(hub, port);
	if (ret < 0) {
		log_error("usb_hub_port_reset failed\n");
		return ret;
	}

	/* Now we are ready to enumerate this device */
	ret = usb_hub_port_device_init(hub, dev);
	if (ret < 0) {
		log_error("usb_hub_port_device_init failed\n");
		return ret;
	}

	return 0;
}

static void usb_hub_port_connect(struct usb_hub *hub, unsigned int port,
		uint16_t port_status, uint16_t port_change) {
	usb_new_device(hub->dev, hub->dev->hcd, port);
}

static void usb_hub_port_event(struct usb_hub *hub, unsigned int port) {
	uint16_t port_status, port_change;

	if (usb_hub_port_get_status(hub, port, &port_status, &port_change) < 0) {
		return;
	}

	if (port_change & USB_PORT_STAT_CONNECTION) {
		usb_hub_clear_port_feature(hub, port, USB_PORT_FEATURE_C_CONNECTION);
		usb_hub_port_connect(hub, port, port_status, port_change);
	}
}

static void usb_hub_event(struct usb_hub *hub) {
	int i;

	mutex_lock(&hub->mutex);
	/* Check each port for event occured.  */
	for (i = 0; i < hub->port_n; i++) {
		usb_hub_port_event(hub, i);
	}
	mutex_unlock(&hub->mutex);
}

static void usb_hubs_enumerate_if_needed(void) {
	struct usb_hub *hub;

	/* Check each hub for event occured. */
	dlist_foreach_entry(hub, &usb_hubs_list, lnk) {
		usb_hub_event(hub);
	}
}

static void *usb_hub_event_hnd(void *arg) {
	while (1) {
		usb_hubs_enumerate_if_needed();
		usleep(USB_HUBS_HANDLE_INTERVAL);
	}

	return NULL;
}

static int usb_hub_probe(struct usb_dev *dev) {
	struct usb_hub *hub;
	struct usb_desc_hub hub_desc;

	hub = pool_alloc(&usb_hubs);
	if (!hub) {
		log_error("%s: pool_alloc failed\n", __func__);
		return -1;
	}
	hub->dev = dev;
	dev->driver_data = hub;

	if (usb_hub_get_descriptor(dev, &hub_desc) < 0) {
		pool_free(&usb_hubs, hub);
		log_error("%s: usb_hub_get_descriptor failed\n", __func__);
		return -1;
	}
	hub->port_n = hub_desc.b_nbr_ports;

	dlist_head_init(&hub->lnk);
	dlist_add_next(&hub->lnk, &usb_hubs_list);

	mutex_init(&hub->mutex);

	/* Try to handle attached devices immediately, without
	 * waiting for usb_hub_event_hnd thread. */
	usb_hub_event(hub);

	return 0;
}

static struct usb_device_id usb_hub_id_table[] = {
	{USB_CLASS_HUB, 0xffff, 0xffff},
	{ },
};

struct usb_driver usb_driver_hub = {
	.name = "hub",
	.probe = usb_hub_probe,
	.id_table = usb_hub_id_table,
};

static int usb_hub_driver_init(void) {
	usb_hubs_thread = thread_create(0, usb_hub_event_hnd, NULL);
	if (err(usb_hubs_thread)) {
		return err(usb_hubs_thread);
	}

	return usb_driver_register(&usb_driver_hub);
}

/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    09.12.2019
 */

#include <util/log.h>

#include <unistd.h>

#include <mem/misc/pool.h>
#include <mem/sysmalloc.h>

#include <util/dlist.h>
#include <util/err.h>

#include <kernel/thread.h>
#include <kernel/thread/thread_sched_wait.h>

#include <drivers/usb/usb_driver.h>
#include <drivers/usb/usb.h>

#include <embox/unit.h>

#define USB_HUBS_HANDLE_INTERVAL (1000 * 1000) /* 1 sec */
#define USB_HUB_PORT_STS_TIMEOUT 1000
#define USB_CTRL_GET_TIMEOUT     1000

#define USE_THREAD    OPTION_GET(BOOLEAN, use_thread)

EMBOX_UNIT_INIT(usb_hub_driver_init);

POOL_DEF(usb_hubs, struct usb_hub, USB_MAX_HUB);
POOL_DEF(usb_devs, struct usb_dev, USB_MAX_DEV);

static DLIST_DEFINE(usb_hubs_list);
static DLIST_DEFINE(usb_devs_list);
#if USE_THREAD
static struct thread *usb_hubs_thread;
static volatile int port_status_changed = 0;
#endif

static int usb_hub_port_init(struct usb_hub *hub, struct usb_dev *dev,
		unsigned int port_nr);

static struct usb_hub *usb_dev_to_hub(struct usb_dev *dev) {
	return (struct usb_hub *)dev->usb_dev_configs[0].usb_iface[0]->driver_data;
}

extern int usb_create_root_interface(struct usb_dev *dev);

/* TODO May be to use usb bus instead of hcd and
 * get hcd from bus? */
struct usb_dev *usb_new_device(struct usb_dev *parent,
		struct usb_hcd *hcd, unsigned int port) {
	struct usb_dev *dev;
	struct usb_hub *hub;

	dev = pool_alloc(&usb_devs);
	if (!dev) {
		log_error("pool_alloc failed");
		return NULL;
	}
	memset(dev, 0, sizeof(struct usb_dev));
	dlist_head_init(&dev->dev_link);
	dlist_add_next(&dev->dev_link, &usb_devs_list);

	dev->hcd = hcd;
	dev->parent = parent;

	/* Fill control endpoint. */
	if (usb_get_ep0(dev) < 0) {
		log_error("usb_get_ep0 failed");
		goto out_err;
	}

	if (parent) { /* It's not a Root Hub */
		int cfg;

		hub = usb_dev_to_hub(parent);
		assert(hub);
		/* Here we reset device and set address. */
		if (usb_hub_port_init(hub, dev, port) < 0) {
			log_error("usb_hub_port_init failed");
			goto out_err;
		}

		dev->current_config = &dev->usb_dev_configs[0];

		for (cfg = 0; cfg < dev->dev_desc.b_num_configurations; cfg++) {
			int len;

			if (USB_DEV_MAX_CONFIG <= cfg) {
				/* error max_conf is not enough */
				break;
			}
			/* Fill device configuration. */
			len = usb_get_config_desc(dev, cfg);
			if (len < 0){
				log_error("usb_get_config_desc failed");
				goto out_err;
			}
			dev->usb_dev_configs[cfg].config_buf = sysmalloc(len);
			if (!dev->usb_dev_configs[cfg].config_buf) {
				log_error("couldn't allocate config descriptor");
				goto out_err;
			}
			/* Fill device configuration. */
			if (usb_get_configuration(dev, cfg, len) < 0) {
				log_error("usb_get_configuration failed");
				goto out_err;
			}
		}

		/* Set device default configuration. */
		/* http://www.usbmadesimple.co.uk/ums_4.htm */
		/* Usually ( when there is one configuration) the Set Configuration
		 * request will have wValue set to 1, which will select the first configuration.
		 * Set Configuration can also be used, with wValue set to 0, to deconfigure the device.
		 */
		if (usb_set_configuration(dev, cfg - 1) < 0) {
			log_error("usb_set_configuration failed");
			goto out_err;
		}
		dev->current_config = &dev->usb_dev_configs[cfg - 1];
	} else {
		dev->current_config = &dev->usb_dev_configs[0];
		usb_create_root_interface(dev);
	}

	/* Ok, now we can make USB driver specific stuff. */
	if (usb_driver_probe(dev->current_config->usb_iface[0]) < 0) {
		log_error("Usb driver not found for device ID %04x:%04x",
			dev->dev_desc.id_vendor,
			dev->dev_desc.id_product);
		/* Return device even the driver was not found. */
	}

	log_info("port(%d) bus(%d) addr(%d)", port, dev->bus_idx, dev->addr);

	return dev;

out_err:
	log_error("failed");
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

	ret = usb_endp_control_wait(&dev->endp0,
		USB_DIR_IN | USB_RT_HUB,
		USB_REQ_GET_DESCRIPTOR, USB_DT_HUB << 8,
		0, 7, desc, USB_CTRL_GET_TIMEOUT);
	if (ret) {
		log_error("failed");
		return ret;
	}
	return 0;
}

static int usb_hub_port_get_status(struct usb_hub *hub,
		unsigned int port, uint16_t *status, uint16_t *change) {
	int ret;
	uint16_t portstatus[2];

	ret = usb_endp_control_wait(&hub->dev->endp0,
		USB_DIR_IN | USB_RT_PORT, USB_REQ_GET_STATUS,
		HUB_PORT_STATUS,
		port + 1, 4, portstatus, USB_HUB_PORT_STS_TIMEOUT);
	if (ret) {
		log_error("failed");
		return ret;
	}

	*status = portstatus[0];
	*change = portstatus[1];

	log_debug("port=%d, status=0x%04x, change=0x%04x", port, *status, *change);

	return 0;
}

static int usb_hub_clear_port_feature(struct usb_hub *hub,
		unsigned int port, int feature) {
	return usb_endp_control_wait(&hub->dev->endp0,
		USB_RT_PORT, USB_REQ_CLEAR_FEATURE, feature,
		port + 1, 0, NULL, 1000);
}

static int usb_hub_set_port_feature(struct usb_hub *hub,
		unsigned int port, int feature) {
	return usb_endp_control_wait(&hub->dev->endp0,
		USB_RT_PORT, USB_REQ_SET_FEATURE, feature,
		port + 1, 0, NULL, 1000);
}

static int usb_hub_port_reset(struct usb_hub *hub, unsigned int port) {
	int ret;
	int timeout = 100;

	log_debug("%d:%d port[%d]", hub->dev->bus_idx, hub->dev->addr, port);

	ret = usb_hub_set_port_feature(hub, port, USB_PORT_FEATURE_RESET);
	if (ret) {
		return -1;
	}

	/* Wait until reset done. */
	ret = -1;
	while (timeout--) {
		uint16_t port_status = 0, port_change = 0;

		usleep(1000 * 1000);

		usb_hub_port_get_status(hub, port, &port_status, &port_change);
		if (!(port_status & USB_PORT_STAT_RESET) &&
				(port_status & USB_PORT_STAT_CONNECTION)) {
			usb_hub_clear_port_feature(hub, port, USB_PORT_FEATURE_C_RESET);
			ret = 0;
			break;
		}
	}

	log_debug("%s", (!ret ? "OK" : "ERROR"));

	return ret;
}

/* This function initializes device at a given port. */
static int usb_device_init(struct usb_hub *hub, struct usb_dev *dev) {
	int ret;
	uint32_t addr;

	ret = usb_endp_control_wait(&dev->endp0,
		USB_DIR_IN | USB_REQ_TYPE_STANDARD | USB_REQ_RECIP_DEVICE,
		USB_REQ_GET_DESCRIPTOR,
		USB_DESC_TYPE_DEV << 8,
		0, sizeof(struct usb_desc_device), &dev->dev_desc, 1000);
	if (ret < 0) {
		log_error("GET_DESC failed\n");
		return ret;
	}
	log_info("Device %d:%d config:"
			"\n\t\t len=%d type=%d bcd=0x%x class=%d subclass=%d vid=0x%04x pid=0x%04x",
			dev->bus_idx, dev->addr,
			dev->dev_desc.b_length, dev->dev_desc.b_desc_type,
			dev->dev_desc.bcd_usb, dev->dev_desc.b_dev_class,
			dev->dev_desc.b_dev_subclass, dev->dev_desc.id_vendor,
			dev->dev_desc.id_product);

	addr = index_alloc(&dev->hcd->enumerator, INDEX_MIN);
	assert(addr != INDEX_NONE);
	assert(addr < USB_HC_MAX_DEV);

	/* Set device address */
	ret = usb_endp_control_wait(&dev->endp0,
		USB_DIR_OUT | USB_REQ_TYPE_STANDARD | USB_REQ_RECIP_DEVICE,
		USB_REQ_SET_ADDRESS, addr,
		0, 0, NULL, 1000);
	if (ret < 0) {
		log_error("SET_ADDR (addr=%d) failed\n", addr);
		return ret;
	}
	log_debug("SET_ADDR (addr=%d) OK", addr);
	dev->addr = addr;

	usleep(1000 * 1000);

	return 0;
}

static int usb_hub_port_init(struct usb_hub *hub, struct usb_dev *dev,
		unsigned int port) {
	int ret;

	log_debug("hub(%d:%d) port[%d]", hub->dev->bus_idx, hub->dev->addr, port);

	ret = usb_hub_port_reset(hub, port);
	if (ret < 0) {
		log_error("usb_hub_port_reset failed\n");
		return ret;
	}

	/* Now we are ready to enumerate this device */
	ret = usb_device_init(hub, dev);
	if (ret < 0) {
		log_error("usb_device_init failed ret %d", ret);
		return ret;
	}

	return 0;
}

static int usb_hub_port_event(struct usb_hub *hub, unsigned int port) {
	uint16_t port_status = 0, port_change = 0;

	log_debug("hub(%d:%d) port[%d]", hub->dev->bus_idx, hub->dev->addr, port);

	if (usb_hub_port_get_status(hub, port, &port_status, &port_change) < 0) {
		return 0;
	}

	if (port_change & USB_PORT_STAT_CONNECTION) {
		usb_hub_clear_port_feature(hub, port, USB_PORT_FEATURE_C_CONNECTION);
		usb_new_device(hub->dev, hub->dev->hcd, port);
		return 1;
	}
	return 0;
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

#if USE_THREAD
static void usb_hubs_enumerate_if_needed(void) {
	struct usb_hub *hub;

	/* Check each hub for event occured. */
	dlist_foreach_entry(hub, &usb_hubs_list, lnk) {
		usb_hub_event(hub);
	}
}


static inline void *usb_hub_event_hnd(void *arg) {
	while (1) {
		SCHED_WAIT_TIMEOUT(port_status_changed, 10000);
		port_status_changed = 0;
		usb_hubs_enumerate_if_needed();
		//usleep(USB_HUBS_HANDLE_INTERVAL);
	}

	return NULL;
}

void usb_hubs_notify(void) {
	port_status_changed = 1;
	sched_wakeup(&usb_hubs_thread->schedee);
}

#else
void usb_hubs_notify(void) {
}
#endif

static int usb_hub_get_status(struct usb_hub *hub,
		uint16_t *status, uint16_t *change) {
	int ret;
	uint16_t hubstatus[2];

	ret = usb_endp_control_wait(&hub->dev->endp0,
		USB_DIR_IN | USB_RT_HUB, USB_REQ_GET_STATUS,
		0, 0, 4, hubstatus, USB_HUB_PORT_STS_TIMEOUT);
	if (ret) {
		log_error("failed with %d", ret);
		return ret;
	}

	*status = hubstatus[0];
	*change = hubstatus[1];

	return 0;
}

static int usb_hub_probe(struct usb_interface *iface) {
	struct usb_hub *hub;
	struct usb_desc_hub hub_desc;
	int i;
	int ret;
	struct usb_dev *dev;

	hub = pool_alloc(&usb_hubs);
	if (!hub) {
		log_error("pool_alloc failed");
		return -1;
	}

	dev = iface->usb_dev;

	hub->dev = dev;
	iface->driver_data = hub;

	if (usb_hub_get_descriptor(dev, &hub_desc) < 0) {
		pool_free(&usb_hubs, hub);
		log_error("usb_hub_get_descriptor failed");
		return -1;
	}
	hub->port_n = hub_desc.b_nbr_ports;

	log_info("Hub with %d ports:"
			"\n\t\t protocol = 0x%x"
			"\n\t\t characteristics = 0x%04x"
			"\n\t\t b_pwr_on_2_pwr_good = %d"
			"\n\t\t b_hub_contr_current = %d",
			hub->port_n, dev->dev_desc.b_dev_protocol,
			hub_desc.w_hub_characteristics,
			hub_desc.b_pwr_on_2_pwr_good, hub_desc.b_hub_contr_current);

	if (!is_root_hub(dev)) {
		uint16_t status = 0, hubstatus = 0, hubchange = 0;

		ret = usb_endp_control_wait(&dev->endp0,
			USB_DIR_IN | USB_REQ_TYPE_STANDARD | USB_REQ_RECIP_DEVICE,
			USB_REQ_GET_STATUS,
			USB_DESC_TYPE_DEV << 8,
			0, 2, &status, 1000);
		if (ret < 0) {
			log_error("Cannot get device status!");
			return -1;
		}
		log_debug("  device status = 0x%04x", status);

		ret = usb_hub_get_status(hub, &hubstatus, &hubchange);
		if (ret < 0) {
			log_error("Cannot get hub status!");
			return -1;
		}
		log_debug("  hubstatus=0x%04x, hubchange=0x%04x", hubstatus, hubchange);
	}

	for (i = 0; i < hub->port_n; i++) {
		usb_hub_set_port_feature(hub, i, USB_PORT_FEATURE_POWER);
	}
	usleep(100 * 1000);

	for (i = 0; i < hub->port_n; i++) {
		uint16_t port_status, port_change;

		if (usb_hub_port_get_status(hub, i, &port_status, &port_change) < 0) {
			log_error("FAILED status");
			return -1;
		}
	}

	dlist_head_init(&hub->lnk);
	dlist_add_next(&hub->lnk, &usb_hubs_list);

	mutex_init(&hub->mutex);
#if USE_THREAD
	thread_launch(usb_hubs_thread);
#else
	/* Try to handle attached devices immediately, without
	 * waiting for usb_hub_event_hnd thread. */
	usb_hub_event(hub);
#endif

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
#if USE_THREAD
	usb_hubs_thread = thread_create(THREAD_FLAG_SUSPENDED, usb_hub_event_hnd, NULL);
	if (err(usb_hubs_thread)) {
		return err(usb_hubs_thread);
	}
#endif

	return usb_driver_register(&usb_driver_hub);
}

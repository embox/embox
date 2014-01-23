/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    23.01.2014
 */

#include <drivers/usb/usb.h>

static void usb_port_st_reset_awaiting(struct usb_hub_port *port);
static void usb_port_st_reset_settle(struct usb_hub_port *port);
static void usb_port_st_connected(struct usb_hub_port *port);

static void usb_port_set_state(struct usb_hub_port *port, usb_hub_port_state_t state) {
	port->state = state;
}

static inline void usb_port_notify(struct usb_hub_port *port) {

	assert(port->state);
	port->state(port);
}

static void usb_dev_posted_handle(struct sys_timer *timer, void *param) {
	struct usb_hub_port *port = param;

	usb_port_notify(port);
}

static int usb_port_post(struct usb_hub_port *port, unsigned int ms) {

	return timer_init_msec(&port->post_timer, TIMER_ONESHOT, ms, usb_dev_posted_handle,
			port);
}

static void __attribute__((used)) usb_port_post_cancel(struct usb_hub_port *port) {

	timer_close(&port->post_timer);
}

static int usb_hcd_port_do_reset(struct usb_hcd *hcd) {
	struct usb_queue_link *ul;
	struct usb_hub_port *port;

	ul = usb_queue_peek(&hcd->reset_queue);
	if (!ul) {
		return 0;
	}

	port = member_cast_out(ul, struct usb_hub_port, reset_link);

	usb_hub_ctrl(port, USB_HUB_REQ_PORT_SET, USB_HUB_PORT_RESET);

	usb_port_post(port, USB_RESET_HIGH_DELAY_MS);

	return 0;
}

static int usb_hcd_port_reset(struct usb_hub_port *port) {
	struct usb_hcd *hcd = port->hub->hcd;
	bool is_resseting;

	is_resseting = usb_queue_add(&hcd->reset_queue, &port->reset_link);
	if (!is_resseting) {
		usb_hcd_port_do_reset(hcd);
	}

	return 0;
}

static void usb_hcd_port_reset_done(struct usb_hub_port *port) {
	struct usb_hcd *hcd = port->hub->hcd;

	usb_queue_done(&hcd->reset_queue, &port->reset_link);

	usb_hcd_port_do_reset(hcd);
}

static void usb_port_st_connected(struct usb_hub_port *port) {

	/*usb_dev_set_state(dev, USB_DEV_POWERED);*/
	usb_port_set_state(port, usb_port_st_reset_awaiting);
	usb_hcd_port_reset(port);
}

static void usb_port_st_reset_awaiting(struct usb_hub_port *port) {

	usb_hub_ctrl(port, USB_HUB_REQ_PORT_CLEAR, USB_HUB_PORT_RESET);

	/*usb_dev_set_state(dev, USB_DEV_DEFAULT);*/

	usb_port_set_state(port, usb_port_st_reset_settle);
	usb_port_post(port, 10);
}

static void usb_port_st_reset_settle(struct usb_hub_port *port) {

	usb_port_reset_done(port);
}

int usb_port_reset_lock(struct usb_hub_port *port, bool warm_reset) {

	usb_port_set_state(port, usb_port_st_connected);

	if (warm_reset) {
		usb_port_notify(port);
	} else {
		usb_port_post(port, 100);
	}

	return 0;
}

int usb_port_reset_unlock(struct usb_hub_port *port) {
	usb_hcd_port_reset_done(port);
	return 0;
}

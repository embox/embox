/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    23.01.2014
 */

#include <drivers/usb/usb.h>

static void usb_port_set_state(struct usb_hub_port *port, usb_hub_port_state_t state);

static void usb_port_st_reset_awaiting(struct usb_hub_port *port);
static void usb_port_st_reset_settle(struct usb_hub_port *port);
static void usb_port_st_connected(struct usb_hub_port *port);
static void usb_port_st_idle(struct usb_hub_port *port);

void usb_hub_port_init(struct usb_hub_port *port, struct usb_hub *hub,
	       	usb_hub_port_t i) {

	port->hub = hub;
	port->idx = i;
	port->status = port->changed = 0;
	port->dev = NULL;

	usb_queue_link_init(&port->reset_link);

	usb_port_set_state(port, usb_port_st_idle);
}

static void usb_port_set_state(struct usb_hub_port *port, usb_hub_port_state_t state) {
	port->state = state;
}

void usb_port_notify(struct usb_hub_port *port) {

	assert(port->state);
	port->state(port);
	port->changed = 0;
}

static void usb_dev_posted_handle(struct sys_timer *timer, void *param) {
	struct usb_hub_port *port = param;

	port->changed |= USB_HUB_PORT_TIMEOUT;
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

	if (usb_queue_remove(&hcd->reset_queue, &port->reset_link)) {
		usb_hcd_port_do_reset(hcd);
	}
}

static bool usb_port_if_disconnect(struct usb_hub_port *port) {
	bool disconn = !(port->status & USB_HUB_PORT_CONNECT);

	if (disconn) {
		usb_port_set_state(port, usb_port_st_idle);
		usb_hub_ctrl(port, USB_HUB_REQ_PORT_CLEAR,
			USB_HUB_PORT_POWER | USB_HUB_PORT_ENABLE);
	}

	return disconn;
}

static void usb_port_st_idle(struct usb_hub_port *port) {

	if ((port->changed & USB_HUB_PORT_CONNECT) &&
		(port->status & USB_HUB_PORT_CONNECT)) {

			usb_hub_ctrl(port, USB_HUB_REQ_PORT_SET,
				USB_HUB_PORT_POWER | USB_HUB_PORT_ENABLE);

			usb_port_set_state(port, usb_port_st_connected);
			usb_port_post(port, 100);
	}
}

static void usb_port_st_connected(struct usb_hub_port *port) {

	if (usb_port_if_disconnect(port)) {
		return;
	}

	if (port->changed & USB_HUB_PORT_TIMEOUT) {
		usb_port_set_state(port, usb_port_st_reset_awaiting);
		usb_hcd_port_reset(port);
	}
}

static void usb_port_st_reset_awaiting(struct usb_hub_port *port) {
	bool timeout = port->changed & USB_HUB_PORT_TIMEOUT;

	usb_hub_ctrl(port, USB_HUB_REQ_PORT_CLEAR, USB_HUB_PORT_RESET);

	if (usb_port_if_disconnect(port)) {
		usb_hcd_port_reset_done(port);
		return;
	}

	if (timeout) {

		usb_port_set_state(port, usb_port_st_reset_settle);
		usb_port_post(port, 10);
	}
}

static void usb_port_st_reset_settle(struct usb_hub_port *port) {

	if (usb_port_if_disconnect(port)) {
		usb_dev_disconnect(port);
		return;
	}

	if (port->changed & USB_HUB_PORT_TIMEOUT) {
		usb_port_reset_done(port);
	}
}

int usb_port_reset_unlock(struct usb_hub_port *port) {
	usb_hcd_port_reset_done(port);
	return 0;
}

static void usb_port_st_addr_settle(struct usb_hub_port *port) {

	if (usb_port_if_disconnect(port)) {
		usb_dev_disconnect(port);
		return;
	}

	if (port->changed & USB_HUB_PORT_TIMEOUT) {
		usb_dev_addr_settled(port->dev);
	}
}

int usb_port_address_setle_wait(struct usb_hub_port *port, int ms) {
	usb_port_set_state(port, usb_port_st_addr_settle);
	usb_port_post(port, ms);
	return 0;
}


/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.10.2013
 */
#include <errno.h>
#include <string.h>
#include <util/member.h>
#include <mem/misc/pool.h>
#include <drivers/usb/usb_hid.h>
#include <drivers/usb/usb.h>
#include <drivers/input/input_dev.h>

#define USB_HID_MAX_INPUT_LEN    8

struct usb_hid_indev {
	struct input_dev input_dev;
	struct usb_dev *usb_dev;
	char input_data[USB_HID_MAX_INPUT_LEN];
	bool running;
};

POOL_DEF(usb_hid_indevs, struct usb_hid_indev, USB_HID_MAX_DEVS);

static void usb_hid_notify(struct usb_request *req, void *arg);

static int usb_hid_event_get(struct input_dev *dev, struct input_event *ev) {
	struct usb_hid_indev *hindev;

	hindev = member_cast_out(dev, struct usb_hid_indev, input_dev);

	ev->type = hindev->input_data[0];

#if 0
	printk("\nusb_hid: received =");
	for (int i = 0; i < hindev->usb_dev->endpoints[1]->max_packet_size; i++) {
		printk(" %.2x", hindev->input_data[i]);
	}
	printk("\n");
#endif

	ev->value = ((int16_t) hindev->input_data[1]) << 16
		| (0xffff & ((int16_t) hindev->input_data[2]));

	return 0;
}

static void usb_hid_intr_request(struct usb_hid_indev *hindev) {
	struct usb_endp *intr_endp;

	intr_endp = hindev->usb_dev->endpoints[1];
	assert(intr_endp->max_packet_size <= USB_HID_MAX_INPUT_LEN);

	usb_endp_interrupt(intr_endp, usb_hid_notify, hindev->input_data,
			intr_endp->max_packet_size);
}

static void usb_hid_notify(struct usb_request *req, void *arg) {
	struct usb_dev *dev = req->endp->dev;
	struct usb_class_hid *hid = usb2hiddata(dev);
	struct usb_hid_indev *hindev;

	hindev = member_cast_out(hid->indev, struct usb_hid_indev, input_dev);

	if (!hindev->running) {
		return;
	}

	input_dev_input(&hindev->input_dev);

	usb_hid_intr_request(hindev);
}

static int usb_hid_start(struct input_dev *indev) {
	struct usb_hid_indev *hindev;

	hindev = member_cast_out(indev, struct usb_hid_indev, input_dev);

	hindev->running = 1;

	usb_hid_intr_request(hindev);

	return 0;
}

static int usb_hid_stop(struct input_dev *indev) {
	struct usb_hid_indev *hindev;

	hindev = member_cast_out(indev, struct usb_hid_indev, input_dev);

	hindev->running = 0;

	return 0;
}

static const struct input_dev_ops usb_hid_input_ops = {
	.start = usb_hid_start,
	.stop = usb_hid_stop,
	.event_get = usb_hid_event_get,
};

static void usb_hid_indev_init(struct input_dev *indev) {

	indev->name = "usb_hid";
	indev->type = INPUT_DEV_MOUSE;
	indev->ops = &usb_hid_input_ops;
	indev->irq = 0;
}

int usb_hid_found(struct usb_dev *dev) {
	struct usb_class_hid *hid = usb2hiddata(dev);
	struct usb_hid_indev *hindev;

	usb_dev_use_inc(dev);

	hindev = pool_alloc(&usb_hid_indevs);
	if (!hindev) {
		return -ENOMEM;
	}

	hindev->usb_dev = dev;

	hid->indev = &hindev->input_dev;

	usb_hid_indev_init(&hindev->input_dev);

	return input_dev_register(&hindev->input_dev);
}

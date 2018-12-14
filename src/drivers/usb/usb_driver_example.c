/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    13.11.2013
 */

#include <errno.h>
#include <util/err.h>

#include <fs/file_operation.h>
#include <drivers/usb/usb_driver.h>
#include <drivers/usb/usb_dev_desc.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(usb_example_init);

static int usb_example_probe(struct usb_driver *drv, struct usb_dev *dev,
		void **data) {

	return 0;
}

static void usb_example_disconnect(struct usb_dev *dev, void *data) {

}

static struct idesc * example_open(struct node *node, struct file_desc *file_desc, int flags) {
	struct usb_dev_desc *ddesc;
	int res;

	res = usb_driver_open_by_node(node, &ddesc);
	if (0 != res) {
		return err_ptr(-res);
	}

	assert(ddesc);

	file_desc->file_info = ddesc;

	return &file_desc->idesc;
}

static int example_close(struct file_desc *desc) {

	usb_dev_desc_close(desc->file_info);

	return 0;
}

static size_t example_read(struct file_desc *desc, void *buf, size_t size) {
	struct usb_dev_desc *ddesc = desc->file_info;
	size_t actual_size;
	int ret;

	if (size > 4) {
		actual_size = 4;
	} else {
		actual_size = size;
	}

	ret = usb_request(ddesc, 1, USB_TOKEN_IN, buf, actual_size);
	if (ret < 0) {
		return ret;
	}

	return actual_size;
}

static void example_write_hnd(struct usb_request *req, void *arg) {

}

static size_t example_write(struct file_desc *desc, void *buf, size_t size) {
	struct usb_dev_desc *ddesc = desc->file_info;
	int ret;

	ret = usb_request_cb(ddesc, 2, USB_TOKEN_OUT, buf, size,
			example_write_hnd, NULL);
	if (ret < 0) {
		return ret;
	}

	return size;
}
static struct file_operations example_file_ops = {
	.open = example_open,
	.close = example_close,
	.read = example_read,
	.write = example_write,
};

static struct usb_device_id example_id_table[] = {
	{ 0x0627, 0x0001},
	{ },
};

struct usb_driver usb_driver_example = {
	.probe = usb_example_probe,
	.disconnect = usb_example_disconnect,
	.file_ops = &example_file_ops,
	.id_table = example_id_table,
};

static int usb_example_init(void) {

	return usb_driver_register(&usb_driver_example);
}

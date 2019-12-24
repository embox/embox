/**
 * @file
 *
 * @brief Show list of USB devices
 *
 * @date 14.05.2015
 * @author Anton Bondarev
 */
#include <stdio.h>
#include <unistd.h>

#include <drivers/usb/usb.h>

static void print_usage(void) {
	printf("Usage: lsusb [-h]\n");
	printf("\t[-h]      - print this help\n");
}

static void print_error(void) {
	printf("Wrong parameters\n");
	print_usage();
}

static void show_usb_dev(struct usb_dev *usb_dev) {
	printf("Bus %03d Device %03d ID %04x:%04x\n",
			usb_dev->bus_idx,
			usb_dev->addr,
			usb_dev->dev_desc.id_vendor,
			usb_dev->dev_desc.id_product);

}

int main(int argc, char **argv) {
	struct usb_dev *usb_dev = NULL;
	int opt;

	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch (opt) {
		case '?':
		case 'h':
			print_usage();
			return 0;
		default:
			print_error();
			return 0;
		}
	}

	while ((usb_dev = usb_dev_iterate(usb_dev))) {
		show_usb_dev(usb_dev);
	}

	return 0;
}

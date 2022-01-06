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
	printf("Usage: lsusb [-h] [-v]\n");
	printf("\t[-h]      - print this help\n");
	printf("\t[-v]      - prints verbose output with device, configuration" 
		   " and interface descriptors\n");

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

static void show_usb_desc_device(struct usb_dev *usb_dev) {
	printf(" Device Descriptor:\n"
			"   b_length             %5u\n"
			"   b_desc_type          %5u\n"
			"   bcd_usb              %2x.%02x\n"
			"   b_dev_class          %5u\n"
			"   b_dev_subclass       %5u\n"
			"   b_dev_protocol       %5u\n"
			"   b_max_packet_size    %5u\n"
			"   id_vendor           0x%04x\n"
			"   id_product          0x%04x \n"
			"   bcd_device           %2x.%02x\n"
			"   i_manufacter         %5u\n"
			"   i_product            %5u\n"
			"   i_serial_number      %5u\n"
			"   b_num_configurations %5u\n\n",
			usb_dev->dev_desc.b_length, 
			usb_dev->dev_desc.b_desc_type,
			usb_dev->dev_desc.bcd_usb >> 8, usb_dev->dev_desc.bcd_usb & 0xff,
			usb_dev->dev_desc.b_dev_class,
			usb_dev->dev_desc.b_dev_subclass,
			usb_dev->dev_desc.b_dev_protocol, 
			usb_dev->dev_desc.b_max_packet_size,
			usb_dev->dev_desc.id_vendor, 
			usb_dev->dev_desc.id_product,
			usb_dev->dev_desc.bcd_device >> 8, usb_dev->dev_desc.bcd_device & 0xff,
			usb_dev->dev_desc.i_manufacter,
			usb_dev->dev_desc.i_product,
			usb_dev->dev_desc.i_serial_number,
			usb_dev->dev_desc.b_num_configurations);
}

static void show_usb_desc_interface(struct usb_dev *usb_dev) {
	if (!usb_dev->usb_iface[0]->iface_desc[0]) {
		printf(" Interface Descriptor:\n"
			   "   No interfaces\n"
		);
		return;
	}

	printf(" Interface Descriptor:\n"
			"   b_length             %5u\n"
			"   b_desc_type          %5u\n"
			"   b_interface_number   %5u\n"
			"   b_alternate_setting  %5u\n"
			"   b_num_endpoints      %5u\n"
			"   b_interface_class    %5u\n"
			"   b_interface_subclass %5u\n"
			"   b_interface_protocol %5u\n"
			"   i_interface          %5u\n",
			usb_dev->usb_iface[0]->iface_desc[0]->b_length,
			usb_dev->usb_iface[0]->iface_desc[0]->b_desc_type,
			usb_dev->usb_iface[0]->iface_desc[0]->b_interface_number,
			usb_dev->usb_iface[0]->iface_desc[0]->b_alternate_setting,
			usb_dev->usb_iface[0]->iface_desc[0]->b_num_endpoints,
			usb_dev->usb_iface[0]->iface_desc[0]->b_interface_class,
			usb_dev->usb_iface[0]->iface_desc[0]->b_interface_subclass,
			usb_dev->usb_iface[0]->iface_desc[0]->b_interface_protocol,
			usb_dev->usb_iface[0]->iface_desc[0]->i_interface);
}

static void show_usb_desc_configuration(struct usb_desc_configuration *config) {
	
	if (!config) {
		printf(" Configuration Descriptor:\n"
			   "   No configurations\n\n"
		);
		return;
	}

	printf(" Configuration Descriptor:\n"
			"   b_length             %5u\n"
			"   b_desc_type          %5u\n"
			"   w_total_length      0x%04x\n"
			"   b_num_interfaces     %5u\n"
			"   bConfigurationValue  %5u\n"
			"   i_configuration      %5u\n"
			"   bm_attributes         0x%02x\n"
			"   b_max_power          %5u\n\n",
			config->b_length, 
			config->b_desc_type,
			config->w_total_length,
			config->b_num_interfaces, 
			config->b_configuration_value,
			config->i_configuration,
			config->bm_attributes,
			config->b_max_power);	
}


int main(int argc, char **argv) {
	struct usb_dev *usb_dev = NULL;
	int opt, flag = 0;

	while (-1 != (opt = getopt(argc, argv, "h:v"))) {
		switch (opt) {
		case '?':
		case 'h':
			print_usage();
			return 0;
		case 'v':
			flag = 1;
			break;
		default:
			print_error();
			return 0;
		}
	}

	while ((usb_dev = usb_dev_iterate(usb_dev))) {
		show_usb_dev(usb_dev);
		if(flag) {
			int conf_cnt = 0;
			struct usb_desc_configuration *config = usb_dev->current_config->config_buf;

			show_usb_desc_device(usb_dev);

			for (conf_cnt = 0;
					conf_cnt < usb_dev->dev_desc.b_num_configurations;
					conf_cnt ++) {

				show_usb_desc_configuration(config);
				/* TODO the only first configuration */
				if (conf_cnt == 0) {
					show_usb_desc_interface(usb_dev);
				}

				config = ((void *)config + config->w_total_length);
			}
		}
	}

	return 0;
}

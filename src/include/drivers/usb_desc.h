/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    11.11.2013
 */

#ifndef DRIVERS_USB_DESC_H_
#define DRIVERS_USB_DESC_H_

struct usb_control_header {
	uint8_t		bm_request_type;
	uint8_t 	b_request;
	uint16_t 	w_value;
	uint16_t 	w_index;
	uint16_t	w_length;
} __attribute__((packed));

struct usb_desc_device {
	uint8_t		b_lenght;
	uint8_t 	b_desc_type;
	uint16_t	bcd_usb;
	uint8_t 	b_dev_class;
	uint8_t 	b_dev_subclass;
	uint8_t 	b_dev_protocol;
	uint8_t 	b_max_packet_size;
	uint16_t	id_vendor;
	uint16_t	id_product;
	uint16_t	bcd_device;
	uint8_t 	i_manufacter;
	uint8_t 	i_product;
	uint8_t		i_serial_number;
	uint8_t		i_num_configurations;
} __attribute__((packed));

struct usb_desc_configuration {
	uint8_t		b_lenght;
	uint8_t 	b_desc_type;
	uint16_t	w_total_length;
	uint8_t 	b_num_interfaces;
	uint8_t 	b_configuration_value;
	uint8_t		i_configuration;
	uint8_t 	bm_attributes;
	uint8_t 	b_max_power;
} __attribute__((packed));

struct usb_desc_interface {
	uint8_t		b_lenght;
	uint8_t 	b_desc_type;
	uint8_t 	b_interface_number;
	uint8_t 	b_alternate_setting;
	uint8_t		b_num_endpoints;
	uint8_t		b_interface_class;
	uint8_t		b_interface_subclass;
	uint8_t		b_interface_protocol;
	uint8_t 	i_interface;
}__attribute__((packed));

struct usb_desc_endpoint {
	uint8_t		b_lenght;
	uint8_t 	b_desc_type;
	uint8_t 	b_endpoint_address;
	uint8_t 	bm_attributes;
	uint16_t	w_max_packet_size;
	uint8_t		b_interval;
}__attribute__((packed));

#endif /* DRIVERS_USB_DESC_H_ */


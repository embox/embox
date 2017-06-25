/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    11.11.2013
 */

#ifndef DRIVERS_USB_DESC_H_
#define DRIVERS_USB_DESC_H_

#define USB_DESC_TYPE_DEV               1
#define USB_DESC_TYPE_CONFIG            2
#define USB_DESC_TYPE_STRING            3
#define USB_DESC_TYPE_ENDPOINT          5

#define USB_DESC_ENDP_TYPE_MASK         0x03
#define USB_DESC_ENDP_TYPE_CTRL         0x00
#define USB_DESC_ENDP_TYPE_ISOCHR       0x01
#define USB_DESC_ENDP_TYPE_BULK         0x02
#define USB_DESC_ENDP_TYPE_INTR         0x03

#define USB_DESC_ENDP_ADDR_MASK         0x0f
#define USB_DESC_ENDP_ADDR_DIR_MASK     0x80
#define USB_DESC_ENDP_ADDR_OUT          0x00
#define USB_DESC_ENDP_ADDR_IN           0x80

struct usb_control_header {
	uint8_t		bm_request_type;
	uint8_t 	b_request;
	uint16_t 	w_value;
	uint16_t 	w_index;
	uint16_t	w_length;
} __attribute__((packed));

struct usb_desc_device {
	uint8_t		b_length;
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
	uint8_t		b_length;
	uint8_t 	b_desc_type;
	uint16_t	w_total_length;
	uint8_t 	b_num_interfaces;
	uint8_t 	b_configuration_value;
	uint8_t		i_configuration;
	uint8_t 	bm_attributes;
	uint8_t 	b_max_power;
} __attribute__((packed));

struct usb_desc_interface {
	uint8_t		b_length;
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
	uint8_t		b_length;
	uint8_t 	b_desc_type;
	uint8_t 	b_endpoint_address;
	uint8_t 	bm_attributes;
	uint16_t	w_max_packet_size;
	uint8_t		b_interval;
}__attribute__((packed));

struct usb_desc_hub {
    uint8_t  b_desc_length;
    uint8_t  b_desc_type;
    uint8_t  b_nbr_ports;
    uint16_t w_hub_characteristics;
    uint8_t  b_pwr_on_2_pwr_good;
    uint8_t  b_hub_contr_current;
    uint8_t  var_data[]; /* Variable length field; 64 should be the maximum possible
                      length (255 ports => 2 x 32 bytes of data) */
}__attribute__((packed));

#endif /* DRIVERS_USB_DESC_H_ */


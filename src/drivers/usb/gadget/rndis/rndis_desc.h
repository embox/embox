/**
 * @file
 *
 * @date May 12, 2020
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_USB_GADGET_RNDIS_RNDIS_DESC_H_
#define SRC_DRIVERS_USB_GADGET_RNDIS_RNDIS_DESC_H_

#include <stdint.h>

#include <drivers/usb/usb_desc.h>

#define RNDIS_MANUFACTURER_STR_INDEX  1
#define RNDIS_PRODUCT_STR_INDEX       2
#define RNDIS_SERIAL_STR_INDEX        3
#define RNDIS_CONFIGURATION_STR_INDEX 4
#define RNDIS_MS_OS_STR_INDEX         5

/* M$ OS string */
extern const unsigned char OS_String[18];

 /* MS OS Feature descriptor */
extern const unsigned char OS_feature_dsc[];

extern const struct usb_desc_device rndis_device_desc;

extern const struct usb_desc_configuration rndis_config_desc;

/* IAD descriptor */
extern const uint8_t iad_descriptor[8];

/* 4 desciptors here */
extern const uint8_t interface0_functional_desc[4][5];

/* Interface 0 descriptor */
extern const struct usb_desc_interface rndis_interface0_desc;
/* Interface 1 descriptor */
extern const struct usb_desc_interface rndis_interface1_desc;

#endif /* SRC_DRIVERS_USB_GADGET_RNDIS_RNDIS_DESC_H_ */

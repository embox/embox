/**
 * @file
 *
 * @date May 12, 2020
 * @author Anton Bondarev
 */

#include <stdint.h>

#include <drivers/usb/usb_desc.h>

#include <drivers/usb/gadget/rndis_desc.h>

#define RNDIS_NUM_OF_INTERFACES 2

#define RNDIS_VID 0xDEAD
#define RNDIS_PID 0xBEAF

/* Enable or disable handling of MS OS descriptors and string */
#define USBD_MS_OS_DSC_SUPPORT 1
#define USBD_MS_OS_VENDOR_CODE 0xBC /* bMS_VendorCode, must match the vendor code returned in OS string descriptor */

#define USB_BM_ATTR_SELF_POWER     (1 << 6)

/* M$ OS string */
const unsigned char OS_String[18] =
	{ 18, 3, 'M', 0, 'S', 0, 'F', 0,'T', 0, '1', 0, '0', 0, '0', 0, USBD_MS_OS_VENDOR_CODE , 0x00 };

/* MS OS Feature descriptor */
const unsigned char OS_feature_dsc[] =
{
    0x28, 0x00, 0x00, 0x00, /* dwLength */
    0x00, 0x01, /* bcdVersion */
    0x04, 0x00, /* wIndex */
    0x01, /* bCount */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* reserve 7 bytes */
    0x00, /* bInterfaceNumber */
    0x01, /* reserved */
    /* The following is compatible ID 8 bytes (RNDIS) */
    0x52, 0x4E, 0x44, 0x49, 0x53, 0x00, 0x00, 0x00,
    /* The following is SubCompatible ID null 8 bytes (5162001) */
    0x35, 0x31, 0x36, 0x32, 0x30, 0x30, 0x31, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00 /* reserve 6 bytes */
};

const struct usb_desc_device rndis_device_desc = {
        sizeof(struct usb_desc_device),
		USB_DESC_TYPE_DEV,
	    0x0200,                                         /* uint16 bcdUSB; */
	    0xE0,                                           /* uint8  bDeviceClass = Wireless Controller; */
	    0x00,                                           /* uint8  bDeviceSubClass; */
	    0x00,                                           /* uint8  bDeviceProtocol; */
	    0x08,                                           /* uint8  bMaxPacketSize0; */
		RNDIS_VID,                                  /* uint16 idVendor; */
	    RNDIS_PID,                                  /* uint16 idProduct; */
	    0xFFFF,                                         /* uint16 bcdDevice; */
		RNDIS_MANUFACTURER_STR_INDEX,                         /* uint8  iManufacturer; */
		RNDIS_PRODUCT_STR_INDEX,                              /* uint8  iProduct; */
		RNDIS_SERIAL_STR_INDEX,                               /* uint8  iSerialNumber; */
	    0x01                                            /* uint8  bNumConfigurations; */
};

const struct usb_desc_configuration rndis_config_desc = {
	    sizeof(struct  usb_desc_configuration),
		USB_DESC_TYPE_CONFIG,

	    0x0BAD,                                         /* uint16 wTotalLength; */
	    RNDIS_NUM_OF_INTERFACES,                        /* uint8  bNumInterfaces; */
	    0x01,                                           /* uint8  bConfigurationValue; */
		RNDIS_CONFIGURATION_STR_INDEX,                        /* uint8  iConfiguration; */
		USB_BM_ATTR_SELF_POWER,                         /* uint8  bmAttributes; */
	    0x30                                            /* uint8  bMaxPower = 96 mA; */
};

/* IAD descriptor */
const uint8_t iad_descriptor[8] = {
		0x08, /*    bLength */
		0x0B, /*    bDescriptorType */
		0x00, /*    bFirstInterface */
		0x02, /*    bInterfaceCount */
		0xE0, /*    bFunctionClass   (Wireless Controller) */
		0x01, /*    bFunctionSubClass */
		0x03, /*    bFunctionProtocol */
		RNDIS_CONFIGURATION_STR_INDEX /*    iFunction   "RNDIS" */
};

/* Interface 0 descriptor */
const struct usb_desc_interface rndis_interface0_desc = {
    sizeof(struct usb_desc_interface),
	USB_DESC_TYPE_INTERFACE,

    0x00,                                           // uint8  bInterfaceNumber;
    0x00,                                           // uint8  bAlternateSetting;
    1,                                              // uint8  bNumEndpoints;
    0xE0,                                  			// uint8  bInterfaceClass: Wireless Controller;
    0x01,                                           // uint8  bInterfaceSubClass
    0x03,                                           // uint8  bInterfaceProtocol
    0      											// uint8  iInterface;
};

const uint8_t interface0_functional_desc[4][5] = {
		/* Header Functional Descriptor */
		[0] = { 0x05,                              // bFunctionLength
		        0x24,                              // bDescriptorType = CS Interface
		        0x00,                              // bDescriptorSubtype
		        0x10,                              // bcdCDC = 1.10
		        0x01 },                             // bcdCDC = 1.10

		/* Call Management Functional Descriptor */
		[1] = { 0x05,                              // bFunctionLength
		        0x24,                              // bDescriptorType = CS Interface
		        0x01,                              // bDescriptorSubtype = Call Management
		        0x00,                              // bmCapabilities
		        0x01 },                             // bDataInterface

		/* Abstract Control Management Functional Descriptor */
		[2] = { 0x04,                              // bFunctionLength
		        0x24,                              // bDescriptorType = CS Interface
		        0x02,                              // bDescriptorSubtype = Abstract Control Management
		    //  0x08,                              // bmCapabilities = Device supports the notification Network_Connection
		        0x00 },                             // bmCapabilities = Device supports the notification Network_Connection

		/* Union Functional Descriptor */
		[3] = { 0x05,                              // bFunctionLength
		        0x24,                              // bDescriptorType = CS Interface
		        0x06,                              // bDescriptorSubtype = Union
		        0x00,                              // bControlInterface = "RNDIS Communications Control"
		        0x01 },                             // bSubordinateInterface0 = "RNDIS Ethernet Data"
};

const struct usb_desc_interface rndis_interface1_desc = {
	    sizeof(struct usb_desc_interface),
		USB_DESC_TYPE_INTERFACE,

		0x01,                                           // uint8  bInterfaceNumber;
		0x00,                                           // uint8  bAlternateSetting;
		2,                                              // uint8  bNumEndpoints;
		0x0A,                                           // uint8  bInterfaceClass: CDC;
		0x00,                                           // uint8  bInterfaceSubClass;
		0x00,                                           // uint8  bInterfaceProtocol;
		0   											// uint8  iInterface;
};


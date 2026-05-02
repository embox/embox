/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    21.04.2026
 */

#ifndef USB_CLASS_USB_CDC_H_
#define USB_CLASS_USB_CDC_H_

#include <stdint.h>

/** Communications Class Subclass Codes */
#define USB_CDC_SUBCLASS_ACM          0x02
#define USB_CDC_SUBCLASS_ETHERNET     0x06
#define USB_CDC_SUBCLASS_EEM          0x0c
#define USB_CDC_SUBCLASS_NCM          0x0d

/** Communications Class Protocol Codes */
#define USB_CDC_PROTOCOL_ATV250		0x01
#define USB_CDC_PROTOCOL_EEM        0x07
#define USB_CDC_PROTOCOL_ACM_VENDOR 0xFF
#define USB_CDC_PROTOCOL_NCM_DATA   0x01

/**
 * @brief Data Class Interface Codes
 * @note CDC120-20101103-track.pdf, 4.5, Table 6
 */
#define DATA_INTERFACE_CLASS       0x0A

/**
 * @brief bDescriptor SubType for Communications
 * Class Functional Descriptors
 * @note CDC120-20101103-track.pdf, 5.2.3, Table 13
 */
#define USB_CDC_HEADER_TYPE         0x00
#define USB_CDC_CALL_TYPE           0x01
#define USB_CDC_ACM_TYPE            0x02
#define USB_CDC_UNION_TYPE          0x06
#define USB_CDC_ETHERNET_TYPE       0x0f


/**
 * @brief PSTN Subclass Specific Requests
 * for ACM devices
 * @note PSTN120.pdf, 6.3, Table 13
 */
#define CDC_SEND_ENC_CMD            0x00
#define CDC_GET_ENC_RSP             0x01
#define SET_LINE_CODING	            0x20
#define GET_LINE_CODING	            0x21
#define SET_CONTROL_LINE_STATE      0x22

/** Data structure for GET_LINE_CODING / SET_LINE_CODING class requests */
struct cdc_acm_line_coding {
	uint32_t dwDTERate;
	uint8_t bCharFormat;
	uint8_t bParityType;
	uint8_t bDataBits;
} __attribute__((packed));

/** Header Functional Descriptor */
struct cdc_header_descriptor {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint16_t bcdCDC;
} __attribute__((packed));

/** Union Interface Functional Descriptor */
struct cdc_union_descriptor {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bControlInterface;
	uint8_t bSubordinateInterface0;
} __attribute__((packed));

/** Call Management Functional Descriptor */
struct cdc_cm_descriptor {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bmCapabilities;
	uint8_t bDataInterface;
} __attribute__((packed));

/** Abstract Control Management Functional Descriptor */
struct cdc_acm_descriptor {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t bmCapabilities;
} __attribute__((packed));

/** Data structure for the notification about SerialState */
struct cdc_acm_notification {
	uint8_t bmRequestType;
	uint8_t bNotificationType;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
	uint16_t data;
} __attribute__((packed));

/** Ethernet Networking Functional Descriptor */
struct cdc_ecm_descriptor {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint8_t iMACAddress;
	uint32_t bmEthernetStatistics;
	uint16_t wMaxSegmentSize;
	uint16_t wNumberMCFilters;
	uint8_t bNumberPowerFilters;
} __attribute__((packed));

/** Ethernet Network Control Model (NCM) Descriptor */
struct cdc_ncm_descriptor {
	uint8_t bFunctionLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubtype;
	uint16_t bcdNcmVersion;
	uint8_t bmNetworkCapabilities;
} __attribute__((packed));

#endif /* USB_CLASS_USB_CDC_H_ */

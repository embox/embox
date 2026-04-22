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


#endif /* USB_CLASS_USB_CDC_H_ */

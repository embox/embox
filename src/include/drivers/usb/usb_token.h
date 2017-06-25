/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    11.11.2013
 */

#ifndef DRIVERS_USB_TOKEN_H_
#define DRIVERS_USB_TOKEN_H_

typedef unsigned short usb_token_t;

#define USB_TOKEN_SETUP                 0x0001
#define USB_TOKEN_IN                    0x0002
#define USB_TOKEN_OUT                   0x0004
#define USB_TOKEN_STATUS                0x0008

#endif /* DRIVERS_USB_TOKEN_H_ */


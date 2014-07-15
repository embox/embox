/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    31.01.2014
 */

#ifndef USB_WHITELIST_DEV_H_
#define USB_WHITELIST_DEV_H_

#include <sys/ioctl.h>
#include <limits.h>

#define USB_WHITELIST_PID_ANY USHRT_MAX
#define USB_WHITELIST_SN_ANY  "*"

#define USB_WHITELIST_SN_LEN 64
struct usb_whitelist_rule {
	unsigned int id;
	unsigned short vid;
	unsigned short pid;
	char sn[USB_WHITELIST_SN_LEN];
};

#define USB_WHITELIST_IO_ADD   _IOW('u', 0, struct usb_whitelist_rule)
#define USB_WHITELIST_IO_DEL   _IOW('u', 1, int)
#define USB_WHITELIST_IO_FLUSH  _IO('u', 2)
#define USB_WHITELIST_IO_GETN  _IOR('u', 3, int)

#define USB_WHITELIST_DEV_NAME "usb_whitelist"

#endif /* USB_WHITELIST_DEV_H_ */



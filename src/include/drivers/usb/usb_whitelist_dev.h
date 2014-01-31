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
#define USB_WHITELIST_SN_ANY  UCHAR_MAX

struct usb_whitelist_rule {
	unsigned short vid;
	unsigned short pid;
	unsigned char  sn;
};

#define USB_WHITELIST_IO_ADD   _IOW('u', 0, struct usb_whitelist_rule)
#define USB_WHITELIST_IO_FLUSH  _IO('u', 1)
#define USB_WHITELIST_IO_GETN  _IOR('u', 2, int)

#define USB_WHITELIST_DEV_NAME "usb_whitelist"

#endif /* USB_WHITELIST_DEV_H_ */



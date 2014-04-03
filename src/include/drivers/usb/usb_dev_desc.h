/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    11.11.2013
 */

#ifndef DRIVERS_USB_DEV_DESC_H_
#define DRIVERS_USB_DEV_DESC_H_

#include <stdint.h>
#include <stdbool.h>
#include <drivers/usb/usb_desc.h>
#include <drivers/usb/usb_token.h>

struct usb_dev_desc;
struct usb_request;
typedef void (*usb_notify_hnd_t)(struct usb_request *req, void *arg);

/**
 * @brief Open usb device
 *
 * @param vid Vendor id of device
 * @param pid Product id of device
 *
 * @return Pointer usb_dev_desc on success
 * @return NULL on error
 */
extern struct usb_dev_desc *usb_dev_open(uint16_t vid, uint16_t pid);

/**
 * @brief Close usb device
 *
 * @param ddesc
 */
extern void usb_dev_desc_close(struct usb_dev_desc *ddesc);

/**
 * @brief Get usb device descriptor and interface descriptor
 *
 * @param ddesc USB device descriptor
 * @param desc Pointer to descriptor of usb device to fill
 * @param idesc Pointer to descriptor of usb device's interface to fill
 *
 * @return 0 on success
 * @return Negative on error
 */
extern int usb_dev_desc_get_desc(struct usb_dev_desc *ddesc, struct usb_desc_device *desc,
		struct usb_desc_interface *idesc);

/**
 * @brief Get endpoint descriptor
 *
 * @param ddesc Device's descriptor
 * @param endp Endpoint number
 * @param desc Pointer to descriptor of endpoint to fill
 *
 * @return 0 on success
 * @return Negative on error
 */
extern int usb_dev_desc_get_endp_desc(struct usb_dev_desc *ddesc, int endp,
		struct usb_desc_endpoint *desc);

/**
 * @brief Make request to endpoint and wait till complete.
 *
 * @param ddesc Device descriptor
 * @param endp_n Endpoint number
 * @param token Token mask
 * @param buf Pointer to buffer
 * @param len Length of buffer
 *
 * @return 0 on success
 * @return Negative on error
 */
extern int usb_request(struct usb_dev_desc *ddesc, int endp_n, usb_token_t token,
		void *buf, size_t len);

/**
 * @brief Make request to endpoint and call callback when it's done.
 *
 * @param ddesc Device descriptor
 * @param endp_n Endpoint number
 * @param token Token mask
 * @param buf Pointer to buffer
 * @param len Length of buffer
 * @param notify_hnd Callback to call
 *
 * @return 0 on success
 * @return Negative on error
 */
extern int usb_request_cb(struct usb_dev_desc *ddesc, int endp_n, usb_token_t token,
		void *buf, size_t len, usb_notify_hnd_t notify_hnd, void *arg);

#endif /* DRIVERS_USB_DEV_DESC_H_ */


/**
 * @file
 * @brief Registers definition
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 26.06.2019
 */

#ifndef IMX_USB_REGS_H_
#define IMX_USB_REGS_H_

#define  USB_UOG_ID          (0x000)
# define USB_UOG_ID_REV_OFFT 16
# define USB_UOG_ID_REV_MASK 0xff
# define USB_UOG_ID_NID_OFFT 8
# define USB_UOG_ID_NID_MASK 0x3f
# define USB_UOG_ID_ID_OFFT  0
# define USB_UOG_ID_ID_MASK  0x3f

#define USB_UOG_HWGENERAL    (0x004)
#define USB_UOG_HWHOST       (0x008)
#define USB_UOG_HWDEVICE     (0x00C)
#define USB_UOG_HWTXBUF      (0x010)
#define USB_UOG_HWRXBUF      (0x014)

#define USB_UOG_USBMODE      (IMX_USB_CORE_BASE + 0x1A8)

#endif /* IMX_USB_REGS_H_ */

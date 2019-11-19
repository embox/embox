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

/* PHY-related stuff */
/* There are two ports in i.MX6 processorts */
#define USBPHY_BASE(port) (0x20C9000 + 0x1000 * port)

#define USBPHY_PWD(port)           (USBPHY_BASE(port) + 0x00)
#define USBPHY_PWD_SET(port)       (USBPHY_BASE(port) + 0x04)
#define USBPHY_PWD_CLR(port)       (USBPHY_BASE(port) + 0x08)
#define USBPHY_PWD_TOG(port)       (USBPHY_BASE(port) + 0x0C)
#define USBPHY_TX(port)            (USBPHY_BASE(port) + 0x10)
#define USBPHY_TX_SET(port)        (USBPHY_BASE(port) + 0x14)
#define USBPHY_TX_CLR(port)        (USBPHY_BASE(port) + 0x18)
#define USBPHY_TX_TOG(port)        (USBPHY_BASE(port) + 0x1C)
#define USBPHY_RX(port)            (USBPHY_BASE(port) + 0x20)
#define USBPHY_RX_SET(port)        (USBPHY_BASE(port) + 0x24)
#define USBPHY_RX_CLR(port)        (USBPHY_BASE(port) + 0x28)
#define USBPHY_RX_TOG(port)        (USBPHY_BASE(port) + 0x2C)
#define USBPHY_CTRL(port)          (USBPHY_BASE(port) + 0x30)
#define USBPHY_CTRL_SET(port)      (USBPHY_BASE(port) + 0x34)
#define USBPHY_CTRL_CLR(port)      (USBPHY_BASE(port) + 0x38)
#define USBPHY_CTRL_TOG(port)      (USBPHY_BASE(port) + 0x3C)
# define USBPHY_CTRL_SFTRST        (1 << 31)
# define USBPHY_CTRL_CLKGATE       (1 << 30)
# define USBPHY_CTRL_ENUTMILEVEL3  (1 << 15)
# define USBPHY_CTRL_ENUTMILEVEL2  (1 << 14)
#define USBPHY_STATUS(port)        (USBPHY_BASE(port) + 0x40)
#define USBPHY_DEBUG(port)         (USBPHY_BASE(port) + 0x50)
#define USBPHY_DEBUG_SET(port)     (USBPHY_BASE(port) + 0x54)
#define USBPHY_DEBUG_CLR(port)     (USBPHY_BASE(port) + 0x58)
#define USBPHY_DEBUG_TOG(port)     (USBPHY_BASE(port) + 0x5C)
#define USBPHY_DEBUG0_STATUS(port) (USBPHY_BASE(port) + 0x60)
#define USBPHY_DEBUG1(port)        (USBPHY_BASE(port) + 0x70)
#define USBPHY_DEBUG1_SET(port)    (USBPHY_BASE(port) + 0x74)
#define USBPHY_DEBUG1_CLR(port)    (USBPHY_BASE(port) + 0x78)
#define USBPHY_DEBUG1_TOG(port)    (USBPHY_BASE(port) + 0x7C)
#define USBPHY_VERSION(port)       (USBPHY_BASE(port) + 0x80)

#define USB_ANALOG_BASE(port) (0x20C81A0 + 0x80 * port)

#define USB_ANALOG_VBUS_DETECT(port)      (USB_ANALOG_BASE(port) + 0x00)
#define USB_ANALOG_VBUS_DETECT_SET(port)  (USB_ANALOG_BASE(port) + 0x04)
#define USB_ANALOG_VBUS_DETECT_CLR(port)  (USB_ANALOG_BASE(port) + 0x08)
#define USB_ANALOG_VBUS_DETECT_TOG(port)  (USB_ANALOG_BASE(port) + 0x0C)
#define USB_ANALOG_CHRG_DETECT(port)      (USB_ANALOG_BASE(port) + 0x10)
#define USB_ANALOG_CHRG_DETECT_SET(port)  (USB_ANALOG_BASE(port) + 0x14)
#define USB_ANALOG_CHRG_DETECT_CLR(port)  (USB_ANALOG_BASE(port) + 0x18)
#define USB_ANALOG_CHRG_DETECT_TOG(port)  (USB_ANALOG_BASE(port) + 0x1C)
# define USB_ANALOG_EN_B                  (1 << 20)
# define USB_ANALOG_CHK_CHRG_B            (1 << 19)
# define USB_ANALOG_CHK_CONTACT           (1 << 18)
#define USB_ANALOG_VBUS_DETECT_STAT(port) (USB_ANALOG_BASE(port) + 0x20)
#define USB_ANALOG_CHRG_DETECT_STAT(port) (USB_ANALOG_BASE(port) + 0x30)
#define USB_ANALOG_MISC_DETECT(port)      (USB_ANALOG_BASE(port) + 0x40)
#define USB_ANALOG_MISC_DETECT_SET(port)  (USB_ANALOG_BASE(port) + 0x44)
#define USB_ANALOG_MISC_DETECT_CLR(port)  (USB_ANALOG_BASE(port) + 0x48)
#define USB_ANALOG_MISC_DETECT_TOG(port)  (USB_ANALOG_BASE(port) + 0x4C)
#define USB_ANALOG_DIGPROG                (USB_ANALOG_BASE(0)    + 0xA0)

#define USBPHY_PWD_SET(port)       (USBPHY_BASE(port) + 0x04)
#define USBPHY_PWD_CLR(port)       (USBPHY_BASE(port) + 0x08)
#define USBPHY_PWD_TOG(port)       (USBPHY_BASE(port) + 0x0C)
#define USBPHY_TX(port)            (USBPHY_BASE(port) + 0x10)
#define USBPHY_TX_SET(port)        (USBPHY_BASE(port) + 0x14)
#define USBPHY_TX_CLR(port)        (USBPHY_BASE(port) + 0x18)
#define USBPHY_TX_TOG(port)        (USBPHY_BASE(port) + 0x1C)
#define USBPHY_RX(port)            (USBPHY_BASE(port) + 0x20)
#define USBPHY_RX_SET(port)        (USBPHY_BASE(port) + 0x24)
#define USBPHY_RX_CLR(port)        (USBPHY_BASE(port) + 0x28)
#define USBPHY_RX_TOG(port)        (USBPHY_BASE(port) + 0x2C)
#define USBPHY_CTRL(port)          (USBPHY_BASE(port) + 0x30)
#define USBPHY_CTRL_SET(port)      (USBPHY_BASE(port) + 0x34)
#define USBPHY_CTRL_CLR(port)      (USBPHY_BASE(port) + 0x38)
#define USBPHY_CTRL_TOG(port)      (USBPHY_BASE(port) + 0x3C)
# define USBPHY_CTRL_CLKGATE       (1 << 30)
#define USBPHY_STATUS(port)        (USBPHY_BASE(port) + 0x40)
#define USBPHY_DEBUG(port)         (USBPHY_BASE(port) + 0x50)
#define USBPHY_DEBUG_SET(port)     (USBPHY_BASE(port) + 0x54)
#define USBPHY_DEBUG_CLR(port)     (USBPHY_BASE(port) + 0x58)
#define USBPHY_DEBUG_TOG(port)     (USBPHY_BASE(port) + 0x5C)
#define USBPHY_DEBUG0_STATUS(port) (USBPHY_BASE(port) + 0x60)
#define USBPHY_DEBUG1(port)        (USBPHY_BASE(port) + 0x70)
#define USBPHY_DEBUG1_SET(port)    (USBPHY_BASE(port) + 0x74)
#define USBPHY_DEBUG1_CLR(port)    (USBPHY_BASE(port) + 0x78)
#define USBPHY_DEBUG1_TOG(port)    (USBPHY_BASE(port) + 0x7C)
#define USBPHY_VERSION(port)       (USBPHY_BASE(port) + 0x80)

#endif /* IMX_USB_REGS_H_ */

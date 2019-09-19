/**
 * @file
 * @brief Registers definition
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 26.06.2019
 */

#ifndef IMX_USB_REGS_H_
#define IMX_USB_REGS_H_

#define IMX_USB_CORE_BASE 0x02184000

#define USB_UOG_ID           (IMX_USB_CORE_BASE + 0x000)
# define USB_UOG_ID_REV_OFFT 16
# define USB_UOG_ID_REV_MASK 0xff
# define USB_UOG_ID_NID_OFFT 8
# define USB_UOG_ID_NID_MASK 0x3f
# define USB_UOG_ID_ID_OFFT  0
# define USB_UOG_ID_ID_MASK  0x3f

#define USB_UOG_HWGENERAL    (IMX_USB_CORE_BASE + 0x004)
#define USB_UOG_HWHOST       (IMX_USB_CORE_BASE + 0x008)
#define USB_UOG_HWDEVICE     (IMX_USB_CORE_BASE + 0x00C)
#define USB_UOG_HWTXBUF      (IMX_USB_CORE_BASE + 0x010)
#define USB_UOG_HWRXBUF      (IMX_USB_CORE_BASE + 0x014)

#define USB_UOG_USBCMD       (IMX_USB_CORE_BASE + 0x140)
# define USB_USBCMD_RS       (1 << 0)
# define USB_USBCMD_RST      (1 << 1)
# define USB_USBCMD_PSE      (1 << 4)
# define USB_USBCMD_ASE      (1 << 5)
# define USB_USBCMD_IAA      (1 << 6)
# define USB_USBCMD_ASPE     (1 << 11)
# define USB_USBCMD_ATDTW    (1 << 12)
# define USB_USBCMD_SUTW     (1 << 13)

#define USB_USBCMD_FS_MASK   ((3 << 2) | (1 << 15))

#define USB_USBCMD_ITC(x)    ((x) << 16)
#define USB_USBCMD_ITC_MASK  (0xFF << 16)


#define USB_UOG_USBSTS       (IMX_USB_CORE_BASE + 0x144)
#define USB_UOG_USBINTR      (IMX_USB_CORE_BASE + 0x148)

#define USB_UOG_USBMODE      (IMX_USB_CORE_BASE + 0x1A8)

#define QTD_TERMINATE     0x1
#define QTD_POINTER_MASK ~0x1F
#define QTD_TOKEN_TOOGLE (1 << 31)
#define QTD_TOKEN_BYTES_OFFT   16
#define QTD_TOKEN_BYTES_MASK   0x7FFF
#define QTD_TOKEN_IOC          (1 << 15)
#define QTD_TOKEN_CPAGE_OFFT   12
#define QTD_TOKEN_CPAGE_MASK   0x1F

#define QTD_TOKEN_PID_OFFT   8
#define QTD_TOKEN_PID_MASK   0x3
#define QTD_TOKEN_STATUS_MASK   0xFF

#define QTD_BUFFER_CURRENT_OFFT(x) (x &  0xFFF)
#define QTD_BUFFER_POINTER(x)      (x & ~0xFFF)
/* Data structures as decribed in iMX6DQPRM.pdf */
struct qtd {
	uint32_t next;
	uint32_t altnext;
	uint32_t token;
	uint32_t buffer[5];
};

struct queue_head {
	uint32_t link;
	uint32_t endpt1;
	uint32_t endpt2;
	uint32_t curtd;
	struct qtd qtd;
	uint32_t buffer;
};


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

/**
 * @file
 *
 * @date Aug 12, 2019
 * @author  Anton Bondarev
 */

#ifndef SRC_DRIVERS_USB_HC_EHCI_REGS_H_
#define SRC_DRIVERS_USB_HC_EHCI_REGS_H_

#include <stdint.h>

#define EHCI_INTR_MASK (EHCI_STS_IAA | EHCI_STS_FATAL | EHCI_STS_PCD | EHCI_STS_ERR | EHCI_STS_INT)

#define EHCI_HC_LENGTH(p)        (0x00ff & ((p) >> 0))
#define EHCI_HC_VERSION(ehci, p) (0xffff & ((p) >> 16))
#define EHCI_HCS_N_PORTS(p)      (((p) >> 0) & 0xf) /* bits 3:0, ports on HC */

/* HCCPARAMS */
#define EHCI_HCC_32FRAME_PERIODIC_LIST(p) ((p) & (1 << 19))
#define EHCI_HCC_PER_PORT_CHANGE_EVENT(p) ((p) & (1 << 18))
#define EHCI_HCC_LPM(p)                   ((p) & (1 << 17))
#define EHCI_HCC_HW_PREFETCH(p)           ((p) & (1 << 16))
#define EHCI_HCC_EXT_CAPS(p)              ((p) >> 8) & 0xff) /* for pci extended caps */
#define EHCI_HCC_ISOC_CACHE(p)            ((p) & (1 << 7))   /* true: can cache isoc frame */
#define EHCI_HCC_ISOC_THRES(p)            (((p) >> 4) & 0x7) /* bits 6:4, uframes cached */
#define EHCI_HCC_CANPARK(p)               ((p) & (1 << 2))   /* true: can park on async qh */
#define EHCI_HCC_PGM_FRAMELISTLEN(p)      ((p) & (1 << 1))   /* true: periodic_size changes*/
#define EHCI_HCC_64BIT_ADDR(p)            ((p) & (1))        /* true: can use 64-bit addr */

/* Section 2.2 Host Controller Capability Registers */
struct ehci_caps {
	/* 0 Capability Register Length
	 * 1 reserved
	 * 2 Interface Version Number (BCD) */
	uint32_t hc_capbase;

	uint32_t hcs_params;  /* HCSPARAMS (Structural Parameters) - offset 0x4  */
	uint32_t hcc_params;  /* HCCPARAMS (Capability Parameters) - offset 0x8 */

	uint8_t portroute[8];  /* Companion Port Route Description - offset 0xC */
};

#define EHCI_CMD_HIRD         (0xf<<24)    /* host initiated resume duration */
#define EHCI_CMD_PPCEE        (1<<15)      /* per port change event enable */
#define EHCI_CMD_FSP          (1<<14)      /* fully synchronized prefetch */
#define EHCI_CMD_ASPE         (1<<13)      /* async schedule prefetch enable */
#define EHCI_CMD_PSPE         (1<<12)      /* periodic schedule prefetch enable */
#define EHCI_CMD_PARK    (1 << 11)    /* eAsync Schedule Park Mode Enable  */
#define EHCI_CMD_LRESET  (1 << 7)     /* Light Host Controller Reset */
#define EHCI_CMD_IAAD    (1 << 6)     /* Interrupt On Async Advance Doorbell */
#define EHCI_CMD_ASE     (1 << 5)     /* async schedule enable */
#define EHCI_CMD_PSE     (1 << 4)     /* periodic schedule enable */
#define EHCI_CMD_RESET   (1 << 1)     /* reset HC not bus */
#define EHCI_CMD_RUN     (1 << 0)     /* start/stop HC */

#define EHCI_STS_ASS     (1 << 15)      /* Async Schedule Status */
#define EHCI_STS_PSS     (1 << 14)      /* Periodic Schedule Status */
#define EHCI_STS_RECL    (1 << 13)      /* Reclamation */
#define EHCI_STS_HALT    (1 << 12)      /* Halted */
#define EHCI_STS_IAA     (1 << 5)       /* Doorbell Interrupt  */
#define EHCI_STS_FATAL   (1 << 4)       /* Host System Error  */
#define EHCI_STS_FLR     (1 << 3)       /* Frame List Rollover  */
#define EHCI_STS_PCD     (1 << 2)       /* Port Change Detect  */
#define EHCI_STS_ERR     (1 << 1)       /* USB Error Interrupt */
#define EHCI_STS_INT     (1 << 0)       /* USB Transfer Interrupt */

/*  Port Status/Control Register  */
#define EHCI_PORT_WKOC_E     (1 << 22)      /* Wake On Overcurrent Enable */
#define EHCI_PORT_WKDISC_E   (1 << 21)      /* Wake On Disconnect Enable */
#define EHCI_PORT_WKCONN_E   (1 << 20)      /* Wake On Connect Enable */
#define EHCI_PORT_TEST(x)    (((x) & 0xf) << 16) /* Port Test Control */
#define EHCI_PORT_TEST_PKT   PORT_TEST(0x4) /* Port Test Control - packet test */
#define EHCI_PORT_TEST_FORCE PORT_TEST(0x5) /* Port Test Control - force enable */
#define EHCI_PORT_LED_OFF    (0 << 14)
#define EHCI_PORT_LED_AMBER  (1 << 14)
#define EHCI_PORT_LED_GREEN  (2 << 14)
#define EHCI_PORT_LED_MASK   (3 << 14)
#define EHCI_PORT_OWNER      (1 << 13)      /* Companion Port Control  */
#define EHCI_PORT_POWER      (1 << 12)      /* Port Power */
#define EHCI_PORT_USB11(x)   (((x) & (3 << 10)) == (1 << 10)) /* USB 1.1 device */
#define EHCI_PORT_RESET      (1 << 8)      /* reset port */
#define EHCI_PORT_SUSPEND    (1 << 7)      /* suspend port */
#define EHCI_PORT_RESUME     (1 << 6)      /* resume it */
#define EHCI_PORT_OCC        (1 << 5)      /* over current change */
#define EHCI_PORT_OC         (1 << 4)      /* over current active */
#define EHCI_PORT_PEC        (1 << 3)      /* port enable change */
#define EHCI_PORT_PE         (1 << 2)      /* port enable */
#define EHCI_PORT_CSC        (1 << 1)      /* connect status change */
#define EHCI_PORT_CONNECT    (1 << 0)      /* device connected */

/* CONFIGFLAG */
#define EHCI_CF_DONE                      (1 << 0)   /* true: we'll support "high speed" */

/* Section 2.3 Host Controller Operational Registers */
struct ehci_regs {
	uint32_t command;     /* USBCMD: offset 0x00 */
	uint32_t status;      /* USBSTS: offset 0x04 */
	uint32_t intr_enable; /* USBINTR: offset 0x08 */
	uint32_t frame_index; /* FRINDEX: offset 0x0C */
	uint32_t segment;     /* CTRLDSSEGMENT: offset 0x10 */
	uint32_t frame_list;  /* PERIODICLISTBASE: offset 0x14 */
	uint32_t async_next;  /* ASYNCLISTADDR: offset 0x18 */
	uint32_t reserved1[2];
	/* TXFILLTUNING: offset 0x24 */
	uint32_t txfill_tuning; /* TX FIFO Tuning register */
	uint32_t reserved2[6];
	uint32_t configured_flag; /* CONFIGFLAG: offset 0x40 */
	uint32_t port_status[0]; /* PORTSC: offset 0x44 */
};

#endif /* SRC_DRIVERS_USB_HC_EHCI_REGS_H_ */

/**
 * @file
 * @brief
 *
 * @author  Filipp Chubukov
 * @date    01.10.2020
 */


/* UHCI Controller I/O Registers */
#include <asm/io.h>

#define REG_CMD                         0x00        /* USB Command */
#define REG_STS                         0x02        /* USB Status */
#define REG_INTR                        0x04        /* USB Interrupt Enable */
#define REG_FRNUM                       0x06        /* Frame Number */
#define REG_FRBASEADD                   0x08        /* Frame List Base Address */
#define REG_SOFMOD                      0x0C        /* Start of Frame Modify */
#define REG_PORT1                       0x10        /* Port 1 Status/Control */
#define REG_PORT2                       0x12        /* Port 2 Status/Control */
#define REG_LEGSUP                      0xc0        /* Legacy Support */

/* USB Command Register */

#define CMD_RS                          (1 << 0)    /* Run/Stop */
#define CMD_HCRESET                     (1 << 1)    /* Host Controller Reset */
#define CMD_GRESET                      (1 << 2)    /* Global Reset */
#define CMD_EGSM                        (1 << 3)    /* Enter Global Suspend Resume */
#define CMD_FGR                         (1 << 4)    /* Force Global Resume */
#define CMD_SWDBG                       (1 << 5)    /* Software Debug */
#define CMD_CF                          (1 << 6)    /* Configure Flag */
#define CMD_MAXP                        (1 << 7)    /* Max Packet (0 = 32, 1 = 64) */

/* USB Status Register */

#define STS_USBINT                      (1 << 0)    /* USB Interrupt */
#define STS_ERROR                       (1 << 1)    /* USB Error Interrupt */
#define STS_RD                          (1 << 2)    /* Resume Detect */
#define STS_HSE                         (1 << 3)    /* Host System Error */
#define STS_HCPE                        (1 << 4)    /* Host Controller Process Error */
#define STS_HCH                         (1 << 5)    /* HC Halted */

/* USB Interrupt Enable Register */

#define INTR_TIMEOUT                    (1 << 0)    /* Timeout/CRC Interrupt Enable */
#define INTR_RESUME                     (1 << 1)    /* Resume Interrupt Enable */
#define INTR_IOC                        (1 << 2)    /* Interrupt on Complete Enable */
#define INTR_SP                         (1 << 3)    /* Short Packet Interrupt Enable */

/* Port Status and Control Registers */

#define PORT_CONNECTION                 (1 << 0)    /* Current Connect Status */
#define PORT_CONNECTION_CHANGE          (1 << 1)    /* Connect Status Change */
#define PORT_ENABLE                     (1 << 2)    /* Port Enabled */
#define PORT_ENABLE_CHANGE              (1 << 3)    /* Port Enable Change */
#define PORT_LS                         (3 << 4)    /* Line Status */
#define PORT_RD                         (1 << 6)    /* Resume Detect */
#define PORT_LSDA                       (1 << 8)    /* Low Speed Device Attached */
#define PORT_RESET                      (1 << 9)    /* Port Reset */
#define PORT_SUSP                       (1 << 12)   /* Suspend */
#define PORT_RWC                        (PORT_CONNECTION_CHANGE | PORT_ENABLE_CHANGE)

/* TD Link Pointer */
#define TD_PTR_TERMINATE                (1 << 0)
#define TD_PTR_QH                       (1 << 1)
#define TD_PTR_DEPTH                    (1 << 2)

/* TD Control and Status */
#define TD_CS_ACTLEN                    0x000007ff
#define TD_CS_BITSTUFF                  (1 << 17)     /* Bitstuff Error */
#define TD_CS_CRC_TIMEOUT               (1 << 18)     /* CRC/Time Out Error */
#define TD_CS_NAK                       (1 << 19)     /* NAK Received */
#define TD_CS_BABBLE                    (1 << 20)     /* Babble Detected */
#define TD_CS_DATABUFFER                (1 << 21)     /* Data Buffer Error */
#define TD_CS_STALLED                   (1 << 22)     /* Stalled */
#define TD_CS_ACTIVE                    (1 << 23)     /* Active */
#define TD_CS_IOC                       (1 << 24)     /* Interrupt on Complete */
#define TD_CS_IOS                       (1 << 25)     /* Isochronous Select */
#define TD_CS_LOW_SPEED                 (1 << 26)     /* Low Speed Device */
#define TD_CS_ERROR_MASK                (3 << 27)     /* Error counter */
#define TD_CS_ERROR_SHIFT               27
#define TD_CS_SPD                       (1 << 29)     /* Short Packet Detect */

/* TD Token */
#define TD_TOK_PID_MASK                 0x000000ff    /* Packet Identification */
#define TD_TOK_DEVADDR_MASK             0x00007f00    /* Device Address */
#define TD_TOK_DEVADDR_SHIFT            8
#define TD_TOK_ENDP_MASK                00x0078000    /* Endpoint */
#define TD_TOK_ENDP_SHIFT               15
#define TD_TOK_D                        0x00080000    /* Data Toggle */
#define TD_TOK_D_SHIFT                  19
#define TD_TOK_MAXLEN_MASK              0xffe00000    /* Maximum Length */
#define TD_TOK_MAXLEN_SHIFT             21

#define TD_PACKET_IN                    0x69
#define TD_PACKET_OUT                   0xe1
#define TD_PACKET_SETUP                 0x2d

#define UHCI_READ_16(adress) \
	in16(adress)

#define UHCI_READ_32(adress) \
	in32(adress)

#define UHCI_WRITE_16(value, adress) \
	out16(value, adress)

#define UHCI_WRITE_32(value, adress) \
	out32(value, adress)


struct uhci_td {
    volatile uint32_t link;
    volatile uint32_t cs;
    volatile uint32_t token;
    volatile uint32_t buffer;

    /* internal fields */
    uint32_t td_next;
}__attribute__((packed,aligned(16)));

/* selfmade link structure TODO rewrite with dlist from embox */
struct uhci_link {
	struct uhci_link *prev;
	struct uhci_link *next;
};

struct uhci_qh {
    volatile uint32_t head;
    volatile uint32_t element;

    /* internal fields */
    struct usb_request *req;
    struct uhci_link qh_link;
    uint32_t td_head;
}__attribute__((packed,aligned(16)));

struct uhci_controller {
  uint16_t io_addr;
  uint32_t *framelist;
  struct uhci_qh *qh_async;
}__attribute__((packed,aligned(16)));

struct uhci_hcd {
  struct usb_hcd *hcd;
	struct uhci_controller *uhc;
};

static inline struct uhci_hcd *hcd2uhci(struct usb_hcd *hcd) {
	return (struct uhci_hcd *) hcd->hci_specific;
}

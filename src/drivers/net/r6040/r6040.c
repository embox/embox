/**
 * @file
 * @brief RDC R6040 PCI Fast Ethernet MAC support
 * @description Low level ring-buffer manipulation for r6040 ethernet.
 *
 * @date 25.03.11
 * @author Nikolay Korotky
 */

#include <asm/io.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <kernel/time/ktime.h>
#include <embox/unit.h>
#include <net/l2/ethernet.h>
#include <net/skbuff.h>
#include <net/netdevice.h>
#include <drivers/ethernet/r6040.h>
#include <kernel/irq.h>
#include <net/l0/net_entry.h>

EMBOX_UNIT_INIT(r6040_init);

#define INTERRUPTS_ENABLE 0

/* PHY CHIP Address */
#define PHY1_ADDR      1       /* For MAC1 */
#define PHY2_ADDR      3       /* For MAC2 */
#define PHY_MODE       0x3100  /* PHY CHIP Register 0 */
#define PHY_CAP        0x01E1  /* PHY CHIP Register 4 */

/* RDC MAC I/O Size */
#define R6040_IO_SIZE   256

//TODO: get from PCI
#define IOADDR         0xe800

/* MAC registers */
#define MCR0           (IOADDR + 0x00) /* Control register 0 */
#define MCR0_PROMISC   0x0020          /* Promiscuous mode */
#define MCR0_HASH_EN   0x0100          /* Enable multicast hash table function */
#define MCR1           (IOADDR + 0x04) /* Control register 1 */
#define MAC_RST        0x0001          /* Reset the MAC */
#define MBCR           (IOADDR + 0x08) /* Bus control */
#define MT_ICR         (IOADDR + 0x0C) /* TX interrupt control */
#define MR_ICR         (IOADDR + 0x10) /* RX interrupt control */
#define MTPR           (IOADDR + 0x14) /* TX poll command register */
#define MR_BSR         (IOADDR + 0x18) /* RX buffer size */
#define MR_DCR         (IOADDR + 0x1A) /* RX descriptor control */
#define MLSR           (IOADDR + 0x1C) /* Last status */
#define MMDIO          (IOADDR + 0x20) /* MDIO control register */
#define MDIO_WRITE     0x4000          /* MDIO write */
#define MDIO_READ      0x2000          /* MDIO read  */
#define MMRD           (IOADDR + 0x24) /* MDIO read data register  */
#define MMWD           (IOADDR + 0x28) /* MDIO write data register */
#define TX_START_LOW   (IOADDR + 0x2c) /* TX descriptor start address 0 */
#define TX_START_HIGH  (IOADDR + 0x30) /* TX descriptor start address 1 */
#define RX_START_LOW   (IOADDR + 0x34) /* RX descriptor start address 0 */
#define RX_START_HIGH  (IOADDR + 0x38) /* RX descriptor start address 1 */
#define MISR           (IOADDR + 0x3c) /* Status register */
#define MIER           (IOADDR + 0x40) /* INT enable register */
#define MSK_INT        0x0000          /* Mask off interrupts */
#define RX_FINISH      0x0001          /* RX finished */
#define RX_NO_DESC     0x0002          /* No RX descriptor available */
#define RX_FIFO_FULL   0x0004          /* RX FIFO full */
#define RX_EARLY       0x0008          /* RX early */
#define TX_FINISH      0x0010          /* TX finished */
#define TX_EARLY       0x0080          /* TX early */
#define EVENT_OVRFL    0x0100          /* Event counter overflow */
#define LINK_CHANGED   0x0200          /* PHY link changed */
#define ME_CISR        (IOADDR + 0x44) /* Event counter INT status */
#define ME_CIER        (IOADDR + 0x48) /* Event counter INT enable */
#define MR_CNT         (IOADDR + 0x50) /* Successfully received packet counter */
#define ME_CNT0        (IOADDR + 0x52) /* Event counter 0 */
#define ME_CNT1        (IOADDR + 0x54) /* Event counter 1 */
#define ME_CNT2        (IOADDR + 0x56) /* Event counter 2 */
#define ME_CNT3        (IOADDR + 0x58) /* Event counter 3 */
#define MT_CNT         (IOADDR + 0x5A) /* Successfully transmit packet counter */
#define ME_CNT4        (IOADDR + 0x5C) /* Event counter 4 */
#define MP_CNT         (IOADDR + 0x5E) /* Pause frame counter register */
#define MAR0           (IOADDR + 0x60) /* Hash table 0 */
#define MAR1           (IOADDR + 0x62) /* Hash table 1 */
#define MAR2           (IOADDR + 0x64) /* Hash table 2 */
#define MAR3           (IOADDR + 0x66) /* Hash table 3 */

#define MAC_SM         (IOADDR + 0xAC) /* MAC status machine */

#define TX_DCNT         0x80    /* TX descriptor count */
#define RX_DCNT         0x80    /* RX descriptor count */
#define MAX_BUF_SIZE    0x600
//#define RX_DESC_SIZE    (RX_DCNT * sizeof(struct eth_desc))
//#define TX_DESC_SIZE    (TX_DCNT * sizeof(struct eth_desc))
#define MBCR_DEFAULT    0x012A  /* MAC Bus Control Register */

/* Descriptor status */
#define DSC_OWNER_MAC   0x8000  /* MAC is the owner of this descriptor */
#define DSC_RX_OK       0x4000  /* RX was successful */
#define DSC_RX_ERR      0x0800  /* RX PHY error */
#define DSC_RX_ERR_DRI  0x0400  /* RX dribble packet */
#define DSC_RX_ERR_BUF  0x0200  /* RX length exceeds buffer size */
#define DSC_RX_ERR_LONG 0x0100  /* RX length > maximum packet length */
#define DSC_RX_ERR_RUNT 0x0080  /* RX packet length < 64 byte */
#define DSC_RX_ERR_CRC  0x0040  /* RX CRC error */
#define DSC_RX_BCAST    0x0020  /* RX broadcast (no error) */
#define DSC_RX_MCAST    0x0010  /* RX multicast (no error) */
#define DSC_RX_MCH_HIT  0x0008  /* RX multicast hit in hash table (no error) */
#define DSC_RX_MIDH_HIT 0x0004  /* RX MID table hit (no error) */
#define DSC_RX_IDX_MID_MASK 3   /* RX mask for the index of matched MIDx */

/* RX and TX interrupts that we handle */
#define RX_INTS        (RX_FIFO_FULL | RX_NO_DESC | RX_FINISH)
#define TX_INTS        (TX_FINISH)
#define INT_MASK       (RX_INTS | TX_INTS)

/**
 * Descriptor definition and manip functions.
 * doubles as the TX and RX descriptor
 */
typedef struct eth_desc {
	volatile uint16_t status;
	uint16_t       dlen;
	unsigned char *buf;
	struct eth_desc *DNX;
	unsigned short HIDX;
	unsigned short Reserved1;
	unsigned short Reserved2;
	/* Extra info, used by driver */
	unsigned short dlen_orig;
} __attribute__((aligned(32))) eth_desc_t;

static uint8_t descrxbuff[0x800];
static uint8_t etherrxbuff[0x800];

/* Pass in the next pointer */
static eth_desc_t *rxd_init(size_t pkt_size) {
	eth_desc_t *rxd = (eth_desc_t *) descrxbuff;
	unsigned char *pkt = (unsigned char *) etherrxbuff;

	/* Clear it */
	memset(rxd, 0, sizeof(eth_desc_t));

	/* clear pkt area */
	memset(pkt, 0, pkt_size);

	/* Make this one owned by the MAC */
	rxd->status = DSC_OWNER_MAC;

	/* Set the buffer pointer */
	rxd->buf = pkt;
	rxd->dlen = pkt_size;

	return rxd;
}

static void r6040_tx_enable(void) {
	unsigned short tmp = in16(MCR0);
	out16(tmp | (1 << 12), MCR0);
}

static void r6040_tx_disable(void) {
	unsigned short tmp = in16(MCR0);
	out16(tmp & ~(1 << 12), MCR0);
}

void r6040_rx_enable(void) {
	unsigned short tmp = in16(MCR0);
	out16(tmp | (1 << 1), MCR0);
//	out16(2, MCR0);
}

static void r6040_rx_disable(void) {
	out8(0, MCR0);
}

static void r6040_set_tx_start(eth_desc_t* desc) {
	unsigned long tmp = (unsigned long) desc;
	out16((tmp & 0xffff), TX_START_LOW);
	tmp >>= 16;
	out16((tmp & 0xffff), TX_START_HIGH);
}

static void r6040_set_rx_start(eth_desc_t* desc) {
	unsigned long tmp = (unsigned long) desc;
	out16((tmp & 0xffff), RX_START_LOW);
	tmp >>= 16;
	out16((tmp & 0xffff), RX_START_HIGH);
}
#include <kernel/printk.h>
#if INTERRUPTS_ENABLE
/* The RDC interrupt handler */
static irq_return_t irq_handler(unsigned int irq_num, void *dev_id) {
	uint16_t misr, status;

	printk("IRQ!\n");

	/* Save MIER */
	misr = in16(MIER);
	/* Mask off RDC MAC interrupt */
	out16(MSK_INT, MIER);
	/* Read MISR status and clear */
	status = in16(MISR);

	if (status == 0x0000 || status == 0xffff) {
		/* Restore RDC MAC interrupt */
		out16(misr, MIER);
		return IRQ_NONE;
	}
	//TODO:

	/* Restore RDC MAC interrupt */
	out16(misr, MIER);
	return IRQ_HANDLED;
}
#endif
/* Interface */

/* Keep track of what we've allocated, so we can easily walk it */
eth_desc_t *g_rx_descriptor_list[R6040_RX_DESCRIPTORS];
eth_desc_t *g_rx_descriptor_next;
eth_desc_t *g_tx_descriptor_next;

#if 0
/* Disable packet reception */
void r6040_done(void) {
	out8(0, MCR0);
}
#endif

static void discard_descriptor(void) {
	/* reset the length field to original value. */
	g_rx_descriptor_next->dlen = g_rx_descriptor_next->dlen_orig;
	g_rx_descriptor_next->status = DSC_OWNER_MAC; /* give back to the MAC */
	g_rx_descriptor_next = g_rx_descriptor_next->DNX; /* Move along */
}

/* Returns size of pkt, or zero if none received */
size_t r6040_rx(unsigned char* pkt, size_t max_len) {
	size_t ret = 0;
	printk("MIER=0x%08x\n", *((unsigned int *) MIER));
	printk("MISR=0x%08x\n", *((unsigned int *) MISR));
	printk("MR_ICR=0x%08x\n", *((unsigned int *) MR_ICR));

	if (g_rx_descriptor_next->status & DSC_OWNER_MAC) {
		/* Still owned by the MAC, nothing received */
		return ret;
	}

	if (!(g_rx_descriptor_next->status & DSC_RX_OK)) {
		/* Descriptor descarded with error */
		discard_descriptor();
		return ret;
	}

	/* If the buffer isn't long enough discard this packet */
	if (g_rx_descriptor_next->dlen > max_len) {
		/* Descriptor descarded (buffer too short) */
		discard_descriptor();
		return ret;
	}

	/* Otherwise copy out and advance buffer pointer */

	memcpy(pkt, g_rx_descriptor_next->buf, g_rx_descriptor_next->dlen);
	ret = g_rx_descriptor_next->dlen;
	ret -= 4;   /* chop the checksum, we don't need it */
	discard_descriptor();
	return ret;
}

/* queue packet for transmission */
void r6040_tx(unsigned char* pkt, size_t length) {
	r6040_tx_disable();

	/* copy this packet into the transmit descriptor */
	memset(g_tx_descriptor_next->buf, 0, 60);

	memcpy(g_tx_descriptor_next->buf, pkt, length);
	g_tx_descriptor_next->dlen = (length < 60) ? 60 : length;

	/* Copy the descriptor address (will have been set to zero by last op) */
	r6040_set_tx_start(g_tx_descriptor_next);

	/* Make the mac own it */
	g_tx_descriptor_next->status = DSC_OWNER_MAC;

	//desc_dump(g_tx_descriptor_next);

	/* Start xmit */
	r6040_tx_enable();

	/* poll for mac to no longer own it */
	while (g_tx_descriptor_next->status & DSC_OWNER_MAC) {};
	/* Stop any other activity */
	r6040_tx_disable();

	//desc_dump(g_tx_descriptor_next);
}

unsigned short r6040_mdio_read(int reg, int phy) {
	out8(MDIO_READ + reg + (phy << 8), MMDIO);
	/* Wait for the read bit to be cleared */
	while (in16(MMDIO) & MDIO_READ);
	return in16(MMRD);
}

/* Wait for linkup, or timeout. */
int r6040_wait_linkup(void) {
	unsigned short tmp, i;

	for (i = 0; i < 300; i++) {
		/* Check if link up. */
		tmp = r6040_mdio_read(1, 1) & (1 << 2);
		if (tmp) {
			return 1;
		}
		/* Wait 10mS more */
		ksleep(10);
	}
	return 0;
}

int r6040_open(struct net_device *dev) {
#if INTERRUPTS_ENABLE
	if (-1 == irq_attach(0x0a, irq_handler, 0, dev, "RDC r6040")) {
		return -1;
	}
	out16(0, MT_ICR);
	out16(0, MR_ICR);
	out16(INT_MASK, MIER);
#endif
	return 0;

}
#if INTERRUPTS_ENABLE
static int r6040_stop(struct net_device *dev) {
	return 0;

}

static const struct net_driver r6040_drv_ops = {
//	.xmit = r6040_xmit,
	.start       = r6040_open,
	.stop       = r6040_stop,
//	.set_macaddr = set_mac_address
};
#endif


/* setup descriptors, start packet reception */
static int r6040_init(void) {
	size_t i;
	r6040_wait_linkup();
	r6040_rx_disable();
	r6040_tx_disable();
	for (i = 0; i < R6040_RX_DESCRIPTORS; i++) {
		/* most packets will be no larger than this */
		g_rx_descriptor_list[i] = rxd_init(1536);
		if (i) {
			g_rx_descriptor_list[i - 1]->DNX = g_rx_descriptor_list[i];
		}
	}
	/* Make ring buffer */
	g_rx_descriptor_list[R6040_RX_DESCRIPTORS - 1]->DNX = g_rx_descriptor_list[0];
	r6040_set_rx_start(g_rx_descriptor_list[0]);
	g_rx_descriptor_next = g_rx_descriptor_list[0];
	g_tx_descriptor_next = rxd_init(1536);

	r6040_rx_enable();
	return 0;
}

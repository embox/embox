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
#include <types.h>
#include <embox/unit.h>
#include <linux/init.h>
#include <net/etherdevice.h>
#include <net/skbuff.h>
#include <net/netdevice.h>

EMBOX_UNIT_INIT(r6040_init);

#define R6040_RX_DESCRIPTORS 32

#define IOADDR         0xe800

/* MAC registers */
#define MCR0           (IOADDR + 0x00) /* Control register 0 */
#define MCR1           (IOADDR + 0x04) /* Control register 1 */
#define MBCR           (IOADDR + 0x08) /* Bus control */
#define MT_ICR         (IOADDR + 0x0C) /* TX interrupt control */
#define MR_ICR         (IOADDR + 0x10) /* RX interrupt control */
#define MTPR           (IOADDR + 0x14) /* TX poll command register */
#define MR_BSR         (IOADDR + 0x18) /* RX buffer size */
#define MR_DCR         (IOADDR + 0x1A) /* RX descriptor control */
#define MLSR           (IOADDR + 0x1C) /* Last status */
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
#define TX_FINISH      0x0010          /* TX finished */

#define MMDIO          (IOADDR + 0x20) /* MDIO control register */
#define MDIO_WRITE     0x4000          /* MDIO write */
#define MDIO_READ      0x2000          /* MDIO read  */
#define MMRD           (IOADDR + 0x24) /* MDIO read data register  */
#define MMWD           (IOADDR + 0x28) /* MDIO write data register */

/* Descriptor status */
#define DSC_OWNER_MAC  0x8000          /* MAC is the owner of this descriptor */
#define DSC_RX_OK      0x4000          /* RX was successful */

/* RX and TX interrupts that we handle */
#define RX_INTS        (RX_FIFO_FULL | RX_NO_DESC | RX_FINISH)
#define TX_INTS        (TX_FINISH)
#define INT_MASK       (RX_INTS | TX_INTS)

/**
 * Descriptor definition and manip functions.
 * doubles as the TX and RX descriptor
 */
typedef struct eth_desc {
	uint16_t       status;
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
	/* allocate the descriptor memory */
	eth_desc_t *rxd = (eth_desc_t *) descrxbuff;

	/* Clear it */
	memset(rxd, 0, sizeof(eth_desc_t));

	/* allocate some space for a packet */
	unsigned char *pkt = (unsigned char *) etherrxbuff;

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
	out8(tmp | (1 << 12), MCR0);
}

static void r6040_tx_disable(void) {
	unsigned short tmp = in16(MCR0);
	out8(tmp & ~(1 << 12), MCR0);
}

static void r6040_rx_enable(void) {
	unsigned short tmp = in16(MCR0);
	out8(tmp | (1 << 1), MCR0);
//	out8(2, MCR0);
}

static void r6040_rx_disable(void) {
	out8(0, MCR0);
}

static void r6040_set_tx_start(eth_desc_t* desc) {
	unsigned long tmp = (unsigned long) desc;
	out8((tmp & 0xffff), TX_START_LOW);
	tmp >>= 16;
	out8((tmp & 0xffff), TX_START_HIGH);
}

static void r6040_set_rx_start(eth_desc_t* desc) {
	unsigned long tmp = (unsigned long) desc;
	out8((tmp & 0xffff), RX_START_LOW);
	tmp >>= 16;
	out8((tmp & 0xffff), RX_START_HIGH);
}

/* The RDC interrupt handler */
static irq_return_t irq_handler(irq_nr_t irq_num, void *dev_id) {
	uint16_t misr, status;
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

/* Interface */

/* Keep track of what we've allocated, so we can easily walk it */
eth_desc_t *g_rx_descriptor_list[R6040_RX_DESCRIPTORS];
eth_desc_t *g_rx_descriptor_next;
eth_desc_t *g_tx_descriptor_next;

/* Disable packet reception */
static void r6040_done(void) {
	out8(0, MCR0);
}

static void discard_descriptor(void) {
	/* reset the length field to original value. */
	g_rx_descriptor_next->dlen = g_rx_descriptor_next->dlen_orig;
	g_rx_descriptor_next->status = DSC_OWNER_MAC; /* give back to the MAC */
	g_rx_descriptor_next = g_rx_descriptor_next->DNX; /* Move along */
}

/* Returns size of pkt, or zero if none received */
static size_t r6040_rx(unsigned char* pkt, size_t max_len) {
	size_t ret=0;
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
//static int r6040_start_xmit(struct sk_buff *skb, struct net_device *dev) {
static void r6040_start_xmit(unsigned char* pkt, size_t length) {
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
	while (g_tx_descriptor_next->status & DSC_OWNER_MAC) {
	}
	/* Stop any other activity */
	r6040_tx_disable();

	//desc_dump(g_tx_descriptor_next);
}

static unsigned short r6040_mdio_read(int reg, int phy) {
	out8(MDIO_READ + reg + (phy << 8), MMDIO);
	/* Wait for the read bit to be cleared */
	while (in16(MMDIO) & MDIO_READ);
	return in16(MMRD);
}

/* Wait for linkup, or timeout. */
static int r6040_wait_linkup(void) {
	unsigned short tmp, i;

	for (i = 0; i < 300; i++) {
		/* Check if link up. */
		tmp = r6040_mdio_read(1, 1) & (1 << 2);
		if (tmp) {
			return 1;
		}
		/* Wait 10mS more */
		usleep(10);
	}
	return 0;
}

static int r6040_open(net_device_t *dev) {
	if (-1 == irq_attach(0x0a, irq_handler, 0, dev, "RDC r6040")) {
		return -1;
	}
}

static int r6040_stop(net_device_t *dev) {

}

static const struct net_device_ops r6040_netdev_ops = {
	.ndo_start_xmit = r6040_start_xmit,
	.ndo_open       = r6040_open,
	.ndo_stop       = r6040_stop,
//	.ndo_get_stats  = r6040_get_eth_stat,
//	.ndo_set_mac_address = set_mac_address
};

/* setup descriptors, start packet reception */
static int __init r6040_init(void) {
	size_t i;
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

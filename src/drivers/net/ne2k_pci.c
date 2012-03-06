/**
 * @file
 * @brief Device driver for PCI NE2000 PCI clones (e.g. RealTek RTL-8029).
 * @details Driver to fool Qemu into sending and receiving
 *          packets for us via it's model=ne2k_pci emulation
 *          This driver is unlikely to work with real hardware
 *          without substantial modifications and is purely for
 *          helping with the development of network stacks.
 *          Interrupts are not supported.
 *
 * @date 11.01.11
 * @author Biff
 *         - Initial implementation
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 *         - Adaptation for Embox
 */

#include <asm/io.h>
#include <embox/unit.h>
#include <err.h>
#include <errno.h>
#include <drivers/pci.h>
#include <kernel/irq.h>
#include <linux/init.h>
#include <net/etherdevice.h>
#include <net/in.h>
#include <drivers/ne2k_pci.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

//#include <pnet/core.h>

EMBOX_UNIT_INIT(unit_init);

static net_device_stats_t *get_eth_stat(struct net_device *dev);

/* The per-packet-header format. */
struct e8390_pkt_hdr {
	uint8_t status; /* status */
	uint8_t next;   /* pointer to next packet. */
	uint16_t count; /* header + packet lenght in bytes */
};

#define DEBUG 0
#if DEBUG
/* Debugging routines */
static inline void show_packet(uint8_t *raw, uint16_t size, char *title) {
	uint8_t i, val;

	printf("\nPACKET %s:", title);
	for (i = 0; i < size; i++) {
		if (!(i % 16)) {
			printf("\n");
		}
		val = *(raw + i);
		if (val < 0x10) {
			printf(" 0%X", val);
		} else {
			printf(" %X", val);
		}
	}
	printf("\n.\n");
}
#endif

static inline void ne2k_get_addr_from_prom(struct net_device *dev) {
	uint8_t i;

	dev->addr_len = ETHER_ADDR_LEN;
	/* Copy the station address into the DS8390 registers,
	   and set the multicast hash bitmap to receive all multicasts. */
	out8(NE_PAGE1_STOP, dev->base_addr); /* 0x61 */
	out8(NESM_START_PG_RX, dev->base_addr + EN1_CURPAG);

	/* Get mac-address from prom*/
	out8(E8390_PAGE0 | E8390_RREAD, dev->base_addr + E8390_CMD);
	for (i = 0; i < ETHER_ADDR_LEN; i++) {
		dev->dev_addr[i] = in8(dev->base_addr + NE_DATAPORT);
	}

	/* Copy the station address and set the multicast
	 * hash bitmap to recive all multicast */
	out8(E8390_PAGE1 | E8390_START, dev->base_addr + E8390_CMD);
	for (i = 0; i < ETHER_ADDR_LEN; i++) {
		out8(dev->dev_addr[i], dev->base_addr + EN1_PHYS_SHIFT(i));
		out8(0xFF, dev->base_addr + EN1_MULT_SHIFT(i));
	}
}

/* configure board */
static inline void ne2k_config(struct net_device *dev) {
	unsigned int base_addr;

	base_addr = dev->base_addr;

	/*stop*/
	out8(E8390_PAGE0 | E8390_STOP, base_addr);
	out8(0x49, base_addr + EN0_DCFG); /*16 bit & normal & 4fifo */

	/* Set the transmit page and receive ring. */
	out8(NESM_START_PG_TX, base_addr + EN0_TPSR);
	out8(NESM_START_PG_RX, base_addr + EN0_STARTPG);
	out8(NESM_STOP_PG - 1, base_addr + EN0_BOUNDARY);
	out8(NESM_STOP_PG, base_addr + EN0_STOPPG);

	out8(0xFF, base_addr + EN0_ISR);
	out8(ENISR_ALL, base_addr + EN0_IMR);
	out8(E8390_TXCONFIG, base_addr + EN0_TXCR); /* xmit on */
	out8(E8390_RXCONFIG, base_addr + EN0_RXCR); /* rx on */

//	out8(E8390_PAGE1 | E8390_STOP, base_addr);
//	out8(NESM_START_PG_RX, base_addr + EN1_CURPAG); /* set current page */
}

static inline void set_tx_count(uint16_t val, unsigned long base_addr) {
	/* Set how many bytes we're going to send. */
	out8(val & 0xff, EN0_TBCR_LO + base_addr);
	out8(val >> 8, EN0_TBCR_HI + base_addr);
}

static inline void set_rem_address(uint16_t val, unsigned long base_addr) {
	out8(val & 0xff, EN0_RSARLO + base_addr);
	out8(val >> 8, EN0_RSARHI + base_addr);
}

static inline void set_rem_byte_count(uint16_t val, unsigned long base_addr) {
	out8(val & 0xff, EN0_RCNTLO + base_addr);
	out8(val >> 8, EN0_RCNTHI + base_addr);
}

static inline void copy_data_to_card(uint16_t dest, uint8_t *src,
				uint16_t len, unsigned long base_addr) {
	out8(E8390_NODMA | E8390_PAGE0, base_addr + NE_CMD); // switch to PAGE0
	set_rem_address(dest, base_addr);
	set_rem_byte_count(len, base_addr);
	out8(E8390_RWRITE | E8390_START | E8390_PAGE0, base_addr + NE_CMD);
	if (len & 1) { /* rounding */
		len++;
	}
	outsw(base_addr + NE_DATAPORT, src, len >> 1);
	while (0 == (in8(base_addr + EN0_ISR) & ENISR_RDC));
}

static inline void copy_data_from_card(uint16_t src, uint8_t *dest,
				uint32_t length, unsigned long base_addr) {
	out8(E8390_NODMA | E8390_PAGE0, base_addr + NE_CMD);
	set_rem_address(src, base_addr);
	set_rem_byte_count(length, base_addr);
	out8(E8390_RREAD | E8390_START | E8390_PAGE0, base_addr + NE_CMD);
	insw(base_addr + NE_DATAPORT, dest, length>>1);
	if (length & 1) {
		*(dest + length - 1) = in8(base_addr + NE_DATAPORT);
	}
}

static int start_xmit(struct sk_buff *skb, struct net_device *dev) {
	uint16_t count;
	unsigned long base_addr;

	if ((skb == NULL) || (dev == NULL)) {
		return -EINVAL;
	}

	base_addr = dev->base_addr;

	if (in8(base_addr + NE_CMD) & E8390_TRANS) { /* no matter, which page is active */
		printf("%s: start_xmit() called with the transmitter busy.\n", dev->name);
		return -EBUSY;
	}

	count = skb->len;
	copy_data_to_card(NESM_START_PG_TX << 8, skb->data, count, base_addr);

	/* switch off dma */
	out8(E8390_NODMA | E8390_START | E8390_PAGE0, base_addr + NE_CMD);

	/* send */
	set_tx_count(count, base_addr);
	out8(NESM_START_PG_TX, base_addr + EN0_TPSR);
	out8(E8390_TRANS | E8390_NODMA | E8390_START, base_addr + NE_CMD);
	/* Wait for transmission to complete. */
	while (in8(base_addr + NE_CMD) & E8390_TRANS) ;

#if DEBUG
	show_packet(skb->data, skb->len, "send");
#endif
	kfree_skb(skb); /* free packet */

	return ENOERR;
}

static struct sk_buff * get_skb_from_card(uint16_t total_length,
				uint16_t offset, struct net_device *dev) {
	struct sk_buff *skb;

	skb = alloc_skb(total_length, 0);
	if (skb == NULL) {
		return NULL;
	}

	skb->dev = dev;
	copy_data_from_card(offset, skb->data, total_length, dev->base_addr);
	skb->protocol = ntohs(skb->mac.ethh->h_proto);
#if DEBUG
	show_packet(skb->data, skb->len, "recv");
#endif

	return skb;
}

static size_t ne2k_receive(struct net_device *dev) {
	uint16_t total_length, ring_offset;
	uint8_t this_frame, next_frame;//, current;
	struct e8390_pkt_hdr rx_frame = {0};
	net_device_stats_t *stat;
	unsigned long base_addr;
	struct sk_buff *skb;

	base_addr = dev->base_addr;
	stat = get_eth_stat(dev);

	/* Get the rx page (incoming packet pointer). */
	out8(E8390_PAGE1, base_addr + E8390_CMD);

	/* Remove one frame from the ring.  Boundary is alway a page behind. */
	out8(E8390_PAGE0 | E8390_START, base_addr + E8390_CMD);
	this_frame = in8(base_addr + EN0_BOUNDARY) + 1;
	if (this_frame >= NESM_STOP_PG) {
		this_frame = NESM_START_PG_RX;
	}

	ring_offset =  this_frame << 8;
	/* Find out where the next packet is in card memory */
	copy_data_from_card(ring_offset, (uint8_t *) &rx_frame, sizeof(rx_frame),
			base_addr);
	// FIXME -O2: error: ‘rx_frame’ is used uninitialized in this function.
	total_length = rx_frame.count - sizeof(rx_frame);
	next_frame = this_frame + 1 + (rx_frame.count >> 8);
	if ((rx_frame.next != next_frame)
		&& (rx_frame.next != next_frame + 1)
		&& (rx_frame.next != next_frame - (NESM_STOP_PG - NESM_START_PG_RX))
		&& (rx_frame.next != next_frame + 1 - (NESM_STOP_PG - NESM_START_PG_RX))) {
		stat->rx_err++;
	}
	if ((total_length < 60) || (total_length > 1518)) {
		stat->rx_err++;
		LOG_WARN("ne2k_receive: bad packet size\n");
	} else if ((rx_frame.status & 0x0F) == ENRSR_RXOK) {
		skb = get_skb_from_card(total_length, ring_offset + sizeof(struct e8390_pkt_hdr), dev);
		if (skb) {
			stat->rx_packets++;
			stat->rx_bytes += skb->len;
			netif_rx(skb);
		} else {
			stat->rx_dropped++;
			LOG_WARN("ne2k_receive: couldn't allocate memory for packet\n");
		}
	} else {
		if (rx_frame.status & ENRSR_FO) {
			stat->rx_fifo_errors++;
		}
		LOG_WARN("ne2k_receive: rx_frame.status=0x%X\n", rx_frame.status);
	}
	out8(rx_frame.next - 1, base_addr + EN0_BOUNDARY);

	return ENOERR;
}

static irq_return_t ne2k_handler(irq_nr_t irq_num, void *dev_id) {
	uint8_t isr, status;
	net_device_stats_t *stat;
	unsigned long base_addr;

	stat = get_eth_stat((struct net_device *)dev_id);
	base_addr = ((struct net_device *)dev_id)->base_addr;
	out8(E8390_PAGE0, base_addr + E8390_CMD);
	isr = in8(base_addr + EN0_ISR); /* Gets Interrupt Status Register */
	out8(0xFF, base_addr + EN0_ISR); /* Clean ISR */
	/* IF (isr & (ENISR_RDC | ENISR_TX_ERR)) DO NOTHING */
	if (isr & ENISR_OVER) {
		stat->rx_over_errors++;
		out8(E8390_STOP, base_addr + E8390_CMD);
		while ((in8(base_addr + EN0_ISR) & ENISR_RESET)) ;
		ne2k_receive(dev_id); /* Remove packets right away. */
		out8(E8390_TXCONFIG, base_addr + EN0_TXCR); /* xmit on. */
	} else if (isr & (ENISR_RX + ENISR_RX_ERR)) {
		ne2k_receive(dev_id);
	}
	if (isr & ENISR_TX) {
		status = in8(base_addr + EN0_TSR);
		if (status & ENTSR_COL) {
			stat->collisions++;
		}
		if (status & ENTSR_PTX) {
			stat->tx_packets++;
		} else {
			stat->tx_err++;
			if (status & ENTSR_ABT) { stat->tx_aborted_errors++; }
			if (status & ENTSR_CRS) { stat->tx_carrier_errors++; }
			if (status & ENTSR_FU) { stat->tx_fifo_errors++; }
			if (status & ENTSR_CDH) { stat->tx_heartbeat_errors++; }
			if (status & ENTSR_OWC) { stat->tx_window_errors++; }
		}
	} else if (isr & ENISR_COUNTERS) {
		stat->rx_frame_errors += in8(base_addr + EN0_COUNTER0);
		stat->rx_crc_errors += in8(base_addr + EN0_COUNTER1);
		stat->rx_missed_errors += in8(base_addr + EN0_COUNTER2);
	}

	return IRQ_HANDLED;
}

static net_device_stats_t * get_eth_stat(struct net_device *dev) {
	return &(dev->stats);
}

static int set_mac_address(struct net_device *dev, void *addr) {
	uint32_t i;

	if ((dev == NULL) || (addr == NULL)) {
		return -EINVAL;
	}

	out8(E8390_PAGE1, dev->base_addr + E8390_CMD);
	for (i = 0; i < ETHER_ADDR_LEN; i++) {
		out8(*((uint8_t *)addr + i), dev->base_addr + EN1_PHYS_SHIFT(i));
#if 0
		out8(0xFF, dev->base_addr + EN1_MULT_SHIFT(i));
#endif
	}
	memcpy(dev->dev_addr, addr, ETHER_ADDR_LEN);

	return ENOERR;
}

static int open(struct net_device *dev) {
	if (dev == NULL) {
		return -EINVAL;
	}

	ne2k_get_addr_from_prom(dev);
	ne2k_config(dev);

	return ENOERR;
}

static int stop(struct net_device *dev) {
	if (dev == NULL) {
		return -EINVAL;
	}

	/* reset */
	out8(E8390_PAGE0 | E8390_STOP, dev->base_addr);
	out8(0xFF, dev->base_addr + EN0_ISR);
	out8(0x00, dev->base_addr + EN0_IMR);

	return ENOERR;
}

static const struct net_device_ops _netdev_ops = {
	.ndo_start_xmit = start_xmit,
	.ndo_open = open,
	.ndo_stop = stop,
	.ndo_get_stats = get_eth_stat,
	.ndo_set_mac_address = set_mac_address
};

static int __init unit_init(void) {
	int res;
	uint32_t nic_base;
	struct net_device *nic;
	struct pci_dev *pci_dev;

	//TODO: only RealTek RTL-8029 is available
	pci_dev = pci_find_dev(PCI_VENDOR_ID_REALTEK, PCI_DEV_ID_REALTEK_8029);
	if (pci_dev == NULL) {
		LOG_WARN("Couldn't find NE2K_PCI device\n");
		return ENOERR;
	}

	nic_base = pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK;

	nic = alloc_etherdev(0);
	if (nic == NULL) {
		LOG_ERROR("Couldn't alloc netdev for NE2K_PCI\n");
		return -ENOMEM;
	}
	nic->netdev_ops = &_netdev_ops;
	nic->irq = pci_dev->irq;
	nic->base_addr = nic_base;

	res = irq_attach(pci_dev->irq, ne2k_handler, 0, nic, "ne2k");
	if (res < 0) {
		return res;
	}

	return register_netdev(nic);
}

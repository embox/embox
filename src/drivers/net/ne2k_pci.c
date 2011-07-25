/**
 * @file
 * @brief An implementation of an ne2000 PCI ethernet adapter.
 * @details Driver to fool Qemu into sending and receiving
 *          packets for us via it's ne2k_isa emulation
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
 *         - Adaptation for embox
 */

#include <types.h>
#include <string.h>
#include <asm/io.h>
#include <kernel/irq.h>
#include <net/if_ether.h>
#include <net/skbuff.h>
#include <net/netdevice.h>
#include <net/net.h>
#include <net/etherdevice.h>
#include <embox/unit.h>
#include <linux/init.h>
#include <drivers/pci.h>
#include <net/ne2k_pci.h>
#include <stdio.h>
#include <net/in.h>

EMBOX_UNIT_INIT(unit_init);


static net_device_stats_t * get_eth_stat(struct net_device *dev);

#define E8390_STOP     0x01 /* Stop and reset the chip */
#define E8390_START    0x02 /* Start the chip, clear reset */
#define E8390_TXOFF    0x02 /* EN0_TXCR: Transmitter off */
#define E8390_TRANS    0x04 /* Transmit a frame */
#define E8390_RREAD    0x08 /* Remote read */
#define E8390_RWRITE   0x10 /* Remote write  */
#define E8390_NODMA    0x20 /* Remote DMA */
#define E8390_RXOFF    0x20 /* EN0_RXCR: Accept no packets */
#define E8390_PAGE0    0x00 /* Select page chip registers */
#define E8390_PAGE1    0x40 /* using the two high-order bits */
#define E8390_PAGE2    0x80 /* Page 3 is invalid. */
#define E8390_CMD      0x00 /* The command register (for all pages) */
#define E8390_RXCONFIG 0x04 /* EN0_RXCR: broadcasts, no multicast,errors */
#define E8390_TXCONFIG 0x00 /* EN0_TXCR: Normal transmit mode */

/* The per-packet-header format. */
struct e8390_pkt_hdr {
	uint8_t status; /* status */
	uint8_t next;   /* pointer to next packet. */
	uint16_t count; /* header + packet lenght in bytes */
};



/* Debugging routines */
static inline void ne2k_show_page(unsigned long base_addr) {
	uint8_t i, page, val;
	/* Page Dump*/
    printf("\n            ");
    for (i = 0; i < 16; i++) {
    	printf(" 0%X", i);
    }
	for (page = 0; page < 4; page++) {
		printf("\n8390 page %d:", page);
		out8(E8390_NODMA | (page << 6), base_addr + E8390_CMD);
		for(i = 0; i < 16; i++) {
			val = in8(base_addr + i);
			if (!val) {
				printf(" ..");
			}
			else if (val < 0x10) {
				printf(" 0%X", val);
			}
			else {
				printf(" %X", val);
			}
		}
	}
    printf("\n.\n");
}

static inline void ne2k_show_packet(uint8_t *raw, uint16_t size, char *title) {
	uint8_t i, val;
	/* Page Dump*/
    printf("\nPACKET %s:", title);
	for (i = 0; i < size; i++) {
		if (!(i % 16)) {
			printf("\n");
		}
		val = *(raw + i);
		if (val < 0x10) {
			printf(" 0%X", val);
		}
		else {
			printf(" %X", val);
		}
	}
    printf("\n.\n");
}

static inline void ne2k_get_addr_from_prom(struct net_device *dev) {
	uint8_t i;

	dev->addr_len = ETHER_ADDR_LEN;
	/* Copy the station address into the DS8390 registers,
	   and set the multicast hash bitmap to receive all multicasts. */
	out8(NE_PAGE1_STOP, dev->base_addr); /* 0x61 */
	out8(NESM_START_PG_RX, dev->base_addr + EN1_CURPAG);

	/* Get mac-address from prom*/
	out8(E8390_PAGE0 | E8390_RREAD, dev->base_addr + E8390_CMD);
	for(i = 0; i < ETHER_ADDR_LEN; i++) {
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


static inline void copy_data_to_card(uint16_t dest, uint8_t *src, uint16_t len, unsigned long base_addr) {
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

static inline void copy_data_from_card(uint16_t src, uint8_t *dest, uint32_t length, unsigned long base_addr) {
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

	base_addr = dev->base_addr;
    count = skb->len;

	copy_data_to_card(NESM_START_PG_TX << 8, skb->data, count, base_addr);

	/* switch off dma */
	out8(E8390_NODMA | E8390_START | E8390_PAGE0, base_addr + NE_CMD);

	// send
	if (in8(base_addr + NE_CMD) & E8390_TRANS) {
		printf("%s: start_xmit() called with the transmitter busy.\n", dev->name);
	}

	set_tx_count(count, base_addr);
	out8(NESM_START_PG_TX, base_addr + EN0_TPSR);
	out8(E8390_TRANS | E8390_NODMA | E8390_START, base_addr + NE_CMD);
	while (in8(base_addr + NE_CMD) & E8390_TRANS) ; /* Wait for transmission to complete. */

	kfree_skb(skb); /* free packet */

	return count;
}

static struct sk_buff * get_skb_from_card(uint16_t total_length, uint16_t offset, struct net_device *dev) {
	struct sk_buff *skb;

	if ((skb = alloc_skb(total_length + sizeof(struct sk_buff), 0))) {
		skb->dev = dev;
		skb->len = total_length;
		copy_data_from_card(offset, skb->data, total_length, dev->base_addr);
		skb->mac.raw = skb->data;
		skb->protocol = ntohs(skb->mac.ethh->h_proto);
//		ne2k_show_packet(skb->data, skb->len, "recive");
	}
	return skb;
}

static size_t ne2k_receive(struct net_device *dev) {
	uint16_t total_length, ring_offset;
	uint8_t this_frame, next_frame;//, current;
	struct e8390_pkt_hdr rx_frame;
	net_device_stats_t *stat;
	unsigned long base_addr;
	struct sk_buff *skb;

	base_addr = dev->base_addr;
	stat = get_eth_stat(dev);
///	while {
	/* Get the rx page (incoming packet pointer). */
	out8(E8390_PAGE1, base_addr + E8390_CMD);
//	current = in8(base_addr + EN1_CURPAG);
	/* Remove one frame from the ring.  Boundary is alway a page behind. */
	out8(E8390_PAGE0 | E8390_START, base_addr + E8390_CMD);
	this_frame = in8(base_addr + EN0_BOUNDARY) + 1;
	if (this_frame >= NESM_STOP_PG) {
		this_frame = NESM_START_PG_RX;
	}
//	if (this_frame == current) {
//		;
// ///		break;
//	}
	ring_offset =  this_frame << 8;
	/* Find out where the next packet is in card memory */
	copy_data_from_card(ring_offset, (uint8_t *)&rx_frame, sizeof(struct e8390_pkt_hdr), base_addr);
	total_length = rx_frame.count - sizeof(struct e8390_pkt_hdr);
	next_frame = this_frame + 1 + (rx_frame.count >> 8);
	if ((rx_frame.next != next_frame)
		&& (rx_frame.next != next_frame + 1)
		&& (rx_frame.next != next_frame - (NESM_STOP_PG - NESM_START_PG_RX))
		&& (rx_frame.next != next_frame + 1 - (NESM_STOP_PG - NESM_START_PG_RX))) {
		stat->rx_err++;
//		out8(current - 1, base_addr + EN0_BOUNDARY);
///		continue;
	}
	if ((total_length < 60) || (total_length > 1518)) {
		stat->rx_err++;
		printf("\nWARNING: packet size\n");
	}
	else if ((rx_frame.status & 0x0F) == ENRSR_RXOK) {
		skb = get_skb_from_card(total_length, ring_offset + sizeof(struct e8390_pkt_hdr), dev);
		if (skb) {
			stat->rx_packets++;
			netif_rx(skb);
		}
		else {
			stat->rx_dropped++;
			printf("\nWARNING: couldn't allocate emmory for packet\n");
///			break;
		}
	}
	else {
		printf("\nWARNING rx_frame.status=0x%X\n", rx_frame.status);
		if (rx_frame.status & ENRSR_FO) {
			stat->rx_fifo_errors++;
		}
	}
	out8(rx_frame.next - 1, base_addr + EN0_BOUNDARY);
/// }
	return 0;
}

static irq_return_t ne2k_handler(irq_nr_t irq_num, void *dev_id) {
	uint8_t isr, status;
	net_device_stats_t *stat;
	unsigned long base_addr;

	stat = get_eth_stat((struct net_device *)dev_id);
	base_addr = ((struct net_device *)dev_id)->base_addr;
	out8(E8390_PAGE0, base_addr + E8390_CMD);
	while ((isr = in8(base_addr + EN0_ISR))) {
		if (isr & ENISR_RDC) {
			/* Ack meaningless DMA complete. */
			out8(ENISR_RDC, base_addr + EN0_ISR);
		}
		if (isr & ENISR_OVER) {
			stat->rx_over_errors++;
			out8(E8390_STOP, base_addr + E8390_CMD);
			while ((in8(base_addr + EN0_ISR) & ENISR_RESET)) ;
			ne2k_receive(dev_id); /* Remove packets right away. */
			out8(ENISR_OVER, base_addr + EN0_ISR);
			out8(0xFF, base_addr + EN0_ISR);
			out8(E8390_TXCONFIG, base_addr + EN0_TXCR); /* xmit on. */
		}
		else if (isr & (ENISR_RX + ENISR_RX_ERR)) {
			ne2k_receive(dev_id);
			out8(ENISR_RX + ENISR_RX_ERR, base_addr + EN0_ISR);
		}
		if (isr & ENISR_TX) {
			status = in8(base_addr + EN0_TSR);
			out8(ENISR_TX, base_addr + EN0_ISR); /* Ack intr. */
			if (status & ENTSR_COL) { stat->collisions++; }
			if (status & ENTSR_PTX) { stat->tx_packets++; }
			else {
				stat->tx_err++;
				if (status & ENTSR_ABT) { stat->tx_aborted_errors++; }
				if (status & ENTSR_CRS) { stat->tx_carrier_errors++; }
				if (status & ENTSR_FU) { stat->tx_fifo_errors++; }
				if (status & ENTSR_CDH) { stat->tx_heartbeat_errors++; }
				if (status & ENTSR_OWC) { stat->tx_window_errors++; }
			}
		}
		else if (isr & ENISR_COUNTERS) {
			stat->rx_frame_errors += in8(base_addr + EN0_COUNTER0);
			stat->rx_crc_errors += in8(base_addr + EN0_COUNTER1);
			stat->rx_missed_errors += in8(base_addr + EN0_COUNTER2);
			out8(ENISR_COUNTERS, base_addr + EN0_ISR);
		}
		if (isr & ENISR_TX_ERR) {
			out8(ENISR_TX_ERR, base_addr + EN0_ISR);
		}
	}
	return IRQ_HANDLED;
}

static net_device_stats_t * get_eth_stat(struct net_device *dev) {
	return &(dev->stats);
}

static int set_mac_address(struct net_device *dev, void *addr) {
	uint32_t i;

	if (NULL == dev || NULL == addr) {
		return -1;
	}
	out8(E8390_PAGE1, dev->base_addr + E8390_CMD);
	for (i = 0; i < ETHER_ADDR_LEN; i++) {
		out8(*((uint8_t *)addr + i), dev->base_addr + EN1_PHYS_SHIFT(i));
		out8(0xFF, dev->base_addr + EN1_MULT_SHIFT(i));
	}
	memcpy(dev->dev_addr, addr, ETHER_ADDR_LEN);

	return 0;
}


static int open(struct net_device *dev) {
	if (NULL == dev) {
		return -1;
	}

	ne2k_get_addr_from_prom(dev);
	ne2k_config(dev);
	ne2k_show_page(dev->base_addr);

	return 0;
}

static int stop(struct net_device *dev) {
	if (NULL == dev) {
		return -1;
	}
	/* reset */
	out8(E8390_PAGE0 | E8390_STOP, dev->base_addr);
	out8(0xFF, dev->base_addr + EN0_ISR);
	out8(0x00, dev->base_addr + EN0_IMR);

	return 0;
}

static const struct net_device_ops _netdev_ops = {
	.ndo_start_xmit = start_xmit,
	.ndo_open = open,
	.ndo_stop = stop,
	.ndo_get_stats = get_eth_stat,
	.ndo_set_mac_address = set_mac_address
};

static int __init unit_init(void) {
	uint32_t nic_base;
	struct net_device *nic;
	struct pci_dev *pci_dev;

	pci_dev = pci_find_dev(0x10EC, 0x8029); //TODO pci ID
	if (NULL == pci_dev) {
		LOG_WARN("Couldn't find NE2K_PCI device");
		return 0;
	}
	nic_base = pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK;
	if (NULL != (nic = alloc_etherdev(0))) {
		nic->netdev_ops = &_netdev_ops;
		nic->irq = pci_dev->irq;
		nic->base_addr = nic_base;
	}
	else {
		LOG_WARN("Couldn't alloc netdev\n");
		return -1;
	}
	if (-1 == irq_attach(pci_dev->irq, ne2k_handler, 0, nic, "ne2k")) {
		return -1;
	}

	return 0;
}

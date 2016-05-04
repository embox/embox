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

#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <net/l0/net_entry.h>

#include <asm/io.h>


#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>
#include <kernel/irq.h>
#include <net/l2/ethernet.h>
#include <drivers/net/ne2k_pci.h>

#include <net/netdevice.h>
#include <net/inetdevice.h>
#include <net/skbuff.h>

#include <embox/unit.h>


PCI_DRIVER("ne2k_pci", ne2k_init, PCI_VENDOR_ID_REALTEK, PCI_DEV_ID_REALTEK_8029);

/* The per-packet-header format. */
struct e8390_pkt_hdr {
	uint8_t status; /* status */
	uint8_t next;   /* pointer to next packet. */
	uint16_t count; /* header + packet lenght in bytes */
};

#define DEBUG 0
#if DEBUG
#include <kernel/printk.h>
/* Debugging routines */
static inline void show_packet(uint8_t *raw, int size, char *title) {
	int i;

	printk("\nPACKET(%d) %s:", size, title);
	for (i = 0; i < size; i++) {
		if (!(i % 16)) {
			printk("\n");
		}
		printk(" %02hhX", *(raw + i));
	}
	printk("\n.\n");
}
#endif

static void ne2k_get_addr_from_prom(struct net_device *dev) {
	uint8_t i;

	dev->addr_len = ETH_ALEN;
	/* Copy the station address into the DS8390 registers,
	   and set the multicast hash bitmap to receive all multicasts. */
	out8(NE_PAGE1_STOP, dev->base_addr); /* 0x61 */
	out8(NESM_START_PG_RX, dev->base_addr + EN1_CURPAG);

	/* Get mac-address from prom*/
	out8(E8390_PAGE0 | E8390_RREAD | E8390_NODMA, dev->base_addr + E8390_CMD);
	for (i = 0; i < ETH_ALEN; i++) {
		dev->dev_addr[i] = in8(dev->base_addr + NE_DATAPORT);
		(void)in8(dev->base_addr + NE_DATAPORT);
	}
}

/* Configure board */
static void ne2k_config(struct net_device *dev) {
	unsigned int base_addr;
	int i;

	base_addr = dev->base_addr;

	/* Stop */
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

	/* Copy the station address and set the multicast
	 * hash bitmap to recive all multicast */
	out8(E8390_NODMA | E8390_PAGE1, dev->base_addr + E8390_CMD);
	for (i = 0; i < ETH_ALEN; i++) {
		out8(dev->dev_addr[i], dev->base_addr + EN1_PHYS_SHIFT(i));
		out8(0xFF, dev->base_addr + EN1_MULT_SHIFT(i));
	}
	out8(E8390_NODMA | E8390_PAGE0 | E8390_START, dev->base_addr + E8390_CMD);

//	out8(E8390_PAGE1 | E8390_STOP, base_addr);
//	out8(NESM_START_PG_RX, base_addr + EN1_CURPAG); /* set current page */
}

static void set_tx_count(uint16_t val, unsigned long base_addr) {
	/* Set how many bytes we're going to send. */
	out8(val & 0xff, EN0_TBCR_LO + base_addr);
	out8(val >> 8, EN0_TBCR_HI + base_addr);
}

static void set_rem_address(uint16_t val, unsigned long base_addr) {
	out8(val & 0xff, EN0_RSARLO + base_addr);
	out8(val >> 8, EN0_RSARHI + base_addr);
}

static void set_rem_byte_count(uint16_t val, unsigned long base_addr) {
	out8(val & 0xff, EN0_RCNTLO + base_addr);
	out8(val >> 8, EN0_RCNTHI + base_addr);
}

static void copy_data_to_card(uint16_t dest, uint8_t *src,
				uint16_t len, unsigned long base_addr) {
	out8(E8390_NODMA | E8390_PAGE0, base_addr + NE_CMD); // switch to PAGE0
	set_rem_address(dest, base_addr);
	set_rem_byte_count(len, base_addr);
	out8(E8390_RWRITE | E8390_START | E8390_PAGE0, base_addr + NE_CMD);
	if (len & 1) { /* rounding */
		len++;
	}
	outsw(base_addr + NE_DATAPORT, src, len >> 1);
//	while (0 == (in8(base_addr + EN0_ISR) & ENISR_RDC)); /* FIXME sometimes it requires much time */
//	out8(ENISR_RDC, base_addr + EN0_ISR);
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

static int xmit(struct net_device *dev, struct sk_buff *skb) {
	uint16_t count;
	unsigned long base_addr;

	if ((skb == NULL) || (dev == NULL)) {
		return -EINVAL;
	}

	base_addr = dev->base_addr;

	if (in8(base_addr + NE_CMD) & E8390_TRANS) { /* no matter, which page is active */
		return -EBUSY;
	}

	count = skb->len;
	copy_data_to_card(NESM_START_PG_TX << 8, skb->mac.raw, count, base_addr);

	/* switch off dma */
	out8(E8390_NODMA | E8390_START | E8390_PAGE0, base_addr + NE_CMD);

	/* send */
	set_tx_count(count, base_addr);
	out8(NESM_START_PG_TX, base_addr + EN0_TPSR);
	out8(E8390_TRANS | E8390_NODMA | E8390_START, base_addr + NE_CMD);
	/* Wait for transmission to complete. */
	while (in8(base_addr + NE_CMD) & E8390_TRANS);

#if DEBUG
	show_packet(skb->mac.raw, skb->len, "send");
#endif
	skb_free(skb); /* free packet */

	return ENOERR;
}

static struct sk_buff * get_skb_from_card(uint16_t total_length,
				uint16_t offset, struct net_device *dev) {
	struct sk_buff *skb;

	skb = skb_alloc(total_length);
	if (skb == NULL) {
		return NULL;
	}

	skb->dev = dev;
	copy_data_from_card(offset, skb->mac.raw, total_length, dev->base_addr);
#if DEBUG
	show_packet(skb->mac.raw, skb->len, "recv");
#endif

	return skb;
}

static void ne2k_receive(struct net_device *dev) {
	uint16_t total_length, ring_offset;
	uint8_t this_frame, next_frame, current_page;
	struct e8390_pkt_hdr rx_frame = {0};
	net_device_stats_t *stat;
	unsigned long base_addr;
	struct sk_buff *skb;
	uint8_t tries;

	base_addr = dev->base_addr;
	stat = &dev->stats;

	tries = 10;
	while (--tries) { /* XXX It's an infinite loop or not? */
		/* Get the current page */
		out8(E8390_NODMA | E8390_PAGE1, base_addr + E8390_CMD);
		current_page = in8(base_addr + EN1_CURPAG);
		out8(E8390_NODMA | E8390_PAGE0, base_addr + E8390_CMD);

		/* Remove one frame from the ring.  Boundary is always a page behind. */
		this_frame = in8(base_addr + EN0_BOUNDARY) + 1;
		if (this_frame >= NESM_STOP_PG) {
			this_frame = NESM_START_PG_RX;
		}

		if (this_frame == current_page) {
			break; /* No any more package to receive */
		}

		ring_offset = this_frame << 8;
		/* Find out where the next packet is in card memory */
		copy_data_from_card(ring_offset, (uint8_t *) &rx_frame, sizeof(rx_frame),
				base_addr);

		total_length = rx_frame.count - sizeof(rx_frame);
		next_frame = this_frame + 1 + (rx_frame.count >> 8);
		if ((rx_frame.next != next_frame)
			&& (rx_frame.next != next_frame + 1)
			&& (rx_frame.next != next_frame - (NESM_STOP_PG - NESM_START_PG_RX))
			&& (rx_frame.next != next_frame + 1 - (NESM_STOP_PG - NESM_START_PG_RX))) {
			stat->rx_err++;
			out8(current_page - 1, base_addr + EN0_BOUNDARY);
			continue;
		}
		if ((total_length < 60) || (total_length > 1518)) {
			stat->rx_err++;
		} else if ((rx_frame.status & 0x0F) == ENRSR_RXOK) {
			skb = get_skb_from_card(total_length, ring_offset + sizeof(struct e8390_pkt_hdr), dev);
			if (skb) {
				stat->rx_packets++;
				stat->rx_bytes += skb->len;
				netif_rx(skb);
			} else {
				stat->rx_dropped++;
			}
		} else {
			if (rx_frame.status & ENRSR_FO) {
				stat->rx_fifo_errors++;
			}
		}
		out8(rx_frame.next - 1, base_addr + EN0_BOUNDARY);
	}
	if (!tries) {

	}

	out8(ENISR_RX | ENISR_RX_ERR, base_addr + EN0_ISR);
}

static irq_return_t ne2k_handler(unsigned int irq_num, void *dev_id) {
	uint8_t isr, status;
	net_device_stats_t *stat;
	unsigned long base_addr;

	stat = &((struct net_device *)dev_id)->stats;
	base_addr = ((struct net_device *)dev_id)->base_addr;

	out8(E8390_NODMA | E8390_PAGE0, base_addr + E8390_CMD);
	/* Get Interrupt Status Register */
	while ((isr = in8(base_addr + EN0_ISR)) != 0) {
		if (isr & ENISR_OVER) {
			stat->rx_over_errors++;
			status = in8(base_addr + E8390_CMD);
			out8(E8390_NODMA | E8390_PAGE0 | E8390_STOP, base_addr + E8390_CMD);
			out8(0x00, base_addr + EN0_RCNTLO);
			out8(0x00, base_addr + EN0_RCNTHI);
			status = status ? !(isr & (ENISR_TX | ENISR_TX_ERR)) : 0;
			out8(E8390_TXOFF, base_addr + EN0_TXCR);
			out8(E8390_NODMA | E8390_PAGE0 | E8390_START, base_addr + E8390_CMD);
			ne2k_receive(dev_id); /* Remove packets right away. */
			out8(ENISR_OVER, base_addr + EN0_ISR);
			out8(E8390_TXCONFIG, base_addr + EN0_TXCR); /* xmit on. */
			if (status) {
				out8(E8390_NODMA | E8390_PAGE0 | E8390_START | E8390_TRANS, base_addr + E8390_CMD); /* resend packet */
			}
		} else if (isr & (ENISR_RX | ENISR_RX_ERR)) {
			ne2k_receive(dev_id);
		}
		if (isr & ENISR_TX) {
			status = in8(base_addr + EN0_TSR);
			out8(ENISR_TX, base_addr + EN0_ISR);
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
			if ((status & ENTSR_COL) && (status & ENTSR_ABT)) {
				netdev_flag_down(dev_id, IFF_RUNNING);
			}
			else {
				netdev_flag_up(dev_id, IFF_RUNNING);
			}
		} else if (isr & ENISR_TX_ERR) {
			out8(ENISR_TX_ERR, base_addr + EN0_ISR); // TODO
		}
		if (isr & ENISR_COUNTERS) {
			stat->rx_frame_errors += in8(base_addr + EN0_COUNTER0);
			stat->rx_crc_errors += in8(base_addr + EN0_COUNTER1);
			stat->rx_missed_errors += in8(base_addr + EN0_COUNTER2);
			out8(ENISR_COUNTERS, base_addr + EN0_ISR);
		}
		if (isr & ENISR_RDC) {
			out8(ENISR_RDC, base_addr + EN0_ISR);
		}
		out8(E8390_NODMA | E8390_PAGE0 | E8390_START, base_addr + E8390_CMD);
	}

	return IRQ_HANDLED;
}

static int set_mac_address(struct net_device *dev, const void *addr) {
	uint32_t i;

	if ((dev == NULL) || (addr == NULL)) {
		return -EINVAL;
	}

	out8(E8390_NODMA | E8390_PAGE1, dev->base_addr + E8390_CMD);
	for (i = 0; i < ETH_ALEN; i++) {
		out8(*((uint8_t *)addr + i), dev->base_addr + EN1_PHYS_SHIFT(i));
	}

	memcpy(dev->dev_addr, addr, ETH_ALEN);

	return ENOERR;
}

static int ne2k_open(struct net_device *dev) {
	if (dev == NULL) {
		return -EINVAL;
	}

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

static const struct net_driver _drv_ops = {
	.xmit = xmit,
	.start = ne2k_open,
	.stop = stop,
	.set_macaddr = set_mac_address
};

static int ne2k_init(struct pci_slot_dev *pci_dev) {
	int res;
	uint32_t nic_base;
	struct net_device *nic;

	assert(pci_dev != NULL);

	nic_base = pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK;

	nic = etherdev_alloc(0);
	if (nic == NULL) {
		return -ENOMEM;
	}
	nic->drv_ops = &_drv_ops;
	nic->irq = pci_dev->irq;
	nic->base_addr = nic_base;

	ne2k_get_addr_from_prom(nic);

	res = irq_attach(pci_dev->irq, ne2k_handler, IF_SHARESUP, nic, "ne2k");
	if (res < 0) {
		return res;
	}

	return inetdev_register_dev(nic);
}

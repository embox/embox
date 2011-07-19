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

EMBOX_UNIT_INIT(unit_init);

#define E8390_STOP   0x01 /* Stop and reset the chip */
#define E8390_START  0x02 /* Start the chip, clear reset */
#define E8390_TXOFF  0x02 /* EN0_TXCR: Transmitter off */
#define E8390_TRANS  0x04 /* Transmit a frame */
#define E8390_RREAD  0x08 /* Remote read */
#define E8390_RWRITE 0x10 /* Remote write  */
#define E8390_NODMA  0x20 /* Remote DMA */
#define E8390_RXOFF  0x20 /* EN0_RXCR: Accept no packets */
#define E8390_PAGE0  0x00 /* Select page chip registers */
#define E8390_PAGE1  0x40 /* using the two high-order bits */
#define E8390_PAGE2  0x80 /* Page 3 is invalid. */
#define E8390_CMD    0x00 /* The command register (for all pages) */

static inline void ne2kpci_config(unsigned long base_addr) {
    out8(E8390_PAGE0, base_addr + E8390_CMD); /* Select page 0*/
    out8(0x00, base_addr + EN0_TXCR);
    out8(0x00, base_addr + EN0_IMR); /* Mask completion irq. */
    out8(0xFF, base_addr + EN0_ISR);
///    out8(E8390_RXOFF, nic_base + EN0_RXCR);*/ /* 0x20  Set to monitor */
    out8(13, base_addr + EN0_DCFG); /* Word mode */
}

static inline void rx_enable(void) {
	out8(NE_PAGE0_STOP,   NE_CMD);
	out8(RX_BUFFER_START, EN0_BOUNDARY);
	out8(NE_PAGE1_STOP,   NE_CMD);
	out8(RX_BUFFER_START, EN1_CURPAG);
	out8(NE_START,        NE_CMD);
}

static inline void rx_disable(void) {
	/* do nothing */
	out8(NE_STOP, NE_CMD);
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

static void copy_data_to_card(uint16_t dest, uint8_t *src, uint16_t length, unsigned long base_addr) {
    out8(E8390_PAGE0 + E8390_NODMA, base_addr + NE_CMD);
	set_rem_address(dest, base_addr);
	set_rem_byte_count(length, base_addr);
    out8(E8390_RWRITE, base_addr + NE_CMD);
	if (length & 1) {
		length++;
	}
	outsw(base_addr + NE_DATAPORT, src, length>>1);
}

#if 0
// XXX defined but not used
static void copy_data_from_card(uint32_t src, uint8_t *dest, uint32_t length) {
	uint32_t i;
	set_rem_address(src);
	set_rem_byte_count(length);
	for (i = 0; i < length; i++) {
		*dest = in8(NE_BASE + 0x10);
		dest++;
	}
}
/**
 * Copy data out of the receive buffer.
 */
static size_t copy_pkt_from_card(uint8_t *dest, uint32_t max_len) {
	uint32_t next, total_length;
	uint8_t header[4];
	/* Find out where the next packet is in card memory */
	uint32_t src = in8(EN0_BOUNDARY) * 256;

	copy_data_from_card(src, header, sizeof(header));

	next = header[1];

	total_length = header[3];
	total_length <<= 8;
	total_length |=  header[2];

	/* Now copy it to buffer, if possible, skipping the info header. */
	src += 4;
	total_length -= 4;
	copy_data_from_card(src, dest, total_length);

	/* Release the buffer by increasing the boundary pointer. */
	out8(next, EN0_BOUNDARY);

	return total_length;
}

/**
 * Returns size of pkt, or zero if none received.
 */
static size_t pkt_receive(struct sk_buff *skb) {
	uint32_t boundary, current;
	size_t ret = 0;

	out8(NE_PAGE1, NE_CMD);
	current = in8(EN1_CURPAG);

	/* Check if rsr fired. */
	out8(NE_PAGE0, NE_CMD);
	boundary = in8(EN0_BOUNDARY);

	if (boundary != current) {
		ret = copy_pkt_from_card(skb->data, skb->len);
	}
	return ret;
}
#endif

/**
 * queue packet for transmission
 */
static int start_xmit(struct sk_buff *skb, struct net_device *dev) {
	unsigned long nic_base;
	uint16_t count;

	nic_base = dev->base_addr;
    count = skb->len;
    // copy
    copy_data_to_card(NESM_START_PG << 8, skb->data, count, nic_base);
    // send
    out8(E8390_NODMA + E8390_PAGE0, nic_base + NE_CMD);
    if (in8(nic_base) & E8390_TRANS) {
    	printf("%s: start_xmit() called with the transmitter busy.\n", dev->name);
    }
    set_tx_count(count, nic_base);
    out8(NESM_START_PG, nic_base + EN0_TPSR);
    out8(E8390_NODMA + E8390_TRANS + E8390_START, nic_base + NE_CMD);
    while (in8(nic_base + NE_CMD) & E8390_TRANS) ; /* Wait for transmission to complete. */
    out8(E8390_STOP, nic_base + NE_CMD);
    return count;
}

static int open(net_device_t *dev) {
	unsigned long nic_base;
	if (NULL == dev) {
		return -1;
	}
	nic_base = dev->base_addr;
#if 0
	/* 8-bit access only, makes the maths simpler. */
	out8(0, nic_base + 0x0e);

	/* setup receive buffer location */
	out8(RX_BUFFER_START, EN0_STARTPG);
	out8(RX_BUFFER_END, EN0_STOPPG);
#endif
	return 0;
}

static int stop(net_device_t *dev) {
	if (NULL == dev) {
		return 0;
	}
	//TODO
	return 1;
}

static int set_mac_address(struct net_device *dev, void *addr) {
	uint32_t i;
	if (NULL == dev || NULL == addr) {
		return -1;
	}
	out8(E8390_NODMA + E8390_PAGE1, dev->base_addr + E8390_CMD);
	for (i = 0; i < ETHER_ADDR_LEN; i++) {
		out8(*((uint8_t *)addr + i), dev->base_addr + i + 1);
	}
	return 0;
}

/**
 * Get RX/TX stats
 */
static net_device_stats_t *get_eth_stat(net_device_t *dev) {
	return &(dev->stats);
}

static irq_return_t ne2k_handler(irq_nr_t irq_num, void *dev_id) {
//	printf("irq fired\n");
	return IRQ_HANDLED;
}

static const struct net_device_ops _netdev_ops = {
        .ndo_start_xmit      = start_xmit,
        .ndo_open            = open,
        .ndo_stop            = stop,
        .ndo_get_stats       = get_eth_stat,
        .ndo_set_mac_address = set_mac_address
};

static int __init unit_init(void) {
	net_device_t *nic;
	uint32_t i, nic_base;
	struct pci_dev *pci_dev;

	pci_dev = pci_find_dev(0x10EC, 0x8029);
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
		return -1;
	}
	if (-1 == irq_attach(pci_dev->irq, ne2k_handler, 0, nic, "ne2k")) {
		return -1;
	}
	ne2kpci_config(nic_base);
    out8(E8390_PAGE0 + E8390_RREAD, nic_base + E8390_CMD);
    for(i = 0; i < ETHER_ADDR_LEN; i++) {
		nic->dev_addr[i] = in8(nic_base + NE_DATAPORT);
    }
    nic->addr_len = ETHER_ADDR_LEN;
    set_mac_address(nic, nic->dev_addr);
#if 0
    /* Page Dump*/
	for (int page = 0; page < 4; page++) {
		printf("8390 page %d:", page);
		out8(E8390_NODMA + (page << 6), nic_base + E8390_CMD);
		for(int i = 0; i < 16; i++) {
			printf(" %2x", in8(nic_base + i));
		}
		printf(".\n");
	}
#endif
	return 0;
}

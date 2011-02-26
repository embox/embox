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
#include <net/net_pack_manager.h>
#include <net/etherdevice.h>
#include <embox/unit.h>
#include <linux/init.h>
#include <drivers/pci.h>

EMBOX_UNIT_INIT(unit_init);

/* Port addresses */
#define NE_BASE	        0x300
#define NE_CMD          (NE_BASE + 0x00) /* The command register (for all pages) */
/* Page 0 register offsets */
#define EN0_STARTPG     (NE_BASE + 0x01) /* Starting page of ring buffer */
#define EN0_STOPPG      (NE_BASE + 0x02) /* Ending page +1 of ring buffer */
#define EN0_BOUNDARY    (NE_BASE + 0x03) /* Boundary page of ring buffer */
#define EN0_TPSR        (NE_BASE + 0x04) /* Transmit status reg */
#define EN0_TBCR_LO     (NE_BASE + 0x05) /* Low  byte of tx byte count */
#define EN0_TBCR_HI     (NE_BASE + 0x06) /* High byte of tx byte count */
#define EN0_ISR         (NE_BASE + 0x07) /* Interrupt status reg */
/* Where to DMA data to/from */
#define EN0_RSARLO      (NE_BASE + 0x08) /* Remote start address reg 0 */
#define EN0_RSARHI      (NE_BASE + 0x09) /* Remote start address reg 1 */
/* How much data to DMA */
#define EN0_RCNTLO      (NE_BASE + 0x0a) /* Remote byte count reg */
#define EN0_RCNTHI      (NE_BASE + 0x0b) /* Remote byte count reg */

#define EN0_RXCR        (NE_BASE + 0x0c) /* RX configuration reg */
#define EN0_TXCR        (NE_BASE + 0x0d) /* TX configuration reg */
#define EN0_DCFG        (NE_BASE + 0x0e) /* Data configuration reg */
#define EN0_IMR         (NE_BASE + 0x0f) /* Interrupt mask reg */

/* Bits in EN0_ISR - Interrupt status register */
#define ENISR_RX        0x01    /* Receiver, no error */
#define ENISR_TX        0x02    /* Transmitter, no error */
#define ENISR_RX_ERR    0x04    /* Receiver, with error */
#define ENISR_TX_ERR    0x08    /* Transmitter, with error */
#define ENISR_OVER      0x10    /* Receiver overwrote the ring */
#define ENISR_COUNTERS  0x20    /* Counters need emptying */
#define ENISR_RDC       0x40    /* remote dma complete */
#define ENISR_RESET     0x80    /* Reset completed */
#define ENISR_ALL       0x3f    /* Interrupts we will enable */

/* Page 1 register offsets */
#define EN1_PHYS          (NE_BASE + 0x01)  /* This board's physical eth addr */
#define EN1_PHYS_SHIFT(i) (NE_BASE + i + 1) /* Get and set mac address */
#define EN1_CURPAG        (NE_BASE + 0x07)  /* Current memory page RD WR */
#define EN1_MULT          (NE_BASE + 0x08)  /* Multicast filter mask array (8 bytes) */
#define EN1_MULT_SHIFT(i) (NE_BASE + 8 + i) /* Get and set multicast filter */

#define NE_DATAPORT       0x10 /* NatSemi-defined port window offset*/
#define NE_RESET          0x1f

/* Commands to select the different pages. */
#define NE_PAGE0_STOP      0x21
#define NE_PAGE1_STOP      0x61

#define NE_PAGE0           0x20
#define NE_PAGE1           0x60

#define NE_START           0x02
#define NE_STOP            0x01

#define NE_PAR0            (NE_BASE + 0x01)
#define NE_PAR1            (NE_BASE + 0x02)
#define NE_PAR2            (NE_BASE + 0x03)
#define NE_PAR3            (NE_BASE + 0x04)
#define NE_PAR4            (NE_BASE + 0x05)
#define NE_PAR5            (NE_BASE + 0x06)

#define MEMBASE            (16 * 1024)
#define NE_PAGE_SIZE       256
#define TX_BUFFER_START    (MEMBASE / NE_PAGE_SIZE)
#define NE_TXBUF_SIZE      6
#define RX_BUFFER_START    (TX_BUFFER_START + NE_TXBUF_SIZE)
#define RX_BUFFER_END      ((32 * 1024) / NE_PAGE_SIZE)

/* Applies to ne2000 version of the card. */
#define NESM_START_PG      0x40    /* First page of TX buffer */
#define NESM_STOP_PG       0x80    /* Last page +1 of RX ring */

inline static void rx_enable(void) {
	out8(NE_PAGE0_STOP,   NE_CMD);
	out8(RX_BUFFER_START, EN0_BOUNDARY);
	out8(NE_PAGE1_STOP,   NE_CMD);
	out8(RX_BUFFER_START, EN1_CURPAG);
	out8(NE_START,        NE_CMD);
}

inline static void rx_disable(void) {
	/* do nothing */
	out8(NE_STOP, NE_CMD);
}

inline static void set_tx_count(uint32_t val) {
	/* Set how many bytes we're going to send. */
	out8(val & 0xff, EN0_TBCR_LO);
	out8((val & 0xff00) >> 8, EN0_TBCR_HI);
}

inline static void set_rem_address(uint32_t val) {
	/* Set how many bytes we're going to send. */
	out8(val & 0xff, EN0_RSARLO);
	out8((val & 0xff00) >> 8, EN0_RSARHI);
}

inline static void set_rem_byte_count(uint32_t val) {
	/* Set how many bytes we're going to send. */
	out8(val & 0xff, EN0_RCNTLO);
	out8((val & 0xff00) >> 8, EN0_RCNTHI);
}

static void copy_data_to_card(uint32_t dest, uint8_t* src, uint32_t length) {
	uint32_t i;
	set_rem_address(dest);
	set_rem_byte_count(length);
	for (i = 0; i < length; i++) {
		out8(*src, NE_BASE + 0x10);
		src++;
	}
}

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

/**
 * queue packet for transmission
 */
static int start_xmit(struct sk_buff *skb, struct net_device *dev) {
	/* Set TPSR, start of tx buffer memory to zero
	 * (this value is count of pages) */
	out8(TX_BUFFER_START, EN0_TPSR);

	copy_data_to_card(16 * 1024, skb->data, skb->len);

	/* Set how many bytes to transmit */
	set_tx_count(skb->len);

	/* issue command to actually transmit a frame */
	out8(0x04, NE_CMD);

	/* Wait for transmission to complete. */
	while (in8(NE_CMD) & 0x04);

	TRACE("Done transmit\n");

	return skb->len;
}

static int open(net_device_t *dev) {
	if (NULL == dev) {
		return -1;
	}
	/* 8-bit access only, makes the maths simpler. */
	out8(0, NE_BASE + 0x0e);

	/* setup receive buffer location */
	out8(RX_BUFFER_START, EN0_STARTPG);
	out8(RX_BUFFER_END, EN0_STOPPG);

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
	if (NULL == dev || NULL == addr) {
		return -1;
	}
	//TODO
	return 0;
}

/**
 * Get RX/TX stats
 */
static net_device_stats_t *get_eth_stat(net_device_t *dev) {
	return &(dev->stats);
}

static irq_return_t ne2k_handler(irq_nr_t irq_num, void *dev_id) {
	TRACE("irq fired\n");
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
	uint8_t *mac;
	uint16_t new_command, pci_command;
	uint8_t  pci_latency;

	if (NULL != (nic = alloc_etherdev(0))) {
		nic->netdev_ops = &_netdev_ops;
		//TODO: get devfn=0x18 from pci_find_dev
		pci_read_config32(0, 0x18, PCI_BASE_ADDR_REG_0, &nic->base_addr);
		pci_read_config8(0, 0x18, PCI_INTERRUPT_LINE, &nic->irq);
		nic->base_addr &= PCI_BASE_ADDR_IO_MASK;
	}

	/* Set device to be bus master */
	pci_read_config16(0, 0x18, PCI_COMMAND, &pci_command);
	new_command = pci_command | PCI_COMMAND_MASTER | PCI_COMMAND_IO;
	pci_write_config16(0, 0x18, PCI_COMMAND, new_command);

	/* Set latency */
	pci_read_config8(0, 0x18, PCI_LATENCY_TIMER, &pci_latency);
	if (pci_latency < 32) {
		pci_write_config8(0, 0x18, PCI_LATENCY_TIMER, 32);
	}

	/* Reset */
	out8(in8(nic->base_addr + NE_RESET), nic->base_addr + NE_RESET);
	out8(ENISR_RESET, nic->base_addr + EN0_ISR);

	if (-1 == irq_attach(nic->irq, ne2k_handler, 0, nic, "ne2k")) {
		return -1;
	}

	/* Back to page 0 */
	out8(NE_PAGE0_STOP, NE_CMD);

	/* That's for the card area, however we must also set the mac
	 * in the card ram as well, because that's what the
	 * qemu emulation actually uses to determine if the packet's
	 * bound for this NIC.
	 */
//	mac = (uint8_t*) ETHER_MAC;
//	for (uint32_t i = 0; i < 6; i++) {
//		copy_data_to_card(i * 2, mac, 1);
//		mac++;
//	}

//	rx_disable();
	return 0;
}


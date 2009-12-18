/**
 *
 */
#include <types.h>
#include <autoconf.h>
#include <kernel/module.h>
#include <net/if_ether.h>
#include <net/skbuff.h>
#include <net/netdevice.h>
#include <net/net.h>
#include <net/if_ether.h>
#include <net/net_pack_manager.h>
#include <net/etherdevice.h>
#include <kernel/irq.h>
#include <net/etherdevice.h>
#include <asm/spin_lock.h>
#include <common.h>

DECLARE_MODULE("Ethernet Emac lite", module_init)

#define PKTSIZE 0x800

/* Xmit complete */
#define XEL_TSR_XMIT_BUSY_MASK		0x00000001UL
/* Xmit interrupt enable bit */
#define XEL_TSR_XMIT_IE_MASK		0x00000008UL
/* Buffer is active, SW bit only */
#define XEL_TSR_XMIT_ACTIVE_MASK	0x80000000UL
/* Program the MAC address */
#define XEL_TSR_PROGRAM_MASK		0x00000002UL
/* define for programming the MAC address into the EMAC Lite */
#define XEL_TSR_PROG_MAC_ADDR	(XEL_TSR_XMIT_BUSY_MASK | XEL_TSR_PROGRAM_MASK)

/* Transmit packet length upper byte */
#define XEL_TPLR_LENGTH_MASK_HI		0x0000FF00UL
/* Transmit packet length lower byte */
#define XEL_TPLR_LENGTH_MASK_LO		0x000000FFUL

/* Recv complete */
#define XEL_RSR_RECV_DONE_MASK		0x00000001UL
/* Recv interrupt enable bit */
#define XEL_RSR_RECV_IE_MASK		0x00000008UL

typedef struct mdio_regs {
	uint32_t regs;
} mdio_regs_t;

typedef struct xilinx_emaclite_regs {
	uint8_t tx_pack[0x07F0];
	mdio_regs_t mdio_regs;
	uint32_t tx_len; /*0x07F4*/
	uint32_t gio;
	uint32_t tx_ctrl;
	uint32_t pong_buff[0x800];
	uint8_t rx_pack[0x17FC]; /*0x1000*/
	uint32_t rx_ctrl;
} xilinx_emaclite_regs_t;

static struct xilinx_emaclite_regs *emaclite =
		(struct xilinx_emaclite_regs *) XILINX_EMACLITE_BASEADDR;

static uint8_t etherrxbuff[PKTSIZE]; /* Receive buffer */

/**
 * Send a packet on this device.
 */
static int start_xmit(struct sk_buff *pack, struct net_device *dev) {
	return -1;
}

/**
 *
 */
static void pack_received() {
	uint16_t length, proto_type;
	uint32_t tmp;
	if (emaclite->rx_ctrl & XEL_RSR_RECV_DONE_MASK) {
		/* Get the protocol type of the ethernet frame that arrived */
		tmp = *(volatile uint32_t *) (emaclite->rx_pack + 0xC);
		proto_type = (tmp >> 0x10) & 0xFFFF;
		TRACE("proto = 0x%X\n", proto_type);

		/* Check if received ethernet frame is a raw ethernet frame
		 * or an IP packet or an ARP packet */
		switch (proto_type) {
		case ETH_P_IP: {
			length
					= ((*(volatile uint32_t *) (emaclite->rx_pack + 0x10))
							>> 16) & 0xFFFF;
			length += ETH_HLEN + ETH_FCS_LEN;
		}
		case ETH_P_ARP: {
			length = 28 + ETH_HLEN + ETH_FCS_LEN;
		}
		default: {
			/* Field contains type other than IP or ARP, use max
			 * frame size and let user parse it */
			length = 0x600;
		}
		}

		/* Read from the EmacLite device */

		/* Acknowledge the frame */
		emaclite->rx_ctrl &= ~XEL_RSR_RECV_DONE_MASK;
	}

#if 0
	netif_rx(pack);
#endif
}

/**
 * IRQ handler
 */
static void irq_handler() {
	if (emaclite->rx_ctrl & XEL_RSR_RECV_DONE_MASK) {
		pack_received();
	}
}

static int open(net_device_t *dev) {
	if (NULL == dev) {
		return 0;
	}

	/*
	 * TX - TX_PING & TX_PONG initialization
	 */
	/* Restart PING TX */
	emaclite->tx_len = 0;
	/* Copy MAC address */
#if 0
	/*default 00-00-5E-00-FA-CE*/
	set_mac_address(dev, dev->hw_addr);
#endif

#ifdef CONFIG_XILINX_EMACLITE_TX_PING_PONG
	/* The same operation with PONG TX */
	out_be32 (emaclite.baseaddress + XEL_TSR_OFFSET + XEL_BUFFER_OFFSET, 0);
	xemaclite_alignedwrite (dev->enetaddr, emaclite.baseaddress +
			XEL_BUFFER_OFFSET, ENET_ADDR_LENGTH);
	out_be32 (emaclite.baseaddress + XEL_TPLR_OFFSET, ENET_ADDR_LENGTH);
	out_be32 (emaclite.baseaddress + XEL_TSR_OFFSET + XEL_BUFFER_OFFSET,
			XEL_TSR_PROG_MAC_ADDR);
	while ((in_be32 (emaclite.baseaddress + XEL_TSR_OFFSET +
							XEL_BUFFER_OFFSET) & XEL_TSR_PROG_MAC_ADDR) != 0);
#endif

	/*
	 * RX - RX_PING & RX_PONG initialization
	 */
	/* Write out the value to flush the RX buffer */
	emaclite->rx_ctrl = XEL_RSR_RECV_IE_MASK;
#ifdef CONFIG_XILINX_EMACLITE_RX_PING_PONG
	out_be32 (emaclite.baseaddress + XEL_RSR_OFFSET + XEL_BUFFER_OFFSET,
			XEL_RSR_RECV_IE_MASK);
#endif

	return 0;
}

static int stop(net_device_t *dev) {
	if (NULL == dev) {
		return 0;
	}

	return 1;
}

static int set_mac_address(struct net_device *dev, void *addr) {
	if (NULL == dev || NULL == addr) {
		return -1;
	}
#if 0
	out_be32 (emaclite.baseaddress + XEL_TSR_OFFSET, 0);
	/* Set the length */
	out_be32 (emaclite.baseaddress + XEL_TPLR_OFFSET, ENET_ADDR_LENGTH);
	/* Update the MAC address in the EMAC Lite */
	out_be32 (emaclite.baseaddress + XEL_TSR_OFFSET, XEL_TSR_PROG_MAC_ADDR);
	/* Wait for EMAC Lite to finish with the MAC address update */
	while ((in_be32 (emaclite.baseaddress + XEL_TSR_OFFSET) &
					XEL_TSR_PROG_MAC_ADDR) != 0);
#endif
	return 0;
}

/*
 * Get RX/TX stats
 */
static net_device_stats_t *get_eth_stat(net_device_t *dev) {
	return NULL;
}

static const struct net_device_ops _netdev_ops = {
		.ndo_start_xmit = start_xmit, .ndo_open = open, .ndo_stop = stop,
		.ndo_get_stats = get_eth_stat, .ndo_set_mac_address = set_mac_address };

static int module_init() {
	/*if some module lock irq number we break initializing*/

	net_device_t *net_device;
	/*initialize net_device structures and save information about them to local massive*/
	if (NULL != (net_device = alloc_etherdev(0))) {
		net_device->netdev_ops = &_netdev_ops;
		net_device->irq = XILINX_EMACLITE_IRQ_NUM;
		net_device->base_addr = XILINX_EMACLITE_BASEADDR;
	}
	if (-1 == request_irq(XILINX_EMACLITE_IRQ_NUM, irq_handler, 0, "xilinx emaclite", net_device )) {
		return -1;
	}

}

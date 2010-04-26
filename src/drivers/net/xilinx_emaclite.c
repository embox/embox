/**
 * @file
 * @brief Emaclite driver.
 *
 * @date 18.12.2009
 * @author Anton Bondarev
 */

#include <types.h>
#include <string.h>

#include <asm/cpu_conf.h>
#include <kernel/irq.h>
#include <net/if_ether.h>
#include <net/skbuff.h>
#include <net/netdevice.h>
#include <net/net.h>
#include <net/net_pack_manager.h>
#include <net/etherdevice.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(unit_init);

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

/* Global Interrupt Enable Register (GIER) Bit Masks */
#define XEL_GIER_GIE_MASK	0x80000000 	/* Global Enable */

/* Transmit Packet Length Register (TPLR) */
#define XEL_TPLR_LENGTH_MASK	0x0000FFFF 	/* Tx packet length */

typedef struct mdio_regs {
	uint32_t regs;
} mdio_regs_t;

typedef struct pingpong_regs {
	uint8_t pack[0x07F0];
	mdio_regs_t mdio_regs;
	uint32_t len; /*0x07F4*/
	uint32_t gie;
	uint32_t ctrl;
} pingpong_regs_t;

typedef struct xilinx_emaclite_regs {
	pingpong_regs_t tx_ping;
	pingpong_regs_t tx_pong;
	pingpong_regs_t rx_ping;
	pingpong_regs_t rx_pong;
} xilinx_emaclite_regs_t;

static struct xilinx_emaclite_regs *emaclite =
		(struct xilinx_emaclite_regs *) XILINX_EMACLITE_BASEADDR;
static pingpong_regs_t *current_rx_regs = NULL;
static pingpong_regs_t *current_tx_regs = NULL;

#define GIE_REG      (&emaclite->tx_ping)->gie
#define RX_PACK      ((uint8_t *)current_rx_regs->pack)
#define TX_PACK      ((uint8_t *)current_tx_regs->pack)
#define TX_LEN_REG   current_tx_regs->len
#define TX_CTRL_REG  current_tx_regs->ctrl
#define RX_CTRL_REG  current_rx_regs->ctrl

static void switch_rx_buff(void) {
#ifdef PINPONG_BUFFER
	if (current_rx_regs == emaclite->rx_ping) {
		current_rx_regs = emaclite->rx_ping;
	}
	else {
		current_rx_regs = emaclite->rx_pong;
	}
#else
	current_rx_regs = &emaclite->rx_ping;
#endif
}

static void switch_tx_buff(void) {
#ifdef PINPONG_BUFFER
	if (current_tx_regs == emaclite->tx_ping) {
		current_tx_regs = emaclite->tx_ping;
	}
	else {
		current_tx_regs = emaclite->tx_pong;
	}
#else
	current_tx_regs = &emaclite->tx_ping;
#endif
}

static void restart_buff(void) {
	switch_tx_buff();
	TX_LEN_REG = 0;
	switch_tx_buff();
	TX_LEN_REG = 0;
}

static pingpong_regs_t *get_rx_buff(void) {
	if (current_rx_regs->ctrl & XEL_RSR_RECV_DONE_MASK) {
		return current_rx_regs;
	}
	switch_rx_buff();
	if (current_rx_regs->ctrl & XEL_RSR_RECV_DONE_MASK) {
		return current_rx_regs;
	}
	return NULL;
}
#if 0
static uint8_t etherrxbuff[PKTSIZE]; /* Receive buffer */
#endif
/*FIXME bad function (may be use if dest and src align 4)*/
static void memcpy32(volatile uint32_t *dest, void *src, size_t len) {
	size_t lenw = (size_t) ((len & (~3)) >> 2);
	volatile uint32_t *srcw = (uint32_t*) ((uint32_t) (src) & (~3));

	while (lenw--) {
		*dest++ = *srcw++;
	}
	if (len & (~3)) {
		*dest++ = *srcw++;
	}
}

/**
 * Send a packet on this device.
 */
static int start_xmit(struct sk_buff *skb, struct net_device *dev) {
	if ((NULL == skb) || (NULL == dev)) {
		return -1;
	}

	if (0 != (TX_CTRL_REG & XEL_TSR_XMIT_BUSY_MASK)) {
		switch_tx_buff();
		if (0 != (TX_CTRL_REG & XEL_TSR_XMIT_BUSY_MASK)) {
			return -1; /*transmitter is busy*/
		}
	}
	memcpy32((uint32_t*) TX_PACK, skb->data, skb->len);
	TX_LEN_REG = skb->len & XEL_TPLR_LENGTH_MASK;
	TX_CTRL_REG |= XEL_TSR_XMIT_BUSY_MASK;

	return skb->len;
}

/**
 *
 */
static void pack_receiving(void *dev_id) {
	uint16_t len, proto_type;
	uint32_t tmp;
	sk_buff_t *skb;

	/* Get the protocol type of the ethernet frame that arrived */
	tmp = *(volatile uint32_t *) (RX_PACK + 0xC);
	proto_type = (tmp >> 0x10) & 0xFFFF;

	/* Check if received ethernet frame is a raw ethernet frame
	 * or an IP packet or an ARP packet */
	switch (proto_type) {
	case ETH_P_IP: {
		len = (((*(volatile uint32_t *) (RX_PACK + 0x10))) >> 16) & 0xFFFF;
		len += ETH_HLEN + ETH_FCS_LEN;
		break;
	}
	case ETH_P_ARP: {
		len = 28 + ETH_HLEN + ETH_FCS_LEN;
		break;
	}
	default: {
		/* Field contains type other than IP or ARP, use max
		 * frame size and let user parse it */
		len = ETH_FRAME_LEN;
		break;
	}
	}

	/* Read from the EmacLite device */

	skb = alloc_skb(len + 4, 0);
	if (NULL == skb) {
		LOG_ERROR("Can't allocate packet, pack_pool is full\n");
		current_rx_regs->ctrl &= ~XEL_RSR_RECV_DONE_MASK;
		switch_rx_buff();
		return;
	}

	memcpy32((uint32_t *) skb->data, RX_PACK, (size_t) len);
	/* Acknowledge the frame */
	current_rx_regs->ctrl &= ~XEL_RSR_RECV_DONE_MASK;
	switch_rx_buff();

	skb->mac.ethh = (ethhdr_t *) skb->data;
	skb->protocol = skb->mac.ethh->h_proto;

	skb->dev = dev_id;
	netif_rx(skb);
}

/**
 * IRQ handler
 */
static irq_return_t irq_handler(irq_nr_t irq_num, void *dev_id) {
	if (NULL != get_rx_buff()) {
		pack_receiving(dev_id);
	}
	return 0;
}
/*default 00-00-5E-00-FA-CE*/
const unsigned char default_mac[ETH_ALEN] = { 0x00, 0x00, 0x5E, 0x00, 0xFA,
		0xCE };

static int open(net_device_t *dev) {
	if (NULL == dev) {
		return -1;
	}

	current_rx_regs = &emaclite->rx_ping;
	current_tx_regs = &emaclite->tx_ping;
	/*
	 * TX - TX_PING & TX_PONG initialization
	 */
	/* Restart PING TX */

	restart_buff();
	/* Copy MAC address */
	memcpy(dev->dev_addr, default_mac, ETH_ALEN);
#if 0
	/*default 00-00-5E-00-FA-CE*/
	set_mac_address(dev, dev->hw_addr);
#endif

	/*
	 * RX - RX_PING & RX_PONG initialization
	 */
	TRACE("emaclite->rx_ctrl addr = 0x%X\n", (unsigned int)&RX_CTRL_REG);
	RX_CTRL_REG = XEL_RSR_RECV_IE_MASK;
#ifdef PINPONG_BUFFER
	switch_rx_buff();
	TRACE("emaclite->rx_ctrl addr = 0x%X\n", &RX_CTRL_REG);
	RX_CTRL_REG = XEL_RSR_RECV_IE_MASK;
	switch_rx_buff();
#endif

	GIE_REG = XEL_GIER_GIE_MASK;
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

static int unit_init() {
	/*if some module lock irq number we break initializing*/
	net_device_t *net_device;
	/*initialize net_device structures and save information about them to local massive*/
	if (NULL != (net_device = alloc_etherdev(0))) {
		net_device->netdev_ops = &_netdev_ops;
		net_device->irq = XILINX_EMACLITE_IRQ_NUM;
		net_device->base_addr = XILINX_EMACLITE_BASEADDR;
	}

	if (-1 == irq_attach(XILINX_EMACLITE_IRQ_NUM, irq_handler, 0,
			net_device, "xilinx emaclite")) {
		return -1;
	}
	return 0;
}

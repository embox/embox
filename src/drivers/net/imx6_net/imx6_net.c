/**
 * @file imx6_net.c
 * @brief iMX6 MAC-NET driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-05-11
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include <hal/reg.h>

#include <kernel/irq.h>
#include <kernel/printk.h>

#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/skbuff.h>

#include <util/log.h>

#include <embox/unit.h>

#include <framework/mod/options.h>

#include "imx6_net.h"

#if 0
static void _reg_dump(void) {
	log_debug("ENET_EIR  %10x", REG32_LOAD(ENET_EIR ));
	log_debug("ENET_EIMR %10x", REG32_LOAD(ENET_EIMR));
	log_debug("ENET_RDAR %10x", REG32_LOAD(ENET_RDAR));
	log_debug("ENET_TDAR %10x", REG32_LOAD(ENET_TDAR));
	log_debug("ENET_ECR  %10x", REG32_LOAD(ENET_ECR ));
	log_debug("ENET_MSCR %10x", REG32_LOAD(ENET_MSCR));
	log_debug("ENET_RCR  %10x", REG32_LOAD(ENET_RCR ));
	log_debug("ENET_TCR  %10x", REG32_LOAD(ENET_TCR ));
	log_debug("MAC_LOW   %10x", REG32_LOAD(MAC_LOW  ));
	log_debug("MAC_HI    %10x", REG32_LOAD(MAC_HI   ));
	log_debug("ENET_IAUR %10x", REG32_LOAD(ENET_IAUR));
	log_debug("ENET_IALR %10x", REG32_LOAD(ENET_IALR));
	log_debug("ENET_GAUR %10x", REG32_LOAD(ENET_GAUR));
	log_debug("ENET_GALR %10x", REG32_LOAD(ENET_GALR));
	log_debug("ENET_TFWR %10x", REG32_LOAD(ENET_TFWR));
	log_debug("ENET_RDSR %10x", REG32_LOAD(ENET_RDSR));
	log_debug("ENET_TDSR %10x", REG32_LOAD(ENET_TDSR));
	log_debug("ENET_MRBR %10x", REG32_LOAD(ENET_MRBR));
}
#endif
static uint32_t _uboot_regs[128];
static void _mem_dump(void) {
	for (int i = 0; i < 128; i++) {
		_uboot_regs[i] = REG32_LOAD(NIC_BASE + i * 4);
	}
}

static void _mem_restore(void) {
	for (int i = 0; i < 128; i++) {
		REG32_STORE(NIC_BASE + i * 4,_uboot_regs[i]);
	}
}

static uint8_t _macaddr[6];

static void _write_macaddr(void) {
	uint32_t mac_hi, mac_lo;

	mac_hi  = (_macaddr[5] << 16) |
	          (_macaddr[4] << 24);
	mac_lo  = (_macaddr[3] <<  0) |
	          (_macaddr[2] <<  8) |
	          (_macaddr[1] << 16) |
	          (_macaddr[0] << 24);

	REG32_STORE(MAC_LOW, mac_lo);
	REG32_STORE(MAC_HI, mac_hi);
}

static struct imx6_buf_desc _tx_desc_ring[TX_BUF_FRAMES];
static struct imx6_buf_desc _rx_desc_ring[RX_BUF_FRAMES];

static uint8_t _tx_buf[TX_BUF_LEN] __attribute__ ((aligned(0x10000)));
static uint8_t _rx_buf[RX_BUF_LEN] __attribute__ ((aligned(0x10000)));

extern void dcache_inval(const void *p, size_t size);
extern void dcache_flush(const void *p, size_t size);

#define dcache_flush(x, y) ;
#define dcache_inval(x, y) ;

static int imx6_net_xmit(struct net_device *dev, struct sk_buff *skb) {
	uint8_t *data;
	static int desc_num = 0;
	struct imx6_buf_desc *desc;

	log_debug("Transmitting packet %2d", desc_num);

	ipl_t sp;

	assert(dev);
	assert(skb);

	sp = ipl_save();
	{
		REG32_STORE(ENET_TCR, (1 << 2));
		data = (uint8_t*) skb_data_cast_in(skb->data);

		if (!data) {
			log_error("No skb data!\n");
			ipl_restore(sp);
			return -1;
		}
		memcpy(&_tx_buf[desc_num], data, skb->len);
		dcache_flush(&_tx_buf[desc_num], skb->len);

		skb_free(skb);

		desc = &_tx_desc_ring[desc_num];

		desc->data_pointer = (uint32_t) &_tx_buf[desc_num];
		desc->len          = skb->len;
		desc->flags1       = FLAG_R | FLAG_L | FLAG_TC;

		if (desc_num == TX_BUF_FRAMES - 1)
			desc->flags1 |= FLAG_W;

		dcache_flush(desc, sizeof(struct imx6_buf_desc));

		REG32_STORE(ENET_TDAR, 0x01000000);

		int timeout = 0xFF;
		while(timeout--) {
			if (!(REG32_LOAD(ENET_TDAR) & 0x01000000))
				break;
		}

		if (timeout == 0)
			log_debug("TX timeout...");

		REG32_STORE(ENET_TCR, (1 << 2) | 1);
	}
	ipl_restore(sp);

	desc_num = (desc_num + 1) % TX_BUF_FRAMES;

	return 0;
}

static void _init_buffers() {
	struct imx6_buf_desc *desc;

	memset(&_tx_desc_ring[0], 0,
	        TX_BUF_FRAMES * sizeof(struct imx6_buf_desc));
	memset(&_rx_desc_ring[0], 0,
	        RX_BUF_FRAMES * sizeof(struct imx6_buf_desc));

	/* Mark last buffer (i.e. set wrap flag) */
	_rx_desc_ring[RX_BUF_FRAMES - 1].flags1 = FLAG_W;
	_tx_desc_ring[TX_BUF_FRAMES - 1].flags1 = FLAG_W;

	for (int i = 0; i < RX_BUF_FRAMES; i++) {
		desc = &_rx_desc_ring[i];
		desc->data_pointer = (uint32_t) _rx_buf[i * FRAME_LEN];
		desc->flags1 |= FLAG_E;
	}

	assert((((uint32_t) &_rx_desc_ring[0]) & 0xF) == 0);
	REG32_STORE(ENET_RDSR, ((uint32_t) &_rx_desc_ring[0]));

	assert((((uint32_t) &_tx_desc_ring[0]) & 0xF) == 0);
	REG32_STORE(ENET_TDSR, ((uint32_t) &_tx_desc_ring[0]));

	assert((RX_BUF_LEN & 0xF) == 0);
	REG32_STORE(ENET_MRBR, (FRAME_LEN - 1));

	dcache_flush(&_tx_desc_ring[0],
	              TX_BUF_FRAMES * sizeof(struct imx6_buf_desc));
	dcache_flush(&_rx_desc_ring[0],
	              RX_BUF_FRAMES * sizeof(struct imx6_buf_desc));
}

#if 0
static void _reg_setup(void) {
	uint32_t t;

	/* Disable IRQ */
	REG32_STORE(ENET_EIMR, 0);

	/* Clear pending interrupts */
	//REG32_STORE(ENET_EIR, EIR_MASK);

	/* Setup RX */
	t  = (FRAME_LEN - 1) << FRAME_LEN_OFFSET;
	t |= RCR_FCE | RCR_MII_MODE;
	REG32_STORE(ENET_RCR, t);

	_setup_mii_speed();

	/* Enable IRQ */
	REG32_STORE(ENET_EIMR, 0x7FFF0000);
}
#endif

static void _reset(void) {
	log_debug("ENET reset...\n");
	_mem_dump();
	REG32_STORE(ENET_ECR, RESET);
	_mem_restore();
	_init_buffers();

	REG32_STORE(ENET_EIMR, 0x7FFF0000);
	REG32_STORE(ENET_EIR, 0x7FFF0000);
	REG32_STORE(ENET_RDAR, (1 << 24));
	REG32_STORE(ENET_TCR, (1 << 2));

	uint32_t t = REG32_LOAD(ENET_ECR);
	t |= ETHEREN;
	REG32_STORE(ENET_ECR, t); /* Note: should be last ENET-related init step */

#if 0
	int tmp = 10000;


	REG32_STORE(ENET_ECR, RESET);
	while(tmp--);

	_write_macaddr();

	//REG32_STORE(ENET_EIR, 0xFFC00000);

	REG32_STORE(ENET_IAUR, 0);
	REG32_STORE(ENET_IALR, 0);
	REG32_STORE(ENET_GAUR, 0);
	REG32_STORE(ENET_GALR, 0);

	assert((RX_BUF_LEN & 0xF) == 0);
	REG32_STORE(ENET_MRBR, RX_BUF_LEN);

	assert((((uint32_t) &_rx_desc_ring[0]) & 0xF) == 0);
	REG32_STORE(ENET_RDSR, ((uint32_t) &_rx_desc_ring[0]));

	assert((((uint32_t) &_tx_desc_ring[0]) & 0xF) == 0);
	REG32_STORE(ENET_TDSR, ((uint32_t) &_tx_desc_ring[0]));

	/* dirty_tx, cur_tx, cur_rx */

	for (int i = 0; i < TX_BUF_FRAMES; i++)
		memset(&_tx_desc_ring[i], 0, sizeof(_tx_desc_ring[i]));

	REG32_STORE(ENET_TCR, (1 << 2) | 1);

	//REG32_STORE(ENET_MSCR, 0x8); /* TODO fix? */
	_setup_mii_speed();

	uint32_t rcntl = 0x40000000 | 0x00000020;
	rcntl &= ~(1 << 8);
	/* 10 mbps */
	rcntl |= (1 << 9);
	REG32_STORE(ENET_RCR, rcntl);

	uint32_t ecntl = 0x2 | (1 << 8);
	REG32_STORE(ENET_TFWR, (1 << 8));

	REG32_STORE(ENET_ECR, ecntl);

	REG32_STORE(ENET_RDAR, 0);

	REG32_STORE(ENET_EIMR, 0x0A800000);

	return;

	_reg_dump();
	_reg_setup();


	/* TODO set FEC clock? */


	_init_buffers();

	REG32_STORE(ENET_EIMR, EIMR_TXF | EIMR_RXF | EIR_MASK);

	_write_macaddr();

	REG32_STORE(ENET_TFWR, (1 << 8) | 0x1);
	REG32_STORE(ENET_ECR, ETHEREN | ECR_DBSWP); /* Note: should be last init step */
#endif
}

static int imx6_net_open(struct net_device *dev) {
	return 0;
}

static int imx6_net_set_macaddr(struct net_device *dev, const void *addr) {
	assert(dev);
	assert(addr);

	memcpy(&_macaddr[0], (uint8_t *) addr, 6);
	_write_macaddr();

	return 0;
}

static irq_return_t imx6_irq_handler(unsigned int irq_num, void *dev_id) {
	uint32_t state;

	state = REG32_LOAD(ENET_EIR);

	log_debug("Interrupt mask %#08x", state);

	REG32_STORE(ENET_EIR, state);

	if (state & EIR_EBERR) {
		log_error("Ethernet bus error, resetting ENET!");
		REG32_STORE(ENET_ECR, RESET);
		_reset();

		return IRQ_HANDLED;
	}

	if (state & EIR_RXB) {
		log_debug("RX interrupt");
	}

	if (state & (EIR_TXB | EIR_TXF)) {
		log_debug("finished TX");
	}

	for (int id = 0; id < TX_BUF_FRAMES; id++) {
		dcache_inval(&_tx_desc_ring[id], sizeof(struct imx6_buf_desc));
		if (_tx_desc_ring[id].flags1 != 0x8c00 && _tx_desc_ring[id].flags1 != 0x0 && _tx_desc_ring[id].flags1 != 0x2000 && _tx_desc_ring[id].flags1 != 0xac00)
			log_debug("Frame %2d status %#06x", id, _tx_desc_ring[id].flags1);
	}

	return IRQ_HANDLED;
}

static const struct net_driver imx6_net_drv_ops = {
	.xmit = imx6_net_xmit,
	.start = imx6_net_open,
	.set_macaddr = imx6_net_set_macaddr
};

EMBOX_UNIT_INIT(imx6_net_init);
static int imx6_net_init(void) {
	struct net_device *nic;
	int tmp;

	if (NULL == (nic = etherdev_alloc(0))) {
                return -ENOMEM;
        }

	nic->drv_ops = &imx6_net_drv_ops;

	tmp = irq_attach(ENET_IRQ, imx6_irq_handler, 0, nic, "i.MX6 enet");
	if (tmp)
		return tmp;

	_reset();

	return inetdev_register_dev(nic);
}

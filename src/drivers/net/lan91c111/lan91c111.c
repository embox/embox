/**
 * @file
 *
 * @date 09.04.2016
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <sys/mman.h>

#include <util/log.h>
#include <drivers/common/memory.h>

#include <kernel/irq.h>
#include <mem/misc/pool.h>
#include <hal/reg.h>

#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <net/util/show_packet.h>

#include "lan91c111.h"

#include <embox/unit.h>

EMBOX_UNIT_INIT(lan91c111_init);

/* Internal I/O space mapping */

#define BANK_BASE_ADDR OPTION_GET(NUMBER,base_addr)

struct smc_local {
	struct sk_buff_head pending_queue;
};

/**
 * @brief Set active bank ID
 */
static void lan91c111_set_bank(int n) {
	static int cur_bank = -1;

	assert(0 <= n && n <= 3);

	if (cur_bank != n) {
		REG16_STORE(BANK_BANK, (uint16_t) n);
		cur_bank = n;
	}
}

/**
 * @brief Set appropriate opcode
 */
static void lan91c111_set_cmd(int opcode) {
	assert(0 <= opcode && opcode <= 7);

	lan91c111_set_bank(2);
	if (opcode == CMD_RX_POP_AND_RELEASE) {
		/* MMU is busy by another release cmd */
		while (REG16_LOAD(BANK_MMU_CMD) & MMU_BUSY) { }
	}

	REG16_STORE(BANK_MMU_CMD, opcode << 5);
}

static void lan91c111_push_data(uint8_t *data, int len) {
	int i;

	for (i = 0; i < len >> 2; i++) {
		REG32_STORE(BANK_DATA, *((uint32_t*)data));
		data += 4;
	}

	for (i = 0; i < len % 4; i++) {
		REG8_STORE(BANK_DATA, *data);
		data++;
	}
}

static int lan91c111_send_hw(struct sk_buff *skb) {
	uint8_t packet_num;
	uint16_t packet_len;
	uint8_t *data;

	lan91c111_set_bank(2);

	packet_num = REG8_LOAD(BANK_TX_ALLOC) & PNUM_MASK;

	REG8_STORE(BANK_PNR, packet_num);

	/* Write header */
	REG16_STORE(BANK_POINTER, AUTO_INCR | LAN91C111_STATUS_BYTES);
	packet_len = (uint16_t) skb->len;
	packet_len += LAN91C111_STATUS_BYTES +
		sizeof(packet_len) + LAN91C111_CTL_BYTES;
	packet_len &= PLEN_MASK;
	REG16_STORE(BANK_DATA, packet_len);

	data = (uint8_t *)skb_data_cast_in(skb->data);
	lan91c111_push_data(data, skb->len);

	if (skb->len % 2) {
		REG8_STORE(BANK_DATA, ODD_CONTROL);
	} else {
		REG16_STORE(BANK_DATA, 0x0);
	}

	lan91c111_set_cmd(CMD_TX_ENQUEUE);

	return 0;
}

static int lan91c111_add_pending(struct smc_local *smc, struct sk_buff *skb) {
	uint8_t int_mask;

	lan91c111_set_bank(2);

	skb_queue_push(&smc->pending_queue, skb);

	int_mask = REG8_LOAD(BANK_INTERRUPT_MASK);
	int_mask |= ALLOC_INT;
	REG8_STORE(BANK_INTERRUPT_MASK, int_mask);

	return 0;
}

static int lan91c111_tx_try(struct smc_local *smc, struct sk_buff *skb) {
	lan91c111_set_cmd(CMD_TX_ALLOC);

	lan91c111_set_bank(2);

	if (!(REG8_LOAD(BANK_INTERRUPT_STATUS) & ALLOC_MASK)) {
		lan91c111_add_pending(smc, skb);
		return -EBUSY;
	}

	lan91c111_send_hw(skb);

	return 0;
}

static int lan91c111_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct smc_local *smc;
	int ret = 0;
	ipl_t ipl;

	smc = netdev_priv(dev);

	ipl = ipl_save();
	{
		if (skb_queue_front(&smc->pending_queue)) {
			lan91c111_add_pending(smc, skb);
			goto out;
		}

		ret = lan91c111_tx_try(smc, skb);
		if (ret) {
			goto out;
		}

		skb_free(skb);
	}
out:
	ipl_restore(ipl);

	return 0;
}

static int lan91c111_open(struct net_device *dev) {
	lan91c111_set_bank(0);
	REG16_STORE(BANK_RCR, RX_EN);
	REG16_STORE(BANK_TCR, TX_EN);

	lan91c111_set_bank(2);
	REG8_STORE(BANK_INTERRUPT_MASK, RX_INT);

	return 0;
}

static int lan91c111_set_macaddr(struct net_device *dev, const void *addr) {
	uint8_t *mac = (uint8_t*) addr;
	uint16_t mac_hi, mac_mid, mac_lo;

	assert(mac);

	mac_hi = (mac[5] << 8) | (mac[4] << 0);
	mac_mid = (mac[3] << 8) | (mac[2] << 0);
	mac_lo = (mac[1] << 8) | (mac[0] << 0);

	lan91c111_set_bank(1);

	REG16_STORE(BANK_IA01, mac_lo);
	REG16_STORE(BANK_IA23, mac_mid);
	REG16_STORE(BANK_IA45, mac_hi);

	return 0;
}

static const struct net_driver lan91c111_drv_ops = {
	.xmit = lan91c111_xmit,
	.start = lan91c111_open,
	.set_macaddr = lan91c111_set_macaddr
};

static int lan91c111_tx_pend(struct net_device *dev) {
	struct sk_buff *skb;
	struct smc_local *smc;
	int ret;

	smc = netdev_priv(dev);

	while (NULL != (skb = skb_queue_pop(&smc->pending_queue))) {
		ret = lan91c111_send_hw(skb);
		if (ret) {
			return ret;
		}
		skb_free(skb);
	}

	return 0;
}

static int lan91c111_recv_hw(struct net_device *dev_id) {
	int i, packet;
	struct sk_buff *skb;
	uint32_t buf;
	uint16_t len;
	uint8_t *skb_data;

	packet = (REG16_LOAD(BANK_FIFO_PORTS) >> 8) & 0xFF;
	if (packet == FIFO_EMPTY) {
		return 0;
	}

	REG16_STORE(BANK_POINTER,
			RX_FIFO_PACKET | AUTO_INCR | LAN91C111_STATUS_BYTES);
	len = (REG16_LOAD(BANK_DATA) & PLEN_MASK) - 10;
	/* In original structure, byte count includes headers, so
	 * we shrink it to data size */

	skb = skb_alloc(len + LAN91C111_STATUS_BYTES);
	assert(skb);
	skb->len = len;
	skb->dev = dev_id;

	skb_data = skb_data_cast_in(skb->data);
	assert(skb_data);

	for (i = 0; i < len >> 2; i++) {
		buf = REG32_LOAD(BANK_DATA);
		*((uint32_t *)(skb_data + i * 4)) = buf;
	}

	if (len % 4 == 2) {
		buf = REG16_LOAD(BANK_DATA);
		*((uint16_t *)(skb_data + i * 4)) = buf & 0xFFFF;
	}

	/* Skip 4 bytes CRC */
	buf = REG32_LOAD(BANK_DATA);
	buf = REG16_LOAD(BANK_DATA);

	lan91c111_set_cmd(CMD_RX_POP_AND_RELEASE);

	if (buf & (ODD_CONTROL << 8)) {
		skb->len++;
		skb_data[skb->len -1] = (uint8_t)(buf & 0xFF);
	}
	show_packet(skb_data, len, "rx_pack");

	netif_rx(skb);

	return 0;
}

static irq_return_t lan91c111_int_handler(unsigned int irq_num, void *dev_id) {
	uint8_t int_status;
	uint8_t int_mask;

	ipl_t ipl;

	ipl = ipl_save();
	{
		lan91c111_set_bank(2);

		int_status = REG8_LOAD(BANK_INTERRUPT_STATUS);
		int_mask = REG8_LOAD(BANK_INTERRUPT_MASK);

		int_status &= int_mask;

		if (int_status & RX_MASK) {
			lan91c111_recv_hw(dev_id);
		}

		if (int_status & ALLOC_MASK) {
			int_mask &= ~ALLOC_INT;
			REG8_STORE(BANK_INTERRUPT_MASK, int_mask);
			lan91c111_tx_pend(dev_id);
		}

	}
	ipl_restore(ipl);

	return 0;
}

static int lan91c111_init(void) {
	struct net_device *nic;
	struct smc_local *smc_local;

	if (NULL == (nic = etherdev_alloc(sizeof(struct smc_local)))) {
		return -ENOMEM;
	}

	irq_attach(LAN91C111_IRQ, lan91c111_int_handler, 0, nic, "lan91c111");

	smc_local = netdev_priv(nic);
	skb_queue_init(&smc_local->pending_queue);

	nic->drv_ops = &lan91c111_drv_ops;

	lan91c111_set_bank(1);
	REG16_STORE(BANK_CONTROL, 0x0800);

	return inetdev_register_dev(nic);
}

PERIPH_MEMORY_DEFINE(lan91c111, BANK_BASE_ADDR, 0x10);

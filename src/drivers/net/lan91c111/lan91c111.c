/**
 * @file
 *
 * @data 09.04.2016
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <sys/mman.h>

#include <util/log.h>
#include <drivers/common/memory.h>
#include <hal/reg.h>
#include <kernel/printk.h>
#include <kernel/irq.h>
#include <mem/misc/pool.h>

#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <net/util/show_packet.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(lan91c111_init);

/* Internal I/O space mapping */

#define BANK_BASE_ADDR 0xC8000000

/* Bank 0 */
#define BANK_TCR        (BANK_BASE_ADDR + 0x0)
#define BANK_EPH_STATUS (BANK_BASE_ADDR + 0x2)
#define BANK_RCR        (BANK_BASE_ADDR + 0x4)
#define BANK_COUNTER    (BANK_BASE_ADDR + 0x6)
#define BANK_MIR        (BANK_BASE_ADDR + 0x8)
#define BANK_RPCR       (BANK_BASE_ADDR + 0xA)
/*      BANK_RESERVED   (BANK_BASE_ADDR + 0xC) */
#define BANK_BANK       (BANK_BASE_ADDR + 0xE) /* Common for all banks */

/* Bank 1 */
#define BANK_CONFIG          (BANK_BASE_ADDR + 0x0)
#define BANK_BASE            (BANK_BASE_ADDR + 0x2)
#define BANK_IA01            (BANK_BASE_ADDR + 0x4)
#define BANK_IA23            (BANK_BASE_ADDR + 0x6)
#define BANK_IA45            (BANK_BASE_ADDR + 0x8)
#define BANK_GENERAL_PURPOSE (BANK_BASE_ADDR + 0xA)
#define BANK_CONTROL         (BANK_BASE_ADDR + 0xC)
/*      BANK_BANK            (BANK_BASE_ADDR + 0xE) -- already defined above */

/* Bank 2 */
#define BANK_MMU_CMD           (BANK_BASE_ADDR + 0x0)
#define BANK_PNR               (BANK_BASE_ADDR + 0x2)
#define BANK_TX_ALLOC          (BANK_BASE_ADDR + 0x3)
#define BANK_FIFO_PORTS        (BANK_BASE_ADDR + 0x4)
#define BANK_POINTER           (BANK_BASE_ADDR + 0x6)
#define BANK_DATA              (BANK_BASE_ADDR + 0x8)
/*      BANK_DATA              (BANK_BASE_ADDR + 0xA) -- another 2 bytes */
#define BANK_INTERRUPT_STATUS  (BANK_BASE_ADDR + 0xC)
#define BANK_INTERRUPT_MASK    (BANK_BASE_ADDR + 0xD)
/*      BANK_BANK       (BANK_BASE_ADDR + 0xE) -- already defined above */

/* Bank 3 */
#define BANK_MT01     (BANK_BASE_ADDR + 0x0)
#define BANK_MT23     (BANK_BASE_ADDR + 0x2)
#define BANK_MT45     (BANK_BASE_ADDR + 0x4)
#define BANK_MT67     (BANK_BASE_ADDR + 0x6)
#define BANK_MGMT     (BANK_BASE_ADDR + 0x8)
#define BANK_REVISION (BANK_BASE_ADDR + 0xA)
#define BANK_RCV      (BANK_BASE_ADDR + 0xC)
/*      BANK_BANK     (BANK_BASE_ADDR + 0xE) -- already defined above */

/* Commands */
#define CMD_NOP                0
#define CMD_TX_ALLOC           1
#define CMD_RESET_MMU          2
#define CMD_RX_POP             3
#define CMD_RX_POP_AND_RELEASE 4
#define CMD_PACKET_FREE        5
#define CMD_TX_ENQUEUE         6
#define CMD_TX_FIFO_RESET      7

/* Various flags */
#define MMU_BUSY 0x0001
#define RX_EN    0x0100
#define TX_EN    0x0001

#define RX_INT   0x01
#define TX_INT   0x02

#define FIFO_EMPTY 0x80

#define ALLOC_MASK  0x08
#define TX_MASK     0x02
#define RX_MASK     0x01

#define CRC_CONTROL   0x10
#define ODD_CONTROL   0x20

#define LAN91C111_FRAME_SIZE_MAX 2048
#define LAN91C111_IRQ            27

#define PNUM_MASK 0x3F
#define PLEN_MASK 0x7FF

#define RX_FIFO_PACKET 0x8000
#define AUTO_INCR      0x4000

#define LAN91C111_STATUS_BYTES  2
#define LAN91C111_CTL_BYTES     2

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

static inline void lan91c111_regdump(void) {
	int i;
	for (i = 0; i <= 3; i++) {
		lan91c111_set_bank(i);
		printk("Bank %d\n", i);
		for (int j = 0; j < 7; j++) {
			printk("%4x\n", (uint16_t) REG16_LOAD(BANK_BASE_ADDR + 2 * j));
		}
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

static int lan91c111_xmit(struct net_device *dev, struct sk_buff *skb) {
	uint8_t packet_num;
	uint8_t *data;
	int ret = 0;
	uint16_t packet_len;
	ipl_t ipl;

	ipl = ipl_save();
	{
		lan91c111_set_cmd(CMD_TX_ALLOC);

		lan91c111_set_bank(2);

		if (!(REG8_LOAD(BANK_INTERRUPT_STATUS) & ALLOC_MASK)) {
			log_error("Failed to allocate packet for TX");
			ret = -EBUSY;
			goto out;
		}

		packet_num = REG8_LOAD(BANK_TX_ALLOC) & PNUM_MASK;

		REG8_STORE(BANK_PNR, packet_num);

		/* Write header */
		REG16_STORE(BANK_POINTER, AUTO_INCR | LAN91C111_STATUS_BYTES);
		packet_len = (uint16_t) skb->len;
		packet_len += LAN91C111_STATUS_BYTES +
			sizeof(packet_len) + LAN91C111_CTL_BYTES;
		packet_len &= PLEN_MASK;
		REG16_STORE(BANK_DATA, packet_len);

		data = (uint8_t*) skb_data_cast_in(skb->data);
		lan91c111_push_data(data, skb->len);

		if (skb->len % 2) {
			REG8_STORE(BANK_DATA, ODD_CONTROL);
		} else {
			REG16_STORE(BANK_DATA, 0x0);
		}

		lan91c111_set_cmd(CMD_TX_ENQUEUE);
	}
out:
	ipl_restore(ipl);

	if (ret == 0) {
		skb_free(skb);
	}

	return ret;
}

static int lan91c111_open(struct net_device *dev) {
	lan91c111_set_bank(0);
	REG16_STORE(BANK_RCR, RX_EN);
	REG16_STORE(BANK_TCR, TX_EN);

	lan91c111_set_bank(2);
	REG16_STORE(BANK_INTERRUPT_MASK, RX_INT | TX_INT);

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

static irq_return_t lan91c111_int_handler(unsigned int irq_num, void *dev_id) {
	struct sk_buff *skb;
	uint32_t buf;
	uint16_t len;
	uint8_t *skb_data;
	uint16_t int_status;
	int i, packet;
	ipl_t ipl;

	ipl = ipl_save();
	{
		lan91c111_set_bank(2);

		int_status = REG16_LOAD(BANK_INTERRUPT_STATUS);

		if (int_status & TX_INT) {
			/* Clear TX interrupt */
			int_status &= ~TX_INT;
			REG8_STORE(BANK_INTERRUPT_STATUS, int_status);
		}

		if (!(int_status & RX_INT)) {
			goto out;
		}

		packet = (REG16_LOAD(BANK_FIFO_PORTS) >> 8) & 0xFF;
		if (packet == FIFO_EMPTY) {
			goto out;
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
	}
out:
	ipl_restore(ipl);

	return 0;
}

static int lan91c111_init(void) {
	struct net_device *nic;

	if (NULL == (nic = etherdev_alloc(0))) {
		return -ENOMEM;
	}

	irq_attach(LAN91C111_IRQ, lan91c111_int_handler, 0, nic, "lan91c111");

	nic->drv_ops = &lan91c111_drv_ops;

	lan91c111_set_bank(1);
	REG16_STORE(BANK_CONTROL, 0x0800);

	return inetdev_register_dev(nic);
}

PERIPH_MEMORY_DEFINE(lan91c111, BANK_BASE_ADDR, 0x10);

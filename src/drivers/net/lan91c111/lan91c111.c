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
#define BANK_MMU_CMD    (BANK_BASE_ADDR + 0x0)
#define BANK_PNR        (BANK_BASE_ADDR + 0x2)
#define BANK_FIFO_PORTS (BANK_BASE_ADDR + 0x4)
#define BANK_POINTER    (BANK_BASE_ADDR + 0x6)
#define BANK_DATA       (BANK_BASE_ADDR + 0x8)
/*      BANK_DATA       (BANK_BASE_ADDR + 0xA) -- another 2 bytes */
#define BANK_INTERRUPT  (BANK_BASE_ADDR + 0xC)
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

#define REG32_LOAD(addr) \
	*((volatile uint32_t *)(addr))

#define REG32_STORE(addr, val) \
	do { *((volatile uint32_t *)(addr)) = (val); } while (0)

#define REG16_LOAD(addr) \
	*((volatile uint16_t *)(addr))

#define REG16_STORE(addr, val) \
	do { *((volatile uint16_t *)(addr)) = (val); } while (0)

#define REG8_LOAD(addr) \
	*((volatile uint8_t *)(addr))

#define REG8_STORE(addr, val) \
	do { *((volatile uint8_t *)(addr)) = (val); } while (0)

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

#define RX_INT   0x0100
#define TX_INT   0x0200
#define TX_ACK   0x0002

#define TX_EMPTY 0x80

#define TX_MASK  0x0002
#define RX_MASK  0x0001

#define CRC_CONTROL   0x10
#define ODD_CONTROL   0x20

#define LAN91C111_FRAME_SIZE_MAX 2048
#define LAN91C111_IRQ            27

#define PNUM_MASK 0x3F

#define AUTO_INCR 0x4000

/**
 * @brief Set active bank ID
 */
static void _set_bank(int n) {
	static int cur_bank = -1;

	assert(0 <= n && n <= 3);

	if (cur_bank != n) {
		REG16_STORE(BANK_BANK, (uint16_t) n);
		cur_bank = n;
	}
}
#if 0
static void _regdump(void) {
	for (int i = 0; i <= 3; i++) {
		_set_bank(i);
		printk("Bank %d\n", i);
		for (int j = 0; j < 7; j++) {
			printk("%4x\n", (uint16_t) REG16_LOAD(BANK_BASE_ADDR + 2 * j));
		}
	}
}
#endif

/**
 * @brief Set approprate opcode
 */
static void _set_cmd(int opcode) {
	assert(0 <= opcode && opcode <= 7);

	_set_bank(2);
	if (opcode == CMD_RX_POP_AND_RELEASE) {
		/* MMU is busy by another release cmd */
		while (REG16_LOAD(BANK_MMU_CMD) & MMU_BUSY) { }
	}

	REG16_STORE(BANK_MMU_CMD, opcode << 5);
}

static void _push_data(uint8_t *data, int len) {
	for (int i = 0; i < len >> 2; i++) {
		REG32_STORE(BANK_DATA, *((uint32_t*)data));
		data += 4;
	}

	for (int i = 0; i < len % 4; i++) {
		REG8_STORE(BANK_DATA, *data);
		data++;
	}
}

static int lan91c111_xmit(struct net_device *dev, struct sk_buff *skb) {
	uint16_t packet_num;
	uint8_t *data;

	_set_cmd(CMD_TX_ALLOC);

	_set_bank(2);

	packet_num = (REG16_LOAD(BANK_PNR) >> 8) & PNUM_MASK;

	REG16_STORE(BANK_PNR, packet_num << 8);

	/* Write header */
	REG16_STORE(BANK_POINTER, AUTO_INCR | 2);
	REG16_STORE(BANK_DATA, (uint16_t) (skb->len & 0xfffe) + 6);

	data = (uint8_t*) skb_data_cast_in(skb->data);
	_push_data(data, skb->len);

	if (skb->len % 2) {
		REG8_STORE(BANK_DATA, ODD_CONTROL);
	} else {
		REG16_STORE(BANK_DATA, 0x0);
	}

	_set_cmd(CMD_TX_ENQUEUE);

	skb_free(skb);
	return 0;
}

static int lan91c111_open(struct net_device *dev) {
        _set_bank(0);
	REG16_STORE(BANK_RCR, RX_EN);
	REG16_STORE(BANK_TCR, TX_EN);

	_set_bank(2);
	REG16_STORE(BANK_INTERRUPT, RX_INT);

	return 0;
}

static int lan91c111_set_macaddr(struct net_device *dev, const void *addr) {
        uint8_t *mac = (uint8_t*) addr;
	uint16_t mac_hi, mac_mid, mac_lo;

        assert(mac);

	mac_hi  = (mac[5] << 8) | (mac[4] << 0);
	mac_mid = (mac[3] << 8) | (mac[2] << 0);
	mac_lo  = (mac[1] << 8) | (mac[0] << 0);

        _set_bank(1);

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

static irq_return_t lan91c111_int_handler(unsigned int irq_num,
		void *dev_id) {
	struct sk_buff *skb;
	uint32_t buf;
	uint16_t len;
	uint8_t *skb_data;
	int i;

	_set_bank(2);

	if (REG16_LOAD(BANK_INTERRUPT) & TX_MASK) {
		/* TX interrupt */
		REG16_STORE(BANK_INTERRUPT, RX_INT | TX_INT | TX_ACK);
		return 0;
	}

	if (!(REG16_LOAD(BANK_INTERRUPT) & RX_MASK))
		return 0;

	REG16_STORE(BANK_POINTER, (0x8000 | AUTO_INCR | 2));
	len = (REG16_LOAD(BANK_DATA) & 0x7FF) - 10;
	/* In original structure, byte count includes headers, so
	 * we shrink it to data size */

	skb = skb_alloc(len + 2);
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

	_set_cmd(CMD_RX_POP_AND_RELEASE);

	if (buf & (ODD_CONTROL << 8)) {
		skb->len++;
		skb_data[skb->len -1] = (uint8_t)(buf & 0xFF);
	}
	show_packet(skb_data, len, "rx_pack");
	netif_rx(skb);

	return 0;
}

EMBOX_UNIT_INIT(lan91c111_init);
static int lan91c111_init(void) {
        struct net_device *nic;

        if (NULL == (nic = etherdev_alloc(0))) {
                return -ENOMEM;
        }

	irq_attach(LAN91C111_IRQ, lan91c111_int_handler, 0, nic, "lan91c111");

        nic->drv_ops = &lan91c111_drv_ops;

	_set_bank(1);
	REG16_STORE(BANK_CONTROL, 0x0800);

	return inetdev_register_dev(nic);
}

PERIPH_MEMORY_DEFINE(lan91c111, BANK_BASE_ADDR, 0x10);

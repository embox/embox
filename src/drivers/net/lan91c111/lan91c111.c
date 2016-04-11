/**
 * @file
 *
 * @data 09.04.2016
 * @author: Anton Bondarev
 */
#include <kernel/irq.h>
#include <mem/misc/pool.h>
#include <net/skbuff.h>
#include <net/netdevice.h>
#include <embox/unit.h>

/* Internal I/O space mapping */

#define BANK_BASE_ADDR 0x00000000 /* TODO */

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
#define BANK_IA35            (BANK_BASE_ADDR + 0x8)
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

#define REG16_LOAD(addr) \
	*((volatile uint16_t *)(addr))

#define REG16_STORE(addr, val) \
	do { *((volatile uint16_t *)(addr)) = (val); } while (0)

/* Commands */
#define CMD_NOP                0
#define CMD_TX_ALLOC           1
#define CMD_RESET_MMU          2
#define CMD_RX_POP             3
#define CMD_RX_POP_AND_RELEASE 4
#define CMD_PACKET_FREE        5
#define CMD_TX_ENQUEUE         6
#define CMD_TX_FIFO_RESET      7

#define LAN91C111_FRAME_SIZE_MAX 2048

struct lan91c111_frame {
	uint16_t status;
	uint16_t count; /* In bytes, 5 high bits are reserved */
	uint16_t data[LAN91C111_FRAME_SIZE_MAX];
};

/**
 * @brief Set active bank ID
 */
static void _set_bank(int n) {
	assert(0 <= n && n <= 3);
	REG16_STORE(BANK_BANK, (uint16_t) n);
}

/**
 * @brief Set approprate opcode
 */
static void _set_cmd(int opcode) {
	assert(0 <= opcode && opcode <= 7);

	_set_bank(2);
	while (REG16_LOAD(BANK_MMU_CMD) & 0x1) {
		/* BUSY */
	}
	REG16_STORE(BANK_MMU_CMD, (opcode << 5));
}

static int lan91c111_xmit(struct net_device *dev, struct sk_buff *skb) {
	uint8_t packet_num;

	_set_cmd(CMD_TX_ALLOC);

	_set_bank(2);
	while (!(REG16_LOAD(BANK_INTERRUPT) & (1 << 3))){
	};

	packet_num = REG16_LOAD(BANK_PNR) & 0x3F;

	/* TODO set data addr */

	REG16_STORE(BANK_PNR, packet_num << 8);

	_set_cmd(CMD_TX_ENQUEUE);

	/* TODO handle service interrupt */

	skb_free(skb);
	return 0;
}

static int lan91c111_open(struct net_device *dev) {
	return 0;
}

static int lan91c111_set_macaddr(struct net_device *dev, const void *addr) {
	return 0;
}

static const struct net_driver lan91c111_drv_ops = {
	.xmit = lan91c111_xmit,
	.start = lan91c111_open,
	.set_macaddr = lan91c111_set_macaddr
};

EMBOX_UNIT_INIT(lan91c111_init);
static int lan91c111_init(void) {
	_set_bank(0);
	REG16_STORE(BANK_RCR, 0x0100); /* Enable RX interrupts */
	/* TODO add rx interrupt handle */
	return 0;
}

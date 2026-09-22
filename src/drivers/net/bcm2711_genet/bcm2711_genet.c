/**
 * @file
 * @brief GENET v5 Ethernet for BCM2711 (Raspberry Pi 4)
 *
 * A Broadcom GENET v5 MAC at /scb/ethernet@7d580000, wired over RGMII to a
 * BCM54213PE PHY at MDIO address 1. /scb maps 0x7c000000 to 0xfc000000, so
 * the ARM view of the block is 0xFD580000.
 *
 * GENET reaches all of DRAM 1:1 (/scb dma-ranges), so descriptors point at
 * ordinary static arrays; nothing is coherent, so buffers are maintained in
 * both directions.
 *
 * Receive runs off the interrupt (INTRL2_0, GIC SPI 157 -> INTID 189) with a
 * slow poller underneath as a backstop; with use_irq=0 the poller carries
 * everything. Autonegotiation runs in its own thread and the interface
 * appears when the link does.
 *
 */

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <drivers/common/memory.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/cache.h>
#include <kernel/irq.h>
#include <kernel/thread.h>
#include <kernel/time/ktime.h>
#include <net/inetdevice.h>
#include <net/l0/net_entry.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/skbuff.h>
#include <util/log.h>

#include "genet_regs.h"

#define BASE_ADDR     OPTION_GET(NUMBER, base_addr)
#define PHY_ADDR      OPTION_GET(NUMBER, phy_addr)
#define ENABLED       OPTION_GET(NUMBER, enabled)
#define ANEG_MS       OPTION_GET(NUMBER, aneg_timeout_ms)
#define ID_MODE_DIS_V OPTION_GET(NUMBER, id_mode_dis)
#define PHY_RX_SKEW   OPTION_GET(NUMBER, phy_rx_skew)
#define PROMISC       OPTION_GET(NUMBER, promisc)
#define USE_IRQ       OPTION_GET(NUMBER, use_irq)
#define IRQ_NUM       OPTION_GET(NUMBER, irq_num)
#define POLL_MS       OPTION_GET(NUMBER, poll_ms)
#define POLL_MS_IRQ   OPTION_GET(NUMBER, poll_ms_irq)

#define GENET_MMIO_LEN 0x10000

#define MDIO_SPINS 100000u

PERIPH_MEMORY_DEFINE(bcm2711_genet, BASE_ADDR, GENET_MMIO_LEN);

static struct {
	uintptr_t base;
	uint8_t phy_addr;
	uint8_t mac[6];
	uint8_t mac_ok;
	uint8_t link;
	uint8_t full_duplex;
	uint16_t speed;
	uint16_t bmsr;
	uint32_t aneg_ms;

	uint8_t use_irq; /* the interrupt is attached and carrying traffic */
	uint8_t promisc;
	uint32_t mdf_ctrl;
	uint32_t mdf_mask; /* which enable bits MDF_CTRL really has */

	uint32_t tx_sent;
	uint32_t tx_done;
	uint32_t tx_fail;
	uint32_t rx_packets;
	uint32_t rx_bytes;
	uint32_t rx_dropped;
	uint32_t rx_errors;
	uint32_t rx_frag;
	uint32_t rx_by_irq;
	uint32_t rx_by_poll;
	uint32_t irq_count;
	uint32_t irq_rx;
} genet_state;

#define MBOX_BASE   0xFE00B880UL
#define MBOX_READ   (MBOX_BASE + 0x00)
#define MBOX_STATUS (MBOX_BASE + 0x18)
#define MBOX_WRITE  (MBOX_BASE + 0x20)

#define MBOX_FULL      0x80000000u
#define MBOX_EMPTY     0x40000000u
#define MBOX_CHAN_TAGS 8u
#define MBOX_RESP_OK   0x80000000u
#define MBOX_SPINS     10000000u

#define TAG_GET_BOARD_MAC 0x10003u

PERIPH_MEMORY_DEFINE(bcm2711_genet_mbox, MBOX_BASE, 0x40);

/* Static and cache-line aligned, not on the stack: dcache_inval() discards
 * whole lines, and a stack buffer would share its line with live spills. */
static uint32_t genet_mbox_buf[16] __attribute__((aligned(64)));

/* @bus_base selects which VideoCore alias of the buffer the firmware is
 * given: 0 is the cached view, 0xC0000000 the uncached one. Which one a
 * given firmware honours varies, so the caller tries both. */
static int genet_mailbox_mac(uint8_t mac[6], uint32_t bus_base) {
	uint32_t *buf = genet_mbox_buf;
	uint32_t msg;
	uint32_t got;
	uint32_t spins;

	memset(buf, 0, 32);
	buf[0] = 32;
	buf[1] = 0;
	buf[2] = TAG_GET_BOARD_MAC;
	/* The value buffer is declared at its padded size: six bytes of address
	 * rounded up to two words (5 and 6), with the end tag at 7. */
	buf[3] = 8;
	buf[4] = 0;
	buf[7] = 0;

	msg = (uint32_t)(uintptr_t)buf + bus_base;
	if (msg & 0xfu) {
		log_error("mailbox: buffer %p is not 16-byte aligned", (void *)buf);
		return -EINVAL;
	}

	dcache_flush(buf, 64);
	dsb(sy);

	for (spins = 0; REG32_LOAD(MBOX_STATUS) & MBOX_FULL; spins++) {
		if (spins > MBOX_SPINS) {
			log_error("mailbox: write side never drained");
			return -ETIMEDOUT;
		}
	}
	dmb(sy);
	REG32_STORE(MBOX_WRITE, msg | MBOX_CHAN_TAGS);

	spins = 0;
	for (;;) {
		while (REG32_LOAD(MBOX_STATUS) & MBOX_EMPTY) {
			if (++spins > MBOX_SPINS) {
				log_error("mailbox: no reply");
				return -ETIMEDOUT;
			}
		}
		got = REG32_LOAD(MBOX_READ);
		if ((got & 0xfu) == MBOX_CHAN_TAGS) {
			break;
		}
	}

	dcache_inval(buf, 64);
	dsb(sy);

	if ((got & ~0xfu) != msg || buf[1] != MBOX_RESP_OK) {
		log_error("mailbox: reply %08x want %08x, code %08x, tag resp %08x",
		    got, msg, buf[1], buf[4]);
		return -EIO;
	}

	memcpy(mac, &buf[5], 6);
	return 0;
}

static void genet_set_hwaddr(uintptr_t base, const uint8_t mac[6]) {
	genet_wr(base, GENET_UMAC_MAC0,
	    ((uint32_t)mac[0] << 24) | ((uint32_t)mac[1] << 16)
	        | ((uint32_t)mac[2] << 8) | mac[3]);
	genet_wr(base, GENET_UMAC_MAC1, ((uint32_t)mac[4] << 8) | mac[5]);
}

/* ------------------------------------------------------------------ mdio */

static int genet_mdio_read(uintptr_t base, uint8_t phy, uint8_t reg) {
	uint32_t cmd;
	unsigned spins;

	cmd = GENET_MDIO_RD | ((uint32_t)phy << GENET_MDIO_PMD_SHIFT)
	      | ((uint32_t)reg << GENET_MDIO_REG_SHIFT);

	/* Load the fields, then start: START_BUSY in the same store as the
	 * address fields does not reliably latch them. */
	genet_wr(base, GENET_MDIO_CMD, cmd);
	genet_wr(base, GENET_MDIO_CMD, cmd | GENET_MDIO_START_BUSY);

	for (spins = 0; genet_rd(base, GENET_MDIO_CMD) & GENET_MDIO_START_BUSY;
	    spins++) {
		if (spins > MDIO_SPINS) {
			log_error("MDIO phy %u reg %u: never went idle", phy, reg);
			return -ETIMEDOUT;
		}
	}

	cmd = genet_rd(base, GENET_MDIO_CMD);
	if (cmd & GENET_MDIO_READ_FAIL) {
		log_error("MDIO phy %u reg %u: read failed", phy, reg);
		return -EIO;
	}

	return (int)(cmd & 0xffff);
}

static int genet_mdio_write(uintptr_t base, uint8_t phy, uint8_t reg, uint16_t val) {
	uint32_t cmd;
	unsigned spins;

	cmd = GENET_MDIO_WR | ((uint32_t)phy << GENET_MDIO_PMD_SHIFT)
	      | ((uint32_t)reg << GENET_MDIO_REG_SHIFT) | val;

	genet_wr(base, GENET_MDIO_CMD, cmd);
	genet_wr(base, GENET_MDIO_CMD, cmd | GENET_MDIO_START_BUSY);

	for (spins = 0; genet_rd(base, GENET_MDIO_CMD) & GENET_MDIO_START_BUSY;
	    spins++) {
		if (spins > MDIO_SPINS) {
			log_error("MDIO write phy %u reg %u: never went idle", phy, reg);
			return -ETIMEDOUT;
		}
	}
	return 0;
}

/* ------------------------------------------------------- phy vendor regs
 *
 * `phy-mode = "rgmii-rxid"` means the PHY supplies the receive skew, but it
 * does not set it by itself after a BMCR_RESET. Without the skew the link
 * negotiates, transmits normally, and receives nothing the MAC will keep.
 */

static int genet_auxctl_read(uintptr_t base, uint8_t phy, uint16_t shadow) {
	/* The shadow is named twice: in the low bits, which select where a write
	 * lands, and at bit 12, which selects what the next read returns. */
	if (genet_mdio_write(base, phy, MII_BCM54XX_AUXCTL,
	        (uint16_t)(BCM54XX_AUXCTL_SHDWSEL_MISC
	                   | (shadow << BCM54XX_AUXCTL_READ_SHIFT)))) {
		return -EIO;
	}
	return genet_mdio_read(base, phy, MII_BCM54XX_AUXCTL);
}

static int genet_shd_read(uintptr_t base, uint8_t phy, uint16_t shadow) {
	int v;

	if (genet_mdio_write(base, phy, MII_BCM54XX_SHD,
	        (uint16_t)BCM54XX_SHD_SEL(shadow))) {
		return -EIO;
	}
	v = genet_mdio_read(base, phy, MII_BCM54XX_SHD);
	return v < 0 ? v : (int)BCM54XX_SHD_DATA((uint16_t)v);
}

/* @want: 0 leave the PHY as reset left it, 1 enable the skew, 2 disable it.
 * The value before and after is logged, since the reset default depends on
 * the board's straps. */
static void genet_phy_rgmii_skew(uintptr_t base, uint8_t phy, int want) {
	int before;
	int after;
	uint16_t clk_ctl;

	before = genet_auxctl_read(base, phy, BCM54XX_AUXCTL_SHDWSEL_MISC);
	if (before < 0) {
		log_error("PHY AUXCTL unreadable; leaving the RGMII skew alone");
		return;
	}
	clk_ctl = (uint16_t)genet_shd_read(base, phy, BCM54810_SHD_CLK_CTL);

	if (want) {
		uint16_t val = (uint16_t)before | BCM54XX_AUXCTL_MISC_WREN;

		if (want == 1) {
			val |= BCM54XX_AUXCTL_MISC_RXD_RXC_SKEW;
		}
		else {
			val &= (uint16_t)~BCM54XX_AUXCTL_MISC_RXD_RXC_SKEW;
		}
		genet_mdio_write(base, phy, MII_BCM54XX_AUXCTL,
		    (uint16_t)(BCM54XX_AUXCTL_SHDWSEL_MISC | val));
	}

	after = genet_auxctl_read(base, phy, BCM54XX_AUXCTL_SHDWSEL_MISC);
	if (after < 0) {
		after = 0;
	}

	log_info("PHY RGMII skew: AUXCTL 0x%04x -> 0x%04x (RXD-RXC %s), "
	         "CLK_CTL 0x%04x (PHY tx delay %s)",
	    (unsigned)before, (unsigned)after,
	    (after & BCM54XX_AUXCTL_MISC_RXD_RXC_SKEW) ? "on" : "off", clk_ctl,
	    (clk_ctl & BCM54810_SHD_CLK_CTL_GTXCLK_EN) ? "on" : "off");
}

/* ------------------------------------------------------------------ reset */

/* Enough of U-Boot's umac reset to make MDIO usable; the rings are set up
 * separately, once the link is known. */
static void genet_umac_reset(uintptr_t base) {
	uint32_t reg;

	reg = genet_rd(base, GENET_SYS_RBUF_FLUSH_CTRL);
	genet_wr(base, GENET_SYS_RBUF_FLUSH_CTRL, reg | (1u << 1));
	usleep(10);
	genet_wr(base, GENET_SYS_RBUF_FLUSH_CTRL, reg & ~(1u << 1));
	usleep(10);

	/* And the transmit side, which U-Boot's reset leaves alone: if the
	 * firmware left TBUF flushing, the DMA retires descriptors while the MAC
	 * throws the frames away. */
	genet_wr(base, GENET_SYS_TBUF_FLUSH_CTRL, 0);

	genet_wr(base, GENET_UMAC_CMD, 0);
	genet_wr(base, GENET_UMAC_CMD, GENET_CMD_SW_RESET | GENET_CMD_LCL_LOOP_EN);
	usleep(2);
	genet_wr(base, GENET_UMAC_CMD, 0);

	genet_wr(base, GENET_UMAC_MIB_CTRL,
	    GENET_MIB_RESET_RX | GENET_MIB_RESET_TX | GENET_MIB_RESET_RUNT);
	genet_wr(base, GENET_UMAC_MIB_CTRL, 0);

	genet_wr(base, GENET_UMAC_MAX_FRAME_LEN, GENET_MAX_MTU);

	reg = genet_rd(base, GENET_RBUF_CTRL);
	genet_wr(base, GENET_RBUF_CTRL, reg | GENET_RBUF_ALIGN_2B);
	genet_wr(base, GENET_RBUF_TBUF_SIZE_CTRL, 1);
}

/* -------------------------------------------------------------- rgmii/link
 *
 * The RGMII block first: until it is enabled and taken out of out-of-band
 * control, the PHY's clock does not reach the MAC. Then ordinary MII
 * autonegotiation.
 */

static void genet_rgmii_init(uintptr_t base) {
	uint32_t reg;

	genet_wr(base, GENET_SYS_PORT_CTRL, GENET_PORT_MODE_EXT_GPHY);

	reg = genet_rd(base, GENET_EXT_RGMII_OOB_CTRL);
	reg &= ~GENET_OOB_DISABLE;
	reg |= GENET_RGMII_MODE_EN;

	/* ID_MODE_DIS *set*: the MAC adds no transmit delay. phy-mode
	 * "rgmii-rxid" describes only what the PHY does; unless the mode says
	 * -txid or -id the MAC's internal delay stays off, as in bcmgenet. */
	if (ID_MODE_DIS_V) {
		reg |= GENET_ID_MODE_DIS;
	}
	else {
		reg &= ~GENET_ID_MODE_DIS;
	}
	genet_wr(base, GENET_EXT_RGMII_OOB_CTRL, reg);

	log_info("RGMII enabled, OOB_CTRL 0x%08x",
	    genet_rd(base, GENET_EXT_RGMII_OOB_CTRL));
}

static int genet_phy_reset(uintptr_t base, uint8_t phy) {
	unsigned ms;
	int bmcr;

	if (genet_mdio_write(base, phy, MII_BMCR, BMCR_RESET)) {
		return -EIO;
	}
	for (ms = 0; ms < 500; ms++) {
		bmcr = genet_mdio_read(base, phy, MII_BMCR);
		if (bmcr < 0) {
			return bmcr;
		}
		if (!(bmcr & BMCR_RESET)) {
			return 0;
		}
		usleep(1000);
	}
	log_error("PHY reset did not clear");
	return -ETIMEDOUT;
}

static void genet_link_up(uintptr_t base, uint8_t phy) {
	uint64_t start;
	int bmsr;
	int lpa;
	int stat;
	unsigned ms;

	if (genet_phy_reset(base, phy)) {
		return;
	}

	/* Before autonegotiation: the skew is a property of the link it is about
	 * to establish. */
	genet_phy_rgmii_skew(base, phy, PHY_RX_SKEW);

	genet_mdio_write(base, phy, MII_ANAR,
	    ANAR_802_3 | ANAR_10HD | ANAR_10FD | ANAR_100HD | ANAR_100FD);
	genet_mdio_write(base, phy, MII_CTRL1000, CTRL1000_1000HD | CTRL1000_1000FD);
	genet_mdio_write(base, phy, MII_BMCR, BMCR_ANENABLE | BMCR_ANRESTART);

	start = ktime_get_ns();
	for (ms = 0; ms < ANEG_MS; ms++) {
		/* Twice: BMSR_LINK is latching-low, so only the second read is the
		 * state now. */
		(void)genet_mdio_read(base, phy, MII_BMSR);
		bmsr = genet_mdio_read(base, phy, MII_BMSR);
		if (bmsr < 0) {
			return;
		}
		genet_state.bmsr = (uint16_t)bmsr;
		if ((bmsr & BMSR_ANEGDONE) && (bmsr & BMSR_LINK)) {
			break;
		}
		usleep(1000);
	}
	genet_state.aneg_ms = (uint32_t)((ktime_get_ns() - start) / 1000000u);
	genet_state.link = !!(genet_state.bmsr & BMSR_LINK);

	if (!genet_state.link) {
		log_error("no link after %u ms (BMSR 0x%04x) -- cable?",
		    genet_state.aneg_ms, genet_state.bmsr);
		return;
	}

	stat = genet_mdio_read(base, phy, MII_STAT1000);
	lpa = genet_mdio_read(base, phy, MII_LPA);
	if (stat < 0) {
		stat = 0;
	}
	if (lpa < 0) {
		lpa = 0;
	}

	if (stat & (STAT1000_1000FD | STAT1000_1000HD)) {
		genet_state.speed = 1000;
		genet_state.full_duplex = !!(stat & STAT1000_1000FD);
	}
	else if (lpa & (ANAR_100FD | ANAR_100HD)) {
		genet_state.speed = 100;
		genet_state.full_duplex = !!(lpa & ANAR_100FD);
	}
	else {
		genet_state.speed = 10;
		genet_state.full_duplex = !!(lpa & ANAR_10FD);
	}

	log_info("link up: %u Mbps %s duplex after %u ms (BMSR 0x%04x)",
	    genet_state.speed, genet_state.full_duplex ? "full" : "half",
	    genet_state.aneg_ms, genet_state.bmsr);
}

/* ------------------------------------------------------------------ tx
 *
 * One descriptor ring and one buffer: genet_send() waits for the descriptor
 * to retire before returning, so a single buffer serves every frame.
 */

/* Each iteration is a device register read, so the bound is tens of
 * milliseconds: out of reach of a real transmit, and short enough to report
 * a dead link rather than hang on it. */
#define GENET_TX_SPINS 200000u

static uint8_t genet_tx_buf[GENET_MAX_MTU] __attribute__((aligned(64)));
static uint32_t genet_tx_index;
static uint32_t genet_tx_prod;

static void genet_tx_ring_init(uintptr_t base) {
	uint32_t cons;

	genet_wr(base, GENET_TDMA_SCB_BURST_SIZE, GENET_DMA_MAX_BURST);
	genet_wr(base, GENET_TDMA_START_ADDR, 0);
	genet_wr(base, GENET_TDMA_READ_PTR, 0);
	genet_wr(base, GENET_TDMA_WRITE_PTR, 0);
	/* END_ADDR counts 32-bit words, not descriptors and not bytes. */
	genet_wr(base, GENET_TDMA_END_ADDR,
	    GENET_TOTAL_DESCS * GENET_DMA_DESC_SIZE / 4 - 1);

	/* Adopt the hardware's consumer index rather than assuming zero: the
	 * firmware may have used this ring. */
	cons = genet_rd(base, GENET_TDMA_CONS_INDEX);
	genet_wr(base, GENET_TDMA_PROD_INDEX, cons);
	genet_tx_prod = cons & 0xffff;
	genet_tx_index = cons & 0xff;

	genet_wr(base, GENET_TDMA_DONE_THRESH, 1);
	genet_wr(base, GENET_TDMA_FLOW_PERIOD, 0);
	genet_wr(base, GENET_TDMA_RING_BUF_SIZE,
	    ((uint32_t)GENET_TOTAL_DESCS << GENET_DMA_RING_SIZE_SHIFT)
	        | GENET_RX_BUF_LENGTH);
	genet_wr(base, GENET_TDMA_RING_CFG, 1u << GENET_DEFAULT_Q);
}

static void genet_adjust_link(uintptr_t base) {
	uint32_t reg;
	uint32_t speed;

	speed = genet_state.speed == 1000  ? GENET_UMAC_SPEED_1000
	        : genet_state.speed == 100 ? GENET_UMAC_SPEED_100
	                                   : GENET_UMAC_SPEED_10;

	reg = genet_rd(base, GENET_EXT_RGMII_OOB_CTRL);
	genet_wr(base, GENET_EXT_RGMII_OOB_CTRL, reg | GENET_RGMII_LINK);

	reg = genet_rd(base, GENET_UMAC_CMD);
	reg &= ~(GENET_CMD_SPEED_MASK << GENET_CMD_SPEED_SHIFT);
	reg |= speed << GENET_CMD_SPEED_SHIFT;
	if (genet_state.full_duplex) {
		reg &= ~GENET_CMD_HD_EN;
	}
	else {
		reg |= GENET_CMD_HD_EN;
	}
	genet_wr(base, GENET_UMAC_CMD, reg);
}

static void genet_tx_enable(uintptr_t base) {
	uint32_t reg;

	reg = genet_rd(base, GENET_UMAC_CMD);
	/* Transmit only: receive is enabled in genet_start(), once there is a
	 * ring to put frames in. */
	genet_wr(base, GENET_UMAC_CMD, reg | GENET_CMD_TX_EN);

	genet_wr(base, GENET_TDMA_CTRL,
	    (1u << (GENET_DEFAULT_Q + GENET_DMA_RING_BUF_EN_SHIFT)) | GENET_DMA_EN);
}

static int genet_send(uintptr_t base, const void *frame, size_t len) {
	uint64_t addr;
	uint32_t desc;
	uint32_t len_stat;
	unsigned spins;

	if (len > sizeof(genet_tx_buf)) {
		return -EINVAL;
	}

	memcpy(genet_tx_buf, frame, len);

	/* Pad to the 60-byte Ethernet minimum in software: the MAC appends the
	 * CRC but does not pad (CMD_PAD_EN is left at its default), and Embox's
	 * 42-byte ARP replies would otherwise leave as runts. */
	if (len < 60) {
		memset(genet_tx_buf + len, 0, 60 - len);
		len = 60;
	}

	dcache_flush(genet_tx_buf, sizeof(genet_tx_buf));
	dsb(sy);

	addr = (uint64_t)(uintptr_t)genet_tx_buf;
	desc = GENET_TX_OFF + genet_tx_index * GENET_DMA_DESC_SIZE;

	len_stat = ((uint32_t)len << GENET_DMA_BUFLENGTH_SHIFT)
	           | (GENET_DMA_TX_QTAG_MASK << GENET_DMA_TX_QTAG_SHIFT)
	           | GENET_DMA_TX_APPEND_CRC | GENET_DMA_SOP | GENET_DMA_EOP;

	/* Address first, status last: the status word is what hands the
	 * descriptor over. */
	genet_wr(base, desc + GENET_DESC_ADDRESS_LO, (uint32_t)addr);
	genet_wr(base, desc + GENET_DESC_ADDRESS_HI, (uint32_t)(addr >> 32));
	genet_wr(base, desc + GENET_DESC_LENGTH_STATUS, len_stat);

	genet_tx_index = (genet_tx_index + 1) % GENET_TOTAL_DESCS;
	genet_tx_prod = (genet_tx_prod + 1) & 0xffff;
	genet_wr(base, GENET_TDMA_PROD_INDEX, genet_tx_prod);

	genet_state.tx_sent++;

	/* Busy-wait, not usleep(): netif_tx_action() calls xmit inside
	 * sched_lock(), where sleeping would stop the scheduler with the lock
	 * held. A full-size frame at a gigabit is twelve microseconds. */
	for (spins = 0; spins < GENET_TX_SPINS; spins++) {
		if ((genet_rd(base, GENET_TDMA_CONS_INDEX) & 0xffff) == genet_tx_prod) {
			genet_state.tx_done++;
			return 0;
		}
	}

	genet_state.tx_fail++;
	return -ETIMEDOUT;
}

/* ------------------------------------------------------------------ rx
 *
 * The descriptors are registers; only the frame buffers are in DRAM, and each
 * is invalidated before it is read. The ring is index-driven: the hardware
 * advances the producer index and the driver's write of the consumer index
 * returns a descriptor, so nothing is re-armed per frame.
 */

static uint8_t genet_rx_buf[GENET_TOTAL_DESCS][GENET_RX_BUF_LENGTH]
    __attribute__((aligned(64)));
static uint32_t genet_rx_index; /* which descriptor is next    */
static uint32_t genet_rx_cons;  /* the index the block sees    */

static struct net_device *genet_netdev;

static void genet_link_bringup(void);
static void *genet_link_thread(void *arg);

static void genet_rx_ring_init(uintptr_t base) {
	genet_wr(base, GENET_RDMA_SCB_BURST_SIZE, GENET_DMA_MAX_BURST);

	genet_wr(base, GENET_RDMA_START_ADDR, 0);
	genet_wr(base, GENET_RDMA_READ_PTR, 0);
	genet_wr(base, GENET_RDMA_WRITE_PTR, 0);
	/* END_ADDR counts 32-bit words, as on the transmit side. */
	genet_wr(base, GENET_RDMA_END_ADDR,
	    GENET_TOTAL_DESCS * GENET_DMA_DESC_SIZE / 4 - 1);

	/* Both indices to zero, and the driver's copies with them. Unlike the
	 * transmit ring there is nothing to adopt: anything the firmware
	 * received landed in buffers this driver does not own. */
	genet_wr(base, GENET_RDMA_PROD_INDEX, 0);
	genet_wr(base, GENET_RDMA_CONS_INDEX, 0);
	genet_rx_cons = 0;
	genet_rx_index = 0;

	genet_wr(base, GENET_RDMA_RING_BUF_SIZE,
	    ((uint32_t)GENET_TOTAL_DESCS << GENET_DMA_RING_SIZE_SHIFT)
	        | GENET_RX_BUF_LENGTH);
	genet_wr(base, GENET_RDMA_XON_XOFF_THRESH, GENET_DMA_FC_THRESH);
	genet_wr(base, GENET_RDMA_RING_CFG, 1u << GENET_DEFAULT_Q);
}

static void genet_rx_descs_init(uintptr_t base) {
	uint32_t len_stat;
	uint32_t i;

	/* Clean the whole buffer array before the device may write into it: it
	 * is BSS, so startup zeroing left dirty lines behind, and a later
	 * write-back would land on top of a received frame. */
	dcache_flush(genet_rx_buf, sizeof(genet_rx_buf));
	dsb(sy);

	len_stat = ((uint32_t)GENET_RX_BUF_LENGTH << GENET_DMA_BUFLENGTH_SHIFT)
	           | GENET_DMA_OWN;

	for (i = 0; i < GENET_TOTAL_DESCS; i++) {
		uint64_t addr = (uint64_t)(uintptr_t)genet_rx_buf[i];
		uint32_t desc = GENET_RX_OFF + i * GENET_DMA_DESC_SIZE;

		genet_wr(base, desc + GENET_DESC_ADDRESS_LO, (uint32_t)addr);
		genet_wr(base, desc + GENET_DESC_ADDRESS_HI, (uint32_t)(addr >> 32));
		genet_wr(base, desc + GENET_DESC_LENGTH_STATUS, len_stat);
	}
}

/* Take everything the ring holds and hand it to the stack. Returns how many
 * frames were delivered, so the poller can tell busy from idle. */
static int genet_rx_drain(int from_irq) {
	uintptr_t base = genet_state.base;
	struct net_device *dev = genet_netdev;
	uint32_t prod;
	int delivered = 0;

	if (!dev) {
		return 0;
	}

	prod = genet_rd(base, GENET_RDMA_PROD_INDEX) & 0xffff;

	while (genet_rx_cons != prod) {
		uint32_t desc = GENET_RX_OFF + genet_rx_index * GENET_DMA_DESC_SIZE;
		uint32_t st = genet_rd(base, desc + GENET_DESC_LENGTH_STATUS);
		uint32_t len = (st >> GENET_DMA_BUFLENGTH_SHIFT) & GENET_DMA_BUFLENGTH_MASK;
		uint8_t *buf = genet_rx_buf[genet_rx_index];

		dcache_inval(buf, GENET_RX_BUF_LENGTH);

		if ((st & (GENET_DMA_SOP | GENET_DMA_EOP))
		    != (GENET_DMA_SOP | GENET_DMA_EOP)) {
			/* A frame split across descriptors, which 2048-byte buffers and
			 * a 1536-byte MTU rule out; counted apart from CRC errors. */
			genet_state.rx_frag++;
			dev->stats.rx_err++;
		}
		else if (st & GENET_DMA_RX_ERRORS) {
			genet_state.rx_errors++;
			dev->stats.rx_err++;
			if (st & GENET_DMA_RX_CRC_ERROR) {
				dev->stats.rx_crc_errors++;
			}
			if (st & GENET_DMA_RX_OV) {
				dev->stats.rx_over_errors++;
			}
		}
		else if (len <= GENET_RX_BUF_OFFSET || len > GENET_RX_BUF_LENGTH) {
			genet_state.rx_errors++;
			dev->stats.rx_length_errors++;
		}
		else {
			size_t plen = len - GENET_RX_BUF_OFFSET;
			struct sk_buff *skb = skb_alloc(plen);

			if (!skb) {
				/* The descriptor is still returned below, so a burst that
				 * outruns the pool costs frames, not the ring. The first
				 * drop is logged: Embox's TCP has neither fast retransmit
				 * nor SACK, so one lost segment costs a retransmit timeout. */
				if (!genet_state.rx_dropped) {
					log_info("skb pool empty at %u frames, %u bytes; "
					         "ring prod %u cons %u",
					    genet_state.rx_packets, genet_state.rx_bytes, prod,
					    genet_rx_cons);
				}
				genet_state.rx_dropped++;
				dev->stats.rx_dropped++;
			}
			else {
				memcpy(skb->mac.raw, buf + GENET_RX_BUF_OFFSET, plen);
				skb->len = plen;
				skb->dev = dev;

				genet_state.rx_packets++;
				genet_state.rx_bytes += (uint32_t)plen;
				dev->stats.rx_packets++;
				dev->stats.rx_bytes += plen;

				if (from_irq) {
					genet_state.rx_by_irq++;
				}
				else {
					genet_state.rx_by_poll++;
				}

				netif_rx(skb);
				delivered++;
			}
		}

		genet_rx_cons = (genet_rx_cons + 1) & 0xffff;
		genet_rx_index = (genet_rx_index + 1) % GENET_TOTAL_DESCS;
		/* Writing the consumer index is what gives the descriptor back. */
		genet_wr(base, GENET_RDMA_CONS_INDEX, genet_rx_cons);
	}

	return delivered;
}

/* The receive backstop: drains the ring every poll_ms_irq milliseconds so
 * that an interrupt line that turns out to be silent costs latency rather
 * than connectivity. With use_irq=0 it is the only receive path and sleeps
 * for poll_ms instead. It also logs the counters every five seconds in which
 * anything moved. */
static void *genet_rx_thread(void *arg) {
	uint32_t last_packets = 0;
	uint32_t last_errors = 0;
	unsigned quiet_ms = 0;

	(void)arg;

	while (1) {
		unsigned nap = genet_state.use_irq ? POLL_MS_IRQ : POLL_MS;

		if (genet_rx_drain(0) == 0) {
			usleep(nap * 1000);
			quiet_ms += nap;
		}

		if (quiet_ms >= 5000) {
			quiet_ms = 0;

			if (genet_state.rx_packets != last_packets
			    || genet_state.rx_errors != last_errors) {
				/* irq_count is separate from rx_by_irq on purpose: a line
				 * that never fires and a handler that delivers nothing are
				 * different faults with the same symptom. */
				log_info("rx %u frames (%u by irq, %u by poll), %u bytes, "
				         "%u dropped, %u bad (tx %u sent, %u failed, "
				         "irq %u fired / %u rx, filter %s)",
				    genet_state.rx_packets, genet_state.rx_by_irq,
				    genet_state.rx_by_poll, genet_state.rx_bytes,
				    genet_state.rx_dropped, genet_state.rx_errors,
				    genet_state.tx_sent, genet_state.tx_fail,
				    genet_state.irq_count, genet_state.irq_rx,
				    genet_state.promisc ? "promiscuous" : "MDF");
				last_packets = genet_state.rx_packets;
				last_errors = genet_state.rx_errors;
			}
		}
	}
	return NULL;
}

static irq_return_t genet_irq_handler(unsigned int irq, void *dev_id) {
	uintptr_t base = genet_state.base;
	uint32_t st;

	(void)irq;
	(void)dev_id;

	/* Only the unmasked bits: the status word reports every source whether
	 * or not it was asked for. */
	st = genet_rd(base, GENET_INTRL2_CPU_STAT)
	     & ~genet_rd(base, GENET_INTRL2_CPU_MASK_STATUS);
	genet_wr(base, GENET_INTRL2_CPU_CLEAR, st);

	genet_state.irq_count++;
	/* All three receive-done bits, as at the unmask: this silicon raises
	 * PDONE and BDONE for the receive ring, never the mailbox-done bit
	 * Linux calls RXDMA_DONE. */
	if (st & (GENET_IRQ_RXDMA_DONE | GENET_IRQ_RXDMA_PDONE | GENET_IRQ_RXDMA_BDONE)) {
		genet_state.irq_rx++;
		genet_rx_drain(1);
	}

	return IRQ_HANDLED;
}

/* --------------------------------------------------------- net_device ops */

/* One filter slot: two words of address, and the enable bit that goes with
 * them. @slot is the FILTER index, not the word index (Linux's helper steps
 * one counter by two words and another by one filter). The enable bit is
 * counted down from @top_bit, which genet_mdf_width() measures. */
static void genet_mdf_set(uintptr_t base, const uint8_t *addr, unsigned slot,
    unsigned top_bit) {
	uint32_t reg;

	genet_wr(base, GENET_UMAC_MDF_ADDR + slot * 8,
	    ((uint32_t)addr[0] << 8) | addr[1]);
	genet_wr(base, GENET_UMAC_MDF_ADDR + slot * 8 + 4,
	    ((uint32_t)addr[2] << 24) | ((uint32_t)addr[3] << 16)
	        | ((uint32_t)addr[4] << 8) | addr[5]);

	if (slot > top_bit) {
		return;
	}
	reg = genet_rd(base, GENET_UMAC_MDF_CTRL);
	genet_wr(base, GENET_UMAC_MDF_CTRL, reg | (1u << (top_bit - slot)));
}

/* How many enable bits MDF_CTRL really has: write all ones, read back what
 * stuck, restore zero. The datasheet's 17 filters and the register's
 * behaviour disagree on this board. */
static unsigned genet_mdf_width(uintptr_t base) {
	uint32_t mask;
	unsigned top;

	genet_wr(base, GENET_UMAC_MDF_CTRL, 0xffffffffu);
	mask = genet_rd(base, GENET_UMAC_MDF_CTRL);
	genet_wr(base, GENET_UMAC_MDF_CTRL, 0);

	genet_state.mdf_mask = mask;
	if (!mask) {
		return 0;
	}
	for (top = 31; top > 0 && !(mask & (1u << top)); top--) {}
	return top;
}

/* The MDF filter this driver intends, kept so promiscuous mode can be
 * lifted again. */
static uint32_t genet_mdf_wanted;

/* CMD_PROMISC does NOT override the MDF filter: with a non-empty MDF_CTRL
 * the interface stays as selective as before while reporting itself
 * promiscuous. So MDF_CTRL is zeroed along with the bit, as in Linux, and
 * genet_mdf_wanted holds the filter to put back. */
static void genet_set_promisc(uintptr_t base, int on) {
	uint32_t reg = genet_rd(base, GENET_UMAC_CMD);

	if (on) {
		reg |= GENET_CMD_PROMISC;
		genet_wr(base, GENET_UMAC_MDF_CTRL, 0);
	}
	else {
		reg &= ~GENET_CMD_PROMISC;
		genet_wr(base, GENET_UMAC_MDF_CTRL, genet_mdf_wanted);
	}
	genet_wr(base, GENET_UMAC_CMD, reg);
	genet_state.promisc = on ? 1 : 0;
	genet_state.mdf_ctrl = genet_rd(base, GENET_UMAC_MDF_CTRL);
}

static void genet_set_rx_filter(uintptr_t base) {
	static const uint8_t bcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	unsigned top;

	top = genet_mdf_width(base);
	genet_mdf_set(base, bcast, 0, top);
	genet_mdf_set(base, genet_state.mac, 1, top);
	genet_mdf_wanted = genet_rd(base, GENET_UMAC_MDF_CTRL);

	genet_set_promisc(base, PROMISC);

	log_info("rx filter: MDF writable 0x%08x (top bit %u), wanted 0x%08x, "
	         "MDF_CTRL 0x%08x, promiscuous %s",
	    genet_state.mdf_mask, top, genet_mdf_wanted, genet_state.mdf_ctrl,
	    genet_state.promisc ? "on" : "off");
}

static int genet_start(struct net_device *dev) {
	uintptr_t base = genet_state.base;
	uint32_t reg;

	(void)dev;

	genet_rx_ring_init(base);
	genet_rx_descs_init(base);
	genet_set_rx_filter(base);

	reg = genet_rd(base, GENET_UMAC_CMD);
	genet_wr(base, GENET_UMAC_CMD, reg | GENET_CMD_TX_EN | GENET_CMD_RX_EN);

	genet_wr(base, GENET_TDMA_CTRL,
	    (1u << (GENET_DEFAULT_Q + GENET_DMA_RING_BUF_EN_SHIFT)) | GENET_DMA_EN);
	reg = genet_rd(base, GENET_RDMA_CTRL);
	genet_wr(base, GENET_RDMA_CTRL,
	    reg | (1u << (GENET_DEFAULT_Q + GENET_DMA_RING_BUF_EN_SHIFT))
	        | GENET_DMA_EN);

	if (USE_IRQ) {
		genet_wr(base, GENET_INTRL2_CPU_CLEAR, 0xffffffffu);
		genet_wr(base, GENET_INTRL2_CPU_MASK_SET, 0xffffffffu);
		/* All three receive-done bits, not just MBDONE: this silicon raises
		 * PDONE and BDONE (bits 14 and 15) for the receive ring and leaves
		 * bit 13, the mailbox-done bit Linux calls RXDMA_DONE, clear. */
		genet_wr(base, GENET_INTRL2_CPU_MASK_CLEAR,
		    GENET_IRQ_RXDMA_DONE | GENET_IRQ_RXDMA_PDONE | GENET_IRQ_RXDMA_BDONE);
	}
	else {
		/* Masked: an unhandled interrupt on a level-triggered line would
		 * wedge the board. */
		genet_wr(base, GENET_INTRL2_CPU_MASK_SET, 0xffffffffu);
	}

	log_info("rx enabled: RDMA_CTRL 0x%08x RING_CFG 0x%08x UMAC_CMD 0x%08x",
	    genet_rd(base, GENET_RDMA_CTRL), genet_rd(base, GENET_RDMA_RING_CFG),
	    genet_rd(base, GENET_UMAC_CMD));

	return 0;
}

static int genet_stop(struct net_device *dev) {
	uintptr_t base = genet_state.base;
	uint32_t reg;

	(void)dev;

	genet_wr(base, GENET_INTRL2_CPU_MASK_SET, 0xffffffffu);

	reg = genet_rd(base, GENET_RDMA_CTRL);
	genet_wr(base, GENET_RDMA_CTRL, reg & ~GENET_DMA_EN);
	reg = genet_rd(base, GENET_TDMA_CTRL);
	genet_wr(base, GENET_TDMA_CTRL, reg & ~GENET_DMA_EN);

	reg = genet_rd(base, GENET_UMAC_CMD);
	genet_wr(base, GENET_UMAC_CMD, reg & ~(GENET_CMD_TX_EN | GENET_CMD_RX_EN));

	return 0;
}

static int genet_xmit(struct net_device *dev, struct sk_buff *skb) {
	int ret;

	ret = genet_send(genet_state.base, skb->mac.raw, skb->len);
	skb_free(skb);

	return ret;
}

static int genet_drv_set_macaddr(struct net_device *dev, const void *addr) {
	memcpy(genet_state.mac, addr, 6);
	genet_set_hwaddr(genet_state.base, genet_state.mac);

	return 0;
}

static const struct net_driver genet_drv_ops = {
    .start = genet_start,
    .stop = genet_stop,
    .xmit = genet_xmit,
    .set_macaddr = genet_drv_set_macaddr,
};

/* Bring the interface into service. No IP address is configured here: it
 * comes from netmanager, ifconfig or a BOOTP client as for any other Embox
 * interface. */
static int genet_netdev_up(void) {
	struct net_device *dev;
	int err;

	dev = etherdev_alloc(0);
	if (!dev) {
		log_error("etherdev_alloc failed");
		return -ENOMEM;
	}

	dev->base_addr = genet_state.base;
	dev->irq = IRQ_NUM;
	dev->drv_ops = &genet_drv_ops;
	memcpy(&dev->dev_addr[0], genet_state.mac, 6);

	genet_netdev = dev;

	if (USE_IRQ) {
		err = irq_attach(IRQ_NUM, genet_irq_handler, 0, dev, "bcm2711_genet");
		if (err) {
			log_error("irq_attach(%u) = %d; the poller still runs", IRQ_NUM, err);
		}
		else {
			genet_state.use_irq = 1;
		}
	}

	err = inetdev_register_dev(dev);
	if (err) {
		log_error("inetdev_register_dev = %d", err);
		genet_netdev = NULL;
		return err;
	}

	err = netdev_flag_up(dev, IFF_UP);
	if (err) {
		log_error("netdev_flag_up = %d", err);
		return err;
	}

	log_info("%s up: %02x:%02x:%02x:%02x:%02x:%02x, %u Mbps %s duplex%s",
	    dev->name, genet_state.mac[0], genet_state.mac[1], genet_state.mac[2],
	    genet_state.mac[3], genet_state.mac[4], genet_state.mac[5],
	    genet_state.speed, genet_state.full_duplex ? "full" : "half",
	    genet_state.use_irq ? "" : " (polled)");

	if (!thread_create(0, genet_rx_thread, NULL)) {
		log_error("could not start the receive backstop");
		return -EAGAIN;
	}

	return 0;
}

/* ------------------------------------------------------------------- init */

EMBOX_UNIT_INIT(genet_init);

static int genet_init(void) {
	uintptr_t base = BASE_ADDR;
	uint32_t rev;
	uint8_t major;
	int id1;
	int id2;

	if (!ENABLED) {
		/* Reading an unmapped address on this SoC is a bus abort rather
		 * than 0xffffffff, so the "absent" branch below never runs: where
		 * the block is known not to be there, do not probe for it. */
		log_info("genet probe disabled for this machine");
		return 0;
	}

	genet_state.base = base;
	genet_state.phy_addr = PHY_ADDR;

	rev = genet_rd(base, GENET_SYS_REV_CTRL);

	/* A block that is absent, unclocked or at the wrong address reads back
	 * all ones or all zeros; a real one carries a plausible generation. */
	if (rev == 0 || rev == 0xffffffff) {
		log_error("no GENET at %p (SYS_REV_CTRL = 0x%08x)", (void *)base, rev);
		return 0;
	}

	major = GENET_MAJOR(rev);
	major = major == 6 ? 5 : major == 5 ? 4 : major == 0 ? 1 : major;

	log_info("genet v%u.%u at %p, SYS_REV_CTRL 0x%08x", major, GENET_MINOR(rev),
	    (void *)base, rev);

	if (major != 5) {
		/* Not fatal: this driver only ever touches registers GENET has had
		 * since v3, so report the surprise and carry on. */
		log_error("expected GENET v5 on BCM2711, got v%u", major);
	}

	genet_umac_reset(base);

	id1 = genet_mdio_read(base, PHY_ADDR, MII_PHYSID1);
	id2 = genet_mdio_read(base, PHY_ADDR, MII_PHYSID2);
	if (id1 < 0 || id2 < 0) {
		log_error("no PHY answered at MDIO address %u", (unsigned)PHY_ADDR);
		return 0;
	}

	log_info("PHY id 0x%08x at MDIO address %u",
	    ((uint32_t)id1 << 16) | (uint32_t)id2, (unsigned)PHY_ADDR);

	genet_rgmii_init(base);

	/* Try the cached view of the buffer, then the uncached one. */
	if (genet_mailbox_mac(genet_state.mac, 0) == 0
	    || genet_mailbox_mac(genet_state.mac, 0xC0000000u) == 0) {
		genet_state.mac_ok = 1;
		log_info("MAC %02x:%02x:%02x:%02x:%02x:%02x from the VideoCore OTP",
		    genet_state.mac[0], genet_state.mac[1], genet_state.mac[2],
		    genet_state.mac[3], genet_state.mac[4], genet_state.mac[5]);
	}
	else {
		/* Locally administered, and spells GENET after the 02. Not a reason
		 * to give up: the MAC transmits with any source address. */
		static const uint8_t fallback[6] = {0x02, 0x47, 0x45, 0x4e, 0x45, 0x54};

		memcpy(genet_state.mac, fallback, sizeof(fallback));
		log_error("no MAC from the mailbox; using the fallback "
		          "02:47:45:4e:45:54");
	}
	genet_set_hwaddr(base, genet_state.mac);

	/* Autonegotiation takes seconds against a real switch and nothing at
	 * boot needs the link, so it runs in a thread. */
	if (!thread_create(0, genet_link_thread, NULL)) {
		log_error("could not start the link thread; negotiating inline");
		genet_link_bringup();
	}

	return 0;
}

/* The rest of bring-up, off the boot path: negotiate, configure the MAC for
 * whatever was negotiated, start the rings, and register the interface. */
static void genet_link_bringup(void) {
	uintptr_t base = genet_state.base;

	genet_link_up(base, PHY_ADDR);

	if (!genet_state.link) {
		log_error("no link, not starting the interface");
		return;
	}

	genet_adjust_link(base);
	genet_tx_ring_init(base);
	genet_tx_enable(base);

	genet_netdev_up();
}

static void *genet_link_thread(void *arg) {
	(void)arg;
	genet_link_bringup();
	return NULL;
}

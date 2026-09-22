/**
 * @file
 * @brief SD host driver for the BCM2711 EMMC2 controller
 *
 * An Arasan SD Host Controller 3.00 behind the Raspberry Pi 4 microSD slot.
 * Card identification, the block device and the MBR partitions are stock
 * embox.driver.mmc.mmc_core; this is only the host side.
 *
 * PIO by default, polled: there is no interrupt handler, INT_ENABLE is on so
 * the status bits latch and INT_SIGNAL_ENABLE stays off so nothing reaches
 * the GIC. SDMA is optional and used only for this driver's own buffers.
 *
 *   real Pi 4  0xFE340000  EMMC2, the microSD slot (device tree: emmc2)
 *   QEMU       0xFE300000  raspi4b hangs -drive if=sd off the GPIO mux,
 *                          which reaches the legacy EMMC, never EMMC2
 *
 * Hence base_addr is an option. Never aim at 0xFE300000 on real hardware:
 * there the node is disabled and the pins carry the WiFi SDIO.
 *
 * Run on a Pi 4B: identification, reads and writes at one data line and
 * default speed, PIO. Four data lines, high speed and SDMA are implemented,
 * verify themselves by reading blocks back, and are off by default.
 */

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <drivers/block_dev.h>
#include <drivers/common/memory.h>
#include <drivers/mmc/mmc_core.h>
#include <drivers/mmc/mmc_host.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/cache.h>
#include <hal/mem_barriers.h>
#include <hal/reg.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/time/ktime.h>
#include <util/log.h>

#include "sdhci_std.h"

#define BASE_ADDR      OPTION_GET(NUMBER, base_addr)
#define FALLBACK_CLOCK OPTION_GET(NUMBER, fallback_clock)
#define BUS_WIDTH_4    OPTION_GET(NUMBER, bus_width_4)
#define HIGH_SPEED     OPTION_GET(NUMBER, high_speed)
#define READAHEAD      OPTION_GET(NUMBER, readahead)
#define WRITE_COMBINE  OPTION_GET(NUMBER, write_combine)
#define USE_DMA        OPTION_GET(NUMBER, use_dma)

/* The Cortex-A72's data cache line. A buffer handed to SDMA must own every
 * line it touches: see emmc2_dma_usable(). */
#define EMMC2_DMA_ALIGN 64u

/* emmc2 sits behind its own bus: per emmc2bus dma-ranges in
 * bcm2711-rpi-4-b.dtb a bus address is the ARM physical one plus
 * 0xC0000000, and only the first gigabyte of DRAM is reachable. RAM on this
 * board runs past that, so every address is checked rather than assumed. */
#define EMMC2_DMA_BUS_BASE OPTION_GET(NUMBER, dma_bus_base)
#define EMMC2_DMA_LIMIT    0x40000000u

/* Identification runs at <= 400 kHz, then the card moves to default speed.
 * High speed (50 MHz) needs CMD6 and HOST_CONTROL.HISPD. */
#define EMMC2_ID_CLOCK  400000u
#define EMMC2_RUN_CLOCK 25000000u
#define EMMC2_HS_CLOCK  50000000u

/* ACMD6 argument: 0 = one data line, 2 = four. */
#define SD_ACMD_SET_BUS_WIDTH 6
#define SD_BUS_WIDTH_1        0
#define SD_BUS_WIDTH_4        2

/* CMD6, set mode, group 1 (access mode) = 1 (high speed), rest unchanged.
 * The card answers with 64 bytes whose byte 16 low nibble is the function
 * it actually selected. */
#define SD_CMD_SWITCH_FUNC   6
#define SD_SWITCH_TO_HS      0x80fffff1u
#define SD_SWITCH_STATUS_LEN 64

#define SD_CMD_READ_SINGLE_BLOCK    17
#define SD_CMD_READ_MULTIPLE_BLOCK  18
#define SD_CMD_WRITE_BLOCK          24
#define SD_CMD_WRITE_MULTIPLE_BLOCK 25

#define EMMC2_BLOCK_SIZE 512u

/* Any base clock outside this window is a reporting bug, not a clock. */
#define EMMC2_CLOCK_MIN 1000000u
#define EMMC2_CLOCK_MAX 400000000u

/* Poll tight first (a command answers in microseconds), then fall back to
 * 1 ms sleeps so a dead card cannot hold the CPU for its whole timeout. */
#define EMMC2_SPINS 100000u

/* Kept short on purpose: mmc_try_sd polls ACMD41 a hundred times, so a card
 * that is not there delays boot by a hundred times this. */
#define EMMC2_CMD_TIMEOUT_MS  200u
#define EMMC2_DATA_TIMEOUT_MS 2000u
#define EMMC2_BUSY_TIMEOUT_MS 5000u

/* Writes get their own, much longer, deadline: a card doing a
 * read-modify-write of a whole erase block per 512 bytes, and relocating one
 * now and then, can stall well past the deadline a read is given. */
#define EMMC2_WRITE_TIMEOUT_MS 15000u

struct emmc2_host {
	uintptr_t base;
	uint32_t base_clock; /* Hz, controller input */
	uint32_t cur_clock;  /* Hz, SDCLK as last programmed */
	struct mmc_host *mmc;
};

static struct emmc2_host emmc2_host;

/* What the probe and the tuning settled on. */
static struct {
	uint32_t caps;      /* CAPABILITIES register */
	uint8_t identified; /* mmc_scan() succeeded */
	uint8_t bus_width;  /* data lines in use: 1 or 4 */
	uint8_t high_speed; /* card and host switched to high speed */
	uint8_t dma;        /* SDMA verified against PIO and in use */
} emmc2_state;

PERIPH_MEMORY_DEFINE(bcm2711_emmc2, BASE_ADDR, 0x1000);

/* ------------------------------------------------------ mailbox: EMMC2 clock
 *
 * CAPABILITIES may report base clock 0, leaving the VideoCore firmware as
 * the only source. Embox's bcm2835_mailbox_property stops its clock enum at
 * PWM (10) and defaults to the Pi 1 mailbox base, hence the raw
 * property-channel access here.
 */

#define MBOX_BASE   0xFE00B880UL
#define MBOX_READ   (MBOX_BASE + 0x00)
#define MBOX_STATUS (MBOX_BASE + 0x18)
#define MBOX_WRITE  (MBOX_BASE + 0x20)

#define MBOX_FULL      0x80000000u
#define MBOX_EMPTY     0x40000000u
#define MBOX_CHAN_TAGS 8u
#define MBOX_RESP_OK   0x80000000u

#define TAG_GET_CLOCK_RATE 0x30002u
#define MBOX_CLOCK_EMMC2   12u /* not in Embox's enum, which ends at PWM */

#define MBOX_SPINS 10000000u

PERIPH_MEMORY_DEFINE(bcm2711_emmc2_mbox, MBOX_BASE, 0x40);

static uint32_t emmc2_mbox_clock(void) {
	uint32_t buf[8] __attribute__((aligned(16)));
	uint32_t msg;
	uint32_t got;
	uint32_t spins;

	memset(buf, 0, sizeof(buf));
	buf[0] = sizeof(buf);
	buf[1] = 0; /* request */
	buf[2] = TAG_GET_CLOCK_RATE;
	buf[3] = 8; /* value buffer size */
	buf[4] = 8; /* request size */
	buf[5] = MBOX_CLOCK_EMMC2;
	buf[6] = 0; /* rate in Hz comes back here */
	buf[7] = 0; /* end tag */

	msg = (uint32_t)(uintptr_t)buf;
	if (msg & 0xfu) {
		log_error("mailbox buffer not 16-aligned");
		return 0;
	}

	dcache_flush(buf, sizeof(buf));
	dsb(sy);

	for (spins = 0; REG32_LOAD(MBOX_STATUS) & MBOX_FULL; spins++) {
		if (spins > MBOX_SPINS) {
			log_error("mailbox write timeout");
			return 0;
		}
	}
	dmb(sy);
	REG32_STORE(MBOX_WRITE, msg | MBOX_CHAN_TAGS);

	spins = 0;
	for (;;) {
		while (REG32_LOAD(MBOX_STATUS) & MBOX_EMPTY) {
			if (++spins > MBOX_SPINS) {
				log_error("mailbox read timeout");
				return 0;
			}
		}
		got = REG32_LOAD(MBOX_READ);
		if ((got & 0xfu) == MBOX_CHAN_TAGS) {
			break;
		}
	}

	dcache_inval(buf, sizeof(buf));
	dsb(sy);

	if ((got & ~0xfu) != msg || buf[1] != MBOX_RESP_OK) {
		log_error("mailbox reply %08x code %08x", got, buf[1]);
		return 0;
	}

	return buf[6];
}

/* ------------------------------------------------------------- primitives */

static int emmc2_reset(struct emmc2_host *host, uint8_t mask) {
	unsigned spins;

	sdhci_wr8(host->base, SDHCI_SOFTWARE_RESET, mask);

	for (spins = 0; sdhci_rd8(host->base, SDHCI_SOFTWARE_RESET) & mask; spins++) {
		if (spins > EMMC2_SPINS) {
			log_error("reset %02x did not clear", mask);
			return -ETIMEDOUT;
		}
	}

	return 0;
}

static int emmc2_wait_state(struct emmc2_host *host, uint32_t mask, unsigned ms) {
	unsigned spins = 0;
	unsigned slept = 0;
	uint32_t state;

	for (;;) {
		state = sdhci_rd32(host->base, SDHCI_PRESENT_STATE);
		if (!(state & mask)) {
			return 0;
		}
		if (spins < EMMC2_SPINS) {
			spins++;
			continue;
		}
		if (slept >= ms) {
			break;
		}
		usleep(USEC_PER_MSEC);
		slept++;
	}

	log_error("present state %08x still busy on %08x after %u sleep(s)", state,
	    mask, slept);
	return -ETIMEDOUT;
}

/* Returns 0 when one of @mask latched, -EIO when the controller flagged an
 * error, -ETIMEDOUT otherwise. The status word is left for the caller to
 * clear. */
static int emmc2_wait_int(struct emmc2_host *host, uint32_t mask, unsigned ms) {
	unsigned spins = 0;
	unsigned slept = 0;
	uint32_t sts = 0;

	for (;;) {
		sts = sdhci_rd32(host->base, SDHCI_INT_STATUS);
		/* Errors before the wanted bit: the status word is cleared before
		 * every command, and the two can arrive in the same read -- a data
		 * CRC error on the last block of a multi-block transfer sets both
		 * DATA_END and DATA_CRC. */
		if (sts & SDHCI_INT_ERROR) {
			log_error("error irq: normal=%04x error=%04x (waited %08x)",
			    sts & 0xffff, sts >> 16, mask);
			return -EIO;
		}
		if (sts & mask) {
			return 0;
		}
		if (spins < EMMC2_SPINS) {
			spins++;
			continue;
		}
		if (slept >= ms) {
			break;
		}
		usleep(USEC_PER_MSEC);
		slept++;
	}

	log_error("timeout on irq %08x, status %08x after %u sleep(s)", mask, sts,
	    slept);
	return -ETIMEDOUT;
}

static int emmc2_set_clock(struct emmc2_host *host, uint32_t hz) {
	uint32_t div;
	uint16_t clk;
	unsigned spins;

	sdhci_wr16(host->base, SDHCI_CLOCK_CONTROL, 0);

	if (hz == 0) {
		host->cur_clock = 0;
		return 0;
	}

	/* SDCLK = base / (2 * div); div == 0 means base itself. */
	div = 0;
	if (host->base_clock > hz) {
		for (div = 1; div < SDHCI_DIV_MASK; div++) {
			if (host->base_clock / (2 * div) <= hz) {
				break;
			}
		}
	}

	clk = SDHCI_MAKE_DIV(div) | SDHCI_CLOCK_INT_EN;
	sdhci_wr16(host->base, SDHCI_CLOCK_CONTROL, clk);

	for (spins = 0;
	    !(sdhci_rd16(host->base, SDHCI_CLOCK_CONTROL) & SDHCI_CLOCK_INT_STABLE);
	    spins++) {
		if (spins > EMMC2_SPINS) {
			log_error("internal clock never stabilised");
			return -ETIMEDOUT;
		}
	}

	sdhci_wr16(host->base, SDHCI_CLOCK_CONTROL, clk | SDHCI_CLOCK_CARD_EN);

	host->cur_clock = div ? host->base_clock / (2 * div) : host->base_clock;
	log_debug("SDCLK %u Hz (base %u, div %u)", host->cur_clock,
	    host->base_clock, div);

	return 0;
}

/* ---------------------------------------------------------- command layer */

static void emmc2_read_response(struct emmc2_host *host, struct mmc_command *cmd) {
	int i;

	if (!(cmd->flags & MMC_RSP_PRESENT)) {
		return;
	}

	if (cmd->flags & MMC_RSP_136) {
		/* The controller strips the CRC byte, so R2 needs the same
		 * shift-and-stitch Embox's uSDHC driver does — that code is
		 * standard-SDHCI-correct and sd.c's CSD parsing expects it. */
		for (i = 0; i < 4; i++) {
			cmd->resp[i] = sdhci_rd32(host->base, SDHCI_RESPONSE + (3 - i) * 4)
			               << 8;
			if (i != 3) {
				cmd->resp[i] |= sdhci_rd8(host->base,
				    SDHCI_RESPONSE + (3 - i) * 4 - 1);
			}
		}
	}
	else {
		cmd->resp[0] = sdhci_rd32(host->base, SDHCI_RESPONSE);
	}
}

/* The bus translation is a property of the machine, not of the driver, so it
 * is verified rather than trusted: the first DMA transfer is compared with a
 * PIO read of the same block, and DMA stays off for good if they differ. */
#define EMMC2_DMA_UNTESTED 0
#define EMMC2_DMA_GOOD     1
#define EMMC2_DMA_OFF      2

static int emmc2_dma_state = EMMC2_DMA_UNTESTED;

/* Set only around the read-ahead fill: identification, tuning and the reads
 * that verify them must go down the path already known to work. */
static int emmc2_dma_wanted;

static void emmc2_do_request(struct mmc_host *mmc, struct mmc_request *req);
static int emmc2_read_block_raw(struct mmc_host *mmc, uint32_t blkno, void *buf);

/* Can the controller reach this buffer at all? */
static int emmc2_dma_usable(uintptr_t addr, size_t len) {
	if (!USE_DMA || !emmc2_dma_wanted || emmc2_dma_state == EMMC2_DMA_OFF) {
		return 0;
	}
	if (!(emmc2_state.caps & SDHCI_CAP_SDMA)) {
		/* No SDMA: the request would be ignored and the payload left in
		 * the FIFO. */
		return 0;
	}
	/* Whole cache lines only, at both ends: a read ends in dcache_inval(),
	 * and DC IVAC discards the whole line, neighbouring bytes written while
	 * the transfer ran included. Anything unaligned goes by PIO. */
	if ((addr | len) & (EMMC2_DMA_ALIGN - 1)) {
		return 0;
	}
	if (addr + len > EMMC2_DMA_LIMIT) {
		return 0;
	}
	return 1;
}

/* SDMA: the controller moves the payload itself, so the CPU only waits. Not
 * coherent with the ARM caches, so the buffer is cleaned before the transfer
 * -- a dirty line written back afterwards would land on top of what the card
 * delivered -- and invalidated after it. */
static int emmc2_dma(struct emmc2_host *host, struct mmc_request *req) {
	size_t len = req->data.blksz * (req->data.blocks ? req->data.blocks : 1);
	int is_read = !!(req->cmd.flags & MMC_DATA_READ);
	uint32_t sts;
	int err;

	for (;;) {
		err = emmc2_wait_int(host, SDHCI_INT_DATA_END | SDHCI_INT_DMA_END,
		    EMMC2_DATA_TIMEOUT_MS);
		if (err) {
			return err;
		}

		sts = sdhci_rd32(host->base, SDHCI_INT_STATUS);
		if (sts & SDHCI_INT_DATA_END) {
			sdhci_wr32(host->base, SDHCI_INT_STATUS, SDHCI_INT_DATA_END);
			break;
		}

		/* The 512 KiB SDMA boundary was crossed and the transfer stopped:
		 * acknowledge and hand the address back to restart it. */
		sdhci_wr32(host->base, SDHCI_INT_STATUS, SDHCI_INT_DMA_END);
		sdhci_wr32(host->base, SDHCI_SDMA_ADDRESS,
		    sdhci_rd32(host->base, SDHCI_SDMA_ADDRESS));
	}

	if (is_read) {
		dcache_inval((void *)req->data.addr, len);
	}

	req->data.bytes_xfered = len;
	return 0;
}

static int emmc2_pio(struct emmc2_host *host, struct mmc_request *req) {
	uint32_t *words;
	uint32_t staging[EMMC2_BLOCK_SIZE / sizeof(uint32_t)];
	int is_read = !!(req->cmd.flags & MMC_DATA_READ);
	size_t nwords;
	size_t nblocks;
	size_t blk;
	size_t i;
	int err;

	nwords = req->data.blksz / sizeof(uint32_t);
	nblocks = req->data.blocks ? req->data.blocks : 1;

	/* bcache hands out aligned buffers, but nothing in the block layer
	 * promises it. Stage through the stack when it does not. */
	if ((req->data.addr & 3) && nblocks == 1) {
		words = staging;
		if (!is_read) {
			memcpy(staging, (void *)req->data.addr, req->data.blksz);
		}
	}
	else {
		words = (uint32_t *)req->data.addr;
	}

	/* One FIFO round per block: the controller raises the ready bit again
	 * for each block of a multi-block transfer. */
	for (blk = 0; blk < nblocks; blk++) {
		uint32_t *dst = words + blk * nwords;

		err = emmc2_wait_int(host,
		    is_read ? SDHCI_INT_DATA_AVAIL : SDHCI_INT_SPACE_AVAIL,
		    is_read ? EMMC2_DATA_TIMEOUT_MS : EMMC2_WRITE_TIMEOUT_MS);
		if (err) {
			return err;
		}
		sdhci_wr32(host->base, SDHCI_INT_STATUS,
		    is_read ? SDHCI_INT_DATA_AVAIL : SDHCI_INT_SPACE_AVAIL);

		if (is_read) {
			for (i = 0; i < nwords; i++) {
				dst[i] = sdhci_rd32(host->base, SDHCI_BUFFER);
			}
		}
		else {
			for (i = 0; i < nwords; i++) {
				sdhci_wr32(host->base, SDHCI_BUFFER, dst[i]);
			}
		}
	}

	err = emmc2_wait_int(host, SDHCI_INT_DATA_END,
	    is_read ? EMMC2_DATA_TIMEOUT_MS : EMMC2_WRITE_TIMEOUT_MS);
	if (err) {
		return err;
	}
	sdhci_wr32(host->base, SDHCI_INT_STATUS, SDHCI_INT_DATA_END);

	log_debug("%s blk: first %08x %08x last %08x", is_read ? "read" : "wrote",
	    words[0], words[1], words[nwords - 1]);

	if (is_read && words == staging) {
		memcpy((void *)req->data.addr, staging, req->data.blksz);
	}

	req->data.bytes_xfered = req->data.blksz * nblocks;
	return 0;
}

static void emmc2_do_request(struct mmc_host *mmc, struct mmc_request *req) {
	struct emmc2_host *host = mmc->priv;
	uint32_t inhibit;
	uint32_t mode;
	uint32_t cmd;
	uint32_t flags;
	int has_data;
	int use_dma;
	int err;

	flags = req->cmd.flags;
	has_data = !!(flags & MMC_DATA_XFER);
	req->cmd.error = 0;

	inhibit = SDHCI_CMD_INHIBIT;
	if (has_data || (flags & MMC_RSP_BUSY)) {
		inhibit |= SDHCI_DATA_INHIBIT;
	}
	err = emmc2_wait_state(host, inhibit, EMMC2_CMD_TIMEOUT_MS);
	if (err) {
		req->cmd.error = -EBUSY;
		goto recover;
	}

	sdhci_wr32(host->base, SDHCI_INT_STATUS, 0xffffffff);

	/* Response type. mmc_core marks CMD17/CMD24 R1B, but a data command
	 * must not ask the controller for a busy response — the busy phase
	 * after the payload is what DATA_INHIBIT reports. Drop BUSY here. */
	cmd = 0;
	if (!(flags & MMC_RSP_PRESENT)) {
		cmd |= SDHCI_CMD_RESP_NONE;
	}
	else if (flags & MMC_RSP_136) {
		cmd |= SDHCI_CMD_RESP_LONG;
	}
	else if ((flags & MMC_RSP_BUSY) && !has_data) {
		cmd |= SDHCI_CMD_RESP_SHORT_BUSY;
	}
	else {
		cmd |= SDHCI_CMD_RESP_SHORT;
	}
	if (flags & MMC_RSP_CRC) {
		cmd |= SDHCI_CMD_CRC;
	}
	if (flags & MMC_RSP_OPCODE) {
		cmd |= SDHCI_CMD_INDEX;
	}

	mode = 0;
	use_dma = 0;
	if (has_data) {
		size_t len = req->data.blksz * (req->data.blocks ? req->data.blocks : 1);

		cmd |= SDHCI_CMD_DATA;
		mode = SDHCI_TRNS_BLK_CNT_EN;
		if (flags & MMC_DATA_READ) {
			mode |= SDHCI_TRNS_READ;
		}
		if (emmc2_dma_usable(req->data.addr, len)) {
			use_dma = 1;
			mode |= SDHCI_TRNS_DMA;
			/* Clean in both directions: a write needs the payload in
			 * memory, a read must leave no dirty line to be written back
			 * over what the card delivers. */
			dcache_flush((void *)req->data.addr, len);
			sdhci_wr32(host->base, SDHCI_SDMA_ADDRESS,
			    (uint32_t)req->data.addr + EMMC2_DMA_BUS_BASE);
		}
		if (req->data.blocks > 1) {
			mode |= SDHCI_TRNS_MULTI | SDHCI_TRNS_AUTO_CMD12;
		}
		sdhci_wr32(host->base, SDHCI_BLOCK_SIZE,
		    SDHCI_MAKE_BLKSZ(7, req->data.blksz)
		        | ((uint32_t)(req->data.blocks ? req->data.blocks : 1) << 16));
		sdhci_wr8(host->base, SDHCI_TIMEOUT_CONTROL, SDHCI_MAX_TIMEOUT_CONTROL);
	}

	sdhci_wr32(host->base, SDHCI_ARGUMENT, req->cmd.arg);

	log_debug("cmd %u arg %08x flags %04x -> cmdreg %04x mode %04x",
	    req->cmd.opcode, req->cmd.arg, flags,
	    SDHCI_MAKE_CMD(req->cmd.opcode, cmd), mode);

	/* One 32-bit store: TRANSFER_MODE in the low half, COMMAND in the
	 * high half. Writing COMMAND is what issues the command. */
	sdhci_wr32(host->base, SDHCI_TRANSFER_MODE,
	    mode | (SDHCI_MAKE_CMD(req->cmd.opcode, cmd) << 16));

	err = emmc2_wait_int(host, SDHCI_INT_RESPONSE, EMMC2_CMD_TIMEOUT_MS);
	if (err) {
		req->cmd.error = err;
		goto recover;
	}
	sdhci_wr32(host->base, SDHCI_INT_STATUS, SDHCI_INT_RESPONSE);

	emmc2_read_response(host, &req->cmd);

	if (has_data) {
		err = use_dma ? emmc2_dma(host, req) : emmc2_pio(host, req);
		if (err) {
			req->cmd.error = err;
			goto recover;
		}
	}

	/* A busy response (and the programming phase after a write) holds
	 * DAT0 low. Nothing else may be issued until it lifts. */
	if ((flags & MMC_RSP_BUSY) || (flags & MMC_DATA_WRITE)) {
		if (emmc2_wait_state(host, SDHCI_DATA_INHIBIT, EMMC2_BUSY_TIMEOUT_MS)) {
			req->cmd.error = -ETIMEDOUT;
			goto recover;
		}
	}

	return;

recover:
	/* Leave the controller usable for the next probe attempt: mmc_scan
	 * walks SDIO -> SD -> MMC and expects a failed command to be
	 * survivable. */
	log_debug("recovering after cmd %u (err %d)", req->cmd.opcode, req->cmd.error);
	emmc2_reset(host, SDHCI_RESET_CMD);
	if (has_data) {
		emmc2_reset(host, SDHCI_RESET_DATA);
	}
	sdhci_wr32(host->base, SDHCI_INT_STATUS, 0xffffffff);
}

/* --------------------------------------------------------- read-ahead
 *
 * Most of what a 512-byte block costs is command latency, not bandwidth, and
 * the block layer asks for one block at a time with no way to widen that
 * contract from here. So a CMD17 for block N becomes a CMD18 for N..N+K-1
 * and the next K-1 requests are answered from RAM. Anything that is not a
 * plain single-block read throws the window away.
 */

/* One controller, more than one thread that wants it, and nothing below this
 * point is re-entrant: two threads in here at once interleave register
 * writes into one command and consume each other's interrupt status. Neither
 * mmc_core nor Embox's block layer serialises anything. Recursive because
 * the request path reaches emmc2_read_block_raw, which takes it too.
 */
static struct mutex emmc2_mutex = RMUTEX_INIT_STATIC;

static void emmc2_lock(void) {
	mutex_lock(&emmc2_mutex);
}

static void emmc2_unlock(void) {
	mutex_unlock(&emmc2_mutex);
}

#define EMMC2_RA_MAX 32

static uint8_t emmc2_ra_buf[EMMC2_RA_MAX * EMMC2_BLOCK_SIZE]
    __attribute__((aligned(64)));
static uint32_t emmc2_ra_base;
static uint32_t emmc2_ra_have;   /* blocks currently cached, 0 = empty */
static uint32_t emmc2_ra_window; /* blocks to fetch at a time, 0 = disabled */

static void emmc2_set_readahead(unsigned blocks) {
	emmc2_lock();
	emmc2_ra_window = blocks > EMMC2_RA_MAX ? EMMC2_RA_MAX : blocks;
	emmc2_ra_have = 0;
	emmc2_unlock();
}

static uint32_t emmc2_blk_of(struct mmc_host *mmc, uint32_t arg) {
	return mmc->high_capacity ? arg : arg / EMMC2_BLOCK_SIZE;
}

static int wt_all_zero(const uint8_t *buf, size_t len) {
	size_t i;

	for (i = 0; i < len; i++) {
		if (buf[i]) {
			return 0;
		}
	}
	return 1;
}

static int emmc2_ra_fill(struct mmc_host *mmc, uint32_t blkno) {
	uint8_t check[EMMC2_BLOCK_SIZE] __attribute__((aligned(8)));
	struct mmc_request req;
	uint32_t count;
	uint64_t total;

	count = emmc2_ra_window;
	total = mmc->bdev ? mmc->bdev->size / EMMC2_BLOCK_SIZE : 0;
	if (total && blkno + count > total) {
		count = (uint32_t)(total - blkno);
	}
	if (count < 2) {
		return -1; /* nothing to amortise */
	}

	memset(&req, 0, sizeof(req));
	req.cmd.opcode = SD_CMD_READ_MULTIPLE_BLOCK;
	req.cmd.arg = mmc->high_capacity ? blkno : blkno * EMMC2_BLOCK_SIZE;
	req.cmd.flags = MMC_RSP_R1 | MMC_DATA_READ;
	req.data.addr = (uintptr_t)emmc2_ra_buf;
	req.data.blksz = EMMC2_BLOCK_SIZE;
	req.data.blocks = count;

	emmc2_dma_wanted = 1;
	emmc2_do_request(mmc, &req);
	emmc2_dma_wanted = 0;
	if (req.cmd.error) {
		emmc2_ra_have = 0;
		return req.cmd.error;
	}

	if (USE_DMA && (emmc2_state.caps & SDHCI_CAP_SDMA)
	    && emmc2_dma_state == EMMC2_DMA_UNTESTED) {
		/* Force the comparison read down the PIO path. */
		emmc2_dma_state = EMMC2_DMA_OFF;
		if (emmc2_read_block_raw(mmc, blkno, check) == 0
		    && memcmp(check, emmc2_ra_buf, EMMC2_BLOCK_SIZE) == 0) {
			emmc2_dma_state = EMMC2_DMA_GOOD;
			emmc2_state.dma = 1;
			log_info("SDMA verified, bus base 0x%08x",
			    (unsigned)EMMC2_DMA_BUS_BASE);
		}
		else {
			const uint32_t *got = (const uint32_t *)emmc2_ra_buf;
			const uint32_t *want = (const uint32_t *)check;
			int zero = wt_all_zero(emmc2_ra_buf, EMMC2_BLOCK_SIZE);

			log_error("SDMA gave different bytes than PIO; disabling it");
			log_error("  buffer va %p  bus 0x%08x  base 0x%08x  %s",
			    (void *)emmc2_ra_buf,
			    (unsigned)((uintptr_t)emmc2_ra_buf + EMMC2_DMA_BUS_BASE),
			    (unsigned)EMMC2_DMA_BUS_BASE,
			    zero ? "buffer untouched (all zero)" : "buffer has content");
			log_error("  dma  %08x %08x %08x %08x", got[0], got[1], got[2], got[3]);
			log_error("  pio  %08x %08x %08x %08x", want[0], want[1], want[2],
			    want[3]);

			/* A failed DMA leaves the data path in a state that slows every
			 * later request; reset it properly. */
			emmc2_reset(&emmc2_host, SDHCI_RESET_DATA);
			emmc2_reset(&emmc2_host, SDHCI_RESET_CMD);
			sdhci_wr32(emmc2_host.base, SDHCI_INT_STATUS, 0xffffffff);

			emmc2_ra_have = 0;
			return -EIO;
		}
	}

	emmc2_ra_base = blkno;
	emmc2_ra_have = count;
	return 0;
}

/* ----------------------------------------------------- write combining
 *
 * The mirror image of the read-ahead above: consecutive single-block writes
 * are accumulated and issued as one CMD25. Anything that is not a
 * continuation -- a read included -- flushes the buffer first. The cost, and
 * why the window is an option: a write is acknowledged before the card has
 * taken it, and a failed run is reported to whoever asks next.
 */
#define EMMC2_WB_MAX 32

static uint8_t emmc2_wb_buf[EMMC2_WB_MAX * EMMC2_BLOCK_SIZE]
    __attribute__((aligned(64)));

static uint32_t emmc2_wb_base;   /* first block held */
static uint32_t emmc2_wb_have;   /* blocks held, 0 = empty */
static uint32_t emmc2_wb_window; /* blocks to gather; 0 or 1 = off */
static int emmc2_wb_error;       /* a run that failed, not yet reported */

/* Send what is held. Returns the error, and remembers it for the caller that
 * has not asked yet. Must be called with the lock. */
static int emmc2_wb_flush(struct mmc_host *mmc) {
	struct mmc_request req;
	uint32_t have = emmc2_wb_have;

	if (!have) {
		return 0;
	}
	emmc2_wb_have = 0; /* before the request: a failure must not re-enter */

	memset(&req, 0, sizeof(req));
	req.cmd.opcode = have > 1 ? SD_CMD_WRITE_MULTIPLE_BLOCK : SD_CMD_WRITE_BLOCK;
	req.cmd.arg = mmc->high_capacity ? emmc2_wb_base
	                                 : emmc2_wb_base * EMMC2_BLOCK_SIZE;
	req.cmd.flags = MMC_RSP_R1B | MMC_DATA_WRITE;
	req.data.addr = (uintptr_t)emmc2_wb_buf;
	req.data.blksz = EMMC2_BLOCK_SIZE;
	req.data.blocks = have;

	emmc2_do_request(mmc, &req);

	if (req.cmd.error) {
		emmc2_wb_error = req.cmd.error;
		log_error("combined write of %u blocks at %u failed: %d",
		    (unsigned)have, (unsigned)emmc2_wb_base, req.cmd.error);
	}
	return req.cmd.error;
}

/* A failure from an earlier run, handed to the next caller. */
static int emmc2_wb_take_error(void) {
	int err = emmc2_wb_error;

	emmc2_wb_error = 0;
	return err;
}

static void emmc2_set_write_combine(unsigned blocks) {
	emmc2_lock();
	if (emmc2_host.mmc) {
		emmc2_wb_flush(emmc2_host.mmc);
	}
	emmc2_wb_window = blocks > EMMC2_WB_MAX ? EMMC2_WB_MAX : blocks;
	emmc2_unlock();
}

static void emmc2_request_locked(struct mmc_host *mmc, struct mmc_request *req) {
	uint32_t blkno;

	/* A plain single-block write that continues the run: buffer it and
	 * acknowledge at once. */
	if (emmc2_wb_window >= 2 && req->cmd.opcode == SD_CMD_WRITE_BLOCK
	    && (req->cmd.flags & MMC_DATA_WRITE) && req->data.blocks == 1
	    && req->data.blksz == EMMC2_BLOCK_SIZE) {
		uint32_t wblk = emmc2_blk_of(mmc, req->cmd.arg);

		if (emmc2_wb_have && wblk != emmc2_wb_base + emmc2_wb_have) {
			emmc2_wb_flush(mmc);
		}
		if (!emmc2_wb_have) {
			emmc2_wb_base = wblk;
		}
		memcpy(emmc2_wb_buf + emmc2_wb_have * EMMC2_BLOCK_SIZE,
		    (const void *)req->data.addr, EMMC2_BLOCK_SIZE);
		emmc2_wb_have++;
		emmc2_ra_have = 0; /* the read window may now name stale blocks */
		req->data.bytes_xfered = EMMC2_BLOCK_SIZE;
		req->cmd.error = emmc2_wb_take_error();
		if (emmc2_wb_have >= emmc2_wb_window) {
			emmc2_wb_flush(mmc);
		}
		return;
	}

	/* Not a continuation: whatever is held goes to the card first, reads
	 * included. */
	if (emmc2_wb_have) {
		emmc2_wb_flush(mmc);
	}

	if (emmc2_ra_window < 2 || req->cmd.opcode != SD_CMD_READ_SINGLE_BLOCK
	    || !(req->cmd.flags & MMC_DATA_READ) || req->data.blocks > 1
	    || req->data.blksz != EMMC2_BLOCK_SIZE) {
		/* Not a plain single-block read: bypass the window and drop it. */
		emmc2_ra_have = 0;
		emmc2_do_request(mmc, req);
		return;
	}

	blkno = emmc2_blk_of(mmc, req->cmd.arg);

	if (emmc2_ra_have && blkno >= emmc2_ra_base
	    && blkno < emmc2_ra_base + emmc2_ra_have) {
		memcpy((void *)req->data.addr,
		    emmc2_ra_buf + (blkno - emmc2_ra_base) * EMMC2_BLOCK_SIZE,
		    EMMC2_BLOCK_SIZE);
		req->cmd.error = 0;
		req->data.bytes_xfered = EMMC2_BLOCK_SIZE;
		return;
	}

	if (emmc2_ra_fill(mmc, blkno) == 0) {
		memcpy((void *)req->data.addr, emmc2_ra_buf, EMMC2_BLOCK_SIZE);
		req->cmd.error = 0;
		req->data.bytes_xfered = EMMC2_BLOCK_SIZE;
		return;
	}

	/* Multi-block did not work here; the single-block path always does. */
	emmc2_ra_have = 0;
	emmc2_do_request(mmc, req);
}

/* Every command from mmc_core arrives here, and one at a time. */
static void emmc2_request(struct mmc_host *mmc, struct mmc_request *req) {
	emmc2_lock();
	emmc2_request_locked(mmc, req);
	emmc2_unlock();
}

static int emmc2_get_cd(struct mmc_host *mmc) {
	struct emmc2_host *host = mmc->priv;

	return (sdhci_rd32(host->base, SDHCI_PRESENT_STATE) & SDHCI_CARD_PRESENT) ? 1
	                                                                          : 0;
}

static int emmc2_get_ro(struct mmc_host *mmc) {
	struct emmc2_host *host = mmc->priv;

	return (sdhci_rd32(host->base, SDHCI_PRESENT_STATE) & SDHCI_WRITE_PROTECT)
	           ? 0
	           : 1;
}

static const struct mmc_host_ops emmc2_ops = {
    .request = emmc2_request,
    .get_cd = emmc2_get_cd,
    .get_ro = emmc2_get_ro,
};

/* ------------------------------------------------------------------ tuning
 *
 * Identification has to happen at 1 bit and 400 kHz; everything faster is
 * negotiated afterwards. Both steps below are self-checking: block 0 is read
 * before the change and again after, and anything that does not come back
 * identical is undone rather than left to corrupt silently. Commands travel
 * on CMD regardless of bus width, so a revert can always be sent.
 */

static int emmc2_read_block_raw(struct mmc_host *mmc, uint32_t blkno, void *buf) {
	struct mmc_request req;
	int err;

	memset(&req, 0, sizeof(req));
	req.cmd.opcode = 17;
	req.cmd.arg = mmc->high_capacity ? blkno : blkno * EMMC2_BLOCK_SIZE;
	req.cmd.flags = MMC_RSP_R1 | MMC_DATA_READ;
	req.data.addr = (uintptr_t)buf;
	req.data.blksz = EMMC2_BLOCK_SIZE;
	req.data.blocks = 1;

	/* Straight to the worker, never through the read-ahead wrapper: this is
	 * what verifies a bus-mode change and what checks SDMA against PIO, and
	 * both need bytes that came off the card just now. */
	emmc2_lock();
	emmc2_do_request(mmc, &req);
	err = req.cmd.error;
	emmc2_unlock();
	return err;
}

/* The card stays switched to the high-speed function once CMD6 grants it;
 * only the clock and the bus width move when benchmarking the two modes. */
static int emmc2_hs_granted;

static uint8_t emmc2_ref[EMMC2_BLOCK_SIZE] __attribute__((aligned(8)));
static uint8_t emmc2_cmp[EMMC2_BLOCK_SIZE] __attribute__((aligned(8)));

/* Does block 0 still read back the way it did before the change? */
static int emmc2_still_good(struct mmc_host *mmc) {
	if (emmc2_read_block_raw(mmc, 0, emmc2_cmp)) {
		return 0;
	}
	return memcmp(emmc2_ref, emmc2_cmp, EMMC2_BLOCK_SIZE) == 0;
}

static void emmc2_set_host_bus_width(struct emmc2_host *host, int four) {
	uint8_t ctrl;

	ctrl = sdhci_rd8(host->base, SDHCI_HOST_CONTROL);
	if (four) {
		ctrl |= SDHCI_CTRL_4BITBUS;
	}
	else {
		ctrl &= (uint8_t)~SDHCI_CTRL_4BITBUS;
	}
	sdhci_wr8(host->base, SDHCI_HOST_CONTROL, ctrl);
}

static void emmc2_try_4bit(struct emmc2_host *host, struct mmc_host *mmc) {
	uint32_t resp[4];

	/* Card first, then host: between the two the link is inconsistent, and
	 * only the host side can be fixed without talking to the card. */
	mmc_send_cmd(mmc, 55, mmc->rca << 16, MMC_RSP_R1, resp);
	mmc_send_cmd(mmc, SD_ACMD_SET_BUS_WIDTH, SD_BUS_WIDTH_4, MMC_RSP_R1, resp);
	emmc2_set_host_bus_width(host, 1);

	if (emmc2_still_good(mmc)) {
		emmc2_state.bus_width = 4;
		log_info("bus width 4");
		return;
	}

	log_error("4-bit bus did not verify, falling back to 1 bit");
	emmc2_set_host_bus_width(host, 0);
	mmc_send_cmd(mmc, 55, mmc->rca << 16, MMC_RSP_R1, resp);
	mmc_send_cmd(mmc, SD_ACMD_SET_BUS_WIDTH, SD_BUS_WIDTH_1, MMC_RSP_R1, resp);
	if (!emmc2_still_good(mmc)) {
		log_error("card did not come back at 1 bit either");
	}
}

static void emmc2_try_high_speed(struct emmc2_host *host, struct mmc_host *mmc) {
	uint8_t status[SD_SWITCH_STATUS_LEN] __attribute__((aligned(8)));
	struct mmc_request req;
	uint8_t ctrl;

	if (!(emmc2_state.caps & SDHCI_CAP_HISPD)) {
		log_info("controller reports no high speed support");
		return;
	}

	memset(status, 0, sizeof(status));
	memset(&req, 0, sizeof(req));
	req.cmd.opcode = SD_CMD_SWITCH_FUNC;
	req.cmd.arg = SD_SWITCH_TO_HS;
	req.cmd.flags = MMC_RSP_R1 | MMC_DATA_READ;
	req.data.addr = (uintptr_t)status;
	req.data.blksz = SD_SWITCH_STATUS_LEN;
	req.data.blocks = 1;
	emmc2_request(mmc, &req);

	if (req.cmd.error) {
		log_error("CMD6 switch failed (%d), staying at default speed",
		    req.cmd.error);
		return;
	}
	if ((status[16] & 0x0f) != 1) {
		log_info("card did not select high speed (status[16]=%02x), staying",
		    status[16]);
		return;
	}

	ctrl = sdhci_rd8(host->base, SDHCI_HOST_CONTROL);
	sdhci_wr8(host->base, SDHCI_HOST_CONTROL, ctrl | SDHCI_CTRL_HISPD);
	emmc2_set_clock(host, EMMC2_HS_CLOCK);

	if (emmc2_still_good(mmc)) {
		emmc2_state.high_speed = 1;
		emmc2_hs_granted = 1;
		log_info("high speed, SDCLK %u Hz", host->cur_clock);
		return;
	}

	log_error("high speed did not verify, falling back to default speed");
	emmc2_set_clock(host, EMMC2_RUN_CLOCK);
	sdhci_wr8(host->base, SDHCI_HOST_CONTROL, ctrl);
	if (!emmc2_still_good(mmc)) {
		log_error("card did not come back at default speed either");
	}
}

static void emmc2_tune(struct emmc2_host *host, struct mmc_host *mmc) {
	emmc2_state.bus_width = 1;
	emmc2_ra_have = 0;

	if (!BUS_WIDTH_4 && !HIGH_SPEED) {
		return;
	}

	/* The reference every check below compares against, taken in the mode
	 * identification already proved works. */
	if (emmc2_read_block_raw(mmc, 0, emmc2_ref)) {
		log_error("cannot read block 0 before tuning, skipping it");
		return;
	}

	if (BUS_WIDTH_4) {
		emmc2_try_4bit(host, mmc);
	}
	if (HIGH_SPEED) {
		emmc2_try_high_speed(host, mmc);
	}
}

/* -------------------------------------------------------------- bring-up */

static uint32_t emmc2_probe_base_clock(struct emmc2_host *host) {
	uint32_t caps;
	uint32_t from_caps;
	uint32_t from_mbox;

	caps = sdhci_rd32(host->base, SDHCI_CAPABILITIES);
	from_caps = SDHCI_CAP_BASE_CLOCK(caps) * 1000000u;
	from_mbox = emmc2_mbox_clock();

	log_info("caps %08x (base clock %u MHz, %s), mailbox emmc2 %u Hz", caps,
	    from_caps / 1000000u, (caps & SDHCI_CAP_HISPD) ? "hi-speed" : "no hs",
	    from_mbox);

	/* CAPABILITIES first, mailbox as the fallback, both bounds-checked. The
	 * firmware owns the EMMC2 clock, but an emulator may answer
	 * GET_CLOCK_RATE for an id it does not know with a blanket value, and
	 * EMMC2 (12) is one of those. */
	if (from_caps >= EMMC2_CLOCK_MIN && from_caps <= EMMC2_CLOCK_MAX) {
		log_info("base clock %u Hz (from caps)", from_caps);
		return from_caps;
	}
	if (from_mbox >= EMMC2_CLOCK_MIN && from_mbox <= EMMC2_CLOCK_MAX) {
		log_info("base clock %u Hz (from mailbox)", from_mbox);
		return from_mbox;
	}

	log_error("caps %u Hz and mailbox %u Hz both implausible, assuming %u Hz",
	    from_caps, from_mbox, (unsigned)FALLBACK_CLOCK);
	return FALLBACK_CLOCK;
}

EMBOX_UNIT_INIT(emmc2_init);

static int emmc2_init(void) {
	struct emmc2_host *host = &emmc2_host;
	struct mmc_host *mmc;
	uint32_t version;
	uint32_t state;

	host->base = BASE_ADDR;

	version = sdhci_rd16(host->base, SDHCI_HOST_VERSION);
	log_info("bcm2711_emmc2 @ %p, host version %04x (spec %u.00)",
	    (void *)host->base, version, (version & 0xff) + 1);

	if (version == 0xffff || version == 0) {
		log_error("no controller at %p", (void *)host->base);
		return 0;
	}

	if (emmc2_reset(host, SDHCI_RESET_ALL)) {
		return 0;
	}

	host->base_clock = emmc2_probe_base_clock(host);
	emmc2_state.caps = sdhci_rd32(host->base, SDHCI_CAPABILITIES);

	if (emmc2_set_clock(host, EMMC2_ID_CLOCK)) {
		return 0;
	}

	sdhci_wr8(host->base, SDHCI_POWER_CONTROL, SDHCI_POWER_330 | SDHCI_POWER_ON);
	sdhci_wr8(host->base, SDHCI_TIMEOUT_CONTROL, SDHCI_MAX_TIMEOUT_CONTROL);
	sdhci_wr8(host->base, SDHCI_HOST_CONTROL, 0); /* 1-bit, normal speed */

	/* Latch every event, signal none: this driver polls (no GIC wiring). */
	sdhci_wr32(host->base, SDHCI_INT_ENABLE, 0xffffffff);
	sdhci_wr32(host->base, SDHCI_SIGNAL_ENABLE, 0);
	sdhci_wr32(host->base, SDHCI_INT_STATUS, 0xffffffff);

	state = sdhci_rd32(host->base, SDHCI_PRESENT_STATE);
	log_info("present state %08x, card %s", state,
	    (state & SDHCI_CARD_PRESENT) ? "inserted" : "absent");

	/* Deliberately not gated on CARD_PRESENT: card detect is not
	 * guaranteed to be wired to the controller on this board. */

	mmc = mmc_alloc_host();
	if (mmc == NULL) {
		log_error("mmc_alloc_host failed");
		return 0;
	}
	mmc->ops = &emmc2_ops;
	mmc->priv = host;
	host->mmc = mmc;

	if (mmc_scan(mmc) != 0) {
		log_error("no card identified at %p", (void *)host->base);
		mmc_dev_destroy(mmc);
		host->mmc = NULL;
		return 0;
	}

	/* Identification is over: default speed, then negotiate anything
	 * faster. */
	emmc2_set_clock(host, EMMC2_RUN_CLOCK);
	emmc2_tune(host, mmc);
	emmc2_set_readahead(READAHEAD);
	emmc2_set_write_combine(WRITE_COMBINE);
	emmc2_state.identified = 1;

	log_info("card ready: %llu bytes, %u byte blocks, SDCLK %u Hz, %u-bit%s",
	    (unsigned long long)mmc->bdev->size, mmc->bdev->block_size, host->cur_clock,
	    emmc2_state.bus_width, emmc2_state.high_speed ? ", high speed" : "");

	return 0;
}

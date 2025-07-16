/**
 * @file pl181.c
 * @brief ARM PrimeCell Multimedia Card v2 driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 18.10.2019
 *
 * @note pl181 is an extension for pl180. This driver was never tested with
 * pl180, but it should work for it as well.
 *
 * Refer to ARM PrimeCell Multimedia Card Interface (PL180) Technical Reference
 * Manual (file is named DDI0172.pdf) for more details.
 */
#include <assert.h>

#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <util/log.h>

#include <drivers/common/memory.h>
#include <drivers/mmc/mmc_core.h>
#include <drivers/mmc/mmc_host.h>

#define PL181_BASE OPTION_GET(NUMBER, base_addr)

/* PL181 has two interrupt signals, it's possible to assign every
 * event to one of the inetrrupt signals by modifying MCIMask0-1 */
#define PL181_IRQ0 OPTION_GET(NUMBER, irq0_num)
#define PL181_IRQ1 OPTION_GET(NUMBER, irq1_num)

#define MCI_POWER              (PL181_BASE + 0x000)
# define MCI_POWER_CTRL_MASK   (0x3)
# define MCI_POWER_CTRL_ON     (0x3)
#define MCI_CLOCK              (PL181_BASE + 0x004)
# define MCI_CLOCK_ENABLE      (1 << 8)
#define MCI_ARGUMENT           (PL181_BASE + 0x008)
#define MMC_COMMAND            (PL181_BASE + 0x00C)
# define MMC_CMD_IDX_MASK      (0x3F)
# define MMC_CMD_RSP           (1 << 6)
# define MMC_CMD_LONGRSP       (1 << 7)
# define MMC_CMD_ENABLE        (1 << 10)
#define MCI_RESPCMD            (PL181_BASE + 0x010)
#define MCI_RESPONSE0          (PL181_BASE + 0x014)
#define MCI_RESPONSE1          (PL181_BASE + 0x018)
#define MCI_RESPONSE2          (PL181_BASE + 0x01C)
#define MCI_RESPONSE3          (PL181_BASE + 0x020)
#define MCI_DATATIMER          (PL181_BASE + 0x024)
#define MCI_DATALENGTH         (PL181_BASE + 0x028)
#define MCI_DATACTRL           (PL181_BASE + 0x02C)
# define MCI_DATACTRL_EN       (1 << 0)
# define MCI_DATACTRL_DIR      (1 << 1)
# define MCI_DATACTRL_MODE     (1 << 2)
# define MCI_DATACTRL_DMA      (1 << 3)
# define MCI_DATACTRL_BLK_MASK (0xF0)
#define MCI_DATACNT            (PL181_BASE + 0x030)
#define MCI_STATUS             (PL181_BASE + 0x034)
# define MCI_STATUS_TXFHE      (1 << 14)
# define MCI_STATUS_TXD        (1 << 20)
# define MCI_STATUS_RXD        (1 << 21)
#define MCI_CLEAR              (PL181_BASE + 0x038)
#define MCI_MASK0              (PL181_BASE + 0x03C)
#define MCI_MASK1              (PL181_BASE + 0x040)
#define MCI_SELECT             (PL181_BASE + 0x044)
#define MCI_FIFOCNT            (PL181_BASE + 0x048)
#define MCI_FIFO(n)            (PL181_BASE + 0x080 + 4 * (n))

#define PL181_STATUS_NUM 22
static const char *pl181_status[PL181_STATUS_NUM] = {
	"Command response received (CRC check failed)",
	"Data block sent/received (CRC check failed)",
	"Command response timeout",
	"Data timeout",
	"Transmit FIFO underrun error",
	"Receive FIFO overrun error",
	"Command response received (CRC check passed)",
	"Command sent (no response required)",
	"Data end (data counter is zero)",
	"Start bit not detected on all data signals in wide bus mode",
	"Data block sent/received (CRC check passed)",
	"Command transfer in progress",
	"Data transmit in progress",
	"Data receive in progress",
	"Transmit FIFO half empty",
	"Receive FIFO half full",
	"Transmit FIFO full",
	"Receive FIFO full",
	"Transmit FIFO empty",
	"Receive FIFO empty",
	"Data available in transmit FIFO",
	"Data available in receive FIFO",
};

static void pl181_dump_status(void) {
	uint32_t tmp = REG32_LOAD(MCI_STATUS);

	(void) pl181_status; /* Avoid warning for debug_level = 0 */

	log_debug("PL181 status:");
	log_debug("=============");

	if (tmp == 0) {
		log_debug("All bits are set to zero");
	} else {
		for (int i = 0; i < PL181_STATUS_NUM; i++) {
			if (tmp & (1 << i)) {
				log_debug("%s", pl181_status[i]);
			}
		}
	}

	log_debug("=============");

	REG32_STORE(MCI_CLEAR, tmp);
}

static int pl181_send_cmd(int cmd, int arg, int resp, int longresp) {
	uint32_t wcmd = (uint32_t) cmd & MMC_CMD_IDX_MASK;

	if (wcmd != (uint32_t) cmd) {
		log_error("Wrong MMC command passed: CMD%d", cmd);
		return -1;
	}

	REG32_STORE(MCI_ARGUMENT, arg);

	if (resp) {
		wcmd |= MMC_CMD_RSP;
	}

	if (longresp) {
		wcmd |= MMC_CMD_LONGRSP;
	}

	wcmd |= MMC_CMD_ENABLE;

	REG32_STORE(MMC_COMMAND, wcmd);

	return 0;
}

static void pl181_mmc_request(struct mmc_host *host, struct mmc_request *req) {
	uint32_t *buff;

	assert(host);
	assert(req);

	pl181_send_cmd(req->cmd.opcode,
			req->cmd.arg,
			req->cmd.flags & MMC_RSP_PRESENT,
			req->cmd.flags & MMC_RSP_136);

	req->cmd.resp[0] = REG32_LOAD(MCI_RESPONSE0);
	req->cmd.resp[1] = REG32_LOAD(MCI_RESPONSE1);
	req->cmd.resp[2] = REG32_LOAD(MCI_RESPONSE2);
	req->cmd.resp[3] = REG32_LOAD(MCI_RESPONSE3);

	if (req->cmd.flags & MMC_DATA_XFER) {
		int xfer = req->data.blksz * req->data.blocks;

		REG32_STORE(MCI_DATALENGTH, req->data.blocks * req->data.blksz);
		REG32_STORE(MCI_DATALENGTH, req->data.blksz);
		log_debug("Set datalen %d", req->data.blksz);

		REG32_STORE(MCI_DATACTRL,
				MCI_DATACTRL_EN | (req->cmd.flags & MMC_DATA_READ ? MCI_DATACTRL_DIR : 0) | (8 << 4));

		buff = (void *) req->data.addr;
		log_debug("buff=%p", buff);

		while (xfer > 0) {
			if (req->cmd.flags & MMC_DATA_READ) {
				if (REG32_LOAD(MCI_STATUS) & MCI_STATUS_RXD) {
					*buff++ = REG32_LOAD(MCI_FIFO(0));
					xfer -= sizeof(uint32_t);
				}
			} else {
				if (REG32_LOAD(MCI_STATUS) & MCI_STATUS_TXFHE) {
					REG32_STORE(MCI_FIFO(0), *buff);
					buff++;
					xfer -= sizeof(uint32_t);
				}
			}
		}
	}
}

const struct mmc_host_ops pl181_mmc_ops = {
	.request = pl181_mmc_request,
};

extern int stm32cube_sdio_hw_init(void);

EMBOX_UNIT_INIT(pl181_init);
static int pl181_init(void) {
	/* It seems that  we can't check directly
	 * if any card present, so we just assume
	 * that we have a single card number zero */
	struct mmc_host *mmc = mmc_alloc_host();
	mmc->ops = &pl181_mmc_ops;

#if (OPTION_GET(NUMBER,hw_init) != 0)
	stm32cube_sdio_hw_init();
#endif /* (OPTION_GET(NUMBER,hw_init) != 0) */

	/* Power-on */
	REG32_CLEAR(MCI_POWER, MCI_POWER_CTRL_MASK);
	REG32_ORIN(MCI_POWER, MCI_POWER_CTRL_ON);

	pl181_dump_status();

	mmc_scan(mmc);

	return 0;
}

PERIPH_MEMORY_DEFINE(pl181, PL181_BASE, 0x100);

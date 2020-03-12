/**
 * @file
 *
 * @date Oct 7, 2019
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <util/math.h>

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <mem/misc/pool.h>

#include <hal/clock.h>
#include <kernel/time/time.h>
#include <drivers/common/memory.h>
#include <drivers/common/periph_memory_alloc.h>
#include <kernel/irq.h>

#include <linux/byteorder.h>

#include <drivers/clk/socfpga.h>
#include <drivers/mmc/mmc.h>
#include <drivers/mmc/mmc_core.h>
#include <drivers/mmc/mmc_host.h>
#include <drivers/mmc/mmc_card.h>

#include "dw_mmc.h"

#include <embox/unit.h>

extern void dcache_flush(const void *p, size_t size);
extern void dcache_inval(const void *p, size_t size);
#define BASE_ADDR OPTION_GET(NUMBER, base_addr)
#define IRQ_NUM   OPTION_GET(NUMBER, irq_num)
#define SLOTS_QUANTITY OPTION_GET(NUMBER, slots_quantity)

/* Common flag combinations */
#define DW_MCI_DATA_ERROR_FLAGS (SDMMC_INT_DRTO | SDMMC_INT_DCRC | \
		SDMMC_INT_HTO | SDMMC_INT_SBE  | \
		SDMMC_INT_EBE | SDMMC_INT_HLE)

#define DW_MCI_CMD_ERROR_FLAGS (SDMMC_INT_RTO | SDMMC_INT_RCRC | \
		SDMMC_INT_RESP_ERR | SDMMC_INT_HLE)

#define DW_MCI_ERROR_FLAGS (DW_MCI_DATA_ERROR_FLAGS | DW_MCI_CMD_ERROR_FLAGS)

#define DW_MCI_SEND_STATUS   1
#define DW_MCI_RECV_STATUS   2
#define DW_MCI_DMA_THRESHOLD 16

#define DW_MCI_FREQ_MAX 200000000  /* unit: HZ */
#define DW_MCI_FREQ_MIN 400000     /* unit: HZ */

#define IDMAC_INT_CLR (SDMMC_IDMAC_INT_AI | SDMMC_IDMAC_INT_NI | \
		SDMMC_IDMAC_INT_CES | SDMMC_IDMAC_INT_DU | \
		SDMMC_IDMAC_INT_FBE | SDMMC_IDMAC_INT_RI | \
		SDMMC_IDMAC_INT_TI)

#define DESC_RING_BUF_SZ 4096

struct idmac_desc {
	uint32_t des0; /* Control Descriptor */
#define IDMAC_DES0_DIC BIT(1)
#define IDMAC_DES0_LD  BIT(2)
#define IDMAC_DES0_FD  BIT(3)
#define IDMAC_DES0_CH  BIT(4)
#define IDMAC_DES0_ER  BIT(5)
#define IDMAC_DES0_CES BIT(30)
#define IDMAC_DES0_OWN BIT(31)

	uint32_t des1; /* Buffer sizes */
#define IDMAC_SET_BUFFER1_SIZE(d, s) \
	((d)->des1 = ((d)->des1 & cpu_to_le32(0x03ffe000)) | (cpu_to_le32((s) & 0x1fff)))

	uint32_t des2; /* buffer 1 physical address */

	uint32_t des3; /* buffer 2 physical address */
};

/* Each descriptor can transfer up to 4KB of data in chained mode */
#define DW_MCI_DESC_DATA_LENGTH	0x1000

POOL_DEF(dw_mci_slot_pool, struct dw_mci_slot, SLOTS_QUANTITY);

static bool dw_mci_ctrl_reset(struct dw_mci *host, uint32_t reset) {
	uint32_t ctrl;
	int timeout = 2000;

	ctrl = mci_readl(host, CTRL);
	ctrl |= reset;
	mci_writel(host, CTRL, ctrl);

	/* wait till resets clear */
	do {
		ctrl = mci_readl(host, CTRL);
		if (!(ctrl & reset)) {
			return true;
		}

		usleep(USEC_PER_MSEC);
	} while (timeout-- > 0);

	log_error("Timeout resetting block (ctrl reset %#x)", ctrl & reset);

	return false;
}

static void dw_mci_idmac_reset(struct dw_mci *host) {
	uint32_t bmod = mci_readl(host, BMOD);
	bmod |= SDMMC_IDMAC_SWRESET;
	mci_writel(host, BMOD, bmod);
}

static void dw_mci_idmac_stop_dma(struct dw_mci *host) {
	uint32_t temp;
	log_debug("STOP");

	/* Disable and reset the IDMAC interface */
	temp = mci_readl(host, CTRL);
	temp &= ~SDMMC_CTRL_USE_IDMAC;
	temp |= SDMMC_CTRL_DMA_RESET;
	mci_writel(host, CTRL, temp);

	/* Stop the IDMAC running */
	temp = mci_readl(host, BMOD);
	temp &= ~(SDMMC_IDMAC_ENABLE | SDMMC_IDMAC_FB);
	temp |= SDMMC_IDMAC_SWRESET;
	mci_writel(host, BMOD, temp);
}

static int dw_mci_idmac_init(struct dw_mci *host) {
	struct idmac_desc *p;
	int i;

	/* Number of descriptors in the ring buffer */
	host->ring_size = DESC_RING_BUF_SZ / sizeof(struct idmac_desc);

	/* Forward link the descriptor list */
	p = host->desc_ring;
	for (i = 0; i < host->ring_size - 1; i++) {
		memset(p + i, 0, sizeof(*p));
		p[i].des3 = cpu_to_le32(host->desc_ring
				+ (sizeof(struct idmac_desc) * (i + 1)));
		p[i].des1 = 0;

		dcache_flush(p, sizeof(*p));
	}

	/* Set the last descriptor as the end-of-ring descriptor */
	p[i].des3 = cpu_to_le32(host->desc_ring);
	p[i].des0 = cpu_to_le32(IDMAC_DES0_ER);
	dcache_flush(p + i, sizeof(*p));

	dw_mci_idmac_reset(host);

	mci_writel(host, IDSTS, IDMAC_INT_CLR);
	mci_writel(host, IDINTEN, SDMMC_IDMAC_INT_NI |
			SDMMC_IDMAC_INT_RI | SDMMC_IDMAC_INT_TI);

	/* Set the descriptor base address */
	mci_writel(host, DBADDR, (uint32_t) host->desc_ring);

	return 0;
}

static inline int dw_mci_prepare_desc32(struct dw_mci *host, struct mmc_data *data, unsigned int sg_len) {
	unsigned int desc_len;
	struct idmac_desc *desc_first, *desc_last, *desc;
	int i;

	sg_len = 1;

	desc_first = desc_last = desc = host->desc_ring;

	for (i = 0; i < sg_len; i++) {
		unsigned int length;
		uint32_t mem_addr;

		length = data->blksz;
		mem_addr = data->addr;

		log_debug("mem_addr(%x), length(%d); desc_first(%x)", mem_addr, length, desc_first);

		for ( ; length ; desc++) {
			int timeout = 1000;
			desc_len = (length <= DW_MCI_DESC_DATA_LENGTH) ?
				length : DW_MCI_DESC_DATA_LENGTH;

			length -= desc_len;

			while (REG_LOAD(&desc->des0) & cpu_to_le32(IDMAC_DES0_OWN)) {
				usleep(USEC_PER_MSEC);
				if (timeout-- < 0) {
					log_error("DMA desc is not freed for too long!");
					return -1;
				}
			}

			desc->des0 = cpu_to_le32(IDMAC_DES0_OWN |
					IDMAC_DES0_DIC |
					IDMAC_DES0_CH);

			IDMAC_SET_BUFFER1_SIZE(desc, desc_len);

			desc->des2 = cpu_to_le32(mem_addr);

			mem_addr += desc_len;

			desc_last = desc;
			dcache_flush(desc, sizeof(*desc));
		}
	}

	desc_first->des0 |= cpu_to_le32(IDMAC_DES0_FD);

	desc_last->des0 &= cpu_to_le32(~(IDMAC_DES0_CH | IDMAC_DES0_DIC));
	desc_last->des0 |= cpu_to_le32(IDMAC_DES0_LD);
	dcache_flush(desc_first, sizeof(*desc_first));
	dcache_flush(desc_last, sizeof(*desc_last));

	return 0;
}

static int dw_mci_idmac_start_dma(struct dw_mci *host, unsigned int sg_len) {
	uint32_t temp;
	int ret = 0;

	ret = dw_mci_prepare_desc32(host, host->data, sg_len);
	if (ret) {
		log_error("Failed to prepare descriptor");
		return ret;
	}

	dw_mci_ctrl_reset(host, SDMMC_CTRL_DMA_RESET);
	dw_mci_idmac_reset(host);

	temp = mci_readl(host, CTRL);
	temp |= SDMMC_CTRL_USE_IDMAC;
	mci_writel(host, CTRL, temp);

	temp = mci_readl(host, BMOD);
	temp |= SDMMC_IDMAC_ENABLE | SDMMC_IDMAC_FB;
	mci_writel(host, BMOD, temp);

	mci_writel(host, PLDMND, 1);

	return 0;
}

static int dw_mci_init_dma(struct dw_mci *host) {
	int addr_config;

	if (SDMMC_GET_TRANS_MODE(mci_readl(host, HCON)) == DMA_INTERFACE_IDMA) {
		host->use_dma = TRANS_MODE_IDMAC;
	} else {
		return -1;
	}

	addr_config = SDMMC_GET_ADDR_CONFIG(mci_readl(host, HCON));

	if (addr_config == 1) {
		/* host supports IDMAC in 64-bit address mode */
		log_info("IDMAC supports 64-bit address mode, but "
				"driver doesn't support it");
		return -1;
	} else {
		/* host supports IDMAC in 32-bit address mode */
		host->dma_64bit_address = 0;
		log_info("IDMAC supports 32-bit address mode.");
	}

	/* Alloc memory for sg translation */
	host->desc_ring = periph_memory_alloc(DESC_RING_BUF_SZ);
	if (!host->desc_ring) {
		log_error("could not alloc memory for descriptors");
		return -1;
	}

	memset(host->desc_ring, 0, DESC_RING_BUF_SZ);

	log_info("Using internal DMA controller.");

	if (dw_mci_idmac_init(host)) {
		log_error("Unable to initialize DMA Controller.");
		return -1;
	}

	return 0;
}

static void dw_mci_wait_while_busy(struct dw_mci *host, uint32_t cmd_flags) {
	int timeout = 1000;

	/*
	 * Databook says that before issuing a new data transfer command
	 * we need to check to see if the card is busy.  Data transfer commands
	 * all have SDMMC_CMD_PRV_DAT_WAIT set, so we'll key off that.
	 *
	 * ...also allow sending for SDMMC_CMD_VOLT_SWITCH where busy is
	 * expected.
	 */
	if ((cmd_flags & SDMMC_CMD_PRV_DAT_WAIT) &&
			!(cmd_flags & SDMMC_CMD_VOLT_SWITCH)) {
		while (mci_readl(host, STATUS) & SDMMC_STATUS_BUSY) {
			if (timeout-- < 0) {
				/* Command will fail; we'll pass error then */
				log_error("Busy; trying anyway");
				break;
			}
			usleep(USEC_PER_MSEC);
		}
	}
}

static void dw_mci_start_command(struct dw_mci *host, struct mmc_command *cmd, uint32_t cmd_flags) {
	host->cmd = cmd;
	log_debug("start command: ARGR=0x%08x CMDR=0x%08x", cmd->arg, cmd_flags);

	mci_writel(host, CMDARG, cmd->arg);
	/* TODO drain writebuffer */
	dw_mci_wait_while_busy(host, cmd_flags);

	mci_writel(host, CMD, cmd_flags | SDMMC_CMD_START);
}

static int dw_mci_pre_dma_transfer(struct dw_mci *host, struct mmc_data *data,
		bool next) {
	if (data->blocks * data->blksz < DW_MCI_DMA_THRESHOLD) {
		return -EINVAL;
	}

	if (data->blksz & 3) {
		return -EINVAL;
	}

	return data->blocks * data->blksz;
}

static int dw_mci_submit_data_dma(struct dw_mci *host, struct mmc_data *data) {
	int sg_len;
	uint32_t temp;

	/* If we don't have a channel, we can't do DMA */
	if (!host->use_dma) {
		return -ENODEV;
	}

	sg_len = dw_mci_pre_dma_transfer(host, data, 0);
	if (sg_len < 0) {
		dw_mci_idmac_stop_dma(host);
		return sg_len;
	}

	log_debug("desc_ring: %#lx sg_len: %d", host->desc_ring, sg_len);

	/* Enable the DMA interface */
	temp = mci_readl(host, CTRL);
	temp |= SDMMC_CTRL_DMA_ENABLE;
	mci_writel(host, CTRL, temp);

	/* Disable RX/TX IRQs, let DMA handle it */
	temp = mci_readl(host, INTMASK);
	temp  &= ~(SDMMC_INT_RXDR | SDMMC_INT_TXDR);
	mci_writel(host, INTMASK, temp);
	if (dw_mci_idmac_start_dma(host, sg_len)) {
		dw_mci_idmac_stop_dma(host);
		log_debug("fall back to PIO mode for current transfer");
		return -ENODEV;
	}

	return 0;
}

static void dw_mci_submit_data(struct dw_mci *host, struct mmc_data *data) {
	host->data = data;

	if (dw_mci_submit_data_dma(host, data)) {
		log_error("Failed to submit data via DMA");
	}

	host->prev_blksz = data->blksz;
}

static void mci_send_cmd(struct dw_mci_slot *slot, uint32_t cmd, uint32_t arg) {
	struct dw_mci *host = slot->host;
	unsigned long timeout = 1000;
	unsigned int cmd_status = 0;

	mci_writel(host, CMDARG, arg);
	dw_mci_wait_while_busy(host, cmd);
	mci_writel(host, CMD, SDMMC_CMD_START | cmd);

	while (timeout--) {
		cmd_status = mci_readl(host, CMD);
		if (!(cmd_status & SDMMC_CMD_START)) {
			return;
		}

		usleep(USEC_PER_MSEC);
	}

	log_error("Timeout sending command (cmd %#x arg %#x status %#x)",
			cmd, arg, cmd_status);
}

static uint32_t dw_mci_prepare_command(struct mmc_host *mmc, struct mmc_command *cmd,
		struct mmc_data *data) {
	uint32_t cmdr = cmd->opcode;

	if (cmd->opcode == MMC_STOP_TRANSMISSION ||
			cmd->opcode == MMC_GO_IDLE_STATE ||
			cmd->opcode == MMC_GO_INACTIVE_STATE) {
		cmdr |= SDMMC_CMD_STOP;
	} else if (cmd->opcode != MMC_SEND_STATUS && data->addr) {
		cmdr |= SDMMC_CMD_PRV_DAT_WAIT;
	}

	if (cmd->flags & MMC_RSP_PRESENT) {
		cmdr |= SDMMC_CMD_RESP_EXP;
		if (cmd->flags & MMC_RSP_136) {
			cmdr |= SDMMC_CMD_RESP_LONG;
		}
	}

	if (cmd->flags & MMC_RSP_CRC) {
		cmdr |= SDMMC_CMD_RESP_CRC;
	}

	if (data->addr) {
		cmdr |= SDMMC_CMD_DAT_EXP;
		if (cmd->flags & MMC_DATA_WRITE) {
			cmdr |= SDMMC_CMD_DAT_WR;
		}
	}

	cmdr |= SDMMC_CMD_USE_HOLD_REG;

	return cmdr;
}

static void dw_mci_start_request(struct dw_mci *host,
		struct dw_mci_slot *slot, struct mmc_command *cmd) {
	struct mmc_request *mrq;
	struct mmc_data *data;
	uint32_t cmdflags;

	assert(slot);
	assert(host);
	assert(cmd);

	mrq = slot->mrq;
	host->mrq = mrq;

	host->pending_events = 0;
	host->completed_events = 0;
	host->cmd_status = 0;
	host->data_status = 0;

	cmdflags = dw_mci_prepare_command(slot->mmc, cmd, &mrq->data);

	if (slot->flags & DW_MMC_CARD_NEED_INIT ) {
		cmdflags |= SDMMC_CMD_INIT;
		slot->flags &= ~DW_MMC_CARD_NEED_INIT;
	}

	data = &mrq->data;
	if (data->addr) {
		mci_writel(host, TMOUT, 0xFFFFFFFF);
		mci_writel(host, BYTCNT, data->blksz * data->blocks);
		mci_writel(host, BLKSIZ, data->blksz);
		if (cmd->flags & MMC_DATA_WRITE) {
			dcache_flush((void *) data->addr, data->blksz * data->blocks);
		}

		dw_mci_submit_data(host, data);
	}

	dw_mci_start_command(host, cmd, cmdflags);

	if (data->addr) {
		int timeout = 1000;
		while (!(host->data_status & SDMMC_INT_DATA_OVER)) {
			usleep(USEC_PER_MSEC);
			if (timeout-- < 0) {
				log_error("Data transfer timeout");
				break;
			}
		}

		host->data_status &= ~SDMMC_INT_DATA_OVER;

		if (cmd->flags & MMC_DATA_READ) {
			dcache_inval((void *) data->addr, data->blksz * data->blocks);
		}
	}
}

static void dw_mci_setup_bus(struct dw_mci_slot *slot, bool force_clkinit) {
	struct dw_mci *host = slot->host;
	uint32_t div;
	uint32_t clk_en_a;
	uint32_t sdmmc_cmd_bits = SDMMC_CMD_UPD_CLK | SDMMC_CMD_PRV_DAT_WAIT;

	log_debug("slot id(%d)", slot->id);

	div = 0;

	/* disable clock */
	mci_writel(host, CLKENA, 0);
	mci_writel(host, CLKSRC, 0);

	/* inform CIU */
	mci_send_cmd(slot, sdmmc_cmd_bits, 0);

	/* set clock to desired speed */
	mci_writel(host, CLKDIV, div);

	/* inform CIU */
	mci_send_cmd(slot, sdmmc_cmd_bits, 0);

	/* enable clock; only low power if no SDIO */
	clk_en_a = SDMMC_CLKEN_ENABLE << slot->id;
	clk_en_a |= SDMMC_CLKEN_LOW_PWR << slot->id;

	mci_writel(host, CLKENA, clk_en_a);

	/* inform CIU */
	mci_send_cmd(slot, sdmmc_cmd_bits, 0);

	/* Set the current slot bus width */
	mci_writel(host, CTYPE, (slot->ctype << slot->id));
}

static int dw_mci_get_cd(struct mmc_host *mmc) {
	int slot_id = 0; /* XXX */
	uint32_t present = 0;
	struct dw_mci *host;

	assert(mmc);

	host = mmc->priv;
	present = mci_readl(host, CDETECT) & (1 << slot_id);

	return !present;
}

static void dw_mci_request(struct mmc_host *mmc, struct mmc_request *mrq) {
	struct dw_mci_slot *slot;
	struct dw_mci *host;
	int retry = 1000;

	assert(mmc);
	assert(mrq);

	slot = mmc->priv;
	assert(slot);

	host = slot->host;

	log_debug("host(%p) slot(%d); cmd:opcode(%d)", host, slot->id, mrq->cmd.opcode);

	slot->mrq = mrq;

	dw_mci_start_request(host, slot, &mrq->cmd);

	while(!(host->pending_events & (1 << EVENT_CMD_COMPLETE))) {
		retry--;
		usleep(USEC_PER_MSEC);
		if (retry <= 0) {
			log_error("Command complete timeout");
			break;
		}
	}

	host->pending_events &= ~(1 << EVENT_CMD_COMPLETE);

	log_debug("cmd.resp: %08x %08x %08x %08x", mrq->cmd.resp[0],
			mrq->cmd.resp[1], mrq->cmd.resp[2],mrq->cmd.resp[3]);
}

static void dw_mci_set_ios(struct mmc_host *mmc, struct mmc_ios *ios) {
	struct dw_mci_slot *slot = mmc_priv(mmc);
	uint32_t regs;
	slot->flags |= DW_MMC_CARD_NEED_INIT;

	regs = mci_readl(slot->host, PWREN);
	regs |= (1 << slot->id);
	mci_writel(slot->host, PWREN, regs);
	dw_mci_setup_bus(slot, false);
}

static int dw_mci_get_ro(struct mmc_host *mmc) {
	int slot_id = 0; /* XXX */
	uint32_t read_only;
	struct dw_mci *host;

	assert(mmc);

	host = mmc->priv;
	read_only = mci_readl(host, WRTPRT) & (1 << slot_id);

	return !!read_only;
}

static const struct mmc_host_ops dw_mci_ops = {
	.request = dw_mci_request,
	.set_ios = dw_mci_set_ios,
	.get_ro  = dw_mci_get_ro,
	.get_cd  = dw_mci_get_cd,
};

static int dw_mci_command_complete(struct dw_mci *host, struct mmc_command *cmd) {
	uint32_t status = host->cmd_status;

	host->cmd_status = 0;

	/* Read the response from the card (up to 16 bytes) */
	if (cmd->flags & MMC_RSP_PRESENT) {
		if (cmd->flags & MMC_RSP_136) {
			cmd->resp[3] = mci_readl(host, RESP0);
			cmd->resp[2] = mci_readl(host, RESP1);
			cmd->resp[1] = mci_readl(host, RESP2);
			cmd->resp[0] = mci_readl(host, RESP3);
		} else {
			cmd->resp[0] = mci_readl(host, RESP0);
			cmd->resp[1] = 0;
			cmd->resp[2] = 0;
			cmd->resp[3] = 0;
		}
	}

	if (status & SDMMC_INT_RTO)
		cmd->error = -ETIMEDOUT;
	else if ((cmd->flags & MMC_RSP_CRC) && (status & SDMMC_INT_RCRC))
		cmd->error = -EILSEQ;
	else if (status & SDMMC_INT_RESP_ERR)
		cmd->error = -EIO;
	else
		cmd->error = 0;

	return cmd->error;
}

static void dw_mci_cmd_interrupt(struct dw_mci *host, uint32_t status) {
	if (!host->cmd_status) {
		host->cmd_status = status;
	}

	host->pending_events |= (1 << EVENT_CMD_COMPLETE);

	dw_mci_command_complete(host, host->cmd);
}

static irq_return_t dw_mci_interrupt(unsigned int irq, void *dev_id) {
	struct dw_mci *host = dev_id;
	uint32_t pending;
	int i;

	pending = mci_readl(host, MINTSTS); /* read-only mask reg */
	log_debug("host (%p) pending MINTSTS (%x)", host, pending);

	if (pending) {
		if (pending & DW_MCI_CMD_ERROR_FLAGS) {
			mci_writel(host, RINTSTS, DW_MCI_CMD_ERROR_FLAGS);
			host->cmd_status = pending;
			dw_mci_ctrl_reset(host, SDMMC_CTRL_ALL_RESET_FLAGS);
		}

		if (pending & DW_MCI_DATA_ERROR_FLAGS) {
			mci_writel(host, RINTSTS, DW_MCI_DATA_ERROR_FLAGS);
			host->data_status |= pending;
		}

		if (pending & SDMMC_INT_DATA_OVER) {
			mci_writel(host, RINTSTS, SDMMC_INT_DATA_OVER);
			host->data_status |= SDMMC_INT_DATA_OVER;
		}

		if (pending & SDMMC_INT_RXDR) {
			mci_writel(host, RINTSTS, SDMMC_INT_RXDR);
		}

		if (pending & SDMMC_INT_TXDR) {
			mci_writel(host, RINTSTS, SDMMC_INT_TXDR);
		}

		if (pending & SDMMC_INT_CMD_DONE) {
			mci_writel(host, RINTSTS, SDMMC_INT_CMD_DONE);
			dw_mci_cmd_interrupt(host, pending);
		}

		if (pending & SDMMC_INT_CD) {
			mci_writel(host, RINTSTS, SDMMC_INT_CD);
		}

		/* Handle SDIO Interrupts */
		for (i = 0; i < host->num_slots; i++) {
			struct dw_mci_slot *slot = host->slot[i];

			if (!slot)
				continue;

			if (pending & SDMMC_INT_SDIO(slot->sdio_id)) {
				mci_writel(host, RINTSTS, SDMMC_INT_SDIO(slot->sdio_id));
			}
		}

	}

	/* Handle IDMA interrupts */
	if (host->dma_64bit_address == 1) {
		pending = mci_readl(host, IDSTS64);
		if (pending & (SDMMC_IDMAC_INT_TI | SDMMC_IDMAC_INT_RI)) {
			mci_writel(host, IDSTS64, SDMMC_IDMAC_INT_TI |
					SDMMC_IDMAC_INT_RI);
			mci_writel(host, IDSTS64, SDMMC_IDMAC_INT_NI);
		}
	} else {
		pending = mci_readl(host, IDSTS);
		if (pending & (SDMMC_IDMAC_INT_TI | SDMMC_IDMAC_INT_RI)) {
			mci_writel(host, IDSTS, SDMMC_IDMAC_INT_TI | SDMMC_IDMAC_INT_RI);
			mci_writel(host, IDSTS, SDMMC_IDMAC_INT_NI);
		}
	}

	return IRQ_HANDLED;
}

static int dw_mci_init_slot(struct dw_mci *host, unsigned int id) {
	struct mmc_host *mmc;
	struct dw_mci_slot *slot;

	mmc = mmc_alloc_host();
	if (!mmc) {
		log_error("couldn't allocate mmc host");
		return -ENOMEM;
	}

	mmc->priv = pool_alloc(&dw_mci_slot_pool);
	if (!mmc->priv) {
		log_error("couldn't allocate dw_mci_slot");
		return -ENOMEM;
	}

	slot = mmc_priv(mmc);
	memset(slot, 0, sizeof(*slot));
	slot->id = id;
	slot->sdio_id = host->sdio_id0 + id;
	slot->mmc = mmc;
	slot->host = host;
	host->slot[id] = slot;

	mmc->ops = &dw_mci_ops;

	mmc->f_min = DW_MCI_FREQ_MIN;
	mmc->f_max = DW_MCI_FREQ_MAX;

	mmc->max_segs = host->ring_size;
	mmc->max_blk_size = 65535;
	mmc->max_seg_size = 0x1000;
	mmc->max_req_size = mmc->max_seg_size * host->ring_size;
	mmc->max_blk_count = mmc->max_req_size / 512;

	dw_mci_get_cd(mmc);

	return 0;
}

static void dw_mci_enable_cd(struct dw_mci *host) {
	uint32_t temp;

	assert(host);

	temp = mci_readl(host, INTMASK);
	temp |= SDMMC_INT_CD;
	mci_writel(host, INTMASK, temp);
}

int dw_mci_probe(struct dw_mci *host) {
	int ret = 0;
	int init_slots = 0;

	/* Reset all blocks */
	if (!dw_mci_ctrl_reset(host, SDMMC_CTRL_ALL_RESET_FLAGS)) {
		return -ENODEV;
	}

	if (dw_mci_init_dma(host)) {
		log_error("Failed to init DMA");
		return -1;
	}

	/* Clear the interrupts for the host controller */
	mci_writel(host, RINTSTS, 0xFFFFFFFF);
	mci_writel(host, INTMASK, 0); /* disable all mmc interrupt first */

	/* Put in max timeout */
	mci_writel(host, TMOUT, 0xFFFFFFFF);

	mci_writel(host, CLKENA, 0);
	mci_writel(host, CLKSRC, 0);

	/* In 2.40a spec, Data offset is changed.
	 * Need to check the version-id and set data-offset for DATA register. */
	host->verid = SDMMC_GET_VERID(mci_readl(host, VERID));
	log_info("Version ID is %04x", host->verid);

	host->num_slots = 1;

	/* Enable interrupts for command done, data over, data empty,
	 * receive ready and error such as transmit, receive timeout, crc error */
	mci_writel(host, INTMASK, SDMMC_INT_CMD_DONE | SDMMC_INT_DATA_OVER |
			SDMMC_INT_TXDR | SDMMC_INT_RXDR | DW_MCI_ERROR_FLAGS);
	mci_writel(host, CTRL, SDMMC_CTRL_INT_ENABLE);

	for (int i = 0; i < host->num_slots; i++) {
		ret = dw_mci_init_slot(host, i);
		if (ret) {
			log_debug("slot %d init failed", i);
		} else {
			init_slots++;
		}
	}

	if (!init_slots) {
		log_debug("attempted to initialize %d slots, but failed on all",
				host->num_slots);
		return -EBUSY;
	}

	log_info("%d slots initialized", init_slots);

	/* Now that slots are all setup, we can enable card detect */
	dw_mci_enable_cd(host);

	return 0;
}

void dw_mmc_clocks_enable(void) {
	clk_enable( "l4_main_clk");
	clk_enable( "l3_mp_clk" );
	clk_enable( "l4_mp_clk" );
	clk_enable( "l4_sp_clk" );
	clk_enable( "cfg_clk" );

	clk_enable( "main_pll_clk" );
	clk_enable( "periph_pll_clk" );

	clk_enable( "h2f_user0_clk" );
	clk_enable( "emac_0_clk" );
	clk_enable( "emac_1_clk" );
	clk_enable( "usb_mp_clk" );
	clk_enable( "spi_m_clk" );
	clk_enable( "can0_clk" );
	clk_enable( "can1_clk" );
	clk_enable( "gpio_db_clk" );
	clk_enable( "h2f_user1_clk" );
	clk_enable( "sdmmc_clk" );
	clk_enable( "sdmmc_clk_divided" );
	clk_enable( "nand_x_clk" );
	clk_enable( "nand_clk" );
	clk_enable( "qspi_clk" );
	clk_enable( "ddr_dqs_clk_gate" );
	clk_enable( "ddr_2x_dqs_clk_gate" );
	clk_enable( "ddr_dq_clk_gate" );
	clk_enable( "h2f_user2_clk" );

	clk_enable("emac0_clk" );
	clk_enable("emac1_clk" );
	clk_enable("per_qsi_clk" );
	clk_enable("per_nand_mmc_clk" );
	clk_enable("per_base_clk" );
	clk_enable("h2f_usr1_clk" );
}

static struct dw_mci dw_mci;
EMBOX_UNIT_INIT(dw_mmc_sockfpga_init);
static int dw_mmc_sockfpga_init(void) {
	int res;

	dw_mmc_clocks_enable();

	dw_mci.regs = (uintptr_t *)BASE_ADDR;
	dw_mci.irq = IRQ_NUM;
	res = dw_mci_probe(&dw_mci);
	if (res) {
		log_error("dw_mci_probe return %d", res);
		return res;
	}

	res = irq_attach(IRQ_NUM, dw_mci_interrupt, 0, &dw_mci, "dw_mmc");
	if (res) {
		log_error("irq_attach return %d", res);
		return res;
	}
	/* TODO just for timeout. Sometimes It's broken while reseting if there are no output */
	log_debug("SDMMC_CDETECT %x", mci_readl(&dw_mci, CDETECT));
	log_debug("SDMMC_RINTSTS %x", mci_readl(&dw_mci, RINTSTS));
	log_debug("SDMMC_INTMASK %x", mci_readl(&dw_mci, INTMASK));
	log_debug("SDMMC_CTRL %x", mci_readl(&dw_mci, CTRL));

	dw_mci_set_ios(dw_mci.slot[0]->mmc, &dw_mci.slot[0]->mmc->ios);
	mmc_scan(dw_mci.slot[0]->mmc);

	return 0;
}

PERIPH_MEMORY_DEFINE(dw_mmc, BASE_ADDR, 0x4000);

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
#include <asm-generic/dma-mapping.h>
#include <kernel/irq.h>

#include <linux/byteorder.h>

#include <drivers/clk/socfpga.h>
#include <drivers/mmc/mmc.h>
#include <drivers/mmc/mmc_core.h>
#include <drivers/mmc/mmc_host.h>
#include <drivers/mmc/mmc_card.h>

#include "dw_mmc.h"

#include <embox/unit.h>

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

#define DESC_RING_BUF_SZ PAGE_SIZE()

struct idmac_desc_64addr {
	uint32_t des0; /* Control Descriptor */

	uint32_t des1; /* Reserved */

	uint32_t des2; /*Buffer sizes */
#define IDMAC_64ADDR_SET_BUFFER1_SIZE(d, s) \
	((d)->des2 = ((d)->des2 & cpu_to_le32(0x03ffe000)) | \
	 ((cpu_to_le32(s)) & cpu_to_le32(0x1fff)))

	uint32_t des3; /* Reserved */

	uint32_t des4; /* Lower 32-bits of Buffer Address Pointer 1*/
	uint32_t des5; /* Upper 32-bits of Buffer Address Pointer 1*/

	uint32_t des6; /* Lower 32-bits of Next Descriptor Address */
	uint32_t des7; /* Upper 32-bits of Next Descriptor Address */
};

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

POOL_DEF(dw_mci_slot_pool, struct dw_mci_slot, SLOTS_QUANTITY);

static bool dw_mci_ctrl_reset(struct dw_mci *host, uint32_t reset) {
	uint32_t ctrl;
	unsigned long timeout;

	timeout = clock_sys_ticks() + ms2jiffies(2000);

	ctrl = mci_readl(host, CTRL);
	ctrl |= reset;
	mci_writel(host, CTRL, ctrl);

	/* wait till resets clear */
	do {
		ctrl = mci_readl(host, CTRL);
		if (!(ctrl & reset)) {
			return true;
		}
	} while (time_before(clock_sys_ticks(), timeout));

	log_error("Timeout resetting block (ctrl reset %#x)", ctrl & reset);

	return false;
}

/* push final bytes to part_buf, only use during push */
static inline void dw_mci_set_part_bytes(struct dw_mci *host, void *buf, int cnt) {
	memcpy((void *)&host->part_buf, buf, cnt);
	host->part_buf_count = cnt;
}

/* append bytes to part_buf, only use during push */
static inline int dw_mci_push_part_bytes(struct dw_mci *host, void *buf, int cnt) {
	cnt = min(cnt, (1 << host->data_shift) - host->part_buf_count);
	memcpy((void *)&host->part_buf + host->part_buf_count, buf, cnt);
	host->part_buf_count += cnt;
	return cnt;
}

/* pull first bytes from part_buf, only use during pull */
static int dw_mci_pull_part_bytes(struct dw_mci *host, void *buf, int cnt) {
	cnt = min((int)cnt, (int)host->part_buf_count);
	if (cnt) {
		memcpy(buf, (void *)&host->part_buf + host->part_buf_start, cnt);
		host->part_buf_count -= cnt;
		host->part_buf_start += cnt;
	}
	return cnt;
}

/* pull final bytes from the part_buf, assuming it's just been filled */
static inline void dw_mci_pull_final_bytes(struct dw_mci *host, void *buf, int cnt) {
	memcpy(buf, &host->part_buf, cnt);
	host->part_buf_start = cnt;
	host->part_buf_count = (1 << host->data_shift) - cnt;
}

static void dw_mci_push_data16(struct dw_mci *host, void *buf, int cnt) {
}

static void dw_mci_pull_data16(struct dw_mci *host, void *buf, int cnt) {
}

static void dw_mci_push_data32(struct dw_mci *host, void *buf, int cnt) {
	struct mmc_data *data = host->data;
	int init_cnt = cnt;

	/* try and push anything in the part_buf */
	if ((host->part_buf_count)) {
		int len = dw_mci_push_part_bytes(host, buf, cnt);

		buf += len;
		cnt -= len;
		if (host->part_buf_count == 4) {
			mci_fifo_writel(host->fifo_reg, host->part_buf32);
			host->part_buf_count = 0;
		}
	}

	{
		uint32_t *pdata = buf;

		for (; cnt >= 4; cnt -= 4)
			mci_fifo_writel(host->fifo_reg, *pdata++);
		buf = pdata;
	}
	/* put anything remaining in the part_buf */
	if (cnt) {
		dw_mci_set_part_bytes(host, buf, cnt);
		 /* Push data if we have reached the expected data length */
		if ((data->bytes_xfered + init_cnt) ==
		    (data->blksz * data->blocks))
			mci_fifo_writel(host->fifo_reg, host->part_buf32);
	}
}

static void dw_mci_pull_data32(struct dw_mci *host, void *buf, int cnt) {
	{
		uint32_t *pdata = buf;

		for (; cnt >= 4; cnt -= 4)
			*pdata++ = mci_fifo_readl(host->fifo_reg);
		buf = pdata;
	}
	if (cnt) {
		host->part_buf32 = mci_fifo_readl(host->fifo_reg);
		dw_mci_pull_final_bytes(host, buf, cnt);
	}
}

static void dw_mci_push_data64(struct dw_mci *host, void *buf, int cnt) {
}

static void dw_mci_pull_data64(struct dw_mci *host, void *buf, int cnt) {
}

static inline void dw_mci_pull_data(struct dw_mci *host, void *buf, int cnt) {
	int len;

	/* get remaining partial bytes */
	len = dw_mci_pull_part_bytes(host, buf, cnt);
	if (unlikely(len == cnt))
		return;
	buf += len;
	cnt -= len;

	/* get the rest of the data */
	host->pull_data(host, buf, cnt);
}

static inline void dw_mci_read_data_pio(struct dw_mci *host, bool dto) {
	void *buf;
	unsigned int offset;
	struct mmc_data *data = host->data;
	int shift = host->data_shift;
	uint32_t status;
	unsigned int len;
	unsigned int remain, fcnt;

	do {
#if 0
		if (!sg_miter_next(sg_miter))
			goto done;

		host->sg = sg_miter->piter.sg;
		buf = sg_miter->addr;
		remain = sg_miter->length;
#endif
		offset = 0;

		do {
			fcnt = (SDMMC_GET_FCNT(mci_readl(host, STATUS))
					<< shift) + host->part_buf_count;
			len = min(remain, fcnt);
			if (!len)
				break;
			dw_mci_pull_data(host, (void *)(buf + offset), len);
			data->bytes_xfered += len;
			offset += len;
			remain -= len;
		} while (remain);
#if 0
		sg_miter->consumed = offset;
#endif
		status = mci_readl(host, MINTSTS);
		mci_writel(host, RINTSTS, SDMMC_INT_RXDR);
	/* if the RXDR is ready read again */
	} while ((status & SDMMC_INT_RXDR) ||
		 (dto && SDMMC_GET_FCNT(mci_readl(host, STATUS))));
}

static inline void dw_mci_write_data_pio(struct dw_mci *host) {
	void *buf;
	unsigned int offset;
	struct mmc_data *data = host->data;
	int shift = host->data_shift;
	uint32_t status;
	unsigned int len;
	unsigned int fifo_depth = host->fifo_depth;
	unsigned int remain, fcnt;

	do {
#if 0
		if (!sg_miter_next(sg_miter))
			goto done;

		host->sg = sg_miter->piter.sg;
		buf = sg_miter->addr;
		remain = sg_miter->length;
#endif
		offset = 0;

		do {
			fcnt = ((fifo_depth -
				 SDMMC_GET_FCNT(mci_readl(host, STATUS)))
					<< shift) - host->part_buf_count;
			len = min(remain, fcnt);
			if (!len)
				break;
			host->push_data(host, (void *)(buf + offset), len);
			data->bytes_xfered += len;
			offset += len;
			remain -= len;
		} while (remain);
#if 0
		sg_miter->consumed = offset;
#endif
		status = mci_readl(host, MINTSTS);
		mci_writel(host, RINTSTS, SDMMC_INT_TXDR);
	} while (status & SDMMC_INT_TXDR); /* if TXDR write again */
}


/* DMA interface functions */
static inline void dw_mci_stop_dma(struct dw_mci *host) {
	if (host->using_dma) {
		host->dma_ops->stop(host);
		host->dma_ops->cleanup(host);
	}

	/* Data transfer was stopped by the interrupt handler */
//	set_bit(EVENT_XFER_COMPLETE, &host->pending_events);
}

static inline int dw_mci_get_dma_dir(struct mmc_data *data) {
	if (data->flags & MMC_DATA_WRITE) {
		return DMA_TO_DEVICE;
	} else {
		return DMA_FROM_DEVICE;
	}
}

static void dw_mci_dma_cleanup(struct dw_mci *host) {
	struct mmc_data *data = host->data;

	if (data) {
#if 0
		if (!data->host_cookie) {
			dma_unmap_sg(host->dev, data->sg, data->sg_len,
					dw_mci_get_dma_dir(data));
		}
#endif
	}
}

static void dw_mci_idmac_reset(struct dw_mci *host) {
	uint32_t bmod = mci_readl(host, BMOD);
	/* Software reset of DMA */
	bmod |= SDMMC_IDMAC_SWRESET;
	mci_writel(host, BMOD, bmod);
}

static void dw_mci_idmac_stop_dma(struct dw_mci *host) {
	uint32_t temp;

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

static void dw_mci_dmac_complete_dma(void *arg) {
	struct dw_mci *host = arg;
	struct mmc_data *data = host->data;

	log_debug("DMA complete");

	if ((host->use_dma == TRANS_MODE_EDMAC) &&
			data && (data->flags & MMC_DATA_READ)) {
		log_error("TRANS_MODE_EDMAC not support");
#if 0
		/* Invalidate cache after read */
		dma_sync_sg_for_cpu(mmc_dev(host->cur_slot->mmc),
				    data->sg,
				    data->sg_len,
				    DMA_FROM_DEVICE);
#endif
	}

	host->dma_ops->cleanup(host);

	/*
	 * If the card was removed, data will be NULL. No point in trying to
	 * send the stop command or waiting for NBUSY in this case.
	 */
	if (data) {
#if 0
		set_bit(EVENT_XFER_COMPLETE, &host->pending_events);
		tasklet_schedule(&host->tasklet);
#endif
	}
}

static int dw_mci_idmac_init(struct dw_mci *host) {
	int i;

	if (host->dma_64bit_address == 1) {
		struct idmac_desc_64addr *p;
		/* Number of descriptors in the ring buffer */
		host->ring_size =
			DESC_RING_BUF_SZ / sizeof(struct idmac_desc_64addr);

		/* Forward link the descriptor list */
		for (i = 0, p = host->sg_cpu; i < host->ring_size - 1; i++, p++) {
			p->des6 = (host->sg_dma +
					(sizeof(struct idmac_desc_64addr) * (i + 1))) & 0xffffffff;

			p->des7 = (uint64_t)(host->sg_dma +
					(sizeof(struct idmac_desc_64addr) * (i + 1))) >> 32;
			/* Initialize reserved and buffer size fields to "0" */
			p->des1 = 0;
			p->des2 = 0;
			p->des3 = 0;
		}

		/* Set the last descriptor as the end-of-ring descriptor */
		p->des6 = host->sg_dma & 0xffffffff;
		p->des7 = (uint64_t)host->sg_dma >> 32;
		p->des0 = IDMAC_DES0_ER;

	} else {
		struct idmac_desc *p;
		/* Number of descriptors in the ring buffer */
		host->ring_size = DESC_RING_BUF_SZ / sizeof(struct idmac_desc);

		/* Forward link the descriptor list */
		for (i = 0, p = host->sg_cpu; i < host->ring_size - 1; i++, p++) {
			p->des3 = cpu_to_le32(host->sg_dma
					+ (sizeof(struct idmac_desc) * (i + 1)));
			p->des1 = 0;
		}

		/* Set the last descriptor as the end-of-ring descriptor */
		p->des3 = cpu_to_le32(host->sg_dma);
		p->des0 = cpu_to_le32(IDMAC_DES0_ER);
	}

	dw_mci_idmac_reset(host);

	if (host->dma_64bit_address == 1) {
		/* Mask out interrupts - get Tx & Rx complete only */
		mci_writel(host, IDSTS64, IDMAC_INT_CLR);
		mci_writel(host, IDINTEN64, SDMMC_IDMAC_INT_NI |
				SDMMC_IDMAC_INT_RI | SDMMC_IDMAC_INT_TI);

		/* Set the descriptor base address */
		mci_writel(host, DBADDRL, host->sg_dma & 0xffffffff);
		mci_writel(host, DBADDRU, (uint64_t)host->sg_dma >> 32);

	} else {
		/* Mask out interrupts - get Tx & Rx complete only */
		mci_writel(host, IDSTS, IDMAC_INT_CLR);
		mci_writel(host, IDINTEN, SDMMC_IDMAC_INT_NI |
				SDMMC_IDMAC_INT_RI | SDMMC_IDMAC_INT_TI);

		/* Set the descriptor base address */
		mci_writel(host, DBADDR, host->sg_dma);
	}

	return 0;
}

static int dw_mci_idmac_start_dma(struct dw_mci *host, unsigned int sg_len) {
	uint32_t temp;
	int ret = 0;

#if 0
	if (host->dma_64bit_address == 1)
		ret = dw_mci_prepare_desc64(host, host->data, sg_len);
	else
		ret = dw_mci_prepare_desc32(host, host->data, sg_len);

	if (ret)
		goto out;
#endif
	/* TODO drain writebuffer */

	/* Make sure to reset DMA in case we did PIO before this */
	dw_mci_ctrl_reset(host, SDMMC_CTRL_DMA_RESET);
	dw_mci_idmac_reset(host);

	/* Select IDMAC interface */
	temp = mci_readl(host, CTRL);
	temp |= SDMMC_CTRL_USE_IDMAC;
	mci_writel(host, CTRL, temp);

	/* TODO drain writebuffer */

	/* Enable the IDMAC */
	temp = mci_readl(host, BMOD);
	temp |= SDMMC_IDMAC_ENABLE | SDMMC_IDMAC_FB;
	mci_writel(host, BMOD, temp);

	/* Start it running */
	mci_writel(host, PLDMND, 1);

	return ret;
}

static const struct dw_mci_dma_ops dw_mci_idmac_ops = {
	.init = dw_mci_idmac_init,
	.start = dw_mci_idmac_start_dma,
	.stop = dw_mci_idmac_stop_dma,
	.complete = dw_mci_dmac_complete_dma,
	.cleanup = dw_mci_dma_cleanup,
};

static void dw_mci_init_dma(struct dw_mci *host) {
	int addr_config;

	/*
	* Check transfer mode from HCON[17:16]
	* Clear the ambiguous description of dw_mmc databook:
	* 2b'00: No DMA Interface -> Actually means using Internal DMA block
	* 2b'01: DesignWare DMA Interface -> Synopsys DW-DMA block
	* 2b'10: Generic DMA Interface -> non-Synopsys generic DMA block
	* 2b'11: Non DW DMA Interface -> pio only
	* Compared to DesignWare DMA Interface, Generic DMA Interface has a
	* simpler request/acknowledge handshake mechanism and both of them
	* are regarded as external dma master for dw_mmc.
	*/
	host->use_dma = SDMMC_GET_TRANS_MODE(mci_readl(host, HCON));
	if (host->use_dma == DMA_INTERFACE_IDMA) {
		host->use_dma = TRANS_MODE_IDMAC;
	} else if (host->use_dma == DMA_INTERFACE_DWDMA ||
		   host->use_dma == DMA_INTERFACE_GDMA) {
		host->use_dma = TRANS_MODE_EDMAC;
	} else {
		goto no_dma;
	}

	/* Determine which DMA interface to use */
	if (host->use_dma == TRANS_MODE_IDMAC) {
		/*
		* Check ADDR_CONFIG bit in HCON to find
		* IDMAC address bus width
		*/
		addr_config = SDMMC_GET_ADDR_CONFIG(mci_readl(host, HCON));

		if (addr_config == 1) {
			/* host supports IDMAC in 64-bit address mode */
			host->dma_64bit_address = 1;
			log_info("IDMAC supports 64-bit address mode.");
#if 0
			if (!dma_set_mask(host->dev, DMA_BIT_MASK(64))) {
				dma_set_coherent_mask(host->dev, DMA_BIT_MASK(64));
			}
#endif
		} else {
			/* host supports IDMAC in 32-bit address mode */
			host->dma_64bit_address = 0;
			log_info("IDMAC supports 32-bit address mode.");
		}

		/* Alloc memory for sg translation */
		host->sg_cpu = dma_alloc_coherent(NULL, DESC_RING_BUF_SZ, &host->sg_dma, 0);

		if (!host->sg_cpu) {
			log_error("could not alloc DMA memory");
			goto no_dma;
		}

		host->sg_dma = (uintptr_t)host->sg_cpu; // TODO

		host->dma_ops = &dw_mci_idmac_ops;

		log_info("Using internal DMA controller.");
	} else {
#if 0
		/* TRANS_MODE_EDMAC: check dma bindings again */
		if ((device_property_read_string_array(dev, "dma-names",
						       NULL, 0) < 0) ||
		    !device_property_present(dev, "dmas")) {
			goto no_dma;
		}
		host->dma_ops = &dw_mci_edmac_ops;
#endif
		log_info("Using external DMA controller.");
	}

	if (host->dma_ops->init && host->dma_ops->start &&
		host->dma_ops->stop && host->dma_ops->cleanup) {
		if (host->dma_ops->init(host)) {
			log_error("Unable to initialize DMA Controller.");
			goto no_dma;
		}
	} else {
		log_error("DMA initialization not found.");
		goto no_dma;
	}

	return;

no_dma:
	log_info("Using PIO mode.");
	host->use_dma = TRANS_MODE_PIO;
}

static void dw_mci_cmd_interrupt(struct dw_mci *host, uint32_t status) {
	if (!host->cmd_status) {
		host->cmd_status = status;
	}

	host->pending_events |= 1 << EVENT_CMD_COMPLETE;
}

static void dw_mci_handle_cd(struct dw_mci *host) {
	int i;

	for (i = 0; i < host->num_slots; i++) {
		struct dw_mci_slot *slot = host->slot[i];

		if (!slot) {
			continue;
		}
#if 0
		if (slot->mmc->ops->card_event)
			slot->mmc->ops->card_event(slot->mmc);
		mmc_detect_change(slot->mmc,
			msecs_to_jiffies(host->pdata->detect_delay_ms));
#endif
	}
}

static irq_return_t dw_mci_interrupt(unsigned int irq, void *dev_id) {
	struct dw_mci *host = dev_id;
	uint32_t pending;
	int i;

	pending = mci_readl(host, MINTSTS); /* read-only mask reg */
	log_debug("host (%p) pending MINTSTS (%x)", host, pending);

	if (pending) {
		/* Check volt switch first, since it can look like an error */
		if ((host->state == STATE_SENDING_CMD11) &&
				(pending & SDMMC_INT_VOLT_SWITCH)) {
#if 0
			unsigned long irqflags;
#endif
			mci_writel(host, RINTSTS, SDMMC_INT_VOLT_SWITCH);
			pending &= ~SDMMC_INT_VOLT_SWITCH;
#if 0
			/*
			 * Hold the lock; we know cmd11_timer can't be kicked
			 * off after the lock is released, so safe to delete.
			 */
			spin_lock_irqsave(&host->irq_lock, irqflags);
			dw_mci_cmd_interrupt(host, pending);
			spin_unlock_irqrestore(&host->irq_lock, irqflags);

			del_timer(&host->cmd11_timer);
#endif
		}

		if (pending & DW_MCI_CMD_ERROR_FLAGS) {
			mci_writel(host, RINTSTS, DW_MCI_CMD_ERROR_FLAGS);
			host->cmd_status = pending;
#if 0
			smp_wmb(); /* drain writebuffer */
			set_bit(EVENT_CMD_COMPLETE, &host->pending_events);
#endif
		}

		if (pending & DW_MCI_DATA_ERROR_FLAGS) {
			/* if there is an error report DATA_ERROR */
			mci_writel(host, RINTSTS, DW_MCI_DATA_ERROR_FLAGS);
			host->data_status = pending;
#if 0
			smp_wmb(); /* drain writebuffer */
			set_bit(EVENT_DATA_ERROR, &host->pending_events);
			tasklet_schedule(&host->tasklet);
#endif
		}

		if (pending & SDMMC_INT_DATA_OVER) {
#if 0
			del_timer(&host->dto_timer);
#endif
			mci_writel(host, RINTSTS, SDMMC_INT_DATA_OVER);
			if (!host->data_status) {
				host->data_status = pending;
			}
#if 0
			smp_wmb(); /* drain writebuffer */

			if (host->dir_status == DW_MCI_RECV_STATUS) {
				if (host->sg != NULL) {
					dw_mci_read_data_pio(host, true);
				}
			}

			set_bit(EVENT_DATA_COMPLETE, &host->pending_events);
			tasklet_schedule(&host->tasklet);
#endif
		}

		if (pending & SDMMC_INT_RXDR) {
			mci_writel(host, RINTSTS, SDMMC_INT_RXDR);
#if 0
			if (host->dir_status == DW_MCI_RECV_STATUS && host->sg) {
				dw_mci_read_data_pio(host, false);
			}
#endif
		}

		if (pending & SDMMC_INT_TXDR) {
			mci_writel(host, RINTSTS, SDMMC_INT_TXDR);
#if 0
			if (host->dir_status == DW_MCI_SEND_STATUS && host->sg) {
				dw_mci_write_data_pio(host);
			}
#endif
		}

		if (pending & SDMMC_INT_CMD_DONE) {
			mci_writel(host, RINTSTS, SDMMC_INT_CMD_DONE);
			dw_mci_cmd_interrupt(host, pending);
		}

		if (pending & SDMMC_INT_CD) {
			mci_writel(host, RINTSTS, SDMMC_INT_CD);
			dw_mci_handle_cd(host);
		}

		/* Handle SDIO Interrupts */
		for (i = 0; i < host->num_slots; i++) {
			struct dw_mci_slot *slot = host->slot[i];

			if (!slot)
				continue;

			if (pending & SDMMC_INT_SDIO(slot->sdio_id)) {
				mci_writel(host, RINTSTS, SDMMC_INT_SDIO(slot->sdio_id));
#if 0
				mmc_signal_sdio_irq(slot->mmc);
#endif
			}
		}

	}

	if (host->use_dma != TRANS_MODE_IDMAC)
		return IRQ_HANDLED;

	/* Handle IDMA interrupts */
	if (host->dma_64bit_address == 1) {
		pending = mci_readl(host, IDSTS64);
		if (pending & (SDMMC_IDMAC_INT_TI | SDMMC_IDMAC_INT_RI)) {
			mci_writel(host, IDSTS64, SDMMC_IDMAC_INT_TI |
							SDMMC_IDMAC_INT_RI);
			mci_writel(host, IDSTS64, SDMMC_IDMAC_INT_NI);
#if 0
			if (!test_bit(EVENT_DATA_ERROR, &host->pending_events))
				host->dma_ops->complete((void *)host);
#endif
		}
	} else {
		pending = mci_readl(host, IDSTS);
		if (pending & (SDMMC_IDMAC_INT_TI | SDMMC_IDMAC_INT_RI)) {
			mci_writel(host, IDSTS, SDMMC_IDMAC_INT_TI | SDMMC_IDMAC_INT_RI);
			mci_writel(host, IDSTS, SDMMC_IDMAC_INT_NI);
#if 0
			if (!test_bit(EVENT_DATA_ERROR, &host->pending_events))
				host->dma_ops->complete((void *)host);
#endif
		}
	}

	return IRQ_HANDLED;
}

static void dw_mci_wait_while_busy(struct dw_mci *host, uint32_t cmd_flags) {
	unsigned long timeout = clock_sys_ticks() + ms2jiffies(500);

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
			if (time_after(clock_sys_ticks(), timeout)) {
				/* Command will fail; we'll pass error then */
				log_error("Busy; trying anyway");
				break;
			}
			usleep(10);
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

static void mci_send_cmd(struct dw_mci_slot *slot, uint32_t cmd, uint32_t arg) {
	struct dw_mci *host = slot->host;
	unsigned long timeout = 1000000;
	unsigned int cmd_status = 0;

	mci_writel(host, CMDARG, arg);
	/* TODO drain writebuffer */
	dw_mci_wait_while_busy(host, cmd);
	mci_writel(host, CMD, SDMMC_CMD_START | cmd);

	while(timeout --) {
		cmd_status = mci_readl(host, CMD);
		if (!(cmd_status & SDMMC_CMD_START)) {
			return;
		}
	}

	log_error("Timeout sending command (cmd %#x arg %#x status %#x)",
		cmd, arg, cmd_status);
}

static uint32_t dw_mci_prepare_command(struct mmc_host *mmc, struct mmc_command *cmd,
		struct mmc_data *data) {
	uint32_t cmdr;

	cmdr = cmd->opcode;

	if (cmd->opcode == MMC_STOP_TRANSMISSION ||
	    cmd->opcode == MMC_GO_IDLE_STATE ||
	    cmd->opcode == MMC_GO_INACTIVE_STATE) {
		cmdr |= SDMMC_CMD_STOP;
	} else if (cmd->opcode != MMC_SEND_STATUS && data->addr) {
		cmdr |= SDMMC_CMD_PRV_DAT_WAIT;
	}

#if 0
	if (cmd->opcode == SD_SWITCH_VOLTAGE) {
		u32 clk_en_a;

		/* Special bit makes CMD11 not die */
		cmdr |= SDMMC_CMD_VOLT_SWITCH;

		/* Change state to continue to handle CMD11 weirdness */
		WARN_ON(slot->host->state != STATE_SENDING_CMD);
		slot->host->state = STATE_SENDING_CMD11;

		/*
		 * We need to disable low power mode (automatic clock stop)
		 * while doing voltage switch so we don't confuse the card,
		 * since stopping the clock is a specific part of the UHS
		 * voltage change dance.
		 *
		 * Note that low power mode (SDMMC_CLKEN_LOW_PWR) will be
		 * unconditionally turned back on in dw_mci_setup_bus() if it's
		 * ever called with a non-zero clock.  That shouldn't happen
		 * until the voltage change is all done.
		 */
		clk_en_a = mci_readl(host, CLKENA);
		clk_en_a &= ~(SDMMC_CLKEN_LOW_PWR << slot->id);
		mci_writel(host, CLKENA, clk_en_a);
		mci_send_cmd(slot, SDMMC_CMD_UPD_CLK |
			     SDMMC_CMD_PRV_DAT_WAIT, 0);
	}
#endif
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
		if (data->flags & MMC_DATA_WRITE) {
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
	host->dir_status = 0;

	data = &mrq->data;
	if (data->addr) {
		mci_writel(host, TMOUT, 0xFFFFFFFF);
		mci_writel(host, BYTCNT, data->blksz*data->blocks);
		mci_writel(host, BLKSIZ, data->blksz);
	}

	cmdflags = dw_mci_prepare_command(slot->mmc, cmd, &mrq->data);
	if (slot->flags & DW_MMC_CARD_NEED_INIT ) {
		cmdflags |= SDMMC_CMD_INIT;
		slot->flags &= ~DW_MMC_CARD_NEED_INIT;
	}

	if (data->addr) {
		//dw_mci_submit_data(host, data);
		/* TODO drain writebuffer */
	}

	dw_mci_start_command(host, cmd, cmdflags);
#if 0
	if (cmd->opcode == SD_SWITCH_VOLTAGE) {
		unsigned long irqflags;

		/*
		 * Databook says to fail after 2ms w/ no response, but evidence
		 * shows that sometimes the cmd11 interrupt takes over 130ms.
		 * We'll set to 500ms, plus an extra jiffy just in case jiffies
		 * is just about to roll over.
		 *
		 * We do this whole thing under spinlock and only if the
		 * command hasn't already completed (indicating the the irq
		 * already ran so we don't want the timeout).
		 */
		spin_lock_irqsave(&host->irq_lock, irqflags);
		if (!test_bit(EVENT_CMD_COMPLETE, &host->pending_events))
			mod_timer(&host->cmd11_timer,
				jiffies + msecs_to_jiffies(500) + 1);
		spin_unlock_irqrestore(&host->irq_lock, irqflags);
	}

	if (mrq->stop)
		host->stop_cmdr = dw_mci_prepare_command(slot->mmc, mrq->stop);
	else
		host->stop_cmdr = dw_mci_prep_stop_abort(host, cmd);
#endif
}

static void dw_mci_setup_bus(struct dw_mci_slot *slot, bool force_clkinit) {
	struct dw_mci *host = slot->host;
	unsigned int clock = slot->clock;
	uint32_t div;
	uint32_t clk_en_a;
	uint32_t sdmmc_cmd_bits = SDMMC_CMD_UPD_CLK | SDMMC_CMD_PRV_DAT_WAIT;

	log_debug("slot id(%d)", slot->id);

	/* We must continue to set bit 28 in CMD until the change is complete */
	if (host->state == STATE_WAITING_CMD11_DONE) {
		sdmmc_cmd_bits |= SDMMC_CMD_VOLT_SWITCH;
	}

#if 0
	if (!clock) {
		mci_writel(host, CLKENA, 0);
		mci_send_cmd(slot, sdmmc_cmd_bits, 0);
	}
	else if (clock != host->current_speed || force_clkinit) {
		div = host->bus_hz / clock;
		if (host->bus_hz % clock && host->bus_hz > clock)
			/*
			 * move the + 1 after the divide to prevent
			 * over-clocking the card.
			 */
			div += 1;

		div = (host->bus_hz != clock) ? DIV_ROUND_UP(div, 2) : 0;

		if (clock != slot->__clk_old || force_clkinit)
			dev_info(&slot->mmc->class_dev,
				 "Bus speed (slot %d) = %dHz (slot req %dHz, actual %dHZ div = %d)\n",
				 slot->id, host->bus_hz, clock,
				 div ? ((host->bus_hz / div) >> 1) :
				 host->bus_hz, div);
#endif
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
#if 0
		if (!test_bit(DW_MMC_CARD_NO_LOW_PWR, &slot->flags)) {
			clk_en_a |= SDMMC_CLKEN_LOW_PWR << slot->id;
		}
#endif
		clk_en_a |= SDMMC_CLKEN_LOW_PWR << slot->id;

		mci_writel(host, CLKENA, clk_en_a);

		/* inform CIU */
		mci_send_cmd(slot, sdmmc_cmd_bits, 0);

		/* keep the last clock value that was requested from core */
		slot->__clk_old = clock;
#if 0
	}
	host->current_speed = clock;
#endif
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

	host->pending_events &= ~EVENT_CMD_COMPLETE;

	mrq->cmd.resp[0] = mci_readl(host, RESP0);
	mrq->cmd.resp[1] = mci_readl(host, RESP1);
	mrq->cmd.resp[2] = mci_readl(host, RESP2);
	mrq->cmd.resp[3] = mci_readl(host, RESP3);
	log_debug("cmd.resp: %08x %08x %08x %08x", mrq->cmd.resp[0],
			mrq->cmd.resp[1], mrq->cmd.resp[2],mrq->cmd.resp[3]);
}

static void dw_mci_set_ios(struct mmc_host *mmc, struct mmc_ios *ios) {
	struct dw_mci_slot *slot = mmc_priv(mmc);
	uint32_t regs;
#if 0
	const struct dw_mci_drv_data *drv_data = slot->host->drv_data;
	uint32_t regs;
	int ret;

	switch (ios->bus_width) {
	case MMC_BUS_WIDTH_4:
		slot->ctype = SDMMC_CTYPE_4BIT;
		break;
	case MMC_BUS_WIDTH_8:
		slot->ctype = SDMMC_CTYPE_8BIT;
		break;
	default:
		/* set default 1 bit mode */
		slot->ctype = SDMMC_CTYPE_1BIT;
	}

	regs = mci_readl(slot->host, UHS_REG);

	/* DDR mode set */
	if (ios->timing == MMC_TIMING_MMC_DDR52 ||
	    ios->timing == MMC_TIMING_UHS_DDR50 ||
	    ios->timing == MMC_TIMING_MMC_HS400)
		regs |= ((0x1 << slot->id) << 16);
	else
		regs &= ~((0x1 << slot->id) << 16);

	mci_writel(slot->host, UHS_REG, regs);
	slot->host->timing = ios->timing;

	/*
	 * Use mirror of ios->clock to prevent race with mmc
	 * core ios update when finding the minimum.
	 */
	slot->clock = ios->clock;

	if (drv_data && drv_data->set_ios)
		drv_data->set_ios(slot->host, ios);

	switch (ios->power_mode) {
	case MMC_POWER_UP:
		if (!IS_ERR(mmc->supply.vmmc)) {
			ret = mmc_regulator_set_ocr(mmc, mmc->supply.vmmc,
					ios->vdd);
			if (ret) {
				dev_err(slot->host->dev,
					"failed to enable vmmc regulator\n");
				/*return, if failed turn on vmmc*/
				return;
			}
		}
		set_bit(DW_MMC_CARD_NEED_INIT, &slot->flags);
#endif
		slot->flags |= DW_MMC_CARD_NEED_INIT;

		regs = mci_readl(slot->host, PWREN);
		regs |= (1 << slot->id);
		mci_writel(slot->host, PWREN, regs);
#if 0
		break;
	case MMC_POWER_ON:
		if (!slot->host->vqmmc_enabled) {
			if (!IS_ERR(mmc->supply.vqmmc)) {
				ret = regulator_enable(mmc->supply.vqmmc);
				if (ret < 0)
					dev_err(slot->host->dev,
						"failed to enable vqmmc\n");
				else
					slot->host->vqmmc_enabled = true;

			} else {
				/* Keep track so we don't reset again */
				slot->host->vqmmc_enabled = true;
			}

			/* Reset our state machine after powering on */
			dw_mci_ctrl_reset(slot->host,
					  SDMMC_CTRL_ALL_RESET_FLAGS);
		}
#endif
		/* Adjust clock / bus width after power is up */
		dw_mci_setup_bus(slot, false);
#if 0
		break;
	case MMC_POWER_OFF:
		/* Turn clock off before power goes down */
		dw_mci_setup_bus(slot, false);

		if (!IS_ERR(mmc->supply.vmmc))
			mmc_regulator_set_ocr(mmc, mmc->supply.vmmc, 0);

		if (!IS_ERR(mmc->supply.vqmmc) && slot->host->vqmmc_enabled)
			regulator_disable(mmc->supply.vqmmc);
		slot->host->vqmmc_enabled = false;

		regs = mci_readl(slot->host, PWREN);
		regs &= ~(1 << slot->id);
		mci_writel(slot->host, PWREN, regs);
		break;
	default:
		break;
	}

	if (slot->host->state == STATE_WAITING_CMD11_DONE && ios->clock != 0)
		slot->host->state = STATE_IDLE;
#endif
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

static int dw_mci_init_slot(struct dw_mci *host, unsigned int id) {
	struct mmc_host *mmc;
	struct dw_mci_slot *slot;
#if 0
	const struct dw_mci_drv_data *drv_data = host->drv_data;
	int ctrl_id, ret;
	uint32_t freq[2];
#endif

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
	slot->id = id;
	slot->sdio_id = host->sdio_id0 + id;
	slot->mmc = mmc;
	slot->host = host;
	host->slot[id] = slot;

	mmc->ops = &dw_mci_ops;

	mmc->f_min = DW_MCI_FREQ_MIN;
	mmc->f_max = DW_MCI_FREQ_MAX;
#if 0
	if (device_property_read_u32_array(host->dev, "clock-freq-min-max",
					   freq, 2)) {
		mmc->f_min = DW_MCI_FREQ_MIN;
		mmc->f_max = DW_MCI_FREQ_MAX;
	} else {
		mmc->f_min = freq[0];
		mmc->f_max = freq[1];
	}

	/*if there are external regulators, get them*/
	ret = mmc_regulator_get_supply(mmc);
	if (ret == -EPROBE_DEFER)
		goto err_host_allocated;

	if (!mmc->ocr_avail)
		mmc->ocr_avail = MMC_VDD_32_33 | MMC_VDD_33_34;

	if (host->pdata->caps)
		mmc->caps = host->pdata->caps;

	/*
	 * Support MMC_CAP_ERASE by default.
	 * It needs to use trim/discard/erase commands.
	 */
	mmc->caps |= MMC_CAP_ERASE;

	if (host->pdata->pm_caps)
		mmc->pm_caps = host->pdata->pm_caps;

	if (host->dev->of_node) {
		ctrl_id = of_alias_get_id(host->dev->of_node, "mshc");
		if (ctrl_id < 0)
			ctrl_id = 0;
	} else {
		ctrl_id = to_platform_device(host->dev)->id;
	}
	if (drv_data && drv_data->caps)
		mmc->caps |= drv_data->caps[ctrl_id];

	if (host->pdata->caps2)
		mmc->caps2 = host->pdata->caps2;

	ret = mmc_of_parse(mmc);
	if (ret)
		goto err_host_allocated;
#endif
	/* Useful defaults if platform data is unset. */
	if (host->use_dma == TRANS_MODE_IDMAC) {
		mmc->max_segs = host->ring_size;
		mmc->max_blk_size = 65535;
		mmc->max_seg_size = 0x1000;
		mmc->max_req_size = mmc->max_seg_size * host->ring_size;
		mmc->max_blk_count = mmc->max_req_size / 512;
	} else if (host->use_dma == TRANS_MODE_EDMAC) {
		mmc->max_segs = 64;
		mmc->max_blk_size = 65535;
		mmc->max_blk_count = 65535;
		mmc->max_req_size = mmc->max_blk_size * mmc->max_blk_count;
		mmc->max_seg_size = mmc->max_req_size;
	} else {
		/* TRANS_MODE_PIO */
		mmc->max_segs = 64;
		mmc->max_blk_size = 65535; /* BLKSIZ is 16 bits */
		mmc->max_blk_count = 512;
		mmc->max_req_size = mmc->max_blk_size * mmc->max_blk_count;
		mmc->max_seg_size = mmc->max_req_size;
	}

	dw_mci_get_cd(mmc);
#if 0
	ret = mmc_add_host(mmc);
	if (ret)
		goto err_host_allocated;

#if defined(CONFIG_DEBUG_FS)
	dw_mci_init_debugfs(slot);
#endif
#endif
	return 0;
#if 0
err_host_allocated:
	mmc_free_host(mmc);
	return ret;
#endif
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
	int i;
	int width;
	uint32_t fifo_size;
	int init_slots = 0;

	/*
	 * Get the host data width - this assumes that HCON has been set with
	 * the correct values.
	 */
	i = SDMMC_GET_HDATA_WIDTH(mci_readl(host, HCON));
	if (!i) {
		host->push_data = dw_mci_push_data16;
		host->pull_data = dw_mci_pull_data16;
		width = 16;
		host->data_shift = 1;
	} else if (i == 2) {
		host->push_data = dw_mci_push_data64;
		host->pull_data = dw_mci_pull_data64;
		width = 64;
		host->data_shift = 3;
	} else {
		/* Check for a reserved value, and warn if it is */
		if (i != 1) {
			log_info("HCON reports a reserved host data width!");
			log_info("Defaulting to 32-bit access.");
		}

		host->push_data = dw_mci_push_data32;
		host->pull_data = dw_mci_pull_data32;
		width = 32;
		host->data_shift = 2;
	}

	/* Reset all blocks */
	if (!dw_mci_ctrl_reset(host, SDMMC_CTRL_ALL_RESET_FLAGS)) {
		return -ENODEV;
	}

	dw_mci_init_dma(host);

	/* Clear the interrupts for the host controller */
	mci_writel(host, RINTSTS, 0xFFFFFFFF);
	mci_writel(host, INTMASK, 0); /* disable all mmc interrupt first */

	/* Put in max timeout */
	mci_writel(host, TMOUT, 0xFFFFFFFF);

	/*
	 * FIFO threshold settings  RxMark  = fifo_size / 2 - 1,
	 *                          Tx Mark = fifo_size / 2 DMA Size = 8
	 */
	/*
	 * Power-on value of RX_WMark is FIFO_DEPTH-1, but this may
	 * have been overwritten by the bootloader, just like we're
	 * about to do, so if you know the value for your hardware, you
	 * should put it in the platform data.
	 */
	fifo_size = mci_readl(host, FIFOTH);
	fifo_size = 1 + ((fifo_size >> 16) & 0xfff);

	host->fifo_depth = fifo_size;
	host->fifoth_val = SDMMC_SET_FIFOTH(0x2, fifo_size / 2 - 1, fifo_size / 2);
	mci_writel(host, FIFOTH, host->fifoth_val);

	/* disable clock to CIU */
	mci_writel(host, CLKENA, 0);
	mci_writel(host, CLKSRC, 0);

	/*
	 * In 2.40a spec, Data offset is changed.
	 * Need to check the version-id and set data-offset for DATA register.
	 */
	host->verid = SDMMC_GET_VERID(mci_readl(host, VERID));
	log_info("Version ID is %04x", host->verid);

	if (host->verid < DW_MMC_240A) {
		host->fifo_reg = host->regs + DATA_OFFSET;
	} else{
		host->fifo_reg = host->regs + DATA_240A_OFFSET;
	}

	host->num_slots = 1;

	/* Enable interrupts for command done, data over, data empty,
	 * receive ready and error such as transmit, receive timeout, crc error */
	mci_writel(host, INTMASK, SDMMC_INT_CMD_DONE | SDMMC_INT_DATA_OVER |
			SDMMC_INT_TXDR | SDMMC_INT_RXDR | DW_MCI_ERROR_FLAGS);
	/* Enable mci interrupt */
	mci_writel(host, CTRL, SDMMC_CTRL_INT_ENABLE);

	log_info("DW MMC controller at irq %d,%d bit host data width,%u deep fifo",
			host->irq, width, fifo_size);

	/* We need at least one slot to succeed */
	for (i = 0; i < host->num_slots; i++) {
		ret = dw_mci_init_slot(host, i);
		if (ret) {
			log_debug("slot %d init failed", i);
		} else {
			init_slots++;
		}
	}

	if (init_slots) {
		log_info("%d slots initialized", init_slots);
	} else {
		log_debug("attempted to initialize %d slots, but failed on all",
			host->num_slots);
		return -EBUSY;
	}

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

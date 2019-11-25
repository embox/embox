/**
 * @file sdhc_ops.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 30.10.2019
 */
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <drivers/mmc/mmc_core.h>
#include <drivers/mmc/mmc_host.h>
#include <util/log.h>

#include "sdhci_ops.h"

static void sdhci_reg_dump(struct sdhci_host *host) {
	log_debug("======================================");
	log_debug("REG DUMP");
	log_debug("======================================");
	log_debug("USDHC_DS_ADDR              =0x%08x", sdhci_readl(host, 0x00));
	log_debug("USDHC_BLK_ATT              =0x%08x", sdhci_readl(host, 0x04));
	log_debug("USDHC_CMD_ARG              =0x%08x", sdhci_readl(host, 0x08));
	log_debug("USDHC_CMD_XFR_TYP          =0x%08x", sdhci_readl(host, 0x0C));
	log_debug("USDHC_CMD_RSP0             =0x%08x", sdhci_readl(host, 0x10));
	log_debug("USDHC_CMD_RSP1             =0x%08x", sdhci_readl(host, 0x14));
	log_debug("USDHC_CMD_RSP2             =0x%08x", sdhci_readl(host, 0x18));
	log_debug("USDHC_CMD_RSP3             =0x%08x", sdhci_readl(host, 0x1C));
	log_debug("USDHC_DATA_BUFF_ACC_PORT   =0x%08x", sdhci_readl(host, 0x20));
	log_debug("USDHC_PRES_STATE           =0x%08x", sdhci_readl(host, 0x24));
	log_debug("USDHC_PROT_CTRL            =0x%08x", sdhci_readl(host, 0x28));
	log_debug("USDHC_SYS_CTRL             =0x%08x", sdhci_readl(host, 0x2C));
	log_debug("USDHC_INT_STATUS           =0x%08x", sdhci_readl(host, 0x30));
	log_debug("USDHC_INT_STATUS_EN        =0x%08x", sdhci_readl(host, 0x34));
	log_debug("USDHC_INT_SIGNAL_EN        =0x%08x", sdhci_readl(host, 0x38));
	log_debug("USDHC_AUTOCMD12_ERR_STATUS =0x%08x", sdhci_readl(host, 0x3C));
	log_debug("USDHC_HOST_CTRL_CAP        =0x%08x", sdhci_readl(host, 0x40));
	log_debug("USDHC_WTMK_LVL             =0x%08x", sdhci_readl(host, 0x44));
	log_debug("USDHC_MIX_CTRL             =0x%08x", sdhci_readl(host, 0x48));
	log_debug("USDHC_FORCE_EVENT          =0x%08x", sdhci_readl(host, 0x50));
	log_debug("USDHC_ADMA_ERR_STATUS      =0x%08x", sdhci_readl(host, 0x54));
	log_debug("USDHC_ADMA_SYS_ADDR        =0x%08x", sdhci_readl(host, 0x58));
	log_debug("USDHC_DLL_CTRl             =0x%08x", sdhci_readl(host, 0x60));
	log_debug("USDHC_DLL_STATUS           =0x%08x", sdhci_readl(host, 0x64));
	log_debug("USDHC_CLK_TUNE_CTRL_STATUS =0x%08x", sdhci_readl(host, 0x68));
	log_debug("USDHC_VEND_SPEC            =0x%08x", sdhci_readl(host, 0xC0));
	log_debug("USDHC_MMC_BOOT             =0x%08x", sdhci_readl(host, 0xC4));
	log_debug("USDHC_VEND_SPEC2           =0x%08x", sdhci_readl(host, 0xC8));
}

#define RETRIES    100
static int sdhci_reset(struct sdhci_host *host) {
	int retry = RETRIES;

	sdhci_orl(host, USDHC_SYS_CTRL, USDHC_SYS_CTRL_RSTA);

	sdhci_writel(host, USDHC_SYS_CTRL, 0x8b011f);

	/* Wait until both CMD and DAT lines are inactive */
	while (sdhci_readl(host, USDHC_PRES_STATE) & (
			USDHC_PRES_STATE_CDIHB | USDHC_PRES_STATE_CIHB)) {
		usleep(USEC_PER_MSEC);
		if (retry-- <= 0) {
			log_error("Timeout!");
			return EBUSY;
		}
	};

	/* Wait 80 clocks */
	sdhci_orl(host, USDHC_SYS_CTRL, USDHC_SYS_CTRL_INITA);

	if (sdhci_readl(host, USDHC_SYS_CTRL) & USDHC_SYS_CTRL_RSTA) {
		log_error("Reset timeout");
		return EBUSY;
	}

	sdhci_writel(host, USDHC_MIX_CTRL, 0);
	sdhci_writel(host, USDHC_CLK_TUNE_CTRL_STATUS, 0);

	usleep(100 * USEC_PER_MSEC);
	return 0;
}

static void sdhci_cmd_done(struct sdhci_host *host, struct mmc_request *req) {
	int i;

	assert(host);
	assert(req);

	if (req->cmd.flags & MMC_RSP_136) {
		/* CRC is stripped so we need to do some shifting. */
		for (i = 0; i < 4; i++) {
			req->cmd.resp[i] = sdhci_readl(host,
					USDHC_CMD_RSP0 + (3 - i) * 4) << 8;
			if (i != 3) {
				req->cmd.resp[i] |= sdhci_readb(host,
						USDHC_CMD_RSP0 + (3 - i) * 4 - 1);
			}
		}
	} else {
		req->cmd.resp[0] = sdhci_readl(host, USDHC_CMD_RSP0);
	}
}

static void imx6_usdhc_send_cmd(struct sdhci_host *host, int cmd_idx, uint32_t arg, int flags) {
	uint32_t wcmd = 0, mix = 0;
	int retry = RETRIES;

	wcmd |= (cmd_idx & 0x3f) << 24;

	if (!(flags & MMC_RSP_PRESENT)) {
	} else if (flags & MMC_RSP_136) {
		wcmd |= 0x1 << 16;
	} else if (flags & MMC_RSP_BUSY){
		wcmd |= 0x3 << 16;
	} else {
		wcmd |= 0x2 << 16;
	}

	if (flags & MMC_RSP_CRC) {
		wcmd |= 1 << 19;
	}

	if (flags & MMC_DATA_XFER) {
		wcmd |= 0x1 << 21;

		mix |= MIX_DMAEN;

		if (flags & MMC_DATA_READ) {
			mix |= MIX_CARD2HOST;
		}
	}


	sdhci_writel(host, USDHC_INT_STATUS, 0xffffffff);

	while(sdhci_readl(host, USDHC_PRES_STATE) & 0x7) {
		usleep(USEC_PER_MSEC);
		if (retry-- <= 0) {
			log_error("Card is busy! PRES_STATE=%08x",
					sdhci_readl(host, USDHC_PRES_STATE));
			return;
		}
	}

	sdhci_writel(host, USDHC_CMD_ARG, arg);
	sdhci_writel(host, USDHC_MIX_CTRL, mix);

	log_debug("send cmd: 0x%08x idx = %d arg = %x", wcmd, cmd_idx, arg);

	sdhci_writel(host, USDHC_CMD_XFR_TYP, wcmd);

	/* Wait command to be completed */
	retry = RETRIES;
	while(!(sdhci_readl(host, USDHC_INT_STATUS) & USDHC_INT_STATUS_CC)) {
		usleep(USEC_PER_MSEC);
		if (retry-- <= 0) {
			log_error("Timeout!");
			sdhci_reset(host);
			break;
		}
	}

	sdhci_writel(host, USDHC_INT_STATUS, USDHC_INT_STATUS_CC);
}

static int sdhci_get_cd(struct mmc_host *host) {
	assert(host);

	if (sdhci_readl(host->priv, USDHC_PRES_STATE) & USDHC_PRES_STATE_CDPL) {
		return 1;
	} else {
		return 0;
	}
}

void dcache_inval(const void *p, size_t size);
void dcache_flush(const void *p, size_t size);

static uint8_t _buf[512] __attribute__ ((aligned(128))); /* Workaround for unaligned buffer */
static void sdhci_mmc_request(struct mmc_host *mmc_host, struct mmc_request *req) {
	struct sdhci_host *host;

	assert(mmc_host);
	assert(req);

	host = mmc_host->priv;

	log_debug("req->cmd.opcode %x, req->cmd.arg %x, req->cmd.flags %x",
			req->cmd.opcode, req->cmd.arg, req->cmd.flags);

	if (req->cmd.flags & MMC_DATA_XFER) {
		memset(_buf, 0, sizeof(_buf));
		sdhci_writel(host, USDHC_DS_ADDR, (uint32_t) _buf);
		if (req->cmd.flags & MMC_DATA_WRITE) {
			memcpy(_buf, (void *) req->data.addr, sizeof(_buf));
			dcache_flush(_buf, sizeof(_buf));
		}
	}

	imx6_usdhc_send_cmd(host, req->cmd.opcode, req->cmd.arg, req->cmd.flags);

	sdhci_cmd_done(host, req);

	if (req->cmd.flags & MMC_DATA_XFER) {
		int timeout = RETRIES;
		while((sdhci_readl(host, USDHC_PRES_STATE) & (1 << 9))) {
			usleep(10 * USEC_PER_MSEC);
			if (timeout-- <= 0) {
				log_error("Data transfer timeout");
				break;
			}
		}
		if (req->cmd.flags & MMC_DATA_READ) {
			dcache_inval(_buf, sizeof(_buf));
			memcpy((void *) req->data.addr, _buf, sizeof(_buf));
		}
	}
}

static const struct mmc_host_ops sdhci_mmc_ops = {
	.request = sdhci_mmc_request,
	.get_cd  = sdhci_get_cd,
};

static void sdhci_set_bus_width(struct sdhci_host *host, int bits) {
	uint32_t tmp;

	tmp = sdhci_readl(host, USDHC_PROT_CTRL);
	tmp &= ~USDHC_PROT_CTRL_DTW_MASK;
	switch (bits) {
	case 1:
		tmp |= USDHC_PROT_CTRL_DTW_1BIT;
		break;
	case 4:
		tmp |= USDHC_PROT_CTRL_DTW_4BIT;
		break;
	case 8:
		tmp |= USDHC_PROT_CTRL_DTW_4BIT;
		break;
	default:
		log_error("Unsupported data width: %d bits, use 1 bit anyway",
				bits);
		tmp |= USDHC_PROT_CTRL_DTW_1BIT;
	}

	sdhci_writel(host, USDHC_PROT_CTRL, tmp);
}

int sdhci_hw_probe(struct sdhci_host *host) {
	uint32_t tmp;
	int err;
	struct mmc_host *mmc;
	if (0 != (err = sdhci_reset(host))) {
		return err;
	}

	sdhci_writel(host, USDHC_INT_STATUS_EN, -1);
	/* TODO: figure the real data bus width */
	sdhci_set_bus_width(host, 1);

	tmp = 512 | (1 << 16);
	sdhci_writel(host, USDHC_BLK_ATT, tmp);

	mmc = mmc_alloc_host();
	mmc->ops = &sdhci_mmc_ops;
	mmc->priv = host;
	if (0 != mmc_scan(mmc)) {
		mmc_dev_destroy(mmc);
	}

	sdhci_reg_dump(host);

	return 0;
}

/**
 * @file sdhc_ops.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 30.10.2019
 */
#include <assert.h>
#include <errno.h>
#include <unistd.h>

#include <drivers/mmc/mmc_core.h>
#include <drivers/mmc/mmc_host.h>
#include <util/log.h>

#include "sdhci_ops.h"

void sdhci_reg_dump(struct sdhci_host *host) {
	log_debug("===================");
	log_debug("REG DUMP");
	log_debug("===================");
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

#define RETRIES    1000
#define TIMEOUT    1000
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
	uint32_t wcmd = 0;
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
		/* enable DMA in imx it places in MIX_CTRL */
		wcmd |= 0x1;
		if (flags & MMC_DATA_READ) {
			wcmd |= 0x10;
		}
	}

	sdhci_writel(host, USDHC_CMD_ARG, arg);

	log_debug("send cmd: 0x%08x idx = %d arg = %x", wcmd, cmd_idx, arg);

	sdhci_writel(host, USDHC_CMD_XFR_TYP, wcmd);

	/* Wait command to be completed */
	while(!(sdhci_readl(host, USDHC_INT_STATUS) & USDHC_INT_STATUS_CC)) {
		usleep(USEC_PER_MSEC);
		if (retry-- <= 0) {
			log_error("Timeout!");
			break;
		}
	}

	log_debug("USDHC_INT_STATUS=%08x", sdhci_readl(host, USDHC_INT_STATUS));
	sdhci_writel(host, USDHC_INT_STATUS, USDHC_INT_STATUS_CC);
}

static int sdhci_reset(struct sdhci_host *host) {
	sdhci_writel(host, USDHC_SYS_CTRL, sdhci_readl(host, USDHC_SYS_CTRL) | USDHC_SYS_CTRL_RSTA);

	int timeout = TIMEOUT;
	while(timeout--);

	if (sdhci_readl(host, USDHC_SYS_CTRL) & USDHC_SYS_CTRL_RSTA) {
		log_error("Reset timeout");
		return EBUSY;
	}

	sdhci_writel(host, USDHC_VEND_SPEC,
			sdhci_readl(host, USDHC_VEND_SPEC) | (1 << 31) | (1 << 14) | (1 << 13));

	return 0;
}

static int sdhci_get_cd(struct mmc_host *host) {
	assert(host);

	if (sdhci_readl(host->priv, USDHC_PRES_STATE) & USDHC_PRES_STATE_CDPL) {
		return 1;
	} else {
		return 0;
	}
}

static void sdhci_mmc_request(struct mmc_host *mmc_host, struct mmc_request *req) {
	struct sdhci_host *host;

	assert(mmc_host);
	assert(req);

	host = mmc_host->priv;

	log_debug("req->cmd.opcode %x, req->cmd.arg %x, req->cmd.flags %x",
			req->cmd.opcode, req->cmd.arg, req->cmd.flags);

	if (req->cmd.flags & MMC_DATA_XFER) {
		sdhci_writel(host, USDHC_INT_STATUS, -1);

		sdhci_writel(host, USDHC_DS_ADDR, (uint32_t) req->data.addr);
	}

	imx6_usdhc_send_cmd(host, req->cmd.opcode, req->cmd.arg, req->cmd.flags);

	sdhci_cmd_done(host, req);
}

static const struct mmc_host_ops sdhci_mmc_ops = {
	.request = sdhci_mmc_request,
	.get_cd  = sdhci_get_cd,
};

int sdhci_hw_probe(struct sdhci_host *host) {
	uint32_t tmp;
	int err;
	struct mmc_host *mmc;

	if (0 != (err = sdhci_reset(host))) {
		return err;
	}

	sdhci_writel(host, USDHC_SYS_CTRL, 0xb011f);
	sdhci_writel(host, USDHC_INT_STATUS_EN, -1);
	sdhci_orl(host, USDHC_PROT_CTRL,
			USDHC_PROT_CTRL_LCTL | USDHC_PROT_CTRL_DTW_4BIT);

	tmp = 512 | (1 << 16);
	sdhci_writel(host, USDHC_BLK_ATT, tmp);
	if (sdhci_readl(host, USDHC_PRES_STATE) & USDHC_PRES_STATE_CDPL) {
		mmc = mmc_alloc_host();
		mmc->ops = &sdhci_mmc_ops;
		mmc->priv = host;
		mmc_scan(mmc);
		sdhci_reg_dump(host);
	} else {
		log_debug("CDPL bit is zero! %08x", sdhci_readl(host, USDHC_PRES_STATE));
	}

	return 0;
}

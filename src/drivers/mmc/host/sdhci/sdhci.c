/**
 * @file
 * @brief base SDHCI driver for MMC card
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 01.05.2017
 */

#include <string.h>
#include <unistd.h>

#include <drivers/block_dev.h>
#include <drivers/common/memory.h>
#include <drivers/mmc/mmc_core.h>
#include <drivers/mmc/mmc_host.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <util/log.h>

#include <embox/unit.h>

#include "sdhci.h"

#define NEW_API 1

#define BASE_ADDR  OPTION_GET(NUMBER, base_addr)
#define IRQ_NUM    OPTION_GET(NUMBER, irq_num)
#define MAX_HC_NUM OPTION_GET(NUMBER, hc_quantity)

#define BLK_LEN    512

#define TIMEOUT    0x1FFFFF
#define RETRIES    1000

#define SDHCI_IRQ_EN_BITS (1 | 2 | 0x100 | 0x10000 | 0x20000 | 0x40000 | 0x80000 | 0x100000 | 0x400000 | 0x20 | 0x10 | 0x8)

EMBOX_UNIT_INIT(sdhci_init);

static void _reg_dump(struct sdhci_host *host) {
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
#if NEW_API

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

#else
	 wcmd = (cmd_idx & 0x3f) << 24 | (1 << 20) | (1 << 21) | (cmd_idx == 17 ? (0x10) : 0) | 1 | (cmd_idx == 9 ? 0x3 << 16: 0);
#endif
	sdhci_writel(host, USDHC_CMD_ARG, arg);

	log_debug("send cmd: 0x%08x idx = %d arg = %x", wcmd, cmd_idx, arg);

	sdhci_writel(host, USDHC_CMD_XFR_TYP, wcmd);

	/* Wait command to be completed */
	while(!(sdhci_readl(host, USDHC_INT_STATUS) & 0x1) && retry > 0) {
		usleep(USEC_PER_MSEC);
		retry--;
	}

	if (retry == 0) {
		log_debug("Timeout!");
	}

	log_debug("USDHC_INT_STATUS=%08x", sdhci_readl(host, USDHC_INT_STATUS));
	sdhci_writel(host, USDHC_INT_STATUS, 0x1);
}

static void imx6_usdhc_reset(struct sdhci_host *host) {
	sdhci_writel(host, USDHC_SYS_CTRL, sdhci_readl(host, USDHC_SYS_CTRL) | USDHC_SYS_CTRL_RSTA);

	int timeout = TIMEOUT;
	while(timeout--);

	if (sdhci_readl(host, USDHC_SYS_CTRL) & USDHC_SYS_CTRL_RSTA) {
		log_error("Reset timeout");
	}

	sdhci_writel(host, USDHC_VEND_SPEC, sdhci_readl(host, USDHC_VEND_SPEC) | (1 << 31) | (1 << 14) | (1 << 13));
}

static inline void imx6_usdhc_set_block_len(struct sdhci_host *host, size_t len) {
	uint32_t tmp;

	imx6_usdhc_send_cmd(host, 16, len, 0);

	tmp = len | (1 << 16);
	sdhci_writel(host, USDHC_BLK_ATT, tmp);
}

static int imx6_usdhc_read(struct block_dev *bdev, char *buffer, size_t count,
		blkno_t blkno) {
	struct sdhci_host *host = bdev->privdata;
	int timeout = TIMEOUT;

	log_debug("sdhc read, addr=0x%x, count=%d", blkno * bdev->block_size, count);

	memset(buffer, 0, count);

	sdhci_writel(host, USDHC_INT_STATUS, -1);

	sdhci_writel(host, USDHC_DS_ADDR, (uint32_t) buffer);

	imx6_usdhc_send_cmd(host, 17, blkno * BLK_LEN, 0);

	while(!(sdhci_readl(host, USDHC_INT_STATUS) & 1) && timeout--);

	if (timeout == 0) {
		log_debug("read timeout");
	}

	return count;
}

static int imx6_usdhc_write(struct block_dev *bdev, char *buffer, size_t count,
		blkno_t blkno) {
	struct sdhci_host *host = bdev->privdata;

	log_debug("sdhc write, addr=0x%x, count=%d", blkno * bdev->block_size, count);

	sdhci_writel(host, USDHC_INT_STATUS, -1);

	sdhci_writel(host, USDHC_DS_ADDR, (uint32_t) buffer);

	imx6_usdhc_send_cmd(host, 24, blkno * BLK_LEN, 0);

	return count;
}

struct block_dev_driver imx6_usdhc_driver = {
	.name  = "imx6_usdhc_driver",
	.probe = NULL,
	.read  = imx6_usdhc_read,
	.write = imx6_usdhc_write
};

PERIPH_MEMORY_DEFINE(imx6_usdhc, BASE_ADDR, 0x4000 * MAX_HC_NUM);

#if !NEW_API
static uint32_t sdhc_get_size(struct sdhci_host *host) {
	uint32_t t;
	/* Assume it's not high-capacity SD card */
	imx6_usdhc_send_cmd(host, 9, 0x4567 << 16, 7);
	t = (sdhci_readl(host, USDHC_CMD_RSP1) >> 6) & 0x3;
	t |= (sdhci_readl(host, USDHC_CMD_RSP1) >> 24) << 2;
	t |= (sdhci_readl(host, USDHC_CMD_RSP2) & 0x3) << 10;

	if (t == 0) {
		return 0;
	}
	return 256 * 1024 * (t + 1);
}
#endif

static inline void sdhci_mmc_request(struct mmc_host *mmc_host, struct mmc_request *req) {
	struct sdhci_host *host;

	assert(mmc_host);
	assert(req);

	host = mmc_host->priv;

	log_debug("req->cmd.opcode %x, req->cmd.arg %x, req->cmd.flags %x", req->cmd.opcode, req->cmd.arg, req->cmd.flags);

	if (req->cmd.flags & MMC_DATA_XFER) {
		sdhci_writel(host, USDHC_INT_STATUS, -1);

		sdhci_writel(host, USDHC_DS_ADDR, (uint32_t) req->data.addr);
	}

	imx6_usdhc_send_cmd(host, req->cmd.opcode, req->cmd.arg, req->cmd.flags);

	sdhci_cmd_done(host, req);
}

static const struct mmc_host_ops sdhci_mmc_ops = {
	.request = sdhci_mmc_request,
};

static struct sdhci_host sdhci_hosts[MAX_HC_NUM];

static  inline int sdhci_hw_probe(struct sdhci_host *host) {
	uint32_t tmp;
#if NEW_API
	struct mmc_host *mmc;
#else
	struct block_dev *bdev;
	size_t bsize;
#endif
	/* SDHC initialization */
	imx6_usdhc_reset(host);

	sdhci_writel(host, USDHC_SYS_CTRL, 0xb011f);
	sdhci_writel(host, USDHC_INT_STATUS_EN, -1);
	sdhci_writel(host, USDHC_PROT_CTRL, sdhci_readl(host, USDHC_PROT_CTRL) | 1 | 4);

	tmp = 512 | (1 << 16);
	sdhci_writel(host, USDHC_BLK_ATT, tmp);
//	imx6_usdhc_set_block_len(host, BLK_LEN);
#if !NEW_API
	/* SD initialization */
	imx6_usdhc_send_cmd(host, 0, 0, 0);
	imx6_usdhc_send_cmd(host, 8, 0x1AA, 0);
	imx6_usdhc_send_cmd(host, 55, 0x0, 0x15);
	imx6_usdhc_send_cmd(host, 41, 0x4000FFFF, 1);

	imx6_usdhc_send_cmd(host, 2, 0, 7);
	imx6_usdhc_send_cmd(host, 3, 0, 15);

	bsize = sdhc_get_size(host);
	log_debug("bdev size is %d\n", bsize);
	if (bsize == 0) {
		return 0;
	}
	imx6_usdhc_send_cmd(host, 7, 0x4567 << 16, 0);

	bdev = block_dev_create("sdhc", &imx6_usdhc_driver, host);

	bdev->block_size = BLK_LEN;
	bdev->size = bsize;
#else /* !NEW_API */
	if (sdhci_readl(host, USDHC_PRES_STATE) & USDHC_PRES_STATE_CDPL) {
		mmc = mmc_alloc_host();
		mmc->ops = &sdhci_mmc_ops;
		mmc->priv = host;
		mmc_sw_reset(mmc);
	}
#endif /* !NEW_API */

	_reg_dump(host);

	return 0;
}

static int sdhci_init(void) {
	int i;

	for (i = 0; i < MAX_HC_NUM; i++) {
		sdhci_hosts[i].ioaddr = BASE_ADDR + i * 0x4000;
		sdhci_hw_probe(&sdhci_hosts[i]);
	}

	return 0;
}

/**
 * @file
 * @brief i.MX6 uSDHC driver for MMC card
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 01.05.2017
 */
#include <util/log.h>

#include <string.h>

#include <drivers/block_dev.h>
#include <drivers/common/memory.h>
#include <hal/reg.h>

#include <framework/mod/options.h>

#define BASE_ADDR OPTION_GET(NUMBER, base_addr)

#define DWMCI_CTRL		(BASE_ADDR + 0x000)
#define	DWMCI_PWREN		(BASE_ADDR + 0x004)
#define DWMCI_CLKDIV		(BASE_ADDR + 0x008)
#define DWMCI_CLKSRC		(BASE_ADDR + 0x00C)
#define DWMCI_CLKENA		(BASE_ADDR + 0x010)
#define DWMCI_TMOUT		(BASE_ADDR + 0x014)
#define DWMCI_CTYPE		(BASE_ADDR + 0x018)
#define DWMCI_BLKSIZ		(BASE_ADDR + 0x01C)
#define DWMCI_BYTCNT		(BASE_ADDR + 0x020)
#define DWMCI_INTMASK		(BASE_ADDR + 0x024)
#define DWMCI_CMDARG		(BASE_ADDR + 0x028)
#define DWMCI_CMD		(BASE_ADDR + 0x02C)
#define DWMCI_RESP0		(BASE_ADDR + 0x030)
#define DWMCI_RESP1		(BASE_ADDR + 0x034)
#define DWMCI_RESP2		(BASE_ADDR + 0x038)
#define DWMCI_RESP3		(BASE_ADDR + 0x03C)
#define DWMCI_MINTSTS		(BASE_ADDR + 0x040)
#define DWMCI_RINTSTS		(BASE_ADDR + 0x044)
#define DWMCI_STATUS		(BASE_ADDR + 0x048)
#define DWMCI_FIFOTH		(BASE_ADDR + 0x04C)
#define DWMCI_CDETECT		(BASE_ADDR + 0x050)
#define DWMCI_WRTPRT		(BASE_ADDR + 0x054)
#define DWMCI_GPIO		(BASE_ADDR + 0x058)
#define DWMCI_TCMCNT		(BASE_ADDR + 0x05C)
#define DWMCI_TBBCNT		(BASE_ADDR + 0x060)
#define DWMCI_DEBNCE		(BASE_ADDR + 0x064)
#define DWMCI_USRID		(BASE_ADDR + 0x068)
#define DWMCI_VERID		(BASE_ADDR + 0x06C)
#define DWMCI_HCON		(BASE_ADDR + 0x070)
#define DWMCI_UHS_REG		(BASE_ADDR + 0x074)
#define DWMCI_BMOD		(BASE_ADDR + 0x080)
#define DWMCI_PLDMND		(BASE_ADDR + 0x084)
#define DWMCI_DBADDR		(BASE_ADDR + 0x088)
#define DWMCI_IDSTS		(BASE_ADDR + 0x08C)
#define DWMCI_IDINTEN		(BASE_ADDR + 0x090)
#define DWMCI_DSCADDR		(BASE_ADDR + 0x094)
#define DWMCI_BUFADDR		(BASE_ADDR + 0x098)
#define DWMCI_DATA		(BASE_ADDR + 0x200)

/* Interrupt Mask register */
#define DWMCI_INTMSK_ALL	0xffffffff
#define DWMCI_INTMSK_RE		(1 << 1)
#define DWMCI_INTMSK_CDONE	(1 << 2)
#define DWMCI_INTMSK_DTO	(1 << 3)
#define DWMCI_INTMSK_TXDR	(1 << 4)
#define DWMCI_INTMSK_RXDR	(1 << 5)
#define DWMCI_INTMSK_DCRC	(1 << 7)
#define DWMCI_INTMSK_RTO	(1 << 8)
#define DWMCI_INTMSK_DRTO	(1 << 9)
#define DWMCI_INTMSK_HTO	(1 << 10)
#define DWMCI_INTMSK_FRUN	(1 << 11)
#define DWMCI_INTMSK_HLE	(1 << 12)
#define DWMCI_INTMSK_SBE	(1 << 13)
#define DWMCI_INTMSK_ACD	(1 << 14)
#define DWMCI_INTMSK_EBE	(1 << 15)

/* Raw interrupt Regsiter */
#define DWMCI_DATA_ERR	(DWMCI_INTMSK_EBE | DWMCI_INTMSK_SBE | DWMCI_INTMSK_HLE |\
			DWMCI_INTMSK_FRUN | DWMCI_INTMSK_EBE | DWMCI_INTMSK_DCRC)
#define DWMCI_DATA_TOUT	(DWMCI_INTMSK_HTO | DWMCI_INTMSK_DRTO)
/* CTRL register */
#define DWMCI_CTRL_RESET	(1 << 0)
#define DWMCI_CTRL_FIFO_RESET	(1 << 1)
#define DWMCI_CTRL_DMA_RESET	(1 << 2)
#define DWMCI_DMA_EN		(1 << 5)
#define DWMCI_CTRL_SEND_AS_CCSD	(1 << 10)
#define DWMCI_IDMAC_EN		(1 << 25)
#define DWMCI_RESET_ALL		(DWMCI_CTRL_RESET | DWMCI_CTRL_FIFO_RESET |\
				DWMCI_CTRL_DMA_RESET)

/* CMD register */
#define DWMCI_CMD_RESP_EXP	(1 << 6)
#define DWMCI_CMD_RESP_LENGTH	(1 << 7)
#define DWMCI_CMD_CHECK_CRC	(1 << 8)
#define DWMCI_CMD_DATA_EXP	(1 << 9)
#define DWMCI_CMD_RW		(1 << 10)
#define DWMCI_CMD_SEND_STOP	(1 << 12)
#define DWMCI_CMD_ABORT_STOP	(1 << 14)
#define DWMCI_CMD_PRV_DAT_WAIT	(1 << 13)
#define DWMCI_CMD_UPD_CLK	(1 << 21)
#define DWMCI_CMD_USE_HOLD_REG	(1 << 29)
#define DWMCI_CMD_START		(1 << 31)

/* CLKENA register */
#define DWMCI_CLKEN_ENABLE	(1 << 0)
#define DWMCI_CLKEN_LOW_PWR	(1 << 16)

/* Card-type registe */
#define DWMCI_CTYPE_1BIT	0
#define DWMCI_CTYPE_4BIT	(1 << 0)
#define DWMCI_CTYPE_8BIT	(1 << 16)

/* Status Register */
#define DWMCI_BUSY		(1 << 9)
#define DWMCI_FIFO_MASK		0x1fff
#define DWMCI_FIFO_SHIFT	17

/* FIFOTH Register */
#define MSIZE(x)		((x) << 28)
#define RX_WMARK(x)		((x) << 16)
#define TX_WMARK(x)		(x)
#define RX_WMARK_SHIFT		16
#define RX_WMARK_MASK		(0xfff << RX_WMARK_SHIFT)

/* HCON Register */
#define DMA_INTERFACE_IDMA		(0x0)
#define SDMMC_GET_TRANS_MODE(x)		(((x)>>16) & 0x3)

#define DWMCI_IDMAC_OWN		(1 << 31)
#define DWMCI_IDMAC_CH		(1 << 4)
#define DWMCI_IDMAC_FS		(1 << 3)
#define DWMCI_IDMAC_LD		(1 << 2)

/*  Bus Mode Register */
#define DWMCI_BMOD_IDMAC_RESET	(1 << 0)
#define DWMCI_BMOD_IDMAC_FB	(1 << 1)
#define DWMCI_BMOD_IDMAC_EN	(1 << 7)

/* UHS register */
#define DWMCI_DDR_MODE	(1 << 16)

/* quirks */
#define DWMCI_QUIRK_DISABLE_SMU		(1 << 0)

/*
 * DWMCI_MSIZE is uses to set burst size of multiple transaction.
 * The burst size is set to 128 if DWMCI_MSIZE is set to 0x6.
 */
#define DWMCI_MSIZE    0x6
#define BLK_LEN 512

#define TIMEOUT 0x1FFFFF

#define SDHCI_IRQ_EN_BITS (1 | 2 | 0x100 | 0x10000 | 0x20000 | 0x40000 | 0x80000 | 0x100000 | 0x400000 | 0x20 | 0x10 | 0x8)

#define DW_MMC_FIFO_SZ 0x400

static void dw_mmc_reset(uint32_t flags) {
	int timeout = TIMEOUT;

	REG32_ORIN(DWMCI_CTRL, flags);

	while (timeout--);

	if (REG32_LOAD(DWMCI_CTRL) & flags) {
		log_error("Reset timeout");
	}
}

static void _reg_dump(void) {
	log_debug("===================");
	log_debug("REG DUMP");
	log_debug("===================");
	log_debug("DWMCI_CTRL      =0x%08x", REG32_LOAD(DWMCI_CTRL));
        log_debug("DWMCI_PWREN     =0x%08x", REG32_LOAD(DWMCI_PWREN));
        log_debug("DWMCI_CLKDIV    =0x%08x", REG32_LOAD(DWMCI_CLKDIV));
        log_debug("DWMCI_CLKSRC    =0x%08x", REG32_LOAD(DWMCI_CLKSRC));
        log_debug("DWMCI_CLKENA    =0x%08x", REG32_LOAD(DWMCI_CLKENA));
        log_debug("DWMCI_TMOUT     =0x%08x", REG32_LOAD(DWMCI_TMOUT));
        log_debug("DWMCI_CTYPE     =0x%08x", REG32_LOAD(DWMCI_CTYPE));
        log_debug("DWMCI_BLKSIZ    =0x%08x", REG32_LOAD(DWMCI_BLKSIZ));
        log_debug("DWMCI_BYTCNT    =0x%08x", REG32_LOAD(DWMCI_BYTCNT));
        log_debug("DWMCI_INTMASK   =0x%08x", REG32_LOAD(DWMCI_INTMASK));
        log_debug("DWMCI_CMDARG    =0x%08x", REG32_LOAD(DWMCI_CMDARG));
        log_debug("DWMCI_CMD       =0x%08x", REG32_LOAD(DWMCI_CMD));
        log_debug("DWMCI_RESP0     =0x%08x", REG32_LOAD(DWMCI_RESP0));
        log_debug("DWMCI_RESP1     =0x%08x", REG32_LOAD(DWMCI_RESP1));
        log_debug("DWMCI_RESP2     =0x%08x", REG32_LOAD(DWMCI_RESP2));
        log_debug("DWMCI_RESP3     =0x%08x", REG32_LOAD(DWMCI_RESP3));
        log_debug("DWMCI_MINTSTS   =0x%08x", REG32_LOAD(DWMCI_MINTSTS));
        log_debug("DWMCI_RINTSTS   =0x%08x", REG32_LOAD(DWMCI_RINTSTS));
        log_debug("DWMCI_STATUS    =0x%08x", REG32_LOAD(DWMCI_STATUS));
        log_debug("DWMCI_FIFOTH    =0x%08x", REG32_LOAD(DWMCI_FIFOTH));
        log_debug("DWMCI_CDETECT   =0x%08x", REG32_LOAD(DWMCI_CDETECT));
        log_debug("DWMCI_WRTPRT    =0x%08x", REG32_LOAD(DWMCI_WRTPRT));
        log_debug("DWMCI_GPIO      =0x%08x", REG32_LOAD(DWMCI_GPIO));
        log_debug("DWMCI_TCMCNT    =0x%08x", REG32_LOAD(DWMCI_TCMCNT));
        log_debug("DWMCI_TBBCNT    =0x%08x", REG32_LOAD(DWMCI_TBBCNT));
        log_debug("DWMCI_DEBNCE    =0x%08x", REG32_LOAD(DWMCI_DEBNCE));
        log_debug("DWMCI_USRID     =0x%08x", REG32_LOAD(DWMCI_USRID));
        log_debug("DWMCI_VERID     =0x%08x", REG32_LOAD(DWMCI_VERID));
        log_debug("DWMCI_HCON      =0x%08x", REG32_LOAD(DWMCI_HCON));
        log_debug("DWMCI_UHS_REG   =0x%08x", REG32_LOAD(DWMCI_UHS_REG));
        log_debug("DWMCI_BMOD      =0x%08x", REG32_LOAD(DWMCI_BMOD));
        log_debug("DWMCI_PLDMND    =0x%08x", REG32_LOAD(DWMCI_PLDMND));
        log_debug("DWMCI_DBADDR    =0x%08x", REG32_LOAD(DWMCI_DBADDR));
        log_debug("DWMCI_IDSTS     =0x%08x", REG32_LOAD(DWMCI_IDSTS));
        log_debug("DWMCI_IDINTEN   =0x%08x", REG32_LOAD(DWMCI_IDINTEN));
        log_debug("DWMCI_DSCADDR   =0x%08x", REG32_LOAD(DWMCI_DSCADDR));
        log_debug("DWMCI_BUFADDR   =0x%08x", REG32_LOAD(DWMCI_BUFADDR));
        log_debug("DWMCI_DATA      =0x%08x", REG32_LOAD(DWMCI_DATA));
	log_debug("===================");
}

static void _resp_dump(void) {
	log_debug("===================");
	log_debug("RESP DUMP");
	log_debug("===================");
        log_debug("DWMCI_CMD       =0x%08x", REG32_LOAD(DWMCI_CMD));
        log_debug("DWMCI_RESP0     =0x%08x", REG32_LOAD(DWMCI_RESP0));
        log_debug("DWMCI_RESP1     =0x%08x", REG32_LOAD(DWMCI_RESP1));
        log_debug("DWMCI_RESP2     =0x%08x", REG32_LOAD(DWMCI_RESP2));
        log_debug("DWMCI_RESP3     =0x%08x", REG32_LOAD(DWMCI_RESP3));
	log_debug("===================");
}
static void dw_mmc_send_cmd(int cmd_idx, uint32_t arg, uint8_t *data, size_t len) {
	uint32_t wcmd = 0;
	int retry = 0xfffff;

	while (REG32_LOAD(DWMCI_STATUS) & DWMCI_BUSY) {
		retry--;
		if (retry == 0) {
			log_error("DW MMC is busy");
			return;
		}
	}

	REG32_STORE(DWMCI_RINTSTS, DWMCI_INTMSK_ALL);

	if (data != NULL) {
		REG32_STORE(DWMCI_BLKSIZ, BLK_LEN);
		REG32_STORE(DWMCI_BYTCNT, len);
		dw_mmc_reset(DWMCI_CTRL_FIFO_RESET);
	}

	REG32_STORE(DWMCI_CMDARG, arg);

	if (data != NULL) {
		wcmd |= DWMCI_CMD_DATA_EXP;
	}

	if (cmd_idx == 12) {
		wcmd |= DWMCI_CMD_ABORT_STOP;
	} else {
		wcmd |= DWMCI_CMD_PRV_DAT_WAIT;
	}

	wcmd |= cmd_idx | DWMCI_CMD_START | DWMCI_CMD_USE_HOLD_REG;
	log_debug("send cmd: 0x%08x; cmd_idx=0x%08x; arg=0x%08x\n", wcmd, cmd_idx, arg);

	REG32_STORE(DWMCI_CMD, wcmd);

	/* Wait command to be completed */
	while (true) {
		uint32_t mask = REG32_LOAD(DWMCI_RINTSTS);
		if (mask & DWMCI_INTMSK_CDONE) {
			log_debug("cmd done");
			break;
		}
	}
	_resp_dump();
}

#define MAX_STRIDE 64
static int dw_mmc_read(struct block_dev *bdev, char *buffer, size_t count,
		blkno_t blkno) {
	log_debug("sdhc read, addr=0x%x, count=%d", blkno * bdev->block_size, count);

	memset(buffer, 0, count);

	/* Set block length */
	dw_mmc_send_cmd(16, 512, NULL, 0);

	/* Do actual read operation */
	dw_mmc_send_cmd(17, blkno * BLK_LEN, (uint8_t *) buffer, count * BLK_LEN);

	int size = count * BLK_LEN, len;
	int i;

	while (size > 0) {
		uint32_t mask = REG32_LOAD(DWMCI_RINTSTS);
		/* Error during data transfer. */
		if (mask & (DWMCI_DATA_ERR | DWMCI_DATA_TOUT)) {
			log_debug("Data error!");
			while (1);
		}

		len = REG32_LOAD(DWMCI_STATUS);
		len = (len >> DWMCI_FIFO_SHIFT) & DWMCI_FIFO_MASK;

		for (i = 0; i < len / MAX_STRIDE; i++) {
			memcpy(buffer, (void *) DWMCI_DATA, 64);
			buffer += MAX_STRIDE;
		}

		len = i * MAX_STRIDE;
		size -= len;
	}

	REG32_STORE(DWMCI_RINTSTS, DWMCI_INTMSK_RXDR);

	return count;
}

static int dw_mmc_write(struct block_dev *bdev, char *buffer, size_t count,
		blkno_t blkno) {
	log_debug("sdhc write, addr=0x%x, count=%d", blkno * bdev->block_size, count);

	//REG32_STORE(USDHC_INT_STATUS, -1);

	//REG32_STORE(USDHC_DS_ADDR, (uint32_t) buffer);

	dw_mmc_send_cmd(24, blkno * BLK_LEN, NULL, 0);

	return count;
}

static int dw_mmc_probe(void *args);

struct block_dev_driver dw_mmc_driver = {
	.name  = "dw_mmc_driver",
	.probe = dw_mmc_probe,
	.read  = dw_mmc_read,
	.write = dw_mmc_write
};

BLOCK_DEV_DEF("dw_mmc", &dw_mmc_driver);

PERIPH_MEMORY_DEFINE(dw_mmc, BASE_ADDR, 0x4000);

static uint32_t sdhc_get_size() {
	uint32_t t;
	dw_mmc_send_cmd(10 | DWMCI_CMD_RESP_EXP, 0, NULL, 0);
	dw_mmc_send_cmd(9 | DWMCI_CMD_RESP_EXP, 0, NULL, 0);
	t = (REG32_LOAD(DWMCI_RESP1) >> 6) & 0x3;
	t |= (REG32_LOAD(DWMCI_RESP1) >> 24) << 2;
	t |= (REG32_LOAD(DWMCI_RESP2) & 0x3) << 10;

	return 256 * 1024 * (t + 1);
}

static int dwmci_setup_bus(int freq) {
	uint32_t div = 1, status;
	int timeout = 10000;

	if (freq == 0)
		return 0;

	REG32_STORE(DWMCI_CLKENA, 0);
	REG32_STORE(DWMCI_CLKSRC, 0);

	REG32_STORE(DWMCI_CLKDIV, div);
	REG32_STORE(DWMCI_CMD, DWMCI_CMD_PRV_DAT_WAIT |
			DWMCI_CMD_UPD_CLK | DWMCI_CMD_START);

	do {
		status = REG32_LOAD(DWMCI_CMD);
		if (timeout-- < 0) {
			log_error("%s: Timeout!", __func__);
			return -1;
		}
	} while (status & DWMCI_CMD_START);

	REG32_STORE(DWMCI_CLKENA, DWMCI_CLKEN_ENABLE |
			DWMCI_CLKEN_LOW_PWR);

	REG32_STORE(DWMCI_CMD, DWMCI_CMD_PRV_DAT_WAIT |
			DWMCI_CMD_UPD_CLK | DWMCI_CMD_START);

	timeout = 10000;
	do {
		status = REG32_LOAD(DWMCI_CMD);
		if (timeout-- < 0) {
			log_error("%s: Timeout!", __func__);
			return -1;
		}
	} while (status & DWMCI_CMD_START);

	return 0;
}

int clk_enable(char *clk_name);
static int dw_mmc_probe(void *args) {
	struct block_dev *bdev;

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
	_reg_dump();
	_resp_dump();
	dw_mmc_send_cmd(7, 0, NULL, 0);
	dw_mmc_send_cmd(9 | DWMCI_CMD_RESP_EXP, 0, NULL, 0);
	_resp_dump();
	log_debug("DW MMC probe");

	bdev = block_dev_create("sdhc", &dw_mmc_driver, NULL);
	bdev->privdata = NULL;
	bdev->block_size = BLK_LEN;

	/* HW initialization */
	REG32_STORE(DWMCI_PWREN, 0);
	REG32_STORE(DWMCI_PWREN, 1);
	REG32_ANDIN(DWMCI_CTRL, ~(DWMCI_DMA_EN | DWMCI_IDMAC_EN));

	dw_mmc_reset(DWMCI_RESET_ALL);
if (1) {

	dwmci_setup_bus(1000);

}
	REG32_STORE(DWMCI_RINTSTS, 0xFFFFFFFF);
	REG32_STORE(DWMCI_INTMASK, 0);

	REG32_STORE(DWMCI_TMOUT, 0xFFFFFFFF);

	REG32_STORE(DWMCI_IDINTEN, 0);
	REG32_STORE(DWMCI_BMOD, 1);

	REG32_STORE(DWMCI_FIFOTH, DW_MMC_FIFO_SZ);

	_reg_dump();
	/* SD initialization */
	dw_mmc_send_cmd(0, 0, NULL, 0);
	dw_mmc_send_cmd(1, 0, NULL, 0);
	dw_mmc_send_cmd(2, 0, NULL, 0);
	dw_mmc_send_cmd(3, 0, NULL, 0);
	/*
	dw_mmc_send_cmd(8, 0x1AA, NULL, 0);
	dw_mmc_send_cmd(55, 0x0, NULL, 0);
	dw_mmc_send_cmd(41, 0x4000FFFF, NULL, 0);

	dw_mmc_send_cmd(2, 0, NULL, 0);
	dw_mmc_send_cmd(3, 0, NULL, 0); */

	bdev->size = sdhc_get_size();
	log_debug("bdev size is %d\n", bdev->size);
	dw_mmc_send_cmd(7, 0x4567 << 16, NULL, 0);

	_reg_dump();

	return 0;
}

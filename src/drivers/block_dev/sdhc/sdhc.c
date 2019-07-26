/**
 * @file imx6_usdhc.c
 * @brief i.MX6 uSDHC driver for MMC card
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 01.05.2017
 */

#include <drivers/block_dev.h>
#include <drivers/common/memory.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <string.h>
#include <util/log.h>

#define BASE_ADDR OPTION_GET(NUMBER, base_addr)

#define USDHC_DS_ADDR               (BASE_ADDR + 0x00)
#define USDHC_BLK_ATT               (BASE_ADDR + 0x04)
#define USDHC_CMD_ARG               (BASE_ADDR + 0x08)
#define USDHC_CMD_XFR_TYP           (BASE_ADDR + 0x0C)
#define USDHC_CMD_RSP0              (BASE_ADDR + 0x10)
#define USDHC_CMD_RSP1              (BASE_ADDR + 0x14)
#define USDHC_CMD_RSP2              (BASE_ADDR + 0x18)
#define USDHC_CMD_RSP3              (BASE_ADDR + 0x1C)
#define USDHC_DATA_BUFF_ACC_PORT    (BASE_ADDR + 0x20)
#define USDHC_PRES_STATE            (BASE_ADDR + 0x24)
#define USDHC_PROT_CTRL             (BASE_ADDR + 0x28)
#define USDHC_SYS_CTRL              (BASE_ADDR + 0x2C)
# define USDHC_SYS_CTRL_RSTA        (1 << 24)
#define USDHC_INT_STATUS            (BASE_ADDR + 0x30)
#define USDHC_INT_STATUS_EN         (BASE_ADDR + 0x34)
#define USDHC_INT_SIGNAL_EN         (BASE_ADDR + 0x38)
#define USDHC_AUTOCMD12_ERR_STATUS  (BASE_ADDR + 0x3C)
#define USDHC_HOST_CTRL_CAP         (BASE_ADDR + 0x40)
#define USDHC_WTMK_LVL              (BASE_ADDR + 0x44)
#define USDHC_MIX_CTRL              (BASE_ADDR + 0x48)
#define USDHC_FORCE_EVENT           (BASE_ADDR + 0x50)
#define USDHC_ADMA_ERR_STATUS       (BASE_ADDR + 0x54)
#define USDHC_ADMA_SYS_ADDR         (BASE_ADDR + 0x58)
#define USDHC_DLL_CTRl              (BASE_ADDR + 0x60)
#define USDHC_DLL_STATUS            (BASE_ADDR + 0x64)
#define USDHC_CLK_TUNE_CTRL_STATUS  (BASE_ADDR + 0x68)
#define USDHC_VEND_SPEC             (BASE_ADDR + 0xC0)
#define USDHC_MMC_BOOT              (BASE_ADDR + 0xC4)
#define USDHC_VEND_SPEC2            (BASE_ADDR + 0xC8)

#define BLK_LEN 512

#define TIMEOUT 0x1FFFFF

#define SDHCI_IRQ_EN_BITS (1 | 2 | 0x100 | 0x10000 | 0x20000 | 0x40000 | 0x80000 | 0x100000 | 0x400000 | 0x20 | 0x10 | 0x8)

static void _reg_dump(void) {
	log_debug("===================");
	log_debug("REG DUMP");
	log_debug("===================");
	log_debug("USDHC_DS_ADDR              =0x%08x", REG32_LOAD(BASE_ADDR + 0x00));
	log_debug("USDHC_BLK_ATT              =0x%08x", REG32_LOAD(BASE_ADDR + 0x04));
	log_debug("USDHC_CMD_ARG              =0x%08x", REG32_LOAD(BASE_ADDR + 0x08));
	log_debug("USDHC_CMD_XFR_TYP          =0x%08x", REG32_LOAD(BASE_ADDR + 0x0C));
	log_debug("USDHC_CMD_RSP0             =0x%08x", REG32_LOAD(BASE_ADDR + 0x10));
	log_debug("USDHC_CMD_RSP1             =0x%08x", REG32_LOAD(BASE_ADDR + 0x14));
	log_debug("USDHC_CMD_RSP2             =0x%08x", REG32_LOAD(BASE_ADDR + 0x18));
	log_debug("USDHC_CMD_RSP3             =0x%08x", REG32_LOAD(BASE_ADDR + 0x1C));
	log_debug("USDHC_DATA_BUFF_ACC_PORT   =0x%08x", REG32_LOAD(BASE_ADDR + 0x20));
	log_debug("USDHC_PRES_STATE           =0x%08x", REG32_LOAD(BASE_ADDR + 0x24));
	log_debug("USDHC_PROT_CTRL            =0x%08x", REG32_LOAD(BASE_ADDR + 0x28));
	log_debug("USDHC_SYS_CTRL             =0x%08x", REG32_LOAD(BASE_ADDR + 0x2C));
	log_debug("USDHC_INT_STATUS           =0x%08x", REG32_LOAD(BASE_ADDR + 0x30));
	log_debug("USDHC_INT_STATUS_EN        =0x%08x", REG32_LOAD(BASE_ADDR + 0x34));
	log_debug("USDHC_INT_SIGNAL_EN        =0x%08x", REG32_LOAD(BASE_ADDR + 0x38));
	log_debug("USDHC_AUTOCMD12_ERR_STATUS =0x%08x", REG32_LOAD(BASE_ADDR + 0x3C));
	log_debug("USDHC_HOST_CTRL_CAP        =0x%08x", REG32_LOAD(BASE_ADDR + 0x40));
	log_debug("USDHC_WTMK_LVL             =0x%08x", REG32_LOAD(BASE_ADDR + 0x44));
	log_debug("USDHC_MIX_CTRL             =0x%08x", REG32_LOAD(BASE_ADDR + 0x48));
	log_debug("USDHC_FORCE_EVENT          =0x%08x", REG32_LOAD(BASE_ADDR + 0x50));
	log_debug("USDHC_ADMA_ERR_STATUS      =0x%08x", REG32_LOAD(BASE_ADDR + 0x54));
	log_debug("USDHC_ADMA_SYS_ADDR        =0x%08x", REG32_LOAD(BASE_ADDR + 0x58));
	log_debug("USDHC_DLL_CTRl             =0x%08x", REG32_LOAD(BASE_ADDR + 0x60));
	log_debug("USDHC_DLL_STATUS           =0x%08x", REG32_LOAD(BASE_ADDR + 0x64));
	log_debug("USDHC_CLK_TUNE_CTRL_STATUS =0x%08x", REG32_LOAD(BASE_ADDR + 0x68));
	log_debug("USDHC_VEND_SPEC            =0x%08x", REG32_LOAD(BASE_ADDR + 0xC0));
	log_debug("USDHC_MMC_BOOT             =0x%08x", REG32_LOAD(BASE_ADDR + 0xC4));
	log_debug("USDHC_VEND_SPEC2           =0x%08x", REG32_LOAD(BASE_ADDR + 0xC8));
}

static void imx6_usdhc_send_cmd(int cmd_idx, uint32_t arg) {
	uint32_t wcmd;

	wcmd = (cmd_idx & 0x3f) << 24 | (1 << 20) | (1 << 21) | (cmd_idx == 17 ? (0x10) : 0) | 1 | (cmd_idx == 9 ? 0x3 << 16: 0);

	REG32_STORE(USDHC_CMD_ARG, arg);

	log_debug("send cmd: 0x%08x\n", wcmd);

	REG32_STORE(USDHC_CMD_XFR_TYP, wcmd);

	/* Wait command to be completed */
	while(!(REG32_LOAD(USDHC_INT_STATUS) & 0x1)) {
	}

	REG32_STORE(USDHC_INT_STATUS, 0x1);
}

static void imx6_usdhc_reset(void) {
	REG32_STORE(USDHC_SYS_CTRL, REG32_LOAD(USDHC_SYS_CTRL) | USDHC_SYS_CTRL_RSTA);

	int timeout = TIMEOUT;
	while(timeout--);

	if (REG32_LOAD(USDHC_SYS_CTRL) & USDHC_SYS_CTRL_RSTA) {
		log_error("Reset timeout");
	}

	REG32_STORE(USDHC_VEND_SPEC, REG32_LOAD(USDHC_VEND_SPEC) | (1 << 31) | (1 << 14) | (1 << 13));
}

static void imx6_usdhc_set_block_len(size_t len) {
	uint32_t tmp;
	imx6_usdhc_send_cmd(16, len);

	tmp = len | (1 << 16);
	REG32_STORE(USDHC_BLK_ATT, tmp);
}

static int imx6_usdhc_read(struct block_dev *bdev, char *buffer, size_t count,
		blkno_t blkno) {
	int timeout = TIMEOUT;

	log_debug("sdhc read, addr=0x%x, count=%d", blkno * bdev->block_size, count);

	memset(buffer, 0, count);

	REG32_STORE(USDHC_INT_STATUS, -1);

	REG32_STORE(USDHC_DS_ADDR, (uint32_t) buffer);

	imx6_usdhc_send_cmd(17, blkno * BLK_LEN);

	while(!(REG32_LOAD(USDHC_INT_STATUS) & 1) && timeout--);
	if (timeout == 0)
		log_debug("read timeout");

	return count;
}

static int imx6_usdhc_write(struct block_dev *bdev, char *buffer, size_t count,
		blkno_t blkno) {
	log_debug("sdhc write, addr=0x%x, count=%d", blkno * bdev->block_size, count);

	REG32_STORE(USDHC_INT_STATUS, -1);

	REG32_STORE(USDHC_DS_ADDR, (uint32_t) buffer);

	imx6_usdhc_send_cmd(24, blkno * BLK_LEN);

	return count;
}

static int imx6_usdhc_probe(void *args);

struct block_dev_driver imx6_usdhc_driver = {
	.name  = "imx6_usdhc_driver",
	.probe = imx6_usdhc_probe,
	.read  = imx6_usdhc_read,
	.write = imx6_usdhc_write
};

BLOCK_DEV_DEF("imx6_usdhc", &imx6_usdhc_driver);

PERIPH_MEMORY_DEFINE(imx6_usdhc, BASE_ADDR, 0x4000);

static uint32_t sdhc_get_size() {
	uint32_t t;
	/* Assume it's not high-capacity SD card */
	imx6_usdhc_send_cmd(9, 0x4567 << 16);
	t = (REG32_LOAD(USDHC_CMD_RSP1) >> 6) & 0x3;
	t |= (REG32_LOAD(USDHC_CMD_RSP1) >> 24) << 2;
	t |= (REG32_LOAD(USDHC_CMD_RSP2) & 0x3) << 10;

	return 256 * 1024 * (t + 1);
}

static int imx6_usdhc_probe(void *args) {
	struct block_dev *bdev;

	log_debug("SDHC probe");

	bdev = block_dev_create("sdhc", &imx6_usdhc_driver, NULL);
	bdev->privdata = NULL;
	bdev->block_size = BLK_LEN;

	/* SDHC initialization */
	imx6_usdhc_reset();

	REG32_STORE(USDHC_SYS_CTRL, 0xb011f);
	REG32_STORE(USDHC_INT_STATUS_EN, -1);
	REG32_STORE(USDHC_PROT_CTRL, REG32_LOAD(USDHC_PROT_CTRL) | 1 | 4);
	imx6_usdhc_set_block_len(BLK_LEN);

	/* SD initialization */
	imx6_usdhc_send_cmd(0, 0);
	imx6_usdhc_send_cmd(8, 0x1AA);
	imx6_usdhc_send_cmd(55, 0x0);
	imx6_usdhc_send_cmd(41, 0x4000FFFF);

	imx6_usdhc_send_cmd(2, 0);
	imx6_usdhc_send_cmd(3, 0);

	bdev->size = sdhc_get_size();
	log_debug("bdev size is %d\n", bdev->size);
	imx6_usdhc_send_cmd(7, 0x4567 << 16);

	_reg_dump();

	return 0;
}

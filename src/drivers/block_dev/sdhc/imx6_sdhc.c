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
#include <net/util/show_packet.h>
#include <util/log.h>
#include <kernel/printk.h>
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
# define USDHC_SYS_CTRL_RSTC        (1 << 25)
# define USDHC_SYS_CTRL_RSTD        (1 << 26)
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
#define USDHC_DLL_CTRL              (BASE_ADDR + 0x60)
#define USDHC_DLL_STATUS            (BASE_ADDR + 0x64)
#define USDHC_CLK_TUNE_CTRL_STATUS  (BASE_ADDR + 0x68)
#define USDHC_VEND_SPEC             (BASE_ADDR + 0xC0)
#define USDHC_MMC_BOOT              (BASE_ADDR + 0xC4)
#define USDHC_VEND_SPEC2            (BASE_ADDR + 0xC8)

#define BLK_LEN 512

#define TIMEOUT 0x1FFFFF

#define CMD8_CHECK_PATTERN		0x0AA
#define CMD8_STANDART_VOLTAGE		0x100 /* 2.7-3.6V */

#define SDHCI_IRQ_EN_BITS (1 | 2 | 0x100 | 0x10000 | 0x20000 | 0x40000 | 0x80000 | 0x100000 | 0x400000 | 0x20 | 0x10 | 0x8)

static int imx6_usdhc_send_cmd(int cmd_idx, uint32_t arg);
static void imx6_usdhc_send_app_cmd(int cmd_idx, uint32_t arg, uint32_t rca);

static void _resp_dump(void) {
	log_debug("===================");
	log_debug("USDHC_CMD_RSP0             =0x%08x", REG32_LOAD(BASE_ADDR + 0x10));
	log_debug("USDHC_CMD_RSP1             =0x%08x", REG32_LOAD(BASE_ADDR + 0x14));
	log_debug("USDHC_CMD_RSP2             =0x%08x", REG32_LOAD(BASE_ADDR + 0x18));
	log_debug("USDHC_CMD_RSP3             =0x%08x", REG32_LOAD(BASE_ADDR + 0x1C));
	log_debug("===================");
}

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
	log_debug("USDHC_DLL_CTRL             =0x%08x", REG32_LOAD(BASE_ADDR + 0x60));
	log_debug("USDHC_DLL_STATUS           =0x%08x", REG32_LOAD(BASE_ADDR + 0x64));
	log_debug("USDHC_CLK_TUNE_CTRL_STATUS =0x%08x", REG32_LOAD(BASE_ADDR + 0x68));
	log_debug("USDHC_VEND_SPEC            =0x%08x", REG32_LOAD(BASE_ADDR + 0xC0));
	log_debug("USDHC_MMC_BOOT             =0x%08x", REG32_LOAD(BASE_ADDR + 0xC4));
	log_debug("USDHC_VEND_SPEC2           =0x%08x", REG32_LOAD(BASE_ADDR + 0xC8));
}

#define RSP_136         (1 << 16)
#define RSP_PRESENT     (2 << 16)
#define RSP_BUSY        (3 << 16)
#define RSP_CRC         (1 << 19)
#define RSP_OPCODE      (1 << 20)

#define RSP_NONE        0
#define RSP_R1          (RSP_PRESENT | RSP_CRC | RSP_OPCODE)
#define RSP_R1b         (RSP_CRC | RSP_OPCODE|RSP_BUSY)
#define RSP_R2          (RSP_136 | RSP_CRC)
#define RSP_R3          (RSP_PRESENT)
#define RSP_R4          (RSP_PRESENT)
#define RSP_R5          (RSP_PRESENT | RSP_CRC | RSP_OPCODE)
#define RSP_R6          (RSP_PRESENT | RSP_CRC | RSP_OPCODE)
#define RSP_R7          (RSP_PRESENT | RSP_CRC | RSP_OPCODE)

#define WCMD_DPSEL      (1 << 21)
#define MIX_CARD2HOST	(1 << 4)
#define MIX_DMAEN       (1 << 0)
static int _hcs = 1; /* High capacity storage */

void imx6_usdhc_reset_data(void) {
	REG32_STORE(USDHC_SYS_CTRL, REG32_LOAD(USDHC_SYS_CTRL) | USDHC_SYS_CTRL_RSTD);

	int timeout = TIMEOUT;
	while(timeout--);

	if (REG32_LOAD(USDHC_SYS_CTRL) & USDHC_SYS_CTRL_RSTD) {
		log_error("Reset timeout");
	}
}

void imx6_usdhc_reset_cmd(void) {
	REG32_STORE(USDHC_SYS_CTRL, REG32_LOAD(USDHC_SYS_CTRL) | USDHC_SYS_CTRL_RSTC);

	int timeout = TIMEOUT;
	while(timeout--);

	if (REG32_LOAD(USDHC_SYS_CTRL) & USDHC_SYS_CTRL_RSTC) {
		log_error("Reset timeout");
	}
}

static int imx6_usdhc_send_cmd(int cmd_idx, uint32_t arg) {
	uint32_t wcmd = 0;
	uint32_t mix = 0;
	int t = TIMEOUT;

	wcmd = ((cmd_idx & 0x3f) << 24);

	switch (cmd_idx) {
	case 41:
		wcmd |= RSP_R3;
		break;
	case 24:
		wcmd |= WCMD_DPSEL | RSP_R1;
		mix  |= MIX_DMAEN;
		break;
	case 17:
		wcmd |= WCMD_DPSEL | RSP_R1;
		mix  |= MIX_DMAEN | MIX_CARD2HOST;
		break;
	case 9:
		wcmd |= RSP_R2;
		break;
	case 55:
	case 8:
	case 6:
		wcmd |= RSP_R1;
		break;
	case 7:
		wcmd |= RSP_R1b;
		break;
	case 3:
		wcmd |= RSP_R6;
		break;
	case 13:
	case 2:
		wcmd |= RSP_R2;
		break;
	case 1:
		wcmd |= RSP_R3;
		break;
	case 0:
		arg = 0;
		wcmd = 0;
		break;
	default:
		break;
	}

	t = TIMEOUT;
	while (--t && REG32_LOAD(USDHC_PRES_STATE) & 0x3);
	if (t == 0) {
		log_error("Bus is not idle!!!");
		return -1;
	}

	t = TIMEOUT;
	while (--t && REG32_LOAD(USDHC_PRES_STATE) & (1 << 2));
		if (t == 0) {
		log_error("Data line is active, can not issue cmd");
		return -1;
	}

	t = TIMEOUT;
	while(t--);

	REG32_STORE(USDHC_CMD_ARG, arg);
	REG32_STORE(USDHC_MIX_CTRL, mix);
	REG32_STORE(USDHC_CMD_XFR_TYP, wcmd);

	log_debug("send cmd: id=%d, arg=0x%08x", cmd_idx, arg);

	/* Wait command to be completed */
	t = TIMEOUT;
	while(!(REG32_LOAD(USDHC_INT_STATUS) & -1) && --t);

	if (cmd_idx == 7) {
		t = TIMEOUT;

		while (t-- && !(REG32_LOAD(USDHC_PRES_STATE) & (1 << 24))) {
		}

		if (t == 0) {
			log_error("DAT0 up timeout");
		}
	}

	if (!t) {
		log_error("Command timeout!\n");
		_reg_dump();
	}

	t = REG32_LOAD(USDHC_INT_STATUS);
	log_debug("INTERRUPT STATUS=0x%08x", t);


	REG32_STORE(USDHC_INT_STATUS, t);
	if (t & 1)
		return 0;
	else
		return -1;
}

static void imx6_usdhc_send_app_cmd(int cmd_idx, uint32_t arg, uint32_t rca) {
	if (0 == imx6_usdhc_send_cmd(55, rca << 16) &&
		(REG32_LOAD(USDHC_CMD_RSP0) == 0x120 /* XXX */)) {
		imx6_usdhc_send_cmd(cmd_idx, arg);
	} else {
		log_error("CMD55 cmd fail! (ACMD%d)", cmd_idx);
		_resp_dump();
		while(1);
	}
}

static void imx6_usdhc_reset(void) {
	_reg_dump();

	imx6_usdhc_reset_data();
	imx6_usdhc_reset_cmd();
	REG32_STORE(USDHC_SYS_CTRL, REG32_LOAD(USDHC_SYS_CTRL) | USDHC_SYS_CTRL_RSTA);

	int timeout = TIMEOUT;
	while(timeout--);

	if (REG32_LOAD(USDHC_SYS_CTRL) & USDHC_SYS_CTRL_RSTA) {
		log_error("Reset timeout");
	}


	REG32_ORIN(USDHC_DLL_CTRL, 0x1);

	timeout = TIMEOUT;
	while (--timeout && !(REG32_LOAD(USDHC_PRES_STATE) & (1 << 3)));
	if (timeout == 0) {
		log_error("SDSTB bit is 0 for too long");
	}

	REG32_CLEAR(USDHC_SYS_CTRL, 0xFF << 8);
	REG32_ORIN(USDHC_SYS_CTRL, 0x08 << 8);
	REG32_CLEAR(USDHC_SYS_CTRL, 0x0F << 4);
	REG32_ORIN(USDHC_SYS_CTRL, 0x00 << 4);

	timeout = TIMEOUT;
	while (--timeout && !(REG32_LOAD(USDHC_PRES_STATE) & (1 << 3)));
	if (timeout == 0) {
		log_error("SDSTB bit is 0 for too long");
	}

	REG32_CLEAR(USDHC_DLL_CTRL, 0x1);

	timeout = TIMEOUT;
	while (--timeout && !(0x3 == (0x3 & REG32_LOAD(USDHC_DLL_STATUS))));

	REG32_ORIN(USDHC_DLL_CTRL, (1 << 2));
	REG32_CLEAR(USDHC_DLL_CTRL, (1 << 2));

	log_info("DLL setup finish");

	REG32_STORE(USDHC_VEND_SPEC, REG32_LOAD(USDHC_VEND_SPEC) | (1 << 31) | (1 << 14) | (1 << 13));
	REG32_STORE(USDHC_VEND_SPEC, 0x20007809);
	_reg_dump();

	timeout = TIMEOUT * 0xf;
	while(timeout--);
}

static void imx6_usdhc_set_block_len(size_t len) {
	uint32_t tmp;
	imx6_usdhc_send_cmd(16, len);

	tmp = len | (1 << 16);
	REG32_STORE(USDHC_BLK_ATT, tmp);
}


extern void dcache_inval(const void *p, size_t size);
extern void dcache_flush(const void *p, size_t size);

static uint8_t _buf[BLK_LEN] __attribute__ ((aligned(128))); /* Workaround for unaligned buffer */
static int imx6_usdhc_read(struct block_dev *bdev, char *buffer, size_t count,
		blkno_t blkno) {
	int timeout = TIMEOUT;
	uint32_t pos;

	log_debug("sdhc read, addr=0x%x, count=%d", blkno * bdev->block_size, count);

	REG32_STORE(USDHC_INT_STATUS, -1);

	REG32_STORE(USDHC_DS_ADDR, (uint32_t) _buf);

	memset(_buf, 0, sizeof(_buf));
	pos = blkno;
	if (!_hcs)
		pos *= BLK_LEN;
	imx6_usdhc_send_cmd(17, pos);

	timeout = TIMEOUT;
	while(REG32_LOAD(USDHC_PRES_STATE) & (1 << 9) && --timeout);

	if (timeout == 0) {
		log_debug("transfer active poll timeout");
	}

	dcache_inval(_buf, sizeof(_buf));

	memcpy(buffer, _buf, sizeof(_buf));

	show_packet((void*)buffer, BLK_LEN, "");

	return count;
}

static int imx6_usdhc_write(struct block_dev *bdev, char *buffer, size_t count,
		blkno_t blkno) {
	log_debug("sdhc write, addr=0x%x, count=%d", blkno * bdev->block_size, count);
	int timeout;
	uint32_t pos;

	memcpy(_buf, buffer, BLK_LEN);
	dcache_flush(_buf, BLK_LEN);

	REG32_STORE(USDHC_DS_ADDR, (uint32_t) _buf);

	pos = blkno;
	if (!_hcs)
		pos *= BLK_LEN;
	imx6_usdhc_send_cmd(24, pos);
	timeout = TIMEOUT;
	while(REG32_LOAD(USDHC_PRES_STATE) & (1 << 9) && --timeout);

	if (timeout == 0) {
		log_debug("transfer active poll timeout");
	}

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

static uint32_t sdhc_get_size(uint32_t rca) {
	uint32_t t;
	/* Assume it's not high-capacity SD card */
	imx6_usdhc_send_cmd(9, rca << 16);
	t = (REG32_LOAD(USDHC_CMD_RSP1) >> 6) & 0x3;
	t |= (REG32_LOAD(USDHC_CMD_RSP1) >> 24) << 2;
	t |= (REG32_LOAD(USDHC_CMD_RSP2) & 0x3) << 10;

	return 256 * 1024 * (t + 1);
}

static int imx6_usdhc_probe(void *args) {
	struct block_dev *bdev;
	uint32_t tmp;

	bdev = block_dev_create("sdhc", &imx6_usdhc_driver, NULL);
	bdev->privdata = NULL;
	bdev->block_size = BLK_LEN;

	/* SDHC initialization */
	imx6_usdhc_reset();

	REG32_STORE(USDHC_INT_STATUS_EN, -1);
	REG32_STORE(USDHC_PROT_CTRL, 0x00000020);
	REG32_ORIN(USDHC_SYS_CTRL, 0xf << 16);

	imx6_usdhc_set_block_len(BLK_LEN);
	/* SD initialization. Refer to SDIO specification for more details */
	/* Go idle state */
	imx6_usdhc_send_cmd(0, 0);
	tmp = 0xFFFFF;
	while(tmp--);
	/* Check supported voltage */
	tmp = (CMD8_STANDART_VOLTAGE) | CMD8_CHECK_PATTERN;
	if (0 == imx6_usdhc_send_cmd(8, tmp) && (REG32_LOAD(USDHC_CMD_RSP0) == tmp)) {
		log_info("SDIO card supports standart voltage (2.7-3.6V)");
	} else {
		log_error("Failed to setup voltage, SD card unavailable");
		_resp_dump();
		return -1;
	}

	do {
		tmp = 0xFFFFF;
		while(tmp--);
		imx6_usdhc_send_app_cmd(41, (_hcs << 30) | 0xff8000, 0);
		_resp_dump();
	} while (!(REG32_LOAD(USDHC_CMD_RSP0) & (1 << 31)));

	tmp = 0xFFFFF;
	while(tmp--);

	imx6_usdhc_send_cmd(2, 0);
	_resp_dump();
	imx6_usdhc_send_cmd(3, 0);
	_resp_dump();

	uint32_t rca = (REG32_LOAD(USDHC_CMD_RSP0) & 0xFFFF0000) >> 16;

	log_debug("MMC RCA=0x%04x", rca);
	bdev->size = sdhc_get_size(rca);
	if (_hcs)
		bdev->size *= 8;
	log_info("SD size %d", bdev->size);

	imx6_usdhc_send_cmd(7, rca << 16);
	_resp_dump();

	_reg_dump();

	return 0;
}

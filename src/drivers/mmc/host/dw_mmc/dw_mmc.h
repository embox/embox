/**
 * @file
 *
 * @date Oct 8, 2019
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_MMC_HOST_DW_MMC_DW_MMC_H_
#define SRC_DRIVERS_MMC_HOST_DW_MMC_DW_MMC_H_

#include <stdint.h>
#include <hal/reg.h>

#define BIT(n) (1 << (n))

#define DW_MMC_240A        0x240a
#define DW_MMC_280A        0x280a

#define SDMMC_CTRL         0x000
#define SDMMC_PWREN        0x004
#define SDMMC_CLKDIV       0x008
#define SDMMC_CLKSRC       0x00c
#define SDMMC_CLKENA       0x010
#define SDMMC_TMOUT        0x014
#define SDMMC_CTYPE        0x018
#define SDMMC_BLKSIZ       0x01c
#define SDMMC_BYTCNT       0x020
#define SDMMC_INTMASK      0x024
#define SDMMC_CMDARG       0x028
#define SDMMC_CMD          0x02c
#define SDMMC_RESP0        0x030
#define SDMMC_RESP1        0x034
#define SDMMC_RESP2        0x038
#define SDMMC_RESP3        0x03c
#define SDMMC_MINTSTS      0x040
#define SDMMC_RINTSTS      0x044
#define SDMMC_STATUS       0x048
#define SDMMC_FIFOTH       0x04c
#define SDMMC_CDETECT      0x050
#define SDMMC_WRTPRT       0x054
#define SDMMC_GPIO         0x058
#define SDMMC_TCBCNT       0x05c
#define SDMMC_TBBCNT       0x060
#define SDMMC_DEBNCE       0x064
#define SDMMC_USRID        0x068
#define SDMMC_VERID        0x06c
#define SDMMC_HCON         0x070
#define SDMMC_UHS_REG      0x074
#define SDMMC_RST_N        0x078
#define SDMMC_BMOD         0x080
#define SDMMC_PLDMND       0x084
#define SDMMC_DBADDR       0x088
#define SDMMC_IDSTS        0x08c
#define SDMMC_IDINTEN      0x090
#define SDMMC_DSCADDR      0x094
#define SDMMC_BUFADDR      0x098
#define SDMMC_CDTHRCTL     0x100
#define SDMMC_DATA(x)      (x)
/*
* Registers to support idmac 64-bit address mode
*/
#define SDMMC_DBADDRL      0x088
#define SDMMC_DBADDRU      0x08c
#define SDMMC_IDSTS64      0x090
#define SDMMC_IDINTEN64    0x094
#define SDMMC_DSCADDRL     0x098
#define SDMMC_DSCADDRU     0x09c
#define SDMMC_BUFADDRL     0x0A0
#define SDMMC_BUFADDRU     0x0A4

/*
 * Data offset is difference according to Version
 * Lower than 2.40a : data register offset is 0x100
 */
#define DATA_OFFSET        0x100
#define DATA_240A_OFFSET   0x200

/* shift bit field */
#define _SBF(f, v)        ((v) << (f))

/* Control register defines */
#define SDMMC_CTRL_USE_IDMAC        BIT(25)
#define SDMMC_CTRL_CEATA_INT_EN     BIT(11)
#define SDMMC_CTRL_SEND_AS_CCSD     BIT(10)
#define SDMMC_CTRL_SEND_CCSD        BIT(9)
#define SDMMC_CTRL_ABRT_READ_DATA   BIT(8)
#define SDMMC_CTRL_SEND_IRQ_RESP    BIT(7)
#define SDMMC_CTRL_READ_WAIT        BIT(6)
#define SDMMC_CTRL_DMA_ENABLE       BIT(5)
#define SDMMC_CTRL_INT_ENABLE       BIT(4)
#define SDMMC_CTRL_DMA_RESET        BIT(2)
#define SDMMC_CTRL_FIFO_RESET       BIT(1)
#define SDMMC_CTRL_RESET            BIT(0)
/* Clock Enable register defines */
#define SDMMC_CLKEN_LOW_PWR         BIT(16)
#define SDMMC_CLKEN_ENABLE          BIT(0)
/* time-out register defines */
#define SDMMC_TMOUT_DATA(n)        _SBF(8, (n))
#define SDMMC_TMOUT_DATA_MSK        0xFFFFFF00
#define SDMMC_TMOUT_RESP(n)        ((n) & 0xFF)
#define SDMMC_TMOUT_RESP_MSK        0xFF
/* card-type register defines */
#define SDMMC_CTYPE_8BIT            BIT(16)
#define SDMMC_CTYPE_4BIT            BIT(0)
#define SDMMC_CTYPE_1BIT            0
/* Interrupt status & mask register defines */
#define SDMMC_INT_SDIO(n)           BIT(16 + (n))
#define SDMMC_INT_EBE               BIT(15)
#define SDMMC_INT_ACD               BIT(14)
#define SDMMC_INT_SBE               BIT(13) /* 2000 */
#define SDMMC_INT_HLE               BIT(12) /* 1000 */
#define SDMMC_INT_FRUN              BIT(11) /* 0800 */
#define SDMMC_INT_HTO               BIT(10) /* 0400 */
#define SDMMC_INT_VOLT_SWITCH       BIT(10) /* overloads bit 10! */
#define SDMMC_INT_DRTO              BIT(9)  /* 0200 */
#define SDMMC_INT_RTO               BIT(8)  /* 0100 */
#define SDMMC_INT_DCRC              BIT(7)  /* 0080 */
#define SDMMC_INT_RCRC              BIT(6)  /* 0040 */
#define SDMMC_INT_RXDR              BIT(5)  /* 0020 */
#define SDMMC_INT_TXDR              BIT(4)  /* 0010 */
#define SDMMC_INT_DATA_OVER         BIT(3)  /* 0008 */
#define SDMMC_INT_CMD_DONE          BIT(2)  /* 0004 */
#define SDMMC_INT_RESP_ERR          BIT(1)  /* 0002 */
#define SDMMC_INT_CD                BIT(0)  /* 0001 */
#define SDMMC_INT_ERROR             0xbfc2
/* Command register defines */
#define SDMMC_CMD_START             BIT(31)
#define SDMMC_CMD_USE_HOLD_REG      BIT(29)
#define SDMMC_CMD_VOLT_SWITCH       BIT(28)
#define SDMMC_CMD_CCS_EXP           BIT(23)
#define SDMMC_CMD_CEATA_RD          BIT(22)
#define SDMMC_CMD_UPD_CLK           BIT(21)
#define SDMMC_CMD_INIT              BIT(15)
#define SDMMC_CMD_STOP              BIT(14)
#define SDMMC_CMD_PRV_DAT_WAIT      BIT(13)
#define SDMMC_CMD_SEND_STOP         BIT(12)
#define SDMMC_CMD_STRM_MODE         BIT(11)
#define SDMMC_CMD_DAT_WR            BIT(10)
#define SDMMC_CMD_DAT_EXP           BIT(9)
#define SDMMC_CMD_RESP_CRC          BIT(8)
#define SDMMC_CMD_RESP_LONG         BIT(7)
#define SDMMC_CMD_RESP_EXP          BIT(6)
#define SDMMC_CMD_INDX(n)          ((n) & 0x1F)
/* Status register defines */
#define SDMMC_GET_FCNT(x)          (((x)>>17) & 0x1FFF)
#define SDMMC_STATUS_DMA_REQ        BIT(31)
#define SDMMC_STATUS_BUSY           BIT(9)
/* FIFOTH register defines */
#define SDMMC_SET_FIFOTH(m, r, t) \
			(((m) & 0x7) << 28 | \
			((r) & 0xFFF) << 16 | \
			((t) & 0xFFF))
/* HCON register defines */
#define DMA_INTERFACE_IDMA          (0x0)
#define DMA_INTERFACE_DWDMA         (0x1)
#define DMA_INTERFACE_GDMA          (0x2)
#define DMA_INTERFACE_NODMA         (0x3)
#define SDMMC_GET_TRANS_MODE(x)    (((x)>>16) & 0x3)
#define SDMMC_GET_SLOT_NUM(x)      ((((x)>>1) & 0x1F) + 1)
#define SDMMC_GET_HDATA_WIDTH(x)   (((x)>>7) & 0x7)
#define SDMMC_GET_ADDR_CONFIG(x)   (((x)>>27) & 0x1)
/* Internal DMAC interrupt defines */
#define SDMMC_IDMAC_INT_AI           BIT(9)
#define SDMMC_IDMAC_INT_NI           BIT(8)
#define SDMMC_IDMAC_INT_CES          BIT(5)
#define SDMMC_IDMAC_INT_DU           BIT(4)
#define SDMMC_IDMAC_INT_FBE          BIT(2)
#define SDMMC_IDMAC_INT_RI           BIT(1)
#define SDMMC_IDMAC_INT_TI           BIT(0)
/* Internal DMAC bus mode bits */
#define SDMMC_IDMAC_ENABLE           BIT(7)
#define SDMMC_IDMAC_FB               BIT(1)
#define SDMMC_IDMAC_SWRESET          BIT(0)
/* H/W reset */
#define SDMMC_RST_HWACTIVE           0x1
/* Version ID register define */
#define SDMMC_GET_VERID(x)          ((x) & 0xFFFF)
/* Card read threshold */
#define SDMMC_SET_THLD(v, x)       (((v) & 0xFFF) << 16 | (x))
#define SDMMC_CARD_WR_THR_EN        BIT(2)
#define SDMMC_CARD_RD_THR_EN        BIT(0)
/* UHS-1 register defines */
#define SDMMC_UHS_18V               BIT(0)
/* All ctrl reset bits */
#define SDMMC_CTRL_ALL_RESET_FLAGS \
	(SDMMC_CTRL_RESET | SDMMC_CTRL_FIFO_RESET | SDMMC_CTRL_DMA_RESET)

#define MAX_MCI_SLOTS 2

enum {
	EVENT_CMD_COMPLETE = 0,
	EVENT_XFER_COMPLETE,
	EVENT_DATA_COMPLETE,
	EVENT_DATA_ERROR,
};

enum {
	TRANS_MODE_PIO = 0,
	TRANS_MODE_IDMAC,
	TRANS_MODE_EDMAC
};

struct mmc_host;
struct dw_mci;
struct dw_mci_slot {
	struct mmc_host *mmc;
	struct dw_mci *host;

	uint32_t ctype;

	struct mmc_request *mrq;
	unsigned int clock;

	unsigned long flags;
#define DW_MMC_CARD_PRESENT	0
#define DW_MMC_CARD_NEED_INIT	1
#define DW_MMC_CARD_NO_LOW_PWR	2
#define DW_MMC_CARD_NO_USE_HOLD 3
	int id;
	int sdio_id;
};

struct mmc_request;
struct mmc_command;
struct mmc_data;
struct dw_mci {
	uintptr_t *regs;
	uintptr_t *fifo_reg;
	int irq;

	int sdio_id0;

	struct mmc_request *mrq;
	struct mmc_command *cmd;
	struct mmc_data    *data;

	unsigned int        prev_blksz;

	uint32_t num_slots;
	uint32_t fifoth_val;
	uint16_t verid;
	struct dw_mci_slot *slot[MAX_MCI_SLOTS];

	/* DMA interface members*/
	int use_dma;
	int dma_64bit_address;

	struct idmac_desc *desc_ring;
	/* For idmac */
	unsigned int ring_size;

	uint32_t cmd_status;
	uint32_t data_status;
	uint32_t stop_cmdr;
	uint32_t dir_status;

	unsigned long pending_events;
	unsigned long completed_events;
};

/* Register access macros */
#define mci_readl(dev, reg) \
	REG32_LOAD((dev)->regs + SDMMC_##reg)

#define mci_writel(dev, reg, value) \
	REG32_STORE((dev)->regs + SDMMC_##reg, (value))

/* 16-bit FIFO access macros */
#define mci_readw(dev, reg) \
	REG16_LOAD((dev)->regs + SDMMC_##reg)

#define mci_writew(dev, reg, value) \
	REG16_STORE((dev)->regs + SDMMC_##reg, (value))

#endif /* SRC_DRIVERS_MMC_HOST_DW_MMC_DW_MMC_H_ */

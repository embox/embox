/**
 * @file socfpga.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 24.01.2020
 */
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <drivers/common/memory.h>
#include <drivers/fpga.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#include <hal/reg.h>
#include <util/log.h>

#define SOCFPGA_L3_REGS             0xFF800000
#define SOCFPGA_L3_REMAP            (SOCFPGA_L3_REGS + 0x00)
#define SOCFPGA_SDRAM_REGS          0xFFC20000
#define SOCFPGA_SDRAM_CONFIG        (SOCFPGA_SDRAM_REGS + 0x5080)
#define SOCFPGA_RESETMGR_REGS       0xFFD05000
#define SOCFPGA_RESETMGR_AXI_BRIDGE (SOCFPGA_RESETMGR_REGS + 0x1C)
#define SOCFPGA_SYSMGR_REGS         0xFFD08000
#define SOCFPGA_SYSMGR_INTF         (SOCFPGA_SYSMGR_REGS + 0x28)

#define SOCFPGA_MGR_STAT            0x00
# define SOCFPGA_MODE_MASK          0x7
# define SOCFPGA_MODE_OFF           0x0
# define SOCFPGA_MODE_RESET         0x1
# define SOCFPGA_MODE_CONFIG        0x2
# define SOCFPGA_MODE_INIT          0x3
# define SOCFPGA_MODE_USER          0x4
# define SOCFPGA_MODE_UNKNOWN       0x5
#define SOCFPGA_MGR_CTRL            0x04
# define SOCFPGA_CTRL_EN            (1 << 0)
# define SOCFPGA_CTRL_NCE           (1 << 1)
# define SOCFPGA_CTRL_NCONFIGPULL   (1 << 2)
# define SOCFPGA_CTRL_CDRATIO_OFFT  6
# define SOCFPGA_CTRL_AXICFEN       (1 << 8)
# define SOCFPGA_CTRL_CFGWDTH       (1 << 9)
#define SOCFPGA_MGR_DCLKCNT         0x08
#define SOCFPGA_MGR_DCLKSTAT        0x0C
# define SOCFPGA_DCLKSTAT_DONE      (1 << 0) /* W1C */
#define SOCFPGA_MGR_GPO             0x10
#define SOCFPGA_MGR_GPI             0x14
#define SOCFPGA_MGR_MISCI           0x18

#define SOCFPGA_MGR_GPIO_INTEN          0x830
#define SOCFPGA_MGR_GPIO_INTMASK        0x834
#define SOCFPGA_MGR_GPIO_INTTYPE_LEVEL  0x838
#define SOCFPGA_MGR_GPIO_INT_POLARITY   0x83c
#define SOCFPGA_MGR_GPIO_INTSTATUS      0x840
#define SOCFPGA_MGR_GPIO_RAW_INTSTATUS  0x848
#define SOCFPGA_MGR_GPIO_PORTA_EOI      0x84c
#define SOCFPGA_MGR_GPIO_EXT_PORTA      0x850
# define SOCFPGA_GPIO_EXT_PORTA_NSP     (1 << 9)
# define SOCFPGA_GPIO_EXT_PORTA_CDP     (1 << 10)
#define SOCFPGA_MGR_GPIO_LS_SYNC        0x860
#define SOCFPGA_MGR_GPIO_VER_ID_CODE    0x86C
#define SOCFPGA_MGR_GPIO_CONFIG_REG2    0x870
#define SOCFPGA_MGR_GPIO_CONFIG_REG1    0x874

#define TIMEOUT 10000

/**
 * @brief Reproduce `bridge_enable_handoff` command from u-boot
 */
static void bridge_enable_handoff(void) {
#if 0
	/* This code reprocudes "bridge_enable_handoff" from u-boot */

	/* Pointer to some u-boot function. TODO: disassemble to figure out
	 * what is being done here */
	static const int (*fpga2sdram_apply)(void) = (void *) 0x3ff7d52c;
	REG32_STORE(SOCFPGA_SYSMGR_INTF, 0);
	fpga2sdram_apply();
	REG32_STORE(SOCFPGA_SDRAM_CONFIG, 0);
	REG32_STORE(SOCFPGA_RESETMGR_AXI_BRIDGE, 0);
	REG32_STORE(SOCFPGA_L3_REMAP, 0x19);
#endif
}

struct socfpga {
	uintptr_t mgr_base;
	uintptr_t mgr_data_addr;
};

static void socfpga_or(struct socfpga *socfpga, int offt, uint32_t val) {
	uint32_t tmp = REG32_LOAD(socfpga->mgr_base + offt);

	REG32_STORE(socfpga->mgr_base + offt, val | tmp);
}

static void socfpga_clear(struct socfpga *socfpga, int offt, uint32_t val) {
	uint32_t tmp = REG32_LOAD(socfpga->mgr_base + offt);

	REG32_STORE(socfpga->mgr_base + offt, tmp & ~val);
}

static uint32_t socfpga_read(struct socfpga *socfpga, int offt) {
	return REG32_LOAD(socfpga->mgr_base + offt);
}

static void socfpga_write(struct socfpga *socfpga, int offt, uint32_t val) {
	REG32_STORE(socfpga->mgr_base + offt, val);
}

static void socfpga_data_write(struct socfpga *socfpga, uint32_t val) {
	REG32_STORE(socfpga->mgr_data_addr, val);
}

static void socfpga_regdump(struct socfpga *socfpga) {
	log_debug(" STAT             %08x",
			socfpga_read(socfpga, SOCFPGA_MGR_STAT));
	log_debug(" CTRL             %08x",
			socfpga_read(socfpga, SOCFPGA_MGR_CTRL));
	log_debug(" DCLKCNT          %08x",
			socfpga_read(socfpga, SOCFPGA_MGR_DCLKCNT));
	log_debug(" DCLKSTAT         %08x",
			socfpga_read(socfpga, SOCFPGA_MGR_DCLKSTAT));
	log_debug(" GPO              %08x",
			socfpga_read(socfpga, SOCFPGA_MGR_GPO));
	log_debug(" GPI              %08x",
			socfpga_read(socfpga, SOCFPGA_MGR_GPI));
	log_debug(" MISCI            %08x",
			socfpga_read(socfpga, SOCFPGA_MGR_MISCI));
	log_debug(" GPIO_INTEN       %08x",
			socfpga_read(socfpga, SOCFPGA_MGR_GPIO_INTEN));
	log_debug(" GPIO_INTMASK     %08x",
			socfpga_read(socfpga, SOCFPGA_MGR_GPIO_INTMASK));
	log_debug(" GPIO_INTTYPE_LEV %08x",
			socfpga_read(socfpga, SOCFPGA_MGR_GPIO_INTTYPE_LEVEL));
	log_debug(" GPIO_INT_POLARIT %08x",
			socfpga_read(socfpga, SOCFPGA_MGR_GPIO_INT_POLARITY));
	log_debug(" GPIO_INTSTATUS   %08x",
			socfpga_read(socfpga, SOCFPGA_MGR_GPIO_INTSTATUS));
	log_debug(" GPIO_RAW_INTSTAT %08x",
			socfpga_read(socfpga, SOCFPGA_MGR_GPIO_RAW_INTSTATUS));
	log_debug(" GPIO_PORTA_EOI   %08x",
			socfpga_read(socfpga, SOCFPGA_MGR_GPIO_PORTA_EOI));
	log_debug(" GPIO_EXT_PORTA   %08x",
			socfpga_read(socfpga, SOCFPGA_MGR_GPIO_EXT_PORTA));
	log_debug(" GPIO_LS_SYNC     %08x",
			socfpga_read(socfpga, SOCFPGA_MGR_GPIO_LS_SYNC));
	log_debug(" GPIO_VER_ID_CODE %08x",
			socfpga_read(socfpga, SOCFPGA_MGR_GPIO_VER_ID_CODE));
	log_debug(" GPIO_CONFIG_REG2 %08x",
			socfpga_read(socfpga, SOCFPGA_MGR_GPIO_CONFIG_REG2));
	log_debug(" GPIO_CONFIG_REG1 %08x",
			socfpga_read(socfpga, SOCFPGA_MGR_GPIO_CONFIG_REG1));
}

static uint32_t socfpga_mode(struct socfpga *socfpga) {
	uint32_t ret =
		socfpga_read(socfpga, SOCFPGA_MGR_STAT) & SOCFPGA_MODE_MASK;
	switch (ret) {
	case SOCFPGA_MODE_MASK:
	case SOCFPGA_MODE_OFF:
	case SOCFPGA_MODE_RESET:
	case SOCFPGA_MODE_CONFIG:
	case SOCFPGA_MODE_INIT:
	case SOCFPGA_MODE_USER:
	case SOCFPGA_MODE_UNKNOWN:
		return ret;
	default:
		log_error("incorrect socfpga state: 0x%x", ret);
		return 0;
	}
}

static int socfpga_mode_wait(struct socfpga *socfpga, uint32_t mode) {
	int t = TIMEOUT;

	while (t--) {
		if (socfpga_mode(socfpga) == mode) {
			break;
		}

		usleep(100);
	}

	if (t <= 0) {
		log_error("FPGA reset timeout!");
		return -ETIMEDOUT;
	}

	return 0;
}

static int socfpga_reset(struct socfpga *socfpga) {
	/* Enable FPGA.
	 * Note: this step is not mentioned in datasheet,
	 * but is done in Linux driver */
	socfpga_or(socfpga, SOCFPGA_MGR_CTRL,
			SOCFPGA_CTRL_EN);

	/* Enter reset state */
	socfpga_or(socfpga, SOCFPGA_MGR_CTRL,
			SOCFPGA_CTRL_NCONFIGPULL);

	if (socfpga_mode_wait(socfpga, SOCFPGA_MODE_RESET)) {
		return -ETIMEDOUT;
	}

	/* Exit reset phase */
	socfpga_clear(socfpga, SOCFPGA_MGR_CTRL,
			SOCFPGA_CTRL_NCONFIGPULL);

	return 0;
}


/**
 * @brief Corresponds to Power Up Phase, Reset Phase in docs
 */
static int socfpga_config_init(struct fpga *fpga) {
	struct socfpga *socfpga = fpga->priv;
	uint32_t tmp;
	int err;

	/* TODO: figure out what values should be actualy passed.
	 * According to manual, "You must set the cdratio and
	 * cfgwdth bits appropriatly" */
	tmp = socfpga_read(socfpga, SOCFPGA_MGR_CTRL);
	tmp &= ~SOCFPGA_CTRL_CDRATIO_OFFT;
	tmp &= ~SOCFPGA_CTRL_CFGWDTH;
	/* TODO: Set correct values */
	socfpga_write(socfpga, SOCFPGA_MGR_CTRL, tmp);

	err = socfpga_reset(socfpga);
	if (err != 0) {
		return err;
	}

	err = socfpga_mode_wait(socfpga, SOCFPGA_MODE_CONFIG);
	if (err != 0) {
		return -ETIMEDOUT;
	}

	/* Enable data transfer to FPGA */
	socfpga_or(socfpga, SOCFPGA_MGR_CTRL, SOCFPGA_CTRL_AXICFEN);

	return 0;
}

/**
 * @brief Corresponds to Configuration Phase in docs
 */
static int socfpga_config_write(struct fpga *fpga, const uint8_t *buf, size_t len) {
	struct socfpga *socfpga = fpga->priv;
	uint32_t *buf32 = (uint32_t *) buf;
	uint32_t tmp;

	while (len > sizeof(uint32_t)) {
		socfpga_data_write(socfpga, *buf32);
		buf32++;
		len -= 4;
	}

	if (len) {
		tmp = 0;
		memcpy(buf32, &tmp, len);
		socfpga_data_write(socfpga, tmp);
	}

	return 0;
}

/**
 * @brief Corresponds to Initialization Phase and User Mode in docs
 */
static int socfpga_config_complete(struct fpga *fpga) {
	struct socfpga *socfpga = fpga->priv;
	int err;
	int timeout = TIMEOUT;
	uint32_t tmp;

	/* Wait for config write done */
	while (timeout-- >= 0) {
		tmp = socfpga_read(socfpga, SOCFPGA_MGR_GPIO_EXT_PORTA);
		if ((tmp & SOCFPGA_GPIO_EXT_PORTA_NSP) &&
			(tmp & SOCFPGA_GPIO_EXT_PORTA_CDP)) {
			break;
		}
		usleep(100);
	}

	if (timeout <= 0) {
		log_error("Timeout waiting for config write finish!");
		return -ETIMEDOUT;
	}

	/* Disable data transfer */
	socfpga_clear(socfpga, SOCFPGA_MGR_CTRL, SOCFPGA_CTRL_AXICFEN);

	/* Send 4 clocks to FPGA.
	 * Note: This is not mentioned in docs, but for some reason
	 * HPS LED control example fails without this */
	socfpga_write(socfpga, SOCFPGA_MGR_DCLKSTAT,
			SOCFPGA_DCLKSTAT_DONE); /* W1C */
	socfpga_write(socfpga, SOCFPGA_MGR_DCLKCNT, 4);
	timeout = TIMEOUT;
	while (timeout-- >= 0) {
		tmp = socfpga_read(socfpga, SOCFPGA_MGR_DCLKSTAT);
		if (tmp & SOCFPGA_DCLKSTAT_DONE) {
			break;
		}
		usleep(100);
	}
	if (timeout <= 0) {
		log_error("Timeout waiting for dclk!");
		return -ETIMEDOUT;
	}

	err = socfpga_mode_wait(socfpga, SOCFPGA_MODE_USER);
	if (err != 0) {
		log_error("Timeout switching to User Mode!");
		return err;
	}

	/* Disconnect FPGA manager. Note: this is not required by
	 * the manual, but is done in Linux driver */
	socfpga_clear(socfpga, SOCFPGA_MGR_CTRL, SOCFPGA_CTRL_EN);

	return 0;
}

static struct fpga_ops socfpga_fpga_ops = {
	.config_init     = socfpga_config_init,
	.config_write    = socfpga_config_write,
	.config_complete = socfpga_config_complete,
};

#define FPGA_MGR_BASE OPTION_GET(NUMBER, mgr_base_addr)
#define FPGA_MGR_DATA OPTION_GET(NUMBER, mgr_data_addr)
static struct socfpga socfpga0 = {
	.mgr_base      = FPGA_MGR_BASE,
	.mgr_data_addr = FPGA_MGR_DATA,
};
PERIPH_MEMORY_DEFINE(socfpga_mgr,      FPGA_MGR_BASE, 0x878);
PERIPH_MEMORY_DEFINE(socfpga_mgr_data, FPGA_MGR_DATA, 0x4);

/* TODO: this mapping should be moved to corresponding modules, but
 * for now there are no modules for L3, SDRAM and System Manager, so
 * these mappings are places where are they needed */
PERIPH_MEMORY_DEFINE(socfpga_l3,       SOCFPGA_L3_REGS,       0x1000);
PERIPH_MEMORY_DEFINE(socfpga_sdram,    SOCFPGA_SDRAM_REGS,    0x6000);
PERIPH_MEMORY_DEFINE(socfpga_resetmgr, SOCFPGA_RESETMGR_REGS, 0x1000);
PERIPH_MEMORY_DEFINE(socfpga_sysmgr,   SOCFPGA_SYSMGR_REGS,   0x1000);

static irq_return_t socfpga_irq_handler(unsigned int irq_nr, void *data) {
	log_debug("enter irq");
	return IRQ_HANDLED;
}

EMBOX_UNIT_INIT(socfpga_init);
static int socfpga_init(void) {
	bridge_enable_handoff();

	fpga_register(&socfpga_fpga_ops, &socfpga0);

	socfpga_regdump(&socfpga0);

	return irq_attach(OPTION_GET(NUMBER, irq_num),
	                  socfpga_irq_handler,
	                  0,
	                  NULL,
	                  "socfpga IRQ handler");
}

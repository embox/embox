/**
 * @file imx.c
 * @brief Power management for i.MX6 boards
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 31.01.2018
 */

#include <drivers/common/memory.h>
#include <drivers/power/imx.h>
#include <drivers/serial/uart_device.h>
#include <drivers/serial/diag_serial.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <util/log.h>
#include <kernel/printk.h>
#include <kernel/time/ktime.h>

EMBOX_UNIT_INIT(imx_power_init);

#define ANADIG_BASE		OPTION_GET(NUMBER,anadig_base)
#define ANADIG_CORE		(ANADIG_BASE + 0x30)
#define REG_SET			0x4
#define REG_CLR			0x8

#define GPC_BASE		OPTION_GET(NUMBER,gpc_base)

#define GPC_CNTR		(GPC_BASE + 0x000)
# define GPU_VPU_PUP_REQ	(1 << 1)
# define GPU_VPU_PDN_REQ	(1 << 0)
#define GPC_IMR(n)		(GPC_BASE + 0x008 + 4 * (n))

#define PGC_GPU_CTRL		(GPC_BASE + 0x260)

#define PGC_CPU_PUPSCR		(GPC_BASE + 0x2A4)
#define PGC_SW2ISO_OFFT		8
#define PGC_SW2ISO_MASK		(0x3F << PGC_SW2ISO_OFFT)
#define PGC_SW_MASK		0x3F

#define PGC_CPU_PDNSCR		(GPC_BASE + 0x2A8)
#define PGC_ISO2SW_OFFT		8
#define PGC_ISO2SW_MASK		(0x3F << PGC_ISO2SW_OFFT)
#define PGC_ISO_MASK		0x3F

#define IMR_NUM			4

static int imx_power_init(void) {
	int i;
	uint32_t temp;

	/* Default values */
	const uint32_t sw = 0xf;
	const uint32_t sw2iso = 0xf;
	const uint32_t iso = 0x1;
	const uint32_t iso2sw = 0x1;


	for (i = 0; i < IMR_NUM; i++) {
		REG32_STORE(GPC_IMR(i), 0xFFFFFFFF);
	}

	temp = REG32_LOAD(PGC_CPU_PUPSCR);
	temp &= ~(PGC_SW2ISO_MASK | PGC_SW_MASK);
	temp |= sw | (sw2iso << PGC_SW2ISO_OFFT);
	REG32_STORE(PGC_CPU_PUPSCR, temp);

	temp = REG32_LOAD(PGC_CPU_PDNSCR);
	temp &= ~(PGC_ISO2SW_MASK | PGC_ISO_MASK);
	temp |= iso | (iso2sw << PGC_ISO2SW_OFFT);
	REG32_STORE(PGC_CPU_PDNSCR, temp);

	return 0;
}

#define ANADIG_REG_TARG_MASK	0x1f
#define ANADIG_REG1_TARG_SHIFT	9	/* VDDPU */
#define ANADIG_REG2_TARG_SHIFT	18	/* VDDSOC */

static void imx_anatop_pu_enable(int enable) {
	/* Note: currently this function works only with VDDPU
	 * (VDDSOC is usually initialized by bootloader */
	uint32_t tmp;

	tmp = REG32_LOAD(ANADIG_CORE);

	if (enable) {
		tmp |= ANADIG_REG_TARG_MASK << ANADIG_REG1_TARG_SHIFT;
	} else {
		tmp &= ~(ANADIG_REG_TARG_MASK << ANADIG_REG1_TARG_SHIFT);
	}

	REG32_STORE(ANADIG_CORE, tmp);
	ksleep(50);
}

extern int clk_enable(char *clk_name);
extern int clk_disable(char *clk_name);

void imx_gpu_power_set(int up) {
	if (up) {
		/* Power-up */
		imx_anatop_pu_enable(1);
		/* TODO calculate delay? */
		ksleep(50);

		REG32_ORIN(GPC_CNTR, GPU_VPU_PUP_REQ);

		while (REG32_LOAD(GPC_CNTR) & GPU_VPU_PUP_REQ);
	} else {
		/* Power-down */
		REG32_STORE(PGC_GPU_CTRL, 1);
		REG32_STORE(GPC_CNTR, GPU_VPU_PDN_REQ);

		while (REG32_LOAD(GPC_CNTR) & GPU_VPU_PDN_REQ);

		ksleep(50);
		imx_anatop_pu_enable(0);
	}
}

PERIPH_MEMORY_DEFINE(imx_power, GPC_BASE, 0x2B0);

PERIPH_MEMORY_DEFINE(imx_anadig, ANADIG_BASE, 0x100);

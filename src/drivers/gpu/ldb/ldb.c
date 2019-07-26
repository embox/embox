/**
 * @file ldb.c
 * @brief LVDS Display Bridge
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 07.10.2017
 */

#include <util/log.h>

#include <drivers/common/memory.h>
#include <hal/reg.h>

#include <drivers/clk/ccm_imx6.h>
#include <drivers/iomuxc.h>

#include <framework/mod/options.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(ldb_init);

#define LDB_CTRL       OPTION_GET(NUMBER,base_addr)
#define DATA_WIDTH     OPTION_GET(NUMBER,data_width)

#define LDB_CH0_OFF     0x0
#define LDB_CH0_DI0     0x1
#define LDB_CH0_DI1     0x3

#define LDB_CH1_OFF     0x0
#define LDB_CH1_DI0     0x4
#define LDB_CH1_DI1     0xC

#define CCM_ANALOG_MISC1            0x20C8160

#define LDB_BGREF_RMODE_MASK        0x00008000
#define LDB_BGREF_RMODE_INT         0x00008000
#define LDB_BGREF_RMODE_EXT         0x0

#define LDB_DI1_VS_POL_MASK         0x00000400
#define LDB_DI1_VS_POL_ACT_LOW      0x00000400
#define LDB_DI1_VS_POL_ACT_HIGH     0x0
#define LDB_DI0_VS_POL_MASK         0x00000200
#define LDB_DI0_VS_POL_ACT_LOW      0x00000200
#define LDB_DI0_VS_POL_ACT_HIGH     0x0

#define LDB_BIT_MAP_CH1_MASK        0x00000100
#define LDB_BIT_MAP_CH1_JEIDA       0x00000100
#define LDB_BIT_MAP_CH1_SPWG        0x0
#define LDB_BIT_MAP_CH0_MASK        0x00000040
#define LDB_BIT_MAP_CH0_JEIDA       0x00000040
#define LDB_BIT_MAP_CH0_SPWG        0x0

#define LDB_DATA_WIDTH_CH1_MASK     0x00000080
#define LDB_DATA_WIDTH_CH1_24       0x00000080
#define LDB_DATA_WIDTH_CH1_18       0x0
#define LDB_DATA_WIDTH_CH0_MASK     0x00000020
#define LDB_DATA_WIDTH_CH0_24       0x00000020
#define LDB_DATA_WIDTH_CH0_18       0x0

#define LDB_CH1_MODE_MASK           0x0000000C
#define LDB_CH1_MODE_EN_TO_DI1      0x0000000C
#define LDB_CH1_MODE_EN_TO_DI0      0x00000004
#define LDB_CH1_MODE_DISABLE        0x0
#define LDB_CH0_MODE_MASK           0x00000003
#define LDB_CH0_MODE_EN_TO_DI1      0x00000003
#define LDB_CH0_MODE_EN_TO_DI0      0x00000001
#define LDB_CH0_MODE_DISABLE        0x0

#define LDB_SPLIT_MODE_EN           0x00000010

static int ldb_init(void) {
	uint32_t reg;

	iomuxc_write(IOMUXC_GPR3, iomuxc_read(IOMUXC_GPR3) & ~(3 << 6));

	reg = REG32_LOAD(LDB_CTRL);

	reg &= ~LDB_BGREF_RMODE_MASK;
	reg |= LDB_BGREF_RMODE_EXT;
	reg &= ~(LDB_BIT_MAP_CH0_MASK | LDB_BIT_MAP_CH1_MASK);
	reg |= LDB_BIT_MAP_CH0_SPWG | LDB_BIT_MAP_CH1_SPWG;
	reg &= ~(LDB_CH0_MODE_MASK | LDB_CH1_MODE_MASK);
	reg &= ~(LDB_DATA_WIDTH_CH0_MASK | LDB_DATA_WIDTH_CH1_MASK);
#if DATA_WIDTH == 18
	reg |= LDB_DATA_WIDTH_CH0_18 | LDB_DATA_WIDTH_CH1_18;
#elif DATA_WIDTH == 24
	reg |= LDB_DATA_WIDTH_CH0_24 | LDB_DATA_WIDTH_CH1_24;
#else
#error "Unsupported data width"
#endif
	reg &= ~LDB_SPLIT_MODE_EN;
	reg |= LDB_CH0_MODE_EN_TO_DI0;
	REG32_STORE(LDB_CTRL, reg);

	clk_enable("ldb_di0");

	return 0;
}

int ldb_bits(void) {
	return DATA_WIDTH;
}

PERIPH_MEMORY_DEFINE(ldb_iomux, 0x20E0000, 0x1000);

PERIPH_MEMORY_DEFINE(ccm_analog, 0x20C8000, 0x200);

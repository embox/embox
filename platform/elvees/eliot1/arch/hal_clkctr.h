/**
 * @file
 *
 * @date Jan 24, 2023
 * @author Anton Bondarev
 */

#ifndef PLATFORM_ELVEES_ELIOT1_ARCH_HAL_CLKCTR_H_
#define PLATFORM_ELVEES_ELIOT1_ARCH_HAL_CLKCTR_H_


#define CLKCTR_MAX_SYSCLK_DIV     31
#define CLKCTR_MAX_FCLK_DIV       31
#define CLKCTR_MAX_GNSSCLK_DIV     7
#define CLKCTR_MAX_QSPICLK_DIV    31
#define CLKCTR_MAX_MCOCLK_DIV     31
#define CLKCTR_MAX_I2SCLK_DIV     31
#define CLKCTR_MIN_SYSCLK_DIV      0
#define CLKCTR_MIN_FCLK_DIV        0
#define CLKCTR_MIN_GNSSCLK_DIV     0
#define CLKCTR_MIN_QSPICLK_DIV     0
#define CLKCTR_MIN_MCOCLK_DIV      0
#define CLKCTR_MIN_I2SCLK_DIV      0

#define PLL_MAX_MULTIPLIER     0x176
#define PLL_MIN_MULTIPLIER       0x0

#define CLKCTR_CLK_TYPE_HFI    0
#define CLKCTR_CLK_TYPE_XTI    1
#define CLKCTR_CLK_TYPE_PLL    2

struct clkctr_pll_cfg {
    uint32_t lock;
    uint32_t nr_man;
    uint32_t nf_man;
    uint32_t od_man;
    uint32_t man;
    uint32_t sel;
};


#define CLKCTR_XTI_MIN               1
#define CLKCTR_XTI_MAX        50000000
#define CLKCTR_XTI32_MIN         30000
#define CLKCTR_XTI32_MAX         34000
#define CLKCTR_HFI_MIN               1
#define CLKCTR_HFI_MAX        16000000
#define CLKCTR_LFI_MIN           32112
#define CLKCTR_LFI_MAX           33423
#define CLKCTR_I2S_EXTCLK_MIN        1
#define CLKCTR_I2S_EXTCLK_MAX 50000000

#define CLKCTR_PLLREF_MIN        30000
#define CLKCTR_PLLREF_MAX     50000000
#define CLKCTR_PLLCLK_MIN      1880000
#define CLKCTR_PLLCLK_MAX    375000000
#define CLKCTR_PLLCLK_OD_MIN  30000000
#define CLKCTR_PLLCLK_OD_MAX 375000000
#define CLKCTR_FCLK_MIN              1
#define CLKCTR_FCLK_MAX      150000000
#define CLKCTR_SYSCLK_MIN            1
#define CLKCTR_SYSCLK_MAX     50000000
#define CLKCTR_QSPICLK_MIN           1
#define CLKCTR_QSPICLK_MAX    96000000
#define CLKCTR_GNSSCLK_MIN           1
#define CLKCTR_GNSSCLK_MAX    80000000
#define CLKCTR_I2SCLK_MIN            1
#define CLKCTR_I2SCLK_MAX     25000000


#endif /* PLATFORM_ELVEES_ELIOT1_ARCH_HAL_CLKCTR_H_ */

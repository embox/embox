/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    18.11.2025
 */

#ifndef __BMCU_CRU_H
#define __BMCU_CRU_H

#include <stdint.h>

/**
 * @brief Control Registers Unit
 */
typedef struct
{
    volatile uint32_t CLKSEL;           /*!< Clock select control,                  Address offset: 0x00 */
    volatile uint32_t PLLSET;           /*!< PLL configuration set,                 Address offset: 0x04 */
    volatile uint32_t SYSCLKSEL0;       /*!< Clock domains select control 0,        Address offset: 0x08 */
    volatile uint32_t PCLK0EN;          /*!< APB0 domain clock gating control,      Address offset: 0x0C */
    volatile uint32_t PCLK1EN;          /*!< APB1 domain clock gating control,      Address offset: 0x10 */
    volatile uint32_t PCLK2EN;          /*!< APB2 domain clock gating control,      Address offset: 0x14 */
    volatile uint32_t SYSCTRL0;         /*!< System resource control 0,             Address offset: 0x18 */
    volatile uint32_t SYSCTRL1;         /*!< System resource control 1,             Address offset: 0x1C */
    volatile uint32_t SYSCTRL2;         /*!< System resource control 2,             Address offset: 0x20 */
    volatile uint32_t PRIOR0;           /*!< AXI masters priority control,          Address offset: 0x24 */
    volatile uint32_t PRIOR1;           /*!< AXI slave priority control,            Address offset: 0x28 */
    volatile uint32_t PADPUEN0;         /*!< PullUp control 0,                      Address offset: 0x2C */
    volatile uint32_t PADPUEN1;         /*!< PullUp control 1,                      Address offset: 0x30 */
    volatile uint32_t PADPDEN0;         /*!< PullDown control 0,                    Address offset: 0x34 */
    volatile uint32_t PADPDEN1;         /*!< PullDown control 1,                    Address offset: 0x38 */
    volatile uint32_t PADALT0;          /*!< PAD alternate function control 0,      Address offset: 0x3C */
    volatile uint32_t PADALT1;          /*!< PAD alternate function control 1,      Address offset: 0x40 */
    volatile uint32_t PADALT2;          /*!< PAD alternate function control 2,      Address offset: 0x44 */
    volatile uint32_t PADALT3;          /*!< PAD alternate function control 3,      Address offset: 0x48 */
    volatile uint32_t PADALT4;          /*!< PAD alternate function control 4,      Address offset: 0x4C */
    volatile uint32_t PADALT5;          /*!< PAD alternate function control 5,      Address offset: 0x50 */
    volatile uint32_t PADDS0;           /*!< Port 0 outputs drive strength,         Address offset: 0x54 */
    volatile uint32_t PADDS1;           /*!< Port 1 outputs drive strength,         Address offset: 0x58 */
    volatile uint32_t PADDS2;           /*!< Port 2 outputs drive strength,         Address offset: 0x5C */
    volatile uint32_t LDOCTRL;          /*!< LDO control register,                  Address offset: 0x60 */
    uint32_t      RESERVED0[3];     /*!< Reserved,                              Address offset: 0x64 */
    volatile uint32_t SYSSTAT0;         /*!< System resource status,                Address offset: 0x70 */
    volatile uint32_t WDTCLRKEY;        /*!< Secure key for WDTx reset              Address offset: 0x74 */
    uint32_t      RESERVED1[2];     /*!< Reserved,                              Address offset: 0x78 */
    volatile uint32_t INTCTRL0;         /*!< Interrupt route control,               Address offset: 0x80 */
    volatile uint32_t SYSCLKSEL1;       /*!< Clock domains select control 1,        Address offset: 0x84 */
    volatile uint32_t CRPTADDR;         /*!< MAGMA address register,                Address offset: 0x88 */
    volatile uint32_t CRPTDATA;         /*!< MAGMA data register,                   Address offset: 0x8C */
} CRU_TypeDef;


/* Peripheral memory map */
#define PERIPH_BASE             0x11000000UL                        /*!< Peripheral base address */
#define APB2PERIPH_BASE         (PERIPH_BASE + 0x03000000UL)        /*!< APB2 peripheral base address */

/* APB2 periphery */
#define CRU_BASE                (APB2PERIPH_BASE + 0x00000000UL)    /*!< CRU peripheral base address */
#define CRU                    ((CRU_TypeDef*)CRU_BASE)



/*******************  Bit definition for CRU_CLKSEL register  *****************/

/** @brief CLKSEL[0]: PLL clock source select */
#define CRU_CLKSEL_REFCLKSEL_Pos            (0U)
#define CRU_CLKSEL_REFCLKSEL_Msk            (0x1UL << CRU_CLKSEL_REFCLKSEL_Pos)
#define CRU_CLKSEL_REFCLKSEL                CRU_CLKSEL_REFCLKSEL_Msk

#define CRU_CLKSEL_REFCLKSEL_C0             (0x0UL << CRU_CLKSEL_REFCLKSEL_Pos) /*!< C0 selected as PLL clock source */
#define CRU_CLKSEL_REFCLKSEL_C1             (0x1UL << CRU_CLKSEL_REFCLKSEL_Pos) /*!< C1 selected as PLL clock source */

/** @brief CLKSEL[2:1]: AXI domain clock (CCLK) source select */
#define CRU_CLKSEL_CCLKSEL_Pos              (1U)
#define CRU_CLKSEL_CCLKSEL_Msk              (0x3UL << CRU_CLKSEL_CCLKSEL_Pos)
#define CRU_CLKSEL_CCLKSEL                  CRU_CLKSEL_CCLKSEL_Msk

#define CRU_CLKSEL_CCLKSEL_C0               (0x0UL << CRU_CLKSEL_CCLKSEL_Pos)   /*!< C0 used as AXI domain clock (CCLK) source */
#define CRU_CLKSEL_CCLKSEL_PLL              (0x1UL << CRU_CLKSEL_CCLKSEL_Pos)   /*!< PLL used as AXI domain clock (CCLK) source */
#define CRU_CLKSEL_CCLKSEL_C1               (0x2UL << CRU_CLKSEL_CCLKSEL_Pos)   /*!< C1 used as AXI domain clock (CCLK) source */

/** @brief CLKSEL[4:3]: APB0 domain clock (PCLK0) source select */
#define CRU_CLKSEL_PCLK0SEL_Pos             (3U)
#define CRU_CLKSEL_PCLK0SEL_Msk             (0x3UL << CRU_CLKSEL_PCLK0SEL_Pos)
#define CRU_CLKSEL_PCLK0SEL                 CRU_CLKSEL_PCLK0SEL_Msk

#define CRU_CLKSEL_PCLK0SEL_C0              (0x0UL << CRU_CLKSEL_PCLK0SEL_Pos)  /*!< C0 used as APB0 domain clock (PCLK0) source */
#define CRU_CLKSEL_PCLK0SEL_PLL             (0x1UL << CRU_CLKSEL_PCLK0SEL_Pos)  /*!< PLL used as APB0 domain clock (PCLK0) source */
#define CRU_CLKSEL_PCLK0SEL_C1              (0x2UL << CRU_CLKSEL_PCLK0SEL_Pos)  /*!< C1 used as APB0 domain clock (PCLK0) source */

/** @brief CLKSEL[6:5]: APB1 domain clock (PCLK1) source select */
#define CRU_CLKSEL_PCLK1SEL_Pos             (5U)
#define CRU_CLKSEL_PCLK1SEL_Msk             (0x3UL << CRU_CLKSEL_PCLK1SEL_Pos)
#define CRU_CLKSEL_PCLK1SEL                 CRU_CLKSEL_PCLK1SEL_Msk

#define CRU_CLKSEL_PCLK1SEL_C0              (0x0UL << CRU_CLKSEL_PCLK1SEL_Pos)  /*!< C0 used as APB1 domain clock (PCLK1) source */
#define CRU_CLKSEL_PCLK1SEL_PLL             (0x1UL << CRU_CLKSEL_PCLK1SEL_Pos)  /*!< PLL used as APB1 domain clock (PCLK1) source */
#define CRU_CLKSEL_PCLK1SEL_C1              (0x2UL << CRU_CLKSEL_PCLK1SEL_Pos)  /*!< C1 used as APB1 domain clock (PCLK1) source */

/** @brief CLKSEL[8:7]: APB2 domain clock (PCLK2) source select */
#define CRU_CLKSEL_PCLK2SEL_Pos             (7U)
#define CRU_CLKSEL_PCLK2SEL_Msk             (0x3UL << CRU_CLKSEL_PCLK2SEL_Pos)
#define CRU_CLKSEL_PCLK2SEL                 CRU_CLKSEL_PCLK2SEL_Msk

#define CRU_CLKSEL_PCLK2SEL_C0              (0x0UL << CRU_CLKSEL_PCLK2SEL_Pos)  /*!< C0 used as APB2 domain clock (PCLK2) source */
#define CRU_CLKSEL_PCLK2SEL_PLL             (0x1UL << CRU_CLKSEL_PCLK2SEL_Pos)  /*!< PLL used as APB2 domain clock (PCLK2) source */
#define CRU_CLKSEL_PCLK2SEL_C1              (0x2UL << CRU_CLKSEL_PCLK2SEL_Pos)  /*!< C1 used as APB2 domain clock (PCLK2) source */

/** @brief CLKSEL[10:9]: AHB domain clock (HCLK) source select */
#define CRU_CLKSEL_HCLKSEL_Pos              (9U)
#define CRU_CLKSEL_HCLKSEL_Msk              (0x3UL << CRU_CLKSEL_HCLKSEL_Pos)
#define CRU_CLKSEL_HCLKSEL                  CRU_CLKSEL_HCLKSEL_Msk

#define CRU_CLKSEL_HCLKSEL_C0               (0x0UL << CRU_CLKSEL_HCLKSEL_Pos)   /*!< C0 used as AHB domain clock (HCLK) source */
#define CRU_CLKSEL_HCLKSEL_PLL              (0x1UL << CRU_CLKSEL_HCLKSEL_Pos)   /*!< PLL used as AHB domain clock (HCLK) source */
#define CRU_CLKSEL_HCLKSEL_C1               (0x2UL << CRU_CLKSEL_HCLKSEL_Pos)   /*!< C1 used as AHB domain clock (HCLK) source */

/** @brief CLKSEL[12:11]: TRACE clock (TRACECLK) source select */
#define CRU_CLKSEL_TCLKSEL_Pos              (11U)
#define CRU_CLKSEL_TCLKSEL_Msk              (0x3UL << CRU_CLKSEL_TCLKSEL_Pos)
#define CRU_CLKSEL_TCLKSEL                  CRU_CLKSEL_TCLKSEL_Msk

#define CRU_CLKSEL_TCLKSEL_C0               (0x0UL << CRU_CLKSEL_TCLKSEL_Pos)   /*!< C0 used as TRACE clock (TCLK) source */
#define CRU_CLKSEL_TCLKSEL_PLL              (0x1UL << CRU_CLKSEL_TCLKSEL_Pos)   /*!< PLL used as TRACE clock (TCLK) source */
#define CRU_CLKSEL_TCLKSEL_C1               (0x2UL << CRU_CLKSEL_TCLKSEL_Pos)   /*!< C1 used as TRACE clock (TCLK) source */

/** @brief CLKSEL[14:13]: CANx2 clock (CANx2CLK) source select */
#define CRU_CLKSEL_CANCLKSEL_Pos            (13U)
#define CRU_CLKSEL_CANCLKSEL_Msk            (0x3UL << CRU_CLKSEL_CANCLKSEL_Pos)
#define CRU_CLKSEL_CANCLKSEL                CRU_CLKSEL_CANCLKSEL_Msk

#define CRU_CLKSEL_CANCLKSEL_C0             (0x0UL << CRU_CLKSEL_CANCLKSEL_Pos) /*!< C0 used as CANx2 clock (CANx2CLK) source */
#define CRU_CLKSEL_CANCLKSEL_PLL            (0x1UL << CRU_CLKSEL_CANCLKSEL_Pos) /*!< PLL used as CANx2 clock (CANx2CLK) source */
#define CRU_CLKSEL_CANCLKSEL_C1             (0x2UL << CRU_CLKSEL_CANCLKSEL_Pos) /*!< C1 used as CANx2 clock (CANx2CLK) source */

/** @brief CLKSEL[23:16]: CCLK 1MHz pulse generator prescaler */
#define CRU_CLKSEL_CCLK1MHZ_Pos             (16U)
#define CRU_CLKSEL_CCLK1MHZ_Msk             (0xFFUL << CRU_CLKSEL_CCLK1MHZ_Pos)
#define CRU_CLKSEL_CCLK1MHZ                 CRU_CLKSEL_CCLK1MHZ_Msk

/** @brief CLKSEL[31:24]: HCLK 1MHz pulse generator prescaler */
#define CRU_CLKSEL_HCLK1MHZ_Pos             (24U)
#define CRU_CLKSEL_HCLK1MHZ_Msk             (0xFFUL << CRU_CLKSEL_HCLK1MHZ_Pos)
#define CRU_CLKSEL_HCLK1MHZ                 CRU_CLKSEL_HCLK1MHZ_Msk


/*******************  Bit definition for CRU_PLLSET register  *****************/

/** @brief PLLSET[3:0]: PLL reference divider (NR) value */
#define CRU_PLLSET_CLKR_Pos                 (0U)
#define CRU_PLLSET_CLKR_Msk                 (0xFUL << CRU_PLLSET_CLKR_Pos)
#define CRU_PLLSET_CLKR                     CRU_PLLSET_CLKR_Msk
/** @brief PLLSET[9:4]: PLL multiplication factor (NF) value */
#define CRU_PLLSET_CLKF_Pos                 (4U)
#define CRU_PLLSET_CLKF_Msk                 (0x3FUL << CRU_PLLSET_CLKF_Pos)
#define CRU_PLLSET_CLKF                     CRU_PLLSET_CLKF_Msk
/** @brief PLLSET[13:10]: PLL post VCO divider (OD) value */
#define CRU_PLLSET_CLKOD_Pos                (10U)
#define CRU_PLLSET_CLKOD_Msk                (0xFUL << CRU_PLLSET_CLKOD_Pos)
#define CRU_PLLSET_CLKOD                    CRU_PLLSET_CLKOD_Msk
/** @brief PLLSET[19:14]: PLL bandwidth divider (NB) value */
#define CRU_PLLSET_BWADJ_Pos                (14U)
#define CRU_PLLSET_BWADJ_Msk                (0x3FUL << CRU_PLLSET_BWADJ_Pos)
#define CRU_PLLSET_BWADJ                    CRU_PLLSET_BWADJ_Msk
/** @brief PLLSET[20]: PLL power down */
#define CRU_PLLSET_PWRDN_Pos                (20U)
#define CRU_PLLSET_PWRDN_Msk                (0x1UL << CRU_PLLSET_PWRDN_Pos)
#define CRU_PLLSET_PWRDN                    CRU_PLLSET_PWRDN_Msk
/** @brief PLLSET[21]: PLL internal feedback path select */
#define CRU_PLLSET_INTFB_Pos                (21U)
#define CRU_PLLSET_INTFB_Msk                (0x1UL << CRU_PLLSET_INTFB_Pos)
#define CRU_PLLSET_INTFB                    CRU_PLLSET_INTFB_Msk
/** @brief PLLSET[22]: PLL reference to output bypass */
#define CRU_PLLSET_BYPASS_Pos               (22U)
#define CRU_PLLSET_BYPASS_Msk               (0x1UL << CRU_PLLSET_BYPASS_Pos)
#define CRU_PLLSET_BYPASS                   CRU_PLLSET_BYPASS_Msk
/** @brief PLLSET[23]: PLL reference to counters to output bypass */
#define CRU_PLLSET_TEST_Pos                 (23U)
#define CRU_PLLSET_TEST_Msk                 (0x1UL << CRU_PLLSET_TEST_Pos)
#define CRU_PLLSET_TEST                     CRU_PLLSET_TEST_Msk
/** @brief PLLSET[24]: PLL reset */
#define CRU_PLLSET_RESET_Pos                (24U)
#define CRU_PLLSET_RESET_Msk                (0x1UL << CRU_PLLSET_RESET_Pos)
#define CRU_PLLSET_RESET                    CRU_PLLSET_RESET_Msk

/*****************  Bit definition for CRU_SYSCLKSEL0 register  ***************/

/** @brief SYSCLKSEL0[4:0]: CCLK divider */
#define CRU_SYSCLKSEL0_CCLKDIV_Pos          (0U)
#define CRU_SYSCLKSEL0_CCLKDIV_Msk          (0x1FUL << CRU_SYSCLKSEL0_CCLKDIV_Pos)
#define CRU_SYSCLKSEL0_CCLKDIV              CRU_SYSCLKSEL0_CCLKDIV_Msk

#define CRU_SYSCLKSEL0_CCLKDIV_DIV1         (0x0UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)   /*!< CCLK not divided */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV2         (0x2UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)   /*!< CCLK divided by 2 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV3         (0x4UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)   /*!< CCLK divided by 3 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV4         (0x6UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)   /*!< CCLK divided by 4 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV5         (0x8UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)   /*!< CCLK divided by 5 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV6         (0xAUL << CRU_SYSCLKSEL0_CCLKDIV_Pos)   /*!< CCLK divided by 6 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV7         (0xCUL << CRU_SYSCLKSEL0_CCLKDIV_Pos)   /*!< CCLK divided by 7 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV8         (0xEUL << CRU_SYSCLKSEL0_CCLKDIV_Pos)   /*!< CCLK divided by 8 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV9         (0x10UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)  /*!< CCLK divided by 9 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV10        (0x12UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)  /*!< CCLK divided by 10 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV11        (0x14UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)  /*!< CCLK divided by 11 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV12        (0x16UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)  /*!< CCLK divided by 12 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV13        (0x18UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)  /*!< CCLK divided by 13 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV14        (0x1AUL << CRU_SYSCLKSEL0_CCLKDIV_Pos)  /*!< CCLK divided by 14 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV15        (0x1CUL << CRU_SYSCLKSEL0_CCLKDIV_Pos)  /*!< CCLK divided by 15 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV16        (0x1EUL << CRU_SYSCLKSEL0_CCLKDIV_Pos)  /*!< CCLK divided by 16 */

#define CRU_SYSCLKSEL0_CCLKDIV_DIV1_5       (0x1UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)   /*!< CCLK divided by 1.5 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV2_5       (0x3UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)   /*!< CCLK divided by 2.5 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV3_5       (0x5UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)   /*!< CCLK divided by 3.5 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV4_5       (0x7UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)   /*!< CCLK divided by 4.5 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV5_5       (0x9UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)   /*!< CCLK divided by 5.5 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV6_5       (0xBUL << CRU_SYSCLKSEL0_CCLKDIV_Pos)   /*!< CCLK divided by 6.5 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV7_5       (0xDUL << CRU_SYSCLKSEL0_CCLKDIV_Pos)   /*!< CCLK divided by 7.5 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV8_5       (0xFUL << CRU_SYSCLKSEL0_CCLKDIV_Pos)   /*!< CCLK divided by 8.5 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV9_5       (0x11UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)  /*!< CCLK divided by 9.5 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV10_5      (0x13UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)  /*!< CCLK divided by 10.5 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV11_5      (0x15UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)  /*!< CCLK divided by 11.5 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV12_5      (0x17UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)  /*!< CCLK divided by 12.5 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV13_5      (0x19UL << CRU_SYSCLKSEL0_CCLKDIV_Pos)  /*!< CCLK divided by 13.5 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV14_5      (0x1BUL << CRU_SYSCLKSEL0_CCLKDIV_Pos)  /*!< CCLK divided by 14.5 */
#define CRU_SYSCLKSEL0_CCLKDIV_DIV15_5      (0x1DUL << CRU_SYSCLKSEL0_CCLKDIV_Pos)  /*!< CCLK divided by 15.5 */

/** @brief SYSCLKSEL0[5]: CCLK divider enable */
#define CRU_SYSCLKSEL0_CCLKDIVEN_Pos        (5U)
#define CRU_SYSCLKSEL0_CCLKDIVEN_Msk        (0x1UL << CRU_SYSCLKSEL0_CCLKDIVEN_Pos)
#define CRU_SYSCLKSEL0_CCLKDIVEN            CRU_SYSCLKSEL0_CCLKDIVEN_Msk

/** @brief SYSCLKSEL0[10:6]: PCLK0 divider */
#define CRU_SYSCLKSEL0_PCLK0DIV_Pos         (6U)
#define CRU_SYSCLKSEL0_PCLK0DIV_Msk         (0x1FUL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)
#define CRU_SYSCLKSEL0_PCLK0DIV             CRU_SYSCLKSEL0_PCLK0DIV_Msk

#define CRU_SYSCLKSEL0_PCLK0DIV_DIV1        (0x0UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)   /*!< PCLK0 not divided */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV2        (0x2UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)   /*!< PCLK0 divided by 2 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV3        (0x4UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)   /*!< PCLK0 divided by 3 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV4        (0x6UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)   /*!< PCLK0 divided by 4 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV5        (0x8UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)   /*!< PCLK0 divided by 5 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV6        (0xAUL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)   /*!< PCLK0 divided by 6 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV7        (0xCUL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)   /*!< PCLK0 divided by 7 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV8        (0xEUL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)   /*!< PCLK0 divided by 8 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV9        (0x10UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)  /*!< PCLK0 divided by 9 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV10       (0x12UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)  /*!< PCLK0 divided by 10 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV11       (0x14UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)  /*!< PCLK0 divided by 11 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV12       (0x16UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)  /*!< PCLK0 divided by 12 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV13       (0x18UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)  /*!< PCLK0 divided by 13 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV14       (0x1AUL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)  /*!< PCLK0 divided by 14 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV15       (0x1CUL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)  /*!< PCLK0 divided by 15 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV16       (0x1EUL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)  /*!< PCLK0 divided by 16 */

#define CRU_SYSCLKSEL0_PCLK0DIV_DIV1_5      (0x1UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)   /*!< PCLK0 divided by 1.5 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV2_5      (0x3UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)   /*!< PCLK0 divided by 2.5 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV3_5      (0x5UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)   /*!< PCLK0 divided by 3.5 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV4_5      (0x7UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)   /*!< PCLK0 divided by 4.5 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV5_5      (0x9UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)   /*!< PCLK0 divided by 5.5 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV6_5      (0xBUL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)   /*!< PCLK0 divided by 6.5 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV7_5      (0xDUL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)   /*!< PCLK0 divided by 7.5 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV8_5      (0xFUL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)   /*!< PCLK0 divided by 8.5 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV9_5      (0x11UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)  /*!< PCLK0 divided by 9.5 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV10_5     (0x13UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)  /*!< PCLK0 divided by 10.5 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV11_5     (0x15UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)  /*!< PCLK0 divided by 11.5 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV12_5     (0x17UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)  /*!< PCLK0 divided by 12.5 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV13_5     (0x19UL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)  /*!< PCLK0 divided by 13.5 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV14_5     (0x1BUL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)  /*!< PCLK0 divided by 14.5 */
#define CRU_SYSCLKSEL0_PCLK0DIV_DIV15_5     (0x1DUL << CRU_SYSCLKSEL0_PCLK0DIV_Pos)  /*!< PCLK0 divided by 15.5 */

/** @brief SYSCLKSEL0[11]: PCLK0 divider enable */
#define CRU_SYSCLKSEL0_PCLK0DIVEN_Pos       (11U)
#define CRU_SYSCLKSEL0_PCLK0DIVEN_Msk       (0x1UL << CRU_SYSCLKSEL0_PCLK0DIVEN_Pos)
#define CRU_SYSCLKSEL0_PCLK0DIVEN           CRU_SYSCLKSEL0_PCLK0DIVEN_Msk

/** @brief SYSCLKSEL0[16:12]: PCLK1 divider */
#define CRU_SYSCLKSEL0_PCLK1DIV_Pos         (12U)
#define CRU_SYSCLKSEL0_PCLK1DIV_Msk         (0x1FUL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)
#define CRU_SYSCLKSEL0_PCLK1DIV             CRU_SYSCLKSEL0_PCLK1DIV_Msk

#define CRU_SYSCLKSEL0_PCLK1DIV_DIV1        (0x0UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)   /*!< PCLK1 not divided */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV2        (0x2UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)   /*!< PCLK1 divided by 2 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV3        (0x4UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)   /*!< PCLK1 divided by 3 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV4        (0x6UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)   /*!< PCLK1 divided by 4 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV5        (0x8UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)   /*!< PCLK1 divided by 5 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV6        (0xAUL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)   /*!< PCLK1 divided by 6 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV7        (0xCUL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)   /*!< PCLK1 divided by 7 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV8        (0xEUL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)   /*!< PCLK1 divided by 8 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV9        (0x10UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)  /*!< PCLK1 divided by 9 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV10       (0x12UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)  /*!< PCLK1 divided by 10 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV11       (0x14UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)  /*!< PCLK1 divided by 11 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV12       (0x16UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)  /*!< PCLK1 divided by 12 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV13       (0x18UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)  /*!< PCLK1 divided by 13 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV14       (0x1AUL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)  /*!< PCLK1 divided by 14 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV15       (0x1CUL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)  /*!< PCLK1 divided by 15 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV16       (0x1EUL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)  /*!< PCLK1 divided by 16 */

#define CRU_SYSCLKSEL0_PCLK1DIV_DIV1_5      (0x1UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)   /*!< PCLK1 divided by 1.5 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV2_5      (0x3UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)   /*!< PCLK1 divided by 2.5 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV3_5      (0x5UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)   /*!< PCLK1 divided by 3.5 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV4_5      (0x7UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)   /*!< PCLK1 divided by 4.5 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV5_5      (0x9UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)   /*!< PCLK1 divided by 5.5 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV6_5      (0xBUL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)   /*!< PCLK1 divided by 6.5 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV7_5      (0xDUL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)   /*!< PCLK1 divided by 7.5 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV8_5      (0xFUL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)   /*!< PCLK1 divided by 8.5 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV9_5      (0x11UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)  /*!< PCLK1 divided by 9.5 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV10_5     (0x13UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)  /*!< PCLK1 divided by 10.5 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV11_5     (0x15UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)  /*!< PCLK1 divided by 11.5 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV12_5     (0x17UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)  /*!< PCLK1 divided by 12.5 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV13_5     (0x19UL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)  /*!< PCLK1 divided by 13.5 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV14_5     (0x1BUL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)  /*!< PCLK1 divided by 14.5 */
#define CRU_SYSCLKSEL0_PCLK1DIV_DIV15_5     (0x1DUL << CRU_SYSCLKSEL0_PCLK1DIV_Pos)  /*!< PCLK1 divided by 15.5 */

/** @brief SYSCLKSEL0[17]: PCLK1 divider enable */
#define CRU_SYSCLKSEL0_PCLK1DIVEN_Pos       (17U)
#define CRU_SYSCLKSEL0_PCLK1DIVEN_Msk       (0x1UL << CRU_SYSCLKSEL0_PCLK1DIVEN_Pos)
#define CRU_SYSCLKSEL0_PCLK1DIVEN           CRU_SYSCLKSEL0_PCLK1DIVEN_Msk

/** @brief SYSCLKSEL0[22:18]: PCLK2 divider */
#define CRU_SYSCLKSEL0_PCLK2DIV_Pos         (18U)
#define CRU_SYSCLKSEL0_PCLK2DIV_Msk         (0x1FUL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)
#define CRU_SYSCLKSEL0_PCLK2DIV             CRU_SYSCLKSEL0_PCLK2DIV_Msk

#define CRU_SYSCLKSEL0_PCLK2DIV_DIV1        (0x0UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)   /*!< PCLK2 not divided */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV2        (0x2UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)   /*!< PCLK2 divided by 2 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV3        (0x4UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)   /*!< PCLK2 divided by 3 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV4        (0x6UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)   /*!< PCLK2 divided by 4 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV5        (0x8UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)   /*!< PCLK2 divided by 5 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV6        (0xAUL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)   /*!< PCLK2 divided by 6 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV7        (0xCUL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)   /*!< PCLK2 divided by 7 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV8        (0xEUL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)   /*!< PCLK2 divided by 8 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV9        (0x10UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)  /*!< PCLK2 divided by 9 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV10       (0x12UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)  /*!< PCLK2 divided by 10 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV11       (0x14UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)  /*!< PCLK2 divided by 11 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV12       (0x16UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)  /*!< PCLK2 divided by 12 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV13       (0x18UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)  /*!< PCLK2 divided by 13 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV14       (0x1AUL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)  /*!< PCLK2 divided by 14 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV15       (0x1CUL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)  /*!< PCLK2 divided by 15 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV16       (0x1EUL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)  /*!< PCLK2 divided by 16 */

#define CRU_SYSCLKSEL0_PCLK2DIV_DIV1_5      (0x1UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)   /*!< PCLK2 divided by 1.5 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV2_5      (0x3UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)   /*!< PCLK2 divided by 2.5 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV3_5      (0x5UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)   /*!< PCLK2 divided by 3.5 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV4_5      (0x7UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)   /*!< PCLK2 divided by 4.5 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV5_5      (0x9UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)   /*!< PCLK2 divided by 5.5 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV6_5      (0xBUL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)   /*!< PCLK2 divided by 6.5 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV7_5      (0xDUL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)   /*!< PCLK2 divided by 7.5 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV8_5      (0xFUL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)   /*!< PCLK2 divided by 8.5 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV9_5      (0x11UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)  /*!< PCLK2 divided by 9.5 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV10_5     (0x13UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)  /*!< PCLK2 divided by 10.5 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV11_5     (0x15UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)  /*!< PCLK2 divided by 11.5 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV12_5     (0x17UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)  /*!< PCLK2 divided by 12.5 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV13_5     (0x19UL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)  /*!< PCLK2 divided by 13.5 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV14_5     (0x1BUL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)  /*!< PCLK2 divided by 14.5 */
#define CRU_SYSCLKSEL0_PCLK2DIV_DIV15_5     (0x1DUL << CRU_SYSCLKSEL0_PCLK2DIV_Pos)  /*!< PCLK2 divided by 15.5 */

/** @brief SYSCLKSEL0[23]: PCLK2 divider enable */
#define CRU_SYSCLKSEL0_PCLK2DIVEN_Pos       (23U)
#define CRU_SYSCLKSEL0_PCLK2DIVEN_Msk       (0x1UL << CRU_SYSCLKSEL0_PCLK2DIVEN_Pos)
#define CRU_SYSCLKSEL0_PCLK2DIVEN           CRU_SYSCLKSEL0_PCLK2DIVEN_Msk

/** @brief SYSCLKSEL0[28:24]: HCLK divider */
#define CRU_SYSCLKSEL0_HCLKDIV_Pos          (24U)
#define CRU_SYSCLKSEL0_HCLKDIV_Msk          (0x1FUL << CRU_SYSCLKSEL0_HCLKDIV_Pos)
#define CRU_SYSCLKSEL0_HCLKDIV              CRU_SYSCLKSEL0_HCLKDIV_Msk

#define CRU_SYSCLKSEL0_HCLKDIV_DIV1         (0x0UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)   /*!< HCLK not divided */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV2         (0x2UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)   /*!< HCLK divided by 2 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV3         (0x4UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)   /*!< HCLK divided by 3 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV4         (0x6UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)   /*!< HCLK divided by 4 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV5         (0x8UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)   /*!< HCLK divided by 5 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV6         (0xAUL << CRU_SYSCLKSEL0_HCLKDIV_Pos)   /*!< HCLK divided by 6 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV7         (0xCUL << CRU_SYSCLKSEL0_HCLKDIV_Pos)   /*!< HCLK divided by 7 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV8         (0xEUL << CRU_SYSCLKSEL0_HCLKDIV_Pos)   /*!< HCLK divided by 8 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV9         (0x10UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)  /*!< HCLK divided by 9 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV10        (0x12UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)  /*!< HCLK divided by 10 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV11        (0x14UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)  /*!< HCLK divided by 11 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV12        (0x16UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)  /*!< HCLK divided by 12 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV13        (0x18UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)  /*!< HCLK divided by 13 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV14        (0x1AUL << CRU_SYSCLKSEL0_HCLKDIV_Pos)  /*!< HCLK divided by 14 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV15        (0x1CUL << CRU_SYSCLKSEL0_HCLKDIV_Pos)  /*!< HCLK divided by 15 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV16        (0x1EUL << CRU_SYSCLKSEL0_HCLKDIV_Pos)  /*!< HCLK divided by 16 */

#define CRU_SYSCLKSEL0_HCLKDIV_DIV1_5       (0x1UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)   /*!< HCLK divided by 1.5 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV2_5       (0x3UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)   /*!< HCLK divided by 2.5 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV3_5       (0x5UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)   /*!< HCLK divided by 3.5 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV4_5       (0x7UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)   /*!< HCLK divided by 4.5 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV5_5       (0x9UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)   /*!< HCLK divided by 5.5 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV6_5       (0xBUL << CRU_SYSCLKSEL0_HCLKDIV_Pos)   /*!< HCLK divided by 6.5 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV7_5       (0xDUL << CRU_SYSCLKSEL0_HCLKDIV_Pos)   /*!< HCLK divided by 7.5 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV8_5       (0xFUL << CRU_SYSCLKSEL0_HCLKDIV_Pos)   /*!< HCLK divided by 8.5 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV9_5       (0x11UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)  /*!< HCLK divided by 9.5 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV10_5      (0x13UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)  /*!< HCLK divided by 10.5 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV11_5      (0x15UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)  /*!< HCLK divided by 11.5 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV12_5      (0x17UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)  /*!< HCLK divided by 12.5 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV13_5      (0x19UL << CRU_SYSCLKSEL0_HCLKDIV_Pos)  /*!< HCLK divided by 13.5 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV14_5      (0x1BUL << CRU_SYSCLKSEL0_HCLKDIV_Pos)  /*!< HCLK divided by 14.5 */
#define CRU_SYSCLKSEL0_HCLKDIV_DIV15_5      (0x1DUL << CRU_SYSCLKSEL0_HCLKDIV_Pos)  /*!< HCLK divided by 15.5 */

/** @brief SYSCLKSEL0[29]: HCLK divider enable */
#define CRU_SYSCLKSEL0_HCLKDIVEN_Pos        (29U)
#define CRU_SYSCLKSEL0_HCLKDIVEN_Msk        (0x1UL << CRU_SYSCLKSEL0_HCLKDIVEN_Pos)
#define CRU_SYSCLKSEL0_HCLKDIVEN            CRU_SYSCLKSEL0_HCLKDIVEN_Msk

/** @brief SYSCLKSEL0[30]: RC generator enable */
#define CRU_SYSCLKSEL0_RCGENEN_Pos          (30U)
#define CRU_SYSCLKSEL0_RCGENEN_Msk          (0x1UL << CRU_SYSCLKSEL0_RCGENEN_Pos)
#define CRU_SYSCLKSEL0_RCGENEN              CRU_SYSCLKSEL0_RCGENEN_Msk

/** @brief SYSCLKSEL0[31]: Boot clock source select */
#define CRU_SYSCLKSEL0_BOOTCLKSEL_Pos       (31U)
#define CRU_SYSCLKSEL0_BOOTCLKSEL_Msk       (0x1UL << CRU_SYSCLKSEL0_BOOTCLKSEL_Pos)
#define CRU_SYSCLKSEL0_BOOTCLKSEL           CRU_SYSCLKSEL0_BOOTCLKSEL_Msk

#endif /* __BMCU_CRU_H */

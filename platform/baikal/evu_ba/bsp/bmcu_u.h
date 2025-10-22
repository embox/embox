/**
 * *****************************************************************************
 *  @file       bmcu_u.h
 *  @author     Baikal electronics SDK team
 *  @brief      BASIS Baikal-U registers and their fields definitions
 *  @version    1.3
 *  @date       2025-06-30
 * *****************************************************************************
 *  @copyright Copyright (c) 2025 Baikal Electronics
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#ifndef __BMCU_U_H
#define __BMCU_U_H

#include <stdint.h>
#include "basis_core_riscv.h"

#define __ALIGNED(x)                __attribute__((aligned(x)))
#define __PACKED       __attribute__((packed, aligned(4)))
#define __IO            volatile        /*!< Defines 'read/write' permissions */

/******************************************************************************/
/*                             Interrupt ID Definition                        */
/******************************************************************************/

/** @brief CLINT Interrupts */
typedef enum
{
    CLINT_Reserved0             = 0,    /*!< Reserved */
    CLINT_Reserved1             = 1,    /*!< Reserved */
    CLINT_Reserved2             = 2,    /*!< Reserved */
    CLINT_MS_IRQn               = 3,    /*!< Machine Software Interrupt */
    CLINT_Reserved3             = 4,    /*!< Reserved */
    CLINT_Reserved4             = 5,    /*!< Reserved */
    CLINT_Reserved5             = 6,    /*!< Reserved */
    CLINT_MT_IRQn               = 7,    /*!< Machine Timer Interrupt */
    CLINT_Reserved6             = 8,    /*!< Reserved */
    CLINT_Reserved7             = 9,    /*!< Reserved */
    CLINT_Reserved8             = 10,   /*!< Reserved */
    CLINT_Reserved9             = 11,   /*!< Reserved */
    CLINT_Reserved10            = 12,   /*!< Reserved */
    CLINT_Reserved11            = 13,   /*!< Reserved */
    CLINT_Reserved12            = 14,   /*!< Reserved */
    CLINT_Reserved13            = 15    /*!< Reserved */
} CLINT_IRQn_Type;

/** @brief CLIC Interrupts */
typedef enum
{
    CLIC_US_IRQn                = 0,    /*!< User Software Interrupt */
    CLIC_Reserved0              = 1,    /*!< Reserved */
    CLIC_Reserved1              = 2,    /*!< Reserved */
    CLIC_MS_IRQn                = 3,    /*!< Machine Software Interrupt */
    CLIC_UT_IRQn                = 4,    /*!< User Timer Interrupt */
    CLIC_Reserved2              = 5,    /*!< Reserved */
    CLIC_Reserved3              = 6,    /*!< Reserved */
    CLIC_MT_IRQn                = 7,    /*!< Machine Timer Interrupt */
    CLIC_Reserved4              = 8,    /*!< Reserved */
    CLIC_Reserved5              = 9,    /*!< Reserved */
    CLIC_Reserved6              = 10,   /*!< Reserved */
    CLIC_Reserved7              = 11,   /*!< Reserved */
    CLIC_CS_IRQn                = 12,   /*!< CLIC Software Interrupt */
    CLIC_Reserved8              = 13,   /*!< Reserved */
    CLIC_Reserved9              = 14,   /*!< Reserved */
    CLIC_Reserved10             = 15,   /*!< Reserved */
    CLIC_QSPI0_IRQn             = 16,   /*!< QSPI0 Interrupt */
    CLIC_UART0_IRQn             = 17,   /*!< UART0 Interrupt */
    CLIC_UART1_IRQn             = 18,   /*!< UART1 Interrupt */
    CLIC_UART2_IRQn             = 19,   /*!< UART2 Interrupt */
    CLIC_I2C0_IRQn              = 20,   /*!< I2C0 Interrupt */
    CLIC_I2C1_IRQn              = 21,   /*!< I2C1 Interrupt */
    CLIC_I2S0_IRQn              = 22,   /*!< I2S0 Interrupt */
    CLIC_TIM0_Channel0_IRQn     = 23,   /*!< TIM0 Channel 0 Interrupt */
    CLIC_TIM0_Channel1_IRQn     = 24,   /*!< TIM0 Channel 1 Interrupt */
    CLIC_TIM0_Channel2_IRQn     = 25,   /*!< TIM0 Channel 2 Interrupt */
    CLIC_TIM0_Channel3_IRQn     = 26,   /*!< TIM0 Channel 3 Interrupt */
    CLIC_WDT0_IRQn              = 27,   /*!< WDT0 Interrupt */
    CLIC_GPIO0_IRQn             = 28,   /*!< GPIO0 Interrupt */
    CLIC_PWMG0_IRQn             = 29,   /*!< PWMG0 Interrupt */
    CLIC_SPI0_IRQn              = 30,   /*!< SPI0 Interrupt */
    CLIC_SPI1_IRQn              = 31,   /*!< SPI1 Interrupt */
    CLIC_CANFD0_IRQn            = 32,   /*!< CANFD0 Interrupt */
    CLIC_QSPI1_IRQn             = 33,   /*!< QSPI1 Interrupt */
    CLIC_UART3_IRQn             = 34,   /*!< UART3 Interrupt */
    CLIC_UART4_IRQn             = 35,   /*!< UART4 Interrupt */
    CLIC_UART5_IRQn             = 36,   /*!< UART5 Interrupt */
    CLIC_I2C2_IRQn              = 37,   /*!< I2C2 Interrupt */
    CLIC_I2C3_IRQn              = 38,   /*!< I2C3 Interrupt */
    CLIC_I2S1_IRQn              = 39,   /*!< I2S1 Interrupt */
    CLIC_TIM1_Channel0_IRQn     = 40,   /*!< TIM1 Channel 0 Interrupt */
    CLIC_TIM1_Channel1_IRQn     = 41,   /*!< TIM1 Channel 1 Interrupt */
    CLIC_TIM1_Channel2_IRQn     = 42,   /*!< TIM1 Channel 2 Interrupt */
    CLIC_TIM1_Channel3_IRQn     = 43,   /*!< TIM1 Channel 3 Interrupt */
    CLIC_Reserved11             = 44,   /*!< Reserved */
    CLIC_GPIO1_IRQn             = 45,   /*!< GPIO1 Interrupt */
    CLIC_PWMG1_IRQn             = 46,   /*!< PWMG1 Interrupt */
    CLIC_SPI2_IRQn              = 47,   /*!< SPI2 Interrupt */
    CLIC_SPI3_IRQn              = 48,   /*!< SPI3 Interrupt */
    CLIC_CANFD1_IRQn            = 49,   /*!< CANFD1 Interrupt */
    CLIC_ADC0_IRQn              = 50,   /*!< ADC0 Interrupt */
    CLIC_ADC1_IRQn              = 51,   /*!< ADC1 Interrupt */
    CLIC_ADC2_IRQn              = 52,   /*!< ADC2 Interrupt */
    CLIC_PWMA0_IRQn             = 53,   /*!< PWMA0 Interrupt */
    CLIC_QE0_IRQn               = 54,   /*!< QE0 Interrupt */
    CLIC_PWMA1_IRQn             = 55,   /*!< PWMA1 Interrupt */
    CLIC_QE1_IRQn               = 56,   /*!< QE1 Interrupt */
    CLIC_PWMA2_IRQn             = 57,   /*!< PWMA2 Interrupt */
    CLIC_QE2_IRQn               = 58,   /*!< QE2 Interrupt */
    CLIC_PWMA3_IRQn             = 59,   /*!< PWMA3 Interrupt */
    CLIC_QE3_IRQn               = 60,   /*!< QE3 Interrupt */
    CLIC_GPIO2_IRQn             = 61,   /*!< GPIO2 Interrupt */
    CLIC_UART6_IRQn             = 62,   /*!< UART6 Interrupt */
    CLIC_UART7_IRQn             = 63,   /*!< UART7 Interrupt */
    CLIC_USB_IRQn               = 64,   /*!< USB Interrupt */
    CLIC_DMA0_IRQn              = 65,   /*!< DMA0 Interrupt */
    CLIC_DMA1_IRQn              = 66,   /*!< DMA1 Interrupt */
    CLIC_DMA2_IRQn              = 67,   /*!< DMA2 Interrupt */
    CLIC_CORE2_IRQn             = 68,   /*!< CORE2 Interrupt */
    CLIC_EXTI_PA_IRQn           = 69,   /*!< EXTI PA Interrupt */
    CLIC_EXTI_PB_IRQn           = 70,   /*!< EXTI PB Interrupt */
    CLIC_EXTI_PC_IRQn           = 71,   /*!< EXTI PC Interrupt */
    CLIC_CLKI_Mon_IRQn          = 72,   /*!< CLKI Monitor Interrupt */
    CLIC_CORE0_BusError_IRQn    = 73,   /*!< CORE0 Bus Access Error Interrupt */
    CLIC_CORE1_BusError_IRQn    = 74    /*!< CORE1 Bus Access Error Interrupt */
} CLIC_IRQn_Type;

/** @brief Non-Maskable Interrupts */
typedef enum
{
    NMI_WDT1_IRQn               = 0     /*!< WDT1 Interrupt */
} NMI_IRQn_Type;

/******************************************************************************/
/*                         Peripheral Registers Definition                    */
/******************************************************************************/

/**
 * @brief Analog to Digital Converter
 */
typedef struct
{
    __IO uint32_t CR1;              /*!< Control register 1,                    Address offset: 0x00 */
    __IO uint32_t ASER;             /*!< Set register,                          Address offset: 0x04 */
    __IO uint32_t ASTR;             /*!< Status register,                       Address offset: 0x08 */
    __IO uint32_t ASMPR;            /*!< Sample time register,                  Address offset: 0x0C */
    __IO uint32_t ASQR;             /*!< Sequence register,                     Address offset: 0x10 */
    __IO uint32_t ADR;              /*!< Data register,                         Address offset: 0x14 */
} ADC_TypeDef;

/**
 * @brief Advanced High-performance Bus
 */
typedef struct
{
    __IO uint32_t PL;               /*!< Arbitration Priority Master n Register,    Address offset: 0x00 */
    uint32_t RESERVED0[14];         /*!< Reserved,                                  Address offset: 0x04 */
    __IO uint32_t EBTCOUNT;         /*!< Early Burst Termination Count Register,    Address offset: 0x3C */
    __IO uint32_t EBT_EN;           /*!< Early Burst Termination Enable Register,   Address offset: 0x40 */
    __IO uint32_t EBT;              /*!< Early Burst Termination Register,          Address offset: 0x44 */
    __IO uint32_t DFLT_MASTER;      /*!< Default Master ID Number Register,         Address offset: 0x48 */
    uint32_t RESERVED1[17];         /*!< Reserved,                                  Address offset: 0x4C */
    __IO uint32_t VERSION_ID;       /*!< Component Version ID Register,             Address offset: 0x90 */
} AHB_TypeDef;

/**
 * @brief Advanced eXtensible Interface
 */
typedef struct
{
    uint32_t RESERVED0[256];        /*!< Reserved,                              Address offset: 0x000 */
    __IO uint32_t VERSION_ID;       /*!< Component Version ID Register,         Address offset: 0x400 */
    __IO uint32_t HW_CFG;           /*!< Hardware Configuration Register,       Address offset: 0x404 */
    __IO uint32_t CMD;              /*!< Command Enable Register,               Address offset: 0x408 */
    __IO uint32_t DATA;             /*!< Data Register,                         Address offset: 0x40C */
} AXI_TypeDef;

/**
 * @brief CAN Tx Message Space Registers
 */
typedef struct
{
    __IO uint32_t ID;               /*!< TB ID Register,                        Address offset: 0x00 */
    __IO uint32_t DLC;              /*!< TB DLC Register,                       Address offset: 0x04 */
    __IO uint32_t DW[16];           /*!< TB DWn Register,                       Address offset: 0x08 */
} CANFD_TB_TypeDef;

/**
 * @brief CAN Acceptance Filter Registers
 */
typedef struct
{
    __IO uint32_t AFMR;             /*!< Acceptance Filter Mask Register,       Address offset: 0x00 */
    __IO uint32_t AFIR;             /*!< Acceptance Filter ID Register,         Address offset: 0x04 */
} CANFD_AF_TypeDef;

/**
 * @brief CAN Tx Event FIFO Registers
 */
typedef struct
{
    __IO uint32_t ID;               /*!< TXE FIFO TB ID Register,               Address offset: 0x00 */
    __IO uint32_t DLC;              /*!< TXE FIFO TB DLC Register,              Address offset: 0x04 */
} CANFD_TXE_TB_TypeDef;

/**
 * @brief CAN Rx Message Space Registers
 */
typedef struct
{
    __IO uint32_t ID;               /*!< RB ID Register,                        Address offset: 0x00 */
    __IO uint32_t DLC;              /*!< RB DLC Register,                       Address offset: 0x04 */
    __IO uint32_t DW[16];           /*!< RB DWn Register,                       Address offset: 0x08 */
} CANFD_RB_TypeDef;

/**
 * @brief Controller Area Network
 */
typedef struct
{
    __IO uint32_t SRR;                  /*!< Software Reset Register,                                   Address offset: 0x00 */
    __IO uint32_t MSR;                  /*!< Mode Select Register,                                      Address offset: 0x04 */
    __IO uint32_t BRPR;                 /*!< Arbitration Phase Baud Rate Prescaler Register,            Address offset: 0x08 */
    __IO uint32_t BTR;                  /*!< Arbitration Phase Bit Timing Register,                     Address offset: 0x0C */
    __IO uint32_t ECR;                  /*!< Error Counter Register,                                    Address offset: 0x10 */
    __IO uint32_t ESR;                  /*!< Error Status Register,                                     Address offset: 0x14 */
    __IO uint32_t SR;                   /*!< Status Register,                                           Address offset: 0x18 */
    __IO uint32_t ISR;                  /*!< Interrupt Status Register,                                 Address offset: 0x1C */
    __IO uint32_t IER;                  /*!< Interrupt Enable Register,                                 Address offset: 0x20 */
    __IO uint32_t ICR;                  /*!< Interrupt Clear Register,                                  Address offset: 0x24 */
    __IO uint32_t TSR;                  /*!< Timestamp Register,                                        Address offset: 0x28 */
    uint32_t RESERVED0[23];             /*!< Reserved,                                                  Address offset: 0x2C */
    __IO uint32_t DP_BRPR;              /*!< Data Phase Baud Rate Prescaler Register,                   Address offset: 0x88 */
    __IO uint32_t DP_BTR;               /*!< Data Phase Bit Timing Register,                            Address offset: 0x8C */
    __IO uint32_t TRR;                  /*!< Tx Buffer Ready Request Register,                          Address offset: 0x90 */
    __IO uint32_t IETRS;                /*!< Interrupt Enable Tx Buffer Ready Request Served Register,  Address offset: 0x94 */
    __IO uint32_t TCR;                  /*!< Tx Buffer Cancel Request Register,                         Address offset: 0x98 */
    __IO uint32_t IETCS;                /*!< Interrupt Enable Tx Buffer Cancel Request Served Register, Address offset: 0x9C */
    __IO uint32_t TXE_FSR;              /*!< Tx Event FIFO Status Register,                             Address offset: 0xA0 */
    __IO uint32_t TXE_WMR;              /*!< Tx Event FIFO Watermark Register,                          Address offset: 0xA4 */
    uint32_t RESERVED1[14];             /*!< Reserved,                                                  Address offset: 0xA8 */
    __IO uint32_t AFR;                  /*!< Acceptance Filter Register,                                Address offset: 0xE0 */
    uint32_t RESERVED2;                 /*!< Reserved,                                                  Address offset: 0xE4 */
    __IO uint32_t FSR;                  /*!< Rx FIFO Status Register,                                   Address offset: 0xE8 */
    __IO uint32_t WMR;                  /*!< Rx FIFO Watermark Register,                                Address offset: 0xEC */
    uint32_t RESERVED3[4];              /*!< Reserved,                                                  Address offset: 0xF0 */
    CANFD_TB_TypeDef TB[32];            /*!< Tx Message Space Registers,                                Address offset: 0x0100 */
    CANFD_AF_TypeDef AF[32];            /*!< ID Filter Registers,                                       Address offset: 0x0A00 */
    uint32_t RESERVED4[1344];           /*!< Reserved,                                                  Address offset: 0x0B00 */
    CANFD_TXE_TB_TypeDef TXE_TB[32];    /*!< Tx Event FIFO Registers,                                   Address offset: 0x2000 */
    CANFD_RB_TypeDef RB_FIFO0[64];      /*!< Rx Message Space (FIFO-0),                                 Address offset: 0x2100 */
    uint32_t RESERVED5[896];            /*!< Reserved,                                                  Address offset: 0x3300 */
    CANFD_RB_TypeDef RB_FIFO1[64];      /*!< Rx Message Space (FIFO-1),                                 Address offset: 0x4100 */
} CANFD_TypeDef;

/**
 * @brief Control Registers Unit
 */
typedef struct
{
    __IO uint32_t CLKSEL;           /*!< Clock select control,                  Address offset: 0x00 */
    __IO uint32_t PLLSET;           /*!< PLL configuration set,                 Address offset: 0x04 */
    __IO uint32_t SYSCLKSEL0;       /*!< Clock domains select control 0,        Address offset: 0x08 */
    __IO uint32_t PCLK0EN;          /*!< APB0 domain clock gating control,      Address offset: 0x0C */
    __IO uint32_t PCLK1EN;          /*!< APB1 domain clock gating control,      Address offset: 0x10 */
    __IO uint32_t PCLK2EN;          /*!< APB2 domain clock gating control,      Address offset: 0x14 */
    __IO uint32_t SYSCTRL0;         /*!< System resource control 0,             Address offset: 0x18 */
    __IO uint32_t SYSCTRL1;         /*!< System resource control 1,             Address offset: 0x1C */
    __IO uint32_t SYSCTRL2;         /*!< System resource control 2,             Address offset: 0x20 */
    __IO uint32_t PRIOR0;           /*!< AXI masters priority control,          Address offset: 0x24 */
    __IO uint32_t PRIOR1;           /*!< AXI slave priority control,            Address offset: 0x28 */
    __IO uint32_t PADPUEN0;         /*!< PullUp control 0,                      Address offset: 0x2C */
    __IO uint32_t PADPUEN1;         /*!< PullUp control 1,                      Address offset: 0x30 */
    __IO uint32_t PADPDEN0;         /*!< PullDown control 0,                    Address offset: 0x34 */
    __IO uint32_t PADPDEN1;         /*!< PullDown control 1,                    Address offset: 0x38 */
    __IO uint32_t PADALT0;          /*!< PAD alternate function control 0,      Address offset: 0x3C */
    __IO uint32_t PADALT1;          /*!< PAD alternate function control 1,      Address offset: 0x40 */
    __IO uint32_t PADALT2;          /*!< PAD alternate function control 2,      Address offset: 0x44 */
    __IO uint32_t PADALT3;          /*!< PAD alternate function control 3,      Address offset: 0x48 */
    __IO uint32_t PADALT4;          /*!< PAD alternate function control 4,      Address offset: 0x4C */
    __IO uint32_t PADALT5;          /*!< PAD alternate function control 5,      Address offset: 0x50 */
    __IO uint32_t PADDS0;           /*!< Port 0 outputs drive strength,         Address offset: 0x54 */
    __IO uint32_t PADDS1;           /*!< Port 1 outputs drive strength,         Address offset: 0x58 */
    __IO uint32_t PADDS2;           /*!< Port 2 outputs drive strength,         Address offset: 0x5C */
    __IO uint32_t LDOCTRL;          /*!< LDO control register,                  Address offset: 0x60 */
    uint32_t      RESERVED0[3];     /*!< Reserved,                              Address offset: 0x64 */
    __IO uint32_t SYSSTAT0;         /*!< System resource status,                Address offset: 0x70 */
    __IO uint32_t WDTCLRKEY;        /*!< Secure key for WDTx reset              Address offset: 0x74 */
    uint32_t      RESERVED1[2];     /*!< Reserved,                              Address offset: 0x78 */
    __IO uint32_t INTCTRL0;         /*!< Interrupt route control,               Address offset: 0x80 */
    __IO uint32_t SYSCLKSEL1;       /*!< Clock domains select control 1,        Address offset: 0x84 */
    __IO uint32_t CRPTADDR;         /*!< MAGMA address register,                Address offset: 0x88 */
    __IO uint32_t CRPTDATA;         /*!< MAGMA data register,                   Address offset: 0x8C */
} CRU_TypeDef;

/**
 * @brief DMA Channel Registers
 */
typedef struct
{
    __IO uint32_t SAR;              /*!< Source Address Register for Channel n,                 Address offset: 0x00 + n * 0x58 */
    uint32_t      RESERVED0;        /*!< Reserved,                                              Address offset: 0x04 + n * 0x58 */
    __IO uint32_t DAR;              /*!< Destination Address Register for Channel n,            Address offset: 0x08 + n * 0x58 */
    uint32_t      RESERVED1;        /*!< Reserved,                                              Address offset: 0x0C + n * 0x58 */
    __IO uint32_t LLP;              /*!< Linked List Pointer Register for Channel n,            Address offset: 0x10 + n * 0x58 */
    uint32_t      RESERVED2;        /*!< Reserved,                                              Address offset: 0x14 + n * 0x58 */
    __IO uint32_t CTL0;             /*!< Control Register 0 for Channel n,                      Address offset: 0x18 + n * 0x58 */
    __IO uint32_t CTL1;             /*!< Control Register 1 for Channel n,                      Address offset: 0x1C + n * 0x58 */
    uint32_t      RESERVED3[8];     /*!< Reserved,                                              Address offset: 0x20 + n * 0x58 */
    __IO uint32_t CFG0;             /*!< Configuration Register 0 for Channel n,                Address offset: 0x40 + n * 0x58 */
    __IO uint32_t CFG1;             /*!< Configuration Register 1 for Channel n,                Address offset: 0x44 + n * 0x58 */
    uint32_t      RESERVED4[4];     /*!< Reserved,                                              Address offset: 0x48 + n * 0x58 */
} DMA_Channel_TypeDef;

/**
 * @brief DMA Interrupt Registers
 */
typedef struct
{
    __IO uint32_t RAW_TFR;          /*!< Raw Status for IntTfr Interrupt,       Address offset: 0x2C0 */
    uint32_t      RESERVED0;        /*!< Reserved,                              Address offset: 0x2C4 */
    __IO uint32_t RAW_BLOCK;        /*!< Raw Status for IntBlock Interrupt,     Address offset: 0x2C8 */
    uint32_t      RESERVED1;        /*!< Reserved,                              Address offset: 0x2CC */
    __IO uint32_t RAW_SRC_TRAN;     /*!< Raw Status for IntSrcTran Interrupt,   Address offset: 0x2D0 */
    uint32_t      RESERVED2;        /*!< Reserved,                              Address offset: 0x2D4 */
    __IO uint32_t RAW_DST_TRAN;     /*!< Raw Status for IntDstTran Interrupt,   Address offset: 0x2D8 */
    uint32_t      RESERVED3;        /*!< Reserved,                              Address offset: 0x2DC */
    __IO uint32_t RAW_ERR;          /*!< Raw Status for IntErr Interrupt,       Address offset: 0x2E0 */
    uint32_t      RESERVED4;        /*!< Reserved,                              Address offset: 0x2E4 */
    __IO uint32_t STAT_TFR;         /*!< Status for IntTfr Interrupt,           Address offset: 0x2E8 */
    uint32_t      RESERVED5;        /*!< Reserved,                              Address offset: 0x2EC */
    __IO uint32_t STAT_BLOCK;       /*!< Status for IntBlock Interrupt,         Address offset: 0x2F0 */
    uint32_t      RESERVED6;        /*!< Reserved,                              Address offset: 0x2F4 */
    __IO uint32_t STAT_SRC_TRAN;    /*!< Status for IntSrcTran Interrupt,       Address offset: 0x2F8 */
    uint32_t      RESERVED7;        /*!< Reserved,                              Address offset: 0x2FC */
    __IO uint32_t STAT_DST_TRAN;    /*!< Status for IntDstTran Interrupt,       Address offset: 0x300 */
    uint32_t      RESERVED8;        /*!< Reserved,                              Address offset: 0x304 */
    __IO uint32_t STAT_ERR;         /*!< Status for IntErr Interrupt,           Address offset: 0x308 */
    uint32_t      RESERVED9;        /*!< Reserved,                              Address offset: 0x30C */
    __IO uint32_t MASK_TFR;         /*!< Mask for IntTfr Interrupt,             Address offset: 0x310 */
    uint32_t      RESERVED10;       /*!< Reserved,                              Address offset: 0x314 */
    __IO uint32_t MASK_BLOCK;       /*!< Mask for IntBlock Interrupt,           Address offset: 0x318 */
    uint32_t      RESERVED11;       /*!< Reserved,                              Address offset: 0x31C */
    __IO uint32_t MASK_SRC_TRAN;    /*!< Mask for IntSrcTran Interrupt,         Address offset: 0x320 */
    uint32_t      RESERVED12;       /*!< Reserved,                              Address offset: 0x324 */
    __IO uint32_t MASK_DST_TRAN;    /*!< Mask for IntDstTran Interrupt,         Address offset: 0x328 */
    uint32_t      RESERVED13;       /*!< Reserved,                              Address offset: 0x32C */
    __IO uint32_t MASK_ERR;         /*!< Mask for IntErr Interrupt,             Address offset: 0x330 */
    uint32_t      RESERVED14;       /*!< Reserved,                              Address offset: 0x334 */
    __IO uint32_t CLR_TFR;          /*!< Clear for IntTfr Interrupt,            Address offset: 0x338 */
    uint32_t      RESERVED15;       /*!< Reserved,                              Address offset: 0x33C */
    __IO uint32_t CLR_BLOCK;        /*!< Clear for IntBlock Interrupt,          Address offset: 0x340 */
    uint32_t      RESERVED16;       /*!< Reserved,                              Address offset: 0x344 */
    __IO uint32_t CLR_SRC_TRAN;     /*!< Clear for IntSrcTran Interrupt,        Address offset: 0x348 */
    uint32_t      RESERVED17;       /*!< Reserved,                              Address offset: 0x34C */
    __IO uint32_t CLR_DST_TRAN;     /*!< Clear for IntDstTran Interrupt,        Address offset: 0x350 */
    uint32_t      RESERVED18;       /*!< Reserved,                              Address offset: 0x354 */
    __IO uint32_t CLR_ERR;          /*!< Clear for IntErr Interrupt,            Address offset: 0x358 */
    uint32_t      RESERVED19;       /*!< Reserved,                              Address offset: 0x35C */
    __IO uint32_t STAT;             /*!< Status for each Interrupt type,        Address offset: 0x360 */
    uint32_t      RESERVED20;       /*!< Reserved,                              Address offset: 0x364 */
} DMA_Interrupt_TypeDef;

/**
 * @brief DMA Handshake Registers
 */
typedef struct
{
    __IO uint32_t REQ_SRC;          /*!< Source Software Transaction Request Register,          Address offset: 0x368 */
    uint32_t      RESERVED0;        /*!< Reserved,                                              Address offset: 0x36C */
    __IO uint32_t REQ_DST;          /*!< Destination Software Transaction Request Register,     Address offset: 0x370 */
    uint32_t      RESERVED1;        /*!< Reserved,                                              Address offset: 0x374 */
    __IO uint32_t SGL_REQ_SRC;      /*!< Source Single Transaction Request Register,            Address offset: 0x378 */
    uint32_t      RESERVED2;        /*!< Reserved,                                              Address offset: 0x37C */
    __IO uint32_t SGL_REQ_DST;      /*!< Destination Single Transaction Request Register,       Address offset: 0x380 */
    uint32_t      RESERVED3;        /*!< Reserved,                                              Address offset: 0x384 */
    __IO uint32_t LST_SRC;          /*!< Source Last Transaction Request Register,              Address offset: 0x388 */
    uint32_t      RESERVED4;        /*!< Reserved,                                              Address offset: 0x38C */
    __IO uint32_t LST_DST;          /*!< Destination Last Transaction Request Register,         Address offset: 0x390 */
    uint32_t      RESERVED5;        /*!< Reserved,                                              Address offset: 0x394 */
} DMA_Handshake_TypeDef;

/**
 * @brief DMA Miscellaneous Registers
 */
typedef struct
{
    __IO uint32_t CFG;              /*!< Configuration Register,                Address offset: 0x398 */
    uint32_t      RESERVED0;        /*!< Reserved,                              Address offset: 0x39C */
    __IO uint32_t CH_EN;            /*!< Channel Enable Register,               Address offset: 0x3A0 */
    uint32_t      RESERVED1;        /*!< Reserved,                              Address offset: 0x3A4 */
    __IO uint32_t ID;               /*!< ID Register,                           Address offset: 0x3A8 */
    uint32_t      RESERVED2;        /*!< Reserved,                              Address offset: 0x3AC */
    __IO uint32_t TEST;             /*!< Test Register,                         Address offset: 0x3B0 */
    uint32_t      RESERVED3[17];    /*!< Reserved,                              Address offset: 0x3B4 */
    __IO uint32_t COMP_TYPE;        /*!< Component Type Register,               Address offset: 0x3F8 */
    __IO uint32_t COMP_VERSION;     /*!< Component Version Register,            Address offset: 0x3FC */
} DMA_Misc_TypeDef;

/**
 * @brief Direct Memory Access Controller
 */
typedef struct
{
    DMA_Channel_TypeDef CH[8];      /*!< DMA Channel Registers,                 Address offset: 0x000 */
    DMA_Interrupt_TypeDef INT;      /*!< DMA Interrupt Registers,               Address offset: 0x2C0 */
    DMA_Handshake_TypeDef HS;       /*!< DMA Handshake Registers,               Address offset: 0x368 */
    DMA_Misc_TypeDef MSC;           /*!< DMA Miscellaneous Registers,           Address offset: 0x398 */
} DMA_TypeDef;

/**
 * @brief Embedded Flash
 */
typedef struct
{
    __IO uint32_t CTRL;             /*!< Control register,                      Address offset: 0x00 */
    __IO uint32_t ADDR;             /*!< Address register,                      Address offset: 0x04 */
    __IO uint32_t WDATA;            /*!< Data write register,                   Address offset: 0x08 */
    __IO uint32_t RDATA;            /*!< Data read register,                    Address offset: 0x0C */
    __IO uint32_t STATUS;           /*!< Status register,                       Address offset: 0x10 */
    __IO uint32_t TIME0;            /*!< Time 0 register,                       Address offset: 0x14 */
    __IO uint32_t TIME1;            /*!< Time 1 register,                       Address offset: 0x18 */
    __IO uint32_t TIME2;            /*!< Time 2 register,                       Address offset: 0x1C */
    __IO uint32_t TIME3;            /*!< Time 3 register,                       Address offset: 0x20 */
    __IO uint32_t TIME4;            /*!< Time 4 register,                       Address offset: 0x24 */
    __IO uint32_t TIME5;            /*!< Time 5 register,                       Address offset: 0x28 */
    __IO uint32_t TIME6;            /*!< Time 6 register,                       Address offset: 0x2C */
    __IO uint32_t MANUAL;           /*!< Manual control register,               Address offset: 0x30 */
    __IO uint32_t BAD_ADDR0;        /*!< Flash 0 bad sectors address register,  Address offset: 0x34 */
    __IO uint32_t BAD_ADDR1;        /*!< Flash 1 bad sectors address register,  Address offset: 0x38 */
    __IO uint32_t CFG_FLAGS;        /*!< Flash configuration flags register,    Address offset: 0x3C */
    __IO uint32_t NVR_DIS0;         /*!< NVR register 0,                        Address offset: 0x40 */
    __IO uint32_t NVR_DIS1;         /*!< NVR register 1,                        Address offset: 0x44 */
    __IO uint32_t NVR_DIS2;         /*!< NVR register 2,                        Address offset: 0x48 */
    __IO uint32_t NVR_DIS3;         /*!< NVR register 3,                        Address offset: 0x4C */
    __IO uint32_t NVR_DIS4;         /*!< NVR register 4,                        Address offset: 0x50 */
    __IO uint32_t NVR_DIS5;         /*!< NVR register 5,                        Address offset: 0x54 */
    __IO uint32_t NVR_DIS6;         /*!< NVR register 6,                        Address offset: 0x58 */
    __IO uint32_t NVR_DIS7;         /*!< NVR register 7,                        Address offset: 0x5C */
    __IO uint32_t NVR_DIS8;         /*!< NVR register 8,                        Address offset: 0x60 */
    __IO uint32_t NVR_DIS9;         /*!< NVR register 9,                        Address offset: 0x64 */
    __IO uint32_t NVR_DIS10;        /*!< NVR register 10,                       Address offset: 0x68 */
    __IO uint32_t NVR_DIS11;        /*!< NVR register 11,                       Address offset: 0x6C */
    __IO uint32_t NVR_DIS12;        /*!< NVR register 12,                       Address offset: 0x70 */
    __IO uint32_t NVR_DIS13;        /*!< NVR register 13,                       Address offset: 0x74 */
    __IO uint32_t NVR_DIS14;        /*!< NVR register 14,                       Address offset: 0x78 */
    __IO uint32_t NVR_DIS15;        /*!< NVR register 15,                       Address offset: 0x7C */
} EFLASH_TypeDef;

/**
  * @brief General Purpose I/O
  */
typedef struct {
    __IO uint32_t DR;               /*!< Data                                   Address offset: 0x00 */
    __IO uint32_t DDR;              /*!< Direction                              Address offset: 0x04 */
    uint32_t RESERVED0[10];          /*!<                                        Address offset: 0x08 */
    __IO uint32_t INTEN;            /*!< Interrupt enable                       Address offset: 0x30 */
    __IO uint32_t INTMSK;           /*!< Interrupt mask                         Address offset: 0x34 */
    __IO uint32_t INTLVL;           /*!< Interrupt level                        Address offset: 0x38 */
    __IO uint32_t INTPOLARITY;      /*!< Interrupt polarity                     Address offset: 0x3C */
    __IO uint32_t INTSTAT;          /*!< Interrupt status                       Address offset: 0x40 */
    __IO uint32_t INTSTATRAW;       /*!< Raw interrupt status                   Address offset: 0x44 */
    __IO uint32_t DEBOUNCE;         /*!< Debounce enable                        Address offset: 0x48 */
    __IO uint32_t EOI;              /*!< Clear interrupt                        Address offset: 0x4C */
    __IO uint32_t EXT;              /*!< External port                          Address offset: 0x50 */
    uint32_t RESERVED1[3];          /*!<                                        Address offset: 0x54 */
    __IO uint32_t SYNC;             /*!< Synchronization level                  Address offset: 0x60 */
    uint32_t RESERVED2[1];          /*!<                                        Address offset: 0x64 */
    __IO uint32_t BOTHEDGE;         /*!< Interrupt Both Edge type               Address offset: 0x68 */
} GPIO_TypeDef;

/**
 * @brief Inter Integrated Circuit Interface
 */
typedef struct
{
    __IO uint32_t CON;                      /*!< Control Register,                                  Address offset: 0x00 */
    __IO uint32_t TAR;                      /*!< Target Address Register,                           Address offset: 0x04 */
    __IO uint32_t SAR;                      /*!< Slave Address Register,                            Address offset: 0x08 */
    __IO uint32_t HS_MADDR;                 /*!< High Speed Master Mode Code Address Register,      Address offset: 0x0C */
    __IO uint32_t DATA_CMD;                 /*!< Rx/Tx Data Buffer and Command Register,            Address offset: 0x10 */
    __IO uint32_t SS_SCL_HCNT;              /*!< Standard Speed SCL High Count Register,            Address offset: 0x14 */
    __IO uint32_t SS_SCL_LCNT;              /*!< Standard Speed SCL Low Count Register,             Address offset: 0x18 */
    __IO uint32_t FS_SCL_HCNT;              /*!< Fast Mode SCL High Count Register,                 Address offset: 0x1C */
    __IO uint32_t FS_SCL_LCNT;              /*!< Fast Mode SCL Low Count Register,                  Address offset: 0x20 */
    __IO uint32_t HS_SCL_HCNT;              /*!< High Speed SCL High Count Register,                Address offset: 0x24 */
    __IO uint32_t HS_SCL_LCNT;              /*!< High Speed SCL Low Count Register,                 Address offset: 0x28 */
    __IO uint32_t INTR_STAT;                /*!< Interrupt Status Register,                         Address offset: 0x2C */
    __IO uint32_t INTR_MASK;                /*!< Interrupt Mask Register,                           Address offset: 0x30 */
    __IO uint32_t RAW_INTR_STAT;            /*!< Raw Interrupt Status Register,                     Address offset: 0x34 */
    __IO uint32_t RX_TL;                    /*!< Rx FIFO Threshold Register,                        Address offset: 0x38 */
    __IO uint32_t TX_TL;                    /*!< Tx FIFO Threshold Register,                        Address offset: 0x3C */
    __IO uint32_t CLR_INTR;                 /*!< Clear Combined and Individual Interrupt Register,  Address offset: 0x40 */
    __IO uint32_t CLR_RX_UNDER;             /*!< Clear RX_UNDER Interrupt Register,                 Address offset: 0x44 */
    __IO uint32_t CLR_RX_OVER;              /*!< Clear RX_OVER Interrupt Register,                  Address offset: 0x48 */
    __IO uint32_t CLR_TX_OVER;              /*!< Clear TX_OVER Interrupt Register,                  Address offset: 0x4C */
    __IO uint32_t CLR_RD_REQ;               /*!< Clear RD_REQ Interrupt Register,                   Address offset: 0x50 */
    __IO uint32_t CLR_TX_ABRT;              /*!< Clear TX_ABRT Interrupt Register,                  Address offset: 0x54 */
    __IO uint32_t CLR_RX_DONE;              /*!< Clear RX_DONE Interrupt Register,                  Address offset: 0x58 */
    __IO uint32_t CLR_ACTIVITY;             /*!< Clear ACTIVITY Interrupt Register,                 Address offset: 0x5C */
    __IO uint32_t CLR_STOP_DET;             /*!< Clear STOP_DET Interrupt Register,                 Address offset: 0x60 */
    __IO uint32_t CLR_START_DET;            /*!< Clear START_DET Interrupt Register,                Address offset: 0x64 */
    __IO uint32_t CLR_GEN_CALL;             /*!< Clear GEN_CALL Interrupt Register,                 Address offset: 0x68 */
    __IO uint32_t ENABLE;                   /*!< ENABLE Register,                                   Address offset: 0x6C */
    __IO uint32_t STATUS;                   /*!< STATUS Register,                                   Address offset: 0x70 */
    __IO uint32_t TXFLR;                    /*!< Tx FIFO Level Register,                            Address offset: 0x74 */
    __IO uint32_t RXFLR;                    /*!< Rx FIFO Level Register,                            Address offset: 0x78 */
    __IO uint32_t SDA_HOLD;                 /*!< SDA Hold Time Length Register,                     Address offset: 0x7C */
    __IO uint32_t TX_ABRT_SOURCE;           /*!< Tx Abort Source Register,                          Address offset: 0x80 */
    uint32_t RESERVED0;                     /*!<                                                    Address offset: 0x84 */
    __IO uint32_t DMA_CR;                   /*!< DMA Control Register,                              Address offset: 0x88 */
    __IO uint32_t DMA_TDLR;                 /*!< DMA Tx Data Level Register,                        Address offset: 0x8C */
    __IO uint32_t DMA_RDLR;                 /*!< DMA Rx Data Level Register,                        Address offset: 0x90 */
    __IO uint32_t SDA_SETUP;                /*!< SDA Setup Register,                                Address offset: 0x94 */
    __IO uint32_t ACK_GENERAL_CALL;         /*!< ACK General Call Register,                         Address offset: 0x98 */
    __IO uint32_t ENABLE_STATUS;            /*!< Enable Status Register,                            Address offset: 0x9C */
    __IO uint32_t FS_SPKLEN;                /*!< SS, FS, or FM Spike Suppression Limit Register,    Address offset: 0xA0 */
    __IO uint32_t HS_SPKLEN;                /*!< HS Spike Suppression Limit Register,               Address offset: 0xA4 */
    uint32_t RESERVED1;                     /*!<                                                    Address offset: 0xA8 */
    __IO uint32_t SCL_STUCK_LT;             /*!< SLC Stuck at Low Timeout Register,                 Address offset: 0xAC */
    __IO uint32_t SDA_STUCK_LT;             /*!< SDA Stuck at Low Timeout Register,                 Address offset: 0xB0 */
    __IO uint32_t CLR_SCL_STUCK_DET;        /*!< Clear SCL Stuck at Low Detect Interrupt Register,  Address offset: 0xB4 */
    uint32_t RESERVED2[14];                 /*!<                                                    Address offset: 0xB8 */
    __IO uint32_t REG_TIMEOUT_RST;          /*!< Register Timeout Counter Reset Value,              Address offset: 0xF0 */
    __IO uint32_t COMP_PARAM_1;             /*!< Component Parameter Register 1,                    Address offset: 0xF4 */
    __IO uint32_t COMP_VERSION;             /*!< Component Version Register,                        Address offset: 0xF8 */
    __IO uint32_t COMP_TYPE;                /*!< Component Type Register,                           Address offset: 0xFC */
} I2C_TypeDef;

/**
 * @brief Inter Integrated Circuit Sound Interface
 */
typedef struct
{
    __IO uint32_t IER;                      /*!< Enable Register,                                   Address offset: 0x00 */
    __IO uint32_t IRER;                     /*!< Receiver Block Enable Register,                    Address offset: 0x04 */
    __IO uint32_t ITER;                     /*!< Transmitter Block Enable Register,                 Address offset: 0x08 */
    uint32_t RESERVED0[2];                  /*!<                                                    Address offset: 0x0C */
    __IO uint32_t RXFFR;                    /*!< Receiver Block FIFO Reset Register,                Address offset: 0x14 */
    __IO uint32_t TXFFR;                    /*!< Transmitter Block FIFO Reset Register,             Address offset: 0x18 */
    uint32_t RESERVED1;                     /*!<                                                    Address offset: 0x1C */
    union
    {
        __IO uint32_t LRBR0;                /*!< Left Receive Buffer Register 0,                    Address offset: 0x20 */
        __IO uint32_t LTHR0;                /*!< Left Transmit Holding Register 0,                  Address offset: 0x20 */
    };
    union
    {
        __IO uint32_t RRBR0;                /*!< Right Receive Buffer Register 0,                   Address offset: 0x24 */
        __IO uint32_t RTHR0;                /*!< Right Transmit Holding Register 0,                 Address offset: 0x24 */
    };
    __IO uint32_t RER0;                     /*!< Receive Enable Register 0,                         Address offset: 0x28 */
    __IO uint32_t TER0;                     /*!< Transmit Enable Register 0,                        Address offset: 0x2C */
    __IO uint32_t RCR0;                     /*!< Receive Configuration Register 0,                  Address offset: 0x30 */
    __IO uint32_t TCR0;                     /*!< Transmit Configuration Register 0,                 Address offset: 0x34 */
    __IO uint32_t ISR0;                     /*!< Interrupt Status Register 0,                       Address offset: 0x38 */
    __IO uint32_t IMR0;                     /*!< Interrupt Mask Register 0,                         Address offset: 0x3C */
    __IO uint32_t ROR0;                     /*!< Receive Overrun Register 0,                        Address offset: 0x40 */
    __IO uint32_t TOR0;                     /*!< Transmit Overrun Register 0,                       Address offset: 0x44 */
    __IO uint32_t RFCR0;                    /*!< Receive FIFO Configuration Register 0,             Address offset: 0x48 */
    __IO uint32_t TFCR0;                    /*!< Transmit FIFO Configuration Register 0,            Address offset: 0x4C */
    __IO uint32_t RFF0;                     /*!< Receive FIFO Flush Register 0,                     Address offset: 0x50 */
    __IO uint32_t TFF0;                     /*!< Transmit FIFO Flush Register 0,                    Address offset: 0x54 */
    uint32_t RESERVED2[90];                 /*!<                                                    Address offset: 0x58 */
    __IO uint32_t RXDMA;                    /*!< Receiver Block DMA Register,                       Address offset: 0x1C0 */
    uint32_t RESERVED3;                     /*!<                                                    Address offset: 0x1C4 */
    __IO uint32_t TXDMA;                    /*!< Transmitter Block DMA Register,                    Address offset: 0x1C8 */
    uint32_t RESERVED4[9];                  /*!<                                                    Address offset: 0x1CC */
    __IO uint32_t COMP_PARAM_2;             /*!< Component Parameter Register 2,                    Address offset: 0x1F0 */
    __IO uint32_t COMP_PARAM_1;             /*!< Component Parameter Register 1,                    Address offset: 0x1F4 */
    __IO uint32_t COMP_VERSION;             /*!< Component Version Register,                        Address offset: 0x1F8 */
    __IO uint32_t COMP_TYPE;                /*!< Component Type Register,                           Address offset: 0x1FC */
    __IO uint32_t DMACR;                    /*!< DMA Control Register,                              Address offset: 0x200 */
} I2S_TypeDef;

/**
 * @brief Quad Serial Peripheral Interface
 */
typedef struct {
	__IO uint32_t CTRLR0;          /* Control 0                                Address offset: 0x00 */
	__IO uint32_t CTRLR1;          /* Control 1                                Address offset: 0x04 */
	__IO uint32_t QSPIENR;         /* QSPI Enable                              Address offset: 0x08 */
	__IO uint32_t MWCR;            /* Microwire Control                        Address offset: 0x0c */
	__IO uint32_t SER;             /* Slave Enable                             Address offset: 0x10 */
	__IO uint32_t BAUDR;           /* Baud Rate Select                         Address offset: 0x14 */
	__IO uint32_t TXFTLR;          /* Transmit FIFO Threshold Level            Address offset: 0x18 */
	__IO uint32_t RXFTLR;          /* Receive FIFO Threshold Level             Address offset: 0x1c */
	__IO uint32_t TXFLR;           /* Transmit FIFO Level                      Address offset: 0x20 */
	__IO uint32_t RXFLR;           /* Receive FIFO Level                       Address offset: 0x24 */
	__IO uint32_t SR;              /* Status                                   Address offset: 0x28 */
	__IO uint32_t IMR;             /* Interrupt Mask                           Address offset: 0x2c */
	__IO uint32_t ISR;             /* Interrupt Status                         Address offset: 0x30 */
	__IO uint32_t RISR;            /* Raw Interrupt Status                     Address offset: 0x34 */
	__IO uint32_t TXOICR;          /* Transmit FIFO Overflow Interrupt Clear   Address offset: 0x38 */
	__IO uint32_t RXOICR;          /* Receive FIFO Overflow Interrupt Clear    Address offset: 0x3c */
	__IO uint32_t RXUICR;          /* Receive FIFO Underflow Interrupt Clear   Address offset: 0x40 */
	__IO uint32_t MSTICR;          /* Multi-Master Interrupt Clear             Address offset: 0x44 */
	__IO uint32_t ICR;             /* Interrupt Clear                          Address offset: 0x48 */
	__IO uint32_t DMACR;           /* DMA Control                              Address offset: 0x4c */
	__IO uint32_t DMATDLR;         /* DMA Transmit Data Level                  Address offset: 0x50 */
	__IO uint32_t DMARDLR;         /* DMA Receive Data Level                   Address offset: 0x54 */
	__IO uint32_t IDR;             /* Identification                           Address offset: 0x58 */
	__IO uint32_t VER_ID;          /* Version ID                               Address offset: 0x5c */
	__IO uint32_t DR[36];          /* Data x                                   Address offset: 0x60 */
	__IO uint32_t RX_SAMPLE_DLY;   /* RX Sample Delay                          Address offset: 0xf0 */
	__IO uint32_t SPI_CTRLR0;      /* SPI Control                              Address offset: 0xf4 */
	__IO uint32_t TXD_DRIVE_EDGE;  /* Transmit Drive Edge                      Address offset: 0xf8 */
} QSPI_TypeDef;

/**
 * @brief Serial Peripheral Interface (SPI)
 */
typedef struct {
    __IO uint32_t CTRLR0;          /* Control 0                                Address offset: 0x00 */
	__IO uint32_t CTRLR1;          /* Control 1                                Address offset: 0x04 */
	__IO uint32_t SPIENR;          /* SPI Enable                               Address offset: 0x08 */
	__IO uint32_t MWCR;            /* Microwire Control                        Address offset: 0x0c */
	__IO uint32_t SER;             /* Slave Enable                             Address offset: 0x10 */
	__IO uint32_t BAUDR;           /* Baud Rate Select                         Address offset: 0x14 */
	__IO uint32_t TXFTLR;          /* Transmit FIFO Threshold Level            Address offset: 0x18 */
	__IO uint32_t RXFTLR;          /* Receive FIFO Threshold Level             Address offset: 0x1c */
	__IO uint32_t TXFLR;           /* Transmit FIFO Level                      Address offset: 0x20 */
	__IO uint32_t RXFLR;           /* Receive FIFO Level                       Address offset: 0x24 */
	__IO uint32_t SR;              /* Status                                   Address offset: 0x28 */
	__IO uint32_t IMR;             /* Interrupt Mask                           Address offset: 0x2c */
	__IO uint32_t ISR;             /* Interrupt Status                         Address offset: 0x30 */
	__IO uint32_t RISR;            /* Raw Interrupt Status                     Address offset: 0x34 */
	__IO uint32_t TXOICR;          /* Transmit FIFO Overflow Interrupt Clear   Address offset: 0x38 */
	__IO uint32_t RXOICR;          /* Receive FIFO Overflow Interrupt Clear    Address offset: 0x3c */
	__IO uint32_t RXUICR;          /* Receive FIFO Underflow Interrupt Clear   Address offset: 0x40 */
	__IO uint32_t MSTICR;          /* Multi-Master Interrupt Clear             Address offset: 0x44 */
	__IO uint32_t ICR;             /* Interrupt Clear                          Address offset: 0x48 */
	__IO uint32_t DMACR;           /* DMA Control                              Address offset: 0x4c */
	__IO uint32_t DMATDLR;         /* DMA Transmit Data Level                  Address offset: 0x50 */
	__IO uint32_t DMARDLR;         /* DMA Receive Data Level                   Address offset: 0x54 */
	__IO uint32_t IDR;             /* Identification                           Address offset: 0x58 */
	__IO uint32_t VERSION_ID;      /* Version ID                               Address offset: 0x5c */
	__IO uint32_t DR[36];          /* Data x                                   Address offset: 0x60 */
	__IO uint32_t RX_SAMPLE_DLY;   /* RX Sample Delay                          Address offset: 0xf0 */
	__IO uint32_t SPI_CTRLR0;      /* SPI Control                              Address offset: 0xf4 */
	__IO uint32_t TXD_DRIVE_EDGE;  /* Transmit Drive Edge                      Address offset: 0xf8 */
} SPI_TypeDef;

/**
 * @brief Universal Asynchronous Receiver/Transmitter
 */
typedef struct
{
    union
    {
        __IO uint32_t RBR;          /*!< Receive Buffer Register,               Address offset: 0x00 */
        __IO uint32_t THR;          /*!< Transmit Holding Register,             Address offset: 0x00 */
        __IO uint32_t DLL;          /*!< Divisor Latch (Low),                   Address offset: 0x00 */
    };
    union
    {
        __IO uint32_t IER;          /*!< Interrupt Enable Register,             Address offset: 0x04 */
        __IO uint32_t DLH;          /*!< Divisor Latch High,                    Address offset: 0x04 */
    };
    union
    {
        __IO uint32_t IIR;          /*!< Interrupt Identification Register,     Address offset: 0x08 */
        __IO uint32_t FCR;          /*!< FIFO Control Register,                 Address offset: 0x08 */
    };
    __IO uint32_t LCR;              /*!< Line Control Register,                 Address offset: 0x0C */
    __IO uint32_t MCR;              /*!< Modem Control Register,                Address offset: 0x10 */
    __IO uint32_t LSR;              /*!< Line Status Register,                  Address offset: 0x14 */
    __IO uint32_t MSR;              /*!< Modem Status Register,                 Address offset: 0x18 */
    __IO uint32_t SCR;              /*!< Scratchpad Register,                   Address offset: 0x1C */
    uint32_t RESERVED0[4];          /*!<                                        Address offset: 0x20 */
    union
    {
        __IO uint32_t SRBR[16];     /*!< Shadow Receive Buffer Register,        Address offset: 0x30 + n * 0x04 */
        __IO uint32_t STHR[16];     /*!< Shadow Transmit Holding Register,      Address offset: 0x30 + n * 0x04 */
    };
    __IO uint32_t FAR;              /*!< FIFO Access Register,                  Address offset: 0x70 */
    __IO uint32_t TFR;              /*!< Transmit FIFO Read,                    Address offset: 0x74 */
    __IO uint32_t RFW;              /*!< Receive FIFO Write,                    Address offset: 0x78 */
    __IO uint32_t USR;              /*!< UART Status Register,                  Address offset: 0x7C */
    __IO uint32_t TFL;              /*!< Transmit FIFO Level,                   Address offset: 0x80 */
    __IO uint32_t RFL;              /*!< Receive FIFO Level,                    Address offset: 0x84 */
    __IO uint32_t SRR;              /*!< Software Reset Register,               Address offset: 0x88 */
    __IO uint32_t SRTS;             /*!< Shadow Request to Send,                Address offset: 0x8C */
    __IO uint32_t SBCR;             /*!< Shadow Break Control Register,         Address offset: 0x90 */
    __IO uint32_t SDMAM;            /*!< Shadow DMA Mode Register,              Address offset: 0x94 */
    __IO uint32_t SFE;              /*!< Shadow FIFO Enable Register,           Address offset: 0x98 */
    __IO uint32_t SRT;              /*!< Shadow RCVR Trigger Register,          Address offset: 0x9C */
    __IO uint32_t STET;             /*!< Shadow TX Empty Trigger Register,      Address offset: 0xA0 */
    __IO uint32_t HTX;              /*!< Halt TX,                               Address offset: 0xA4 */
    __IO uint32_t DMASA;            /*!< DMA Software Acknowledge Register,     Address offset: 0xA8 */
    __IO uint32_t TCR;              /*!< Transceiver Control Register,          Address offset: 0xAC */
    __IO uint32_t DE_EN;            /*!< Driver Output Enable Register,         Address offset: 0xB0 */
    __IO uint32_t RE_EN;            /*!< Receiver Output Enable Register,       Address offset: 0xB4 */
    __IO uint32_t DET;              /*!< Driver Output Enable Timing Register,  Address offset: 0xB8 */
    __IO uint32_t TAT;              /*!< TurnAround Timing Register,            Address offset: 0xBC */
    __IO uint32_t DLF;              /*!< Divisor Latch Fraction Register,       Address offset: 0xC0 */
    __IO uint32_t RAR;              /*!< Receive Address Register,              Address offset: 0xC4 */
    __IO uint32_t TAR;              /*!< Transmit Address Register,             Address offset: 0xC8 */
    __IO uint32_t LCR_EXT;          /*!< Line Extended Control Register,        Address offset: 0xCC */
    __IO uint32_t PROT_LEVEL;       /*!< UART Protection Level,                 Address offset: 0xD0 */
    __IO uint32_t REG_TIMEOUT_RST;  /*!< Register Timeout Counter Reset Value,  Address offset: 0xD4 */
    uint32_t RESERVED1[7];          /*!<                                        Address offset: 0xD8 */
    __IO uint32_t CPR;              /*!< Component Parameter Register,          Address offset: 0xF4 */
    __IO uint32_t UCV;              /*!< UART Component Version,                Address offset: 0xF8 */
    __IO uint32_t CTR;              /*!< Component Type Register,               Address offset: 0xFC */
} UART_TypeDef;

/**
 * @brief USB Common registers
 */
typedef struct __PACKED
{
    __IO uint8_t FADDR;             /*!< Function address register,                     Address offset: 0x00 */
    __IO uint8_t POWER;             /*!< Power management register,                     Address offset: 0x01 */
    __IO uint16_t INTRTX;           /*!< Interrupt register for EP 0 plus EPs 1 to 15,  Address offset: 0x02 */
    __IO uint16_t INTRRX;           /*!< Interrupt register for Rx EPs 1 to 15,         Address offset: 0x04 */
    __IO uint16_t INTRTXE;          /*!< Interrupt enable register for IntrTx,          Address offset: 0x06 */
    __IO uint16_t INTRRXE;          /*!< Interrupt enable register for IntrRx,          Address offset: 0x08 */
    __IO uint8_t INTRUSB;           /*!< Interrupt register for common USB interrupts,  Address offset: 0x0A */
    __IO uint8_t INTRUSBE;          /*!< Interrupt enable register for IntrUSB,         Address offset: 0x0B */
    __IO uint16_t FRAME;            /*!< Frame number,                                  Address offset: 0x0C */
    __IO uint8_t INDEX;             /*!< Index register for selecting EP CSR,           Address offset: 0x0E */
    __IO uint8_t TESTMODE;          /*!< Enables the USB2.0 test modes,                 Address offset: 0x0F */
} USB_Common_TypeDef;

/**
 * @brief USB Endpoint Control and Status Registers - Peripheral mode
 */
typedef struct __PACKED
{
    __IO uint16_t TXMAXP;           /*!< Maximum packet size for peripheral Tx EP,                          Address offset: 0x10 */
    union
    {
        __IO uint8_t CSR0L;         /*!< Control Status register for EP 0 Low,                              Address offset: 0x12 */
        __IO uint8_t TXCSRL;        /*!< Control Status register for peripheral Tx EP Low,                  Address offset: 0x12 */
    };
    union
    {
        __IO uint8_t CSR0H;         /*!< Control Status register for EP 0 High,                             Address offset: 0x13 */
        __IO uint8_t TXCSRH;        /*!< Control Status register for peripheral Tx EP High,                 Address offset: 0x13 */
    };
    __IO uint16_t RXMAXP;           /*!< Maximum packet size for peripheral Rx EP,                          Address offset: 0x14 */
    __IO uint8_t RXCSRL;            /*!< Control Status register for peripheral Rx EP Low,                  Address offset: 0x16 */
    __IO uint8_t RXCSRH;            /*!< Control Status register for peripheral Rx EP High,                 Address offset: 0x17 */
    union
    {
        __IO uint8_t COUNT0;        /*!< Number of received bytes in EP 0 FIFO,                             Address offset: 0x18 */
        __IO uint16_t RXCOUNT;      /*!< Number of bytes to be read from peripheral Rx EP FIFO,             Address offset: 0x18 */
    };
    uint8_t RESERVED0[2];           /*!< Reserved,                                                          Address offset: 0x1A */
    uint8_t UNUSED0[3];             /*!< Unused,                                                            Address offset: 0x1C */
    union
    {
    __IO uint8_t CONFIGDATA;        /*!< Returns details of core configuration,                             Address offset: 0x1F */
    __IO uint8_t FIFOSIZE;          /*!< Returns the configured size of the selected Rx FIFO and Tx FIFOs,  Address offset: 0x1F */
    };
} USB_EPCSR_Periph_TypeDef;

/**
 * @brief USB Endpoint Control and Status Registers - Host mode
 */
typedef struct __PACKED
{
    __IO uint16_t TXMAXP;           /*!< Maximum packet size for host Tx EP,                                Address offset: 0x10 */
    union
    {
        __IO uint8_t CSR0L;         /*!< Control Status register for EP 0 Low,                              Address offset: 0x12 */
        __IO uint8_t TXCSRL;        /*!< Control Status register for host Tx EP Low,                        Address offset: 0x12 */
    };
    union
    {
        __IO uint8_t CSR0H;         /*!< Control Status register for EP 0 High,                             Address offset: 0x13 */
        __IO uint8_t TXCSRH;        /*!< Control Status register for host Tx EP High,                       Address offset: 0x13 */
    };
    __IO uint16_t RXMAXP;           /*!< Maximum packet size for host Rx EP,                                Address offset: 0x14 */
    __IO uint8_t RXCSRL;            /*!< Control Status register for host Rx EP Low,                        Address offset: 0x16 */
    __IO uint8_t RXCSRH;            /*!< Control Status register for host Rx EP High,                       Address offset: 0x17 */
    union
    {
        __IO uint8_t COUNT0;        /*!< Number of received bytes in EP 0 FIFO,                             Address offset: 0x18 */
        __IO uint16_t RXCOUNT;      /*!< Number of bytes to be read from host Rx EP FIFO,                   Address offset: 0x18 */
    };
    union
    {
        __IO uint8_t TYPE0;         /*!< Defines the speed of EP 0,                                         Address offset: 0x1A */
        __IO uint8_t TXTYPE;        /*!< Sets protocol, speed and peripheral EP number for the host Tx EP,  Address offset: 0x1A */
    };
    union
    {
        __IO uint8_t NAKLIMIT0;     /*!< Sets NAK response timeout on EP 0,                                 Address offset: 0x1B */
        __IO uint8_t TXINTERVAL;    /*!< Sets polling interval or NAK response timeout for host Tx EP,      Address offset: 0x1B */
    };
    __IO uint8_t RXTYPE;            /*!< Sets protocol, speed and peripheral EP number for the host Rx EP,  Address offset: 0x1C */
    __IO uint8_t RXINTERVAL;        /*!< Sets polling interval or NAK response timeout for host Rx EP,      Address offset: 0x1D */
    uint8_t UNUSED0;                /*!< Unused,                                                            Address offset: 0x1E */
    union
    {
    __IO uint8_t CONFIGDATA;        /*!< Returns details of core configuration,                             Address offset: 0x1F */
    __IO uint8_t FIFOSIZE;          /*!< Returns the configured size of the selected Rx FIFO and Tx FIFOs,  Address offset: 0x1F */
    };
} USB_EPCSR_Host_TypeDef;

/**
 * @brief USB Endpoint Control and Status Registers
 */
typedef union __PACKED
{
    USB_EPCSR_Periph_TypeDef PERIPH;
    USB_EPCSR_Host_TypeDef HOST;
} USB_EPCSR_TypeDef;

/**
 * @brief USB ULPI PHY Registers
 */
typedef struct __PACKED
{
    __IO uint8_t ULPIVBUSCONTROL;   /*!< VBus control,                                  Address offset: 0x70 */
    __IO uint8_t ULPICARKITCONTROL; /*!< CarKit system control,                         Address offset: 0x71 */
    __IO uint8_t ULPIINTMASK;       /*!< Interrupt enable,                              Address offset: 0x72 */
    __IO uint8_t ULPIINTSRC;        /*!< Interrupt source,                              Address offset: 0x73 */
    __IO uint8_t ULPIREGDATA;       /*!< Data associated with register read/writes,     Address offset: 0x74 */
    __IO uint8_t ULPIREGADDR;       /*!< Address of the register being read/written,    Address offset: 0x75 */
    __IO uint8_t ULPIREGCONTROL;    /*!< Control and status bits,                       Address offset: 0x76 */
    __IO uint8_t ULPIRAWDATA;       /*!< Asynchronous and synchronous mode data,        Address offset: 0x77 */
} USB_ULPI_TypeDef;

/**
 * @brief USB Additional Control & Configuration Registers
 */
typedef struct __PACKED
{
    __IO uint8_t DEVCTL;            /*!< OTG device control register,                           Address offset: 0x60 */
    __IO uint8_t MISC;              /*!< Miscellanious register,                                Address offset: 0x61 */
    __IO uint8_t TXFIFOSZ;          /*!< Tx EP FIFO size,                                       Address offset: 0x62 */
    __IO uint8_t RXFIFOSZ;          /*!< Rx EP FIFO size,                                       Address offset: 0x63 */
    __IO uint16_t TXFIFOADD;        /*!< Tx EP FIFO address,                                    Address offset: 0x64 */
    __IO uint16_t RXFIFOADD;        /*!< Rx EP FIFO address,                                    Address offset: 0x66 */
    union
    {
        __IO uint32_t VCONTROL;     /*!< UTMI + PHY vendor registers,                           Address offset: 0x68 */
        __IO uint32_t VSTATUS;      /*!< UTMI + PHY vendor registers,                           Address offset: 0x68 */
    };
    __IO uint16_t HWVERS;           /*!< Hardware version number register,                      Address offset: 0x6C */
    uint8_t UNUSED0[2];             /*!< Unused,                                                Address offset: 0x6E */
    USB_ULPI_TypeDef ULPI;          /*!< ULPI registers,                                        Address offset: 0x70 */
    __IO uint8_t EPINFO;            /*!< Numbers of Tx and Rx EPs,                              Address offset: 0x78 */
    __IO uint8_t RAMINFO;           /*!< The width of RAM and the number of DMA channels,       Address offset: 0x79 */
    __IO uint8_t LINKINFO;          /*!< Delays to be applied,                                  Address offset: 0x7A */
    __IO uint8_t VPLEN;             /*!< Duration of the VBus pulsing charge,                   Address offset: 0x7B */
    __IO uint8_t HS_EOF1;           /*!< Time buffer available on HS transactions,              Address offset: 0x7C */
    __IO uint8_t FS_EOF1;           /*!< Time buffer available on FS transactions,              Address offset: 0x7D */
    __IO uint8_t LS_EOF1;           /*!< Time buffer available on LS transactions,              Address offset: 0x7E */
    __IO uint8_t SOFT_RST;          /*!< Soft reset,                                            Address offset: 0x7F */
} USB_Ctrl_TypeDef;

/**
 * @brief USB Target Address Registers
 */
typedef struct __PACKED
{
    __IO uint8_t TXFUNCADDR;        /*!< Tx EP n function address (Host mode only),             Address offset: 0x80+8*n */
    uint8_t UNUSED0;                /*!< Unused,                                                Address offset: 0x81+8*n */
    __IO uint8_t TXHUBADDR;         /*!< Tx EP n hub address (Host mode only),                  Address offset: 0x82+8*n */
    __IO uint8_t TXHUBPORT;         /*!< Tx EP n hub port (Host mode only),                     Address offset: 0x83+8*n */
    __IO uint8_t RXFUNCADDR;        /*!< Rx EP n function address (Host mode only),             Address offset: 0x84+8*n */
    uint8_t UNUSED1;                /*!< Unused,                                                Address offset: 0x85+8*n */
    __IO uint8_t RXHUBADDR;         /*!< Rx EP n hub address (Host mode only),                  Address offset: 0x86+8*n */
    __IO uint8_t RXHUBPORT;         /*!< Rx EP n hub port (Host mode only),                     Address offset: 0x87+8*n */
} USB_TargetAddr_TypeDef;

/**
 * @brief USB DMA Registers
 */
typedef struct __PACKED
{
    __IO uint8_t DMA_INTR;          /*!< DMA interrupt register,                Address offset: 0x200 */
    uint8_t RESERVED0[3];           /*!< Reserved,                              Address offset: 0x201 */
    struct __PACKED
    {
        __IO uint32_t DMA_CNTL;     /*!< DMA control register for channel n,    Address offset: 0x204+(n-1)*10h */
        __IO uint32_t DMA_ADDR;     /*!< DMA address register for channel n,    Address offset: 0x208+(n-1)*10h */
        __IO uint32_t DMA_COUNT;    /*!< DMA count register for channel n,      Address offset: 0x20C+(n-1)*10h */
        uint32_t RESERVED0;         /*!< Reserved,                              Address offset: 0x210+(n-1)*10h */
    } CH[8];
} USB_DMA_TypeDef;

/**
 * @brief USB Extended Registers
 */
typedef struct __PACKED
{
    __IO uint16_t EP1_RQPKTCOUNT;   /*!< Number of requested packets for Rx EP 1,                   Address offset: 0x304 */
    uint16_t RESERVED0;             /*!< Reserved,                                                  Address offset: 0x306 */
    __IO uint16_t EP2_RQPKTCOUNT;   /*!< Number of requested packets for Rx EP 2,                   Address offset: 0x308 */
    uint16_t RESERVED1;             /*!< Reserved,                                                  Address offset: 0x30A */
    __IO uint16_t EP3_RQPKTCOUNT;   /*!< Number of requested packets for Rx EP 3,                   Address offset: 0x30C */
    uint16_t RESERVED2;             /*!< Reserved,                                                  Address offset: 0x30E */
    __IO uint16_t EP4_RQPKTCOUNT;   /*!< Number of requested packets for Rx EP 4,                   Address offset: 0x310 */
    uint16_t RESERVED3;             /*!< Reserved,                                                  Address offset: 0x312 */
    __IO uint16_t EP5_RQPKTCOUNT;   /*!< Number of requested packets for Rx EP 5,                   Address offset: 0x314 */
    uint16_t RESERVED4;             /*!< Reserved,                                                  Address offset: 0x316 */
    __IO uint16_t EP6_RQPKTCOUNT;   /*!< Number of requested packets for Rx EP 6,                   Address offset: 0x318 */
    uint16_t RESERVED5;             /*!< Reserved,                                                  Address offset: 0x31A */
    __IO uint16_t EP7_RQPKTCOUNT;   /*!< Number of requested packets for Rx EP 7,                   Address offset: 0x31C */
    uint16_t RESERVED6;             /*!< Reserved,                                                  Address offset: 0x31E */
    __IO uint16_t EP8_RQPKTCOUNT;   /*!< Number of requested packets for Rx EP 8,                   Address offset: 0x320 */
    uint16_t RESERVED7;             /*!< Reserved,                                                  Address offset: 0x322 */
    __IO uint16_t EP9_RQPKTCOUNT;   /*!< Number of requested packets for Rx EP 9,                   Address offset: 0x324 */
    uint16_t RESERVED8;             /*!< Reserved,                                                  Address offset: 0x326 */
    __IO uint16_t EP10_RQPKTCOUNT;  /*!< Number of requested packets for Rx EP 10,                  Address offset: 0x328 */
    uint16_t RESERVED9;             /*!< Reserved,                                                  Address offset: 0x32A */
    __IO uint16_t EP11_RQPKTCOUNT;  /*!< Number of requested packets for Rx EP 11,                  Address offset: 0x32C */
    uint16_t RESERVED10;            /*!< Reserved,                                                  Address offset: 0x32E */
    __IO uint16_t EP12_RQPKTCOUNT;  /*!< Number of requested packets for Rx EP 12,                  Address offset: 0x330 */
    uint16_t RESERVED11;            /*!< Reserved,                                                  Address offset: 0x332 */
    __IO uint16_t EP13_RQPKTCOUNT;  /*!< Number of requested packets for Rx EP 13,                  Address offset: 0x334 */
    uint16_t RESERVED12;            /*!< Reserved,                                                  Address offset: 0x336 */
    __IO uint16_t EP14_RQPKTCOUNT;  /*!< Number of requested packets for Rx EP 14,                  Address offset: 0x338 */
    uint16_t RESERVED13;            /*!< Reserved,                                                  Address offset: 0x33A */
    __IO uint16_t EP15_RQPKTCOUNT;  /*!< Number of requested packets for Rx EP 15,                  Address offset: 0x33C */
    uint16_t RESERVED14;            /*!< Reserved,                                                  Address offset: 0x33E */
    __IO uint16_t RXDPKTBUFDIS;     /*!< Double Packet Buffer Disable register for Rx EPs 1 to 15,  Address offset: 0x340 */
    __IO uint16_t TXDPKTBUFDIS;     /*!< Double Packet Buffer Disable register for Tx EPs 1 to 15,  Address offset: 0x342 */
    __IO uint16_t C_T_UCH;          /*!< Chirp Timeout Timer,                                       Address offset: 0x344 */
    __IO uint16_t C_T_HSRTN;        /*!< Delay from the end of HS resume signal,                    Address offset: 0x346 */
} USB_Ext_TypeDef;

/**
 * @brief USB LPM Registers
 */
typedef struct __PACKED
{
    __IO uint16_t LPM_ATTR;         /*!< LPM Attribute register,                Address offset: 0x360 */
    __IO uint8_t LPM_CNTRL;         /*!< LPM Control register,                  Address offset: 0x362 */
    __IO uint8_t LPM_INTREN;        /*!< LPM Interrupt Enable register,         Address offset: 0x363 */
    __IO uint8_t LPM_INTR;          /*!< LPM Interrupt register,                Address offset: 0x364 */
    __IO uint8_t LPM_FADDR;         /*!< LPM Function Address register,         Address offset: 0x365 */
} USB_LPM_TypeDef;

/**
 * @brief USB
 */
typedef struct __PACKED
{
    USB_Common_TypeDef COMMON;
    USB_EPCSR_TypeDef INDEXED_EPCSR;
    __IO uint32_t FIFO[16] __ALIGNED(4);
    USB_Ctrl_TypeDef CTRL;
    USB_TargetAddr_TypeDef TADDR[16];
    USB_EPCSR_TypeDef EPCSR[16];
    USB_DMA_TypeDef DMA;
    uint8_t RESERVED0[128];
    USB_Ext_TypeDef EXT;
    uint8_t RESERVED1[24];
    USB_LPM_TypeDef LPM;
} USB_TypeDef;

/**
 * @brief TIM
 */
typedef struct
{
    __IO uint32_t TIMER0LOADCOUNT;          /*!< Timer 0 Load Count Register,                       Address offset: 0x00 */
    __IO uint32_t TIMER0CURRENTVAL;         /*!< Timer 0 Current Value Register,                    Address offset: 0x04 */
    __IO uint32_t TIMER0CONTROLREG;         /*!< Timer 0 Control Register,                          Address offset: 0x08 */
    __IO uint32_t TIMER0EOI;                /*!< Timer 0 End Of Interrupt Register,                 Address offset: 0x0c */
    __IO uint32_t TIMER0INTSTAT;            /*!< Timer 0 Interrupt Status Register,                 Address offset: 0x10 */
    __IO uint32_t TIMER1LOADCOUNT;          /*!< Timer 1 Load Count Register,                       Address offset: 0x14 */
    __IO uint32_t TIMER1CURRENTVAL;         /*!< Timer 1 Current Value Register,                    Address offset: 0x18 */
    __IO uint32_t TIMER1CONTROLREG;         /*!< Timer 1 Control Register,                          Address offset: 0x1c */
    __IO uint32_t TIMER1EOI;                /*!< Timer 1 End Of Interrupt Register,                 Address offset: 0x20 */
    __IO uint32_t TIMER1INTSTAT;            /*!< Timer 1 Interrupt Status Register,                 Address offset: 0x24 */
    __IO uint32_t TIMER2LOADCOUNT;          /*!< Timer 2 Load Count Register,                       Address offset: 0x28 */
    __IO uint32_t TIMER2CURRENTVAL;         /*!< Timer 2 Current Value Register,                    Address offset: 0x2c */
    __IO uint32_t TIMER2CONTROLREG;         /*!< Timer 2 Control Register,                          Address offset: 0x30 */
    __IO uint32_t TIMER2EOI;                /*!< Timer 2 End Of Interrupt Register,                 Address offset: 0x34 */
    __IO uint32_t TIMER2INTSTAT;            /*!< Timer 2 Interrupt Status Register,                 Address offset: 0x38 */
    __IO uint32_t TIMER3LOADCOUNT;          /*!< Timer 3 Load Count Register,                       Address offset: 0x3c */
    __IO uint32_t TIMER3CURRENTVAL;         /*!< Timer 3 Current Value Register,                    Address offset: 0x40 */
    __IO uint32_t TIMER3CONTROLREG;         /*!< Timer 3 Control Register,                          Address offset: 0x44 */
    __IO uint32_t TIMER3EOI;                /*!< Timer 3 End Of Interrupt Register,                 Address offset: 0x48 */
    __IO uint32_t TIMER3INTSTAT;            /*!< Timer 3 Interrupt Status Register,                 Address offset: 0x4c */
    uint32_t      RESERVED0[20];            /*!< Reserved,                                          Address offset: 0x50 */
    __IO uint32_t TIMERSINTSTATUS;          /*!< Timers Interrupt Status Register,                  Address offset: 0xa0 */
    __IO uint32_t TIMERSEOI;                /*!< Timers End-of-Interrupt Register,                  Address offset: 0xa4 */
    __IO uint32_t TIMERSRAWINTSTAT;         /*!< Timers Raw Interrupt Status Register,              Address offset: 0xa8 */
    __IO uint32_t TIMERSCOMPVERSION;        /*!< Timers Component Version,                          Address offset: 0xac */
    __IO uint32_t TIMER0LOADCOUNT2;         /*!< Timer 0 Load Count2 Register,                      Address offset: 0xb0 */
    __IO uint32_t TIMER1LOADCOUNT2;         /*!< Timer 1 Load Count2 Register,                      Address offset: 0xb4 */
    __IO uint32_t TIMER2LOADCOUNT2;         /*!< Timer 2 Load Count2 Register,                      Address offset: 0xb8 */
    __IO uint32_t TIMER3LOADCOUNT2;         /*!< Timer 3 Load Count2 Register,                      Address offset: 0xbc */
} TIM_TypeDef;

/**
 * @brief PWMA
 */
typedef struct
{
    __IO uint32_t CR1;                      /*!< Control register 1,                                Address offset: 0x00 */
    __IO uint32_t CR2;                      /*!< Control register 2,                                Address offset: 0x04 */
    __IO uint32_t SMCR;                     /*!< Slave mode control register,                       Address offset: 0x08 */
    __IO uint32_t DIER;                     /*!< DMA/Interrupt enable register,                     Address offset: 0x0C */
    __IO uint32_t SR;                       /*!< Status register,                                   Address offset: 0x10 */
    __IO uint32_t EGR;                      /*!< Event generation register,                         Address offset: 0x14 */
    __IO uint32_t CCMR1;                    /*!< Capture/Compare mode register 1,                   Address offset: 0x18 */
    __IO uint32_t CCMR2;                    /*!< Capture/Compare mode register 2,                   Address offset: 0x1C */
    __IO uint32_t CCER;                     /*!< Capture/compare enable register,                   Address offset: 0x20 */
    __IO uint32_t CNT;                      /*!< Counter,                                           Address offset: 0x24 */
    __IO uint32_t PSC;                      /*!< Prescaler,                                         Address offset: 0x28 */
    __IO uint32_t ARR;                      /*!< Auto-reload register,                              Address offset: 0x2C */
    __IO uint32_t RCR;                      /*!< Repetition counter register,                       Address offset: 0x30 */
    __IO uint32_t CCR0;                     /*!< Capture/Compare register 0,                        Address offset: 0x34 */
    __IO uint32_t CCR1;                     /*!< Capture/Compare register 1,                        Address offset: 0x38 */
    __IO uint32_t CCR2;                     /*!< Capture/Compare register 2,                        Address offset: 0x3C */
    __IO uint32_t CCR3;                     /*!< Capture/Compare register 3,                        Address offset: 0x40 */
    __IO uint32_t BDTR;                     /*!< Break and dead-time register,                      Address offset: 0x44 */
    __IO uint32_t DCR;                      /*!< DMA control register,                              Address offset: 0x48 */
    __IO uint32_t DMAR;                     /*!< DMA register for burst access,                     Address offset: 0x4C */
} PWMA_TypeDef;

/**
 * @brief QE
 */
typedef struct
{
    __IO uint32_t QESET;                    /*!< QENC setting register,                             Address offset: 0x00 */
    __IO uint32_t FILTSET;                  /*!< QENC filters setting register,                     Address offset: 0x04 */
    __IO uint32_t QEMAX;                    /*!< Max counter value register,                        Address offset: 0x08 */
    __IO uint32_t INITSET;                  /*!< Init counter value register,                       Address offset: 0x0C */
    __IO uint32_t QECNT;                    /*!< Current counter value register,                    Address offset: 0x10 */
} QE_Type;

/**
 * @brief PWMG
 */
typedef struct
{
    __IO uint32_t CR1;                      /*!< Control register 1,                                Address offset: 0x00 */
    uint32_t      RESERVED0[2];             /*!< Reserved,                                          Address offset: 0x04 */
    __IO uint32_t DIER;                     /*!< DMA/Interrupt enable register,                     Address offset: 0x0C */
    __IO uint32_t SR;                       /*!< Status register,                                   Address offset: 0x10 */
    __IO uint32_t EGR;                      /*!< Event generation register,                         Address offset: 0x14 */
    union
    {
        __IO uint32_t CCMR1_MODE0;          /*!< Capture/Compare mode register 1,                   Address offset: 0x18 */
        __IO uint32_t CCMR1_MODE1;          /*!< Capture/Compare mode register 1,                   Address offset: 0x18 */
    };
    uint32_t      RESERVED1;                /*!< Reserved,                                          Address offset: 0x1C */
    __IO uint32_t CCER;                     /*!< Capture/Compare enable register,                   Address offset: 0x20 */
    __IO uint32_t CNT;                      /*!< Counter,                                           Address offset: 0x24 */
    __IO uint32_t PSC;                      /*!< Prescaler,                                         Address offset: 0x28 */
    __IO uint32_t ARR;                      /*!< Auto-reload register,                              Address offset: 0x2C */
    uint32_t      RESERVED2;                /*!< Reserved,                                          Address offset: 0x30 */
    __IO uint32_t CCR0;                     /*!< Capture/Compare register 0,                        Address offset: 0x34 */
} PWMG_Type;

/**
 * @brief WatchDog Timer (WDT)
 */
typedef struct __PACKED {
    __IO uint32_t CR;                       /*!< Control,                                           Address offset: 0x00 */
    __IO uint32_t TORR;                     /*!< Timeout Range,                                     Address offset: 0x04 */
    __IO uint32_t CCVR;                     /*!< Current Counter Value,                             Address offset: 0x08 */
    __IO uint32_t CRR;                      /*!< Counter Restart,                                   Address offset: 0x0C */
    __IO uint32_t STAT;                     /*!< Interrupt Status,                                  Address offset: 0x10 */
    __IO uint32_t EOI;                      /*!< Interrupt Clear,                                   Address offset: 0x14 */
} WDT_TypeDef;

/**
 * @brief CORE2 subsystem, INT
 */
typedef struct
{
    __IO uint32_t IER;                      /*!< Interrupt enable register,                         Address offset: 0x00 */
} CORE2_INT_TypeDef;

/**
 * @brief CORE2 subsystem, PIO
 */
typedef struct
{
    __IO uint32_t IN;                       /*!< Input data register,                               Address offset: 0x00 */
    __IO uint32_t EN;                       /*!< Direction control register,                        Address offset: 0x04 */
    __IO uint32_t OU;                       /*!< Output data register,                              Address offset: 0x08 */
} CORE2_PIO_TypeDef;

/******************************************************************************/
/*                              Peripheral Memory Map                         */
/******************************************************************************/

/* Peripheral memory map */
#define PERIPH_BASE             0x11000000UL                        /*!< Peripheral base address */

#define APB0PERIPH_BASE         PERIPH_BASE                         /*!< APB0 peripheral base address */
#define APB1PERIPH_BASE         (PERIPH_BASE + 0x02000000UL)        /*!< APB1 peripheral base address */
#define APB2PERIPH_BASE         (PERIPH_BASE + 0x03000000UL)        /*!< APB2 peripheral base address */
#define AHBPERIPH_BASE          (PERIPH_BASE + 0x04000000UL)        /*!< AHB peripheral base address */

/* APB0 periphery */
#define QSPI0_BASE              (APB0PERIPH_BASE + 0x00000000UL)    /*!< QSPI0 peripheral base address */
#define UART0_BASE              (APB0PERIPH_BASE + 0x00001000UL)    /*!< UART0 peripheral base address */
#define UART1_BASE              (APB0PERIPH_BASE + 0x00002000UL)    /*!< UART1 peripheral base address */
#define UART2_BASE              (APB0PERIPH_BASE + 0x00003000UL)    /*!< UART2 peripheral base address */
#define I2C0_BASE               (APB0PERIPH_BASE + 0x00004000UL)    /*!< I2C0 peripheral base address */
#define I2C1_BASE               (APB0PERIPH_BASE + 0x00005000UL)    /*!< I2C1 peripheral base address */
#define I2S0_BASE               (APB0PERIPH_BASE + 0x00006000UL)    /*!< I2S0 peripheral base address */
#define TIM0_BASE               (APB0PERIPH_BASE + 0x00007000UL)    /*!< TIM0 peripheral base address */
#define WDT0_BASE               (APB0PERIPH_BASE + 0x00008000UL)    /*!< WDT0 peripheral base address */
#define GPIO0_BASE              (APB0PERIPH_BASE + 0x00009000UL)    /*!< GPIO0 peripheral base address */
#define PWMG0_BASE              (APB0PERIPH_BASE + 0x0000A000UL)    /*!< PWMG0 peripheral base address */
#define SPI0_BASE               (APB0PERIPH_BASE + 0x0000B000UL)    /*!< SPI0 peripheral base address */
#define SPI1_BASE               (APB0PERIPH_BASE + 0x0000C000UL)    /*!< SPI1 peripheral base address */
#define CANFD0_BASE             (APB0PERIPH_BASE + 0x00010000UL)    /*!< CANFD0 peripheral base address */

/* APB1 periphery */
#define QSPI1_BASE              (APB1PERIPH_BASE + 0x00000000UL)    /*!< QSPI1 peripheral base address */
#define UART3_BASE              (APB1PERIPH_BASE + 0x00001000UL)    /*!< UART3 peripheral base address */
#define UART4_BASE              (APB1PERIPH_BASE + 0x00002000UL)    /*!< UART4 peripheral base address */
#define UART5_BASE              (APB1PERIPH_BASE + 0x00003000UL)    /*!< UART5 peripheral base address */
#define I2C2_BASE               (APB1PERIPH_BASE + 0x00004000UL)    /*!< I2C2 peripheral base address */
#define I2C3_BASE               (APB1PERIPH_BASE + 0x00005000UL)    /*!< I2C3 peripheral base address */
#define I2S1_BASE               (APB1PERIPH_BASE + 0x00006000UL)    /*!< I2S1 peripheral base address */
#define TIM1_BASE               (APB1PERIPH_BASE + 0x00007000UL)    /*!< TIM1 peripheral base address */
#define WDT1_BASE               (APB1PERIPH_BASE + 0x00008000UL)    /*!< WDT1 peripheral base address */
#define GPIO1_BASE              (APB1PERIPH_BASE + 0x00009000UL)    /*!< GPIO1 peripheral base address */
#define PWMG1_BASE              (APB1PERIPH_BASE + 0x0000A000UL)    /*!< PWMG1 peripheral base address */
#define SPI2_BASE               (APB1PERIPH_BASE + 0x0000B000UL)    /*!< SPI2 peripheral base address */
#define SPI3_BASE               (APB1PERIPH_BASE + 0x0000C000UL)    /*!< SPI3 peripheral base address */
#define CANFD1_BASE             (APB1PERIPH_BASE + 0x00010000UL)    /*!< CANFD1 peripheral base address */

/* APB2 periphery */
#define CRU_BASE                (APB2PERIPH_BASE + 0x00000000UL)    /*!< CRU peripheral base address */
#define ADC0_BASE               (APB2PERIPH_BASE + 0x00001000UL)    /*!< ADC0 peripheral base address */
#define ADC1_BASE               (APB2PERIPH_BASE + 0x00002000UL)    /*!< ADC1 peripheral base address */
#define ADC2_BASE               (APB2PERIPH_BASE + 0x00003000UL)    /*!< ADC2 peripheral base address */
#define PWMA0_BASE              (APB2PERIPH_BASE + 0x00004000UL)    /*!< PWMA0 peripheral base address */
#define QE0_BASE                (APB2PERIPH_BASE + 0x00004050UL)    /*!< QE0 peripheral base address */
#define PWMA1_BASE              (APB2PERIPH_BASE + 0x00005000UL)    /*!< PWMA1 peripheral base address */
#define QE1_BASE                (APB2PERIPH_BASE + 0x00005050UL)    /*!< QE1 peripheral base address */
#define PWMA2_BASE              (APB2PERIPH_BASE + 0x00006000UL)    /*!< PWMA2 peripheral base address */
#define QE2_BASE                (APB2PERIPH_BASE + 0x00006050UL)    /*!< QE2 peripheral base address */
#define PWMA3_BASE              (APB2PERIPH_BASE + 0x00007000UL)    /*!< PWMA3 peripheral base address */
#define QE3_BASE                (APB2PERIPH_BASE + 0x00007050UL)    /*!< QE3 peripheral base address */
#define GPIO2_BASE              (APB2PERIPH_BASE + 0x00008000UL)    /*!< GPIO2 peripheral base address */
#define UART6_BASE              (APB2PERIPH_BASE + 0x00009000UL)    /*!< UART6 peripheral base address */
#define UART7_BASE              (APB2PERIPH_BASE + 0x0000A000UL)    /*!< UART7 peripheral base address */
#define AXI_BASE                (APB2PERIPH_BASE + 0x0000B000UL)    /*!< AXI config base address */

/* AHB periphery */
#define USB_BASE                (AHBPERIPH_BASE + 0x00000000UL)     /*!< USB peripheral base address */
#define DMA0_BASE               (AHBPERIPH_BASE + 0x00100000UL)     /*!< DMA0 peripheral base address */
#define DMA1_BASE               (AHBPERIPH_BASE + 0x00101000UL)     /*!< DMA1 peripheral base address */
#define DMA2_BASE               (AHBPERIPH_BASE + 0x00102000UL)     /*!< DMA2 peripheral base address */
#define CORE2_INT_SYS_BASE      (AHBPERIPH_BASE + 0x001037F0UL)     /*!< CORE2 INT peripheral base address (System side) */
#define CORE2_PIO_SYS_BASE      (AHBPERIPH_BASE + 0x001037F4UL)     /*!< CORE2 PIO peripheral base address (System side) */
#define TRACER_ENC_BASE         (AHBPERIPH_BASE + 0x00104000UL)     /*!< Tracer encoder control interface base address */
#define TRACER_RAM_BASE         (AHBPERIPH_BASE + 0x00105000UL)     /*!< Tracer RAM control interface base address */
#define TRACER_PIB_BASE         (AHBPERIPH_BASE + 0x00106000UL)     /*!< Tracer PIB control interface base address */
#define TRACER_SRAM_BASE        (AHBPERIPH_BASE + 0x00107000UL)     /*!< Tracer SRAM interface base address */
#define AHB_BASE                (AHBPERIPH_BASE + 0x00108000UL)     /*!< AHB config base address */

/* CORE2 memory (System side) */
#define CORE2_TCMA_SYS_BASE     (AHBPERIPH_BASE + 0x00103000UL)     /*!< CORE2 TCMA base address (System side) */
#define CORE2_TCMB_SYS_BASE     (AHBPERIPH_BASE + 0x00103400UL)     /*!< CORE2 TCMB base address (System side) */

/* CORE2 periphery */
#define CORE2PERIPH_BASE        0x400007F0UL                        /*!< CORE2 peripheral base address (CORE2 side) */
#define CORE2_INT_BASE          (CORE2PERIPH_BASE + 0x00000000UL)   /*!< CORE2 INT peripheral base address (CORE2 side) */
#define CORE2_PIO_BASE          (CORE2PERIPH_BASE + 0x00000004UL)   /*!< CORE2 PIO peripheral base address (CORE2 side) */

/* System memory */
#define ROM0_BASE               0x40000000UL                        /*!< ROM0 base address */
#define ROM1_BASE               0x40008000UL                        /*!< ROM1 base address */
#define TCMA_BASE               0x40010000UL                        /*!< TCMA base address */
#define TCMB_BASE               0x40020000UL                        /*!< TCMB base address */

/* Processor cores 0 and 1 access ROM0, ROM1, TCMA and TCMB memories directly */
/* at ROM0_BASE, ROM1_BASE, TCMA_BASE, TCMB_BASE addresses through dedicated  */
/* TCM ports. External peripherals can access ROM and TCM memories through    */
/* the slave port (Front Port) of processor core only. Peripheral domains are */
/* commutated to processor core interfaces through AXI interconnect block.    */
/* When  the peripheral (AXI master) accesses ROM and TCM through the core    */
/* Front Port (AXI slave), the memory addresses are mapped to                 */
/* FP_CORE0_ROM0_BASE, FP_CORE0_ROM1_BASE, FP_CORE0_TCMA_BASE and             */
/* FP_CORE0_TCMB_BASE for Core 0, and to FP_CORE1_ROM0_BASE,                  */
/* FP_CORE1_ROM1_BASE, FP_CORE1_TCMA_BASE and FP_CORE1_TCMB_BASE for Core 1   */
/* respectively.                                                              */

#define FP_CORE0_ROM0_BASE      0x50000000UL                        /*!< FP Core 0 ROM0 base address */
#define FP_CORE0_ROM1_BASE      0x50008000UL                        /*!< FP Core 0 ROM1 base address */
#define FP_CORE0_TCMA_BASE      0x50010000UL                        /*!< FP Core 0 TCMA base address */
#define FP_CORE0_TCMB_BASE      0x50020000UL                        /*!< FP Core 0 TCMB base address */

#define FP_CORE1_ROM0_BASE      0x60000000UL                        /*!< FP Core 1 ROM0 base address */
#define FP_CORE1_ROM1_BASE      0x60008000UL                        /*!< FP Core 1 ROM1 base address */
#define FP_CORE1_TCMA_BASE      0x60010000UL                        /*!< FP Core 1 TCMA base address */
#define FP_CORE1_TCMB_BASE      0x60020000UL                        /*!< FP Core 1 TCMB base address */

#define SRAM_BASE               0x70000000UL                        /*!< SRAM base address */
#define XIP0_BASE               0x80000000UL                        /*!< XIP0 base address */
#define XIP1_BASE               0x90000000UL                        /*!< XIP1 base address */
#define EFLASH_BASE             0xA0000000UL                        /*!< EFLASH base address */
#define ROM_BASE                0xA0040000UL                        /*!< ROM base address */
#define EFLASH_REG_BASE         0xA0060000UL                        /*!< EFLASH registers space base address */

/* CORE2 memory (CORE2 side) */
#define CORE2_TCMA_BASE         0x40000000UL                        /*!< CORE2 TCMA base address (CORE2 side) */
#define CORE2_TCMB_BASE         0x40000400UL                        /*!< CORE2 TCMB base address (CORE2 side) */

/******************************************************************************/
/*                           Peripheral Declaration                           */
/******************************************************************************/

#define ADC0                   ((ADC_TypeDef*)ADC0_BASE)
#define ADC1                   ((ADC_TypeDef*)ADC1_BASE)
#define ADC2                   ((ADC_TypeDef*)ADC2_BASE)
#define AHB                    ((AHB_TypeDef*)AHB_BASE)
#define AXI                    ((AXI_TypeDef*)AXI_BASE)
#define CANFD0                 ((CANFD_TypeDef*)CANFD0_BASE)
#define CANFD1                 ((CANFD_TypeDef*)CANFD1_BASE)
#define CRU                    ((CRU_TypeDef*)CRU_BASE)
#define DMA0                   ((DMA_TypeDef*)DMA0_BASE)
#define DMA1                   ((DMA_TypeDef*)DMA1_BASE)
#define DMA2                   ((DMA_TypeDef*)DMA2_BASE)
#define EFLASH                 ((EFLASH_TypeDef*)EFLASH_REG_BASE)
#define GPIO0                  ((GPIO_TypeDef*)GPIO0_BASE)
#define GPIO1                  ((GPIO_TypeDef*)GPIO1_BASE)
#define GPIO2                  ((GPIO_TypeDef*)GPIO2_BASE)
#define I2C0                   ((I2C_TypeDef*)I2C0_BASE)
#define I2C1                   ((I2C_TypeDef*)I2C1_BASE)
#define I2C2                   ((I2C_TypeDef*)I2C2_BASE)
#define I2C3                   ((I2C_TypeDef*)I2C3_BASE)
#define I2S0                   ((I2S_TypeDef*)I2S0_BASE)
#define I2S1                   ((I2S_TypeDef*)I2S1_BASE)
#define PWMA0                  ((PWMA_TypeDef*)PWMA0_BASE)
#define PWMA1                  ((PWMA_TypeDef*)PWMA1_BASE)
#define PWMA2                  ((PWMA_TypeDef*)PWMA2_BASE)
#define PWMA3                  ((PWMA_TypeDef*)PWMA3_BASE)
#define PWMG0                  ((PWMG_Type*)PWMG0_BASE)
#define PWMG1                  ((PWMG_Type*)PWMG1_BASE)
#define QE0                    ((QE_Type*)QE0_BASE)
#define QE1                    ((QE_Type*)QE1_BASE)
#define QE2                    ((QE_Type*)QE2_BASE)
#define QE3                    ((QE_Type*)QE3_BASE)
#define QSPI0                  ((QSPI_TypeDef*)QSPI0_BASE)
#define QSPI1                  ((QSPI_TypeDef*)QSPI1_BASE)
#define SPI0                   ((SPI_TypeDef*)SPI0_BASE)
#define SPI1                   ((SPI_TypeDef*)SPI1_BASE)
#define SPI2                   ((SPI_TypeDef*)SPI2_BASE)
#define SPI3                   ((SPI_TypeDef*)SPI3_BASE)
#define TIM0                   ((TIM_TypeDef*)TIM0_BASE)
#define TIM1                   ((TIM_TypeDef*)TIM1_BASE)
#define UART0                  ((UART_TypeDef*)UART0_BASE)
#define UART1                  ((UART_TypeDef*)UART1_BASE)
#define UART2                  ((UART_TypeDef*)UART2_BASE)
#define UART3                  ((UART_TypeDef*)UART3_BASE)
#define UART4                  ((UART_TypeDef*)UART4_BASE)
#define UART5                  ((UART_TypeDef*)UART5_BASE)
#define UART6                  ((UART_TypeDef*)UART6_BASE)
#define UART7                  ((UART_TypeDef*)UART7_BASE)
#define USB                    ((USB_TypeDef*)USB_BASE)
#define WDT0                   ((WDT_TypeDef*)WDT0_BASE)
#define WDT1                   ((WDT_TypeDef*)WDT1_BASE)

/**
 * @brief CORE2 subsystem (System side)
 * @note Use it only from Core0+Core1 side
 */
#define CORE2_INT_SYS          ((CORE2_INT_TypeDef*)CORE2_INT_SYS_BASE)
#define CORE2_PIO_SYS          ((CORE2_PIO_TypeDef*)CORE2_PIO_SYS_BASE)

/**
 * @brief CORE2 subsystem (CORE2 side)
 * @note Use it only from CORE2 side
 */
#define CORE2_INT              ((CORE2_INT_TypeDef*)CORE2_INT_BASE)
#define CORE2_PIO              ((CORE2_PIO_TypeDef*)CORE2_PIO_BASE)

/******************************************************************************/
/*                         Peripheral Registers Bits Definition               */
/******************************************************************************/

/******************************************************************************/
/*                       Analog to Digital Converter                          */
/******************************************************************************/

/********************  Bit definition for ADC_CR1 register  *******************/

/** @brief CR1[0]: ADC on/off */
#define ADC_CR1_ADON_Pos                    (0U)
#define ADC_CR1_ADON_Msk                    (0x1UL << ADC_CR1_ADON_Pos)
#define ADC_CR1_ADON                        ADC_CR1_ADON_Msk

/** @brief CR1[1]: Continuous conversion enable */
#define ADC_CR1_CONT_Pos                    (1U)
#define ADC_CR1_CONT_Msk                    (0x1UL << ADC_CR1_CONT_Pos)
#define ADC_CR1_CONT                        ADC_CR1_CONT_Msk

#define ADC_CR1_CONT_DISABLE                (0x0UL << ADC_CR1_CONT_Pos)     /*!< Continuous conversion is disabled */
#define ADC_CR1_CONT_ENABLE                 (0x1UL << ADC_CR1_CONT_Pos)     /*!< Continuous conversion is enabled */

/** @brief CR1[2]: Scan mode enable */
#define ADC_CR1_SCANEN_Pos                  (2U)
#define ADC_CR1_SCANEN_Msk                  (0x1UL << ADC_CR1_SCANEN_Pos)
#define ADC_CR1_SCANEN                      ADC_CR1_SCANEN_Msk

#define ADC_CR1_SCANEN_DISABLE              (0x0UL << ADC_CR1_SCANEN_Pos)   /*!< Scan mode is disabled */
#define ADC_CR1_SCANEN_ENABLE               (0x1UL << ADC_CR1_SCANEN_Pos)   /*!< Scan mode is enabled */

/** @brief CR1[3]: Discontinuous mode enable */
#define ADC_CR1_DISCEN_Pos                  (3U)
#define ADC_CR1_DISCEN_Msk                  (0x1UL << ADC_CR1_DISCEN_Pos)
#define ADC_CR1_DISCEN                      ADC_CR1_DISCEN_Msk

#define ADC_CR1_DISCEN_DISABLE              (0x0UL << ADC_CR1_DISCEN_Pos)   /*!< Discontinuous mode is disabled */
#define ADC_CR1_DISCEN_ENABLE               (0x1UL << ADC_CR1_DISCEN_Pos)   /*!< Discontinuous mode is enabled */

/** @brief CR1[4]: EOC interrupt enable */
#define ADC_CR1_IEEOC_Pos                   (4U)
#define ADC_CR1_IEEOC_Msk                   (0x1UL << ADC_CR1_IEEOC_Pos)
#define ADC_CR1_IEEOC                       ADC_CR1_IEEOC_Msk

/** @brief CR1[5]: DMA enable */
#define ADC_CR1_DMAEN_Pos                   (5U)
#define ADC_CR1_DMAEN_Msk                   (0x1UL << ADC_CR1_DMAEN_Pos)
#define ADC_CR1_DMAEN                       ADC_CR1_DMAEN_Msk

#define ADC_CR1_DMAEN_DISABLE               (0x0UL << ADC_CR1_DMAEN_Pos)    /*!< DMA transfer is disabled */
#define ADC_CR1_DMAEN_ENABLE                (0x1UL << ADC_CR1_DMAEN_Pos)    /*!< DMA transfer is enabled */

/** @brief CR1[8:6]: Discontinuous mode channel count */
#define ADC_CR1_DISCNUM_Pos                 (6U)
#define ADC_CR1_DISCNUM_Msk                 (0x7UL << ADC_CR1_DISCNUM_Pos)
#define ADC_CR1_DISCNUM                     ADC_CR1_DISCNUM_Msk

#define ADC_CR1_DISCNUM_1RANK               (0x0UL << ADC_CR1_DISCNUM_Pos)  /*!< 1 rank */
#define ADC_CR1_DISCNUM_2RANKS              (0x1UL << ADC_CR1_DISCNUM_Pos)  /*!< 2 ranks */
#define ADC_CR1_DISCNUM_3RANKS              (0x2UL << ADC_CR1_DISCNUM_Pos)  /*!< 3 ranks */
#define ADC_CR1_DISCNUM_4RANKS              (0x3UL << ADC_CR1_DISCNUM_Pos)  /*!< 4 ranks */
#define ADC_CR1_DISCNUM_5RANKS              (0x4UL << ADC_CR1_DISCNUM_Pos)  /*!< 5 ranks */
#define ADC_CR1_DISCNUM_6RANKS              (0x5UL << ADC_CR1_DISCNUM_Pos)  /*!< 6 ranks */
#define ADC_CR1_DISCNUM_7RANKS              (0x6UL << ADC_CR1_DISCNUM_Pos)  /*!< 7 ranks */
#define ADC_CR1_DISCNUM_8RANKS              (0x7UL << ADC_CR1_DISCNUM_Pos)  /*!< 8 ranks */

/** @brief CR1[13:9]: External event select */
#define ADC_CR1_EXTSEL_Pos                  (9U)
#define ADC_CR1_EXTSEL_Msk                  (0x1FUL << ADC_CR1_EXTSEL_Pos)
#define ADC_CR1_EXTSEL                      ADC_CR1_EXTSEL_Msk

#define ADC_CR1_EXTSEL_0                    (0x0UL << ADC_CR1_EXTSEL_Pos)   /*!< External event 0 */
#define ADC_CR1_EXTSEL_1                    (0x1UL << ADC_CR1_EXTSEL_Pos)   /*!< External event 1 */
#define ADC_CR1_EXTSEL_2                    (0x2UL << ADC_CR1_EXTSEL_Pos)   /*!< External event 2 */
#define ADC_CR1_EXTSEL_3                    (0x3UL << ADC_CR1_EXTSEL_Pos)   /*!< External event 3 */
#define ADC_CR1_EXTSEL_4                    (0x4UL << ADC_CR1_EXTSEL_Pos)   /*!< External event 4 */
#define ADC_CR1_EXTSEL_5                    (0x5UL << ADC_CR1_EXTSEL_Pos)   /*!< External event 5 */
#define ADC_CR1_EXTSEL_6                    (0x6UL << ADC_CR1_EXTSEL_Pos)   /*!< External event 6 */
#define ADC_CR1_EXTSEL_7                    (0x7UL << ADC_CR1_EXTSEL_Pos)   /*!< External event 7 */
#define ADC_CR1_EXTSEL_8                    (0x8UL << ADC_CR1_EXTSEL_Pos)   /*!< External event 8 */
#define ADC_CR1_EXTSEL_9                    (0x9UL << ADC_CR1_EXTSEL_Pos)   /*!< External event 9 */
#define ADC_CR1_EXTSEL_10                   (0xAUL << ADC_CR1_EXTSEL_Pos)   /*!< External event 10 */
#define ADC_CR1_EXTSEL_11                   (0xBUL << ADC_CR1_EXTSEL_Pos)   /*!< External event 11 */
#define ADC_CR1_EXTSEL_12                   (0xCUL << ADC_CR1_EXTSEL_Pos)   /*!< External event 12 */
#define ADC_CR1_EXTSEL_13                   (0xDUL << ADC_CR1_EXTSEL_Pos)   /*!< External event 13 */
#define ADC_CR1_EXTSEL_14                   (0xEUL << ADC_CR1_EXTSEL_Pos)   /*!< External event 14 */
#define ADC_CR1_EXTSEL_15                   (0xFUL << ADC_CR1_EXTSEL_Pos)   /*!< External event 15 */
#define ADC_CR1_EXTSEL_16                   (0x10UL << ADC_CR1_EXTSEL_Pos)  /*!< External event 16 */
#define ADC_CR1_EXTSEL_17                   (0x11UL << ADC_CR1_EXTSEL_Pos)  /*!< External event 17 */
#define ADC_CR1_EXTSEL_18                   (0x12UL << ADC_CR1_EXTSEL_Pos)  /*!< External event 18 */
#define ADC_CR1_EXTSEL_19                   (0x13UL << ADC_CR1_EXTSEL_Pos)  /*!< External event 19 */

/** @brief CR1[14]: External trigger conversion mode enable */
#define ADC_CR1_EXTTRIG_Pos                 (14U)
#define ADC_CR1_EXTTRIG_Msk                 (0x1UL << ADC_CR1_EXTTRIG_Pos)
#define ADC_CR1_EXTTRIG                     ADC_CR1_EXTTRIG_Msk
/** @brief CR1[15]: Start of conversion */
#define ADC_CR1_SWSTART_Pos                 (15U)
#define ADC_CR1_SWSTART_Msk                 (0x1UL << ADC_CR1_SWSTART_Pos)
#define ADC_CR1_SWSTART                     ADC_CR1_SWSTART_Msk

/** @brief CR1[17:16]: Clock division */
#define ADC_CR1_CKD_Pos                     (16U)
#define ADC_CR1_CKD_Msk                     (0x3UL << ADC_CR1_CKD_Pos)
#define ADC_CR1_CKD                         ADC_CR1_CKD_Msk

#define ADC_CR1_CKD_DIV8                    (0x0UL << ADC_CR1_CKD_Pos)  /*!< CK_INT divided by 8 */
#define ADC_CR1_CKD_DIV4                    (0x1UL << ADC_CR1_CKD_Pos)  /*!< CK_INT divided by 4 */
#define ADC_CR1_CKD_DIV2                    (0x2UL << ADC_CR1_CKD_Pos)  /*!< CK_INT divided by 2 */

/** @brief CR1[18]: Clear interrupt */
#define ADC_CR1_CLRINTRPT_Pos               (18U)
#define ADC_CR1_CLRINTRPT_Msk               (0x1UL << ADC_CR1_CLRINTRPT_Pos)
#define ADC_CR1_CLRINTRPT                   ADC_CR1_CLRINTRPT_Msk

/*******************  Bit definition for ADC_ASER register  *******************/

/** @brief ASER[0]: Input type selection */
#define ADC_ASER_SELVI_HD_LS_Pos            (0U)
#define ADC_ASER_SELVI_HD_LS_Msk            (0x1UL << ADC_ASER_SELVI_HD_LS_Pos)
#define ADC_ASER_SELVI_HD_LS                ADC_ASER_SELVI_HD_LS_Msk

#define ADC_ASER_SELVI_HD_LS_SINGLE         (0x0UL << ADC_ASER_SELVI_HD_LS_Pos) /*!< Single end */
#define ADC_ASER_SELVI_HD_LS_DIFF           (0x1UL << ADC_ASER_SELVI_HD_LS_Pos) /*!< Differential */

/** @brief ASER[2:1]: Filter mode of the temperature sensor */
#define ADC_ASER_TS_MODE_Pos                (1U)
#define ADC_ASER_TS_MODE_Msk                (0x3UL << ADC_ASER_TS_MODE_Pos)
#define ADC_ASER_TS_MODE                    ADC_ASER_TS_MODE_Msk

#define ADC_ASER_TS_MODE_BYPASS             (0x0UL << ADC_ASER_TS_MODE_Pos) /*!< Bypass internal lowpass filter */
#define ADC_ASER_TS_MODE_AVG64              (0x1UL << ADC_ASER_TS_MODE_Pos) /*!< 64 times average */
#define ADC_ASER_TS_MODE_AVG128             (0x2UL << ADC_ASER_TS_MODE_Pos) /*!< 128 times average */
#define ADC_ASER_TS_MODE_AVG256             (0x3UL << ADC_ASER_TS_MODE_Pos) /*!< 256 times average */

/** @brief ASER[3]: Output data type selection */
#define ADC_ASER_SELDO_HS_LU_Pos            (3U)
#define ADC_ASER_SELDO_HS_LU_Msk            (0x1UL << ADC_ASER_SELDO_HS_LU_Pos)
#define ADC_ASER_SELDO_HS_LU                ADC_ASER_SELDO_HS_LU_Msk

#define ADC_ASER_SELDO_HS_LU_UNSIGNED       (0x0UL << ADC_ASER_SELDO_HS_LU_Pos) /*!< Unsigned data */
#define ADC_ASER_SELDO_HS_LU_SIGNED         (0x1UL << ADC_ASER_SELDO_HS_LU_Pos) /*!< Signed data */

/** @brief ASER[7:4]: Temperature sensor offset adjustment signal */
#define ADC_ASER_ADJ_TD_OS_Pos              (4U)
#define ADC_ASER_ADJ_TD_OS_Msk              (0xFUL << ADC_ASER_ADJ_TD_OS_Pos)
#define ADC_ASER_ADJ_TD_OS                  ADC_ASER_ADJ_TD_OS_Msk
/** @brief ASER[11:8]: Temperature sensor gain adjustment signal */
#define ADC_ASER_ADJ_TD_GA_Pos              (8U)
#define ADC_ASER_ADJ_TD_GA_Msk              (0xFUL << ADC_ASER_ADJ_TD_GA_Pos)
#define ADC_ASER_ADJ_TD_GA                  ADC_ASER_ADJ_TD_GA_Msk
/** @brief ASER[17]: Test mode enable */
#define ADC_ASER_TEST_EN_Pos                (17U)
#define ADC_ASER_TEST_EN_Msk                (0x1UL << ADC_ASER_TEST_EN_Pos)
#define ADC_ASER_TEST_EN                    ADC_ASER_TEST_EN_Msk
/** @brief ASER[18]: Calibration mode enable */
#define ADC_ASER_CAL_SEL_Pos                (18U)
#define ADC_ASER_CAL_SEL_Msk                (0x1UL << ADC_ASER_CAL_SEL_Pos)
#define ADC_ASER_CAL_SEL                    ADC_ASER_CAL_SEL_Msk

/*******************  Bit definition for ADC_ASTR register  *******************/

/** @brief ASTR[0]: End of conversion */
#define ADC_ASTR_EOC_Pos                    (0U)
#define ADC_ASTR_EOC_Msk                    (0x1UL << ADC_ASTR_EOC_Pos)
#define ADC_ASTR_EOC                        ADC_ASTR_EOC_Msk
/** @brief ASTR[1]: Start of conversion */
#define ADC_ASTR_STRT_Pos                   (1U)
#define ADC_ASTR_STRT_Msk                   (0x1UL << ADC_ASTR_STRT_Pos)
#define ADC_ASTR_STRT                       ADC_ASTR_STRT_Msk

/*******************  Bit definition for ADC_ASMPR register  ******************/

/** @brief ASMPR[2:0]: Sample count 0 */
#define ADC_ASMPR_SMP0_Pos                  (0U)
#define ADC_ASMPR_SMP0_Msk                  (0x7UL << ADC_ASMPR_SMP0_Pos)
#define ADC_ASMPR_SMP0                      ADC_ASMPR_SMP0_Msk

#define ADC_ASMPR_SMP0_3CYCLES              (0x0UL << ADC_ASMPR_SMP0_Pos)   /*!< 3 cycles */
#define ADC_ASMPR_SMP0_25CYCLES             (0x1UL << ADC_ASMPR_SMP0_Pos)   /*!< 25 cycles */
#define ADC_ASMPR_SMP0_50CYCLES             (0x2UL << ADC_ASMPR_SMP0_Pos)   /*!< 50 cycles */
#define ADC_ASMPR_SMP0_100CYCLES            (0x3UL << ADC_ASMPR_SMP0_Pos)   /*!< 100 cycles */
#define ADC_ASMPR_SMP0_150CYCLES            (0x4UL << ADC_ASMPR_SMP0_Pos)   /*!< 150 cycles */
#define ADC_ASMPR_SMP0_180CYCLES            (0x5UL << ADC_ASMPR_SMP0_Pos)   /*!< 180 cycles */
#define ADC_ASMPR_SMP0_250CYCLES            (0x6UL << ADC_ASMPR_SMP0_Pos)   /*!< 250 cycles */
#define ADC_ASMPR_SMP0_300CYCLES            (0x7UL << ADC_ASMPR_SMP0_Pos)   /*!< 300 cycles */

/** @brief ASMPR[5:3]: Sample count 1 */
#define ADC_ASMPR_SMP1_Pos                  (3U)
#define ADC_ASMPR_SMP1_Msk                  (0x7UL << ADC_ASMPR_SMP1_Pos)
#define ADC_ASMPR_SMP1                      ADC_ASMPR_SMP1_Msk

#define ADC_ASMPR_SMP1_3                    (0x0UL << ADC_ASMPR_SMP1_Pos)   /*!< 3 periods */
#define ADC_ASMPR_SMP1_25                   (0x1UL << ADC_ASMPR_SMP1_Pos)   /*!< 25 periods */
#define ADC_ASMPR_SMP1_50                   (0x2UL << ADC_ASMPR_SMP1_Pos)   /*!< 50 periods */
#define ADC_ASMPR_SMP1_100                  (0x3UL << ADC_ASMPR_SMP1_Pos)   /*!< 100 periods */
#define ADC_ASMPR_SMP1_150                  (0x4UL << ADC_ASMPR_SMP1_Pos)   /*!< 150 periods */
#define ADC_ASMPR_SMP1_180                  (0x5UL << ADC_ASMPR_SMP1_Pos)   /*!< 180 periods */
#define ADC_ASMPR_SMP1_250                  (0x6UL << ADC_ASMPR_SMP1_Pos)   /*!< 250 periods */
#define ADC_ASMPR_SMP1_300                  (0x7UL << ADC_ASMPR_SMP1_Pos)   /*!< 300 periods */

/** @brief ASMPR[8:6]: Sample count 2 */
#define ADC_ASMPR_SMP2_Pos                  (6U)
#define ADC_ASMPR_SMP2_Msk                  (0x7UL << ADC_ASMPR_SMP2_Pos)
#define ADC_ASMPR_SMP2                      ADC_ASMPR_SMP2_Msk

#define ADC_ASMPR_SMP2_3                    (0x0UL << ADC_ASMPR_SMP2_Pos)   /*!< 3 periods */
#define ADC_ASMPR_SMP2_25                   (0x1UL << ADC_ASMPR_SMP2_Pos)   /*!< 25 periods */
#define ADC_ASMPR_SMP2_50                   (0x2UL << ADC_ASMPR_SMP2_Pos)   /*!< 50 periods */
#define ADC_ASMPR_SMP2_100                  (0x3UL << ADC_ASMPR_SMP2_Pos)   /*!< 100 periods */
#define ADC_ASMPR_SMP2_150                  (0x4UL << ADC_ASMPR_SMP2_Pos)   /*!< 150 periods */
#define ADC_ASMPR_SMP2_180                  (0x5UL << ADC_ASMPR_SMP2_Pos)   /*!< 180 periods */
#define ADC_ASMPR_SMP2_250                  (0x6UL << ADC_ASMPR_SMP2_Pos)   /*!< 250 periods */
#define ADC_ASMPR_SMP2_300                  (0x7UL << ADC_ASMPR_SMP2_Pos)   /*!< 300 periods */

/** @brief ASMPR[11:9]: Sample count 3 */
#define ADC_ASMPR_SMP3_Pos                  (9U)
#define ADC_ASMPR_SMP3_Msk                  (0x7UL << ADC_ASMPR_SMP3_Pos)
#define ADC_ASMPR_SMP3                      ADC_ASMPR_SMP3_Msk

#define ADC_ASMPR_SMP3_3                    (0x0UL << ADC_ASMPR_SMP3_Pos)   /*!< 3 periods */
#define ADC_ASMPR_SMP3_25                   (0x1UL << ADC_ASMPR_SMP3_Pos)   /*!< 25 periods */
#define ADC_ASMPR_SMP3_50                   (0x2UL << ADC_ASMPR_SMP3_Pos)   /*!< 50 periods */
#define ADC_ASMPR_SMP3_100                  (0x3UL << ADC_ASMPR_SMP3_Pos)   /*!< 100 periods */
#define ADC_ASMPR_SMP3_150                  (0x4UL << ADC_ASMPR_SMP3_Pos)   /*!< 150 periods */
#define ADC_ASMPR_SMP3_180                  (0x5UL << ADC_ASMPR_SMP3_Pos)   /*!< 180 periods */
#define ADC_ASMPR_SMP3_250                  (0x6UL << ADC_ASMPR_SMP3_Pos)   /*!< 250 periods */
#define ADC_ASMPR_SMP3_300                  (0x7UL << ADC_ASMPR_SMP3_Pos)   /*!< 300 periods */

/** @brief ASMPR[14:12]: Sample count 4 */
#define ADC_ASMPR_SMP4_Pos                  (12U)
#define ADC_ASMPR_SMP4_Msk                  (0x7UL << ADC_ASMPR_SMP4_Pos)
#define ADC_ASMPR_SMP4                      ADC_ASMPR_SMP4_Msk

#define ADC_ASMPR_SMP4_3                    (0x0UL << ADC_ASMPR_SMP4_Pos)   /*!< 3 periods */
#define ADC_ASMPR_SMP4_25                   (0x1UL << ADC_ASMPR_SMP4_Pos)   /*!< 25 periods */
#define ADC_ASMPR_SMP4_50                   (0x2UL << ADC_ASMPR_SMP4_Pos)   /*!< 50 periods */
#define ADC_ASMPR_SMP4_100                  (0x3UL << ADC_ASMPR_SMP4_Pos)   /*!< 100 periods */
#define ADC_ASMPR_SMP4_150                  (0x4UL << ADC_ASMPR_SMP4_Pos)   /*!< 150 periods */
#define ADC_ASMPR_SMP4_180                  (0x5UL << ADC_ASMPR_SMP4_Pos)   /*!< 180 periods */
#define ADC_ASMPR_SMP4_250                  (0x6UL << ADC_ASMPR_SMP4_Pos)   /*!< 250 periods */
#define ADC_ASMPR_SMP4_300                  (0x7UL << ADC_ASMPR_SMP4_Pos)   /*!< 300 periods */

/** @brief ASMPR[17:15]: Sample count 5 */
#define ADC_ASMPR_SMP5_Pos                  (15U)
#define ADC_ASMPR_SMP5_Msk                  (0x7UL << ADC_ASMPR_SMP5_Pos)
#define ADC_ASMPR_SMP5                      ADC_ASMPR_SMP5_Msk

#define ADC_ASMPR_SMP5_3                    (0x0UL << ADC_ASMPR_SMP5_Pos)   /*!< 3 periods */
#define ADC_ASMPR_SMP5_25                   (0x1UL << ADC_ASMPR_SMP5_Pos)   /*!< 25 periods */
#define ADC_ASMPR_SMP5_50                   (0x2UL << ADC_ASMPR_SMP5_Pos)   /*!< 50 periods */
#define ADC_ASMPR_SMP5_100                  (0x3UL << ADC_ASMPR_SMP5_Pos)   /*!< 100 periods */
#define ADC_ASMPR_SMP5_150                  (0x4UL << ADC_ASMPR_SMP5_Pos)   /*!< 150 periods */
#define ADC_ASMPR_SMP5_180                  (0x5UL << ADC_ASMPR_SMP5_Pos)   /*!< 180 periods */
#define ADC_ASMPR_SMP5_250                  (0x6UL << ADC_ASMPR_SMP5_Pos)   /*!< 250 periods */
#define ADC_ASMPR_SMP5_300                  (0x7UL << ADC_ASMPR_SMP5_Pos)   /*!< 300 periods */

/** @brief ASMPR[20:18]: Sample count 6 */
#define ADC_ASMPR_SMP6_Pos                  (18U)
#define ADC_ASMPR_SMP6_Msk                  (0x7UL << ADC_ASMPR_SMP6_Pos)
#define ADC_ASMPR_SMP6                      ADC_ASMPR_SMP6_Msk

#define ADC_ASMPR_SMP6_3                    (0x0UL << ADC_ASMPR_SMP6_Pos)   /*!< 3 periods */
#define ADC_ASMPR_SMP6_25                   (0x1UL << ADC_ASMPR_SMP6_Pos)   /*!< 25 periods */
#define ADC_ASMPR_SMP6_50                   (0x2UL << ADC_ASMPR_SMP6_Pos)   /*!< 50 periods */
#define ADC_ASMPR_SMP6_100                  (0x3UL << ADC_ASMPR_SMP6_Pos)   /*!< 100 periods */
#define ADC_ASMPR_SMP6_150                  (0x4UL << ADC_ASMPR_SMP6_Pos)   /*!< 150 periods */
#define ADC_ASMPR_SMP6_180                  (0x5UL << ADC_ASMPR_SMP6_Pos)   /*!< 180 periods */
#define ADC_ASMPR_SMP6_250                  (0x6UL << ADC_ASMPR_SMP6_Pos)   /*!< 250 periods */
#define ADC_ASMPR_SMP6_300                  (0x7UL << ADC_ASMPR_SMP6_Pos)   /*!< 300 periods */

/** @brief ASMPR[23:21]: Sample count 7 */
#define ADC_ASMPR_SMP7_Pos                  (21U)
#define ADC_ASMPR_SMP7_Msk                  (0x7UL << ADC_ASMPR_SMP7_Pos)
#define ADC_ASMPR_SMP7                      ADC_ASMPR_SMP7_Msk

#define ADC_ASMPR_SMP7_3                    (0x0UL << ADC_ASMPR_SMP7_Pos)   /*!< 3 periods */
#define ADC_ASMPR_SMP7_25                   (0x1UL << ADC_ASMPR_SMP7_Pos)   /*!< 25 periods */
#define ADC_ASMPR_SMP7_50                   (0x2UL << ADC_ASMPR_SMP7_Pos)   /*!< 50 periods */
#define ADC_ASMPR_SMP7_100                  (0x3UL << ADC_ASMPR_SMP7_Pos)   /*!< 100 periods */
#define ADC_ASMPR_SMP7_150                  (0x4UL << ADC_ASMPR_SMP7_Pos)   /*!< 150 periods */
#define ADC_ASMPR_SMP7_180                  (0x5UL << ADC_ASMPR_SMP7_Pos)   /*!< 180 periods */
#define ADC_ASMPR_SMP7_250                  (0x6UL << ADC_ASMPR_SMP7_Pos)   /*!< 250 periods */
#define ADC_ASMPR_SMP7_300                  (0x7UL << ADC_ASMPR_SMP7_Pos)   /*!< 300 periods */

/*******************  Bit definition for ADC_ASQR register  *******************/

/** @brief ASQR[2:0]: Channel 0 */
#define ADC_ASQR_SQ0_Pos                    (0U)
#define ADC_ASQR_SQ0_Msk                    (0x7UL << ADC_ASQR_SQ0_Pos)
#define ADC_ASQR_SQ0                        ADC_ASQR_SQ0_Msk
/** @brief ASQR[5:3]: Channel 1 */
#define ADC_ASQR_SQ1_Pos                    (3U)
#define ADC_ASQR_SQ1_Msk                    (0x7UL << ADC_ASQR_SQ1_Pos)
#define ADC_ASQR_SQ1                        ADC_ASQR_SQ1_Msk
/** @brief ASQR[8:6]: Channel 2 */
#define ADC_ASQR_SQ2_Pos                    (6U)
#define ADC_ASQR_SQ2_Msk                    (0x7UL << ADC_ASQR_SQ2_Pos)
#define ADC_ASQR_SQ2                        ADC_ASQR_SQ2_Msk
/** @brief ASQR[11:9]: Channel 3 */
#define ADC_ASQR_SQ3_Pos                    (9U)
#define ADC_ASQR_SQ3_Msk                    (0x7UL << ADC_ASQR_SQ3_Pos)
#define ADC_ASQR_SQ3                        ADC_ASQR_SQ3_Msk
/** @brief ASQR[14:12]: Channel 4 */
#define ADC_ASQR_SQ4_Pos                    (12U)
#define ADC_ASQR_SQ4_Msk                    (0x7UL << ADC_ASQR_SQ4_Pos)
#define ADC_ASQR_SQ4                        ADC_ASQR_SQ4_Msk
/** @brief ASQR[17:15]: Channel 5 */
#define ADC_ASQR_SQ5_Pos                    (15U)
#define ADC_ASQR_SQ5_Msk                    (0x7UL << ADC_ASQR_SQ5_Pos)
#define ADC_ASQR_SQ5                        ADC_ASQR_SQ5_Msk
/** @brief ASQR[20:18]: Channel 6 */
#define ADC_ASQR_SQ6_Pos                    (18U)
#define ADC_ASQR_SQ6_Msk                    (0x7UL << ADC_ASQR_SQ6_Pos)
#define ADC_ASQR_SQ6                        ADC_ASQR_SQ6_Msk
/** @brief ASQR[23:21]: Channel 7 */
#define ADC_ASQR_SQ7_Pos                    (21U)
#define ADC_ASQR_SQ7_Msk                    (0x7UL << ADC_ASQR_SQ7_Pos)
#define ADC_ASQR_SQ7                        ADC_ASQR_SQ7_Msk

/** @brief ASQR[26:24]: The length of the sequence */
#define ADC_ASQR_L_Pos                      (24U)
#define ADC_ASQR_L_Msk                      (0x7UL << ADC_ASQR_L_Pos)
#define ADC_ASQR_L                          ADC_ASQR_L_Msk

#define ADC_ASQR_L_1RANK                    (0x0UL << ADC_ASQR_L_Pos)   /*!< 1 rank */
#define ADC_ASQR_L_2RANKS                   (0x1UL << ADC_ASQR_L_Pos)   /*!< 2 ranks */
#define ADC_ASQR_L_3RANKS                   (0x2UL << ADC_ASQR_L_Pos)   /*!< 3 ranks */
#define ADC_ASQR_L_4RANKS                   (0x3UL << ADC_ASQR_L_Pos)   /*!< 4 ranks */
#define ADC_ASQR_L_5RANKS                   (0x4UL << ADC_ASQR_L_Pos)   /*!< 5 ranks */
#define ADC_ASQR_L_6RANKS                   (0x5UL << ADC_ASQR_L_Pos)   /*!< 6 ranks */
#define ADC_ASQR_L_7RANKS                   (0x6UL << ADC_ASQR_L_Pos)   /*!< 7 ranks */
#define ADC_ASQR_L_8RANKS                   (0x7UL << ADC_ASQR_L_Pos)   /*!< 8 ranks */

/********************  Bit definition for ADC_ADR register  *******************/

/** @brief ADR[11:0]: ADC data */
#define ADC_ADR_Pos                         (0U)
#define ADC_ADR_Msk                         (0xFFFUL << ADC_ADR_Pos)
#define ADC_ADR                             ADC_ADR_Msk

/******************************************************************************/
/*                      Advanced High-performance Bus                         */
/******************************************************************************/

/********************  Bit definition for AHB_PL register  ********************/

/** @brief PL[3:0]: Arbitration priority for master */
#define AHB_PL_Pos                          (0U)
#define AHB_PL_Msk                          (0xFUL << AHB_PL_Pos)
#define AHB_PL                              AHB_PL_Msk

/*****************  Bit definition for AHB_EBTCOUNT register  *****************/

/** @brief EBTCOUNT[9:0]: Early burst termination count */
#define AHB_EBTCOUNT_Pos                    (0U)
#define AHB_EBTCOUNT_Msk                    (0x3FFUL << AHB_EBTCOUNT_Pos)
#define AHB_EBTCOUNT                        AHB_EBTCOUNT_Msk

/******************  Bit definition for AHB_EBT_EN register  ******************/

/** @brief EBT_EN[0]: Early burst termination enable */
#define AHB_EBT_EN_Pos                      (0U)
#define AHB_EBT_EN_Msk                      (0x1UL << AHB_EBT_EN_Pos)
#define AHB_EBT_EN                          AHB_EBT_EN_Msk

/********************  Bit definition for AHB_EBT register  *******************/

/** @brief EBT[0]: Early burst termination */
#define AHB_EBT_Pos                         (0U)
#define AHB_EBT_Msk                         (0x1UL << AHB_EBT_Pos)
#define AHB_EBT                             AHB_EBT_Msk

/****************  Bit definition for AHB_DFLT_MASTER register  ***************/

/** @brief DFLT_MASTER[3:0]: Default master ID number */
#define AHB_DFLT_MASTER_Pos                 (0U)
#define AHB_DFLT_MASTER_Msk                 (0xFUL << AHB_DFLT_MASTER_Pos)
#define AHB_DFLT_MASTER                     AHB_DFLT_MASTER_Msk

/****************  Bit definition for AHB_VERSION_ID register  ****************/

/** @brief VERSION_ID[31:0]: Version ID */
#define AHB_VERSION_ID_Pos                  (0U)
#define AHB_VERSION_ID_Msk                  (0xFFFFFFFFUL << AHB_VERSION_ID_Pos)
#define AHB_VERSION_ID                      AHB_VERSION_ID_Msk

/******************************************************************************/
/*                      Advanced eXtensible Interface                         */
/******************************************************************************/

/****************  Bit definition for AXI_VERSION_ID register  ****************/

/** @brief VERSION_ID[31:0]: Version ID */
#define AXI_VERSION_ID_Pos                  (0U)
#define AXI_VERSION_ID_Msk                  (0xFFFFFFFFUL << AXI_VERSION_ID_Pos)
#define AXI_VERSION_ID                      AXI_VERSION_ID_Msk

/******************  Bit definition for AXI_HW_CFG register  ******************/

/** @brief HW_CFG[0]: QOS support */
#define AXI_HW_CFG_QOS_SUPPORT_Pos          (0U)
#define AXI_HW_CFG_QOS_SUPPORT_Msk          (0x1UL << AXI_HW_CFG_QOS_SUPPORT_Pos)
#define AXI_HW_CFG_QOS_SUPPORT              AXI_HW_CFG_QOS_SUPPORT_Msk
/** @brief HW_CFG[1]: APB3 support */
#define AXI_HW_CFG_APB3_SUPPORT_Pos         (1U)
#define AXI_HW_CFG_APB3_SUPPORT_Msk         (0x1UL << AXI_HW_CFG_APB3_SUPPORT_Pos)
#define AXI_HW_CFG_APB3_SUPPORT             AXI_HW_CFG_APB3_SUPPORT_Msk
/** @brief HW_CFG[2]: AXI4 features support */
#define AXI_HW_CFG_AXI4_SUPPORT_Pos         (2U)
#define AXI_HW_CFG_AXI4_SUPPORT_Msk         (0x1UL << AXI_HW_CFG_AXI4_SUPPORT_Pos)
#define AXI_HW_CFG_AXI4_SUPPORT             AXI_HW_CFG_AXI4_SUPPORT_Msk
/** @brief HW_CFG[3]:  Support for Enable Lock transactions */
#define AXI_HW_CFG_LOCK_EN_Pos              (3U)
#define AXI_HW_CFG_LOCK_EN_Msk              (0x1UL << AXI_HW_CFG_LOCK_EN_Pos)
#define AXI_HW_CFG_LOCK_EN                  AXI_HW_CFG_LOCK_EN_Msk
/** @brief HW_CFG[4]: Support for Enable Trust Zone */
#define AXI_HW_CFG_TRUST_ZONE_EN_Pos        (4U)
#define AXI_HW_CFG_TRUST_ZONE_EN_Msk        (0x1UL << AXI_HW_CFG_TRUST_ZONE_EN_Pos)
#define AXI_HW_CFG_TRUST_ZONE_EN            AXI_HW_CFG_TRUST_ZONE_EN_Msk
/** @brief HW_CFG[5]: Use of External/Internal Decoder */
#define AXI_HW_CFG_DECODER_TYPE_Pos         (5U)
#define AXI_HW_CFG_DECODER_TYPE_Msk         (0x1UL << AXI_HW_CFG_DECODER_TYPE_Pos)
#define AXI_HW_CFG_DECODER_TYPE             AXI_HW_CFG_DECODER_TYPE_Msk
/** @brief HW_CFG[6]: Support for Enable Remap mode */
#define AXI_HW_CFG_REMAP_EN_Pos             (6U)
#define AXI_HW_CFG_REMAP_EN_Msk             (0x1UL << AXI_HW_CFG_REMAP_EN_Pos)
#define AXI_HW_CFG_REMAP_EN                 AXI_HW_CFG_REMAP_EN_Msk
/** @brief HW_CFG[7]: Support for Enable Bi-directional Command */
#define AXI_HW_CFG_BI_DIR_CMD_EN_Pos        (7U)
#define AXI_HW_CFG_BI_DIR_CMD_EN_Msk        (0x1UL << AXI_HW_CFG_BI_DIR_CMD_EN_Pos)
#define AXI_HW_CFG_BI_DIR_CMD_EN            AXI_HW_CFG_BI_DIR_CMD_EN_Msk
/** @brief HW_CFG[8]: Include/exclude low-power interface */
#define AXI_HW_CFG_LOW_POWER_IF_Pos         (8U)
#define AXI_HW_CFG_LOW_POWER_IF_Msk         (0x1UL << AXI_HW_CFG_LOW_POWER_IF_Pos)
#define AXI_HW_CFG_LOW_POWER_IF             AXI_HW_CFG_LOW_POWER_IF_Msk
/** @brief HW_CFG[16:12]: Number of AXI masters */
#define AXI_HW_CFG_NUM_MASTERS_Pos          (12U)
#define AXI_HW_CFG_NUM_MASTERS_Msk          (0x1FUL << AXI_HW_CFG_NUM_MASTERS_Pos)
#define AXI_HW_CFG_NUM_MASTERS              AXI_HW_CFG_NUM_MASTERS_Msk
/** @brief HW_CFG[24:20]: Number of AXI slaves */
#define AXI_HW_CFG_NUM_SLAVES_Pos           (20U)
#define AXI_HW_CFG_NUM_SLAVES_Msk           (0x1FUL << AXI_HW_CFG_NUM_SLAVES_Pos)
#define AXI_HW_CFG_NUM_SLAVES               AXI_HW_CFG_NUM_SLAVES_Msk

/********************  Bit definition for AXI_CMD register  *******************/

/** @brief CMD[2:0]: Command selection for internal registers */
#define AXI_CMD_SEL_Pos                     (0U)
#define AXI_CMD_SEL_Msk                     (0x7UL << AXI_CMD_SEL_Pos)
#define AXI_CMD_SEL                         AXI_CMD_SEL_Msk

#define AXI_CMD_SEL_BURST_REG_EN            (0x0UL << AXI_CMD_SEL_Pos)  /*!< BURSTINESS_REGULATOR_EN register address offset */
#define AXI_CMD_SEL_PEAK_RATE_XCT_RATE      (0x1UL << AXI_CMD_SEL_Pos)  /*!< PEAK_RATE_XCT_RATE register address offset */
#define AXI_CMD_SEL_QOS_VALUE               (0x2UL << AXI_CMD_SEL_Pos)  /*!< QOS_VALUE register address offset */
#define AXI_CMD_SEL_SLV_READY               (0x3UL << AXI_CMD_SEL_Pos)  /*!< SLV_READY register address offset */

/** @brief CMD[7]: Read address channel / write address channel */
#define AXI_CMD_RD_WR_CHAN_Pos              (7U)
#define AXI_CMD_RD_WR_CHAN_Msk              (0x1UL << AXI_CMD_RD_WR_CHAN_Pos)
#define AXI_CMD_RD_WR_CHAN                  AXI_CMD_RD_WR_CHAN_Msk

#define AXI_CMD_RD_WR_CHAN_WRITE            (0x0UL << AXI_CMD_RD_WR_CHAN_Pos)   /*!< The command is for write address channel */
#define AXI_CMD_RD_WR_CHAN_READ             (0x1UL << AXI_CMD_RD_WR_CHAN_Pos)   /*!< The command is for read address channel */

/** @brief CMD[11:8]: Master port ID */
#define AXI_CMD_MASTER_PORT_Pos             (8U)
#define AXI_CMD_MASTER_PORT_Msk             (0xFUL << AXI_CMD_MASTER_PORT_Pos)
#define AXI_CMD_MASTER_PORT                 AXI_CMD_MASTER_PORT_Msk

#define AXI_CMD_MASTER_PORT_1               (0x0UL << AXI_CMD_MASTER_PORT_Pos)  /*!< Master port 1 */
#define AXI_CMD_MASTER_PORT_2               (0x1UL << AXI_CMD_MASTER_PORT_Pos)  /*!< Master port 2 */
#define AXI_CMD_MASTER_PORT_3               (0x2UL << AXI_CMD_MASTER_PORT_Pos)  /*!< Master port 3 */
#define AXI_CMD_MASTER_PORT_4               (0x3UL << AXI_CMD_MASTER_PORT_Pos)  /*!< Master port 4 */
#define AXI_CMD_MASTER_PORT_5               (0x4UL << AXI_CMD_MASTER_PORT_Pos)  /*!< Master port 5 */
#define AXI_CMD_MASTER_PORT_6               (0x5UL << AXI_CMD_MASTER_PORT_Pos)  /*!< Master port 6 */
#define AXI_CMD_MASTER_PORT_7               (0x6UL << AXI_CMD_MASTER_PORT_Pos)  /*!< Master port 7 */
#define AXI_CMD_MASTER_PORT_8               (0x7UL << AXI_CMD_MASTER_PORT_Pos)  /*!< Master port 8 */
#define AXI_CMD_MASTER_PORT_9               (0x8UL << AXI_CMD_MASTER_PORT_Pos)  /*!< Master port 9 */
#define AXI_CMD_MASTER_PORT_10              (0x9UL << AXI_CMD_MASTER_PORT_Pos)  /*!< Master port 10 */
#define AXI_CMD_MASTER_PORT_11              (0xAUL << AXI_CMD_MASTER_PORT_Pos)  /*!< Master port 11 */
#define AXI_CMD_MASTER_PORT_12              (0xBUL << AXI_CMD_MASTER_PORT_Pos)  /*!< Master port 12 */
#define AXI_CMD_MASTER_PORT_13              (0xCUL << AXI_CMD_MASTER_PORT_Pos)  /*!< Master port 13 */
#define AXI_CMD_MASTER_PORT_14              (0xDUL << AXI_CMD_MASTER_PORT_Pos)  /*!< Master port 14 */
#define AXI_CMD_MASTER_PORT_15              (0xEUL << AXI_CMD_MASTER_PORT_Pos)  /*!< Master port 15 */
#define AXI_CMD_MASTER_PORT_16              (0xFUL << AXI_CMD_MASTER_PORT_Pos)  /*!< Master port 16 */

/** @brief CMD[28]: Error bit */
#define AXI_CMD_ERR_BIT_Pos                 (28U)
#define AXI_CMD_ERR_BIT_Msk                 (0x1UL << AXI_CMD_ERR_BIT_Pos)
#define AXI_CMD_ERR_BIT                     AXI_CMD_ERR_BIT_Msk
/** @brief CMD[29]: Internal register reset */
#define AXI_CMD_SOFT_RESET_BIT_Pos          (29U)
#define AXI_CMD_SOFT_RESET_BIT_Msk          (0x1UL << AXI_CMD_SOFT_RESET_BIT_Pos)
#define AXI_CMD_SOFT_RESET_BIT              AXI_CMD_SOFT_RESET_BIT_Msk

/** @brief CMD[30]: Read command / write command */
#define AXI_CMD_RD_WR_CMD_Pos               (30U)
#define AXI_CMD_RD_WR_CMD_Msk               (0x1UL << AXI_CMD_RD_WR_CMD_Pos)
#define AXI_CMD_RD_WR_CMD                   AXI_CMD_RD_WR_CMD_Msk

#define AXI_CMD_RD_WR_CMD_READ              (0x0UL << AXI_CMD_RD_WR_CMD_Pos)    /*!< Read command */
#define AXI_CMD_RD_WR_CMD_WRITE             (0x1UL << AXI_CMD_RD_WR_CMD_Pos)    /*!< Write command */

/** @brief CMD[31]: Command enable */
#define AXI_CMD_EN_Pos                      (31U)
#define AXI_CMD_EN_Msk                      (0x1UL << AXI_CMD_EN_Pos)
#define AXI_CMD_EN                          AXI_CMD_EN_Msk

/*******************  Bit definition for AXI_DATA register  *******************/

/** @brief DATA[31:0]: Data to be written into / read from the QoS internal register */
#define AXI_DATA_Pos                        (0U)
#define AXI_DATA_Msk                        (0xFFFFFFFFUL << AXI_DATA_Pos)
#define AXI_DATA                            AXI_DATA_Msk

/******************************************************************************/
/*                          Controller Area Network                           */
/******************************************************************************/

/*******************  Bit definition for CANFD_SRR register  ******************/

/** @brief SRR[0]: Reset */
#define CANFD_SRR_SRST_Pos                  (0U)
#define CANFD_SRR_SRST_Msk                  (0x1UL << CANFD_SRR_SRST_Pos)
#define CANFD_SRR_SRST                      CANFD_SRR_SRST_Msk
/** @brief SRR[1]: CAN enable */
#define CANFD_SRR_CEN_Pos                   (1U)
#define CANFD_SRR_CEN_Msk                   (0x1UL << CANFD_SRR_CEN_Pos)
#define CANFD_SRR_CEN                       CANFD_SRR_CEN_Msk

/*******************  Bit definition for CANFD_MSR register  ******************/

/** @brief MSR[0]: Sleep mode select/request */
#define CANFD_MSR_SLEEP_Pos                 (0U)
#define CANFD_MSR_SLEEP_Msk                 (0x1UL << CANFD_MSR_SLEEP_Pos)
#define CANFD_MSR_SLEEP                     CANFD_MSR_SLEEP_Msk
/** @brief MSR[1]: Loopback mode select/request */
#define CANFD_MSR_LBACK_Pos                 (1U)
#define CANFD_MSR_LBACK_Msk                 (0x1UL << CANFD_MSR_LBACK_Pos)
#define CANFD_MSR_LBACK                     CANFD_MSR_LBACK_Msk
/** @brief MSR[2]: Snoop mode select/request */
#define CANFD_MSR_SNOOP_Pos                 (2U)
#define CANFD_MSR_SNOOP_Msk                 (0x1UL << CANFD_MSR_SNOOP_Pos)
#define CANFD_MSR_SNOOP                     CANFD_MSR_SNOOP_Msk
/** @brief MSR[3]: CAN FD bitrate switch disable override */
#define CANFD_MSR_BRSD_Pos                  (3U)
#define CANFD_MSR_BRSD_Msk                  (0x1UL << CANFD_MSR_BRSD_Pos)
#define CANFD_MSR_BRSD                      CANFD_MSR_BRSD_Msk
/** @brief MSR[4]: Disable auto retransmission */
#define CANFD_MSR_DAR_Pos                   (4U)
#define CANFD_MSR_DAR_Msk                   (0x1UL << CANFD_MSR_DAR_Pos)
#define CANFD_MSR_DAR                       CANFD_MSR_DAR_Msk
/** @brief MSR[5]: Disable protocol exception event detection/generation */
#define CANFD_MSR_DPEE_Pos                  (5U)
#define CANFD_MSR_DPEE_Msk                  (0x1UL << CANFD_MSR_DPEE_Pos)
#define CANFD_MSR_DPEE                      CANFD_MSR_DPEE_Msk
/** @brief MSR[6]: Start bus-off recovery request */
#define CANFD_MSR_SBR_Pos                   (6U)
#define CANFD_MSR_SBR_Msk                   (0x1UL << CANFD_MSR_SBR_Pos)
#define CANFD_MSR_SBR                       CANFD_MSR_SBR_Msk
/** @brief MSR[7]: Auto bus-off recovery request */
#define CANFD_MSR_ABR_Pos                   (7U)
#define CANFD_MSR_ABR_Msk                   (0x1UL << CANFD_MSR_ABR_Pos)
#define CANFD_MSR_ABR                       CANFD_MSR_ABR_Msk
/** @brief MSR[15:8]: Internal timing optimization */
#define CANFD_MSR_ITO_Pos                   (8U)
#define CANFD_MSR_ITO_Msk                   (0xFFUL << CANFD_MSR_ITO_Pos)
#define CANFD_MSR_ITO                       CANFD_MSR_ITO_Msk

/******************  Bit definition for CANFD_BRPR register  ******************/

/** @brief BRPR[7:0]: Arbitration phase baudrate prescaler */
#define CANFD_BRPR_PR_Pos                   (0U)
#define CANFD_BRPR_PR_Msk                   (0xFFUL << CANFD_BRPR_PR_Pos)
#define CANFD_BRPR_PR                       CANFD_BRPR_PR_Msk

/******************  Bit definition for CANFD_BTR register  *******************/

/** @brief BTR[7:0]: Time segment 1 */
#define CANFD_BTR_TS1_Pos                   (0U)
#define CANFD_BTR_TS1_Msk                   (0xFFUL << CANFD_BTR_TS1_Pos)
#define CANFD_BTR_TS1                       CANFD_BTR_TS1_Msk
/** @brief BTR[14:8]: Time segment 2 */
#define CANFD_BTR_TS2_Pos                   (8U)
#define CANFD_BTR_TS2_Msk                   (0x7FUL << CANFD_BTR_TS2_Pos)
#define CANFD_BTR_TS2                       CANFD_BTR_TS2_Msk
/** @brief BTR[22:16]: Synchronization jump width */
#define CANFD_BTR_SJW_Pos                   (16U)
#define CANFD_BTR_SJW_Msk                   (0x7FUL << CANFD_BTR_SJW_Pos)
#define CANFD_BTR_SJW                       CANFD_BTR_SJW_Msk

/******************  Bit definition for CANFD_ECR register  *******************/

/** @brief ECR[7:0]: Transmit error count */
#define CANFD_ECR_TEC_Pos                   (0U)
#define CANFD_ECR_TEC_Msk                   (0xFFUL << CANFD_ECR_TEC_Pos)
#define CANFD_ECR_TEC                       CANFD_ECR_TEC_Msk
/** @brief ECR[15:8]: Receive error count */
#define CANFD_ECR_REC_Pos                   (8U)
#define CANFD_ECR_REC_Msk                   (0xFFUL << CANFD_ECR_REC_Pos)
#define CANFD_ECR_REC                       CANFD_ECR_REC_Msk

/******************  Bit definition for CANFD_ESR register  *******************/

/** @brief ESR[0]: CRC error */
#define CANFD_ESR_CRCER_Pos                 (0U)
#define CANFD_ESR_CRCER_Msk                 (0x1UL << CANFD_ESR_CRCER_Pos)
#define CANFD_ESR_CRCER                     CANFD_ESR_CRCER_Msk
/** @brief ESR[1]: Form error */
#define CANFD_ESR_FMER_Pos                  (1U)
#define CANFD_ESR_FMER_Msk                  (0x1UL << CANFD_ESR_FMER_Pos)
#define CANFD_ESR_FMER                      CANFD_ESR_FMER_Msk
/** @brief ESR[2]: Stuff error */
#define CANFD_ESR_STER_Pos                  (2U)
#define CANFD_ESR_STER_Msk                  (0x1UL << CANFD_ESR_STER_Pos)
#define CANFD_ESR_STER                      CANFD_ESR_STER_Msk
/** @brief ESR[3]: Bit error */
#define CANFD_ESR_BERR_Pos                  (3U)
#define CANFD_ESR_BERR_Msk                  (0x1UL << CANFD_ESR_BERR_Pos)
#define CANFD_ESR_BERR                      CANFD_ESR_BERR_Msk
/** @brief ESR[4]: ACK error */
#define CANFD_ESR_ACKER_Pos                 (4U)
#define CANFD_ESR_ACKER_Msk                 (0x1UL << CANFD_ESR_ACKER_Pos)
#define CANFD_ESR_ACKER                     CANFD_ESR_ACKER_Msk
/** @brief ESR[8]: CRC error in CAN FD data phase */
#define CANFD_ESR_F_CRCER_Pos               (8U)
#define CANFD_ESR_F_CRCER_Msk               (0x1UL << CANFD_ESR_F_CRCER_Pos)
#define CANFD_ESR_F_CRCER                   CANFD_ESR_F_CRCER_Msk
/** @brief ESR[9]: Form error in CAN FD data phase */
#define CANFD_ESR_F_FMER_Pos                (9U)
#define CANFD_ESR_F_FMER_Msk                (0x1UL << CANFD_ESR_F_FMER_Pos)
#define CANFD_ESR_F_FMER                    CANFD_ESR_F_FMER_Msk
/** @brief ESR[10]: Stuff error in CAN FD data phase */
#define CANFD_ESR_F_STER_Pos                (10U)
#define CANFD_ESR_F_STER_Msk                (0x1UL << CANFD_ESR_F_STER_Pos)
#define CANFD_ESR_F_STER                    CANFD_ESR_F_STER_Msk
/** @brief ESR[11]: Bit error in CAN FD data phase */
#define CANFD_ESR_F_BERR_Pos                (11U)
#define CANFD_ESR_F_BERR_Msk                (0x1UL << CANFD_ESR_F_BERR_Pos)
#define CANFD_ESR_F_BERR                    CANFD_ESR_F_BERR_Msk

/*******************  Bit definition for CANFD_SR register  *******************/

/** @brief SR[0]: Configuration mode indicator */
#define CANFD_SR_CONFIG_Pos                 (0U)
#define CANFD_SR_CONFIG_Msk                 (0x1UL << CANFD_SR_CONFIG_Pos)
#define CANFD_SR_CONFIG                     CANFD_SR_CONFIG_Msk
/** @brief SR[1]: Loopback mode */
#define CANFD_SR_LBACK_Pos                  (1U)
#define CANFD_SR_LBACK_Msk                  (0x1UL << CANFD_SR_LBACK_Pos)
#define CANFD_SR_LBACK                      CANFD_SR_LBACK_Msk
/** @brief SR[2]: Sleep mode */
#define CANFD_SR_SLEEP_Pos                  (2U)
#define CANFD_SR_SLEEP_Msk                  (0x1UL << CANFD_SR_SLEEP_Pos)
#define CANFD_SR_SLEEP                      CANFD_SR_SLEEP_Msk
/** @brief SR[3]: Normal mode */
#define CANFD_SR_NORMAL_Pos                 (3U)
#define CANFD_SR_NORMAL_Msk                 (0x1UL << CANFD_SR_NORMAL_Pos)
#define CANFD_SR_NORMAL                     CANFD_SR_NORMAL_Msk
/** @brief SR[4]: Bus idle */
#define CANFD_SR_BIDLE_Pos                  (4U)
#define CANFD_SR_BIDLE_Msk                  (0x1UL << CANFD_SR_BIDLE_Pos)
#define CANFD_SR_BIDLE                      CANFD_SR_BIDLE_Msk
/** @brief SR[5]: Bus busy */
#define CANFD_SR_BBSY_Pos                   (5U)
#define CANFD_SR_BBSY_Msk                   (0x1UL << CANFD_SR_BBSY_Pos)
#define CANFD_SR_BBSY                       CANFD_SR_BBSY_Msk
/** @brief SR[6]: Error warning */
#define CANFD_SR_ERRWRN_Pos                 (6U)
#define CANFD_SR_ERRWRN_Msk                 (0x1UL << CANFD_SR_ERRWRN_Pos)
#define CANFD_SR_ERRWRN                     CANFD_SR_ERRWRN_Msk

/** @brief SR[8:7]: Error status */
#define CANFD_SR_ESTAT_Pos                  (7U)
#define CANFD_SR_ESTAT_Msk                  (0x3UL << CANFD_SR_ESTAT_Pos)
#define CANFD_SR_ESTAT                      CANFD_SR_ESTAT_Msk

#define CANFD_SR_ESTAT_CONFIG               (0x0UL << CANFD_SR_ESTAT_Pos)   /*!< Configuration mode, error state is undefined */
#define CANFD_SR_ESTAT_ACTIVE               (0x1UL << CANFD_SR_ESTAT_Pos)   /*!< Error active state */
#define CANFD_SR_ESTAT_BUSOFF               (0x2UL << CANFD_SR_ESTAT_Pos)   /*!< Bus-off state */
#define CANFD_SR_ESTAT_PASSIVE              (0x3UL << CANFD_SR_ESTAT_Pos)   /*!< Error passive state */

/** @brief SR[9]: PEE mode indicator */
#define CANFD_SR_PEE_CONFIG_Pos             (9U)
#define CANFD_SR_PEE_CONFIG_Msk             (0x1UL << CANFD_SR_PEE_CONFIG_Pos)
#define CANFD_SR_PEE_CONFIG                 CANFD_SR_PEE_CONFIG_Msk
/** @brief SR[10]: Bus-off recovery mode indicator */
#define CANFD_SR_BSFR_CONFIG_Pos            (10U)
#define CANFD_SR_BSFR_CONFIG_Msk            (0x1UL << CANFD_SR_BSFR_CONFIG_Pos)
#define CANFD_SR_BSFR_CONFIG                CANFD_SR_BSFR_CONFIG_Msk
/** @brief SR[12]: Snoop mode */
#define CANFD_SR_SNOOP_Pos                  (12U)
#define CANFD_SR_SNOOP_Msk                  (0x1UL << CANFD_SR_SNOOP_Pos)
#define CANFD_SR_SNOOP                      CANFD_SR_SNOOP_Msk
/** @brief SR[22:16]: Transmitter delay compensation value */
#define CANFD_SR_TDCV_Pos                   (16U)
#define CANFD_SR_TDCV_Msk                   (0x7FUL << CANFD_SR_TDCV_Pos)
#define CANFD_SR_TDCV                       CANFD_SR_TDCV_Msk

/*******************  Bit definition for CANFD_ISR register  ******************/

/** @brief ISR[0]: Arbitration lost interrupt */
#define CANFD_ISR_ARBLOST_Pos               (0U)
#define CANFD_ISR_ARBLOST_Msk               (0x1UL << CANFD_ISR_ARBLOST_Pos)
#define CANFD_ISR_ARBLOST                   CANFD_ISR_ARBLOST_Msk
/** @brief ISR[1]: Transmission successful interrupt */
#define CANFD_ISR_TXOK_Pos                  (1U)
#define CANFD_ISR_TXOK_Msk                  (0x1UL << CANFD_ISR_TXOK_Pos)
#define CANFD_ISR_TXOK                      CANFD_ISR_TXOK_Msk
/** @brief ISR[2]: Protocol exception event interrupt */
#define CANFD_ISR_PEE_Pos                   (2U)
#define CANFD_ISR_PEE_Msk                   (0x1UL << CANFD_ISR_PEE_Pos)
#define CANFD_ISR_PEE                       CANFD_ISR_PEE_Msk
/** @brief ISR[3]: Bus-off recovery done interrupt */
#define CANFD_ISR_BSFRD_Pos                 (3U)
#define CANFD_ISR_BSFRD_Msk                 (0x1UL << CANFD_ISR_BSFRD_Pos)
#define CANFD_ISR_BSFRD                     CANFD_ISR_BSFRD_Msk
/** @brief ISR[4]: New message received interrupt */
#define CANFD_ISR_RXOK_Pos                  (4U)
#define CANFD_ISR_RXOK_Msk                  (0x1UL << CANFD_ISR_RXOK_Pos)
#define CANFD_ISR_RXOK                      CANFD_ISR_RXOK_Msk
/** @brief ISR[5]: Timestamp counter overflow interrupt */
#define CANFD_ISR_TSCNT_OFLW_Pos            (5U)
#define CANFD_ISR_TSCNT_OFLW_Msk            (0x1UL << CANFD_ISR_TSCNT_OFLW_Pos)
#define CANFD_ISR_TSCNT_OFLW                CANFD_ISR_TSCNT_OFLW_Msk
/** @brief ISR[6]: Rx FIFO-0 overflow interrupt */
#define CANFD_ISR_RXFOFLW_Pos               (6U)
#define CANFD_ISR_RXFOFLW_Msk               (0x1UL << CANFD_ISR_RXFOFLW_Pos)
#define CANFD_ISR_RXFOFLW                   CANFD_ISR_RXFOFLW_Msk
/** @brief ISR[8]: Error interrupt */
#define CANFD_ISR_ERROR_Pos                 (8U)
#define CANFD_ISR_ERROR_Msk                 (0x1UL << CANFD_ISR_ERROR_Pos)
#define CANFD_ISR_ERROR                     CANFD_ISR_ERROR_Msk
/** @brief ISR[9]: Bus-off interrupt */
#define CANFD_ISR_BSOFF_Pos                 (9U)
#define CANFD_ISR_BSOFF_Msk                 (0x1UL << CANFD_ISR_BSOFF_Pos)
#define CANFD_ISR_BSOFF                     CANFD_ISR_BSOFF_Msk
/** @brief ISR[10]: Sleep interrupt */
#define CANFD_ISR_SLP_Pos                   (10U)
#define CANFD_ISR_SLP_Msk                   (0x1UL << CANFD_ISR_SLP_Pos)
#define CANFD_ISR_SLP                       CANFD_ISR_SLP_Msk
/** @brief ISR[11]: Wake-up interrupt */
#define CANFD_ISR_WKUP_Pos                  (11U)
#define CANFD_ISR_WKUP_Msk                  (0x1UL << CANFD_ISR_WKUP_Pos)
#define CANFD_ISR_WKUP                      CANFD_ISR_WKUP_Msk
/** @brief ISR[12]: Rx FIFO-0 watermark full interrupt */
#define CANFD_ISR_RXFWMFLL_Pos              (12U)
#define CANFD_ISR_RXFWMFLL_Msk              (0x1UL << CANFD_ISR_RXFWMFLL_Pos)
#define CANFD_ISR_RXFWMFLL                  CANFD_ISR_RXFWMFLL_Msk
/** @brief ISR[13]: Tx buffer ready request served interrupt */
#define CANFD_ISR_TXRRS_Pos                 (13U)
#define CANFD_ISR_TXRRS_Msk                 (0x1UL << CANFD_ISR_TXRRS_Pos)
#define CANFD_ISR_TXRRS                     CANFD_ISR_TXRRS_Msk
/** @brief ISR[14]: Tx cancellation request served interrupt */
#define CANFD_ISR_TXCRS_Pos                 (14U)
#define CANFD_ISR_TXCRS_Msk                 (0x1UL << CANFD_ISR_TXCRS_Pos)
#define CANFD_ISR_TXCRS                     CANFD_ISR_TXCRS_Msk
/** @brief ISR[15]: Rx FIFO-1 overflow interrupt */
#define CANFD_ISR_RXFOFLW_1_Pos             (15U)
#define CANFD_ISR_RXFOFLW_1_Msk             (0x1UL << CANFD_ISR_RXFOFLW_1_Pos)
#define CANFD_ISR_RXFOFLW_1                 CANFD_ISR_RXFOFLW_1_Msk
/** @brief ISR[16]: Rx FIFO-1 watermark full interrupt */
#define CANFD_ISR_RXFWMFLL_1_Pos            (16U)
#define CANFD_ISR_RXFWMFLL_1_Msk            (0x1UL << CANFD_ISR_RXFWMFLL_1_Pos)
#define CANFD_ISR_RXFWMFLL_1                CANFD_ISR_RXFWMFLL_1_Msk
/** @brief ISR[17]: Rx match not finished interrupt */
#define CANFD_ISR_RXMNF_Pos                 (17U)
#define CANFD_ISR_RXMNF_Msk                 (0x1UL << CANFD_ISR_RXMNF_Pos)
#define CANFD_ISR_RXMNF                     CANFD_ISR_RXMNF_Msk
/** @brief ISR[30]: Tx event FIFO overflow interrupt */
#define CANFD_ISR_TXEOFLW_Pos               (30U)
#define CANFD_ISR_TXEOFLW_Msk               (0x1UL << CANFD_ISR_TXEOFLW_Pos)
#define CANFD_ISR_TXEOFLW                   CANFD_ISR_TXEOFLW_Msk
/** @brief ISR[31]: Tx event FIFO watermark full interrupt */
#define CANFD_ISR_TXEWMFLL_Pos              (31U)
#define CANFD_ISR_TXEWMFLL_Msk              (0x1UL << CANFD_ISR_TXEWMFLL_Pos)
#define CANFD_ISR_TXEWMFLL                  CANFD_ISR_TXEWMFLL_Msk

/*******************  Bit definition for CANFD_IER register  ******************/

/** @brief IER[0]: Arbitration lost interrupt enable */
#define CANFD_IER_EARBLOST_Pos              (0U)
#define CANFD_IER_EARBLOST_Msk              (0x1UL << CANFD_IER_EARBLOST_Pos)
#define CANFD_IER_EARBLOST                  CANFD_IER_EARBLOST_Msk
/** @brief IER[1]: Transmission successful interrupt enable */
#define CANFD_IER_ETXOK_Pos                 (1U)
#define CANFD_IER_ETXOK_Msk                 (0x1UL << CANFD_IER_ETXOK_Pos)
#define CANFD_IER_ETXOK                     CANFD_IER_ETXOK_Msk
/** @brief IER[2]: Protocol exception event interrupt enable */
#define CANFD_IER_EPEE_Pos                  (2U)
#define CANFD_IER_EPEE_Msk                  (0x1UL << CANFD_IER_EPEE_Pos)
#define CANFD_IER_EPEE                      CANFD_IER_EPEE_Msk
/** @brief IER[3]: Bus-off recovery done interrupt enable */
#define CANFD_IER_EBSFRD_Pos                (3U)
#define CANFD_IER_EBSFRD_Msk                (0x1UL << CANFD_IER_EBSFRD_Pos)
#define CANFD_IER_EBSFRD                    CANFD_IER_EBSFRD_Msk
/** @brief IER[4]: New message received interrupt enable */
#define CANFD_IER_ERXOK_Pos                 (4U)
#define CANFD_IER_ERXOK_Msk                 (0x1UL << CANFD_IER_ERXOK_Pos)
#define CANFD_IER_ERXOK                     CANFD_IER_ERXOK_Msk
/** @brief IER[5]: Timestamp counter overflow interrupt enable */
#define CANFD_IER_ETSCNT_OFLW_Pos           (5U)
#define CANFD_IER_ETSCNT_OFLW_Msk           (0x1UL << CANFD_IER_ETSCNT_OFLW_Pos)
#define CANFD_IER_ETSCNT_OFLW               CANFD_IER_ETSCNT_OFLW_Msk
/** @brief IER[6]: Rx FIFO-0 overflow interrupt enable */
#define CANFD_IER_ERXFOFLW_Pos              (6U)
#define CANFD_IER_ERXFOFLW_Msk              (0x1UL << CANFD_IER_ERXFOFLW_Pos)
#define CANFD_IER_ERXFOFLW                  CANFD_IER_ERXFOFLW_Msk
/** @brief IER[8]: Error interrupt enable */
#define CANFD_IER_EERROR_Pos                (8U)
#define CANFD_IER_EERROR_Msk                (0x1UL << CANFD_IER_EERROR_Pos)
#define CANFD_IER_EERROR                    CANFD_IER_EERROR_Msk
/** @brief IER[9]: Bus-off interrupt enable */
#define CANFD_IER_EBSOFF_Pos                (9U)
#define CANFD_IER_EBSOFF_Msk                (0x1UL << CANFD_IER_EBSOFF_Pos)
#define CANFD_IER_EBSOFF                    CANFD_IER_EBSOFF_Msk
/** @brief IER[10]: Sleep interrupt enable */
#define CANFD_IER_ESLP_Pos                  (10U)
#define CANFD_IER_ESLP_Msk                  (0x1UL << CANFD_IER_ESLP_Pos)
#define CANFD_IER_ESLP                      CANFD_IER_ESLP_Msk
/** @brief IER[11]: Wake-up interrupt enable */
#define CANFD_IER_EWKUP_Pos                 (11U)
#define CANFD_IER_EWKUP_Msk                 (0x1UL << CANFD_IER_EWKUP_Pos)
#define CANFD_IER_EWKUP                     CANFD_IER_EWKUP_Msk
/** @brief IER[12]: Rx FIFO-0 watermark full interrupt enable */
#define CANFD_IER_ERXFWMFLL_Pos             (12U)
#define CANFD_IER_ERXFWMFLL_Msk             (0x1UL << CANFD_IER_ERXFWMFLL_Pos)
#define CANFD_IER_ERXFWMFLL                 CANFD_IER_ERXFWMFLL_Msk
/** @brief IER[13]: Tx buffer ready request served interrupt enable */
#define CANFD_IER_ETXRRS_Pos                (13U)
#define CANFD_IER_ETXRRS_Msk                (0x1UL << CANFD_IER_ETXRRS_Pos)
#define CANFD_IER_ETXRRS                    CANFD_IER_ETXRRS_Msk
/** @brief IER[14]: Tx cancellation request served interrupt enable */
#define CANFD_IER_ETXCRS_Pos                (14U)
#define CANFD_IER_ETXCRS_Msk                (0x1UL << CANFD_IER_ETXCRS_Pos)
#define CANFD_IER_ETXCRS                    CANFD_IER_ETXCRS_Msk
/** @brief IER[15]: Rx FIFO-1 overflow interrupt enable */
#define CANFD_IER_ERXFOFLW_1_Pos            (15U)
#define CANFD_IER_ERXFOFLW_1_Msk            (0x1UL << CANFD_IER_ERXFOFLW_1_Pos)
#define CANFD_IER_ERXFOFLW_1                CANFD_IER_ERXFOFLW_1_Msk
/** @brief IER[16]: Rx FIFO-1 watermark full interrupt enable */
#define CANFD_IER_ERXFWMFLL_1_Pos           (16U)
#define CANFD_IER_ERXFWMFLL_1_Msk           (0x1UL << CANFD_IER_ERXFWMFLL_1_Pos)
#define CANFD_IER_ERXFWMFLL_1               CANFD_IER_ERXFWMFLL_1_Msk
/** @brief IER[17]: Rx match not finished interrupt enable */
#define CANFD_IER_ERXMNF_Pos                (17U)
#define CANFD_IER_ERXMNF_Msk                (0x1UL << CANFD_IER_ERXMNF_Pos)
#define CANFD_IER_ERXMNF                    CANFD_IER_ERXMNF_Msk
/** @brief IER[30]: Tx event FIFO overflow interrupt enable */
#define CANFD_IER_ETXEOFLW_Pos              (30U)
#define CANFD_IER_ETXEOFLW_Msk              (0x1UL << CANFD_IER_ETXEOFLW_Pos)
#define CANFD_IER_ETXEOFLW                  CANFD_IER_ETXEOFLW_Msk
/** @brief IER[31]: Tx event FIFO watermark full interrupt enable */
#define CANFD_IER_ETXEWMFLL_Pos             (31U)
#define CANFD_IER_ETXEWMFLL_Msk             (0x1UL << CANFD_IER_ETXEWMFLL_Pos)
#define CANFD_IER_ETXEWMFLL                 CANFD_IER_ETXEWMFLL_Msk

/*******************  Bit definition for CANFD_ICR register  ******************/

/** @brief ICR[0]: Clear arbitration lost interrupt status bit */
#define CANFD_ICR_CARBLOST_Pos              (0U)
#define CANFD_ICR_CARBLOST_Msk              (0x1UL << CANFD_ICR_CARBLOST_Pos)
#define CANFD_ICR_CARBLOST                  CANFD_ICR_CARBLOST_Msk
/** @brief ICR[1]: Clear transmission successful interrupt status bit */
#define CANFD_ICR_CTXOK_Pos                 (1U)
#define CANFD_ICR_CTXOK_Msk                 (0x1UL << CANFD_ICR_CTXOK_Pos)
#define CANFD_ICR_CTXOK                     CANFD_ICR_CTXOK_Msk
/** @brief ICR[2]: Clear  protocol exception event interrupt status bit */
#define CANFD_ICR_CPEE_Pos                  (2U)
#define CANFD_ICR_CPEE_Msk                  (0x1UL << CANFD_ICR_CPEE_Pos)
#define CANFD_ICR_CPEE                      CANFD_ICR_CPEE_Msk
/** @brief ICR[3]: Clear bus-off recovery done interrupt status bit */
#define CANFD_ICR_CBSFRD_Pos                (3U)
#define CANFD_ICR_CBSFRD_Msk                (0x1UL << CANFD_ICR_CBSFRD_Pos)
#define CANFD_ICR_CBSFRD                    CANFD_ICR_CBSFRD_Msk
/** @brief ICR[4]: Clear new message received interrupt status bit */
#define CANFD_ICR_CRXOK_Pos                 (4U)
#define CANFD_ICR_CRXOK_Msk                 (0x1UL << CANFD_ICR_CRXOK_Pos)
#define CANFD_ICR_CRXOK                     CANFD_ICR_CRXOK_Msk
/** @brief ICR[5]: Clear timestamp counter overflow interrupt status bit */
#define CANFD_ICR_CTSCNT_OFLW_Pos           (5U)
#define CANFD_ICR_CTSCNT_OFLW_Msk           (0x1UL << CANFD_ICR_CTSCNT_OFLW_Pos)
#define CANFD_ICR_CTSCNT_OFLW               CANFD_ICR_CTSCNT_OFLW_Msk
/** @brief ICR[6]: Clear Rx FIFO-0 overflow interrupt status bit */
#define CANFD_ICR_CRXFOFLW_Pos              (6U)
#define CANFD_ICR_CRXFOFLW_Msk              (0x1UL << CANFD_ICR_CRXFOFLW_Pos)
#define CANFD_ICR_CRXFOFLW                  CANFD_ICR_CRXFOFLW_Msk
/** @brief ICR[8]: Clear error interrupt status bit */
#define CANFD_ICR_CERROR_Pos                (8U)
#define CANFD_ICR_CERROR_Msk                (0x1UL << CANFD_ICR_CERROR_Pos)
#define CANFD_ICR_CERROR                    CANFD_ICR_CERROR_Msk
/** @brief ICR[9]: Clear bus-off interrupt status bit */
#define CANFD_ICR_CBSOFF_Pos                (9U)
#define CANFD_ICR_CBSOFF_Msk                (0x1UL << CANFD_ICR_CBSOFF_Pos)
#define CANFD_ICR_CBSOFF                    CANFD_ICR_CBSOFF_Msk
/** @brief ICR[10]: Clear sleep interrupt status bit */
#define CANFD_ICR_CSLP_Pos                  (10U)
#define CANFD_ICR_CSLP_Msk                  (0x1UL << CANFD_ICR_CSLP_Pos)
#define CANFD_ICR_CSLP                      CANFD_ICR_CSLP_Msk
/** @brief ICR[11]: Clear wake-up interrupt status bit */
#define CANFD_ICR_CWKUP_Pos                 (11U)
#define CANFD_ICR_CWKUP_Msk                 (0x1UL << CANFD_ICR_CWKUP_Pos)
#define CANFD_ICR_CWKUP                     CANFD_ICR_CWKUP_Msk
/** @brief ICR[12]: Clear Rx FIFO-0 watermark full interrupt status bit */
#define CANFD_ICR_CRXFWMFLL_Pos             (12U)
#define CANFD_ICR_CRXFWMFLL_Msk             (0x1UL << CANFD_ICR_CRXFWMFLL_Pos)
#define CANFD_ICR_CRXFWMFLL                 CANFD_ICR_CRXFWMFLL_Msk
/** @brief ICR[13]: Clear Tx buffer ready request served interrupt status bit */
#define CANFD_ICR_CTXRRS_Pos                (13U)
#define CANFD_ICR_CTXRRS_Msk                (0x1UL << CANFD_ICR_CTXRRS_Pos)
#define CANFD_ICR_CTXRRS                    CANFD_ICR_CTXRRS_Msk
/** @brief ICR[14]: Clear Tx cancellation request served interrupt status bit */
#define CANFD_ICR_CTXCRS_Pos                (14U)
#define CANFD_ICR_CTXCRS_Msk                (0x1UL << CANFD_ICR_CTXCRS_Pos)
#define CANFD_ICR_CTXCRS                    CANFD_ICR_CTXCRS_Msk
/** @brief ICR[15]: Clear Rx FIFO-1 overflow interrupt status bit */
#define CANFD_ICR_CRXFOFLW_1_Pos            (15U)
#define CANFD_ICR_CRXFOFLW_1_Msk            (0x1UL << CANFD_ICR_CRXFOFLW_1_Pos)
#define CANFD_ICR_CRXFOFLW_1                CANFD_ICR_CRXFOFLW_1_Msk
/** @brief ICR[16]: Clear Rx FIFO-1 watermark full interrupt status bit */
#define CANFD_ICR_CRXFWMFLL_1_Pos           (16U)
#define CANFD_ICR_CRXFWMFLL_1_Msk           (0x1UL << CANFD_ICR_CRXFWMFLL_1_Pos)
#define CANFD_ICR_CRXFWMFLL_1               CANFD_ICR_CRXFWMFLL_1_Msk
/** @brief ICR[17]: Clear Rx match not finished interrupt status bit */
#define CANFD_ICR_CRXMNF_Pos                (17U)
#define CANFD_ICR_CRXMNF_Msk                (0x1UL << CANFD_ICR_CRXMNF_Pos)
#define CANFD_ICR_CRXMNF                    CANFD_ICR_CRXMNF_Msk
/** @brief ICR[30]: Clear Tx event FIFO overflow interrupt status bit */
#define CANFD_ICR_CTXEOFLW_Pos              (30U)
#define CANFD_ICR_CTXEOFLW_Msk              (0x1UL << CANFD_ICR_CTXEOFLW_Pos)
#define CANFD_ICR_CTXEOFLW                  CANFD_ICR_CTXEOFLW_Msk
/** @brief ICR[31]: Clear Tx event FIFO watermark full interrupt status bit */
#define CANFD_ICR_CTXEWMFLL_Pos             (31U)
#define CANFD_ICR_CTXEWMFLL_Msk             (0x1UL << CANFD_ICR_CTXEWMFLL_Pos)
#define CANFD_ICR_CTXEWMFLL                 CANFD_ICR_CTXEWMFLL_Msk

/*******************  Bit definition for CANFD_TSR register  ******************/

/** @brief TSR[0]: Clear timestamp counter */
#define CANFD_TSR_CTS_Pos                   (0U)
#define CANFD_TSR_CTS_Msk                   (0x1UL << CANFD_TSR_CTS_Pos)
#define CANFD_TSR_CTS                       CANFD_TSR_CTS_Msk
/** @brief TSR[31:16]: Timestamp counter value */
#define CANFD_TSR_TIMESTAMP_CNT_Pos         (16U)
#define CANFD_TSR_TIMESTAMP_CNT_Msk         (0xFFFFUL << CANFD_TSR_TIMESTAMP_CNT_Pos)
#define CANFD_TSR_TIMESTAMP_CNT             CANFD_TSR_TIMESTAMP_CNT_Msk

/*****************  Bit definition for CANFD_DP_BRPR register  ****************/

/** @brief DP_BRPR[7:0]: Data phase baudrate prescaler */
#define CANFD_DP_BRPR_PR_Pos                (0U)
#define CANFD_DP_BRPR_PR_Msk                (0xFFUL << CANFD_DP_BRPR_PR_Pos)
#define CANFD_DP_BRPR_PR                    CANFD_DP_BRPR_PR_Msk
/** @brief DP_BRPR[13:8]: Transmitter delay compensation offset */
#define CANFD_DP_BRPR_TDCOFF_Pos            (8U)
#define CANFD_DP_BRPR_TDCOFF_Msk            (0x3FUL << CANFD_DP_BRPR_TDCOFF_Pos)
#define CANFD_DP_BRPR_TDCOFF                CANFD_DP_BRPR_TDCOFF_Msk
/** @brief DP_BRPR[16]: Transmitter delay compensation enable */
#define CANFD_DP_BRPR_TDC_Pos               (16U)
#define CANFD_DP_BRPR_TDC_Msk               (0x1UL << CANFD_DP_BRPR_TDC_Pos)
#define CANFD_DP_BRPR_TDC                   CANFD_DP_BRPR_TDC_Msk

/*****************  Bit definition for CANFD_DP_BTR register  *****************/

/** @brief DP_BTR[4:0]: Data phase time segment 1 */
#define CANFD_DP_BTR_TS1_Pos                (0U)
#define CANFD_DP_BTR_TS1_Msk                (0x1FUL << CANFD_DP_BTR_TS1_Pos)
#define CANFD_DP_BTR_TS1                    CANFD_DP_BTR_TS1_Msk
/** @brief DP_BTR[11:8]: Data phase time segment 2 */
#define CANFD_DP_BTR_TS2_Pos                (8U)
#define CANFD_DP_BTR_TS2_Msk                (0xFUL << CANFD_DP_BTR_TS2_Pos)
#define CANFD_DP_BTR_TS2                    CANFD_DP_BTR_TS2_Msk
/** @brief DP_BTR[19:16]: Data phase synchronization jump width */
#define CANFD_DP_BTR_SJW_Pos                (16U)
#define CANFD_DP_BTR_SJW_Msk                (0xFUL << CANFD_DP_BTR_SJW_Pos)
#define CANFD_DP_BTR_SJW                    CANFD_DP_BTR_SJW_Msk

/******************  Bit definition for CANFD_TRR register  *******************/

/** @brief TRR[0]: Tx buffer 0 ready request */
#define CANFD_TRR_RR0_Pos                   (0U)
#define CANFD_TRR_RR0_Msk                   (0x1UL << CANFD_TRR_RR0_Pos)
#define CANFD_TRR_RR0                       CANFD_TRR_RR0_Msk
/** @brief TRR[1]: Tx buffer 1 ready request */
#define CANFD_TRR_RR1_Pos                   (1U)
#define CANFD_TRR_RR1_Msk                   (0x1UL << CANFD_TRR_RR1_Pos)
#define CANFD_TRR_RR1                       CANFD_TRR_RR1_Msk
/** @brief TRR[2]: Tx buffer 2 ready request */
#define CANFD_TRR_RR2_Pos                   (2U)
#define CANFD_TRR_RR2_Msk                   (0x1UL << CANFD_TRR_RR2_Pos)
#define CANFD_TRR_RR2                       CANFD_TRR_RR2_Msk
/** @brief TRR[3]: Tx buffer 3 ready request */
#define CANFD_TRR_RR3_Pos                   (3U)
#define CANFD_TRR_RR3_Msk                   (0x1UL << CANFD_TRR_RR3_Pos)
#define CANFD_TRR_RR3                       CANFD_TRR_RR3_Msk
/** @brief TRR[4]: Tx buffer 4 ready request */
#define CANFD_TRR_RR4_Pos                   (4U)
#define CANFD_TRR_RR4_Msk                   (0x1UL << CANFD_TRR_RR4_Pos)
#define CANFD_TRR_RR4                       CANFD_TRR_RR4_Msk
/** @brief TRR[5]: Tx buffer 5 ready request */
#define CANFD_TRR_RR5_Pos                   (5U)
#define CANFD_TRR_RR5_Msk                   (0x1UL << CANFD_TRR_RR5_Pos)
#define CANFD_TRR_RR5                       CANFD_TRR_RR5_Msk
/** @brief TRR[6]: Tx buffer 6 ready request */
#define CANFD_TRR_RR6_Pos                   (6U)
#define CANFD_TRR_RR6_Msk                   (0x1UL << CANFD_TRR_RR6_Pos)
#define CANFD_TRR_RR6                       CANFD_TRR_RR6_Msk
/** @brief TRR[7]: Tx buffer 7 ready request */
#define CANFD_TRR_RR7_Pos                   (7U)
#define CANFD_TRR_RR7_Msk                   (0x1UL << CANFD_TRR_RR7_Pos)
#define CANFD_TRR_RR7                       CANFD_TRR_RR7_Msk
/** @brief TRR[8]: Tx buffer 8 ready request */
#define CANFD_TRR_RR8_Pos                   (8U)
#define CANFD_TRR_RR8_Msk                   (0x1UL << CANFD_TRR_RR8_Pos)
#define CANFD_TRR_RR8                       CANFD_TRR_RR8_Msk
/** @brief TRR[9]: Tx buffer 9 ready request */
#define CANFD_TRR_RR9_Pos                   (9U)
#define CANFD_TRR_RR9_Msk                   (0x1UL << CANFD_TRR_RR9_Pos)
#define CANFD_TRR_RR9                       CANFD_TRR_RR9_Msk
/** @brief TRR[10]: Tx buffer 10 ready request */
#define CANFD_TRR_RR10_Pos                  (10U)
#define CANFD_TRR_RR10_Msk                  (0x1UL << CANFD_TRR_RR10_Pos)
#define CANFD_TRR_RR10                      CANFD_TRR_RR10_Msk
/** @brief TRR[11]: Tx buffer 11 ready request */
#define CANFD_TRR_RR11_Pos                  (11U)
#define CANFD_TRR_RR11_Msk                  (0x1UL << CANFD_TRR_RR11_Pos)
#define CANFD_TRR_RR11                      CANFD_TRR_RR11_Msk
/** @brief TRR[12]: Tx buffer 12 ready request */
#define CANFD_TRR_RR12_Pos                  (12U)
#define CANFD_TRR_RR12_Msk                  (0x1UL << CANFD_TRR_RR12_Pos)
#define CANFD_TRR_RR12                      CANFD_TRR_RR12_Msk
/** @brief TRR[13]: Tx buffer 13 ready request */
#define CANFD_TRR_RR13_Pos                  (13U)
#define CANFD_TRR_RR13_Msk                  (0x1UL << CANFD_TRR_RR13_Pos)
#define CANFD_TRR_RR13                      CANFD_TRR_RR13_Msk
/** @brief TRR[14]: Tx buffer 14 ready request */
#define CANFD_TRR_RR14_Pos                  (14U)
#define CANFD_TRR_RR14_Msk                  (0x1UL << CANFD_TRR_RR14_Pos)
#define CANFD_TRR_RR14                      CANFD_TRR_RR14_Msk
/** @brief TRR[15]: Tx buffer 15 ready request */
#define CANFD_TRR_RR15_Pos                  (15U)
#define CANFD_TRR_RR15_Msk                  (0x1UL << CANFD_TRR_RR15_Pos)
#define CANFD_TRR_RR15                      CANFD_TRR_RR15_Msk
/** @brief TRR[16]: Tx buffer 16 ready request */
#define CANFD_TRR_RR16_Pos                  (16U)
#define CANFD_TRR_RR16_Msk                  (0x1UL << CANFD_TRR_RR16_Pos)
#define CANFD_TRR_RR16                      CANFD_TRR_RR16_Msk
/** @brief TRR[17]: Tx buffer 17 ready request */
#define CANFD_TRR_RR17_Pos                  (17U)
#define CANFD_TRR_RR17_Msk                  (0x1UL << CANFD_TRR_RR17_Pos)
#define CANFD_TRR_RR17                      CANFD_TRR_RR17_Msk
/** @brief TRR[18]: Tx buffer 18 ready request */
#define CANFD_TRR_RR18_Pos                  (18U)
#define CANFD_TRR_RR18_Msk                  (0x1UL << CANFD_TRR_RR18_Pos)
#define CANFD_TRR_RR18                      CANFD_TRR_RR18_Msk
/** @brief TRR[19]: Tx buffer 19 ready request */
#define CANFD_TRR_RR19_Pos                  (19U)
#define CANFD_TRR_RR19_Msk                  (0x1UL << CANFD_TRR_RR19_Pos)
#define CANFD_TRR_RR19                      CANFD_TRR_RR19_Msk
/** @brief TRR[20]: Tx buffer 20 ready request */
#define CANFD_TRR_RR20_Pos                  (20U)
#define CANFD_TRR_RR20_Msk                  (0x1UL << CANFD_TRR_RR20_Pos)
#define CANFD_TRR_RR20                      CANFD_TRR_RR20_Msk
/** @brief TRR[21]: Tx buffer 21 ready request */
#define CANFD_TRR_RR21_Pos                  (21U)
#define CANFD_TRR_RR21_Msk                  (0x1UL << CANFD_TRR_RR21_Pos)
#define CANFD_TRR_RR21                      CANFD_TRR_RR21_Msk
/** @brief TRR[22]: Tx buffer 22 ready request */
#define CANFD_TRR_RR22_Pos                  (22U)
#define CANFD_TRR_RR22_Msk                  (0x1UL << CANFD_TRR_RR22_Pos)
#define CANFD_TRR_RR22                      CANFD_TRR_RR22_Msk
/** @brief TRR[23]: Tx buffer 23 ready request */
#define CANFD_TRR_RR23_Pos                  (23U)
#define CANFD_TRR_RR23_Msk                  (0x1UL << CANFD_TRR_RR23_Pos)
#define CANFD_TRR_RR23                      CANFD_TRR_RR23_Msk
/** @brief TRR[24]: Tx buffer 24 ready request */
#define CANFD_TRR_RR24_Pos                  (24U)
#define CANFD_TRR_RR24_Msk                  (0x1UL << CANFD_TRR_RR24_Pos)
#define CANFD_TRR_RR24                      CANFD_TRR_RR24_Msk
/** @brief TRR[25]: Tx buffer 25 ready request */
#define CANFD_TRR_RR25_Pos                  (25U)
#define CANFD_TRR_RR25_Msk                  (0x1UL << CANFD_TRR_RR25_Pos)
#define CANFD_TRR_RR25                      CANFD_TRR_RR25_Msk
/** @brief TRR[26]: Tx buffer 26 ready request */
#define CANFD_TRR_RR26_Pos                  (26U)
#define CANFD_TRR_RR26_Msk                  (0x1UL << CANFD_TRR_RR26_Pos)
#define CANFD_TRR_RR26                      CANFD_TRR_RR26_Msk
/** @brief TRR[27]: Tx buffer 27 ready request */
#define CANFD_TRR_RR27_Pos                  (27U)
#define CANFD_TRR_RR27_Msk                  (0x1UL << CANFD_TRR_RR27_Pos)
#define CANFD_TRR_RR27                      CANFD_TRR_RR27_Msk
/** @brief TRR[28]: Tx buffer 28 ready request */
#define CANFD_TRR_RR28_Pos                  (28U)
#define CANFD_TRR_RR28_Msk                  (0x1UL << CANFD_TRR_RR28_Pos)
#define CANFD_TRR_RR28                      CANFD_TRR_RR28_Msk
/** @brief TRR[29]: Tx buffer 29 ready request */
#define CANFD_TRR_RR29_Pos                  (29U)
#define CANFD_TRR_RR29_Msk                  (0x1UL << CANFD_TRR_RR29_Pos)
#define CANFD_TRR_RR29                      CANFD_TRR_RR29_Msk
/** @brief TRR[30]: Tx buffer 30 ready request */
#define CANFD_TRR_RR30_Pos                  (30U)
#define CANFD_TRR_RR30_Msk                  (0x1UL << CANFD_TRR_RR30_Pos)
#define CANFD_TRR_RR30                      CANFD_TRR_RR30_Msk
/** @brief TRR[31]: Tx buffer 31 ready request */
#define CANFD_TRR_RR31_Pos                  (31U)
#define CANFD_TRR_RR31_Msk                  (0x1UL << CANFD_TRR_RR31_Pos)
#define CANFD_TRR_RR31                      CANFD_TRR_RR31_Msk

/*****************  Bit definition for CANFD_IETRS register  ******************/

/** @brief IETRS[0]: Tx buffer 0 ready request served interrupt enable */
#define CANFD_IETRS_ERRS0_Pos               (0U)
#define CANFD_IETRS_ERRS0_Msk               (0x1UL << CANFD_IETRS_ERRS0_Pos)
#define CANFD_IETRS_ERRS0                   CANFD_IETRS_ERRS0_Msk
/** @brief IETRS[1]: Tx buffer 1 ready request served interrupt enable */
#define CANFD_IETRS_ERRS1_Pos               (1U)
#define CANFD_IETRS_ERRS1_Msk               (0x1UL << CANFD_IETRS_ERRS1_Pos)
#define CANFD_IETRS_ERRS1                   CANFD_IETRS_ERRS1_Msk
/** @brief IETRS[2]: Tx buffer 2 ready request served interrupt enable */
#define CANFD_IETRS_ERRS2_Pos               (2U)
#define CANFD_IETRS_ERRS2_Msk               (0x1UL << CANFD_IETRS_ERRS2_Pos)
#define CANFD_IETRS_ERRS2                   CANFD_IETRS_ERRS2_Msk
/** @brief IETRS[3]: Tx buffer 3 ready request served interrupt enable */
#define CANFD_IETRS_ERRS3_Pos               (3U)
#define CANFD_IETRS_ERRS3_Msk               (0x1UL << CANFD_IETRS_ERRS3_Pos)
#define CANFD_IETRS_ERRS3                   CANFD_IETRS_ERRS3_Msk
/** @brief IETRS[4]: Tx buffer 4 ready request served interrupt enable */
#define CANFD_IETRS_ERRS4_Pos               (4U)
#define CANFD_IETRS_ERRS4_Msk               (0x1UL << CANFD_IETRS_ERRS4_Pos)
#define CANFD_IETRS_ERRS4                   CANFD_IETRS_ERRS4_Msk
/** @brief IETRS[5]: Tx buffer 5 ready request served interrupt enable */
#define CANFD_IETRS_ERRS5_Pos               (5U)
#define CANFD_IETRS_ERRS5_Msk               (0x1UL << CANFD_IETRS_ERRS5_Pos)
#define CANFD_IETRS_ERRS5                   CANFD_IETRS_ERRS5_Msk
/** @brief IETRS[6]: Tx buffer 6 ready request served interrupt enable */
#define CANFD_IETRS_ERRS6_Pos               (6U)
#define CANFD_IETRS_ERRS6_Msk               (0x1UL << CANFD_IETRS_ERRS6_Pos)
#define CANFD_IETRS_ERRS6                   CANFD_IETRS_ERRS6_Msk
/** @brief IETRS[7]: Tx buffer 7 ready request served interrupt enable */
#define CANFD_IETRS_ERRS7_Pos               (7U)
#define CANFD_IETRS_ERRS7_Msk               (0x1UL << CANFD_IETRS_ERRS7_Pos)
#define CANFD_IETRS_ERRS7                   CANFD_IETRS_ERRS7_Msk
/** @brief IETRS[8]: Tx buffer 8 ready request served interrupt enable */
#define CANFD_IETRS_ERRS8_Pos               (8U)
#define CANFD_IETRS_ERRS8_Msk               (0x1UL << CANFD_IETRS_ERRS8_Pos)
#define CANFD_IETRS_ERRS8                   CANFD_IETRS_ERRS8_Msk
/** @brief IETRS[9]: Tx buffer 9 ready request served interrupt enable */
#define CANFD_IETRS_ERRS9_Pos               (9U)
#define CANFD_IETRS_ERRS9_Msk               (0x1UL << CANFD_IETRS_ERRS9_Pos)
#define CANFD_IETRS_ERRS9                   CANFD_IETRS_ERRS9_Msk
/** @brief IETRS[10]: Tx buffer 10 ready request served interrupt enable */
#define CANFD_IETRS_ERRS10_Pos              (10U)
#define CANFD_IETRS_ERRS10_Msk              (0x1UL << CANFD_IETRS_ERRS10_Pos)
#define CANFD_IETRS_ERRS10                  CANFD_IETRS_ERRS10_Msk
/** @brief IETRS[11]: Tx buffer 11 ready request served interrupt enable */
#define CANFD_IETRS_ERRS11_Pos              (11U)
#define CANFD_IETRS_ERRS11_Msk              (0x1UL << CANFD_IETRS_ERRS11_Pos)
#define CANFD_IETRS_ERRS11                  CANFD_IETRS_ERRS11_Msk
/** @brief IETRS[12]: Tx buffer 12 ready request served interrupt enable */
#define CANFD_IETRS_ERRS12_Pos              (12U)
#define CANFD_IETRS_ERRS12_Msk              (0x1UL << CANFD_IETRS_ERRS12_Pos)
#define CANFD_IETRS_ERRS12                  CANFD_IETRS_ERRS12_Msk
/** @brief IETRS[13]: Tx buffer 13 ready request served interrupt enable */
#define CANFD_IETRS_ERRS13_Pos              (13U)
#define CANFD_IETRS_ERRS13_Msk              (0x1UL << CANFD_IETRS_ERRS13_Pos)
#define CANFD_IETRS_ERRS13                  CANFD_IETRS_ERRS13_Msk
/** @brief IETRS[14]: Tx buffer 14 ready request served interrupt enable */
#define CANFD_IETRS_ERRS14_Pos              (14U)
#define CANFD_IETRS_ERRS14_Msk              (0x1UL << CANFD_IETRS_ERRS14_Pos)
#define CANFD_IETRS_ERRS14                  CANFD_IETRS_ERRS14_Msk
/** @brief IETRS[15]: Tx buffer 15 ready request served interrupt enable */
#define CANFD_IETRS_ERRS15_Pos              (15U)
#define CANFD_IETRS_ERRS15_Msk              (0x1UL << CANFD_IETRS_ERRS15_Pos)
#define CANFD_IETRS_ERRS15                  CANFD_IETRS_ERRS15_Msk
/** @brief IETRS[16]: Tx buffer 16 ready request served interrupt enable */
#define CANFD_IETRS_ERRS16_Pos              (16U)
#define CANFD_IETRS_ERRS16_Msk              (0x1UL << CANFD_IETRS_ERRS16_Pos)
#define CANFD_IETRS_ERRS16                  CANFD_IETRS_ERRS16_Msk
/** @brief IETRS[17]: Tx buffer 17 ready request served interrupt enable */
#define CANFD_IETRS_ERRS17_Pos              (17U)
#define CANFD_IETRS_ERRS17_Msk              (0x1UL << CANFD_IETRS_ERRS17_Pos)
#define CANFD_IETRS_ERRS17                  CANFD_IETRS_ERRS17_Msk
/** @brief IETRS[18]: Tx buffer 18 ready request served interrupt enable */
#define CANFD_IETRS_ERRS18_Pos              (18U)
#define CANFD_IETRS_ERRS18_Msk              (0x1UL << CANFD_IETRS_ERRS18_Pos)
#define CANFD_IETRS_ERRS18                  CANFD_IETRS_ERRS18_Msk
/** @brief IETRS[19]: Tx buffer 19 ready request served interrupt enable */
#define CANFD_IETRS_ERRS19_Pos              (19U)
#define CANFD_IETRS_ERRS19_Msk              (0x1UL << CANFD_IETRS_ERRS19_Pos)
#define CANFD_IETRS_ERRS19                  CANFD_IETRS_ERRS19_Msk
/** @brief IETRS[20]: Tx buffer 20 ready request served interrupt enable */
#define CANFD_IETRS_ERRS20_Pos              (20U)
#define CANFD_IETRS_ERRS20_Msk              (0x1UL << CANFD_IETRS_ERRS20_Pos)
#define CANFD_IETRS_ERRS20                  CANFD_IETRS_ERRS20_Msk
/** @brief IETRS[21]: Tx buffer 21 ready request served interrupt enable */
#define CANFD_IETRS_ERRS21_Pos              (21U)
#define CANFD_IETRS_ERRS21_Msk              (0x1UL << CANFD_IETRS_ERRS21_Pos)
#define CANFD_IETRS_ERRS21                  CANFD_IETRS_ERRS21_Msk
/** @brief IETRS[22]: Tx buffer 22 ready request served interrupt enable */
#define CANFD_IETRS_ERRS22_Pos              (22U)
#define CANFD_IETRS_ERRS22_Msk              (0x1UL << CANFD_IETRS_ERRS22_Pos)
#define CANFD_IETRS_ERRS22                  CANFD_IETRS_ERRS22_Msk
/** @brief IETRS[23]: Tx buffer 23 ready request served interrupt enable */
#define CANFD_IETRS_ERRS23_Pos              (23U)
#define CANFD_IETRS_ERRS23_Msk              (0x1UL << CANFD_IETRS_ERRS23_Pos)
#define CANFD_IETRS_ERRS23                  CANFD_IETRS_ERRS23_Msk
/** @brief IETRS[24]: Tx buffer 24 ready request served interrupt enable */
#define CANFD_IETRS_ERRS24_Pos              (24U)
#define CANFD_IETRS_ERRS24_Msk              (0x1UL << CANFD_IETRS_ERRS24_Pos)
#define CANFD_IETRS_ERRS24                  CANFD_IETRS_ERRS24_Msk
/** @brief IETRS[25]: Tx buffer 25 ready request served interrupt enable */
#define CANFD_IETRS_ERRS25_Pos              (25U)
#define CANFD_IETRS_ERRS25_Msk              (0x1UL << CANFD_IETRS_ERRS25_Pos)
#define CANFD_IETRS_ERRS25                  CANFD_IETRS_ERRS25_Msk
/** @brief IETRS[26]: Tx buffer 26 ready request served interrupt enable */
#define CANFD_IETRS_ERRS26_Pos              (26U)
#define CANFD_IETRS_ERRS26_Msk              (0x1UL << CANFD_IETRS_ERRS26_Pos)
#define CANFD_IETRS_ERRS26                  CANFD_IETRS_ERRS26_Msk
/** @brief IETRS[27]: Tx buffer 27 ready request served interrupt enable */
#define CANFD_IETRS_ERRS27_Pos              (27U)
#define CANFD_IETRS_ERRS27_Msk              (0x1UL << CANFD_IETRS_ERRS27_Pos)
#define CANFD_IETRS_ERRS27                  CANFD_IETRS_ERRS27_Msk
/** @brief IETRS[28]: Tx buffer 28 ready request served interrupt enable */
#define CANFD_IETRS_ERRS28_Pos              (28U)
#define CANFD_IETRS_ERRS28_Msk              (0x1UL << CANFD_IETRS_ERRS28_Pos)
#define CANFD_IETRS_ERRS28                  CANFD_IETRS_ERRS28_Msk
/** @brief IETRS[29]: Tx buffer 29 ready request served interrupt enable */
#define CANFD_IETRS_ERRS29_Pos              (29U)
#define CANFD_IETRS_ERRS29_Msk              (0x1UL << CANFD_IETRS_ERRS29_Pos)
#define CANFD_IETRS_ERRS29                  CANFD_IETRS_ERRS29_Msk
/** @brief IETRS[30]: Tx buffer 30 ready request served interrupt enable */
#define CANFD_IETRS_ERRS30_Pos              (30U)
#define CANFD_IETRS_ERRS30_Msk              (0x1UL << CANFD_IETRS_ERRS30_Pos)
#define CANFD_IETRS_ERRS30                  CANFD_IETRS_ERRS30_Msk
/** @brief IETRS[31]: Tx buffer 31 ready request served interrupt enable */
#define CANFD_IETRS_ERRS31_Pos              (31U)
#define CANFD_IETRS_ERRS31_Msk              (0x1UL << CANFD_IETRS_ERRS31_Pos)
#define CANFD_IETRS_ERRS31                  CANFD_IETRS_ERRS31_Msk

/******************  Bit definition for CANFD_TCR register  *******************/

/** @brief TCR[0]: Tx buffer 0 cancel request */
#define CANFD_TCR_CR0_Pos                   (0U)
#define CANFD_TCR_CR0_Msk                   (0x1UL << CANFD_TCR_CR0_Pos)
#define CANFD_TCR_CR0                       CANFD_TCR_CR0_Msk
/** @brief TCR[1]: Tx buffer 1 cancel request */
#define CANFD_TCR_CR1_Pos                   (1U)
#define CANFD_TCR_CR1_Msk                   (0x1UL << CANFD_TCR_CR1_Pos)
#define CANFD_TCR_CR1                       CANFD_TCR_CR1_Msk
/** @brief TCR[2]: Tx buffer 2 cancel request */
#define CANFD_TCR_CR2_Pos                   (2U)
#define CANFD_TCR_CR2_Msk                   (0x1UL << CANFD_TCR_CR2_Pos)
#define CANFD_TCR_CR2                       CANFD_TCR_CR2_Msk
/** @brief TCR[3]: Tx buffer 3 cancel request */
#define CANFD_TCR_CR3_Pos                   (3U)
#define CANFD_TCR_CR3_Msk                   (0x1UL << CANFD_TCR_CR3_Pos)
#define CANFD_TCR_CR3                       CANFD_TCR_CR3_Msk
/** @brief TCR[4]: Tx buffer 4 cancel request */
#define CANFD_TCR_CR4_Pos                   (4U)
#define CANFD_TCR_CR4_Msk                   (0x1UL << CANFD_TCR_CR4_Pos)
#define CANFD_TCR_CR4                       CANFD_TCR_CR4_Msk
/** @brief TCR[5]: Tx buffer 5 cancel request */
#define CANFD_TCR_CR5_Pos                   (5U)
#define CANFD_TCR_CR5_Msk                   (0x1UL << CANFD_TCR_CR5_Pos)
#define CANFD_TCR_CR5                       CANFD_TCR_CR5_Msk
/** @brief TCR[6]: Tx buffer 6 cancel request */
#define CANFD_TCR_CR6_Pos                   (6U)
#define CANFD_TCR_CR6_Msk                   (0x1UL << CANFD_TCR_CR6_Pos)
#define CANFD_TCR_CR6                       CANFD_TCR_CR6_Msk
/** @brief TCR[7]: Tx buffer 7 cancel request */
#define CANFD_TCR_CR7_Pos                   (7U)
#define CANFD_TCR_CR7_Msk                   (0x1UL << CANFD_TCR_CR7_Pos)
#define CANFD_TCR_CR7                       CANFD_TCR_CR7_Msk
/** @brief TCR[8]: Tx buffer 8 cancel request */
#define CANFD_TCR_CR8_Pos                   (8U)
#define CANFD_TCR_CR8_Msk                   (0x1UL << CANFD_TCR_CR8_Pos)
#define CANFD_TCR_CR8                       CANFD_TCR_CR8_Msk
/** @brief TCR[9]: Tx buffer 9 cancel request */
#define CANFD_TCR_CR9_Pos                   (9U)
#define CANFD_TCR_CR9_Msk                   (0x1UL << CANFD_TCR_CR9_Pos)
#define CANFD_TCR_CR9                       CANFD_TCR_CR9_Msk
/** @brief TCR[10]: Tx buffer 10 cancel request */
#define CANFD_TCR_CR10_Pos                  (10U)
#define CANFD_TCR_CR10_Msk                  (0x1UL << CANFD_TCR_CR10_Pos)
#define CANFD_TCR_CR10                      CANFD_TCR_CR10_Msk
/** @brief TCR[11]: Tx buffer 11 cancel request */
#define CANFD_TCR_CR11_Pos                  (11U)
#define CANFD_TCR_CR11_Msk                  (0x1UL << CANFD_TCR_CR11_Pos)
#define CANFD_TCR_CR11                      CANFD_TCR_CR11_Msk
/** @brief TCR[12]: Tx buffer 12 cancel request */
#define CANFD_TCR_CR12_Pos                  (12U)
#define CANFD_TCR_CR12_Msk                  (0x1UL << CANFD_TCR_CR12_Pos)
#define CANFD_TCR_CR12                      CANFD_TCR_CR12_Msk
/** @brief TCR[13]: Tx buffer 13 cancel request */
#define CANFD_TCR_CR13_Pos                  (13U)
#define CANFD_TCR_CR13_Msk                  (0x1UL << CANFD_TCR_CR13_Pos)
#define CANFD_TCR_CR13                      CANFD_TCR_CR13_Msk
/** @brief TCR[14]: Tx buffer 14 cancel request */
#define CANFD_TCR_CR14_Pos                  (14U)
#define CANFD_TCR_CR14_Msk                  (0x1UL << CANFD_TCR_CR14_Pos)
#define CANFD_TCR_CR14                      CANFD_TCR_CR14_Msk
/** @brief TCR[15]: Tx buffer 15 cancel request */
#define CANFD_TCR_CR15_Pos                  (15U)
#define CANFD_TCR_CR15_Msk                  (0x1UL << CANFD_TCR_CR15_Pos)
#define CANFD_TCR_CR15                      CANFD_TCR_CR15_Msk
/** @brief TCR[16]: Tx buffer 16 cancel request */
#define CANFD_TCR_CR16_Pos                  (16U)
#define CANFD_TCR_CR16_Msk                  (0x1UL << CANFD_TCR_CR16_Pos)
#define CANFD_TCR_CR16                      CANFD_TCR_CR16_Msk
/** @brief TCR[17]: Tx buffer 17 cancel request */
#define CANFD_TCR_CR17_Pos                  (17U)
#define CANFD_TCR_CR17_Msk                  (0x1UL << CANFD_TCR_CR17_Pos)
#define CANFD_TCR_CR17                      CANFD_TCR_CR17_Msk
/** @brief TCR[18]: Tx buffer 18 cancel request */
#define CANFD_TCR_CR18_Pos                  (18U)
#define CANFD_TCR_CR18_Msk                  (0x1UL << CANFD_TCR_CR18_Pos)
#define CANFD_TCR_CR18                      CANFD_TCR_CR18_Msk
/** @brief TCR[19]: Tx buffer 19 cancel request */
#define CANFD_TCR_CR19_Pos                  (19U)
#define CANFD_TCR_CR19_Msk                  (0x1UL << CANFD_TCR_CR19_Pos)
#define CANFD_TCR_CR19                      CANFD_TCR_CR19_Msk
/** @brief TCR[20]: Tx buffer 20 cancel request */
#define CANFD_TCR_CR20_Pos                  (20U)
#define CANFD_TCR_CR20_Msk                  (0x1UL << CANFD_TCR_CR20_Pos)
#define CANFD_TCR_CR20                      CANFD_TCR_CR20_Msk
/** @brief TCR[21]: Tx buffer 21 cancel request */
#define CANFD_TCR_CR21_Pos                  (21U)
#define CANFD_TCR_CR21_Msk                  (0x1UL << CANFD_TCR_CR21_Pos)
#define CANFD_TCR_CR21                      CANFD_TCR_CR21_Msk
/** @brief TCR[22]: Tx buffer 22 cancel request */
#define CANFD_TCR_CR22_Pos                  (22U)
#define CANFD_TCR_CR22_Msk                  (0x1UL << CANFD_TCR_CR22_Pos)
#define CANFD_TCR_CR22                      CANFD_TCR_CR22_Msk
/** @brief TCR[23]: Tx buffer 23 cancel request */
#define CANFD_TCR_CR23_Pos                  (23U)
#define CANFD_TCR_CR23_Msk                  (0x1UL << CANFD_TCR_CR23_Pos)
#define CANFD_TCR_CR23                      CANFD_TCR_CR23_Msk
/** @brief TCR[24]: Tx buffer 24 cancel request */
#define CANFD_TCR_CR24_Pos                  (24U)
#define CANFD_TCR_CR24_Msk                  (0x1UL << CANFD_TCR_CR24_Pos)
#define CANFD_TCR_CR24                      CANFD_TCR_CR24_Msk
/** @brief TCR[25]: Tx buffer 25 cancel request */
#define CANFD_TCR_CR25_Pos                  (25U)
#define CANFD_TCR_CR25_Msk                  (0x1UL << CANFD_TCR_CR25_Pos)
#define CANFD_TCR_CR25                      CANFD_TCR_CR25_Msk
/** @brief TCR[26]: Tx buffer 26 cancel request */
#define CANFD_TCR_CR26_Pos                  (26U)
#define CANFD_TCR_CR26_Msk                  (0x1UL << CANFD_TCR_CR26_Pos)
#define CANFD_TCR_CR26                      CANFD_TCR_CR26_Msk
/** @brief TCR[27]: Tx buffer 27 cancel request */
#define CANFD_TCR_CR27_Pos                  (27U)
#define CANFD_TCR_CR27_Msk                  (0x1UL << CANFD_TCR_CR27_Pos)
#define CANFD_TCR_CR27                      CANFD_TCR_CR27_Msk
/** @brief TCR[28]: Tx buffer 28 cancel request */
#define CANFD_TCR_CR28_Pos                  (28U)
#define CANFD_TCR_CR28_Msk                  (0x1UL << CANFD_TCR_CR28_Pos)
#define CANFD_TCR_CR28                      CANFD_TCR_CR28_Msk
/** @brief TCR[29]: Tx buffer 29 cancel request */
#define CANFD_TCR_CR29_Pos                  (29U)
#define CANFD_TCR_CR29_Msk                  (0x1UL << CANFD_TCR_CR29_Pos)
#define CANFD_TCR_CR29                      CANFD_TCR_CR29_Msk
/** @brief TCR[30]: Tx buffer 30 cancel request */
#define CANFD_TCR_CR30_Pos                  (30U)
#define CANFD_TCR_CR30_Msk                  (0x1UL << CANFD_TCR_CR30_Pos)
#define CANFD_TCR_CR30                      CANFD_TCR_CR30_Msk
/** @brief TCR[31]: Tx buffer 31 cancel request */
#define CANFD_TCR_CR31_Pos                  (31U)
#define CANFD_TCR_CR31_Msk                  (0x1UL << CANFD_TCR_CR31_Pos)
#define CANFD_TCR_CR31                      CANFD_TCR_CR31_Msk

/*****************  Bit definition for CANFD_IETCS register  ******************/

/** @brief IETCS[0]: Tx buffer 0 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS0_Pos               (0U)
#define CANFD_IETCS_ECRS0_Msk               (0x1UL << CANFD_IETCS_ECRS0_Pos)
#define CANFD_IETCS_ECRS0                   CANFD_IETCS_ECRS0_Msk
/** @brief IETCS[1]: Tx buffer 1 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS1_Pos               (1U)
#define CANFD_IETCS_ECRS1_Msk               (0x1UL << CANFD_IETCS_ECRS1_Pos)
#define CANFD_IETCS_ECRS1                   CANFD_IETCS_ECRS1_Msk
/** @brief IETCS[2]: Tx buffer 2 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS2_Pos               (2U)
#define CANFD_IETCS_ECRS2_Msk               (0x1UL << CANFD_IETCS_ECRS2_Pos)
#define CANFD_IETCS_ECRS2                   CANFD_IETCS_ECRS2_Msk
/** @brief IETCS[3]: Tx buffer 3 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS3_Pos               (3U)
#define CANFD_IETCS_ECRS3_Msk               (0x1UL << CANFD_IETCS_ECRS3_Pos)
#define CANFD_IETCS_ECRS3                   CANFD_IETCS_ECRS3_Msk
/** @brief IETCS[4]: Tx buffer 4 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS4_Pos               (4U)
#define CANFD_IETCS_ECRS4_Msk               (0x1UL << CANFD_IETCS_ECRS4_Pos)
#define CANFD_IETCS_ECRS4                   CANFD_IETCS_ECRS4_Msk
/** @brief IETCS[5]: Tx buffer 5 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS5_Pos               (5U)
#define CANFD_IETCS_ECRS5_Msk               (0x1UL << CANFD_IETCS_ECRS5_Pos)
#define CANFD_IETCS_ECRS5                   CANFD_IETCS_ECRS5_Msk
/** @brief IETCS[6]: Tx buffer 6 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS6_Pos               (6U)
#define CANFD_IETCS_ECRS6_Msk               (0x1UL << CANFD_IETCS_ECRS6_Pos)
#define CANFD_IETCS_ECRS6                   CANFD_IETCS_ECRS6_Msk
/** @brief IETCS[7]: Tx buffer 7 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS7_Pos               (7U)
#define CANFD_IETCS_ECRS7_Msk               (0x1UL << CANFD_IETCS_ECRS7_Pos)
#define CANFD_IETCS_ECRS7                   CANFD_IETCS_ECRS7_Msk
/** @brief IETCS[8]: Tx buffer 8 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS8_Pos               (8U)
#define CANFD_IETCS_ECRS8_Msk               (0x1UL << CANFD_IETCS_ECRS8_Pos)
#define CANFD_IETCS_ECRS8                   CANFD_IETCS_ECRS8_Msk
/** @brief IETCS[9]: Tx buffer 9 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS9_Pos               (9U)
#define CANFD_IETCS_ECRS9_Msk               (0x1UL << CANFD_IETCS_ECRS9_Pos)
#define CANFD_IETCS_ECRS9                   CANFD_IETCS_ECRS9_Msk
/** @brief IETCS[10]: Tx buffer 10 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS10_Pos              (10U)
#define CANFD_IETCS_ECRS10_Msk              (0x1UL << CANFD_IETCS_ECRS10_Pos)
#define CANFD_IETCS_ECRS10                  CANFD_IETCS_ECRS10_Msk
/** @brief IETCS[11]: Tx buffer 11 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS11_Pos              (11U)
#define CANFD_IETCS_ECRS11_Msk              (0x1UL << CANFD_IETCS_ECRS11_Pos)
#define CANFD_IETCS_ECRS11                  CANFD_IETCS_ECRS11_Msk
/** @brief IETCS[12]: Tx buffer 12 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS12_Pos              (12U)
#define CANFD_IETCS_ECRS12_Msk              (0x1UL << CANFD_IETCS_ECRS12_Pos)
#define CANFD_IETCS_ECRS12                  CANFD_IETCS_ECRS12_Msk
/** @brief IETCS[13]: Tx buffer 13 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS13_Pos              (13U)
#define CANFD_IETCS_ECRS13_Msk              (0x1UL << CANFD_IETCS_ECRS13_Pos)
#define CANFD_IETCS_ECRS13                  CANFD_IETCS_ECRS13_Msk
/** @brief IETCS[14]: Tx buffer 14 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS14_Pos              (14U)
#define CANFD_IETCS_ECRS14_Msk              (0x1UL << CANFD_IETCS_ECRS14_Pos)
#define CANFD_IETCS_ECRS14                  CANFD_IETCS_ECRS14_Msk
/** @brief IETCS[15]: Tx buffer 15 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS15_Pos              (15U)
#define CANFD_IETCS_ECRS15_Msk              (0x1UL << CANFD_IETCS_ECRS15_Pos)
#define CANFD_IETCS_ECRS15                  CANFD_IETCS_ECRS15_Msk
/** @brief IETCS[16]: Tx buffer 16 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS16_Pos              (16U)
#define CANFD_IETCS_ECRS16_Msk              (0x1UL << CANFD_IETCS_ECRS16_Pos)
#define CANFD_IETCS_ECRS16                  CANFD_IETCS_ECRS16_Msk
/** @brief IETCS[17]: Tx buffer 17 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS17_Pos              (17U)
#define CANFD_IETCS_ECRS17_Msk              (0x1UL << CANFD_IETCS_ECRS17_Pos)
#define CANFD_IETCS_ECRS17                  CANFD_IETCS_ECRS17_Msk
/** @brief IETCS[18]: Tx buffer 18 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS18_Pos              (18U)
#define CANFD_IETCS_ECRS18_Msk              (0x1UL << CANFD_IETCS_ECRS18_Pos)
#define CANFD_IETCS_ECRS18                  CANFD_IETCS_ECRS18_Msk
/** @brief IETCS[19]: Tx buffer 19 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS19_Pos              (19U)
#define CANFD_IETCS_ECRS19_Msk              (0x1UL << CANFD_IETCS_ECRS19_Pos)
#define CANFD_IETCS_ECRS19                  CANFD_IETCS_ECRS19_Msk
/** @brief IETCS[20]: Tx buffer 20 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS20_Pos              (20U)
#define CANFD_IETCS_ECRS20_Msk              (0x1UL << CANFD_IETCS_ECRS20_Pos)
#define CANFD_IETCS_ECRS20                  CANFD_IETCS_ECRS20_Msk
/** @brief IETCS[21]: Tx buffer 21 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS21_Pos              (21U)
#define CANFD_IETCS_ECRS21_Msk              (0x1UL << CANFD_IETCS_ECRS21_Pos)
#define CANFD_IETCS_ECRS21                  CANFD_IETCS_ECRS21_Msk
/** @brief IETCS[22]: Tx buffer 22 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS22_Pos              (22U)
#define CANFD_IETCS_ECRS22_Msk              (0x1UL << CANFD_IETCS_ECRS22_Pos)
#define CANFD_IETCS_ECRS22                  CANFD_IETCS_ECRS22_Msk
/** @brief IETCS[23]: Tx buffer 23 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS23_Pos              (23U)
#define CANFD_IETCS_ECRS23_Msk              (0x1UL << CANFD_IETCS_ECRS23_Pos)
#define CANFD_IETCS_ECRS23                  CANFD_IETCS_ECRS23_Msk
/** @brief IETCS[24]: Tx buffer 24 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS24_Pos              (24U)
#define CANFD_IETCS_ECRS24_Msk              (0x1UL << CANFD_IETCS_ECRS24_Pos)
#define CANFD_IETCS_ECRS24                  CANFD_IETCS_ECRS24_Msk
/** @brief IETCS[25]: Tx buffer 25 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS25_Pos              (25U)
#define CANFD_IETCS_ECRS25_Msk              (0x1UL << CANFD_IETCS_ECRS25_Pos)
#define CANFD_IETCS_ECRS25                  CANFD_IETCS_ECRS25_Msk
/** @brief IETCS[26]: Tx buffer 26 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS26_Pos              (26U)
#define CANFD_IETCS_ECRS26_Msk              (0x1UL << CANFD_IETCS_ECRS26_Pos)
#define CANFD_IETCS_ECRS26                  CANFD_IETCS_ECRS26_Msk
/** @brief IETCS[27]: Tx buffer 27 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS27_Pos              (27U)
#define CANFD_IETCS_ECRS27_Msk              (0x1UL << CANFD_IETCS_ECRS27_Pos)
#define CANFD_IETCS_ECRS27                  CANFD_IETCS_ECRS27_Msk
/** @brief IETCS[28]: Tx buffer 28 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS28_Pos              (28U)
#define CANFD_IETCS_ECRS28_Msk              (0x1UL << CANFD_IETCS_ECRS28_Pos)
#define CANFD_IETCS_ECRS28                  CANFD_IETCS_ECRS28_Msk
/** @brief IETCS[29]: Tx buffer 29 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS29_Pos              (29U)
#define CANFD_IETCS_ECRS29_Msk              (0x1UL << CANFD_IETCS_ECRS29_Pos)
#define CANFD_IETCS_ECRS29                  CANFD_IETCS_ECRS29_Msk
/** @brief IETCS[30]: Tx buffer 30 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS30_Pos              (30U)
#define CANFD_IETCS_ECRS30_Msk              (0x1UL << CANFD_IETCS_ECRS30_Pos)
#define CANFD_IETCS_ECRS30                  CANFD_IETCS_ECRS30_Msk
/** @brief IETCS[31]: Tx buffer 31 cancellation request served interrupt enable */
#define CANFD_IETCS_ECRS31_Pos              (31U)
#define CANFD_IETCS_ECRS31_Msk              (0x1UL << CANFD_IETCS_ECRS31_Pos)
#define CANFD_IETCS_ECRS31                  CANFD_IETCS_ECRS31_Msk

/****************  Bit definition for CANFD_TXE_FSR register  *****************/

/** @brief TXE_FSR[4:0]: Read index */
#define CANFD_TXE_FSR_RI_Pos                (0U)
#define CANFD_TXE_FSR_RI_Msk                (0x1FUL << CANFD_TXE_FSR_RI_Pos)
#define CANFD_TXE_FSR_RI                    CANFD_TXE_FSR_RI_Msk
/** @brief TXE_FSR[7]: Increment read index by 1 */
#define CANFD_TXE_FSR_IRI_Pos               (7U)
#define CANFD_TXE_FSR_IRI_Msk               (0x1UL << CANFD_TXE_FSR_IRI_Pos)
#define CANFD_TXE_FSR_IRI                   CANFD_TXE_FSR_IRI_Msk
/** @brief TXE_FSR[13:8]: Fill level */
#define CANFD_TXE_FSR_FL_Pos                (8U)
#define CANFD_TXE_FSR_FL_Msk                (0x3FUL << CANFD_TXE_FSR_FL_Pos)
#define CANFD_TXE_FSR_FL                    CANFD_TXE_FSR_FL_Msk

/****************  Bit definition for CANFD_TXE_WMR register  *****************/

/** @brief TXE_WMR[4:0]: Tx event FIFO full watermark */
#define CANFD_TXE_WMR_FWM_Pos               (0U)
#define CANFD_TXE_WMR_FWM_Msk               (0x1FUL << CANFD_TXE_WMR_FWM_Pos)
#define CANFD_TXE_WMR_FWM                   CANFD_TXE_WMR_FWM_Msk

/******************  Bit definition for CANFD_AFR register  *******************/

/** @brief AFR[0]: Acceptance filter mask pair 0 enable */
#define CANFD_AFR_UAF0_Pos                  (0U)
#define CANFD_AFR_UAF0_Msk                  (0x1UL << CANFD_AFR_UAF0_Pos)
#define CANFD_AFR_UAF0                      CANFD_AFR_UAF0_Msk
/** @brief AFR[1]: Acceptance filter mask pair 1 enable */
#define CANFD_AFR_UAF1_Pos                  (1U)
#define CANFD_AFR_UAF1_Msk                  (0x1UL << CANFD_AFR_UAF1_Pos)
#define CANFD_AFR_UAF1                      CANFD_AFR_UAF1_Msk
/** @brief AFR[2]: Acceptance filter mask pair 2 enable */
#define CANFD_AFR_UAF2_Pos                  (2U)
#define CANFD_AFR_UAF2_Msk                  (0x1UL << CANFD_AFR_UAF2_Pos)
#define CANFD_AFR_UAF2                      CANFD_AFR_UAF2_Msk
/** @brief AFR[3]: Acceptance filter mask pair 3 enable */
#define CANFD_AFR_UAF3_Pos                  (3U)
#define CANFD_AFR_UAF3_Msk                  (0x1UL << CANFD_AFR_UAF3_Pos)
#define CANFD_AFR_UAF3                      CANFD_AFR_UAF3_Msk
/** @brief AFR[4]: Acceptance filter mask pair 4 enable */
#define CANFD_AFR_UAF4_Pos                  (4U)
#define CANFD_AFR_UAF4_Msk                  (0x1UL << CANFD_AFR_UAF4_Pos)
#define CANFD_AFR_UAF4                      CANFD_AFR_UAF4_Msk
/** @brief AFR[5]: Acceptance filter mask pair 5 enable */
#define CANFD_AFR_UAF5_Pos                  (5U)
#define CANFD_AFR_UAF5_Msk                  (0x1UL << CANFD_AFR_UAF5_Pos)
#define CANFD_AFR_UAF5                      CANFD_AFR_UAF5_Msk
/** @brief AFR[6]: Acceptance filter mask pair 6 enable */
#define CANFD_AFR_UAF6_Pos                  (6U)
#define CANFD_AFR_UAF6_Msk                  (0x1UL << CANFD_AFR_UAF6_Pos)
#define CANFD_AFR_UAF6                      CANFD_AFR_UAF6_Msk
/** @brief AFR[7]: Acceptance filter mask pair 7 enable */
#define CANFD_AFR_UAF7_Pos                  (7U)
#define CANFD_AFR_UAF7_Msk                  (0x1UL << CANFD_AFR_UAF7_Pos)
#define CANFD_AFR_UAF7                      CANFD_AFR_UAF7_Msk
/** @brief AFR[8]: Acceptance filter mask pair 8 enable */
#define CANFD_AFR_UAF8_Pos                  (8U)
#define CANFD_AFR_UAF8_Msk                  (0x1UL << CANFD_AFR_UAF8_Pos)
#define CANFD_AFR_UAF8                      CANFD_AFR_UAF8_Msk
/** @brief AFR[9]: Acceptance filter mask pair 9 enable */
#define CANFD_AFR_UAF9_Pos                  (9U)
#define CANFD_AFR_UAF9_Msk                  (0x1UL << CANFD_AFR_UAF9_Pos)
#define CANFD_AFR_UAF9                      CANFD_AFR_UAF9_Msk
/** @brief AFR[10]: Acceptance filter mask pair 10 enable */
#define CANFD_AFR_UAF10_Pos                 (10U)
#define CANFD_AFR_UAF10_Msk                 (0x1UL << CANFD_AFR_UAF10_Pos)
#define CANFD_AFR_UAF10                     CANFD_AFR_UAF10_Msk
/** @brief AFR[11]: Acceptance filter mask pair 11 enable */
#define CANFD_AFR_UAF11_Pos                 (11U)
#define CANFD_AFR_UAF11_Msk                 (0x1UL << CANFD_AFR_UAF11_Pos)
#define CANFD_AFR_UAF11                     CANFD_AFR_UAF11_Msk
/** @brief AFR[12]: Acceptance filter mask pair 12 enable */
#define CANFD_AFR_UAF12_Pos                 (12U)
#define CANFD_AFR_UAF12_Msk                 (0x1UL << CANFD_AFR_UAF12_Pos)
#define CANFD_AFR_UAF12                     CANFD_AFR_UAF12_Msk
/** @brief AFR[13]: Acceptance filter mask pair 13 enable */
#define CANFD_AFR_UAF13_Pos                 (13U)
#define CANFD_AFR_UAF13_Msk                 (0x1UL << CANFD_AFR_UAF13_Pos)
#define CANFD_AFR_UAF13                     CANFD_AFR_UAF13_Msk
/** @brief AFR[14]: Acceptance filter mask pair 14 enable */
#define CANFD_AFR_UAF14_Pos                 (14U)
#define CANFD_AFR_UAF14_Msk                 (0x1UL << CANFD_AFR_UAF14_Pos)
#define CANFD_AFR_UAF14                     CANFD_AFR_UAF14_Msk
/** @brief AFR[15]: Acceptance filter mask pair 15 enable */
#define CANFD_AFR_UAF15_Pos                 (15U)
#define CANFD_AFR_UAF15_Msk                 (0x1UL << CANFD_AFR_UAF15_Pos)
#define CANFD_AFR_UAF15                     CANFD_AFR_UAF15_Msk
/** @brief AFR[16]: Acceptance filter mask pair 16 enable */
#define CANFD_AFR_UAF16_Pos                 (16U)
#define CANFD_AFR_UAF16_Msk                 (0x1UL << CANFD_AFR_UAF16_Pos)
#define CANFD_AFR_UAF16                     CANFD_AFR_UAF16_Msk
/** @brief AFR[17]: Acceptance filter mask pair 17 enable */
#define CANFD_AFR_UAF17_Pos                 (17U)
#define CANFD_AFR_UAF17_Msk                 (0x1UL << CANFD_AFR_UAF17_Pos)
#define CANFD_AFR_UAF17                     CANFD_AFR_UAF17_Msk
/** @brief AFR[18]: Acceptance filter mask pair 18 enable */
#define CANFD_AFR_UAF18_Pos                 (18U)
#define CANFD_AFR_UAF18_Msk                 (0x1UL << CANFD_AFR_UAF18_Pos)
#define CANFD_AFR_UAF18                     CANFD_AFR_UAF18_Msk
/** @brief AFR[19]: Acceptance filter mask pair 19 enable */
#define CANFD_AFR_UAF19_Pos                 (19U)
#define CANFD_AFR_UAF19_Msk                 (0x1UL << CANFD_AFR_UAF19_Pos)
#define CANFD_AFR_UAF19                     CANFD_AFR_UAF19_Msk
/** @brief AFR[20]: Acceptance filter mask pair 20 enable */
#define CANFD_AFR_UAF20_Pos                 (20U)
#define CANFD_AFR_UAF20_Msk                 (0x1UL << CANFD_AFR_UAF20_Pos)
#define CANFD_AFR_UAF20                     CANFD_AFR_UAF20_Msk
/** @brief AFR[21]: Acceptance filter mask pair 21 enable */
#define CANFD_AFR_UAF21_Pos                 (21U)
#define CANFD_AFR_UAF21_Msk                 (0x1UL << CANFD_AFR_UAF21_Pos)
#define CANFD_AFR_UAF21                     CANFD_AFR_UAF21_Msk
/** @brief AFR[22]: Acceptance filter mask pair 22 enable */
#define CANFD_AFR_UAF22_Pos                 (22U)
#define CANFD_AFR_UAF22_Msk                 (0x1UL << CANFD_AFR_UAF22_Pos)
#define CANFD_AFR_UAF22                     CANFD_AFR_UAF22_Msk
/** @brief AFR[23]: Acceptance filter mask pair 23 enable */
#define CANFD_AFR_UAF23_Pos                 (23U)
#define CANFD_AFR_UAF23_Msk                 (0x1UL << CANFD_AFR_UAF23_Pos)
#define CANFD_AFR_UAF23                     CANFD_AFR_UAF23_Msk
/** @brief AFR[24]: Acceptance filter mask pair 24 enable */
#define CANFD_AFR_UAF24_Pos                 (24U)
#define CANFD_AFR_UAF24_Msk                 (0x1UL << CANFD_AFR_UAF24_Pos)
#define CANFD_AFR_UAF24                     CANFD_AFR_UAF24_Msk
/** @brief AFR[25]: Acceptance filter mask pair 25 enable */
#define CANFD_AFR_UAF25_Pos                 (25U)
#define CANFD_AFR_UAF25_Msk                 (0x1UL << CANFD_AFR_UAF25_Pos)
#define CANFD_AFR_UAF25                     CANFD_AFR_UAF25_Msk
/** @brief AFR[26]: Acceptance filter mask pair 26 enable */
#define CANFD_AFR_UAF26_Pos                 (26U)
#define CANFD_AFR_UAF26_Msk                 (0x1UL << CANFD_AFR_UAF26_Pos)
#define CANFD_AFR_UAF26                     CANFD_AFR_UAF26_Msk
/** @brief AFR[27]: Acceptance filter mask pair 27 enable */
#define CANFD_AFR_UAF27_Pos                 (27U)
#define CANFD_AFR_UAF27_Msk                 (0x1UL << CANFD_AFR_UAF27_Pos)
#define CANFD_AFR_UAF27                     CANFD_AFR_UAF27_Msk
/** @brief AFR[28]: Acceptance filter mask pair 28 enable */
#define CANFD_AFR_UAF28_Pos                 (28U)
#define CANFD_AFR_UAF28_Msk                 (0x1UL << CANFD_AFR_UAF28_Pos)
#define CANFD_AFR_UAF28                     CANFD_AFR_UAF28_Msk
/** @brief AFR[29]: Acceptance filter mask pair 29 enable */
#define CANFD_AFR_UAF29_Pos                 (29U)
#define CANFD_AFR_UAF29_Msk                 (0x1UL << CANFD_AFR_UAF29_Pos)
#define CANFD_AFR_UAF29                     CANFD_AFR_UAF29_Msk
/** @brief AFR[30]: Acceptance filter mask pair 30 enable */
#define CANFD_AFR_UAF30_Pos                 (30U)
#define CANFD_AFR_UAF30_Msk                 (0x1UL << CANFD_AFR_UAF30_Pos)
#define CANFD_AFR_UAF30                     CANFD_AFR_UAF30_Msk
/** @brief AFR[31]: Acceptance filter mask pair 31 enable */
#define CANFD_AFR_UAF31_Pos                 (31U)
#define CANFD_AFR_UAF31_Msk                 (0x1UL << CANFD_AFR_UAF31_Pos)
#define CANFD_AFR_UAF31                     CANFD_AFR_UAF31_Msk

/******************  Bit definition for CANFD_FSR register  *******************/

/** @brief FSR[5:0]: Rx FIFO-0 read index */
#define CANFD_FSR_RI_Pos                    (0U)
#define CANFD_FSR_RI_Msk                    (0x3FUL << CANFD_FSR_RI_Pos)
#define CANFD_FSR_RI                        CANFD_FSR_RI_Msk
/** @brief FSR[7]: Rx FIFO-0 increment read index by 1 */
#define CANFD_FSR_IRI_Pos                   (7U)
#define CANFD_FSR_IRI_Msk                   (0x1UL << CANFD_FSR_IRI_Pos)
#define CANFD_FSR_IRI                       CANFD_FSR_IRI_Msk
/** @brief FSR[14:8]: Rx FIFO-0 fill level */
#define CANFD_FSR_FL_Pos                    (8U)
#define CANFD_FSR_FL_Msk                    (0x7FUL << CANFD_FSR_FL_Pos)
#define CANFD_FSR_FL                        CANFD_FSR_FL_Msk
/** @brief FSR[21:16]: Rx FIFO-1 read index */
#define CANFD_FSR_RI_1_Pos                  (16U)
#define CANFD_FSR_RI_1_Msk                  (0x3FUL << CANFD_FSR_RI_1_Pos)
#define CANFD_FSR_RI_1                      CANFD_FSR_RI_1_Msk
/** @brief FSR[23]: Rx FIFO-1 increment read index by 1 */
#define CANFD_FSR_IRI_1_Pos                 (23U)
#define CANFD_FSR_IRI_1_Msk                 (0x1UL << CANFD_FSR_IRI_1_Pos)
#define CANFD_FSR_IRI_1                     CANFD_FSR_IRI_1_Msk
/** @brief FSR[30:24]: Rx FIFO-1 fill level */
#define CANFD_FSR_FL_1_Pos                  (24U)
#define CANFD_FSR_FL_1_Msk                  (0x7FUL << CANFD_FSR_FL_1_Pos)
#define CANFD_FSR_FL_1                      CANFD_FSR_FL_1_Msk

/******************  Bit definition for CANFD_WMR register  *******************/

/** @brief WMR[5:0]: Rx FIFO-0 full watermark */
#define CANFD_WMR_RXFWM_Pos                 (0U)
#define CANFD_WMR_RXFWM_Msk                 (0x3FUL << CANFD_WMR_RXFWM_Pos)
#define CANFD_WMR_RXFWM                     CANFD_WMR_RXFWM_Msk
/** @brief WMR[13:8]: Rx FIFO-1 full watermark */
#define CANFD_WMR_RXFWM_1_Pos               (8U)
#define CANFD_WMR_RXFWM_1_Msk               (0x3FUL << CANFD_WMR_RXFWM_1_Pos)
#define CANFD_WMR_RXFWM_1                   CANFD_WMR_RXFWM_1_Msk
/** @brief WMR[20:16]: Receive filter partition */
#define CANFD_WMR_RXFP_Pos                  (16U)
#define CANFD_WMR_RXFP_Msk                  (0x1FUL << CANFD_WMR_RXFP_Pos)
#define CANFD_WMR_RXFP                      CANFD_WMR_RXFP_Msk

/******************  Bit definition for CANFD_TBID register  ******************/

/** @brief TBID[0]: Remote transmission request */
#define CANFD_TBID_RTR_RRS_Pos              (0U)
#define CANFD_TBID_RTR_RRS_Msk              (0x1UL << CANFD_TBID_RTR_RRS_Pos)
#define CANFD_TBID_RTR_RRS                  CANFD_TBID_RTR_RRS_Msk

#define CANFD_TBID_RTR_RRS_DATA             (0x0UL << CANFD_TBID_RTR_RRS_Pos)   /*!< The frame is a CAN data frame */
#define CANFD_TBID_RTR_RRS_REMOTE           (0x1UL << CANFD_TBID_RTR_RRS_Pos)   /*!< The frame is a CAN remote frame */

/** @brief TBID[18:1]: Extended message ID */
#define CANFD_TBID_EXT_ID_Pos               (1U)
#define CANFD_TBID_EXT_ID_Msk               (0x3FFFFUL << CANFD_TBID_EXT_ID_Pos)
#define CANFD_TBID_EXT_ID                   CANFD_TBID_EXT_ID_Msk

/** @brief TBID[19]: Identifier extension */
#define CANFD_TBID_IDE_Pos                  (19U)
#define CANFD_TBID_IDE_Msk                  (0x1UL << CANFD_TBID_IDE_Pos)
#define CANFD_TBID_IDE                      CANFD_TBID_IDE_Msk

#define CANFD_TBID_IDE_STD                  (0x0UL << CANFD_TBID_IDE_Pos)   /*!< Standard identifier */
#define CANFD_TBID_IDE_EXT                  (0x1UL << CANFD_TBID_IDE_Pos)   /*!< Extended identifier */

/** @brief TBID[20]: Substitute remote transmission request */
#define CANFD_TBID_SRR_RTR_RRS_Pos          (20U)
#define CANFD_TBID_SRR_RTR_RRS_Msk          (0x1UL << CANFD_TBID_SRR_RTR_RRS_Pos)
#define CANFD_TBID_SRR_RTR_RRS              CANFD_TBID_SRR_RTR_RRS_Msk

#define CANFD_TBID_SRR_RTR_RRS_DATA         (0x0UL << CANFD_TBID_SRR_RTR_RRS_Pos)   /*!< The frame is a CAN standard data frame */
#define CANFD_TBID_SRR_RTR_RRS_REMOTE       (0x1UL << CANFD_TBID_SRR_RTR_RRS_Pos)   /*!< The frame is a CAN standard remote frame */

/** @brief TBID[31:21]: Standard message ID */
#define CANFD_TBID_STD_ID_Pos               (21U)
#define CANFD_TBID_STD_ID_Msk               (0x7FFUL << CANFD_TBID_STD_ID_Pos)
#define CANFD_TBID_STD_ID                   CANFD_TBID_STD_ID_Msk

/******************  Bit definition for CANFD_TBDLC register  *****************/

/** @brief TBDLC[23:16]: Message marker */
#define CANFD_TBDLC_MM_Pos                  (16U)
#define CANFD_TBDLC_MM_Msk                  (0xFFUL << CANFD_TBDLC_MM_Pos)
#define CANFD_TBDLC_MM                      CANFD_TBDLC_MM_Msk

/** @brief TBDLC[24]: Event FIFO control */
#define CANFD_TBDLC_EFC_Pos                 (24U)
#define CANFD_TBDLC_EFC_Msk                 (0x1UL << CANFD_TBDLC_EFC_Pos)
#define CANFD_TBDLC_EFC                     CANFD_TBDLC_EFC_Msk

#define CANFD_TBDLC_EFC_DISABLE             (0x0UL << CANFD_TBDLC_EFC_Pos)  /*!< Don't store Tx events */
#define CANFD_TBDLC_EFC_ENABLE              (0x1UL << CANFD_TBDLC_EFC_Pos)  /*!< Store Tx events */

/** @brief TBDLC[26]: Bitrate switch */
#define CANFD_TBDLC_BRS_Pos                 (26U)
#define CANFD_TBDLC_BRS_Msk                 (0x1UL << CANFD_TBDLC_BRS_Pos)
#define CANFD_TBDLC_BRS                     CANFD_TBDLC_BRS_Msk

#define CANFD_TBDLC_BRS_DISABLE             (0x0UL << CANFD_TBDLC_BRS_Pos)  /*!< Bit rate is not switched */
#define CANFD_TBDLC_BRS_ENABLE              (0x1UL << CANFD_TBDLC_BRS_Pos)  /*!< Bit rate is switched */

/** @brief TBDLC[27]: Extended data length / FD frame format */
#define CANFD_TBDLC_EDL_FDF_Pos             (27U)
#define CANFD_TBDLC_EDL_FDF_Msk             (0x1UL << CANFD_TBDLC_EDL_FDF_Pos)
#define CANFD_TBDLC_EDL_FDF                 CANFD_TBDLC_EDL_FDF_Msk

#define CANFD_TBDLC_EDL_FDF_CAN             (0x0UL << CANFD_TBDLC_EDL_FDF_Pos)  /*!< CAN format frame */
#define CANFD_TBDLC_EDL_FDF_CANFD           (0x1UL << CANFD_TBDLC_EDL_FDF_Pos)  /*!< CAN FD format frame */

/** @brief TBDLC[31:28]: Data length code */
#define CANFD_TBDLC_DLC_Pos                 (28U)
#define CANFD_TBDLC_DLC_Msk                 (0xFUL << CANFD_TBDLC_DLC_Pos)
#define CANFD_TBDLC_DLC                     CANFD_TBDLC_DLC_Msk

/******************  Bit definition for CANFD_TBDW register  ******************/

/** @brief TBDW[7:0]: Data byte 3 */
#define CANFD_TBDW_BYTE3_Pos                (0U)
#define CANFD_TBDW_BYTE3_Msk                (0xFFUL << CANFD_TBDW_BYTE3_Pos)
#define CANFD_TBDW_BYTE3                    CANFD_TBDW_BYTE3_Msk
/** @brief TBDW[15:8]: Data byte 2 */
#define CANFD_TBDW_BYTE2_Pos                (8U)
#define CANFD_TBDW_BYTE2_Msk                (0xFFUL << CANFD_TBDW_BYTE2_Pos)
#define CANFD_TBDW_BYTE2                    CANFD_TBDW_BYTE2_Msk
/** @brief TBDW[23:16]: Data byte 1 */
#define CANFD_TBDW_BYTE1_Pos                (16U)
#define CANFD_TBDW_BYTE1_Msk                (0xFFUL << CANFD_TBDW_BYTE1_Pos)
#define CANFD_TBDW_BYTE1                    CANFD_TBDW_BYTE1_Msk
/** @brief TBDW[31:24]: Data byte 0 */
#define CANFD_TBDW_BYTE0_Pos                (24U)
#define CANFD_TBDW_BYTE0_Msk                (0xFFUL << CANFD_TBDW_BYTE0_Pos)
#define CANFD_TBDW_BYTE0                    CANFD_TBDW_BYTE0_Msk

/******************  Bit definition for CANFD_AFMR register  ******************/

/** @brief AFMR[0]: Remote transmission request mask */
#define CANFD_AFMR_AMRTR_Pos                (0U)
#define CANFD_AFMR_AMRTR_Msk                (0x1UL << CANFD_AFMR_AMRTR_Pos)
#define CANFD_AFMR_AMRTR                    CANFD_AFMR_AMRTR_Msk
/** @brief AFMR[18:1]: Extended message ID mask */
#define CANFD_AFMR_EXT_AMID_Pos             (1U)
#define CANFD_AFMR_EXT_AMID_Msk             (0x3FFFFUL << CANFD_AFMR_EXT_AMID_Pos)
#define CANFD_AFMR_EXT_AMID                 CANFD_AFMR_EXT_AMID_Msk
/** @brief AFMR[19]: Identifier extension mask */
#define CANFD_AFMR_AMIDE_Pos                (19U)
#define CANFD_AFMR_AMIDE_Msk                (0x1UL << CANFD_AFMR_AMIDE_Pos)
#define CANFD_AFMR_AMIDE                    CANFD_AFMR_AMIDE_Msk
/** @brief AFMR[20]: Substitute remote transmission request mask */
#define CANFD_AFMR_AMSRR_Pos                (20U)
#define CANFD_AFMR_AMSRR_Msk                (0x1UL << CANFD_AFMR_AMSRR_Pos)
#define CANFD_AFMR_AMSRR                    CANFD_AFMR_AMSRR_Msk
/** @brief AFMR[31:21]: Standard message ID mask */
#define CANFD_AFMR_STD_AMID_Pos             (21U)
#define CANFD_AFMR_STD_AMID_Msk             (0x7FFUL << CANFD_AFMR_STD_AMID_Pos)
#define CANFD_AFMR_STD_AMID                 CANFD_AFMR_STD_AMID_Msk

/******************  Bit definition for CANFD_AFIR register  ******************/

/** @brief AFIR[0]: Remote transmission request */
#define CANFD_AFIR_AIRTR_Pos                (0U)
#define CANFD_AFIR_AIRTR_Msk                (0x1UL << CANFD_AFIR_AIRTR_Pos)
#define CANFD_AFIR_AIRTR                    CANFD_AFIR_AIRTR_Msk
/** @brief AFIR[18:1]: Extended message ID */
#define CANFD_AFIR_EXT_AIID_Pos             (1U)
#define CANFD_AFIR_EXT_AIID_Msk             (0x3FFFFUL << CANFD_AFIR_EXT_AIID_Pos)
#define CANFD_AFIR_EXT_AIID                 CANFD_AFIR_EXT_AIID_Msk
/** @brief AFIR[19]: Identifier extension */
#define CANFD_AFIR_AIIDE_Pos                (19U)
#define CANFD_AFIR_AIIDE_Msk                (0x1UL << CANFD_AFIR_AIIDE_Pos)
#define CANFD_AFIR_AIIDE                    CANFD_AFIR_AIIDE_Msk
/** @brief AFIR[20]: Substitute remote transmission request */
#define CANFD_AFIR_AISRR_Pos                (20U)
#define CANFD_AFIR_AISRR_Msk                (0x1UL << CANFD_AFIR_AISRR_Pos)
#define CANFD_AFIR_AISRR                    CANFD_AFIR_AISRR_Msk
/** @brief AFIR[31:21]: Standard message ID */
#define CANFD_AFIR_STD_AIID_Pos             (21U)
#define CANFD_AFIR_STD_AIID_Msk             (0x7FFUL << CANFD_AFIR_STD_AIID_Pos)
#define CANFD_AFIR_STD_AIID                 CANFD_AFIR_STD_AIID_Msk

/****************  Bit definition for CANFD_TXE_TBID register  ****************/

/** @brief TXE_TBID[0]: Remote transmission request */
#define CANFD_TXE_TBID_RTR_RRS_Pos          (0U)
#define CANFD_TXE_TBID_RTR_RRS_Msk          (0x1UL << CANFD_TXE_TBID_RTR_RRS_Pos)
#define CANFD_TXE_TBID_RTR_RRS              CANFD_TXE_TBID_RTR_RRS_Msk

#define CANFD_TXE_TBID_RTR_RRS_DATA         (0x0UL << CANFD_TXE_TBID_RTR_RRS_Pos)   /*!< The frame is a CAN data frame */
#define CANFD_TXE_TBID_RTR_RRS_REMOTE       (0x1UL << CANFD_TXE_TBID_RTR_RRS_Pos)   /*!< The frame is a CAN remote frame */

/** @brief TXE_TBID[18:1]: Extended message ID */
#define CANFD_TXE_TBID_EXT_ID_Pos           (1U)
#define CANFD_TXE_TBID_EXT_ID_Msk           (0x3FFFFUL << CANFD_TXE_TBID_EXT_ID_Pos)
#define CANFD_TXE_TBID_EXT_ID               CANFD_TXE_TBID_EXT_ID_Msk

/** @brief TXE_TBID[19]: Identifier extension */
#define CANFD_TXE_TBID_IDE_Pos              (19U)
#define CANFD_TXE_TBID_IDE_Msk              (0x1UL << CANFD_TXE_TBID_IDE_Pos)
#define CANFD_TXE_TBID_IDE                  CANFD_TXE_TBID_IDE_Msk

#define CANFD_TXE_TBID_IDE_STD              (0x0UL << CANFD_TXE_TBID_IDE_Pos)   /*!< Standard identifier */
#define CANFD_TXE_TBID_IDE_EXT              (0x1UL << CANFD_TXE_TBID_IDE_Pos)   /*!< Extended identifier */

/** @brief TXE_TBID[20]: Substitute remote transmission request */
#define CANFD_TXE_TBID_SRR_RTR_RRS_Pos      (20U)
#define CANFD_TXE_TBID_SRR_RTR_RRS_Msk      (0x1UL << CANFD_TXE_TBID_SRR_RTR_RRS_Pos)
#define CANFD_TXE_TBID_SRR_RTR_RRS          CANFD_TXE_TBID_SRR_RTR_RRS_Msk

#define CANFD_TXE_TBID_SRR_RTR_RRS_DATA     (0x0UL << CANFD_TXE_TBID_SRR_RTR_RRS_Pos)   /*!< The frame is a CAN standard data frame */
#define CANFD_TXE_TBID_SRR_RTR_RRS_REMOTE   (0x1UL << CANFD_TXE_TBID_SRR_RTR_RRS_Pos)   /*!< The frame is a CAN standard remote frame */

/** @brief TXE_TBID[31:21]: Standard message ID */
#define CANFD_TXE_TBID_STD_ID_Pos           (21U)
#define CANFD_TXE_TBID_STD_ID_Msk           (0x7FFUL << CANFD_TXE_TBID_STD_ID_Pos)
#define CANFD_TXE_TBID_STD_ID               CANFD_TXE_TBID_STD_ID_Msk

/***************  Bit definition for CANFD_TXE_TBDLC register  ****************/

/** @brief TXE_TBDLC[15:0]: Timestamp captured after SOF bit */
#define CANFD_TXE_TBDLC_TS_Pos              (0U)
#define CANFD_TXE_TBDLC_TS_Msk              (0xFFFFUL << CANFD_TXE_TBDLC_TS_Pos)
#define CANFD_TXE_TBDLC_TS                  CANFD_TXE_TBDLC_TS_Msk
/** @brief TXE_TBDLC[23:16]: Message marker */
#define CANFD_TXE_TBDLC_MM_Pos              (16U)
#define CANFD_TXE_TBDLC_MM_Msk              (0xFFUL << CANFD_TXE_TBDLC_MM_Pos)
#define CANFD_TXE_TBDLC_MM                  CANFD_TXE_TBDLC_MM_Msk

/** @brief TXE_TBDLC[24]: Event type */
#define CANFD_TXE_TBDLC_ET_Pos              (24U)
#define CANFD_TXE_TBDLC_ET_Msk              (0x3UL << CANFD_TXE_TBDLC_ET_Pos)
#define CANFD_TXE_TBDLC_ET                  CANFD_TXE_TBDLC_ET_Msk

#define CANFD_TXE_TBDLC_ET_FORCED           (0x1UL << CANFD_TXE_TBDLC_ET_Pos)   /*!< Transmitted in spite of cancellation request or DAR mode transmissions */
#define CANFD_TXE_TBDLC_ET_TRANSMITTED      (0x3UL << CANFD_TXE_TBDLC_ET_Pos)   /*!< Transmitted */

/** @brief TXE_TBDLC[26]: Bitrate switch */
#define CANFD_TXE_TBDLC_BRS_Pos             (26U)
#define CANFD_TXE_TBDLC_BRS_Msk             (0x1UL << CANFD_TXE_TBDLC_BRS_Pos)
#define CANFD_TXE_TBDLC_BRS                 CANFD_TXE_TBDLC_BRS_Msk

#define CANFD_TXE_TBDLC_BRS_DISABLE         (0x0UL << CANFD_TXE_TBDLC_BRS_Pos)  /*!< Bit rate is not switched */
#define CANFD_TXE_TBDLC_BRS_ENABLE          (0x1UL << CANFD_TXE_TBDLC_BRS_Pos)  /*!< Bit rate is switched */

/** @brief TXE_TBDLC[27]: Extended data length / FD frame format */
#define CANFD_TXE_TBDLC_EDL_FDF_Pos         (27U)
#define CANFD_TXE_TBDLC_EDL_FDF_Msk         (0x1UL << CANFD_TXE_TBDLC_EDL_FDF_Pos)
#define CANFD_TXE_TBDLC_EDL_FDF             CANFD_TXE_TBDLC_EDL_FDF_Msk

#define CANFD_TXE_TBDLC_EDL_FDF_CAN         (0x0UL << CANFD_TXE_TBDLC_EDL_FDF_Pos)  /*!< CAN format frame */
#define CANFD_TXE_TBDLC_EDL_FDF_CANFD       (0x1UL << CANFD_TXE_TBDLC_EDL_FDF_Pos)  /*!< CAN FD format frame */

/** @brief TXE_TBDLC[31:28]: Data length code */
#define CANFD_TXE_TBDLC_DLC_Pos             (28U)
#define CANFD_TXE_TBDLC_DLC_Msk             (0xFUL << CANFD_TXE_TBDLC_DLC_Pos)
#define CANFD_TXE_TBDLC_DLC                 CANFD_TXE_TBDLC_DLC_Msk

/******************  Bit definition for CANFD_RBID register  ******************/

/** @brief RBID[0]: Remote transmission request */
#define CANFD_RBID_RTR_RRS_Pos              (0U)
#define CANFD_RBID_RTR_RRS_Msk              (0x1UL << CANFD_RBID_RTR_RRS_Pos)
#define CANFD_RBID_RTR_RRS                  CANFD_RBID_RTR_RRS_Msk

#define CANFD_RBID_RTR_RRS_DATA             (0x0UL << CANFD_RBID_RTR_RRS_Pos)   /*!< The frame is a CAN data frame */
#define CANFD_RBID_RTR_RRS_REMOTE           (0x1UL << CANFD_RBID_RTR_RRS_Pos)   /*!< The frame is a CAN remote frame */

/** @brief RBID[18:1]: Extended message ID */
#define CANFD_RBID_EXT_ID_Pos               (1U)
#define CANFD_RBID_EXT_ID_Msk               (0x3FFFFUL << CANFD_RBID_EXT_ID_Pos)
#define CANFD_RBID_EXT_ID                   CANFD_RBID_EXT_ID_Msk

/** @brief RBID[19]: Identifier extension */
#define CANFD_RBID_IDE_Pos                  (19U)
#define CANFD_RBID_IDE_Msk                  (0x1UL << CANFD_RBID_IDE_Pos)
#define CANFD_RBID_IDE                      CANFD_RBID_IDE_Msk

#define CANFD_RBID_IDE_STD                  (0x0UL << CANFD_RBID_IDE_Pos)   /*!< Standard identifier */
#define CANFD_RBID_IDE_EXT                  (0x1UL << CANFD_RBID_IDE_Pos)   /*!< Extended identifier */

/** @brief RBID[20]: Substitute remote transmission request */
#define CANFD_RBID_SRR_RTR_RRS_Pos          (20U)
#define CANFD_RBID_SRR_RTR_RRS_Msk          (0x1UL << CANFD_RBID_SRR_RTR_RRS_Pos)
#define CANFD_RBID_SRR_RTR_RRS              CANFD_RBID_SRR_RTR_RRS_Msk

#define CANFD_RBID_SRR_RTR_RRS_DATA         (0x0UL << CANFD_RBID_SRR_RTR_RRS_Pos)   /*!< The frame is a CAN standard data frame */
#define CANFD_RBID_SRR_RTR_RRS_REMOTE       (0x1UL << CANFD_RBID_SRR_RTR_RRS_Pos)   /*!< The frame is a CAN standard remote frame */

/** @brief RBID[31:21]: Standard message ID */
#define CANFD_RBID_STD_ID_Pos               (21U)
#define CANFD_RBID_STD_ID_Msk               (0x7FFUL << CANFD_RBID_STD_ID_Pos)
#define CANFD_RBID_STD_ID                   CANFD_RBID_STD_ID_Msk

/******************  Bit definition for CANFD_RBDLC register  *****************/

/** @brief RBDLC[15:0]: Timestamp captured after SOF bit */
#define CANFD_RBDLC_TS_Pos                  (0U)
#define CANFD_RBDLC_TS_Msk                  (0xFFFFUL << CANFD_RBDLC_TS_Pos)
#define CANFD_RBDLC_TS                      CANFD_RBDLC_TS_Msk
/** @brief RBDLC[20:16]: Matched filter index */
#define CANFD_RBDLC_MFI_Pos                 (16U)
#define CANFD_RBDLC_MFI_Msk                 (0x1FUL << CANFD_RBDLC_MFI_Pos)
#define CANFD_RBDLC_MFI                     CANFD_RBDLC_MFI_Msk
/** @brief RBDLC[25]: Error state indicator */
#define CANFD_RBDLC_ESI_Pos                 (25U)
#define CANFD_RBDLC_ESI_Msk                 (0x1UL << CANFD_RBDLC_ESI_Pos)
#define CANFD_RBDLC_ESI                     CANFD_RBDLC_ESI_Msk

#define CANFD_RBDLC_ESI_ACTIVE              (0x0UL << CANFD_RBDLC_ESI_Pos)  /*!< The frame was sent by the error active transmitter */
#define CANFD_RBDLC_ESI_PASSIVE             (0x1UL << CANFD_RBDLC_ESI_Pos)  /*!< The frame was sent by the error passive transmitter */

/** @brief RBDLC[26]: Bitrate switch */
#define CANFD_RBDLC_BRS_Pos                 (26U)
#define CANFD_RBDLC_BRS_Msk                 (0x1UL << CANFD_RBDLC_BRS_Pos)
#define CANFD_RBDLC_BRS                     CANFD_RBDLC_BRS_Msk

#define CANFD_RBDLC_BRS_DISABLE             (0x0UL << CANFD_RBDLC_BRS_Pos)  /*!< Bit rate is not switched */
#define CANFD_RBDLC_BRS_ENABLE              (0x1UL << CANFD_RBDLC_BRS_Pos)  /*!< Bit rate is switched */

/** @brief RBDLC[27]: Extended data length / FD frame format */
#define CANFD_RBDLC_EDL_FDF_Pos             (27U)
#define CANFD_RBDLC_EDL_FDF_Msk             (0x1UL << CANFD_RBDLC_EDL_FDF_Pos)
#define CANFD_RBDLC_EDL_FDF                 CANFD_RBDLC_EDL_FDF_Msk

#define CANFD_RBDLC_EDL_FDF_CAN             (0x0UL << CANFD_RBDLC_EDL_FDF_Pos)  /*!< CAN format frame */
#define CANFD_RBDLC_EDL_FDF_CANFD           (0x1UL << CANFD_RBDLC_EDL_FDF_Pos)  /*!< CAN FD format frame */

/** @brief RBDLC[31:28]: Data length code */
#define CANFD_RBDLC_DLC_Pos                 (28U)
#define CANFD_RBDLC_DLC_Msk                 (0xFUL << CANFD_RBDLC_DLC_Pos)
#define CANFD_RBDLC_DLC                     CANFD_RBDLC_DLC_Msk

/******************  Bit definition for CANFD_RBDW register  ******************/

/** @brief RBDW[7:0]: Data byte 3 */
#define CANFD_RBDW_BYTE3_Pos                (0U)
#define CANFD_RBDW_BYTE3_Msk                (0xFFUL << CANFD_RBDW_BYTE3_Pos)
#define CANFD_RBDW_BYTE3                    CANFD_RBDW_BYTE3_Msk
/** @brief RBDW[15:8]: Data byte 2 */
#define CANFD_RBDW_BYTE2_Pos                (8U)
#define CANFD_RBDW_BYTE2_Msk                (0xFFUL << CANFD_RBDW_BYTE2_Pos)
#define CANFD_RBDW_BYTE2                    CANFD_RBDW_BYTE2_Msk
/** @brief RBDW[23:16]: Data byte 1 */
#define CANFD_RBDW_BYTE1_Pos                (16U)
#define CANFD_RBDW_BYTE1_Msk                (0xFFUL << CANFD_RBDW_BYTE1_Pos)
#define CANFD_RBDW_BYTE1                    CANFD_RBDW_BYTE1_Msk
/** @brief RBDW[31:24]: Data byte 0 */
#define CANFD_RBDW_BYTE0_Pos                (24U)
#define CANFD_RBDW_BYTE0_Msk                (0xFFUL << CANFD_RBDW_BYTE0_Pos)
#define CANFD_RBDW_BYTE0                    CANFD_RBDW_BYTE0_Msk

/******************************************************************************/
/*                         Control Registers Unit                             */
/******************************************************************************/

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

/******************  Bit definition for CRU_PCLK0EN register  *****************/

/** @brief PCLK0EN[0]: QSPI0 clock enable */
#define CRU_PCLK0EN_QSPI0CLKEN_Pos          (0U)
#define CRU_PCLK0EN_QSPI0CLKEN_Msk          (0x1UL << CRU_PCLK0EN_QSPI0CLKEN_Pos)
#define CRU_PCLK0EN_QSPI0CLKEN              CRU_PCLK0EN_QSPI0CLKEN_Msk
/** @brief PCLK0EN[1]: SPI0 clock enable */
#define CRU_PCLK0EN_SPI0CLKEN_Pos           (1U)
#define CRU_PCLK0EN_SPI0CLKEN_Msk           (0x1UL << CRU_PCLK0EN_SPI0CLKEN_Pos)
#define CRU_PCLK0EN_SPI0CLKEN               CRU_PCLK0EN_SPI0CLKEN_Msk
/** @brief PCLK0EN[2]: SPI1 clock enable */
#define CRU_PCLK0EN_SPI1CLKEN_Pos           (2U)
#define CRU_PCLK0EN_SPI1CLKEN_Msk           (0x1UL << CRU_PCLK0EN_SPI1CLKEN_Pos)
#define CRU_PCLK0EN_SPI1CLKEN               CRU_PCLK0EN_SPI1CLKEN_Msk
/** @brief PCLK0EN[3]: UART0 clock enable */
#define CRU_PCLK0EN_UART0CLKEN_Pos          (3U)
#define CRU_PCLK0EN_UART0CLKEN_Msk          (0x1UL << CRU_PCLK0EN_UART0CLKEN_Pos)
#define CRU_PCLK0EN_UART0CLKEN              CRU_PCLK0EN_UART0CLKEN_Msk
/** @brief PCLK0EN[4]: UART1 clock enable */
#define CRU_PCLK0EN_UART1CLKEN_Pos          (4U)
#define CRU_PCLK0EN_UART1CLKEN_Msk          (0x1UL << CRU_PCLK0EN_UART1CLKEN_Pos)
#define CRU_PCLK0EN_UART1CLKEN              CRU_PCLK0EN_UART1CLKEN_Msk
/** @brief PCLK0EN[5]: UART2 clock enable */
#define CRU_PCLK0EN_UART2CLKEN_Pos          (5U)
#define CRU_PCLK0EN_UART2CLKEN_Msk          (0x1UL << CRU_PCLK0EN_UART2CLKEN_Pos)
#define CRU_PCLK0EN_UART2CLKEN              CRU_PCLK0EN_UART2CLKEN_Msk
/** @brief PCLK0EN[6]: I2C0 clock enable */
#define CRU_PCLK0EN_I2C0CLKEN_Pos           (6U)
#define CRU_PCLK0EN_I2C0CLKEN_Msk           (0x1UL << CRU_PCLK0EN_I2C0CLKEN_Pos)
#define CRU_PCLK0EN_I2C0CLKEN               CRU_PCLK0EN_I2C0CLKEN_Msk
/** @brief PCLK0EN[7]: I2C1 clock enable */
#define CRU_PCLK0EN_I2C1CLKEN_Pos           (7U)
#define CRU_PCLK0EN_I2C1CLKEN_Msk           (0x1UL << CRU_PCLK0EN_I2C1CLKEN_Pos)
#define CRU_PCLK0EN_I2C1CLKEN               CRU_PCLK0EN_I2C1CLKEN_Msk
/** @brief PCLK0EN[8]: I2S0 clock enable */
#define CRU_PCLK0EN_I2S0CLKEN_Pos           (8U)
#define CRU_PCLK0EN_I2S0CLKEN_Msk           (0x1UL << CRU_PCLK0EN_I2S0CLKEN_Pos)
#define CRU_PCLK0EN_I2S0CLKEN               CRU_PCLK0EN_I2S0CLKEN_Msk
/** @brief PCLK0EN[9]: TIM0 clock enable */
#define CRU_PCLK0EN_TIM0CLKEN_Pos           (9U)
#define CRU_PCLK0EN_TIM0CLKEN_Msk           (0x1UL << CRU_PCLK0EN_TIM0CLKEN_Pos)
#define CRU_PCLK0EN_TIM0CLKEN               CRU_PCLK0EN_TIM0CLKEN_Msk
/** @brief PCLK0EN[10]: WDT0 clock enable */
#define CRU_PCLK0EN_WDT0CLKEN_Pos           (10U)
#define CRU_PCLK0EN_WDT0CLKEN_Msk           (0x1UL << CRU_PCLK0EN_WDT0CLKEN_Pos)
#define CRU_PCLK0EN_WDT0CLKEN               CRU_PCLK0EN_WDT0CLKEN_Msk
/** @brief PCLK0EN[11]: GPIO0 clock enable */
#define CRU_PCLK0EN_GPIO0CLKEN_Pos          (11U)
#define CRU_PCLK0EN_GPIO0CLKEN_Msk          (0x1UL << CRU_PCLK0EN_GPIO0CLKEN_Pos)
#define CRU_PCLK0EN_GPIO0CLKEN              CRU_PCLK0EN_GPIO0CLKEN_Msk
/** @brief PCLK0EN[12]: PWMG0 clock enable */
#define CRU_PCLK0EN_PWMG0CLKEN_Pos          (12U)
#define CRU_PCLK0EN_PWMG0CLKEN_Msk          (0x1UL << CRU_PCLK0EN_PWMG0CLKEN_Pos)
#define CRU_PCLK0EN_PWMG0CLKEN              CRU_PCLK0EN_PWMG0CLKEN_Msk
/** @brief PCLK0EN[13]: CANFD0 clock enable */
#define CRU_PCLK0EN_CANFD0CLKEN_Pos         (13U)
#define CRU_PCLK0EN_CANFD0CLKEN_Msk         (0x1UL << CRU_PCLK0EN_CANFD0CLKEN_Pos)
#define CRU_PCLK0EN_CANFD0CLKEN             CRU_PCLK0EN_CANFD0CLKEN_Msk

/******************  Bit definition for CRU_PCLK1EN register  *****************/

/** @brief PCLK1EN[0]: QSPI1 clock enable */
#define CRU_PCLK1EN_QSPI1CLKEN_Pos          (0U)
#define CRU_PCLK1EN_QSPI1CLKEN_Msk          (0x1UL << CRU_PCLK1EN_QSPI1CLKEN_Pos)
#define CRU_PCLK1EN_QSPI1CLKEN              CRU_PCLK1EN_QSPI1CLKEN_Msk
/** @brief PCLK1EN[1]: SPI2 clock enable */
#define CRU_PCLK1EN_SPI2CLKEN_Pos           (1U)
#define CRU_PCLK1EN_SPI2CLKEN_Msk           (0x1UL << CRU_PCLK1EN_SPI2CLKEN_Pos)
#define CRU_PCLK1EN_SPI2CLKEN               CRU_PCLK1EN_SPI2CLKEN_Msk
/** @brief PCLK1EN[2]: SPI3 clock enable */
#define CRU_PCLK1EN_SPI3CLKEN_Pos           (2U)
#define CRU_PCLK1EN_SPI3CLKEN_Msk           (0x1UL << CRU_PCLK1EN_SPI3CLKEN_Pos)
#define CRU_PCLK1EN_SPI3CLKEN               CRU_PCLK1EN_SPI3CLKEN_Msk
/** @brief PCLK1EN[3]: UART3 clock enable */
#define CRU_PCLK1EN_UART3CLKEN_Pos          (3U)
#define CRU_PCLK1EN_UART3CLKEN_Msk          (0x1UL << CRU_PCLK1EN_UART3CLKEN_Pos)
#define CRU_PCLK1EN_UART3CLKEN              CRU_PCLK1EN_UART3CLKEN_Msk
/** @brief PCLK1EN[4]: UART4 clock enable */
#define CRU_PCLK1EN_UART4CLKEN_Pos          (4U)
#define CRU_PCLK1EN_UART4CLKEN_Msk          (0x1UL << CRU_PCLK1EN_UART4CLKEN_Pos)
#define CRU_PCLK1EN_UART4CLKEN              CRU_PCLK1EN_UART4CLKEN_Msk
/** @brief PCLK1EN[5]: UART5 clock enable */
#define CRU_PCLK1EN_UART5CLKEN_Pos          (5U)
#define CRU_PCLK1EN_UART5CLKEN_Msk          (0x1UL << CRU_PCLK1EN_UART5CLKEN_Pos)
#define CRU_PCLK1EN_UART5CLKEN              CRU_PCLK1EN_UART5CLKEN_Msk
/** @brief PCLK1EN[6]: I2C2 clock enable */
#define CRU_PCLK1EN_I2C2CLKEN_Pos           (6U)
#define CRU_PCLK1EN_I2C2CLKEN_Msk           (0x1UL << CRU_PCLK1EN_I2C2CLKEN_Pos)
#define CRU_PCLK1EN_I2C2CLKEN               CRU_PCLK1EN_I2C2CLKEN_Msk
/** @brief PCLK1EN[7]: I2C3 clock enable */
#define CRU_PCLK1EN_I2C3CLKEN_Pos           (7U)
#define CRU_PCLK1EN_I2C3CLKEN_Msk           (0x1UL << CRU_PCLK1EN_I2C3CLKEN_Pos)
#define CRU_PCLK1EN_I2C3CLKEN               CRU_PCLK1EN_I2C3CLKEN_Msk
/** @brief PCLK1EN[8]: I2S1 clock enable */
#define CRU_PCLK1EN_I2S1CLKEN_Pos           (8U)
#define CRU_PCLK1EN_I2S1CLKEN_Msk           (0x1UL << CRU_PCLK1EN_I2S1CLKEN_Pos)
#define CRU_PCLK1EN_I2S1CLKEN               CRU_PCLK1EN_I2S1CLKEN_Msk
/** @brief PCLK1EN[9]: TIM1 clock enable */
#define CRU_PCLK1EN_TIM1CLKEN_Pos           (9U)
#define CRU_PCLK1EN_TIM1CLKEN_Msk           (0x1UL << CRU_PCLK1EN_TIM1CLKEN_Pos)
#define CRU_PCLK1EN_TIM1CLKEN               CRU_PCLK1EN_TIM1CLKEN_Msk
/** @brief PCLK1EN[10]: WDT1 clock enable */
#define CRU_PCLK1EN_WDT1CLKEN_Pos           (10U)
#define CRU_PCLK1EN_WDT1CLKEN_Msk           (0x1UL << CRU_PCLK1EN_WDT1CLKEN_Pos)
#define CRU_PCLK1EN_WDT1CLKEN               CRU_PCLK1EN_WDT1CLKEN_Msk
/** @brief PCLK1EN[11]: GPIO1 clock enable */
#define CRU_PCLK1EN_GPIO1CLKEN_Pos          (11U)
#define CRU_PCLK1EN_GPIO1CLKEN_Msk          (0x1UL << CRU_PCLK1EN_GPIO1CLKEN_Pos)
#define CRU_PCLK1EN_GPIO1CLKEN              CRU_PCLK1EN_GPIO1CLKEN_Msk
/** @brief PCLK1EN[12]: PWMG1 clock enable */
#define CRU_PCLK1EN_PWMG1CLKEN_Pos          (12U)
#define CRU_PCLK1EN_PWMG1CLKEN_Msk          (0x1UL << CRU_PCLK1EN_PWMG1CLKEN_Pos)
#define CRU_PCLK1EN_PWMG1CLKEN              CRU_PCLK1EN_PWMG1CLKEN_Msk
/** @brief PCLK1EN[13]: CANFD1 clock enable */
#define CRU_PCLK1EN_CANFD1CLKEN_Pos         (13U)
#define CRU_PCLK1EN_CANFD1CLKEN_Msk         (0x1UL << CRU_PCLK1EN_CANFD1CLKEN_Pos)
#define CRU_PCLK1EN_CANFD1CLKEN             CRU_PCLK1EN_CANFD1CLKEN_Msk

/******************  Bit definition for CRU_PCLK2EN register  *****************/

/** @brief PCLK2EN[0]: ADC0 clock enable */
#define CRU_PCLK2EN_ADC0CLKEN_Pos           (0U)
#define CRU_PCLK2EN_ADC0CLKEN_Msk           (0x1UL << CRU_PCLK2EN_ADC0CLKEN_Pos)
#define CRU_PCLK2EN_ADC0CLKEN               CRU_PCLK2EN_ADC0CLKEN_Msk
/** @brief PCLK2EN[1]: ADC1 clock enable */
#define CRU_PCLK2EN_ADC1CLKEN_Pos           (1U)
#define CRU_PCLK2EN_ADC1CLKEN_Msk           (0x1UL << CRU_PCLK2EN_ADC1CLKEN_Pos)
#define CRU_PCLK2EN_ADC1CLKEN               CRU_PCLK2EN_ADC1CLKEN_Msk
/** @brief PCLK2EN[2]: ADC2 clock enable */
#define CRU_PCLK2EN_ADC2CLKEN_Pos           (2U)
#define CRU_PCLK2EN_ADC2CLKEN_Msk           (0x1UL << CRU_PCLK2EN_ADC2CLKEN_Pos)
#define CRU_PCLK2EN_ADC2CLKEN               CRU_PCLK2EN_ADC2CLKEN_Msk
/** @brief PCLK2EN[3]: PWMA0 clock enable */
#define CRU_PCLK2EN_PWMA0CLKEN_Pos          (3U)
#define CRU_PCLK2EN_PWMA0CLKEN_Msk          (0x1UL << CRU_PCLK2EN_PWMA0CLKEN_Pos)
#define CRU_PCLK2EN_PWMA0CLKEN              CRU_PCLK2EN_PWMA0CLKEN_Msk
/** @brief PCLK2EN[4]: PWMA1 clock enable */
#define CRU_PCLK2EN_PWMA1CLKEN_Pos          (4U)
#define CRU_PCLK2EN_PWMA1CLKEN_Msk          (0x1UL << CRU_PCLK2EN_PWMA1CLKEN_Pos)
#define CRU_PCLK2EN_PWMA1CLKEN              CRU_PCLK2EN_PWMA1CLKEN_Msk
/** @brief PCLK2EN[5]: PWMA2 clock enable */
#define CRU_PCLK2EN_PWMA2CLKEN_Pos          (5U)
#define CRU_PCLK2EN_PWMA2CLKEN_Msk          (0x1UL << CRU_PCLK2EN_PWMA2CLKEN_Pos)
#define CRU_PCLK2EN_PWMA2CLKEN              CRU_PCLK2EN_PWMA2CLKEN_Msk
/** @brief PCLK2EN[6]: PWMA3 clock enable */
#define CRU_PCLK2EN_PWMA3CLKEN_Pos          (6U)
#define CRU_PCLK2EN_PWMA3CLKEN_Msk          (0x1UL << CRU_PCLK2EN_PWMA3CLKEN_Pos)
#define CRU_PCLK2EN_PWMA3CLKEN              CRU_PCLK2EN_PWMA3CLKEN_Msk
/** @brief PCLK2EN[7]: GPIO2 clock enable */
#define CRU_PCLK2EN_GPIO2CLKEN_Pos          (7U)
#define CRU_PCLK2EN_GPIO2CLKEN_Msk          (0x1UL << CRU_PCLK2EN_GPIO2CLKEN_Pos)
#define CRU_PCLK2EN_GPIO2CLKEN              CRU_PCLK2EN_GPIO2CLKEN_Msk
/** @brief PCLK2EN[8]: UART6 clock enable */
#define CRU_PCLK2EN_UART6CLKEN_Pos          (8U)
#define CRU_PCLK2EN_UART6CLKEN_Msk          (0x1UL << CRU_PCLK2EN_UART6CLKEN_Pos)
#define CRU_PCLK2EN_UART6CLKEN              CRU_PCLK2EN_UART6CLKEN_Msk
/** @brief PCLK2EN[9]: UART7 clock enable */
#define CRU_PCLK2EN_UART7CLKEN_Pos          (9U)
#define CRU_PCLK2EN_UART7CLKEN_Msk          (0x1UL << CRU_PCLK2EN_UART7CLKEN_Pos)
#define CRU_PCLK2EN_UART7CLKEN              CRU_PCLK2EN_UART7CLKEN_Msk

/*****************  Bit definition for CRU_SYSCTRL0 register  *****************/

/** @brief SYSCTRL0[0]: Core 1 clock enable */
#define CRU_SYSCTRL0_CORE1CLKEN_Pos         (0U)
#define CRU_SYSCTRL0_CORE1CLKEN_Msk         (0x1UL << CRU_SYSCTRL0_CORE1CLKEN_Pos)
#define CRU_SYSCTRL0_CORE1CLKEN             CRU_SYSCTRL0_CORE1CLKEN_Msk
/** @brief SYSCTRL0[1]: Core 1 reset */
#define CRU_SYSCTRL0_CORE1RSTN_Pos          (1U)
#define CRU_SYSCTRL0_CORE1RSTN_Msk          (0x1UL << CRU_SYSCTRL0_CORE1RSTN_Pos)
#define CRU_SYSCTRL0_CORE1RSTN              CRU_SYSCTRL0_CORE1RSTN_Msk
/** @brief SYSCTRL0[2]: Core 1 TCM arbitrator reset */
#define CRU_SYSCTRL0_CORE1FPRSTN_Pos        (2U)
#define CRU_SYSCTRL0_CORE1FPRSTN_Msk        (0x1UL << CRU_SYSCTRL0_CORE1FPRSTN_Pos)
#define CRU_SYSCTRL0_CORE1FPRSTN            CRU_SYSCTRL0_CORE1FPRSTN_Msk
/** @brief SYSCTRL0[3]: Core 1 processor complex reset */
#define CRU_SYSCTRL0_CORE1CXRSTN_Pos        (3U)
#define CRU_SYSCTRL0_CORE1CXRSTN_Msk        (0x1UL << CRU_SYSCTRL0_CORE1CXRSTN_Pos)
#define CRU_SYSCTRL0_CORE1CXRSTN            CRU_SYSCTRL0_CORE1CXRSTN_Msk
/** @brief SYSCTRL0[4]: Core 2 reset */
#define CRU_SYSCTRL0_CORE2RSTN_Pos          (4U)
#define CRU_SYSCTRL0_CORE2RSTN_Msk          (0x1UL << CRU_SYSCTRL0_CORE2RSTN_Pos)
#define CRU_SYSCTRL0_CORE2RSTN              CRU_SYSCTRL0_CORE2RSTN_Msk
/** @brief SYSCTRL0[5]: Core 2 TCM arbitrator reset */
#define CRU_SYSCTRL0_CORE2FPRSTN_Pos        (5U)
#define CRU_SYSCTRL0_CORE2FPRSTN_Msk        (0x1UL << CRU_SYSCTRL0_CORE2FPRSTN_Pos)
#define CRU_SYSCTRL0_CORE2FPRSTN            CRU_SYSCTRL0_CORE2FPRSTN_Msk
/** @brief SYSCTRL0[6]: Core 2 processor complex reset */
#define CRU_SYSCTRL0_CORE2CXRSTN_Pos        (6U)
#define CRU_SYSCTRL0_CORE2CXRSTN_Msk        (0x1UL << CRU_SYSCTRL0_CORE2CXRSTN_Pos)
#define CRU_SYSCTRL0_CORE2CXRSTN            CRU_SYSCTRL0_CORE2CXRSTN_Msk
/** @brief SYSCTRL0[7]: QSPI0 XIP mode enable */
#define CRU_SYSCTRL0_XIP0EN_Pos             (7U)
#define CRU_SYSCTRL0_XIP0EN_Msk             (0x1UL << CRU_SYSCTRL0_XIP0EN_Pos)
#define CRU_SYSCTRL0_XIP0EN                 CRU_SYSCTRL0_XIP0EN_Msk
/** @brief SYSCTRL0[8]: QSPI1 XIP mode enable */
#define CRU_SYSCTRL0_XIP1EN_Pos             (8U)
#define CRU_SYSCTRL0_XIP1EN_Msk             (0x1UL << CRU_SYSCTRL0_XIP1EN_Pos)
#define CRU_SYSCTRL0_XIP1EN                 CRU_SYSCTRL0_XIP1EN_Msk

/** @brief SYSCTRL0[9]: Select core to trace */
#define CRU_SYSCTRL0_TRACESEL_Pos           (9U)
#define CRU_SYSCTRL0_TRACESEL_Msk           (0x1UL << CRU_SYSCTRL0_TRACESEL_Pos)
#define CRU_SYSCTRL0_TRACESEL               CRU_SYSCTRL0_TRACESEL_Msk

#define CRU_SYSCTRL0_TRACESEL_CORE0         (0x0UL << CRU_SYSCTRL0_TRACESEL_Pos)    /*!< Connect Core 0 to trace module */
#define CRU_SYSCTRL0_TRACESEL_CORE1         (0x1UL << CRU_SYSCTRL0_TRACESEL_Pos)    /*!< Connect Core 1 to trace module */

/** @brief SYSCTRL0[10]: EFLASH NVR access enable */
#define CRU_SYSCTRL0_NVR_Pos                (10U)
#define CRU_SYSCTRL0_NVR_Msk                (0x1UL << CRU_SYSCTRL0_NVR_Pos)
#define CRU_SYSCTRL0_NVR                    CRU_SYSCTRL0_NVR_Msk
/** @brief SYSCTRL0[11]: CLKI monitor enable */
#define CRU_SYSCTRL0_CLKI_ALARM_EN_Pos      (11U)
#define CRU_SYSCTRL0_CLKI_ALARM_EN_Msk      (0x1UL << CRU_SYSCTRL0_CLKI_ALARM_EN_Pos)
#define CRU_SYSCTRL0_CLKI_ALARM_EN          CRU_SYSCTRL0_CLKI_ALARM_EN_Msk

/** @brief SYSCTRL0[15:12]: CLKI monitor threshold */
#define CRU_SYSCTRL0_CLKI_ALARM_LVL_Pos     (12U)
#define CRU_SYSCTRL0_CLKI_ALARM_LVL_Msk     (0xFUL << CRU_SYSCTRL0_CLKI_ALARM_LVL_Pos)
#define CRU_SYSCTRL0_CLKI_ALARM_LVL         CRU_SYSCTRL0_CLKI_ALARM_LVL_Msk

#define CRU_SYSCTRL0_CLKI_ALARM_LVL_0       (0x0UL << CRU_SYSCTRL0_CLKI_ALARM_LVL_Pos)  /*!< CLKI monitor threshold level 0 */
#define CRU_SYSCTRL0_CLKI_ALARM_LVL_1       (0x1UL << CRU_SYSCTRL0_CLKI_ALARM_LVL_Pos)  /*!< CLKI monitor threshold level 1 */
#define CRU_SYSCTRL0_CLKI_ALARM_LVL_2       (0x2UL << CRU_SYSCTRL0_CLKI_ALARM_LVL_Pos)  /*!< CLKI monitor threshold level 2 */
#define CRU_SYSCTRL0_CLKI_ALARM_LVL_3       (0x3UL << CRU_SYSCTRL0_CLKI_ALARM_LVL_Pos)  /*!< CLKI monitor threshold level 3 */
#define CRU_SYSCTRL0_CLKI_ALARM_LVL_4       (0x4UL << CRU_SYSCTRL0_CLKI_ALARM_LVL_Pos)  /*!< CLKI monitor threshold level 4 */
#define CRU_SYSCTRL0_CLKI_ALARM_LVL_5       (0x5UL << CRU_SYSCTRL0_CLKI_ALARM_LVL_Pos)  /*!< CLKI monitor threshold level 5 */
#define CRU_SYSCTRL0_CLKI_ALARM_LVL_6       (0x6UL << CRU_SYSCTRL0_CLKI_ALARM_LVL_Pos)  /*!< CLKI monitor threshold level 6 */
#define CRU_SYSCTRL0_CLKI_ALARM_LVL_7       (0x7UL << CRU_SYSCTRL0_CLKI_ALARM_LVL_Pos)  /*!< CLKI monitor threshold level 7 */
#define CRU_SYSCTRL0_CLKI_ALARM_LVL_8       (0x8UL << CRU_SYSCTRL0_CLKI_ALARM_LVL_Pos)  /*!< CLKI monitor threshold level 8 */
#define CRU_SYSCTRL0_CLKI_ALARM_LVL_9       (0x9UL << CRU_SYSCTRL0_CLKI_ALARM_LVL_Pos)  /*!< CLKI monitor threshold level 9 */
#define CRU_SYSCTRL0_CLKI_ALARM_LVL_10      (0xAUL << CRU_SYSCTRL0_CLKI_ALARM_LVL_Pos)  /*!< CLKI monitor threshold level 10 */
#define CRU_SYSCTRL0_CLKI_ALARM_LVL_11      (0xBUL << CRU_SYSCTRL0_CLKI_ALARM_LVL_Pos)  /*!< CLKI monitor threshold level 11 */
#define CRU_SYSCTRL0_CLKI_ALARM_LVL_12      (0xCUL << CRU_SYSCTRL0_CLKI_ALARM_LVL_Pos)  /*!< CLKI monitor threshold level 12 */
#define CRU_SYSCTRL0_CLKI_ALARM_LVL_13      (0xDUL << CRU_SYSCTRL0_CLKI_ALARM_LVL_Pos)  /*!< CLKI monitor threshold level 13 */
#define CRU_SYSCTRL0_CLKI_ALARM_LVL_14      (0xEUL << CRU_SYSCTRL0_CLKI_ALARM_LVL_Pos)  /*!< CLKI monitor threshold level 14 */
#define CRU_SYSCTRL0_CLKI_ALARM_LVL_15      (0xFUL << CRU_SYSCTRL0_CLKI_ALARM_LVL_Pos)  /*!< CLKI monitor threshold level 15 */

/** @brief SYSCTRL0[16]: Clear WDTHIT flag */
#define CRU_SYSCTRL0_WDTHITCLR_Pos          (16U)
#define CRU_SYSCTRL0_WDTHITCLR_Msk          (0x1UL << CRU_SYSCTRL0_WDTHITCLR_Pos)
#define CRU_SYSCTRL0_WDTHITCLR              CRU_SYSCTRL0_WDTHITCLR_Msk
/** @brief SYSCTRL0[17]: Reset WDTx */
#define CRU_SYSCTRL0_WDTCLR_Pos             (17U)
#define CRU_SYSCTRL0_WDTCLR_Msk             (0x1UL << CRU_SYSCTRL0_WDTCLR_Pos)
#define CRU_SYSCTRL0_WDTCLR                 CRU_SYSCTRL0_WDTCLR_Msk
/** @brief SYSCTRL0[20:18]: Single-port memory parameter value */
#define CRU_SYSCTRL0_EMSPVAL_Pos            (18U)
#define CRU_SYSCTRL0_EMSPVAL_Msk            (0x7UL << CRU_SYSCTRL0_EMSPVAL_Pos)
#define CRU_SYSCTRL0_EMSPVAL                CRU_SYSCTRL0_EMSPVAL_Msk
/** @brief SYSCTRL0[24:21]: Dual-port memory parameter value */
#define CRU_SYSCTRL0_EMDPVAL_Pos            (21U)
#define CRU_SYSCTRL0_EMDPVAL_Msk            (0xFUL << CRU_SYSCTRL0_EMDPVAL_Pos)
#define CRU_SYSCTRL0_EMDPVAL                CRU_SYSCTRL0_EMDPVAL_Msk
/** @brief SYSCTRL0[25]: Memory parameters adjustment enable */
#define CRU_SYSCTRL0_EMEN_Pos               (25U)
#define CRU_SYSCTRL0_EMEN_Msk               (0x1UL << CRU_SYSCTRL0_EMEN_Pos)
#define CRU_SYSCTRL0_EMEN                   CRU_SYSCTRL0_EMEN_Msk
/** @brief SYSCTRL0[26]: Disable EFLASH NVR access */
#define CRU_SYSCTRL0_EFLASHNVRDIS_Pos       (26U)
#define CRU_SYSCTRL0_EFLASHNVRDIS_Msk       (0x1UL << CRU_SYSCTRL0_EFLASHNVRDIS_Pos)
#define CRU_SYSCTRL0_EFLASHNVRDIS           CRU_SYSCTRL0_EFLASHNVRDIS_Msk
/** @brief SYSCTRL0[27]: Disable EFLASH access in manual mode */
#define CRU_SYSCTRL0_EFLASHMANDIS_Pos       (27U)
#define CRU_SYSCTRL0_EFLASHMANDIS_Msk       (0x1UL << CRU_SYSCTRL0_EFLASHMANDIS_Pos)
#define CRU_SYSCTRL0_EFLASHMANDIS           CRU_SYSCTRL0_EFLASHMANDIS_Msk
/** @brief SYSCTRL0[28]: MAGMA0 command decryptor enable */
#define CRU_SYSCTRL0_MGM0EN_Pos             (28U)
#define CRU_SYSCTRL0_MGM0EN_Msk             (0x1UL << CRU_SYSCTRL0_MGM0EN_Pos)
#define CRU_SYSCTRL0_MGM0EN                 CRU_SYSCTRL0_MGM0EN_Msk
/** @brief SYSCTRL0[29]: MAGMA1 command decryptor enable */
#define CRU_SYSCTRL0_MGM1EN_Pos             (29U)
#define CRU_SYSCTRL0_MGM1EN_Msk             (0x1UL << CRU_SYSCTRL0_MGM1EN_Pos)
#define CRU_SYSCTRL0_MGM1EN                 CRU_SYSCTRL0_MGM1EN_Msk
/** @brief SYSCTRL0[30]: USB PHY reset */
#define CRU_SYSCTRL0_USBPHYRESET_Pos        (30U)
#define CRU_SYSCTRL0_USBPHYRESET_Msk        (0x1UL << CRU_SYSCTRL0_USBPHYRESET_Pos)
#define CRU_SYSCTRL0_USBPHYRESET            CRU_SYSCTRL0_USBPHYRESET_Msk
/** @brief SYSCTRL0[31]: USB PHY PLL enable */
#define CRU_SYSCTRL0_USBPHYPLLEN_Pos        (31U)
#define CRU_SYSCTRL0_USBPHYPLLEN_Msk        (0x1UL << CRU_SYSCTRL0_USBPHYPLLEN_Pos)
#define CRU_SYSCTRL0_USBPHYPLLEN            CRU_SYSCTRL0_USBPHYPLLEN_Msk

/*****************  Bit definition for CRU_SYSCTRL1 register  *****************/

/** @brief SYSCTRL1[31:0]: Core 1 reset vector address */
#define CRU_SYSCTRL1_CORE1RSTVEC_Pos        (0U)
#define CRU_SYSCTRL1_CORE1RSTVEC_Msk        (0xFFFFFFFFUL << CRU_SYSCTRL1_CORE1RSTVEC_Pos)
#define CRU_SYSCTRL1_CORE1RSTVEC            CRU_SYSCTRL1_CORE1RSTVEC_Msk

/*****************  Bit definition for CRU_SYSCTRL2 register  *****************/

/** @brief SYSCTRL2[31:0]: Core 2 reset vector address */
#define CRU_SYSCTRL2_CORE2RSTVEC_Pos        (0U)
#define CRU_SYSCTRL2_CORE2RSTVEC_Msk        (0xFFFFFFFFUL << CRU_SYSCTRL2_CORE2RSTVEC_Pos)
#define CRU_SYSCTRL2_CORE2RSTVEC            CRU_SYSCTRL2_CORE2RSTVEC_Msk

/******************  Bit definition for CRU_PRIOR0 register  ******************/

/** @brief PRIOR0[3:0]: MP Core0 AXI Interconnect priority level */
#define CRU_PRIOR0_CORE0MP_PL_Pos           (0U)
#define CRU_PRIOR0_CORE0MP_PL_Msk           (0xFUL << CRU_PRIOR0_CORE0MP_PL_Pos)
#define CRU_PRIOR0_CORE0MP_PL               CRU_PRIOR0_CORE0MP_PL_Msk
/** @brief PRIOR0[7:4]: DIO Core0 AXI Interconnect priority level */
#define CRU_PRIOR0_CORE0DIO_PL_Pos          (4U)
#define CRU_PRIOR0_CORE0DIO_PL_Msk          (0xFUL << CRU_PRIOR0_CORE0DIO_PL_Pos)
#define CRU_PRIOR0_CORE0DIO_PL              CRU_PRIOR0_CORE0DIO_PL_Msk
/** @brief PRIOR0[11:8]: MP Core1 AXI Interconnect priority level */
#define CRU_PRIOR0_CORE1MP_PL_Pos           (8U)
#define CRU_PRIOR0_CORE1MP_PL_Msk           (0xFUL << CRU_PRIOR0_CORE1MP_PL_Pos)
#define CRU_PRIOR0_CORE1MP_PL               CRU_PRIOR0_CORE1MP_PL_Msk
/** @brief PRIOR0[15:12]: DIO Core1 AXI Interconnect priority level */
#define CRU_PRIOR0_CORE1DIO_PL_Pos          (12U)
#define CRU_PRIOR0_CORE1DIO_PL_Msk          (0xFUL << CRU_PRIOR0_CORE1DIO_PL_Pos)
#define CRU_PRIOR0_CORE1DIO_PL              CRU_PRIOR0_CORE1DIO_PL_Msk
/** @brief PRIOR0[19:16]: DMA0 AXI Interconnect priority level */
#define CRU_PRIOR0_DMA0_PL_Pos              (16U)
#define CRU_PRIOR0_DMA0_PL_Msk              (0xFUL << CRU_PRIOR0_DMA0_PL_Pos)
#define CRU_PRIOR0_DMA0_PL                  CRU_PRIOR0_DMA0_PL_Msk
/** @brief PRIOR0[23:20]: DMA1 AXI Interconnect priority level */
#define CRU_PRIOR0_DMA1_PL_Pos              (20U)
#define CRU_PRIOR0_DMA1_PL_Msk              (0xFUL << CRU_PRIOR0_DMA1_PL_Pos)
#define CRU_PRIOR0_DMA1_PL                  CRU_PRIOR0_DMA1_PL_Msk
/** @brief PRIOR0[27:24]: DMA2 AXI Interconnect priority level */
#define CRU_PRIOR0_DMA2_PL_Pos              (24U)
#define CRU_PRIOR0_DMA2_PL_Msk              (0xFUL << CRU_PRIOR0_DMA2_PL_Pos)
#define CRU_PRIOR0_DMA2_PL                  CRU_PRIOR0_DMA2_PL_Msk
/** @brief PRIOR0[31:28]: DMA USB AXI Interconnect priority level */
#define CRU_PRIOR0_DMAUSB_PL_Pos            (28U)
#define CRU_PRIOR0_DMAUSB_PL_Msk            (0xFUL << CRU_PRIOR0_DMAUSB_PL_Pos)
#define CRU_PRIOR0_DMAUSB_PL                CRU_PRIOR0_DMAUSB_PL_Msk

/******************  Bit definition for CRU_PRIOR1 register  ******************/

/** @brief PRIOR1[3:0]: FP Core0 AXI Interconnect priority level */
#define CRU_PRIOR1_CORE0FP_PL_Pos           (0U)
#define CRU_PRIOR1_CORE0FP_PL_Msk           (0xFUL << CRU_PRIOR1_CORE0FP_PL_Pos)
#define CRU_PRIOR1_CORE0FP_PL               CRU_PRIOR1_CORE0FP_PL_Msk
/** @brief PRIOR1[7:4]: FP Core1 AXI Interconnect priority level */
#define CRU_PRIOR1_CORE1FP_PL_Pos           (4U)
#define CRU_PRIOR1_CORE1FP_PL_Msk           (0xFUL << CRU_PRIOR1_CORE1FP_PL_Pos)
#define CRU_PRIOR1_CORE1FP_PL               CRU_PRIOR1_CORE1FP_PL_Msk
/** @brief PRIOR1[11:8]: SRAM AXI Interconnect priority level */
#define CRU_PRIOR1_SRAM_PL_Pos              (8U)
#define CRU_PRIOR1_SRAM_PL_Msk              (0xFUL << CRU_PRIOR1_SRAM_PL_Pos)
#define CRU_PRIOR1_SRAM_PL                  CRU_PRIOR1_SRAM_PL_Msk
/** @brief PRIOR1[15:12]: EFLASH AXI Interconnect priority level */
#define CRU_PRIOR1_EFLASH_PL_Pos            (12U)
#define CRU_PRIOR1_EFLASH_PL_Msk            (0xFUL << CRU_PRIOR1_EFLASH_PL_Pos)
#define CRU_PRIOR1_EFLASH_PL                CRU_PRIOR1_EFLASH_PL_Msk
/** @brief PRIOR1[19:16]: APB0 AXI Interconnect priority level */
#define CRU_PRIOR1_APB0_PL_Pos              (16U)
#define CRU_PRIOR1_APB0_PL_Msk              (0xFUL << CRU_PRIOR1_APB0_PL_Pos)
#define CRU_PRIOR1_APB0_PL                  CRU_PRIOR1_APB0_PL_Msk
/** @brief PRIOR1[23:20]: APB1 AXI Interconnect priority level */
#define CRU_PRIOR1_APB1_PL_Pos              (20U)
#define CRU_PRIOR1_APB1_PL_Msk              (0xFUL << CRU_PRIOR1_APB1_PL_Pos)
#define CRU_PRIOR1_APB1_PL                  CRU_PRIOR1_APB1_PL_Msk
/** @brief PRIOR1[27:24]: APB2 AXI Interconnect priority level */
#define CRU_PRIOR1_APB2_PL_Pos              (24U)
#define CRU_PRIOR1_APB2_PL_Msk              (0xFUL << CRU_PRIOR1_APB2_PL_Pos)
#define CRU_PRIOR1_APB2_PL                  CRU_PRIOR1_APB2_PL_Msk
/** @brief PRIOR1[31:28]: AHB AXI Interconnect priority level */
#define CRU_PRIOR1_AHB_PL_Pos               (28U)
#define CRU_PRIOR1_AHB_PL_Msk               (0xFUL << CRU_PRIOR1_AHB_PL_Pos)
#define CRU_PRIOR1_AHB_PL                   CRU_PRIOR1_AHB_PL_Msk

/*****************  Bit definition for CRU_PADPUEN0 register  *****************/

/** @brief PADPUEN0[15:0]: Port0[15:0] pull-up enable */
#define CRU_PADPUEN0_PUENP0_Pos             (0U)
#define CRU_PADPUEN0_PUENP0_Msk             (0xFFFFUL << CRU_PADPUEN0_PUENP0_Pos)
#define CRU_PADPUEN0_PUENP0                 CRU_PADPUEN0_PUENP0_Msk
/** @brief PADPUEN0[31:16]: Port1[15:0] pull-up enable */
#define CRU_PADPUEN0_PUENP1_Pos             (16U)
#define CRU_PADPUEN0_PUENP1_Msk             (0xFFFFUL << CRU_PADPUEN0_PUENP1_Pos)
#define CRU_PADPUEN0_PUENP1                 CRU_PADPUEN0_PUENP1_Msk

/*****************  Bit definition for CRU_PADPUEN1 register  *****************/

/** @brief PADPUEN1[15:0]: Port2[15:0] pull-up enable */
#define CRU_PADPUEN1_PUENP2_Pos             (0U)
#define CRU_PADPUEN1_PUENP2_Msk             (0xFFFFUL << CRU_PADPUEN1_PUENP2_Pos)
#define CRU_PADPUEN1_PUENP2                 CRU_PADPUEN1_PUENP2_Msk

/*****************  Bit definition for CRU_PADPDEN0 register  *****************/

/** @brief PADPDEN0[15:0]: Port0[15:0] pull-down enable */
#define CRU_PADPDEN0_PDENP0_Pos             (0U)
#define CRU_PADPDEN0_PDENP0_Msk             (0xFFFFUL << CRU_PADPDEN0_PDENP0_Pos)
#define CRU_PADPDEN0_PDENP0                 CRU_PADPDEN0_PDENP0_Msk
/** @brief PADPDEN0[31:16]: Port1[15:0] pull-down enable */
#define CRU_PADPDEN0_PDENP1_Pos             (16U)
#define CRU_PADPDEN0_PDENP1_Msk             (0xFFFFUL << CRU_PADPDEN0_PDENP1_Pos)
#define CRU_PADPDEN0_PDENP1                 CRU_PADPDEN0_PDENP1_Msk

/*****************  Bit definition for CRU_PADPDEN1 register  *****************/

/** @brief PADPDEN1[15:0]: Port2[15:0] pull-down enable */
#define CRU_PADPDEN1_PDENP2_Pos             (0U)
#define CRU_PADPDEN1_PDENP2_Msk             (0xFFFFUL << CRU_PADPDEN1_PDENP2_Pos)
#define CRU_PADPDEN1_PDENP2                 CRU_PADPDEN1_PDENP2_Msk

/******************  Bit definition for CRU_PADALT0 register  *****************/

/** @brief PADALT0[2:0]: PA[0] pin alternate function */
#define CRU_PADALT0_ALT0_Pos                (0U)
#define CRU_PADALT0_ALT0_Msk                (0x7UL << CRU_PADALT0_ALT0_Pos)
#define CRU_PADALT0_ALT0                    CRU_PADALT0_ALT0_Msk
/** @brief PADALT0[3]: PA[0] pin input enable */
#define CRU_PADALT0_IE0_Pos                 (3U)
#define CRU_PADALT0_IE0_Msk                 (0x1UL << CRU_PADALT0_IE0_Pos)
#define CRU_PADALT0_IE0                     CRU_PADALT0_IE0_Msk
/** @brief PADALT0[6:4]: PA[1] pin alternate function */
#define CRU_PADALT0_ALT1_Pos                (4U)
#define CRU_PADALT0_ALT1_Msk                (0x7UL << CRU_PADALT0_ALT1_Pos)
#define CRU_PADALT0_ALT1                    CRU_PADALT0_ALT1_Msk
/** @brief PADALT0[7]: PA[1] pin input enable */
#define CRU_PADALT0_IE1_Pos                 (7U)
#define CRU_PADALT0_IE1_Msk                 (0x1UL << CRU_PADALT0_IE1_Pos)
#define CRU_PADALT0_IE1                     CRU_PADALT0_IE1_Msk
/** @brief PADALT0[10:8]: PA[2] pin alternate function */
#define CRU_PADALT0_ALT2_Pos                (8U)
#define CRU_PADALT0_ALT2_Msk                (0x7UL << CRU_PADALT0_ALT2_Pos)
#define CRU_PADALT0_ALT2                    CRU_PADALT0_ALT2_Msk
/** @brief PADALT0[11]: PA[2] pin input enable */
#define CRU_PADALT0_IE2_Pos                 (11U)
#define CRU_PADALT0_IE2_Msk                 (0x1UL << CRU_PADALT0_IE2_Pos)
#define CRU_PADALT0_IE2                     CRU_PADALT0_IE2_Msk
/** @brief PADALT0[14:12]: PA[3] pin alternate function */
#define CRU_PADALT0_ALT3_Pos                (12U)
#define CRU_PADALT0_ALT3_Msk                (0x7UL << CRU_PADALT0_ALT3_Pos)
#define CRU_PADALT0_ALT3                    CRU_PADALT0_ALT3_Msk
/** @brief PADALT0[15]: PA[3] pin input enable */
#define CRU_PADALT0_IE3_Pos                 (15U)
#define CRU_PADALT0_IE3_Msk                 (0x1UL << CRU_PADALT0_IE3_Pos)
#define CRU_PADALT0_IE3                     CRU_PADALT0_IE3_Msk
/** @brief PADALT0[18:16]: PA[4] pin alternate function */
#define CRU_PADALT0_ALT4_Pos                (16U)
#define CRU_PADALT0_ALT4_Msk                (0x7UL << CRU_PADALT0_ALT4_Pos)
#define CRU_PADALT0_ALT4                    CRU_PADALT0_ALT4_Msk
/** @brief PADALT0[19]: PA[4] pin input enable */
#define CRU_PADALT0_IE4_Pos                 (19U)
#define CRU_PADALT0_IE4_Msk                 (0x1UL << CRU_PADALT0_IE4_Pos)
#define CRU_PADALT0_IE4                     CRU_PADALT0_IE4_Msk
/** @brief PADALT0[22:20]: PA[5] pin alternate function */
#define CRU_PADALT0_ALT5_Pos                (20U)
#define CRU_PADALT0_ALT5_Msk                (0x7UL << CRU_PADALT0_ALT5_Pos)
#define CRU_PADALT0_ALT5                    CRU_PADALT0_ALT5_Msk
/** @brief PADALT0[23]: PA[5] pin input enable */
#define CRU_PADALT0_IE5_Pos                 (23U)
#define CRU_PADALT0_IE5_Msk                 (0x1UL << CRU_PADALT0_IE5_Pos)
#define CRU_PADALT0_IE5                     CRU_PADALT0_IE5_Msk
/** @brief PADALT0[26:24]: PA[6] pin alternate function */
#define CRU_PADALT0_ALT6_Pos                (24U)
#define CRU_PADALT0_ALT6_Msk                (0x7UL << CRU_PADALT0_ALT6_Pos)
#define CRU_PADALT0_ALT6                    CRU_PADALT0_ALT6_Msk
/** @brief PADALT0[27]: PA[6] pin input enable */
#define CRU_PADALT0_IE6_Pos                 (27U)
#define CRU_PADALT0_IE6_Msk                 (0x1UL << CRU_PADALT0_IE6_Pos)
#define CRU_PADALT0_IE6                     CRU_PADALT0_IE6_Msk
/** @brief PADALT0[30:28]: PA[7] pin alternate function */
#define CRU_PADALT0_ALT7_Pos                (28U)
#define CRU_PADALT0_ALT7_Msk                (0x7UL << CRU_PADALT0_ALT7_Pos)
#define CRU_PADALT0_ALT7                    CRU_PADALT0_ALT7_Msk
/** @brief PADALT0[31]: PA[7] pin input enable */
#define CRU_PADALT0_IE7_Pos                 (31U)
#define CRU_PADALT0_IE7_Msk                 (0x1UL << CRU_PADALT0_IE7_Pos)
#define CRU_PADALT0_IE7                     CRU_PADALT0_IE7_Msk

/******************  Bit definition for CRU_PADALT1 register  *****************/

/** @brief PADALT1[2:0]: PA[8] pin alternate function */
#define CRU_PADALT1_ALT8_Pos                (0U)
#define CRU_PADALT1_ALT8_Msk                (0x7UL << CRU_PADALT1_ALT8_Pos)
#define CRU_PADALT1_ALT8                    CRU_PADALT1_ALT8_Msk
/** @brief PADALT1[3]: PA[8] pin input enable */
#define CRU_PADALT1_IE8_Pos                 (3U)
#define CRU_PADALT1_IE8_Msk                 (0x1UL << CRU_PADALT1_IE8_Pos)
#define CRU_PADALT1_IE8                     CRU_PADALT1_IE8_Msk
/** @brief PADALT1[6:4]: PA[9] pin alternate function */
#define CRU_PADALT1_ALT9_Pos                (4U)
#define CRU_PADALT1_ALT9_Msk                (0x7UL << CRU_PADALT1_ALT9_Pos)
#define CRU_PADALT1_ALT9                    CRU_PADALT1_ALT9_Msk
/** @brief PADALT1[7]: PA[9] pin input enable */
#define CRU_PADALT1_IE9_Pos                 (7U)
#define CRU_PADALT1_IE9_Msk                 (0x1UL << CRU_PADALT1_IE9_Pos)
#define CRU_PADALT1_IE9                     CRU_PADALT1_IE9_Msk
/** @brief PADALT1[10:8]: PA[10] pin alternate function */
#define CRU_PADALT1_ALT10_Pos               (8U)
#define CRU_PADALT1_ALT10_Msk               (0x7UL << CRU_PADALT1_ALT10_Pos)
#define CRU_PADALT1_ALT10                   CRU_PADALT1_ALT10_Msk
/** @brief PADALT1[11]: PA[10] pin input enable */
#define CRU_PADALT1_IE10_Pos                (11U)
#define CRU_PADALT1_IE10_Msk                (0x1UL << CRU_PADALT1_IE10_Pos)
#define CRU_PADALT1_IE10                    CRU_PADALT1_IE10_Msk
/** @brief PADALT1[14:12]: PA[11] pin alternate function */
#define CRU_PADALT1_ALT11_Pos               (12U)
#define CRU_PADALT1_ALT11_Msk               (0x7UL << CRU_PADALT1_ALT11_Pos)
#define CRU_PADALT1_ALT11                   CRU_PADALT1_ALT11_Msk
/** @brief PADALT1[15]: PA[11] pin input enable */
#define CRU_PADALT1_IE11_Pos                (15U)
#define CRU_PADALT1_IE11_Msk                (0x1UL << CRU_PADALT1_IE11_Pos)
#define CRU_PADALT1_IE11                    CRU_PADALT1_IE11_Msk
/** @brief PADALT1[18:16]: PA[12] pin alternate function */
#define CRU_PADALT1_ALT12_Pos               (16U)
#define CRU_PADALT1_ALT12_Msk               (0x7UL << CRU_PADALT1_ALT12_Pos)
#define CRU_PADALT1_ALT12                   CRU_PADALT1_ALT12_Msk
/** @brief PADALT1[19]: PA[12] pin input enable */
#define CRU_PADALT1_IE12_Pos                (19U)
#define CRU_PADALT1_IE12_Msk                (0x1UL << CRU_PADALT1_IE12_Pos)
#define CRU_PADALT1_IE12                    CRU_PADALT1_IE12_Msk
/** @brief PADALT1[22:20]: PA[13] pin alternate function */
#define CRU_PADALT1_ALT13_Pos               (20U)
#define CRU_PADALT1_ALT13_Msk               (0x7UL << CRU_PADALT1_ALT13_Pos)
#define CRU_PADALT1_ALT13                   CRU_PADALT1_ALT13_Msk
/** @brief PADALT1[23]: PA[13] pin input enable */
#define CRU_PADALT1_IE13_Pos                (23U)
#define CRU_PADALT1_IE13_Msk                (0x1UL << CRU_PADALT1_IE13_Pos)
#define CRU_PADALT1_IE13                    CRU_PADALT1_IE13_Msk
/** @brief PADALT1[26:24]: PA[14] pin alternate function */
#define CRU_PADALT1_ALT14_Pos               (24U)
#define CRU_PADALT1_ALT14_Msk               (0x7UL << CRU_PADALT1_ALT14_Pos)
#define CRU_PADALT1_ALT14                   CRU_PADALT1_ALT14_Msk
/** @brief PADALT1[27]: PA[14] pin input enable */
#define CRU_PADALT1_IE14_Pos                (27U)
#define CRU_PADALT1_IE14_Msk                (0x1UL << CRU_PADALT1_IE14_Pos)
#define CRU_PADALT1_IE14                    CRU_PADALT1_IE14_Msk
/** @brief PADALT1[30:28]: PA[15] pin alternate function */
#define CRU_PADALT1_ALT15_Pos               (28U)
#define CRU_PADALT1_ALT15_Msk               (0x7UL << CRU_PADALT1_ALT15_Pos)
#define CRU_PADALT1_ALT15                   CRU_PADALT1_ALT15_Msk
/** @brief PADALT1[31]: PA[15] pin input enable */
#define CRU_PADALT1_IE15_Pos                (31U)
#define CRU_PADALT1_IE15_Msk                (0x1UL << CRU_PADALT1_IE15_Pos)
#define CRU_PADALT1_IE15                    CRU_PADALT1_IE15_Msk

/******************  Bit definition for CRU_PADALT2 register  *****************/

/** @brief PADALT2[2:0]: PB[0] pin alternate function */
#define CRU_PADALT2_ALT0_Pos                (0U)
#define CRU_PADALT2_ALT0_Msk                (0x7UL << CRU_PADALT2_ALT0_Pos)
#define CRU_PADALT2_ALT0                    CRU_PADALT2_ALT0_Msk
/** @brief PADALT2[3]: PB[0] pin input enable */
#define CRU_PADALT2_IE0_Pos                 (3U)
#define CRU_PADALT2_IE0_Msk                 (0x1UL << CRU_PADALT2_IE0_Pos)
#define CRU_PADALT2_IE0                     CRU_PADALT2_IE0_Msk
/** @brief PADALT2[6:4]: PB[1] pin alternate function */
#define CRU_PADALT2_ALT1_Pos                (4U)
#define CRU_PADALT2_ALT1_Msk                (0x7UL << CRU_PADALT2_ALT1_Pos)
#define CRU_PADALT2_ALT1                    CRU_PADALT2_ALT1_Msk
/** @brief PADALT2[7]: PB[1] pin input enable */
#define CRU_PADALT2_IE1_Pos                 (7U)
#define CRU_PADALT2_IE1_Msk                 (0x1UL << CRU_PADALT2_IE1_Pos)
#define CRU_PADALT2_IE1                     CRU_PADALT2_IE1_Msk
/** @brief PADALT2[10:8]: PB[2] pin alternate function */
#define CRU_PADALT2_ALT2_Pos                (8U)
#define CRU_PADALT2_ALT2_Msk                (0x7UL << CRU_PADALT2_ALT2_Pos)
#define CRU_PADALT2_ALT2                    CRU_PADALT2_ALT2_Msk
/** @brief PADALT2[11]: PB[2] pin input enable */
#define CRU_PADALT2_IE2_Pos                 (11U)
#define CRU_PADALT2_IE2_Msk                 (0x1UL << CRU_PADALT2_IE2_Pos)
#define CRU_PADALT2_IE2                     CRU_PADALT2_IE2_Msk
/** @brief PADALT2[14:12]: PB[3] pin alternate function */
#define CRU_PADALT2_ALT3_Pos                (12U)
#define CRU_PADALT2_ALT3_Msk                (0x7UL << CRU_PADALT2_ALT3_Pos)
#define CRU_PADALT2_ALT3                    CRU_PADALT2_ALT3_Msk
/** @brief PADALT2[15]: PB[3] pin input enable */
#define CRU_PADALT2_IE3_Pos                 (15U)
#define CRU_PADALT2_IE3_Msk                 (0x1UL << CRU_PADALT2_IE3_Pos)
#define CRU_PADALT2_IE3                     CRU_PADALT2_IE3_Msk
/** @brief PADALT2[18:16]: PB[4] pin alternate function */
#define CRU_PADALT2_ALT4_Pos                (16U)
#define CRU_PADALT2_ALT4_Msk                (0x7UL << CRU_PADALT2_ALT4_Pos)
#define CRU_PADALT2_ALT4                    CRU_PADALT2_ALT4_Msk
/** @brief PADALT2[19]: PB[4] pin input enable */
#define CRU_PADALT2_IE4_Pos                 (19U)
#define CRU_PADALT2_IE4_Msk                 (0x1UL << CRU_PADALT2_IE4_Pos)
#define CRU_PADALT2_IE4                     CRU_PADALT2_IE4_Msk
/** @brief PADALT2[22:20]: PB[5] pin alternate function */
#define CRU_PADALT2_ALT5_Pos                (20U)
#define CRU_PADALT2_ALT5_Msk                (0x7UL << CRU_PADALT2_ALT5_Pos)
#define CRU_PADALT2_ALT5                    CRU_PADALT2_ALT5_Msk
/** @brief PADALT2[23]: PB[5] pin input enable */
#define CRU_PADALT2_IE5_Pos                 (23U)
#define CRU_PADALT2_IE5_Msk                 (0x1UL << CRU_PADALT2_IE5_Pos)
#define CRU_PADALT2_IE5                     CRU_PADALT2_IE5_Msk
/** @brief PADALT2[26:24]: PB[6] pin alternate function */
#define CRU_PADALT2_ALT6_Pos                (24U)
#define CRU_PADALT2_ALT6_Msk                (0x7UL << CRU_PADALT2_ALT6_Pos)
#define CRU_PADALT2_ALT6                    CRU_PADALT2_ALT6_Msk
/** @brief PADALT2[27]: PB[6] pin input enable */
#define CRU_PADALT2_IE6_Pos                 (27U)
#define CRU_PADALT2_IE6_Msk                 (0x1UL << CRU_PADALT2_IE6_Pos)
#define CRU_PADALT2_IE6                     CRU_PADALT2_IE6_Msk
/** @brief PADALT2[30:28]: PB[7] pin alternate function */
#define CRU_PADALT2_ALT7_Pos                (28U)
#define CRU_PADALT2_ALT7_Msk                (0x7UL << CRU_PADALT2_ALT7_Pos)
#define CRU_PADALT2_ALT7                    CRU_PADALT2_ALT7_Msk
/** @brief PADALT2[31]: PB[7] pin input enable */
#define CRU_PADALT2_IE7_Pos                 (31U)
#define CRU_PADALT2_IE7_Msk                 (0x1UL << CRU_PADALT2_IE7_Pos)
#define CRU_PADALT2_IE7                     CRU_PADALT2_IE7_Msk

/******************  Bit definition for CRU_PADALT3 register  *****************/

/** @brief PADALT3[2:0]: PB[8] pin alternate function */
#define CRU_PADALT3_ALT8_Pos                (0U)
#define CRU_PADALT3_ALT8_Msk                (0x7UL << CRU_PADALT3_ALT8_Pos)
#define CRU_PADALT3_ALT8                    CRU_PADALT3_ALT8_Msk
/** @brief PADALT3[3]: PB[8] pin input enable */
#define CRU_PADALT3_IE8_Pos                 (3U)
#define CRU_PADALT3_IE8_Msk                 (0x1UL << CRU_PADALT3_IE8_Pos)
#define CRU_PADALT3_IE8                     CRU_PADALT3_IE8_Msk
/** @brief PADALT3[6:4]: PB[9] pin alternate function */
#define CRU_PADALT3_ALT9_Pos                (4U)
#define CRU_PADALT3_ALT9_Msk                (0x7UL << CRU_PADALT3_ALT9_Pos)
#define CRU_PADALT3_ALT9                    CRU_PADALT3_ALT9_Msk
/** @brief PADALT3[7]: PB[9] pin input enable */
#define CRU_PADALT3_IE9_Pos                 (7U)
#define CRU_PADALT3_IE9_Msk                 (0x1UL << CRU_PADALT3_IE9_Pos)
#define CRU_PADALT3_IE9                     CRU_PADALT3_IE9_Msk
/** @brief PADALT3[10:8]: PB[10] pin alternate function */
#define CRU_PADALT3_ALT10_Pos               (8U)
#define CRU_PADALT3_ALT10_Msk               (0x7UL << CRU_PADALT3_ALT10_Pos)
#define CRU_PADALT3_ALT10                   CRU_PADALT3_ALT10_Msk
/** @brief PADALT3[11]: PB[10] pin input enable */
#define CRU_PADALT3_IE10_Pos                (11U)
#define CRU_PADALT3_IE10_Msk                (0x1UL << CRU_PADALT3_IE10_Pos)
#define CRU_PADALT3_IE10                    CRU_PADALT3_IE10_Msk
/** @brief PADALT3[14:12]: PB[11] pin alternate function */
#define CRU_PADALT3_ALT11_Pos               (12U)
#define CRU_PADALT3_ALT11_Msk               (0x7UL << CRU_PADALT3_ALT11_Pos)
#define CRU_PADALT3_ALT11                   CRU_PADALT3_ALT11_Msk
/** @brief PADALT3[15]: PB[11] pin input enable */
#define CRU_PADALT3_IE11_Pos                (15U)
#define CRU_PADALT3_IE11_Msk                (0x1UL << CRU_PADALT3_IE11_Pos)
#define CRU_PADALT3_IE11                    CRU_PADALT3_IE11_Msk
/** @brief PADALT3[18:16]: PB[12] pin alternate function */
#define CRU_PADALT3_ALT12_Pos               (16U)
#define CRU_PADALT3_ALT12_Msk               (0x7UL << CRU_PADALT3_ALT12_Pos)
#define CRU_PADALT3_ALT12                   CRU_PADALT3_ALT12_Msk
/** @brief PADALT3[19]: PB[12] pin input enable */
#define CRU_PADALT3_IE12_Pos                (19U)
#define CRU_PADALT3_IE12_Msk                (0x1UL << CRU_PADALT3_IE12_Pos)
#define CRU_PADALT3_IE12                    CRU_PADALT3_IE12_Msk
/** @brief PADALT3[22:20]: PB[13] pin alternate function */
#define CRU_PADALT3_ALT13_Pos               (20U)
#define CRU_PADALT3_ALT13_Msk               (0x7UL << CRU_PADALT3_ALT13_Pos)
#define CRU_PADALT3_ALT13                   CRU_PADALT3_ALT13_Msk
/** @brief PADALT3[23]: PB[13] pin input enable */
#define CRU_PADALT3_IE13_Pos                (23U)
#define CRU_PADALT3_IE13_Msk                (0x1UL << CRU_PADALT3_IE13_Pos)
#define CRU_PADALT3_IE13                    CRU_PADALT3_IE13_Msk
/** @brief PADALT3[26:24]: PB[14] pin alternate function */
#define CRU_PADALT3_ALT14_Pos               (24U)
#define CRU_PADALT3_ALT14_Msk               (0x7UL << CRU_PADALT3_ALT14_Pos)
#define CRU_PADALT3_ALT14                   CRU_PADALT3_ALT14_Msk
/** @brief PADALT3[27]: PB[14] pin input enable */
#define CRU_PADALT3_IE14_Pos                (27U)
#define CRU_PADALT3_IE14_Msk                (0x1UL << CRU_PADALT3_IE14_Pos)
#define CRU_PADALT3_IE14                    CRU_PADALT3_IE14_Msk
/** @brief PADALT3[30:28]: PB[15] pin alternate function */
#define CRU_PADALT3_ALT15_Pos               (28U)
#define CRU_PADALT3_ALT15_Msk               (0x7UL << CRU_PADALT3_ALT15_Pos)
#define CRU_PADALT3_ALT15                   CRU_PADALT3_ALT15_Msk
/** @brief PADALT3[31]: PB[15] pin input enable */
#define CRU_PADALT3_IE15_Pos                (31U)
#define CRU_PADALT3_IE15_Msk                (0x1UL << CRU_PADALT3_IE15_Pos)
#define CRU_PADALT3_IE15                    CRU_PADALT3_IE15_Msk

/******************  Bit definition for CRU_PADALT4 register  *****************/

/** @brief PADALT4[2:0]: PC[0] pin alternate function */
#define CRU_PADALT4_ALT0_Pos                (0U)
#define CRU_PADALT4_ALT0_Msk                (0x7UL << CRU_PADALT4_ALT0_Pos)
#define CRU_PADALT4_ALT0                    CRU_PADALT4_ALT0_Msk
/** @brief PADALT4[3]: PC[0] pin input enable */
#define CRU_PADALT4_IE0_Pos                 (3U)
#define CRU_PADALT4_IE0_Msk                 (0x1UL << CRU_PADALT4_IE0_Pos)
#define CRU_PADALT4_IE0                     CRU_PADALT4_IE0_Msk
/** @brief PADALT4[6:4]: PC[1] pin alternate function */
#define CRU_PADALT4_ALT1_Pos                (4U)
#define CRU_PADALT4_ALT1_Msk                (0x7UL << CRU_PADALT4_ALT1_Pos)
#define CRU_PADALT4_ALT1                    CRU_PADALT4_ALT1_Msk
/** @brief PADALT4[7]: PC[1] pin input enable */
#define CRU_PADALT4_IE1_Pos                 (7U)
#define CRU_PADALT4_IE1_Msk                 (0x1UL << CRU_PADALT4_IE1_Pos)
#define CRU_PADALT4_IE1                     CRU_PADALT4_IE1_Msk
/** @brief PADALT4[10:8]: PC[2] pin alternate function */
#define CRU_PADALT4_ALT2_Pos                (8U)
#define CRU_PADALT4_ALT2_Msk                (0x7UL << CRU_PADALT4_ALT2_Pos)
#define CRU_PADALT4_ALT2                    CRU_PADALT4_ALT2_Msk
/** @brief PADALT4[11]: PC[2] pin input enable */
#define CRU_PADALT4_IE2_Pos                 (11U)
#define CRU_PADALT4_IE2_Msk                 (0x1UL << CRU_PADALT4_IE2_Pos)
#define CRU_PADALT4_IE2                     CRU_PADALT4_IE2_Msk
/** @brief PADALT4[14:12]: PC[3] pin alternate function */
#define CRU_PADALT4_ALT3_Pos                (12U)
#define CRU_PADALT4_ALT3_Msk                (0x7UL << CRU_PADALT4_ALT3_Pos)
#define CRU_PADALT4_ALT3                    CRU_PADALT4_ALT3_Msk
/** @brief PADALT4[15]: PC[3] pin input enable */
#define CRU_PADALT4_IE3_Pos                 (15U)
#define CRU_PADALT4_IE3_Msk                 (0x1UL << CRU_PADALT4_IE3_Pos)
#define CRU_PADALT4_IE3                     CRU_PADALT4_IE3_Msk
/** @brief PADALT4[18:16]: PC[4] pin alternate function */
#define CRU_PADALT4_ALT4_Pos                (16U)
#define CRU_PADALT4_ALT4_Msk                (0x7UL << CRU_PADALT4_ALT4_Pos)
#define CRU_PADALT4_ALT4                    CRU_PADALT4_ALT4_Msk
/*!< PADALT4[19]: PC[4] pin input enable */
#define CRU_PADALT4_IE4_Pos                 (19U)
#define CRU_PADALT4_IE4_Msk                 (0x1UL << CRU_PADALT4_IE4_Pos)
#define CRU_PADALT4_IE4                     CRU_PADALT4_IE4_Msk
/** @brief PADALT4[22:20]: PC[5] pin alternate function */
#define CRU_PADALT4_ALT5_Pos                (20U)
#define CRU_PADALT4_ALT5_Msk                (0x7UL << CRU_PADALT4_ALT5_Pos)
#define CRU_PADALT4_ALT5                    CRU_PADALT4_ALT5_Msk
/** @brief PADALT4[23]: PC[5] pin input enable */
#define CRU_PADALT4_IE5_Pos                 (23U)
#define CRU_PADALT4_IE5_Msk                 (0x1UL << CRU_PADALT4_IE5_Pos)
#define CRU_PADALT4_IE5                     CRU_PADALT4_IE5_Msk
/** @brief PADALT4[26:24]: PC[6] pin alternate function */
#define CRU_PADALT4_ALT6_Pos                (24U)
#define CRU_PADALT4_ALT6_Msk                (0x7UL << CRU_PADALT4_ALT6_Pos)
#define CRU_PADALT4_ALT6                    CRU_PADALT4_ALT6_Msk
/** @brief PADALT4[27]: PC[6] pin input enable */
#define CRU_PADALT4_IE6_Pos                 (27U)
#define CRU_PADALT4_IE6_Msk                 (0x1UL << CRU_PADALT4_IE6_Pos)
#define CRU_PADALT4_IE6                     CRU_PADALT4_IE6_Msk
/** @brief PADALT4[30:28]: PC[7] pin alternate function */
#define CRU_PADALT4_ALT7_Pos                (28U)
#define CRU_PADALT4_ALT7_Msk                (0x7UL << CRU_PADALT4_ALT7_Pos)
#define CRU_PADALT4_ALT7                    CRU_PADALT4_ALT7_Msk
/** @brief PADALT4[31]: PC[7] pin input enable */
#define CRU_PADALT4_IE7_Pos                 (31U)
#define CRU_PADALT4_IE7_Msk                 (0x1UL << CRU_PADALT4_IE7_Pos)
#define CRU_PADALT4_IE7                     CRU_PADALT4_IE7_Msk

/******************  Bit definition for CRU_PADALT5 register  *****************/

/** @brief PADALT5[2:0]: PC[8] pin alternate function */
#define CRU_PADALT5_ALT8_Pos                (0U)
#define CRU_PADALT5_ALT8_Msk                (0x7UL << CRU_PADALT5_ALT8_Pos)
#define CRU_PADALT5_ALT8                    CRU_PADALT5_ALT8_Msk
/** @brief PADALT5[3]: PC[8] pin input enable */
#define CRU_PADALT5_IE8_Pos                 (3U)
#define CRU_PADALT5_IE8_Msk                 (0x1UL << CRU_PADALT5_IE8_Pos)
#define CRU_PADALT5_IE8                     CRU_PADALT5_IE8_Msk
/** @brief PADALT5[6:4]: PC[9] pin alternate function */
#define CRU_PADALT5_ALT9_Pos                (4U)
#define CRU_PADALT5_ALT9_Msk                (0x7UL << CRU_PADALT5_ALT9_Pos)
#define CRU_PADALT5_ALT9                    CRU_PADALT5_ALT9_Msk
/** @brief PADALT5[7]: PC[9] pin input enable */
#define CRU_PADALT5_IE9_Pos                 (7U)
#define CRU_PADALT5_IE9_Msk                 (0x1UL << CRU_PADALT5_IE9_Pos)
#define CRU_PADALT5_IE9                     CRU_PADALT5_IE9_Msk
/** @brief PADALT5[10:8]: PC[10] pin alternate function */
#define CRU_PADALT5_ALT10_Pos               (8U)
#define CRU_PADALT5_ALT10_Msk               (0x7UL << CRU_PADALT5_ALT10_Pos)
#define CRU_PADALT5_ALT10                   CRU_PADALT5_ALT10_Msk
/** @brief PADALT5[11]: PC[10] pin input enable */
#define CRU_PADALT5_IE10_Pos                (11U)
#define CRU_PADALT5_IE10_Msk                (0x1UL << CRU_PADALT5_IE10_Pos)
#define CRU_PADALT5_IE10                    CRU_PADALT5_IE10_Msk
/** @brief PADALT5[14:12]: PC[11] pin alternate function */
#define CRU_PADALT5_ALT11_Pos               (12U)
#define CRU_PADALT5_ALT11_Msk               (0x7UL << CRU_PADALT5_ALT11_Pos)
#define CRU_PADALT5_ALT11                   CRU_PADALT5_ALT11_Msk
/** @brief PADALT5[15]: PC[11] pin input enable */
#define CRU_PADALT5_IE11_Pos                (15U)
#define CRU_PADALT5_IE11_Msk                (0x1UL << CRU_PADALT5_IE11_Pos)
#define CRU_PADALT5_IE11                    CRU_PADALT5_IE11_Msk
/** @brief PADALT5[18:16]: PC[12] pin alternate function */
#define CRU_PADALT5_ALT12_Pos               (16U)
#define CRU_PADALT5_ALT12_Msk               (0x7UL << CRU_PADALT5_ALT12_Pos)
#define CRU_PADALT5_ALT12                   CRU_PADALT5_ALT12_Msk
/** @brief PADALT5[19]: PC[12] pin input enable */
#define CRU_PADALT5_IE12_Pos                (19U)
#define CRU_PADALT5_IE12_Msk                (0x1UL << CRU_PADALT5_IE12_Pos)
#define CRU_PADALT5_IE12                    CRU_PADALT5_IE12_Msk
/** @brief PADALT5[22:20]: PC[13] pin alternate function */
#define CRU_PADALT5_ALT13_Pos               (20U)
#define CRU_PADALT5_ALT13_Msk               (0x7UL << CRU_PADALT5_ALT13_Pos)
#define CRU_PADALT5_ALT13                   CRU_PADALT5_ALT13_Msk
/** @brief PADALT5[23]: PC[13] pin input enable */
#define CRU_PADALT5_IE13_Pos                (23U)
#define CRU_PADALT5_IE13_Msk                (0x1UL << CRU_PADALT5_IE13_Pos)
#define CRU_PADALT5_IE13                    CRU_PADALT5_IE13_Msk
/** @brief PADALT5[26:24]: PC[14] pin alternate function */
#define CRU_PADALT5_ALT14_Pos               (24U)
#define CRU_PADALT5_ALT14_Msk               (0x7UL << CRU_PADALT5_ALT14_Pos)
#define CRU_PADALT5_ALT14                   CRU_PADALT5_ALT14_Msk
/** @brief PADALT5[27]: PC[14] pin input enable */
#define CRU_PADALT5_IE14_Pos                (27U)
#define CRU_PADALT5_IE14_Msk                (0x1UL << CRU_PADALT5_IE14_Pos)
#define CRU_PADALT5_IE14                    CRU_PADALT5_IE14_Msk
/** @brief PADALT5[30:28]: PC[15] pin alternate function */
#define CRU_PADALT5_ALT15_Pos               (28U)
#define CRU_PADALT5_ALT15_Msk               (0x7UL << CRU_PADALT5_ALT15_Pos)
#define CRU_PADALT5_ALT15                   CRU_PADALT5_ALT15_Msk
/** @brief PADALT5[31]: PC[15] pin input enable */
#define CRU_PADALT5_IE15_Pos                (31U)
#define CRU_PADALT5_IE15_Msk                (0x1UL << CRU_PADALT5_IE15_Pos)
#define CRU_PADALT5_IE15                    CRU_PADALT5_IE15_Msk

/******************  Bit definition for CRU_PADDS0 register  ******************/

/** @brief PADDS0[1:0]: PA[0] output drive mode */
#define CRU_PADDS0_PADS0_Pos                (0U)
#define CRU_PADDS0_PADS0_Msk                (0x3UL << CRU_PADDS0_PADS0_Pos)
#define CRU_PADDS0_PADS0                    CRU_PADDS0_PADS0_Msk
/** @brief PADDS0[3:2]: PA[1] output drive mode */
#define CRU_PADDS0_PADS1_Pos                (2U)
#define CRU_PADDS0_PADS1_Msk                (0x3UL << CRU_PADDS0_PADS1_Pos)
#define CRU_PADDS0_PADS1                    CRU_PADDS0_PADS1_Msk
/** @brief PADDS0[5:4]: PA[2] output drive mode */
#define CRU_PADDS0_PADS2_Pos                (4U)
#define CRU_PADDS0_PADS2_Msk                (0x3UL << CRU_PADDS0_PADS2_Pos)
#define CRU_PADDS0_PADS2                    CRU_PADDS0_PADS2_Msk
/** @brief PADDS0[7:6]: PA[3] output drive mode */
#define CRU_PADDS0_PADS3_Pos                (6U)
#define CRU_PADDS0_PADS3_Msk                (0x3UL << CRU_PADDS0_PADS3_Pos)
#define CRU_PADDS0_PADS3                    CRU_PADDS0_PADS3_Msk
/** @brief PADDS0[9:8]: PA[4] output drive mode */
#define CRU_PADDS0_PADS4_Pos                (8U)
#define CRU_PADDS0_PADS4_Msk                (0x3UL << CRU_PADDS0_PADS4_Pos)
#define CRU_PADDS0_PADS4                    CRU_PADDS0_PADS4_Msk
/** @brief PADDS0[11:10]: PA[5] output drive mode */
#define CRU_PADDS0_PADS5_Pos                (10U)
#define CRU_PADDS0_PADS5_Msk                (0x3UL << CRU_PADDS0_PADS5_Pos)
#define CRU_PADDS0_PADS5                    CRU_PADDS0_PADS5_Msk
/** @brief PADDS0[13:12]: PA[6] output drive mode */
#define CRU_PADDS0_PADS6_Pos                (12U)
#define CRU_PADDS0_PADS6_Msk                (0x3UL << CRU_PADDS0_PADS6_Pos)
#define CRU_PADDS0_PADS6                    CRU_PADDS0_PADS6_Msk
/** @brief PADDS0[15:14]: PA[7] output drive mode */
#define CRU_PADDS0_PADS7_Pos                (14U)
#define CRU_PADDS0_PADS7_Msk                (0x3UL << CRU_PADDS0_PADS7_Pos)
#define CRU_PADDS0_PADS7                    CRU_PADDS0_PADS7_Msk
/** @brief PADDS0[17:16]: PA[8] output drive mode */
#define CRU_PADDS0_PADS8_Pos                (16U)
#define CRU_PADDS0_PADS8_Msk                (0x3UL << CRU_PADDS0_PADS8_Pos)
#define CRU_PADDS0_PADS8                    CRU_PADDS0_PADS8_Msk
/** @brief PADDS0[19:18]: PA[9] output drive mode */
#define CRU_PADDS0_PADS9_Pos                (18U)
#define CRU_PADDS0_PADS9_Msk                (0x3UL << CRU_PADDS0_PADS9_Pos)
#define CRU_PADDS0_PADS9                    CRU_PADDS0_PADS9_Msk
/** @brief PADDS0[21:20]: PA[10] output drive mode */
#define CRU_PADDS0_PADS10_Pos               (20U)
#define CRU_PADDS0_PADS10_Msk               (0x3UL << CRU_PADDS0_PADS10_Pos)
#define CRU_PADDS0_PADS10                   CRU_PADDS0_PADS10_Msk
/** @brief PADDS0[23:22]: PA[11] output drive mode */
#define CRU_PADDS0_PADS11_Pos               (22U)
#define CRU_PADDS0_PADS11_Msk               (0x3UL << CRU_PADDS0_PADS11_Pos)
#define CRU_PADDS0_PADS11                   CRU_PADDS0_PADS11_Msk
/** @brief PADDS0[25:24]: PA[12] output drive mode */
#define CRU_PADDS0_PADS12_Pos               (24U)
#define CRU_PADDS0_PADS12_Msk               (0x3UL << CRU_PADDS0_PADS12_Pos)
#define CRU_PADDS0_PADS12                   CRU_PADDS0_PADS12_Msk
/** @brief PADDS0[27:26]: PA[13] output drive mode */
#define CRU_PADDS0_PADS13_Pos               (26U)
#define CRU_PADDS0_PADS13_Msk               (0x3UL << CRU_PADDS0_PADS13_Pos)
#define CRU_PADDS0_PADS13                   CRU_PADDS0_PADS13_Msk
/** @brief PADDS0[29:28]: PA[14] output drive mode */
#define CRU_PADDS0_PADS14_Pos               (28U)
#define CRU_PADDS0_PADS14_Msk               (0x3UL << CRU_PADDS0_PADS14_Pos)
#define CRU_PADDS0_PADS14                   CRU_PADDS0_PADS14_Msk
/** @brief PADDS0[31:30]: PA[15] output drive mode */
#define CRU_PADDS0_PADS15_Pos               (30U)
#define CRU_PADDS0_PADS15_Msk               (0x3UL << CRU_PADDS0_PADS15_Pos)
#define CRU_PADDS0_PADS15                   CRU_PADDS0_PADS15_Msk

/******************  Bit definition for CRU_PADDS1 register  ******************/

/** @brief PADDS1[1:0]: PB[0] output drive mode */
#define CRU_PADDS1_PBDS0_Pos                (0U)
#define CRU_PADDS1_PBDS0_Msk                (0x3UL << CRU_PADDS1_PBDS0_Pos)
#define CRU_PADDS1_PBDS0                    CRU_PADDS1_PBDS0_Msk
/** @brief PADDS1[3:2]: PB[1] output drive mode */
#define CRU_PADDS1_PBDS1_Pos                (2U)
#define CRU_PADDS1_PBDS1_Msk                (0x3UL << CRU_PADDS1_PBDS1_Pos)
#define CRU_PADDS1_PBDS1                    CRU_PADDS1_PBDS1_Msk
/** @brief PADDS1[5:4]: PB[2] output drive mode */
#define CRU_PADDS1_PBDS2_Pos                (4U)
#define CRU_PADDS1_PBDS2_Msk                (0x3UL << CRU_PADDS1_PBDS2_Pos)
#define CRU_PADDS1_PBDS2                    CRU_PADDS1_PBDS2_Msk
/** @brief PADDS1[7:6]: PB[3] output drive mode */
#define CRU_PADDS1_PBDS3_Pos                (6U)
#define CRU_PADDS1_PBDS3_Msk                (0x3UL << CRU_PADDS1_PBDS3_Pos)
#define CRU_PADDS1_PBDS3                    CRU_PADDS1_PBDS3_Msk
/** @brief PADDS1[9:8]: PB[4] output drive mode */
#define CRU_PADDS1_PBDS4_Pos                (8U)
#define CRU_PADDS1_PBDS4_Msk                (0x3UL << CRU_PADDS1_PBDS4_Pos)
#define CRU_PADDS1_PBDS4                    CRU_PADDS1_PBDS4_Msk
/** @brief PADDS1[11:10]: PB[5] output drive mode */
#define CRU_PADDS1_PBDS5_Pos                (10U)
#define CRU_PADDS1_PBDS5_Msk                (0x3UL << CRU_PADDS1_PBDS5_Pos)
#define CRU_PADDS1_PBDS5                    CRU_PADDS1_PBDS5_Msk
/** @brief PADDS1[13:12]: PB[6] output drive mode */
#define CRU_PADDS1_PBDS6_Pos                (12U)
#define CRU_PADDS1_PBDS6_Msk                (0x3UL << CRU_PADDS1_PBDS6_Pos)
#define CRU_PADDS1_PBDS6                    CRU_PADDS1_PBDS6_Msk
/** @brief PADDS1[15:14]: PB[7] output drive mode */
#define CRU_PADDS1_PBDS7_Pos                (14U)
#define CRU_PADDS1_PBDS7_Msk                (0x3UL << CRU_PADDS1_PBDS7_Pos)
#define CRU_PADDS1_PBDS7                    CRU_PADDS1_PBDS7_Msk
/** @brief PADDS1[17:16]: PB[8] output drive mode */
#define CRU_PADDS1_PBDS8_Pos                (16U)
#define CRU_PADDS1_PBDS8_Msk                (0x3UL << CRU_PADDS1_PBDS8_Pos)
#define CRU_PADDS1_PBDS8                    CRU_PADDS1_PBDS8_Msk
/** @brief PADDS1[19:18]: PB[9] output drive mode*/
#define CRU_PADDS1_PBDS9_Pos                (18U)
#define CRU_PADDS1_PBDS9_Msk                (0x3UL << CRU_PADDS1_PBDS9_Pos)
#define CRU_PADDS1_PBDS9                    CRU_PADDS1_PBDS9_Msk
/** @brief PADDS1[21:20]: PB[10] output drive mode */
#define CRU_PADDS1_PBDS10_Pos               (20U)
#define CRU_PADDS1_PBDS10_Msk               (0x3UL << CRU_PADDS1_PBDS10_Pos)
#define CRU_PADDS1_PBDS10                   CRU_PADDS1_PBDS10_Msk
/** @brief PADDS1[23:22]: PB[11] output drive mode */
#define CRU_PADDS1_PBDS11_Pos               (22U)
#define CRU_PADDS1_PBDS11_Msk               (0x3UL << CRU_PADDS1_PBDS11_Pos)
#define CRU_PADDS1_PBDS11                   CRU_PADDS1_PBDS11_Msk
/** @brief PADDS1[25:24]: PB[12] output drive mode */
#define CRU_PADDS1_PBDS12_Pos               (24U)
#define CRU_PADDS1_PBDS12_Msk               (0x3UL << CRU_PADDS1_PBDS12_Pos)
#define CRU_PADDS1_PBDS12                   CRU_PADDS1_PBDS12_Msk
/** @brief PADDS1[27:26]: PB[13] output drive mode */
#define CRU_PADDS1_PBDS13_Pos               (26U)
#define CRU_PADDS1_PBDS13_Msk               (0x3UL << CRU_PADDS1_PBDS13_Pos)
#define CRU_PADDS1_PBDS13                   CRU_PADDS1_PBDS13_Msk
/** @brief PADDS1[29:28]: PB[14] output drive mode */
#define CRU_PADDS1_PBDS14_Pos               (28U)
#define CRU_PADDS1_PBDS14_Msk               (0x3UL << CRU_PADDS1_PBDS14_Pos)
#define CRU_PADDS1_PBDS14                   CRU_PADDS1_PBDS14_Msk
/** @brief PADDS1[31:30]: PB[15] output drive mode */
#define CRU_PADDS1_PBDS15_Pos               (30U)
#define CRU_PADDS1_PBDS15_Msk               (0x3UL << CRU_PADDS1_PBDS15_Pos)
#define CRU_PADDS1_PBDS15                   CRU_PADDS1_PBDS15_Msk

/******************  Bit definition for CRU_PADDS2 register  ******************/

/** @brief PADDS2[1:0]: PC[0] output drive mode */
#define CRU_PADDS2_PBDS0_Pos                (0U)
#define CRU_PADDS2_PBDS0_Msk                (0x3UL << CRU_PADDS2_PBDS0_Pos)
#define CRU_PADDS2_PBDS0                    CRU_PADDS2_PBDS0_Msk
/** @brief PADDS2[3:2]: PC[1] output drive mode */
#define CRU_PADDS2_PBDS1_Pos                (2U)
#define CRU_PADDS2_PBDS1_Msk                (0x3UL << CRU_PADDS2_PBDS1_Pos)
#define CRU_PADDS2_PBDS1                    CRU_PADDS2_PBDS1_Msk
/** @brief PADDS2[5:4]: PC[2] output drive mode */
#define CRU_PADDS2_PBDS2_Pos                (4U)
#define CRU_PADDS2_PBDS2_Msk                (0x3UL << CRU_PADDS2_PBDS2_Pos)
#define CRU_PADDS2_PBDS2                    CRU_PADDS2_PBDS2_Msk
/** @brief PADDS2[7:6]: PC[3] output drive mode */
#define CRU_PADDS2_PBDS3_Pos                (6U)
#define CRU_PADDS2_PBDS3_Msk                (0x3UL << CRU_PADDS2_PBDS3_Pos)
#define CRU_PADDS2_PBDS3                    CRU_PADDS2_PBDS3_Msk
/** @brief PADDS2[9:8]: PC[4] output drive mode */
#define CRU_PADDS2_PBDS4_Pos                (8U)
#define CRU_PADDS2_PBDS4_Msk                (0x3UL << CRU_PADDS2_PBDS4_Pos)
#define CRU_PADDS2_PBDS4                    CRU_PADDS2_PBDS4_Msk
/** @brief PADDS2[11:10]: PC[5] output drive mode */
#define CRU_PADDS2_PBDS5_Pos                (10U)
#define CRU_PADDS2_PBDS5_Msk                (0x3UL << CRU_PADDS2_PBDS5_Pos)
#define CRU_PADDS2_PBDS5                    CRU_PADDS2_PBDS5_Msk
/** @brief PADDS2[13:12]: PC[6] output drive mode */
#define CRU_PADDS2_PBDS6_Pos                (12U)
#define CRU_PADDS2_PBDS6_Msk                (0x3UL << CRU_PADDS2_PBDS6_Pos)
#define CRU_PADDS2_PBDS6                    CRU_PADDS2_PBDS6_Msk
/** @brief PADDS2[15:14]: PC[7] output drive mode */
#define CRU_PADDS2_PBDS7_Pos                (14U)
#define CRU_PADDS2_PBDS7_Msk                (0x3UL << CRU_PADDS2_PBDS7_Pos)
#define CRU_PADDS2_PBDS7                    CRU_PADDS2_PBDS7_Msk
/** @brief PADDS2[17:16]: PC[8] output drive mode */
#define CRU_PADDS2_PBDS8_Pos                (16U)
#define CRU_PADDS2_PBDS8_Msk                (0x3UL << CRU_PADDS2_PBDS8_Pos)
#define CRU_PADDS2_PBDS8                    CRU_PADDS2_PBDS8_Msk
/** @brief PADDS2[19:18]: PC[9] output drive mode */
#define CRU_PADDS2_PBDS9_Pos                (18U)
#define CRU_PADDS2_PBDS9_Msk                (0x3UL << CRU_PADDS2_PBDS9_Pos)
#define CRU_PADDS2_PBDS9                    CRU_PADDS2_PBDS9_Msk
/** @brief PADDS2[21:20]: PC[10] output drive mode */
#define CRU_PADDS2_PBDS10_Pos               (20U)
#define CRU_PADDS2_PBDS10_Msk               (0x3UL << CRU_PADDS2_PBDS10_Pos)
#define CRU_PADDS2_PBDS10                   CRU_PADDS2_PBDS10_Msk
/** @brief PADDS2[23:22]: PC[11] output drive mode */
#define CRU_PADDS2_PBDS11_Pos               (22U)
#define CRU_PADDS2_PBDS11_Msk               (0x3UL << CRU_PADDS2_PBDS11_Pos)
#define CRU_PADDS2_PBDS11                   CRU_PADDS2_PBDS11_Msk
/** @brief PADDS2[25:24]: PC[12] output drive mode */
#define CRU_PADDS2_PBDS12_Pos               (24U)
#define CRU_PADDS2_PBDS12_Msk               (0x3UL << CRU_PADDS2_PBDS12_Pos)
#define CRU_PADDS2_PBDS12                   CRU_PADDS2_PBDS12_Msk
/** @brief PADDS2[27:26]: PC[13] output drive mode */
#define CRU_PADDS2_PBDS13_Pos               (26U)
#define CRU_PADDS2_PBDS13_Msk               (0x3UL << CRU_PADDS2_PBDS13_Pos)
#define CRU_PADDS2_PBDS13                   CRU_PADDS2_PBDS13_Msk
/** @brief PADDS2[29:28]: PC[14] output drive mode */
#define CRU_PADDS2_PBDS14_Pos               (28U)
#define CRU_PADDS2_PBDS14_Msk               (0x3UL << CRU_PADDS2_PBDS14_Pos)
#define CRU_PADDS2_PBDS14                   CRU_PADDS2_PBDS14_Msk
/** @brief PADDS2[31:30]: PC[15] output drive mode */
#define CRU_PADDS2_PBDS15_Pos               (30U)
#define CRU_PADDS2_PBDS15_Msk               (0x3UL << CRU_PADDS2_PBDS15_Pos)
#define CRU_PADDS2_PBDS15                   CRU_PADDS2_PBDS15_Msk

/******************  Bit definition for CRU_LDOCTRL register  *****************/

/** @brief LDOCTRL[2:0]: LDO CORE output voltage */
#define CRU_LDOCTRL_LDOC_ADJ_Pos            (0U)
#define CRU_LDOCTRL_LDOC_ADJ_Msk            (0x7UL << CRU_LDOCTRL_LDOC_ADJ_Pos)
#define CRU_LDOCTRL_LDOC_ADJ                CRU_LDOCTRL_LDOC_ADJ_Msk

#define CRU_LDOCTRL_LDOC_ADJ_LEV0           (0x0UL << CRU_LDOCTRL_LDOC_ADJ_Pos) /*!< LDO CORE output voltage 1.10V */
#define CRU_LDOCTRL_LDOC_ADJ_LEV1           (0x1UL << CRU_LDOCTRL_LDOC_ADJ_Pos) /*!< LDO CORE output voltage 1.15V */
#define CRU_LDOCTRL_LDOC_ADJ_LEV2           (0x2UL << CRU_LDOCTRL_LDOC_ADJ_Pos) /*!< LDO CORE output voltage 1.00V */
#define CRU_LDOCTRL_LDOC_ADJ_LEV3           (0x3UL << CRU_LDOCTRL_LDOC_ADJ_Pos) /*!< LDO CORE output voltage 1.05V */
#define CRU_LDOCTRL_LDOC_ADJ_LEV4           (0x4UL << CRU_LDOCTRL_LDOC_ADJ_Pos) /*!< LDO CORE output voltage 1.30V */
#define CRU_LDOCTRL_LDOC_ADJ_LEV5           (0x5UL << CRU_LDOCTRL_LDOC_ADJ_Pos) /*!< LDO CORE output voltage 1.35V */
#define CRU_LDOCTRL_LDOC_ADJ_LEV6           (0x6UL << CRU_LDOCTRL_LDOC_ADJ_Pos) /*!< LDO CORE output voltage 1.20V */
#define CRU_LDOCTRL_LDOC_ADJ_LEV7           (0x7UL << CRU_LDOCTRL_LDOC_ADJ_Pos) /*!< LDO CORE output voltage 1.25V */

/** @brief LDOCTRL[5:3]: LDO CORE protection current control */
#define CRU_LDOCTRL_LDOC_LDO_PI_Pos         (3U)
#define CRU_LDOCTRL_LDOC_LDO_PI_Msk         (0x7UL << CRU_LDOCTRL_LDOC_LDO_PI_Pos)
#define CRU_LDOCTRL_LDOC_LDO_PI             CRU_LDOCTRL_LDOC_LDO_PI_Msk

#define CRU_LDOCTRL_LDOC_LDO_PI_LEV0        (0x0UL << CRU_LDOCTRL_LDOC_LDO_PI_Pos)  /*!< LDO CORE protection current 500mA */
#define CRU_LDOCTRL_LDOC_LDO_PI_LEV1        (0x1UL << CRU_LDOCTRL_LDOC_LDO_PI_Pos)  /*!< LDO CORE protection current 460mA */
#define CRU_LDOCTRL_LDOC_LDO_PI_LEV2        (0x2UL << CRU_LDOCTRL_LDOC_LDO_PI_Pos)  /*!< LDO CORE protection current 420mA */
#define CRU_LDOCTRL_LDOC_LDO_PI_LEV3        (0x3UL << CRU_LDOCTRL_LDOC_LDO_PI_Pos)  /*!< LDO CORE protection current 380mA */
#define CRU_LDOCTRL_LDOC_LDO_PI_LEV4        (0x4UL << CRU_LDOCTRL_LDOC_LDO_PI_Pos)  /*!< LDO CORE protection current 660mA */
#define CRU_LDOCTRL_LDOC_LDO_PI_LEV5        (0x5UL << CRU_LDOCTRL_LDOC_LDO_PI_Pos)  /*!< LDO CORE protection current 620mA */
#define CRU_LDOCTRL_LDOC_LDO_PI_LEV6        (0x6UL << CRU_LDOCTRL_LDOC_LDO_PI_Pos)  /*!< LDO CORE protection current 580mA */

/** @brief LDOCTRL[10:8]: LDO EFLASH output voltage */
#define CRU_LDOCTRL_LDOF_ADJ_Pos            (8U)
#define CRU_LDOCTRL_LDOF_ADJ_Msk            (0x7UL << CRU_LDOCTRL_LDOF_ADJ_Pos)
#define CRU_LDOCTRL_LDOF_ADJ                CRU_LDOCTRL_LDOF_ADJ_Msk

#define CRU_LDOCTRL_LDOF_ADJ_LEV0           (0x0UL << CRU_LDOCTRL_LDOF_ADJ_Pos) /*!< LDO EFLASH output voltage 1.70V */
#define CRU_LDOCTRL_LDOF_ADJ_LEV1           (0x1UL << CRU_LDOCTRL_LDOF_ADJ_Pos) /*!< LDO EFLASH output voltage 1.75V */
#define CRU_LDOCTRL_LDOF_ADJ_LEV2           (0x2UL << CRU_LDOCTRL_LDOF_ADJ_Pos) /*!< LDO EFLASH output voltage 1.60V */
#define CRU_LDOCTRL_LDOF_ADJ_LEV3           (0x3UL << CRU_LDOCTRL_LDOF_ADJ_Pos) /*!< LDO EFLASH output voltage 1.65V */
#define CRU_LDOCTRL_LDOF_ADJ_LEV4           (0x4UL << CRU_LDOCTRL_LDOF_ADJ_Pos) /*!< LDO EFLASH output voltage 1.90V */
#define CRU_LDOCTRL_LDOF_ADJ_LEV5           (0x5UL << CRU_LDOCTRL_LDOF_ADJ_Pos) /*!< LDO EFLASH output voltage 1.95V */
#define CRU_LDOCTRL_LDOF_ADJ_LEV6           (0x6UL << CRU_LDOCTRL_LDOF_ADJ_Pos) /*!< LDO EFLASH output voltage 0.80V */
#define CRU_LDOCTRL_LDOF_ADJ_LEV7           (0x7UL << CRU_LDOCTRL_LDOF_ADJ_Pos) /*!< LDO EFLASH output voltage 0.85V */

/** @brief LDOCTRL[13:11]: LDO EFLASH protection current control */
#define CRU_LDOCTRL_LDOF_LDO_PI_Pos         (11U)
#define CRU_LDOCTRL_LDOF_LDO_PI_Msk         (0x7UL << CRU_LDOCTRL_LDOF_LDO_PI_Pos)
#define CRU_LDOCTRL_LDOF_LDO_PI             CRU_LDOCTRL_LDOF_LDO_PI_Msk

#define CRU_LDOCTRL_LDOF_LDO_PI_LEV0        (0x0UL << CRU_LDOCTRL_LDOF_LDO_PI_Pos)  /*!< LDO EFLASH protection current 290mA */
#define CRU_LDOCTRL_LDOF_LDO_PI_LEV1        (0x1UL << CRU_LDOCTRL_LDOF_LDO_PI_Pos)  /*!< LDO EFLASH protection current 260mA */
#define CRU_LDOCTRL_LDOF_LDO_PI_LEV2        (0x2UL << CRU_LDOCTRL_LDOF_LDO_PI_Pos)  /*!< LDO EFLASH protection current 225mA */
#define CRU_LDOCTRL_LDOF_LDO_PI_LEV3        (0x3UL << CRU_LDOCTRL_LDOF_LDO_PI_Pos)  /*!< LDO EFLASH protection current 195mA */
#define CRU_LDOCTRL_LDOF_LDO_PI_LEV4        (0x4UL << CRU_LDOCTRL_LDOF_LDO_PI_Pos)  /*!< LDO EFLASH protection current 410mA */
#define CRU_LDOCTRL_LDOF_LDO_PI_LEV5        (0x5UL << CRU_LDOCTRL_LDOF_LDO_PI_Pos)  /*!< LDO EFLASH protection current 380mA */
#define CRU_LDOCTRL_LDOF_LDO_PI_LEV6        (0x6UL << CRU_LDOCTRL_LDOF_LDO_PI_Pos)  /*!< LDO EFLASH protection current 350mA */
#define CRU_LDOCTRL_LDOF_LDO_PI_LEV7        (0x7UL << CRU_LDOCTRL_LDOF_LDO_PI_Pos)  /*!< LDO EFLASH protection current 320mA */

/*****************  Bit definition for CRU_SYSSTAT0 register  *****************/

/** @brief SYSSTAT0[2:0]: Bootstrap inputs state - Mode */
#define CRU_SYSSTAT0_STRAP_MODE_Pos         (0U)
#define CRU_SYSSTAT0_STRAP_MODE_Msk         (0x7UL << CRU_SYSSTAT0_STRAP_MODE_Pos)
#define CRU_SYSSTAT0_STRAP_MODE             CRU_SYSSTAT0_STRAP_MODE_Msk

#define CRU_SYSSTAT0_STRAP_MODE_EFLASH      (0x0UL << CRU_SYSSTAT0_STRAP_MODE_Pos)  /*!< EFLASH boot */
#define CRU_SYSSTAT0_STRAP_MODE_UART        (0x1UL << CRU_SYSSTAT0_STRAP_MODE_Pos)  /*!< UART0 boot */
#define CRU_SYSSTAT0_STRAP_MODE_QSPI        (0x2UL << CRU_SYSSTAT0_STRAP_MODE_Pos)  /*!< QSPI0 boot */
#define CRU_SYSSTAT0_STRAP_MODE_USB         (0x3UL << CRU_SYSSTAT0_STRAP_MODE_Pos)  /*!< USB boot */
#define CRU_SYSSTAT0_STRAP_MODE_JTAG        (0x4UL << CRU_SYSSTAT0_STRAP_MODE_Pos)  /*!< JTAG chain */
#define CRU_SYSSTAT0_STRAP_MODE_PYTHON      (0x5UL << CRU_SYSSTAT0_STRAP_MODE_Pos)  /*!< Python boot */
#define CRU_SYSSTAT0_STRAP_MODE_MBR         (0x6UL << CRU_SYSSTAT0_STRAP_MODE_Pos)  /*!< MBR boot */
#define CRU_SYSSTAT0_STRAP_MODE_JTAG_TEST   (0x7UL << CRU_SYSSTAT0_STRAP_MODE_Pos)  /*!< JTAG test */

/** @brief SYSSTAT0[3]: Bootstrap inputs state - Clock */
#define CRU_SYSSTAT0_STRAP_CLK_Pos          (3U)
#define CRU_SYSSTAT0_STRAP_CLK_Msk          (0x1UL << CRU_SYSSTAT0_STRAP_CLK_Pos)
#define CRU_SYSSTAT0_STRAP_CLK              CRU_SYSSTAT0_STRAP_CLK_Msk

#define CRU_SYSSTAT0_STRAP_CLK_CLKI         (0x0UL << CRU_SYSSTAT0_STRAP_CLK_Pos)   /*!< CLKI input */
#define CRU_SYSSTAT0_STRAP_CLK_RC           (0x1UL << CRU_SYSSTAT0_STRAP_CLK_Pos)   /*!< RC generator */

/** @brief SYSSTAT0[4]: Secure boot disable flag */
#define CRU_SYSSTAT0_SECUREDIS_Pos          (4U)
#define CRU_SYSSTAT0_SECUREDIS_Msk          (0x1UL << CRU_SYSSTAT0_SECUREDIS_Pos)
#define CRU_SYSSTAT0_SECUREDIS              CRU_SYSSTAT0_SECUREDIS_Msk
/** @brief SYSSTAT0[5]: EFLASH good flag */
#define CRU_SYSSTAT0_EFLASH_OK_Pos          (5U)
#define CRU_SYSSTAT0_EFLASH_OK_Msk          (0x1UL << CRU_SYSSTAT0_EFLASH_OK_Pos)
#define CRU_SYSSTAT0_EFLASH_OK              CRU_SYSSTAT0_EFLASH_OK_Msk

/** @brief SYSSTAT0[6]: MCU pin count */
#define CRU_SYSSTAT0_PACK_Pos               (6U)
#define CRU_SYSSTAT0_PACK_Msk               (0x1UL << CRU_SYSSTAT0_PACK_Pos)
#define CRU_SYSSTAT0_PACK                   CRU_SYSSTAT0_PACK_Msk

#define CRU_SYSSTAT0_PACK_32PIN             (0x0UL << CRU_SYSSTAT0_PACK_Pos)    /*!< 32-pin footprint */
#define CRU_SYSSTAT0_PACK_48PIN             (0x1UL << CRU_SYSSTAT0_PACK_Pos)    /*!< 48-pin footprint */

/** @brief SYSSTAT0[7]: Watchdog reset flag */
#define CRU_SYSSTAT0_WDTHIT_Pos             (7U)
#define CRU_SYSSTAT0_WDTHIT_Msk             (0x1UL << CRU_SYSSTAT0_WDTHIT_Pos)
#define CRU_SYSSTAT0_WDTHIT                 CRU_SYSSTAT0_WDTHIT_Msk

/*****************  Bit definition for CRU_WDTCLRKEY register  ****************/

/** @brief WDTCLRKEY[31:0]: WDTx reset enable key */
#define CRU_WDTCLRKEY_Pos                   (0U)
#define CRU_WDTCLRKEY_Msk                   (0xFFFFFFFFUL << CRU_WDTCLRKEY_Pos)
#define CRU_WDTCLRKEY                       CRU_WDTCLRKEY_Msk

/*****************  Bit definition for CRU_INTCTRL0 register  *****************/

/** @brief INTCTRL0[7:4]: Port 0 interrupt source */
#define CRU_INTCTRL0_PADAINTSEL_Pos         (4U)
#define CRU_INTCTRL0_PADAINTSEL_Msk         (0xFUL << CRU_INTCTRL0_PADAINTSEL_Pos)
#define CRU_INTCTRL0_PADAINTSEL             CRU_INTCTRL0_PADAINTSEL_Msk
/** @brief INTCTRL0[11:8]: Port 1 interrupt source */
#define CRU_INTCTRL0_PADBINTSEL_Pos         (8U)
#define CRU_INTCTRL0_PADBINTSEL_Msk         (0xFUL << CRU_INTCTRL0_PADBINTSEL_Pos)
#define CRU_INTCTRL0_PADBINTSEL             CRU_INTCTRL0_PADBINTSEL_Msk
/** @brief INTCTRL0[15:12]: Port 2 interrupt source */
#define CRU_INTCTRL0_PADCINTSEL_Pos         (12U)
#define CRU_INTCTRL0_PADCINTSEL_Msk         (0xFUL << CRU_INTCTRL0_PADCINTSEL_Pos)
#define CRU_INTCTRL0_PADCINTSEL             CRU_INTCTRL0_PADCINTSEL_Msk

/*****************  Bit definition for CRU_SYSCLKSEL1 register  ***************/

/** @brief SYSCLKSEL1[4:0]: TCLK divider */
#define CRU_SYSCLKSEL1_TCLKDIV_Pos          (0U)
#define CRU_SYSCLKSEL1_TCLKDIV_Msk          (0x1FUL << CRU_SYSCLKSEL1_TCLKDIV_Pos)
#define CRU_SYSCLKSEL1_TCLKDIV              CRU_SYSCLKSEL1_TCLKDIV_Msk

#define CRU_SYSCLKSEL1_TCLKDIV_DIV1         (0x0UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)   /*!< TCLK not divided */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV2         (0x2UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)   /*!< TCLK divided by 2 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV3         (0x4UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)   /*!< TCLK divided by 3 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV4         (0x6UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)   /*!< TCLK divided by 4 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV5         (0x8UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)   /*!< TCLK divided by 5 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV6         (0xAUL << CRU_SYSCLKSEL1_TCLKDIV_Pos)   /*!< TCLK divided by 6 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV7         (0xCUL << CRU_SYSCLKSEL1_TCLKDIV_Pos)   /*!< TCLK divided by 7 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV8         (0xEUL << CRU_SYSCLKSEL1_TCLKDIV_Pos)   /*!< TCLK divided by 8 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV9         (0x10UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)  /*!< TCLK divided by 9 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV10        (0x12UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)  /*!< TCLK divided by 10 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV11        (0x14UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)  /*!< TCLK divided by 11 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV12        (0x16UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)  /*!< TCLK divided by 12 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV13        (0x18UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)  /*!< TCLK divided by 13 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV14        (0x1AUL << CRU_SYSCLKSEL1_TCLKDIV_Pos)  /*!< TCLK divided by 14 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV15        (0x1CUL << CRU_SYSCLKSEL1_TCLKDIV_Pos)  /*!< TCLK divided by 15 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV16        (0x1EUL << CRU_SYSCLKSEL1_TCLKDIV_Pos)  /*!< TCLK divided by 16 */

#define CRU_SYSCLKSEL1_TCLKDIV_DIV1_5       (0x1UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)   /*!< TCLK divided by 1.5 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV2_5       (0x3UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)   /*!< TCLK divided by 2.5 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV3_5       (0x5UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)   /*!< TCLK divided by 3.5 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV4_5       (0x7UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)   /*!< TCLK divided by 4.5 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV5_5       (0x9UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)   /*!< TCLK divided by 5.5 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV6_5       (0xBUL << CRU_SYSCLKSEL1_TCLKDIV_Pos)   /*!< TCLK divided by 6.5 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV7_5       (0xDUL << CRU_SYSCLKSEL1_TCLKDIV_Pos)   /*!< TCLK divided by 7.5 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV8_5       (0xFUL << CRU_SYSCLKSEL1_TCLKDIV_Pos)   /*!< TCLK divided by 8.5 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV9_5       (0x11UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)  /*!< TCLK divided by 9.5 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV10_5      (0x13UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)  /*!< TCLK divided by 10.5 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV11_5      (0x15UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)  /*!< TCLK divided by 11.5 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV12_5      (0x17UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)  /*!< TCLK divided by 12.5 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV13_5      (0x19UL << CRU_SYSCLKSEL1_TCLKDIV_Pos)  /*!< TCLK divided by 13.5 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV14_5      (0x1BUL << CRU_SYSCLKSEL1_TCLKDIV_Pos)  /*!< TCLK divided by 14.5 */
#define CRU_SYSCLKSEL1_TCLKDIV_DIV15_5      (0x1DUL << CRU_SYSCLKSEL1_TCLKDIV_Pos)  /*!< TCLK divided by 15.5 */

/** @brief SYSCLKSEL1[5]: TCLK divider enable */
#define CRU_SYSCLKSEL1_TCLKDIVEN_Pos        (5U)
#define CRU_SYSCLKSEL1_TCLKDIVEN_Msk        (0x1UL << CRU_SYSCLKSEL1_TCLKDIVEN_Pos)
#define CRU_SYSCLKSEL1_TCLKDIVEN            CRU_SYSCLKSEL1_TCLKDIVEN_Msk

/** @brief SYSCLKSEL1[10:6]: CANCLK divider */
#define CRU_SYSCLKSEL1_CANCLKDIV_Pos        (6U)
#define CRU_SYSCLKSEL1_CANCLKDIV_Msk        (0x1FUL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)
#define CRU_SYSCLKSEL1_CANCLKDIV            CRU_SYSCLKSEL1_CANCLKDIV_Msk

#define CRU_SYSCLKSEL1_CANCLKDIV_DIV1       (0x0UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)     /*!< CANCLK not divided */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV2       (0x2UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)     /*!< CANCLK divided by 2 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV3       (0x4UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)     /*!< CANCLK divided by 3 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV4       (0x6UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)     /*!< CANCLK divided by 4 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV5       (0x8UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)     /*!< CANCLK divided by 5 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV6       (0xAUL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)     /*!< CANCLK divided by 6 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV7       (0xCUL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)     /*!< CANCLK divided by 7 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV8       (0xEUL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)     /*!< CANCLK divided by 8 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV9       (0x10UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)    /*!< CANCLK divided by 9 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV10      (0x12UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)    /*!< CANCLK divided by 10 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV11      (0x14UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)    /*!< CANCLK divided by 11 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV12      (0x16UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)    /*!< CANCLK divided by 12 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV13      (0x18UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)    /*!< CANCLK divided by 13 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV14      (0x1AUL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)    /*!< CANCLK divided by 14 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV15      (0x1CUL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)    /*!< CANCLK divided by 15 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV16      (0x1EUL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)    /*!< CANCLK divided by 16 */

#define CRU_SYSCLKSEL1_CANCLKDIV_DIV1_5     (0x1UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)     /*!< CANCLK divided by 1.5 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV2_5     (0x3UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)     /*!< CANCLK divided by 2.5 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV3_5     (0x5UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)     /*!< CANCLK divided by 3.5 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV4_5     (0x7UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)     /*!< CANCLK divided by 4.5 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV5_5     (0x9UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)     /*!< CANCLK divided by 5.5 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV6_5     (0xBUL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)     /*!< CANCLK divided by 6.5 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV7_5     (0xDUL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)     /*!< CANCLK divided by 7.5 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV8_5     (0xFUL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)     /*!< CANCLK divided by 8.5 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV9_5     (0x11UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)    /*!< CANCLK divided by 9.5 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV10_5    (0x13UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)    /*!< CANCLK divided by 10.5 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV11_5    (0x15UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)    /*!< CANCLK divided by 11.5 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV12_5    (0x17UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)    /*!< CANCLK divided by 12.5 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV13_5    (0x19UL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)    /*!< CANCLK divided by 13.5 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV14_5    (0x1BUL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)    /*!< CANCLK divided by 14.5 */
#define CRU_SYSCLKSEL1_CANCLKDIV_DIV15_5    (0x1DUL << CRU_SYSCLKSEL1_CANCLKDIV_Pos)    /*!< CANCLK divided by 15.5 */

/** @brief SYSCLKSEL1[11]: CANCLK divider enable */
#define CRU_SYSCLKSEL1_CANCLKDIVEN_Pos      (11U)
#define CRU_SYSCLKSEL1_CANCLKDIVEN_Msk      (0x1UL << CRU_SYSCLKSEL1_CANCLKDIVEN_Pos)
#define CRU_SYSCLKSEL1_CANCLKDIVEN          CRU_SYSCLKSEL1_CANCLKDIVEN_Msk

/******************  Bit definition for CRU_CRPTADDR register  ****************/

/** @brief CRPTADDR[3:0]: Data destination */
#define CRU_CRPTADDR_DEST_Pos               (0U)
#define CRU_CRPTADDR_DEST_Msk               (0xFUL << CRU_CRPTADDR_DEST_Pos)
#define CRU_CRPTADDR_DEST                   CRU_CRPTADDR_DEST_Msk
/** @brief CRPTADDR[9:8]: Channel */
#define CRU_CRPTADDR_CH_Pos                 (8U)
#define CRU_CRPTADDR_CH_Msk                 (0x3UL << CRU_CRPTADDR_CH_Pos)
#define CRU_CRPTADDR_CH                     CRU_CRPTADDR_CH_Msk

/******************  Bit definition for CRU_CRPTDATA register  ****************/

/** @brief CRPTDATA[31:0]: Data to be written */
#define CRU_CRPTDATA_Pos                    (0U)
#define CRU_CRPTDATA_Msk                    (0xFFFFFFFFUL << CRU_CRPTDATA_Pos)
#define CRU_CRPTDATA                        CRU_CRPTDATA_Msk

/******************************************************************************/
/*                       Direct Memory Access Controller                      */
/******************************************************************************/

/******************  Bit definition for DMA_CH_SAR register  ******************/

/** @brief DMA_CH_SAR[31:0]: Source address of DMA transfer */
#define DMA_CH_SAR_Pos                      (0U)
#define DMA_CH_SAR_Msk                      (0xFFFFFFFFUL << DMA_CH_SAR_Pos)
#define DMA_CH_SAR                          DMA_CH_SAR_Msk

/******************  Bit definition for DMA_CH_DAR register  ******************/

/** @brief DMA_CH_DAR[31:0]: Destination address of DMA transfer */
#define DMA_CH_DAR_Pos                      (0U)
#define DMA_CH_DAR_Msk                      (0xFFFFFFFFUL << DMA_CH_DAR_Pos)
#define DMA_CH_DAR                          DMA_CH_DAR_Msk

/******************  Bit definition for DMA_CH_LLP register  ******************/

/** @brief DMA_CH_LLP[31:2]: Start address of the next LLI */
#define DMA_CH_LLP_LOC_Pos                  (2U)
#define DMA_CH_LLP_LOC_Msk                  (0x3FFFFFFFUL << DMA_CH_LLP_LOC_Pos)
#define DMA_CH_LLP_LOC                      DMA_CH_LLP_LOC_Msk

/*****************  Bit definition for DMA_CH_CTL0 register  ******************/

/** @brief DMA_CH_CTL0[0]: Interrupt enable */
#define DMA_CH_CTL0_INT_EN_Pos              (0U)
#define DMA_CH_CTL0_INT_EN_Msk              (0x1UL << DMA_CH_CTL0_INT_EN_Pos)
#define DMA_CH_CTL0_INT_EN                  DMA_CH_CTL0_INT_EN_Msk

#define DMA_CH_CTL0_INT_EN_DISABLE          (0x0UL << DMA_CH_CTL0_INT_EN_Pos)   /*!< Interrupts disabled */
#define DMA_CH_CTL0_INT_EN_ENABLE           (0x1UL << DMA_CH_CTL0_INT_EN_Pos)   /*!< Interrupts enabled */

/** @brief DMA_CH_CTL0[3:1]: Destination transfer width */
#define DMA_CH_CTL0_DTW_Pos                 (1U)
#define DMA_CH_CTL0_DTW_Msk                 (0x7UL << DMA_CH_CTL0_DTW_Pos)
#define DMA_CH_CTL0_DTW                     DMA_CH_CTL0_DTW_Msk

#define DMA_CH_CTL0_DTW_0                   (0x0UL << DMA_CH_CTL0_DTW_Pos)  /*!< Destination transfer width is 8 bits */
#define DMA_CH_CTL0_DTW_1                   (0x1UL << DMA_CH_CTL0_DTW_Pos)  /*!< Destination transfer width is 16 bits */
#define DMA_CH_CTL0_DTW_2                   (0x2UL << DMA_CH_CTL0_DTW_Pos)  /*!< Destination transfer width is 32 bits */

/** @brief DMA_CH_CTL0[6:4]: Source transfer width */
#define DMA_CH_CTL0_STW_Pos                 (4U)
#define DMA_CH_CTL0_STW_Msk                 (0x7UL << DMA_CH_CTL0_STW_Pos)
#define DMA_CH_CTL0_STW                     DMA_CH_CTL0_STW_Msk

#define DMA_CH_CTL0_STW_0                   (0x0UL << DMA_CH_CTL0_STW_Pos)  /*!< Source transfer width is 8 bits */
#define DMA_CH_CTL0_STW_1                   (0x1UL << DMA_CH_CTL0_STW_Pos)  /*!< Source transfer width is 16 bits */
#define DMA_CH_CTL0_STW_2                   (0x2UL << DMA_CH_CTL0_STW_Pos)  /*!< Source transfer width is 32 bits */

/** @brief DMA_CH_CTL0[8:7]: Destination address increment */
#define DMA_CH_CTL0_DINC_Pos                (7U)
#define DMA_CH_CTL0_DINC_Msk                (0x3UL << DMA_CH_CTL0_DINC_Pos)
#define DMA_CH_CTL0_DINC                    DMA_CH_CTL0_DINC_Msk

#define DMA_CH_CTL0_DINC_0                  (0x0UL << DMA_CH_CTL0_DINC_Pos) /*!< Increments the destination address */
#define DMA_CH_CTL0_DINC_1                  (0x1UL << DMA_CH_CTL0_DINC_Pos) /*!< Decrements the destination address */
#define DMA_CH_CTL0_DINC_2                  (0x2UL << DMA_CH_CTL0_DINC_Pos) /*!< No change in the destination address */
#define DMA_CH_CTL0_DINC_3                  (0x3UL << DMA_CH_CTL0_DINC_Pos) /*!< No change in the destination address */

/** @brief DMA_CH_CTL0[10:9]: Source address increment */
#define DMA_CH_CTL0_SINC_Pos                (9U)
#define DMA_CH_CTL0_SINC_Msk                (0x3UL << DMA_CH_CTL0_SINC_Pos)
#define DMA_CH_CTL0_SINC                    DMA_CH_CTL0_SINC_Msk

#define DMA_CH_CTL0_SINC_0                  (0x0UL << DMA_CH_CTL0_SINC_Pos) /*!< Increments the source address */
#define DMA_CH_CTL0_SINC_1                  (0x1UL << DMA_CH_CTL0_SINC_Pos) /*!< Decrements the source address */
#define DMA_CH_CTL0_SINC_2                  (0x2UL << DMA_CH_CTL0_SINC_Pos) /*!< No change in the source address */
#define DMA_CH_CTL0_SINC_3                  (0x3UL << DMA_CH_CTL0_SINC_Pos) /*!< No change in the source address */

/** @brief DMA_CH_CTL0[13:11]: Destination burst transaction length */
#define DMA_CH_CTL0_DST_MSIZE_Pos           (11U)
#define DMA_CH_CTL0_DST_MSIZE_Msk           (0x7UL << DMA_CH_CTL0_DST_MSIZE_Pos)
#define DMA_CH_CTL0_DST_MSIZE               DMA_CH_CTL0_DST_MSIZE_Msk

#define DMA_CH_CTL0_DST_MSIZE_0             (0x0UL << DMA_CH_CTL0_DST_MSIZE_Pos)    /*!< Number of data items to be transferred is 1 */
#define DMA_CH_CTL0_DST_MSIZE_1             (0x1UL << DMA_CH_CTL0_DST_MSIZE_Pos)    /*!< Number of data items to be transferred is 4 */
#define DMA_CH_CTL0_DST_MSIZE_2             (0x2UL << DMA_CH_CTL0_DST_MSIZE_Pos)    /*!< Number of data items to be transferred is 8 */
#define DMA_CH_CTL0_DST_MSIZE_3             (0x3UL << DMA_CH_CTL0_DST_MSIZE_Pos)    /*!< Number of data items to be transferred is 16 */
#define DMA_CH_CTL0_DST_MSIZE_4             (0x4UL << DMA_CH_CTL0_DST_MSIZE_Pos)    /*!< Number of data items to be transferred is 32 */
#define DMA_CH_CTL0_DST_MSIZE_5             (0x5UL << DMA_CH_CTL0_DST_MSIZE_Pos)    /*!< Number of data items to be transferred is 64 */
#define DMA_CH_CTL0_DST_MSIZE_6             (0x6UL << DMA_CH_CTL0_DST_MSIZE_Pos)    /*!< Number of data items to be transferred is 128 */
#define DMA_CH_CTL0_DST_MSIZE_7             (0x7UL << DMA_CH_CTL0_DST_MSIZE_Pos)    /*!< Number of data items to be transferred is 256 */

/** @brief DMA_CH_CTL0[16:14]: Source burst transaction length */
#define DMA_CH_CTL0_SRC_MSIZE_Pos           (14U)
#define DMA_CH_CTL0_SRC_MSIZE_Msk           (0x7UL << DMA_CH_CTL0_SRC_MSIZE_Pos)
#define DMA_CH_CTL0_SRC_MSIZE               DMA_CH_CTL0_SRC_MSIZE_Msk

#define DMA_CH_CTL0_SRC_MSIZE_0             (0x0UL << DMA_CH_CTL0_SRC_MSIZE_Pos)    /*!< Number of data items to be transferred is 1 */
#define DMA_CH_CTL0_SRC_MSIZE_1             (0x1UL << DMA_CH_CTL0_SRC_MSIZE_Pos)    /*!< Number of data items to be transferred is 4 */
#define DMA_CH_CTL0_SRC_MSIZE_2             (0x2UL << DMA_CH_CTL0_SRC_MSIZE_Pos)    /*!< Number of data items to be transferred is 8 */
#define DMA_CH_CTL0_SRC_MSIZE_3             (0x3UL << DMA_CH_CTL0_SRC_MSIZE_Pos)    /*!< Number of data items to be transferred is 16 */
#define DMA_CH_CTL0_SRC_MSIZE_4             (0x4UL << DMA_CH_CTL0_SRC_MSIZE_Pos)    /*!< Number of data items to be transferred is 32 */
#define DMA_CH_CTL0_SRC_MSIZE_5             (0x5UL << DMA_CH_CTL0_SRC_MSIZE_Pos)    /*!< Number of data items to be transferred is 64 */
#define DMA_CH_CTL0_SRC_MSIZE_6             (0x6UL << DMA_CH_CTL0_SRC_MSIZE_Pos)    /*!< Number of data items to be transferred is 128 */
#define DMA_CH_CTL0_SRC_MSIZE_7             (0x7UL << DMA_CH_CTL0_SRC_MSIZE_Pos)    /*!< Number of data items to be transferred is 256 */

/** @brief DMA_CH_CTL0[21:20]: Transfer type and flow control */
#define DMA_CH_CTL0_TT_FC_Pos               (20U)
#define DMA_CH_CTL0_TT_FC_Msk               (0x3UL << DMA_CH_CTL0_TT_FC_Pos)
#define DMA_CH_CTL0_TT_FC                   DMA_CH_CTL0_TT_FC_Msk

#define DMA_CH_CTL0_TT_FC_0                 (0x0UL << DMA_CH_CTL0_TT_FC_Pos)    /*!< Transfer type is Memory to Memory and Flow Controller is DMA Controller */
#define DMA_CH_CTL0_TT_FC_1                 (0x1UL << DMA_CH_CTL0_TT_FC_Pos)    /*!< Transfer type is Memory to Peripheral and Flow Controller is DMA Controller */
#define DMA_CH_CTL0_TT_FC_2                 (0x2UL << DMA_CH_CTL0_TT_FC_Pos)    /*!< Transfer type is Peripheral to Memory and Flow Controller is DMA Controller */
#define DMA_CH_CTL0_TT_FC_3                 (0x3UL << DMA_CH_CTL0_TT_FC_Pos)    /*!< Transfer type is Peripheral to Peripheral and Flow Controller is DMA Controller */

/** @brief DMA_CH_CTL0[27]: Enable block chaining using Linked List on the destination side */
#define DMA_CH_CTL0_LLP_DST_EN_Pos          (27U)
#define DMA_CH_CTL0_LLP_DST_EN_Msk          (0x1UL << DMA_CH_CTL0_LLP_DST_EN_Pos)
#define DMA_CH_CTL0_LLP_DST_EN              DMA_CH_CTL0_LLP_DST_EN_Msk
/** @brief DMA_CH_CTL0[28]: Enable block chaining using Linked List on the source side */
#define DMA_CH_CTL0_LLP_SRC_EN_Pos          (28U)
#define DMA_CH_CTL0_LLP_SRC_EN_Msk          (0x1UL << DMA_CH_CTL0_LLP_SRC_EN_Pos)
#define DMA_CH_CTL0_LLP_SRC_EN              DMA_CH_CTL0_LLP_SRC_EN_Msk

/*****************  Bit definition for DMA_CH_CTL1 register  ******************/

/** @brief DMA_CH_CTL1[7:0]: Block transfer size */
#define DMA_CH_CTL1_BLOCK_TS_Pos            (0U)
#define DMA_CH_CTL1_BLOCK_TS_Msk            (0xFFUL << DMA_CH_CTL1_BLOCK_TS_Pos)
#define DMA_CH_CTL1_BLOCK_TS                DMA_CH_CTL1_BLOCK_TS_Msk
/** @brief DMA_CH_CTL1[12]: Block transfer is done */
#define DMA_CH_CTL1_DONE_Pos                (12U)
#define DMA_CH_CTL1_DONE_Msk                (0x1UL << DMA_CH_CTL1_DONE_Pos)
#define DMA_CH_CTL1_DONE                    DMA_CH_CTL1_DONE_Msk

/*****************  Bit definition for DMA_CH_CFG0 register  ******************/

/** @brief DMA_CH_CFG0[7:5]: Channel priority */
#define DMA_CH_CFG0_PRIOR_Pos               (5U)
#define DMA_CH_CFG0_PRIOR_Msk               (0x7UL << DMA_CH_CFG0_PRIOR_Pos)
#define DMA_CH_CFG0_PRIOR                   DMA_CH_CFG0_PRIOR_Msk

#define DMA_CH_CFG0_PRIOR_0                 (0x0UL << DMA_CH_CFG0_PRIOR_Pos)    /*!< Channel priority is 0 */
#define DMA_CH_CFG0_PRIOR_1                 (0x1UL << DMA_CH_CFG0_PRIOR_Pos)    /*!< Channel priority is 1 */
#define DMA_CH_CFG0_PRIOR_2                 (0x2UL << DMA_CH_CFG0_PRIOR_Pos)    /*!< Channel priority is 2 */
#define DMA_CH_CFG0_PRIOR_3                 (0x3UL << DMA_CH_CFG0_PRIOR_Pos)    /*!< Channel priority is 3 */
#define DMA_CH_CFG0_PRIOR_4                 (0x4UL << DMA_CH_CFG0_PRIOR_Pos)    /*!< Channel priority is 4 */
#define DMA_CH_CFG0_PRIOR_5                 (0x5UL << DMA_CH_CFG0_PRIOR_Pos)    /*!< Channel priority is 5 */
#define DMA_CH_CFG0_PRIOR_6                 (0x6UL << DMA_CH_CFG0_PRIOR_Pos)    /*!< Channel priority is 6 */
#define DMA_CH_CFG0_PRIOR_7                 (0x7UL << DMA_CH_CFG0_PRIOR_Pos)    /*!< Channel priority is 7 */

/** @brief DMA_CH_CFG0[8]: Channel suspend */
#define DMA_CH_CFG0_SUSP_Pos                (8U)
#define DMA_CH_CFG0_SUSP_Msk                (0x1UL << DMA_CH_CFG0_SUSP_Pos)
#define DMA_CH_CFG0_SUSP                    DMA_CH_CFG0_SUSP_Msk
/** @brief DMA_CH_CFG0[9]: Channel FIFO status */
#define DMA_CH_CFG0_FIFO_EMPTY_Pos          (9U)
#define DMA_CH_CFG0_FIFO_EMPTY_Msk          (0x1UL << DMA_CH_CFG0_FIFO_EMPTY_Pos)
#define DMA_CH_CFG0_FIFO_EMPTY              DMA_CH_CFG0_FIFO_EMPTY_Msk

/** @brief DMA_CH_CFG0[10]: Destination handshaking select */
#define DMA_CH_CFG0_HS_SEL_DST_Pos          (10U)
#define DMA_CH_CFG0_HS_SEL_DST_Msk          (0x1UL << DMA_CH_CFG0_HS_SEL_DST_Pos)
#define DMA_CH_CFG0_HS_SEL_DST              DMA_CH_CFG0_HS_SEL_DST_Msk

#define DMA_CH_CFG0_HS_SEL_DST_HW           (0x0UL << DMA_CH_CFG0_HS_SEL_DST_Pos)   /*!< Hardware handshaking interface */
#define DMA_CH_CFG0_HS_SEL_DST_SW           (0x1UL << DMA_CH_CFG0_HS_SEL_DST_Pos)   /*!< Software handshaking interface */

/** @brief DMA_CH_CFG0[11]: Source handshaking select */
#define DMA_CH_CFG0_HS_SEL_SRC_Pos          (11U)
#define DMA_CH_CFG0_HS_SEL_SRC_Msk          (0x1UL << DMA_CH_CFG0_HS_SEL_SRC_Pos)
#define DMA_CH_CFG0_HS_SEL_SRC              DMA_CH_CFG0_HS_SEL_SRC_Msk

#define DMA_CH_CFG0_HS_SEL_SRC_HW           (0x0UL << DMA_CH_CFG0_HS_SEL_SRC_Pos)   /*!< Hardware handshaking interface */
#define DMA_CH_CFG0_HS_SEL_SRC_SW           (0x1UL << DMA_CH_CFG0_HS_SEL_SRC_Pos)   /*!< Software handshaking interface */

/** @brief DMA_CH_CFG0[18]: Destination handshaking interface polarity */
#define DMA_CH_CFG0_DST_HS_POL_Pos          (18U)
#define DMA_CH_CFG0_DST_HS_POL_Msk          (0x1UL << DMA_CH_CFG0_DST_HS_POL_Pos)
#define DMA_CH_CFG0_DST_HS_POL              DMA_CH_CFG0_DST_HS_POL_Msk

#define DMA_CH_CFG0_DST_HS_POL_ACTIVE_HIGH  (0x0UL << DMA_CH_CFG0_DST_HS_POL_Pos)   /*!< Active high */
#define DMA_CH_CFG0_DST_HS_POL_ACTIVE_LOW   (0x1UL << DMA_CH_CFG0_DST_HS_POL_Pos)   /*!< Active low */

/** @brief DMA_CH_CFG0[19]: Source handshaking interface polarity */
#define DMA_CH_CFG0_SRC_HS_POL_Pos          (19U)
#define DMA_CH_CFG0_SRC_HS_POL_Msk          (0x1UL << DMA_CH_CFG0_SRC_HS_POL_Pos)
#define DMA_CH_CFG0_SRC_HS_POL              DMA_CH_CFG0_SRC_HS_POL_Msk

#define DMA_CH_CFG0_SRC_HS_POL_ACTIVE_HIGH  (0x0UL << DMA_CH_CFG0_SRC_HS_POL_Pos)   /*!< Active high */
#define DMA_CH_CFG0_SRC_HS_POL_ACTIVE_LOW   (0x1UL << DMA_CH_CFG0_SRC_HS_POL_Pos)   /*!< Active low */

/** @brief DMA_CH_CFG0[30]: Automatic source reload */
#define DMA_CH_CFG0_RELOAD_SRC_Pos          (30U)
#define DMA_CH_CFG0_RELOAD_SRC_Msk          (0x1UL << DMA_CH_CFG0_RELOAD_SRC_Pos)
#define DMA_CH_CFG0_RELOAD_SRC              DMA_CH_CFG0_RELOAD_SRC_Msk
/** @brief DMA_CH_CFG0[31]: Automatic destination reload */
#define DMA_CH_CFG0_RELOAD_DST_Pos          (31U)
#define DMA_CH_CFG0_RELOAD_DST_Msk          (0x1UL << DMA_CH_CFG0_RELOAD_DST_Pos)
#define DMA_CH_CFG0_RELOAD_DST              DMA_CH_CFG0_RELOAD_DST_Msk

/*****************  Bit definition for DMA_CH_CFG1 register  ******************/

/** @brief DMA_CH_CFG1[0]: Flow control mode */
#define DMA_CH_CFG1_FCMODE_Pos              (0U)
#define DMA_CH_CFG1_FCMODE_Msk              (0x1UL << DMA_CH_CFG1_FCMODE_Pos)
#define DMA_CH_CFG1_FCMODE                  DMA_CH_CFG1_FCMODE_Msk

#define DMA_CH_CFG1_FCMODE_0                (0x0UL << DMA_CH_CFG1_FCMODE_Pos)   /*!< Source transaction requests are serviced when they occur */
#define DMA_CH_CFG1_FCMODE_1                (0x1UL << DMA_CH_CFG1_FCMODE_Pos)   /*!< Source transaction requests are not serviced until a destination transaction request occurs */

/** @brief DMA_CH_CFG1[1]: FIFO mode select */
#define DMA_CH_CFG1_FIFO_MODE_Pos           (1U)
#define DMA_CH_CFG1_FIFO_MODE_Msk           (0x1UL << DMA_CH_CFG1_FIFO_MODE_Pos)
#define DMA_CH_CFG1_FIFO_MODE               DMA_CH_CFG1_FIFO_MODE_Msk

#define DMA_CH_CFG1_FIFO_MODE_0             (0x0UL << DMA_CH_CFG1_FIFO_MODE_Pos)    /*!< FIFO can transmit or accept a single AHB transfer */
#define DMA_CH_CFG1_FIFO_MODE_1             (0x1UL << DMA_CH_CFG1_FIFO_MODE_Pos)    /*!< FIFO is less or equal than half full to fetch data from source or greater or equal than half full to send data to destination */

/** @brief DMA_CH_CFG1[4:2]: Protection control */
#define DMA_CH_CFG1_PROTCTL_Pos             (2U)
#define DMA_CH_CFG1_PROTCTL_Msk             (0x7UL << DMA_CH_CFG1_PROTCTL_Pos)
#define DMA_CH_CFG1_PROTCTL                 DMA_CH_CFG1_PROTCTL_Msk

#define DMA_CH_CFG1_PROTCTL_0               (0x0UL << DMA_CH_CFG1_PROTCTL_Pos)  /*!< Drive the AHB HPROT[0] bus */
#define DMA_CH_CFG1_PROTCTL_1               (0x1UL << DMA_CH_CFG1_PROTCTL_Pos)  /*!< Drive the AHB HPROT[1] bus */
#define DMA_CH_CFG1_PROTCTL_2               (0x2UL << DMA_CH_CFG1_PROTCTL_Pos)  /*!< Drive the AHB HPROT[2] bus */
#define DMA_CH_CFG1_PROTCTL_3               (0x3UL << DMA_CH_CFG1_PROTCTL_Pos)  /*!< Drive the AHB HPROT[3] bus */
#define DMA_CH_CFG1_PROTCTL_4               (0x4UL << DMA_CH_CFG1_PROTCTL_Pos)  /*!< Drive the AHB HPROT[4] bus */
#define DMA_CH_CFG1_PROTCTL_5               (0x5UL << DMA_CH_CFG1_PROTCTL_Pos)  /*!< Drive the AHB HPROT[5] bus */
#define DMA_CH_CFG1_PROTCTL_6               (0x6UL << DMA_CH_CFG1_PROTCTL_Pos)  /*!< Drive the AHB HPROT[6] bus */
#define DMA_CH_CFG1_PROTCTL_7               (0x7UL << DMA_CH_CFG1_PROTCTL_Pos)  /*!< Drive the AHB HPROT[7] bus */

/** @brief DMA_CH_CFG1[10:7]: Source hardware interface */
#define DMA_CH_CFG1_SRC_PER_Pos             (7U)
#define DMA_CH_CFG1_SRC_PER_Msk             (0xFUL << DMA_CH_CFG1_SRC_PER_Pos)
#define DMA_CH_CFG1_SRC_PER                 DMA_CH_CFG1_SRC_PER_Msk
/** @brief DMA_CH_CFG1[14:11]: Destination hardware interface */
#define DMA_CH_CFG1_DST_PER_Pos             (11U)
#define DMA_CH_CFG1_DST_PER_Msk             (0xFUL << DMA_CH_CFG1_DST_PER_Pos)
#define DMA_CH_CFG1_DST_PER                 DMA_CH_CFG1_DST_PER_Msk

/***************  Bit definition for DMA_INT_RAW_TFR register  ****************/

/** @brief DMA_INT_RAW_TFR[0]: Channel 0 raw status for IntTfr interrupt */
#define DMA_INT_RAW_TFR_CH0_Pos             (0U)
#define DMA_INT_RAW_TFR_CH0_Msk             (0x1UL << DMA_INT_RAW_TFR_CH0_Pos)
#define DMA_INT_RAW_TFR_CH0                 DMA_INT_RAW_TFR_CH0_Msk
/** @brief DMA_INT_RAW_TFR[1]: Channel 1 raw status for IntTfr interrupt */
#define DMA_INT_RAW_TFR_CH1_Pos             (1U)
#define DMA_INT_RAW_TFR_CH1_Msk             (0x1UL << DMA_INT_RAW_TFR_CH1_Pos)
#define DMA_INT_RAW_TFR_CH1                 DMA_INT_RAW_TFR_CH1_Msk
/** @brief DMA_INT_RAW_TFR[2]: Channel 2 raw status for IntTfr interrupt */
#define DMA_INT_RAW_TFR_CH2_Pos             (2U)
#define DMA_INT_RAW_TFR_CH2_Msk             (0x1UL << DMA_INT_RAW_TFR_CH2_Pos)
#define DMA_INT_RAW_TFR_CH2                 DMA_INT_RAW_TFR_CH2_Msk
/** @brief DMA_INT_RAW_TFR[3]: Channel 3 raw status for IntTfr interrupt */
#define DMA_INT_RAW_TFR_CH3_Pos             (3U)
#define DMA_INT_RAW_TFR_CH3_Msk             (0x1UL << DMA_INT_RAW_TFR_CH3_Pos)
#define DMA_INT_RAW_TFR_CH3                 DMA_INT_RAW_TFR_CH3_Msk
/** @brief DMA_INT_RAW_TFR[4]: Channel 4 raw status for IntTfr interrupt */
#define DMA_INT_RAW_TFR_CH4_Pos             (4U)
#define DMA_INT_RAW_TFR_CH4_Msk             (0x1UL << DMA_INT_RAW_TFR_CH4_Pos)
#define DMA_INT_RAW_TFR_CH4                 DMA_INT_RAW_TFR_CH4_Msk
/** @brief DMA_INT_RAW_TFR[5]: Channel 5 raw status for IntTfr interrupt */
#define DMA_INT_RAW_TFR_CH5_Pos             (5U)
#define DMA_INT_RAW_TFR_CH5_Msk             (0x1UL << DMA_INT_RAW_TFR_CH5_Pos)
#define DMA_INT_RAW_TFR_CH5                 DMA_INT_RAW_TFR_CH5_Msk
/** @brief DMA_INT_RAW_TFR[6]: Channel 6 raw status for IntTfr interrupt */
#define DMA_INT_RAW_TFR_CH6_Pos             (6U)
#define DMA_INT_RAW_TFR_CH6_Msk             (0x1UL << DMA_INT_RAW_TFR_CH6_Pos)
#define DMA_INT_RAW_TFR_CH6                 DMA_INT_RAW_TFR_CH6_Msk
/** @brief DMA_INT_RAW_TFR[7]: Channel 7 raw status for IntTfr interrupt */
#define DMA_INT_RAW_TFR_CH7_Pos             (7U)
#define DMA_INT_RAW_TFR_CH7_Msk             (0x1UL << DMA_INT_RAW_TFR_CH7_Pos)
#define DMA_INT_RAW_TFR_CH7                 DMA_INT_RAW_TFR_CH7_Msk

/**************  Bit definition for DMA_INT_RAW_BLOCK register  ***************/

/** @brief DMA_INT_RAW_BLOCK[0]: Channel 0 raw status for IntBlock interrupt */
#define DMA_INT_RAW_BLOCK_CH0_Pos           (0U)
#define DMA_INT_RAW_BLOCK_CH0_Msk           (0x1UL << DMA_INT_RAW_BLOCK_CH0_Pos)
#define DMA_INT_RAW_BLOCK_CH0               DMA_INT_RAW_BLOCK_CH0_Msk
/** @brief DMA_INT_RAW_BLOCK[1]: Channel 1 raw status for IntBlock interrupt */
#define DMA_INT_RAW_BLOCK_CH1_Pos           (1U)
#define DMA_INT_RAW_BLOCK_CH1_Msk           (0x1UL << DMA_INT_RAW_BLOCK_CH1_Pos)
#define DMA_INT_RAW_BLOCK_CH1               DMA_INT_RAW_BLOCK_CH1_Msk
/** @brief DMA_INT_RAW_BLOCK[2]: Channel 2 raw status for IntBlock interrupt */
#define DMA_INT_RAW_BLOCK_CH2_Pos           (2U)
#define DMA_INT_RAW_BLOCK_CH2_Msk           (0x1UL << DMA_INT_RAW_BLOCK_CH2_Pos)
#define DMA_INT_RAW_BLOCK_CH2               DMA_INT_RAW_BLOCK_CH2_Msk
/** @brief DMA_INT_RAW_BLOCK[3]: Channel 3 raw status for IntBlock interrupt */
#define DMA_INT_RAW_BLOCK_CH3_Pos           (3U)
#define DMA_INT_RAW_BLOCK_CH3_Msk           (0x1UL << DMA_INT_RAW_BLOCK_CH3_Pos)
#define DMA_INT_RAW_BLOCK_CH3               DMA_INT_RAW_BLOCK_CH3_Msk
/** @brief DMA_INT_RAW_BLOCK[4]: Channel 4 raw status for IntBlock interrupt */
#define DMA_INT_RAW_BLOCK_CH4_Pos           (4U)
#define DMA_INT_RAW_BLOCK_CH4_Msk           (0x1UL << DMA_INT_RAW_BLOCK_CH4_Pos)
#define DMA_INT_RAW_BLOCK_CH4               DMA_INT_RAW_BLOCK_CH4_Msk
/** @brief DMA_INT_RAW_BLOCK[5]: Channel 5 raw status for IntBlock interrupt */
#define DMA_INT_RAW_BLOCK_CH5_Pos           (5U)
#define DMA_INT_RAW_BLOCK_CH5_Msk           (0x1UL << DMA_INT_RAW_BLOCK_CH5_Pos)
#define DMA_INT_RAW_BLOCK_CH5               DMA_INT_RAW_BLOCK_CH5_Msk
/** @brief DMA_INT_RAW_BLOCK[6]: Channel 6 raw status for IntBlock interrupt */
#define DMA_INT_RAW_BLOCK_CH6_Pos           (6U)
#define DMA_INT_RAW_BLOCK_CH6_Msk           (0x1UL << DMA_INT_RAW_BLOCK_CH6_Pos)
#define DMA_INT_RAW_BLOCK_CH6               DMA_INT_RAW_BLOCK_CH6_Msk
/** @brief DMA_INT_RAW_BLOCK[7]: Channel 7 raw status for IntBlock interrupt */
#define DMA_INT_RAW_BLOCK_CH7_Pos           (7U)
#define DMA_INT_RAW_BLOCK_CH7_Msk           (0x1UL << DMA_INT_RAW_BLOCK_CH7_Pos)
#define DMA_INT_RAW_BLOCK_CH7               DMA_INT_RAW_BLOCK_CH7_Msk

/*************  Bit definition for DMA_INT_RAW_SRC_TRAN register  *************/

/** @brief DMA_INT_RAW_SRC_TRAN[0]: Channel 0 raw status for IntSrcTran interrupt */
#define DMA_INT_RAW_SRC_TRAN_CH0_Pos        (0U)
#define DMA_INT_RAW_SRC_TRAN_CH0_Msk        (0x1UL << DMA_INT_RAW_SRC_TRAN_CH0_Pos)
#define DMA_INT_RAW_SRC_TRAN_CH0            DMA_INT_RAW_SRC_TRAN_CH0_Msk
/** @brief DMA_INT_RAW_SRC_TRAN[1]: Channel 1 raw status for IntSrcTran interrupt */
#define DMA_INT_RAW_SRC_TRAN_CH1_Pos        (1U)
#define DMA_INT_RAW_SRC_TRAN_CH1_Msk        (0x1UL << DMA_INT_RAW_SRC_TRAN_CH1_Pos)
#define DMA_INT_RAW_SRC_TRAN_CH1            DMA_INT_RAW_SRC_TRAN_CH1_Msk
/** @brief DMA_INT_RAW_SRC_TRAN[2]: Channel 2 raw status for IntSrcTran interrupt */
#define DMA_INT_RAW_SRC_TRAN_CH2_Pos        (2U)
#define DMA_INT_RAW_SRC_TRAN_CH2_Msk        (0x1UL << DMA_INT_RAW_SRC_TRAN_CH2_Pos)
#define DMA_INT_RAW_SRC_TRAN_CH2            DMA_INT_RAW_SRC_TRAN_CH2_Msk
/** @brief DMA_INT_RAW_SRC_TRAN[3]: Channel 3 raw status for IntSrcTran interrupt */
#define DMA_INT_RAW_SRC_TRAN_CH3_Pos        (3U)
#define DMA_INT_RAW_SRC_TRAN_CH3_Msk        (0x1UL << DMA_INT_RAW_SRC_TRAN_CH3_Pos)
#define DMA_INT_RAW_SRC_TRAN_CH3            DMA_INT_RAW_SRC_TRAN_CH3_Msk
/** @brief DMA_INT_RAW_SRC_TRAN[4]: Channel 4 raw status for IntSrcTran interrupt */
#define DMA_INT_RAW_SRC_TRAN_CH4_Pos        (4U)
#define DMA_INT_RAW_SRC_TRAN_CH4_Msk        (0x1UL << DMA_INT_RAW_SRC_TRAN_CH4_Pos)
#define DMA_INT_RAW_SRC_TRAN_CH4            DMA_INT_RAW_SRC_TRAN_CH4_Msk
/** @brief DMA_INT_RAW_SRC_TRAN[5]: Channel 5 raw status for IntSrcTran interrupt */
#define DMA_INT_RAW_SRC_TRAN_CH5_Pos        (5U)
#define DMA_INT_RAW_SRC_TRAN_CH5_Msk        (0x1UL << DMA_INT_RAW_SRC_TRAN_CH5_Pos)
#define DMA_INT_RAW_SRC_TRAN_CH5            DMA_INT_RAW_SRC_TRAN_CH5_Msk
/** @brief DMA_INT_RAW_SRC_TRAN[6]: Channel 6 raw status for IntSrcTran interrupt */
#define DMA_INT_RAW_SRC_TRAN_CH6_Pos        (6U)
#define DMA_INT_RAW_SRC_TRAN_CH6_Msk        (0x1UL << DMA_INT_RAW_SRC_TRAN_CH6_Pos)
#define DMA_INT_RAW_SRC_TRAN_CH6            DMA_INT_RAW_SRC_TRAN_CH6_Msk
/** @brief DMA_INT_RAW_SRC_TRAN[7]: Channel 7 raw status for IntSrcTran interrupt */
#define DMA_INT_RAW_SRC_TRAN_CH7_Pos        (7U)
#define DMA_INT_RAW_SRC_TRAN_CH7_Msk        (0x1UL << DMA_INT_RAW_SRC_TRAN_CH7_Pos)
#define DMA_INT_RAW_SRC_TRAN_CH7            DMA_INT_RAW_SRC_TRAN_CH7_Msk

/*************  Bit definition for DMA_INT_RAW_DST_TRAN register  *************/

/** @brief DMA_INT_RAW_DST_TRAN[0]: Channel 0 raw status for IntDstTran interrupt */
#define DMA_INT_RAW_DST_TRAN_CH0_Pos        (0U)
#define DMA_INT_RAW_DST_TRAN_CH0_Msk        (0x1UL << DMA_INT_RAW_DST_TRAN_CH0_Pos)
#define DMA_INT_RAW_DST_TRAN_CH0            DMA_INT_RAW_DST_TRAN_CH0_Msk
/** @brief DMA_INT_RAW_DST_TRAN[1]: Channel 1 raw status for IntDstTran interrupt */
#define DMA_INT_RAW_DST_TRAN_CH1_Pos        (1U)
#define DMA_INT_RAW_DST_TRAN_CH1_Msk        (0x1UL << DMA_INT_RAW_DST_TRAN_CH1_Pos)
#define DMA_INT_RAW_DST_TRAN_CH1            DMA_INT_RAW_DST_TRAN_CH1_Msk
/** @brief DMA_INT_RAW_DST_TRAN[2]: Channel 2 raw status for IntDstTran interrupt */
#define DMA_INT_RAW_DST_TRAN_CH2_Pos        (2U)
#define DMA_INT_RAW_DST_TRAN_CH2_Msk        (0x1UL << DMA_INT_RAW_DST_TRAN_CH2_Pos)
#define DMA_INT_RAW_DST_TRAN_CH2            DMA_INT_RAW_DST_TRAN_CH2_Msk
/** @brief DMA_INT_RAW_DST_TRAN[3]: Channel 3 raw status for IntDstTran interrupt */
#define DMA_INT_RAW_DST_TRAN_CH3_Pos        (3U)
#define DMA_INT_RAW_DST_TRAN_CH3_Msk        (0x1UL << DMA_INT_RAW_DST_TRAN_CH3_Pos)
#define DMA_INT_RAW_DST_TRAN_CH3            DMA_INT_RAW_DST_TRAN_CH3_Msk
/** @brief DMA_INT_RAW_DST_TRAN[4]: Channel 4 raw status for IntDstTran interrupt */
#define DMA_INT_RAW_DST_TRAN_CH4_Pos        (4U)
#define DMA_INT_RAW_DST_TRAN_CH4_Msk        (0x1UL << DMA_INT_RAW_DST_TRAN_CH4_Pos)
#define DMA_INT_RAW_DST_TRAN_CH4            DMA_INT_RAW_DST_TRAN_CH4_Msk
/** @brief DMA_INT_RAW_DST_TRAN[5]: Channel 5 raw status for IntDstTran interrupt */
#define DMA_INT_RAW_DST_TRAN_CH5_Pos        (5U)
#define DMA_INT_RAW_DST_TRAN_CH5_Msk        (0x1UL << DMA_INT_RAW_DST_TRAN_CH5_Pos)
#define DMA_INT_RAW_DST_TRAN_CH5            DMA_INT_RAW_DST_TRAN_CH5_Msk
/** @brief DMA_INT_RAW_DST_TRAN[6]: Channel 6 raw status for IntDstTran interrupt */
#define DMA_INT_RAW_DST_TRAN_CH6_Pos        (6U)
#define DMA_INT_RAW_DST_TRAN_CH6_Msk        (0x1UL << DMA_INT_RAW_DST_TRAN_CH6_Pos)
#define DMA_INT_RAW_DST_TRAN_CH6            DMA_INT_RAW_DST_TRAN_CH6_Msk
/** @brief DMA_INT_RAW_DST_TRAN[7]: Channel 7 raw status for IntDstTran interrupt */
#define DMA_INT_RAW_DST_TRAN_CH7_Pos        (7U)
#define DMA_INT_RAW_DST_TRAN_CH7_Msk        (0x1UL << DMA_INT_RAW_DST_TRAN_CH7_Pos)
#define DMA_INT_RAW_DST_TRAN_CH7            DMA_INT_RAW_DST_TRAN_CH7_Msk

/***************  Bit definition for DMA_INT_RAW_ERR register  ****************/

/** @brief DMA_INT_RAW_ERR[0]: Channel 0 raw status for IntErr interrupt */
#define DMA_INT_RAW_ERR_CH0_Pos             (0U)
#define DMA_INT_RAW_ERR_CH0_Msk             (0x1UL << DMA_INT_RAW_ERR_CH0_Pos)
#define DMA_INT_RAW_ERR_CH0                 DMA_INT_RAW_ERR_CH0_Msk
/** @brief DMA_INT_RAW_ERR[1]: Channel 1 raw status for IntErr interrupt */
#define DMA_INT_RAW_ERR_CH1_Pos             (1U)
#define DMA_INT_RAW_ERR_CH1_Msk             (0x1UL << DMA_INT_RAW_ERR_CH1_Pos)
#define DMA_INT_RAW_ERR_CH1                 DMA_INT_RAW_ERR_CH1_Msk
/** @brief DMA_INT_RAW_ERR[2]: Channel 2 raw status for IntErr interrupt */
#define DMA_INT_RAW_ERR_CH2_Pos             (2U)
#define DMA_INT_RAW_ERR_CH2_Msk             (0x1UL << DMA_INT_RAW_ERR_CH2_Pos)
#define DMA_INT_RAW_ERR_CH2                 DMA_INT_RAW_ERR_CH2_Msk
/** @brief DMA_INT_RAW_ERR[3]: Channel 3 raw status for IntErr interrupt */
#define DMA_INT_RAW_ERR_CH3_Pos             (3U)
#define DMA_INT_RAW_ERR_CH3_Msk             (0x1UL << DMA_INT_RAW_ERR_CH3_Pos)
#define DMA_INT_RAW_ERR_CH3                 DMA_INT_RAW_ERR_CH3_Msk
/** @brief DMA_INT_RAW_ERR[4]: Channel 4 raw status for IntErr interrupt */
#define DMA_INT_RAW_ERR_CH4_Pos             (4U)
#define DMA_INT_RAW_ERR_CH4_Msk             (0x1UL << DMA_INT_RAW_ERR_CH4_Pos)
#define DMA_INT_RAW_ERR_CH4                 DMA_INT_RAW_ERR_CH4_Msk
/** @brief DMA_INT_RAW_ERR[5]: Channel 5 raw status for IntErr interrupt */
#define DMA_INT_RAW_ERR_CH5_Pos             (5U)
#define DMA_INT_RAW_ERR_CH5_Msk             (0x1UL << DMA_INT_RAW_ERR_CH5_Pos)
#define DMA_INT_RAW_ERR_CH5                 DMA_INT_RAW_ERR_CH5_Msk
/** @brief DMA_INT_RAW_ERR[6]: Channel 6 raw status for IntErr interrupt */
#define DMA_INT_RAW_ERR_CH6_Pos             (6U)
#define DMA_INT_RAW_ERR_CH6_Msk             (0x1UL << DMA_INT_RAW_ERR_CH6_Pos)
#define DMA_INT_RAW_ERR_CH6                 DMA_INT_RAW_ERR_CH6_Msk
/** @brief DMA_INT_RAW_ERR[7]: Channel 7 raw status for IntErr interrupt */
#define DMA_INT_RAW_ERR_CH7_Pos             (7U)
#define DMA_INT_RAW_ERR_CH7_Msk             (0x1UL << DMA_INT_RAW_ERR_CH7_Pos)
#define DMA_INT_RAW_ERR_CH7                 DMA_INT_RAW_ERR_CH7_Msk

/***************  Bit definition for DMA_INT_STAT_TFR register  ***************/

/** @brief DMA_INT_STAT_TFR[0]: Channel 0 status for IntTfr interrupt */
#define DMA_INT_STAT_TFR_CH0_Pos            (0U)
#define DMA_INT_STAT_TFR_CH0_Msk            (0x1UL << DMA_INT_STAT_TFR_CH0_Pos)
#define DMA_INT_STAT_TFR_CH0                DMA_INT_STAT_TFR_CH0_Msk
/** @brief DMA_INT_STAT_TFR[1]: Channel 1 status for IntTfr interrupt */
#define DMA_INT_STAT_TFR_CH1_Pos            (1U)
#define DMA_INT_STAT_TFR_CH1_Msk            (0x1UL << DMA_INT_STAT_TFR_CH1_Pos)
#define DMA_INT_STAT_TFR_CH1                DMA_INT_STAT_TFR_CH1_Msk
/** @brief DMA_INT_STAT_TFR[2]: Channel 2 status for IntTfr interrupt */
#define DMA_INT_STAT_TFR_CH2_Pos            (2U)
#define DMA_INT_STAT_TFR_CH2_Msk            (0x1UL << DMA_INT_STAT_TFR_CH2_Pos)
#define DMA_INT_STAT_TFR_CH2                DMA_INT_STAT_TFR_CH2_Msk
/** @brief DMA_INT_STAT_TFR[3]: Channel 3 status for IntTfr interrupt */
#define DMA_INT_STAT_TFR_CH3_Pos            (3U)
#define DMA_INT_STAT_TFR_CH3_Msk            (0x1UL << DMA_INT_STAT_TFR_CH3_Pos)
#define DMA_INT_STAT_TFR_CH3                DMA_INT_STAT_TFR_CH3_Msk
/** @brief DMA_INT_STAT_TFR[4]: Channel 4 status for IntTfr interrupt */
#define DMA_INT_STAT_TFR_CH4_Pos            (4U)
#define DMA_INT_STAT_TFR_CH4_Msk            (0x1UL << DMA_INT_STAT_TFR_CH4_Pos)
#define DMA_INT_STAT_TFR_CH4                DMA_INT_STAT_TFR_CH4_Msk
/** @brief DMA_INT_STAT_TFR[5]: Channel 5 status for IntTfr interrupt */
#define DMA_INT_STAT_TFR_CH5_Pos            (5U)
#define DMA_INT_STAT_TFR_CH5_Msk            (0x1UL << DMA_INT_STAT_TFR_CH5_Pos)
#define DMA_INT_STAT_TFR_CH5                DMA_INT_STAT_TFR_CH5_Msk
/** @brief DMA_INT_STAT_TFR[6]: Channel 6 status for IntTfr interrupt */
#define DMA_INT_STAT_TFR_CH6_Pos            (6U)
#define DMA_INT_STAT_TFR_CH6_Msk            (0x1UL << DMA_INT_STAT_TFR_CH6_Pos)
#define DMA_INT_STAT_TFR_CH6                DMA_INT_STAT_TFR_CH6_Msk
/** @brief DMA_INT_STAT_TFR[7]: Channel 7 status for IntTfr interrupt */
#define DMA_INT_STAT_TFR_CH7_Pos            (7U)
#define DMA_INT_STAT_TFR_CH7_Msk            (0x1UL << DMA_INT_STAT_TFR_CH7_Pos)
#define DMA_INT_STAT_TFR_CH7                DMA_INT_STAT_TFR_CH7_Msk

/**************  Bit definition for DMA_INT_STAT_BLOCK register  **************/

/** @brief DMA_INT_STAT_BLOCK[0]: Channel 0 status for IntBlock interrupt */
#define DMA_INT_STAT_BLOCK_CH0_Pos          (0U)
#define DMA_INT_STAT_BLOCK_CH0_Msk          (0x1UL << DMA_INT_STAT_BLOCK_CH0_Pos)
#define DMA_INT_STAT_BLOCK_CH0              DMA_INT_STAT_BLOCK_CH0_Msk
/** @brief DMA_INT_STAT_BLOCK[1]: Channel 1 status for IntBlock interrupt */
#define DMA_INT_STAT_BLOCK_CH1_Pos          (1U)
#define DMA_INT_STAT_BLOCK_CH1_Msk          (0x1UL << DMA_INT_STAT_BLOCK_CH1_Pos)
#define DMA_INT_STAT_BLOCK_CH1              DMA_INT_STAT_BLOCK_CH1_Msk
/** @brief DMA_INT_STAT_BLOCK[2]: Channel 2 status for IntBlock interrupt */
#define DMA_INT_STAT_BLOCK_CH2_Pos          (2U)
#define DMA_INT_STAT_BLOCK_CH2_Msk          (0x1UL << DMA_INT_STAT_BLOCK_CH2_Pos)
#define DMA_INT_STAT_BLOCK_CH2              DMA_INT_STAT_BLOCK_CH2_Msk
/** @brief DMA_INT_STAT_BLOCK[3]: Channel 3 status for IntBlock interrupt */
#define DMA_INT_STAT_BLOCK_CH3_Pos          (3U)
#define DMA_INT_STAT_BLOCK_CH3_Msk          (0x1UL << DMA_INT_STAT_BLOCK_CH3_Pos)
#define DMA_INT_STAT_BLOCK_CH3              DMA_INT_STAT_BLOCK_CH3_Msk
/** @brief DMA_INT_STAT_BLOCK[4]: Channel 4 status for IntBlock interrupt */
#define DMA_INT_STAT_BLOCK_CH4_Pos          (4U)
#define DMA_INT_STAT_BLOCK_CH4_Msk          (0x1UL << DMA_INT_STAT_BLOCK_CH4_Pos)
#define DMA_INT_STAT_BLOCK_CH4              DMA_INT_STAT_BLOCK_CH4_Msk
/** @brief DMA_INT_STAT_BLOCK[5]: Channel 5 status for IntBlock interrupt */
#define DMA_INT_STAT_BLOCK_CH5_Pos          (5U)
#define DMA_INT_STAT_BLOCK_CH5_Msk          (0x1UL << DMA_INT_STAT_BLOCK_CH5_Pos)
#define DMA_INT_STAT_BLOCK_CH5              DMA_INT_STAT_BLOCK_CH5_Msk
/** @brief DMA_INT_STAT_BLOCK[6]: Channel 6 status for IntBlock interrupt */
#define DMA_INT_STAT_BLOCK_CH6_Pos          (6U)
#define DMA_INT_STAT_BLOCK_CH6_Msk          (0x1UL << DMA_INT_STAT_BLOCK_CH6_Pos)
#define DMA_INT_STAT_BLOCK_CH6              DMA_INT_STAT_BLOCK_CH6_Msk
/** @brief DMA_INT_STAT_BLOCK[7]: Channel 7 status for IntBlock interrupt */
#define DMA_INT_STAT_BLOCK_CH7_Pos          (7U)
#define DMA_INT_STAT_BLOCK_CH7_Msk          (0x1UL << DMA_INT_STAT_BLOCK_CH7_Pos)
#define DMA_INT_STAT_BLOCK_CH7              DMA_INT_STAT_BLOCK_CH7_Msk

/*************  Bit definition for DMA_INT_STAT_SRC_TRAN register  ************/

/** @brief DMA_INT_STAT_SRC_TRAN[0]: Channel 0 status for IntSrcTran interrupt */
#define DMA_INT_STAT_SRC_TRAN_CH0_Pos       (0U)
#define DMA_INT_STAT_SRC_TRAN_CH0_Msk       (0x1UL << DMA_INT_STAT_SRC_TRAN_CH0_Pos)
#define DMA_INT_STAT_SRC_TRAN_CH0           DMA_INT_STAT_SRC_TRAN_CH0_Msk
/** @brief DMA_INT_STAT_SRC_TRAN[1]: Channel 1 status for IntSrcTran interrupt */
#define DMA_INT_STAT_SRC_TRAN_CH1_Pos       (1U)
#define DMA_INT_STAT_SRC_TRAN_CH1_Msk       (0x1UL << DMA_INT_STAT_SRC_TRAN_CH1_Pos)
#define DMA_INT_STAT_SRC_TRAN_CH1           DMA_INT_STAT_SRC_TRAN_CH1_Msk
/** @brief DMA_INT_STAT_SRC_TRAN[2]: Channel 2 status for IntSrcTran interrupt */
#define DMA_INT_STAT_SRC_TRAN_CH2_Pos       (2U)
#define DMA_INT_STAT_SRC_TRAN_CH2_Msk       (0x1UL << DMA_INT_STAT_SRC_TRAN_CH2_Pos)
#define DMA_INT_STAT_SRC_TRAN_CH2           DMA_INT_STAT_SRC_TRAN_CH2_Msk
/** @brief DMA_INT_STAT_SRC_TRAN[3]: Channel 3 status for IntSrcTran interrupt */
#define DMA_INT_STAT_SRC_TRAN_CH3_Pos       (3U)
#define DMA_INT_STAT_SRC_TRAN_CH3_Msk       (0x1UL << DMA_INT_STAT_SRC_TRAN_CH3_Pos)
#define DMA_INT_STAT_SRC_TRAN_CH3           DMA_INT_STAT_SRC_TRAN_CH3_Msk
/** @brief DMA_INT_STAT_SRC_TRAN[4]: Channel 4 status for IntSrcTran interrupt */
#define DMA_INT_STAT_SRC_TRAN_CH4_Pos       (4U)
#define DMA_INT_STAT_SRC_TRAN_CH4_Msk       (0x1UL << DMA_INT_STAT_SRC_TRAN_CH4_Pos)
#define DMA_INT_STAT_SRC_TRAN_CH4           DMA_INT_STAT_SRC_TRAN_CH4_Msk
/** @brief DMA_INT_STAT_SRC_TRAN[5]: Channel 5 status for IntSrcTran interrupt */
#define DMA_INT_STAT_SRC_TRAN_CH5_Pos       (5U)
#define DMA_INT_STAT_SRC_TRAN_CH5_Msk       (0x1UL << DMA_INT_STAT_SRC_TRAN_CH5_Pos)
#define DMA_INT_STAT_SRC_TRAN_CH5           DMA_INT_STAT_SRC_TRAN_CH5_Msk
/** @brief DMA_INT_STAT_SRC_TRAN[6]: Channel 6 status for IntSrcTran interrupt */
#define DMA_INT_STAT_SRC_TRAN_CH6_Pos       (6U)
#define DMA_INT_STAT_SRC_TRAN_CH6_Msk       (0x1UL << DMA_INT_STAT_SRC_TRAN_CH6_Pos)
#define DMA_INT_STAT_SRC_TRAN_CH6           DMA_INT_STAT_SRC_TRAN_CH6_Msk
/** @brief DMA_INT_STAT_SRC_TRAN[7]: Channel 7 status for IntSrcTran interrupt */
#define DMA_INT_STAT_SRC_TRAN_CH7_Pos       (7U)
#define DMA_INT_STAT_SRC_TRAN_CH7_Msk       (0x1UL << DMA_INT_STAT_SRC_TRAN_CH7_Pos)
#define DMA_INT_STAT_SRC_TRAN_CH7           DMA_INT_STAT_SRC_TRAN_CH7_Msk

/*************  Bit definition for DMA_INT_STAT_DST_TRAN register  ************/

/** @brief DMA_INT_STAT_DST_TRAN[0]: Channel 0 status for IntDstTran interrupt */
#define DMA_INT_STAT_DST_TRAN_CH0_Pos       (0U)
#define DMA_INT_STAT_DST_TRAN_CH0_Msk       (0x1UL << DMA_INT_STAT_DST_TRAN_CH0_Pos)
#define DMA_INT_STAT_DST_TRAN_CH0           DMA_INT_STAT_DST_TRAN_CH0_Msk
/** @brief DMA_INT_STAT_DST_TRAN[1]: Channel 1 status for IntDstTran interrupt */
#define DMA_INT_STAT_DST_TRAN_CH1_Pos       (1U)
#define DMA_INT_STAT_DST_TRAN_CH1_Msk       (0x1UL << DMA_INT_STAT_DST_TRAN_CH1_Pos)
#define DMA_INT_STAT_DST_TRAN_CH1           DMA_INT_STAT_DST_TRAN_CH1_Msk
/** @brief DMA_INT_STAT_DST_TRAN[2]: Channel 2 status for IntDstTran interrupt */
#define DMA_INT_STAT_DST_TRAN_CH2_Pos       (2U)
#define DMA_INT_STAT_DST_TRAN_CH2_Msk       (0x1UL << DMA_INT_STAT_DST_TRAN_CH2_Pos)
#define DMA_INT_STAT_DST_TRAN_CH2           DMA_INT_STAT_DST_TRAN_CH2_Msk
/** @brief DMA_INT_STAT_DST_TRAN[3]: Channel 3 status for IntDstTran interrupt */
#define DMA_INT_STAT_DST_TRAN_CH3_Pos       (3U)
#define DMA_INT_STAT_DST_TRAN_CH3_Msk       (0x1UL << DMA_INT_STAT_DST_TRAN_CH3_Pos)
#define DMA_INT_STAT_DST_TRAN_CH3           DMA_INT_STAT_DST_TRAN_CH3_Msk
/** @brief DMA_INT_STAT_DST_TRAN[4]: Channel 4 status for IntDstTran interrupt */
#define DMA_INT_STAT_DST_TRAN_CH4_Pos       (4U)
#define DMA_INT_STAT_DST_TRAN_CH4_Msk       (0x1UL << DMA_INT_STAT_DST_TRAN_CH4_Pos)
#define DMA_INT_STAT_DST_TRAN_CH4           DMA_INT_STAT_DST_TRAN_CH4_Msk
/** @brief DMA_INT_STAT_DST_TRAN[5]: Channel 5 status for IntDstTran interrupt */
#define DMA_INT_STAT_DST_TRAN_CH5_Pos       (5U)
#define DMA_INT_STAT_DST_TRAN_CH5_Msk       (0x1UL << DMA_INT_STAT_DST_TRAN_CH5_Pos)
#define DMA_INT_STAT_DST_TRAN_CH5           DMA_INT_STAT_DST_TRAN_CH5_Msk
/** @brief DMA_INT_STAT_DST_TRAN[6]: Channel 6 status for IntDstTran interrupt */
#define DMA_INT_STAT_DST_TRAN_CH6_Pos       (6U)
#define DMA_INT_STAT_DST_TRAN_CH6_Msk       (0x1UL << DMA_INT_STAT_DST_TRAN_CH6_Pos)
#define DMA_INT_STAT_DST_TRAN_CH6           DMA_INT_STAT_DST_TRAN_CH6_Msk
/** @brief DMA_INT_STAT_DST_TRAN[7]: Channel 7 status for IntDstTran interrupt */
#define DMA_INT_STAT_DST_TRAN_CH7_Pos       (7U)
#define DMA_INT_STAT_DST_TRAN_CH7_Msk       (0x1UL << DMA_INT_STAT_DST_TRAN_CH7_Pos)
#define DMA_INT_STAT_DST_TRAN_CH7           DMA_INT_STAT_DST_TRAN_CH7_Msk

/***************  Bit definition for DMA_INT_STAT_ERR register  ***************/

/** @brief DMA_INT_STAT_ERR[0]: Channel 0 status for IntErr interrupt */
#define DMA_INT_STAT_ERR_CH0_Pos            (0U)
#define DMA_INT_STAT_ERR_CH0_Msk            (0x1UL << DMA_INT_STAT_ERR_CH0_Pos)
#define DMA_INT_STAT_ERR_CH0                DMA_INT_STAT_ERR_CH0_Msk
/** @brief DMA_INT_STAT_ERR[1]: Channel 1 status for IntErr interrupt */
#define DMA_INT_STAT_ERR_CH1_Pos            (1U)
#define DMA_INT_STAT_ERR_CH1_Msk            (0x1UL << DMA_INT_STAT_ERR_CH1_Pos)
#define DMA_INT_STAT_ERR_CH1                DMA_INT_STAT_ERR_CH1_Msk
/** @brief DMA_INT_STAT_ERR[2]: Channel 2 status for IntErr interrupt */
#define DMA_INT_STAT_ERR_CH2_Pos            (2U)
#define DMA_INT_STAT_ERR_CH2_Msk            (0x1UL << DMA_INT_STAT_ERR_CH2_Pos)
#define DMA_INT_STAT_ERR_CH2                DMA_INT_STAT_ERR_CH2_Msk
/** @brief DMA_INT_STAT_ERR[3]: Channel 3 status for IntErr interrupt */
#define DMA_INT_STAT_ERR_CH3_Pos            (3U)
#define DMA_INT_STAT_ERR_CH3_Msk            (0x1UL << DMA_INT_STAT_ERR_CH3_Pos)
#define DMA_INT_STAT_ERR_CH3                DMA_INT_STAT_ERR_CH3_Msk
/** @brief DMA_INT_STAT_ERR[4]: Channel 4 status for IntErr interrupt */
#define DMA_INT_STAT_ERR_CH4_Pos            (4U)
#define DMA_INT_STAT_ERR_CH4_Msk            (0x1UL << DMA_INT_STAT_ERR_CH4_Pos)
#define DMA_INT_STAT_ERR_CH4                DMA_INT_STAT_ERR_CH4_Msk
/** @brief DMA_INT_STAT_ERR[5]: Channel 5 status for IntErr interrupt */
#define DMA_INT_STAT_ERR_CH5_Pos            (5U)
#define DMA_INT_STAT_ERR_CH5_Msk            (0x1UL << DMA_INT_STAT_ERR_CH5_Pos)
#define DMA_INT_STAT_ERR_CH5                DMA_INT_STAT_ERR_CH5_Msk
/** @brief DMA_INT_STAT_ERR[6]: Channel 6 status for IntErr interrupt */
#define DMA_INT_STAT_ERR_CH6_Pos            (6U)
#define DMA_INT_STAT_ERR_CH6_Msk            (0x1UL << DMA_INT_STAT_ERR_CH6_Pos)
#define DMA_INT_STAT_ERR_CH6                DMA_INT_STAT_ERR_CH6_Msk
/** @brief DMA_INT_STAT_ERR[7]: Channel 7 status for IntErr interrupt */
#define DMA_INT_STAT_ERR_CH7_Pos            (7U)
#define DMA_INT_STAT_ERR_CH7_Msk            (0x1UL << DMA_INT_STAT_ERR_CH7_Pos)
#define DMA_INT_STAT_ERR_CH7                DMA_INT_STAT_ERR_CH7_Msk

/***************  Bit definition for DMA_INT_MASK_TFR register  ***************/

/** @brief DMA_INT_MASK_TFR[0]: Channel 0 mask for IntTfr interrupt */
#define DMA_INT_MASK_TFR_CH0_Pos            (0U)
#define DMA_INT_MASK_TFR_CH0_Msk            (0x1UL << DMA_INT_MASK_TFR_CH0_Pos)
#define DMA_INT_MASK_TFR_CH0                DMA_INT_MASK_TFR_CH0_Msk
/** @brief DMA_INT_MASK_TFR[1]: Channel 1 mask for IntTfr interrupt */
#define DMA_INT_MASK_TFR_CH1_Pos            (1U)
#define DMA_INT_MASK_TFR_CH1_Msk            (0x1UL << DMA_INT_MASK_TFR_CH1_Pos)
#define DMA_INT_MASK_TFR_CH1                DMA_INT_MASK_TFR_CH1_Msk
/** @brief DMA_INT_MASK_TFR[2]: Channel 2 mask for IntTfr interrupt */
#define DMA_INT_MASK_TFR_CH2_Pos            (2U)
#define DMA_INT_MASK_TFR_CH2_Msk            (0x1UL << DMA_INT_MASK_TFR_CH2_Pos)
#define DMA_INT_MASK_TFR_CH2                DMA_INT_MASK_TFR_CH2_Msk
/** @brief DMA_INT_MASK_TFR[3]: Channel 3 mask for IntTfr interrupt */
#define DMA_INT_MASK_TFR_CH3_Pos            (3U)
#define DMA_INT_MASK_TFR_CH3_Msk            (0x1UL << DMA_INT_MASK_TFR_CH3_Pos)
#define DMA_INT_MASK_TFR_CH3                DMA_INT_MASK_TFR_CH3_Msk
/** @brief DMA_INT_MASK_TFR[4]: Channel 4 mask for IntTfr interrupt */
#define DMA_INT_MASK_TFR_CH4_Pos            (4U)
#define DMA_INT_MASK_TFR_CH4_Msk            (0x1UL << DMA_INT_MASK_TFR_CH4_Pos)
#define DMA_INT_MASK_TFR_CH4                DMA_INT_MASK_TFR_CH4_Msk
/** @brief DMA_INT_MASK_TFR[5]: Channel 5 mask for IntTfr interrupt */
#define DMA_INT_MASK_TFR_CH5_Pos            (5U)
#define DMA_INT_MASK_TFR_CH5_Msk            (0x1UL << DMA_INT_MASK_TFR_CH5_Pos)
#define DMA_INT_MASK_TFR_CH5                DMA_INT_MASK_TFR_CH5_Msk
/** @brief DMA_INT_MASK_TFR[6]: Channel 6 mask for IntTfr interrupt */
#define DMA_INT_MASK_TFR_CH6_Pos            (6U)
#define DMA_INT_MASK_TFR_CH6_Msk            (0x1UL << DMA_INT_MASK_TFR_CH6_Pos)
#define DMA_INT_MASK_TFR_CH6                DMA_INT_MASK_TFR_CH6_Msk
/** @brief DMA_INT_MASK_TFR[7]: Channel 7 mask for IntTfr interrupt */
#define DMA_INT_MASK_TFR_CH7_Pos            (7U)
#define DMA_INT_MASK_TFR_CH7_Msk            (0x1UL << DMA_INT_MASK_TFR_CH7_Pos)
#define DMA_INT_MASK_TFR_CH7                DMA_INT_MASK_TFR_CH7_Msk
/** @brief DMA_INT_MASK_TFR_WE[8]: Channel 0 mask write enable for IntTfr interrupt */
#define DMA_INT_MASK_TFR_WE_CH0_Pos         (8U)
#define DMA_INT_MASK_TFR_WE_CH0_Msk         (0x1UL << DMA_INT_MASK_TFR_WE_CH0_Pos)
#define DMA_INT_MASK_TFR_WE_CH0             DMA_INT_MASK_TFR_WE_CH0_Msk
/** @brief DMA_INT_MASK_TFR_WE[9]: Channel 1 mask write enable for IntTfr interrupt */
#define DMA_INT_MASK_TFR_WE_CH1_Pos         (9U)
#define DMA_INT_MASK_TFR_WE_CH1_Msk         (0x1UL << DMA_INT_MASK_TFR_WE_CH1_Pos)
#define DMA_INT_MASK_TFR_WE_CH1             DMA_INT_MASK_TFR_WE_CH1_Msk
/** @brief DMA_INT_MASK_TFR_WE[10]: Channel 2 mask write enable for IntTfr interrupt */
#define DMA_INT_MASK_TFR_WE_CH2_Pos         (10U)
#define DMA_INT_MASK_TFR_WE_CH2_Msk         (0x1UL << DMA_INT_MASK_TFR_WE_CH2_Pos)
#define DMA_INT_MASK_TFR_WE_CH2             DMA_INT_MASK_TFR_WE_CH2_Msk
/** @brief DMA_INT_MASK_TFR_WE[11]: Channel 3 mask write enable for IntTfr interrupt */
#define DMA_INT_MASK_TFR_WE_CH3_Pos         (11U)
#define DMA_INT_MASK_TFR_WE_CH3_Msk         (0x1UL << DMA_INT_MASK_TFR_WE_CH3_Pos)
#define DMA_INT_MASK_TFR_WE_CH3             DMA_INT_MASK_TFR_WE_CH3_Msk
/** @brief DMA_INT_MASK_TFR_WE[12]: Channel 4 mask write enable for IntTfr interrupt */
#define DMA_INT_MASK_TFR_WE_CH4_Pos         (12U)
#define DMA_INT_MASK_TFR_WE_CH4_Msk         (0x1UL << DMA_INT_MASK_TFR_WE_CH4_Pos)
#define DMA_INT_MASK_TFR_WE_CH4             DMA_INT_MASK_TFR_WE_CH4_Msk
/** @brief DMA_INT_MASK_TFR_WE[13]: Channel 5 mask write enable for IntTfr interrupt */
#define DMA_INT_MASK_TFR_WE_CH5_Pos         (13U)
#define DMA_INT_MASK_TFR_WE_CH5_Msk         (0x1UL << DMA_INT_MASK_TFR_WE_CH5_Pos)
#define DMA_INT_MASK_TFR_WE_CH5             DMA_INT_MASK_TFR_WE_CH5_Msk
/** @brief DMA_INT_MASK_TFR_WE[14]: Channel 6 mask write enable for IntTfr interrupt */
#define DMA_INT_MASK_TFR_WE_CH6_Pos         (14U)
#define DMA_INT_MASK_TFR_WE_CH6_Msk         (0x1UL << DMA_INT_MASK_TFR_WE_CH6_Pos)
#define DMA_INT_MASK_TFR_WE_CH6             DMA_INT_MASK_TFR_WE_CH6_Msk
/** @brief DMA_INT_MASK_TFR_WE[15]: Channel 7 mask write enable for IntTfr interrupt */
#define DMA_INT_MASK_TFR_WE_CH7_Pos         (15U)
#define DMA_INT_MASK_TFR_WE_CH7_Msk         (0x1UL << DMA_INT_MASK_TFR_WE_CH7_Pos)
#define DMA_INT_MASK_TFR_WE_CH7             DMA_INT_MASK_TFR_WE_CH7_Msk

/**************  Bit definition for DMA_INT_MASK_BLOCK register  **************/

/** @brief DMA_INT_MASK_BLOCK[0]: Channel 0 mask for IntBlock interrupt */
#define DMA_INT_MASK_BLOCK_CH0_Pos          (0U)
#define DMA_INT_MASK_BLOCK_CH0_Msk          (0x1UL << DMA_INT_MASK_BLOCK_CH0_Pos)
#define DMA_INT_MASK_BLOCK_CH0              DMA_INT_MASK_BLOCK_CH0_Msk
/** @brief DMA_INT_MASK_BLOCK[1]: Channel 1 mask for IntBlock interrupt */
#define DMA_INT_MASK_BLOCK_CH1_Pos          (1U)
#define DMA_INT_MASK_BLOCK_CH1_Msk          (0x1UL << DMA_INT_MASK_BLOCK_CH1_Pos)
#define DMA_INT_MASK_BLOCK_CH1              DMA_INT_MASK_BLOCK_CH1_Msk
/** @brief DMA_INT_MASK_BLOCK[2]: Channel 2 mask for IntBlock interrupt */
#define DMA_INT_MASK_BLOCK_CH2_Pos          (2U)
#define DMA_INT_MASK_BLOCK_CH2_Msk          (0x1UL << DMA_INT_MASK_BLOCK_CH2_Pos)
#define DMA_INT_MASK_BLOCK_CH2              DMA_INT_MASK_BLOCK_CH2_Msk
/** @brief DMA_INT_MASK_BLOCK[3]: Channel 3 mask for IntBlock interrupt */
#define DMA_INT_MASK_BLOCK_CH3_Pos          (3U)
#define DMA_INT_MASK_BLOCK_CH3_Msk          (0x1UL << DMA_INT_MASK_BLOCK_CH3_Pos)
#define DMA_INT_MASK_BLOCK_CH3              DMA_INT_MASK_BLOCK_CH3_Msk
/** @brief DMA_INT_MASK_BLOCK[4]: Channel 4 mask for IntBlock interrupt */
#define DMA_INT_MASK_BLOCK_CH4_Pos          (4U)
#define DMA_INT_MASK_BLOCK_CH4_Msk          (0x1UL << DMA_INT_MASK_BLOCK_CH4_Pos)
#define DMA_INT_MASK_BLOCK_CH4              DMA_INT_MASK_BLOCK_CH4_Msk
/** @brief DMA_INT_MASK_BLOCK[5]: Channel 5 mask for IntBlock interrupt */
#define DMA_INT_MASK_BLOCK_CH5_Pos          (5U)
#define DMA_INT_MASK_BLOCK_CH5_Msk          (0x1UL << DMA_INT_MASK_BLOCK_CH5_Pos)
#define DMA_INT_MASK_BLOCK_CH5              DMA_INT_MASK_BLOCK_CH5_Msk
/** @brief DMA_INT_MASK_BLOCK[6]: Channel 6 mask for IntBlock interrupt */
#define DMA_INT_MASK_BLOCK_CH6_Pos          (6U)
#define DMA_INT_MASK_BLOCK_CH6_Msk          (0x1UL << DMA_INT_MASK_BLOCK_CH6_Pos)
#define DMA_INT_MASK_BLOCK_CH6              DMA_INT_MASK_BLOCK_CH6_Msk
/** @brief DMA_INT_MASK_BLOCK[7]: Channel 7 mask for IntBlock interrupt */
#define DMA_INT_MASK_BLOCK_CH7_Pos          (7U)
#define DMA_INT_MASK_BLOCK_CH7_Msk          (0x1UL << DMA_INT_MASK_BLOCK_CH7_Pos)
#define DMA_INT_MASK_BLOCK_CH7              DMA_INT_MASK_BLOCK_CH7_Msk
/** @brief DMA_INT_MASK_BLOCK_WE[8]: Channel 0 mask write enable for IntBlock interrupt */
#define DMA_INT_MASK_BLOCK_WE_CH0_Pos       (8U)
#define DMA_INT_MASK_BLOCK_WE_CH0_Msk       (0x1UL << DMA_INT_MASK_BLOCK_WE_CH0_Pos)
#define DMA_INT_MASK_BLOCK_WE_CH0           DMA_INT_MASK_BLOCK_WE_CH0_Msk
/** @brief DMA_INT_MASK_BLOCK_WE[9]: Channel 1 mask write enable for IntBlock interrupt */
#define DMA_INT_MASK_BLOCK_WE_CH1_Pos       (9U)
#define DMA_INT_MASK_BLOCK_WE_CH1_Msk       (0x1UL << DMA_INT_MASK_BLOCK_WE_CH1_Pos)
#define DMA_INT_MASK_BLOCK_WE_CH1           DMA_INT_MASK_BLOCK_WE_CH1_Msk
/** @brief DMA_INT_MASK_BLOCK_WE[10]: Channel 2 mask write enable for IntBlock interrupt */
#define DMA_INT_MASK_BLOCK_WE_CH2_Pos       (10U)
#define DMA_INT_MASK_BLOCK_WE_CH2_Msk       (0x1UL << DMA_INT_MASK_BLOCK_WE_CH2_Pos)
#define DMA_INT_MASK_BLOCK_WE_CH2           DMA_INT_MASK_BLOCK_WE_CH2_Msk
/** @brief DMA_INT_MASK_BLOCK_WE[11]: Channel 3 mask write enable for IntBlock interrupt */
#define DMA_INT_MASK_BLOCK_WE_CH3_Pos       (11U)
#define DMA_INT_MASK_BLOCK_WE_CH3_Msk       (0x1UL << DMA_INT_MASK_BLOCK_WE_CH3_Pos)
#define DMA_INT_MASK_BLOCK_WE_CH3           DMA_INT_MASK_BLOCK_WE_CH3_Msk
/** @brief DMA_INT_MASK_BLOCK_WE[12]: Channel 4 mask write enable for IntBlock interrupt */
#define DMA_INT_MASK_BLOCK_WE_CH4_Pos       (12U)
#define DMA_INT_MASK_BLOCK_WE_CH4_Msk       (0x1UL << DMA_INT_MASK_BLOCK_WE_CH4_Pos)
#define DMA_INT_MASK_BLOCK_WE_CH4           DMA_INT_MASK_BLOCK_WE_CH4_Msk
/** @brief DMA_INT_MASK_BLOCK_WE[13]: Channel 5 mask write enable for IntBlock interrupt */
#define DMA_INT_MASK_BLOCK_WE_CH5_Pos       (13U)
#define DMA_INT_MASK_BLOCK_WE_CH5_Msk       (0x1UL << DMA_INT_MASK_BLOCK_WE_CH5_Pos)
#define DMA_INT_MASK_BLOCK_WE_CH5           DMA_INT_MASK_BLOCK_WE_CH5_Msk
/** @brief DMA_INT_MASK_BLOCK_WE[14]: Channel 6 mask write enable for IntBlock interrupt */
#define DMA_INT_MASK_BLOCK_WE_CH6_Pos       (14U)
#define DMA_INT_MASK_BLOCK_WE_CH6_Msk       (0x1UL << DMA_INT_MASK_BLOCK_WE_CH6_Pos)
#define DMA_INT_MASK_BLOCK_WE_CH6           DMA_INT_MASK_BLOCK_WE_CH6_Msk
/** @brief DMA_INT_MASK_BLOCK_WE[15]: Channel 7 mask write enable for IntBlock interrupt */
#define DMA_INT_MASK_BLOCK_WE_CH7_Pos       (15U)
#define DMA_INT_MASK_BLOCK_WE_CH7_Msk       (0x1UL << DMA_INT_MASK_BLOCK_WE_CH7_Pos)
#define DMA_INT_MASK_BLOCK_WE_CH7           DMA_INT_MASK_BLOCK_WE_CH7_Msk

/*************  Bit definition for DMA_INT_MASK_SRC_TRAN register  ************/

/** @brief DMA_INT_MASK_SRC_TRAN[0]: Channel 0 mask for IntSrcTran interrupt */
#define DMA_INT_MASK_SRC_TRAN_CH0_Pos       (0U)
#define DMA_INT_MASK_SRC_TRAN_CH0_Msk       (0x1UL << DMA_INT_MASK_SRC_TRAN_CH0_Pos)
#define DMA_INT_MASK_SRC_TRAN_CH0           DMA_INT_MASK_SRC_TRAN_CH0_Msk
/** @brief DMA_INT_MASK_SRC_TRAN[1]: Channel 1 mask for IntSrcTran interrupt */
#define DMA_INT_MASK_SRC_TRAN_CH1_Pos       (1U)
#define DMA_INT_MASK_SRC_TRAN_CH1_Msk       (0x1UL << DMA_INT_MASK_SRC_TRAN_CH1_Pos)
#define DMA_INT_MASK_SRC_TRAN_CH1           DMA_INT_MASK_SRC_TRAN_CH1_Msk
/** @brief DMA_INT_MASK_SRC_TRAN[2]: Channel 2 mask for IntSrcTran interrupt */
#define DMA_INT_MASK_SRC_TRAN_CH2_Pos       (2U)
#define DMA_INT_MASK_SRC_TRAN_CH2_Msk       (0x1UL << DMA_INT_MASK_SRC_TRAN_CH2_Pos)
#define DMA_INT_MASK_SRC_TRAN_CH2           DMA_INT_MASK_SRC_TRAN_CH2_Msk
/** @brief DMA_INT_MASK_SRC_TRAN[3]: Channel 3 mask for IntSrcTran interrupt */
#define DMA_INT_MASK_SRC_TRAN_CH3_Pos       (3U)
#define DMA_INT_MASK_SRC_TRAN_CH3_Msk       (0x1UL << DMA_INT_MASK_SRC_TRAN_CH3_Pos)
#define DMA_INT_MASK_SRC_TRAN_CH3           DMA_INT_MASK_SRC_TRAN_CH3_Msk
/** @brief DMA_INT_MASK_SRC_TRAN[4]: Channel 4 mask for IntSrcTran interrupt */
#define DMA_INT_MASK_SRC_TRAN_CH4_Pos       (4U)
#define DMA_INT_MASK_SRC_TRAN_CH4_Msk       (0x1UL << DMA_INT_MASK_SRC_TRAN_CH4_Pos)
#define DMA_INT_MASK_SRC_TRAN_CH4           DMA_INT_MASK_SRC_TRAN_CH4_Msk
/** @brief DMA_INT_MASK_SRC_TRAN[5]: Channel 5 mask for IntSrcTran interrupt */
#define DMA_INT_MASK_SRC_TRAN_CH5_Pos       (5U)
#define DMA_INT_MASK_SRC_TRAN_CH5_Msk       (0x1UL << DMA_INT_MASK_SRC_TRAN_CH5_Pos)
#define DMA_INT_MASK_SRC_TRAN_CH5           DMA_INT_MASK_SRC_TRAN_CH5_Msk
/** @brief DMA_INT_MASK_SRC_TRAN[6]: Channel 6 mask for IntSrcTran interrupt */
#define DMA_INT_MASK_SRC_TRAN_CH6_Pos       (6U)
#define DMA_INT_MASK_SRC_TRAN_CH6_Msk       (0x1UL << DMA_INT_MASK_SRC_TRAN_CH6_Pos)
#define DMA_INT_MASK_SRC_TRAN_CH6           DMA_INT_MASK_SRC_TRAN_CH6_Msk
/** @brief DMA_INT_MASK_SRC_TRAN[7]: Channel 7 mask for IntSrcTran interrupt */
#define DMA_INT_MASK_SRC_TRAN_CH7_Pos       (7U)
#define DMA_INT_MASK_SRC_TRAN_CH7_Msk       (0x1UL << DMA_INT_MASK_SRC_TRAN_CH7_Pos)
#define DMA_INT_MASK_SRC_TRAN_CH7           DMA_INT_MASK_SRC_TRAN_CH7_Msk
/** @brief DMA_INT_MASK_SRC_TRAN_WE[8]: Channel 0 mask write enable for IntSrcTran interrupt */
#define DMA_INT_MASK_SRC_TRAN_WE_CH0_Pos    (8U)
#define DMA_INT_MASK_SRC_TRAN_WE_CH0_Msk    (0x1UL << DMA_INT_MASK_SRC_TRAN_WE_CH0_Pos)
#define DMA_INT_MASK_SRC_TRAN_WE_CH0        DMA_INT_MASK_SRC_TRAN_WE_CH0_Msk
/** @brief DMA_INT_MASK_SRC_TRAN_WE[9]: Channel 1 mask write enable for IntSrcTran interrupt */
#define DMA_INT_MASK_SRC_TRAN_WE_CH1_Pos    (9U)
#define DMA_INT_MASK_SRC_TRAN_WE_CH1_Msk    (0x1UL << DMA_INT_MASK_SRC_TRAN_WE_CH1_Pos)
#define DMA_INT_MASK_SRC_TRAN_WE_CH1        DMA_INT_MASK_SRC_TRAN_WE_CH1_Msk
/** @brief DMA_INT_MASK_SRC_TRAN_WE[10]: Channel 2 mask write enable for IntSrcTran interrupt */
#define DMA_INT_MASK_SRC_TRAN_WE_CH2_Pos    (10U)
#define DMA_INT_MASK_SRC_TRAN_WE_CH2_Msk    (0x1UL << DMA_INT_MASK_SRC_TRAN_WE_CH2_Pos)
#define DMA_INT_MASK_SRC_TRAN_WE_CH2        DMA_INT_MASK_SRC_TRAN_WE_CH2_Msk
/** @brief DMA_INT_MASK_SRC_TRAN_WE[11]: Channel 3 mask write enable for IntSrcTran interrupt */
#define DMA_INT_MASK_SRC_TRAN_WE_CH3_Pos    (11U)
#define DMA_INT_MASK_SRC_TRAN_WE_CH3_Msk    (0x1UL << DMA_INT_MASK_SRC_TRAN_WE_CH3_Pos)
#define DMA_INT_MASK_SRC_TRAN_WE_CH3        DMA_INT_MASK_SRC_TRAN_WE_CH3_Msk
/** @brief DMA_INT_MASK_SRC_TRAN_WE[12]: Channel 4 mask write enable for IntSrcTran interrupt */
#define DMA_INT_MASK_SRC_TRAN_WE_CH4_Pos    (12U)
#define DMA_INT_MASK_SRC_TRAN_WE_CH4_Msk    (0x1UL << DMA_INT_MASK_SRC_TRAN_WE_CH4_Pos)
#define DMA_INT_MASK_SRC_TRAN_WE_CH4        DMA_INT_MASK_SRC_TRAN_WE_CH4_Msk
/** @brief DMA_INT_MASK_SRC_TRAN_WE[13]: Channel 5 mask write enable for IntSrcTran interrupt */
#define DMA_INT_MASK_SRC_TRAN_WE_CH5_Pos    (13U)
#define DMA_INT_MASK_SRC_TRAN_WE_CH5_Msk    (0x1UL << DMA_INT_MASK_SRC_TRAN_WE_CH5_Pos)
#define DMA_INT_MASK_SRC_TRAN_WE_CH5        DMA_INT_MASK_SRC_TRAN_WE_CH5_Msk
/** @brief DMA_INT_MASK_SRC_TRAN_WE[14]: Channel 6 mask write enable for IntSrcTran interrupt */
#define DMA_INT_MASK_SRC_TRAN_WE_CH6_Pos    (14U)
#define DMA_INT_MASK_SRC_TRAN_WE_CH6_Msk    (0x1UL << DMA_INT_MASK_SRC_TRAN_WE_CH6_Pos)
#define DMA_INT_MASK_SRC_TRAN_WE_CH6        DMA_INT_MASK_SRC_TRAN_WE_CH6_Msk
/** @brief DMA_INT_MASK_SRC_TRAN_WE[15]: Channel 7 mask write enable for IntSrcTran interrupt */
#define DMA_INT_MASK_SRC_TRAN_WE_CH7_Pos    (15U)
#define DMA_INT_MASK_SRC_TRAN_WE_CH7_Msk    (0x1UL << DMA_INT_MASK_SRC_TRAN_WE_CH7_Pos)
#define DMA_INT_MASK_SRC_TRAN_WE_CH7        DMA_INT_MASK_SRC_TRAN_WE_CH7_Msk

/*************  Bit definition for DMA_INT_MASK_DST_TRAN register  ************/

/** @brief DMA_INT_MASK_DST_TRAN[0]: Channel 0 mask for IntDstTran interrupt */
#define DMA_INT_MASK_DST_TRAN_CH0_Pos       (0U)
#define DMA_INT_MASK_DST_TRAN_CH0_Msk       (0x1UL << DMA_INT_MASK_DST_TRAN_CH0_Pos)
#define DMA_INT_MASK_DST_TRAN_CH0           DMA_INT_MASK_DST_TRAN_CH0_Msk
/** @brief DMA_INT_MASK_DST_TRAN[1]: Channel 1 mask for IntDstTran interrupt */
#define DMA_INT_MASK_DST_TRAN_CH1_Pos       (1U)
#define DMA_INT_MASK_DST_TRAN_CH1_Msk       (0x1UL << DMA_INT_MASK_DST_TRAN_CH1_Pos)
#define DMA_INT_MASK_DST_TRAN_CH1           DMA_INT_MASK_DST_TRAN_CH1_Msk
/** @brief DMA_INT_MASK_DST_TRAN[2]: Channel 2 mask for IntDstTran interrupt */
#define DMA_INT_MASK_DST_TRAN_CH2_Pos       (2U)
#define DMA_INT_MASK_DST_TRAN_CH2_Msk       (0x1UL << DMA_INT_MASK_DST_TRAN_CH2_Pos)
#define DMA_INT_MASK_DST_TRAN_CH2           DMA_INT_MASK_DST_TRAN_CH2_Msk
/** @brief DMA_INT_MASK_DST_TRAN[3]: Channel 3 mask for IntDstTran interrupt */
#define DMA_INT_MASK_DST_TRAN_CH3_Pos       (3U)
#define DMA_INT_MASK_DST_TRAN_CH3_Msk       (0x1UL << DMA_INT_MASK_DST_TRAN_CH3_Pos)
#define DMA_INT_MASK_DST_TRAN_CH3           DMA_INT_MASK_DST_TRAN_CH3_Msk
/** @brief DMA_INT_MASK_DST_TRAN[4]: Channel 4 mask for IntDstTran interrupt */
#define DMA_INT_MASK_DST_TRAN_CH4_Pos       (4U)
#define DMA_INT_MASK_DST_TRAN_CH4_Msk       (0x1UL << DMA_INT_MASK_DST_TRAN_CH4_Pos)
#define DMA_INT_MASK_DST_TRAN_CH4           DMA_INT_MASK_DST_TRAN_CH4_Msk
/** @brief DMA_INT_MASK_DST_TRAN[5]: Channel 5 mask for IntDstTran interrupt */
#define DMA_INT_MASK_DST_TRAN_CH5_Pos       (5U)
#define DMA_INT_MASK_DST_TRAN_CH5_Msk       (0x1UL << DMA_INT_MASK_DST_TRAN_CH5_Pos)
#define DMA_INT_MASK_DST_TRAN_CH5           DMA_INT_MASK_DST_TRAN_CH5_Msk
/** @brief DMA_INT_MASK_DST_TRAN[6]: Channel 6 mask for IntDstTran interrupt */
#define DMA_INT_MASK_DST_TRAN_CH6_Pos       (6U)
#define DMA_INT_MASK_DST_TRAN_CH6_Msk       (0x1UL << DMA_INT_MASK_DST_TRAN_CH6_Pos)
#define DMA_INT_MASK_DST_TRAN_CH6           DMA_INT_MASK_DST_TRAN_CH6_Msk
/** @brief DMA_INT_MASK_DST_TRAN[7]: Channel 7 mask for IntDstTran interrupt */
#define DMA_INT_MASK_DST_TRAN_CH7_Pos       (7U)
#define DMA_INT_MASK_DST_TRAN_CH7_Msk       (0x1UL << DMA_INT_MASK_DST_TRAN_CH7_Pos)
#define DMA_INT_MASK_DST_TRAN_CH7           DMA_INT_MASK_DST_TRAN_CH7_Msk
/** @brief DMA_INT_MASK_DST_TRAN_WE[8]: Channel 0 mask write enable for IntDstTran interrupt */
#define DMA_INT_MASK_DST_TRAN_WE_CH0_Pos    (8U)
#define DMA_INT_MASK_DST_TRAN_WE_CH0_Msk    (0x1UL << DMA_INT_MASK_DST_TRAN_WE_CH0_Pos)
#define DMA_INT_MASK_DST_TRAN_WE_CH0        DMA_INT_MASK_DST_TRAN_WE_CH0_Msk
/** @brief DMA_INT_MASK_DST_TRAN_WE[9]: Channel 1 mask write enable for IntDstTran interrupt */
#define DMA_INT_MASK_DST_TRAN_WE_CH1_Pos    (9U)
#define DMA_INT_MASK_DST_TRAN_WE_CH1_Msk    (0x1UL << DMA_INT_MASK_DST_TRAN_WE_CH1_Pos)
#define DMA_INT_MASK_DST_TRAN_WE_CH1        DMA_INT_MASK_DST_TRAN_WE_CH1_Msk
/** @brief DMA_INT_MASK_DST_TRAN_WE[10]: Channel 2 mask write enable for IntDstTran interrupt */
#define DMA_INT_MASK_DST_TRAN_WE_CH2_Pos    (10U)
#define DMA_INT_MASK_DST_TRAN_WE_CH2_Msk    (0x1UL << DMA_INT_MASK_DST_TRAN_WE_CH2_Pos)
#define DMA_INT_MASK_DST_TRAN_WE_CH2        DMA_INT_MASK_DST_TRAN_WE_CH2_Msk
/** @brief DMA_INT_MASK_DST_TRAN_WE[11]: Channel 3 mask write enable for IntDstTran interrupt */
#define DMA_INT_MASK_DST_TRAN_WE_CH3_Pos    (11U)
#define DMA_INT_MASK_DST_TRAN_WE_CH3_Msk    (0x1UL << DMA_INT_MASK_DST_TRAN_WE_CH3_Pos)
#define DMA_INT_MASK_DST_TRAN_WE_CH3        DMA_INT_MASK_DST_TRAN_WE_CH3_Msk
/** @brief DMA_INT_MASK_DST_TRAN_WE[12]: Channel 4 mask write enable for IntDstTran interrupt */
#define DMA_INT_MASK_DST_TRAN_WE_CH4_Pos    (12U)
#define DMA_INT_MASK_DST_TRAN_WE_CH4_Msk    (0x1UL << DMA_INT_MASK_DST_TRAN_WE_CH4_Pos)
#define DMA_INT_MASK_DST_TRAN_WE_CH4        DMA_INT_MASK_DST_TRAN_WE_CH4_Msk
/** @brief DMA_INT_MASK_DST_TRAN_WE[13]: Channel 5 mask write enable for IntDstTran interrupt */
#define DMA_INT_MASK_DST_TRAN_WE_CH5_Pos    (13U)
#define DMA_INT_MASK_DST_TRAN_WE_CH5_Msk    (0x1UL << DMA_INT_MASK_DST_TRAN_WE_CH5_Pos)
#define DMA_INT_MASK_DST_TRAN_WE_CH5        DMA_INT_MASK_DST_TRAN_WE_CH5_Msk
/** @brief DMA_INT_MASK_DST_TRAN_WE[14]: Channel 6 mask write enable for IntDstTran interrupt */
#define DMA_INT_MASK_DST_TRAN_WE_CH6_Pos    (14U)
#define DMA_INT_MASK_DST_TRAN_WE_CH6_Msk    (0x1UL << DMA_INT_MASK_DST_TRAN_WE_CH6_Pos)
#define DMA_INT_MASK_DST_TRAN_WE_CH6        DMA_INT_MASK_DST_TRAN_WE_CH6_Msk
/** @brief DMA_INT_MASK_DST_TRAN_WE[15]: Channel 7 mask write enable for IntDstTran interrupt */
#define DMA_INT_MASK_DST_TRAN_WE_CH7_Pos    (15U)
#define DMA_INT_MASK_DST_TRAN_WE_CH7_Msk    (0x1UL << DMA_INT_MASK_DST_TRAN_WE_CH7_Pos)
#define DMA_INT_MASK_DST_TRAN_WE_CH7        DMA_INT_MASK_DST_TRAN_WE_CH7_Msk

/***************  Bit definition for DMA_INT_MASK_ERR register  ***************/

/** @brief DMA_INT_MASK_ERR[0]: Channel 0 mask for IntErr interrupt */
#define DMA_INT_MASK_ERR_CH0_Pos            (0U)
#define DMA_INT_MASK_ERR_CH0_Msk            (0x1UL << DMA_INT_MASK_ERR_CH0_Pos)
#define DMA_INT_MASK_ERR_CH0                DMA_INT_MASK_ERR_CH0_Msk
/** @brief DMA_INT_MASK_ERR[1]: Channel 1 mask for IntErr interrupt */
#define DMA_INT_MASK_ERR_CH1_Pos            (1U)
#define DMA_INT_MASK_ERR_CH1_Msk            (0x1UL << DMA_INT_MASK_ERR_CH1_Pos)
#define DMA_INT_MASK_ERR_CH1                DMA_INT_MASK_ERR_CH1_Msk
/** @brief DMA_INT_MASK_ERR[2]: Channel 2 mask for IntErr interrupt */
#define DMA_INT_MASK_ERR_CH2_Pos            (2U)
#define DMA_INT_MASK_ERR_CH2_Msk            (0x1UL << DMA_INT_MASK_ERR_CH2_Pos)
#define DMA_INT_MASK_ERR_CH2                DMA_INT_MASK_ERR_CH2_Msk
/** @brief DMA_INT_MASK_ERR[3]: Channel 3 mask for IntErr interrupt */
#define DMA_INT_MASK_ERR_CH3_Pos            (3U)
#define DMA_INT_MASK_ERR_CH3_Msk            (0x1UL << DMA_INT_MASK_ERR_CH3_Pos)
#define DMA_INT_MASK_ERR_CH3                DMA_INT_MASK_ERR_CH3_Msk
/** @brief DMA_INT_MASK_ERR[4]: Channel 4 mask for IntErr interrupt */
#define DMA_INT_MASK_ERR_CH4_Pos            (4U)
#define DMA_INT_MASK_ERR_CH4_Msk            (0x1UL << DMA_INT_MASK_ERR_CH4_Pos)
#define DMA_INT_MASK_ERR_CH4                DMA_INT_MASK_ERR_CH4_Msk
/** @brief DMA_INT_MASK_ERR[5]: Channel 5 mask for IntErr interrupt */
#define DMA_INT_MASK_ERR_CH5_Pos            (5U)
#define DMA_INT_MASK_ERR_CH5_Msk            (0x1UL << DMA_INT_MASK_ERR_CH5_Pos)
#define DMA_INT_MASK_ERR_CH5                DMA_INT_MASK_ERR_CH5_Msk
/** @brief DMA_INT_MASK_ERR[6]: Channel 6 mask for IntErr interrupt */
#define DMA_INT_MASK_ERR_CH6_Pos            (6U)
#define DMA_INT_MASK_ERR_CH6_Msk            (0x1UL << DMA_INT_MASK_ERR_CH6_Pos)
#define DMA_INT_MASK_ERR_CH6                DMA_INT_MASK_ERR_CH6_Msk
/** @brief DMA_INT_MASK_ERR[7]: Channel 7 mask for IntErr interrupt */
#define DMA_INT_MASK_ERR_CH7_Pos            (7U)
#define DMA_INT_MASK_ERR_CH7_Msk            (0x1UL << DMA_INT_MASK_ERR_CH7_Pos)
#define DMA_INT_MASK_ERR_CH7                DMA_INT_MASK_ERR_CH7_Msk
/** @brief DMA_INT_MASK_ERR_WE[8]: Channel 0 mask write enable for IntErr interrupt */
#define DMA_INT_MASK_ERR_WE_CH0_Pos         (8U)
#define DMA_INT_MASK_ERR_WE_CH0_Msk         (0x1UL << DMA_INT_MASK_ERR_WE_CH0_Pos)
#define DMA_INT_MASK_ERR_WE_CH0             DMA_INT_MASK_ERR_WE_CH0_Msk
/** @brief DMA_INT_MASK_ERR_WE[9]: Channel 1 mask write enable for IntErr interrupt */
#define DMA_INT_MASK_ERR_WE_CH1_Pos         (9U)
#define DMA_INT_MASK_ERR_WE_CH1_Msk         (0x1UL << DMA_INT_MASK_ERR_WE_CH1_Pos)
#define DMA_INT_MASK_ERR_WE_CH1             DMA_INT_MASK_ERR_WE_CH1_Msk
/** @brief DMA_INT_MASK_ERR_WE[10]: Channel 2 mask write enable for IntErr interrupt */
#define DMA_INT_MASK_ERR_WE_CH2_Pos         (10U)
#define DMA_INT_MASK_ERR_WE_CH2_Msk         (0x1UL << DMA_INT_MASK_ERR_WE_CH2_Pos)
#define DMA_INT_MASK_ERR_WE_CH2             DMA_INT_MASK_ERR_WE_CH2_Msk
/** @brief DMA_INT_MASK_ERR_WE[11]: Channel 3 mask write enable for IntErr interrupt */
#define DMA_INT_MASK_ERR_WE_CH3_Pos         (11U)
#define DMA_INT_MASK_ERR_WE_CH3_Msk         (0x1UL << DMA_INT_MASK_ERR_WE_CH3_Pos)
#define DMA_INT_MASK_ERR_WE_CH3             DMA_INT_MASK_ERR_WE_CH3_Msk
/** @brief DMA_INT_MASK_ERR_WE[12]: Channel 4 mask write enable for IntErr interrupt */
#define DMA_INT_MASK_ERR_WE_CH4_Pos         (12U)
#define DMA_INT_MASK_ERR_WE_CH4_Msk         (0x1UL << DMA_INT_MASK_ERR_WE_CH4_Pos)
#define DMA_INT_MASK_ERR_WE_CH4             DMA_INT_MASK_ERR_WE_CH4_Msk
/** @brief DMA_INT_MASK_ERR_WE[13]: Channel 5 mask write enable for IntErr interrupt */
#define DMA_INT_MASK_ERR_WE_CH5_Pos         (13U)
#define DMA_INT_MASK_ERR_WE_CH5_Msk         (0x1UL << DMA_INT_MASK_ERR_WE_CH5_Pos)
#define DMA_INT_MASK_ERR_WE_CH5             DMA_INT_MASK_ERR_WE_CH5_Msk
/** @brief DMA_INT_MASK_ERR_WE[14]: Channel 6 mask write enable for IntErr interrupt */
#define DMA_INT_MASK_ERR_WE_CH6_Pos         (14U)
#define DMA_INT_MASK_ERR_WE_CH6_Msk         (0x1UL << DMA_INT_MASK_ERR_WE_CH6_Pos)
#define DMA_INT_MASK_ERR_WE_CH6             DMA_INT_MASK_ERR_WE_CH6_Msk
/** @brief DMA_INT_MASK_ERR_WE[15]: Channel 7 mask write enable for IntErr interrupt */
#define DMA_INT_MASK_ERR_WE_CH7_Pos         (15U)
#define DMA_INT_MASK_ERR_WE_CH7_Msk         (0x1UL << DMA_INT_MASK_ERR_WE_CH7_Pos)
#define DMA_INT_MASK_ERR_WE_CH7             DMA_INT_MASK_ERR_WE_CH7_Msk

/***************  Bit definition for DMA_INT_CLR_TFR register  ****************/

/** @brief DMA_INT_CLR_TFR[0]: Channel 0 clear for IntTfr interrupt */
#define DMA_INT_CLR_TFR_CH0_Pos             (0U)
#define DMA_INT_CLR_TFR_CH0_Msk             (0x1UL << DMA_INT_CLR_TFR_CH0_Pos)
#define DMA_INT_CLR_TFR_CH0                 DMA_INT_CLR_TFR_CH0_Msk
/** @brief DMA_INT_CLR_TFR[1]: Channel 1 clear for IntTfr interrupt */
#define DMA_INT_CLR_TFR_CH1_Pos             (1U)
#define DMA_INT_CLR_TFR_CH1_Msk             (0x1UL << DMA_INT_CLR_TFR_CH1_Pos)
#define DMA_INT_CLR_TFR_CH1                 DMA_INT_CLR_TFR_CH1_Msk
/** @brief DMA_INT_CLR_TFR[2]: Channel 2 clear for IntTfr interrupt */
#define DMA_INT_CLR_TFR_CH2_Pos             (2U)
#define DMA_INT_CLR_TFR_CH2_Msk             (0x1UL << DMA_INT_CLR_TFR_CH2_Pos)
#define DMA_INT_CLR_TFR_CH2                 DMA_INT_CLR_TFR_CH2_Msk
/** @brief DMA_INT_CLR_TFR[3]: Channel 3 clear for IntTfr interrupt */
#define DMA_INT_CLR_TFR_CH3_Pos             (3U)
#define DMA_INT_CLR_TFR_CH3_Msk             (0x1UL << DMA_INT_CLR_TFR_CH3_Pos)
#define DMA_INT_CLR_TFR_CH3                 DMA_INT_CLR_TFR_CH3_Msk
/** @brief DMA_INT_CLR_TFR[4]: Channel 4 clear for IntTfr interrupt */
#define DMA_INT_CLR_TFR_CH4_Pos             (4U)
#define DMA_INT_CLR_TFR_CH4_Msk             (0x1UL << DMA_INT_CLR_TFR_CH4_Pos)
#define DMA_INT_CLR_TFR_CH4                 DMA_INT_CLR_TFR_CH4_Msk
/** @brief DMA_INT_CLR_TFR[5]: Channel 5 clear for IntTfr interrupt */
#define DMA_INT_CLR_TFR_CH5_Pos             (5U)
#define DMA_INT_CLR_TFR_CH5_Msk             (0x1UL << DMA_INT_CLR_TFR_CH5_Pos)
#define DMA_INT_CLR_TFR_CH5                 DMA_INT_CLR_TFR_CH5_Msk
/** @brief DMA_INT_CLR_TFR[6]: Channel 6 clear for IntTfr interrupt */
#define DMA_INT_CLR_TFR_CH6_Pos             (6U)
#define DMA_INT_CLR_TFR_CH6_Msk             (0x1UL << DMA_INT_CLR_TFR_CH6_Pos)
#define DMA_INT_CLR_TFR_CH6                 DMA_INT_CLR_TFR_CH6_Msk
/** @brief DMA_INT_CLR_TFR[7]: Channel 7 clear for IntTfr interrupt */
#define DMA_INT_CLR_TFR_CH7_Pos             (7U)
#define DMA_INT_CLR_TFR_CH7_Msk             (0x1UL << DMA_INT_CLR_TFR_CH7_Pos)
#define DMA_INT_CLR_TFR_CH7                 DMA_INT_CLR_TFR_CH7_Msk

/**************  Bit definition for DMA_INT_CLR_BLOCK register  ***************/

/** @brief DMA_INT_CLR_BLOCK[0]: Channel 0 clear for IntBlock interrupt */
#define DMA_INT_CLR_BLOCK_CH0_Pos           (0U)
#define DMA_INT_CLR_BLOCK_CH0_Msk           (0x1UL << DMA_INT_CLR_BLOCK_CH0_Pos)
#define DMA_INT_CLR_BLOCK_CH0               DMA_INT_CLR_BLOCK_CH0_Msk
/** @brief DMA_INT_CLR_BLOCK[1]: Channel 1 clear for IntBlock interrupt */
#define DMA_INT_CLR_BLOCK_CH1_Pos           (1U)
#define DMA_INT_CLR_BLOCK_CH1_Msk           (0x1UL << DMA_INT_CLR_BLOCK_CH1_Pos)
#define DMA_INT_CLR_BLOCK_CH1               DMA_INT_CLR_BLOCK_CH1_Msk
/** @brief DMA_INT_CLR_BLOCK[2]: Channel 2 clear for IntBlock interrupt */
#define DMA_INT_CLR_BLOCK_CH2_Pos           (2U)
#define DMA_INT_CLR_BLOCK_CH2_Msk           (0x1UL << DMA_INT_CLR_BLOCK_CH2_Pos)
#define DMA_INT_CLR_BLOCK_CH2               DMA_INT_CLR_BLOCK_CH2_Msk
/** @brief DMA_INT_CLR_BLOCK[3]: Channel 3 clear for IntBlock interrupt */
#define DMA_INT_CLR_BLOCK_CH3_Pos           (3U)
#define DMA_INT_CLR_BLOCK_CH3_Msk           (0x1UL << DMA_INT_CLR_BLOCK_CH3_Pos)
#define DMA_INT_CLR_BLOCK_CH3               DMA_INT_CLR_BLOCK_CH3_Msk
/** @brief DMA_INT_CLR_BLOCK[4]: Channel 4 clear for IntBlock interrupt */
#define DMA_INT_CLR_BLOCK_CH4_Pos           (4U)
#define DMA_INT_CLR_BLOCK_CH4_Msk           (0x1UL << DMA_INT_CLR_BLOCK_CH4_Pos)
#define DMA_INT_CLR_BLOCK_CH4               DMA_INT_CLR_BLOCK_CH4_Msk
/** @brief DMA_INT_CLR_BLOCK[5]: Channel 5 clear for IntBlock interrupt */
#define DMA_INT_CLR_BLOCK_CH5_Pos           (5U)
#define DMA_INT_CLR_BLOCK_CH5_Msk           (0x1UL << DMA_INT_CLR_BLOCK_CH5_Pos)
#define DMA_INT_CLR_BLOCK_CH5               DMA_INT_CLR_BLOCK_CH5_Msk
/** @brief DMA_INT_CLR_BLOCK[6]: Channel 6 clear for IntBlock interrupt */
#define DMA_INT_CLR_BLOCK_CH6_Pos           (6U)
#define DMA_INT_CLR_BLOCK_CH6_Msk           (0x1UL << DMA_INT_CLR_BLOCK_CH6_Pos)
#define DMA_INT_CLR_BLOCK_CH6               DMA_INT_CLR_BLOCK_CH6_Msk
/** @brief DMA_INT_CLR_BLOCK[7]: Channel 7 clear for IntBlock interrupt */
#define DMA_INT_CLR_BLOCK_CH7_Pos           (7U)
#define DMA_INT_CLR_BLOCK_CH7_Msk           (0x1UL << DMA_INT_CLR_BLOCK_CH7_Pos)
#define DMA_INT_CLR_BLOCK_CH7               DMA_INT_CLR_BLOCK_CH7_Msk

/*************  Bit definition for DMA_INT_CLR_SRC_TRAN register  *************/

/** @brief DMA_INT_CLR_SRC_TRAN[0]: Channel 0 clear for IntSrcTran interrupt */
#define DMA_INT_CLR_SRC_TRAN_CH0_Pos        (0U)
#define DMA_INT_CLR_SRC_TRAN_CH0_Msk        (0x1UL << DMA_INT_CLR_SRC_TRAN_CH0_Pos)
#define DMA_INT_CLR_SRC_TRAN_CH0            DMA_INT_CLR_SRC_TRAN_CH0_Msk
/** @brief DMA_INT_CLR_SRC_TRAN[1]: Channel 1 clear for IntSrcTran interrupt */
#define DMA_INT_CLR_SRC_TRAN_CH1_Pos        (1U)
#define DMA_INT_CLR_SRC_TRAN_CH1_Msk        (0x1UL << DMA_INT_CLR_SRC_TRAN_CH1_Pos)
#define DMA_INT_CLR_SRC_TRAN_CH1            DMA_INT_CLR_SRC_TRAN_CH1_Msk
/** @brief DMA_INT_CLR_SRC_TRAN[2]: Channel 2 clear for IntSrcTran interrupt */
#define DMA_INT_CLR_SRC_TRAN_CH2_Pos        (2U)
#define DMA_INT_CLR_SRC_TRAN_CH2_Msk        (0x1UL << DMA_INT_CLR_SRC_TRAN_CH2_Pos)
#define DMA_INT_CLR_SRC_TRAN_CH2            DMA_INT_CLR_SRC_TRAN_CH2_Msk
/** @brief DMA_INT_CLR_SRC_TRAN[3]: Channel 3 clear for IntSrcTran interrupt */
#define DMA_INT_CLR_SRC_TRAN_CH3_Pos        (3U)
#define DMA_INT_CLR_SRC_TRAN_CH3_Msk        (0x1UL << DMA_INT_CLR_SRC_TRAN_CH3_Pos)
#define DMA_INT_CLR_SRC_TRAN_CH3            DMA_INT_CLR_SRC_TRAN_CH3_Msk
/** @brief DMA_INT_CLR_SRC_TRAN[4]: Channel 4 clear for IntSrcTran interrupt */
#define DMA_INT_CLR_SRC_TRAN_CH4_Pos        (4U)
#define DMA_INT_CLR_SRC_TRAN_CH4_Msk        (0x1UL << DMA_INT_CLR_SRC_TRAN_CH4_Pos)
#define DMA_INT_CLR_SRC_TRAN_CH4            DMA_INT_CLR_SRC_TRAN_CH4_Msk
/** @brief DMA_INT_CLR_SRC_TRAN[5]: Channel 5 clear for IntSrcTran interrupt */
#define DMA_INT_CLR_SRC_TRAN_CH5_Pos        (5U)
#define DMA_INT_CLR_SRC_TRAN_CH5_Msk        (0x1UL << DMA_INT_CLR_SRC_TRAN_CH5_Pos)
#define DMA_INT_CLR_SRC_TRAN_CH5            DMA_INT_CLR_SRC_TRAN_CH5_Msk
/** @brief DMA_INT_CLR_SRC_TRAN[6]: Channel 6 clear for IntSrcTran interrupt */
#define DMA_INT_CLR_SRC_TRAN_CH6_Pos        (6U)
#define DMA_INT_CLR_SRC_TRAN_CH6_Msk        (0x1UL << DMA_INT_CLR_SRC_TRAN_CH6_Pos)
#define DMA_INT_CLR_SRC_TRAN_CH6            DMA_INT_CLR_SRC_TRAN_CH6_Msk
/** @brief DMA_INT_CLR_SRC_TRAN[7]: Channel 7 clear for IntSrcTran interrupt */
#define DMA_INT_CLR_SRC_TRAN_CH7_Pos        (7U)
#define DMA_INT_CLR_SRC_TRAN_CH7_Msk        (0x1UL << DMA_INT_CLR_SRC_TRAN_CH7_Pos)
#define DMA_INT_CLR_SRC_TRAN_CH7            DMA_INT_CLR_SRC_TRAN_CH7_Msk

/*************  Bit definition for DMA_INT_CLR_DST_TRAN register  *************/

/** @brief DMA_INT_CLR_DST_TRAN[0]: Channel 0 clear for IntDstTran interrupt */
#define DMA_INT_CLR_DST_TRAN_CH0_Pos        (0U)
#define DMA_INT_CLR_DST_TRAN_CH0_Msk        (0x1UL << DMA_INT_CLR_DST_TRAN_CH0_Pos)
#define DMA_INT_CLR_DST_TRAN_CH0            DMA_INT_CLR_DST_TRAN_CH0_Msk
/** @brief DMA_INT_CLR_DST_TRAN[1]: Channel 1 clear for IntDstTran interrupt */
#define DMA_INT_CLR_DST_TRAN_CH1_Pos        (1U)
#define DMA_INT_CLR_DST_TRAN_CH1_Msk        (0x1UL << DMA_INT_CLR_DST_TRAN_CH1_Pos)
#define DMA_INT_CLR_DST_TRAN_CH1            DMA_INT_CLR_DST_TRAN_CH1_Msk
/** @brief DMA_INT_CLR_DST_TRAN[2]: Channel 2 clear for IntDstTran interrupt */
#define DMA_INT_CLR_DST_TRAN_CH2_Pos        (2U)
#define DMA_INT_CLR_DST_TRAN_CH2_Msk        (0x1UL << DMA_INT_CLR_DST_TRAN_CH2_Pos)
#define DMA_INT_CLR_DST_TRAN_CH2            DMA_INT_CLR_DST_TRAN_CH2_Msk
/** @brief DMA_INT_CLR_DST_TRAN[3]: Channel 3 clear for IntDstTran interrupt */
#define DMA_INT_CLR_DST_TRAN_CH3_Pos        (3U)
#define DMA_INT_CLR_DST_TRAN_CH3_Msk        (0x1UL << DMA_INT_CLR_DST_TRAN_CH3_Pos)
#define DMA_INT_CLR_DST_TRAN_CH3            DMA_INT_CLR_DST_TRAN_CH3_Msk
/** @brief DMA_INT_CLR_DST_TRAN[4]: Channel 4 clear for IntDstTran interrupt */
#define DMA_INT_CLR_DST_TRAN_CH4_Pos        (4U)
#define DMA_INT_CLR_DST_TRAN_CH4_Msk        (0x1UL << DMA_INT_CLR_DST_TRAN_CH4_Pos)
#define DMA_INT_CLR_DST_TRAN_CH4            DMA_INT_CLR_DST_TRAN_CH4_Msk
/** @brief DMA_INT_CLR_DST_TRAN[5]: Channel 5 clear for IntDstTran interrupt */
#define DMA_INT_CLR_DST_TRAN_CH5_Pos        (5U)
#define DMA_INT_CLR_DST_TRAN_CH5_Msk        (0x1UL << DMA_INT_CLR_DST_TRAN_CH5_Pos)
#define DMA_INT_CLR_DST_TRAN_CH5            DMA_INT_CLR_DST_TRAN_CH5_Msk
/** @brief DMA_INT_CLR_DST_TRAN[6]: Channel 6 clear for IntDstTran interrupt */
#define DMA_INT_CLR_DST_TRAN_CH6_Pos        (6U)
#define DMA_INT_CLR_DST_TRAN_CH6_Msk        (0x1UL << DMA_INT_CLR_DST_TRAN_CH6_Pos)
#define DMA_INT_CLR_DST_TRAN_CH6            DMA_INT_CLR_DST_TRAN_CH6_Msk
/** @brief DMA_INT_CLR_DST_TRAN[7]: Channel 7 clear for IntDstTran interrupt */
#define DMA_INT_CLR_DST_TRAN_CH7_Pos        (7U)
#define DMA_INT_CLR_DST_TRAN_CH7_Msk        (0x1UL << DMA_INT_CLR_DST_TRAN_CH7_Pos)
#define DMA_INT_CLR_DST_TRAN_CH7            DMA_INT_CLR_DST_TRAN_CH7_Msk

/***************  Bit definition for DMA_INT_CLR_ERR register  ****************/

/** @brief DMA_INT_CLR_ERR[0]: Channel 0 clear for IntErr interrupt */
#define DMA_INT_CLR_ERR_CH0_Pos             (0U)
#define DMA_INT_CLR_ERR_CH0_Msk             (0x1UL << DMA_INT_CLR_ERR_CH0_Pos)
#define DMA_INT_CLR_ERR_CH0                 DMA_INT_CLR_ERR_CH0_Msk
/** @brief DMA_INT_CLR_ERR[1]: Channel 1 clear for IntErr interrupt */
#define DMA_INT_CLR_ERR_CH1_Pos             (1U)
#define DMA_INT_CLR_ERR_CH1_Msk             (0x1UL << DMA_INT_CLR_ERR_CH1_Pos)
#define DMA_INT_CLR_ERR_CH1                 DMA_INT_CLR_ERR_CH1_Msk
/** @brief DMA_INT_CLR_ERR[2]: Channel 2 clear for IntErr interrupt */
#define DMA_INT_CLR_ERR_CH2_Pos             (2U)
#define DMA_INT_CLR_ERR_CH2_Msk             (0x1UL << DMA_INT_CLR_ERR_CH2_Pos)
#define DMA_INT_CLR_ERR_CH2                 DMA_INT_CLR_ERR_CH2_Msk
/** @brief DMA_INT_CLR_ERR[3]: Channel 3 clear for IntErr interrupt */
#define DMA_INT_CLR_ERR_CH3_Pos             (3U)
#define DMA_INT_CLR_ERR_CH3_Msk             (0x1UL << DMA_INT_CLR_ERR_CH3_Pos)
#define DMA_INT_CLR_ERR_CH3                 DMA_INT_CLR_ERR_CH3_Msk
/** @brief DMA_INT_CLR_ERR[4]: Channel 4 clear for IntErr interrupt */
#define DMA_INT_CLR_ERR_CH4_Pos             (4U)
#define DMA_INT_CLR_ERR_CH4_Msk             (0x1UL << DMA_INT_CLR_ERR_CH4_Pos)
#define DMA_INT_CLR_ERR_CH4                 DMA_INT_CLR_ERR_CH4_Msk
/** @brief DMA_INT_CLR_ERR[5]: Channel 5 clear for IntErr interrupt */
#define DMA_INT_CLR_ERR_CH5_Pos             (5U)
#define DMA_INT_CLR_ERR_CH5_Msk             (0x1UL << DMA_INT_CLR_ERR_CH5_Pos)
#define DMA_INT_CLR_ERR_CH5                 DMA_INT_CLR_ERR_CH5_Msk
/** @brief DMA_INT_CLR_ERR[6]: Channel 6 clear for IntErr interrupt */
#define DMA_INT_CLR_ERR_CH6_Pos             (6U)
#define DMA_INT_CLR_ERR_CH6_Msk             (0x1UL << DMA_INT_CLR_ERR_CH6_Pos)
#define DMA_INT_CLR_ERR_CH6                 DMA_INT_CLR_ERR_CH6_Msk
/** @brief DMA_INT_CLR_ERR[7]: Channel 7 clear for IntErr interrupt */
#define DMA_INT_CLR_ERR_CH7_Pos             (7U)
#define DMA_INT_CLR_ERR_CH7_Msk             (0x1UL << DMA_INT_CLR_ERR_CH7_Pos)
#define DMA_INT_CLR_ERR_CH7                 DMA_INT_CLR_ERR_CH7_Msk

/*****************  Bit definition for DMA_INT_STAT register  *****************/

/** @brief DMA_INT_STAT[0]: OR of the contents of STAT_TFR register */
#define DMA_INT_STAT_TFR_Pos                (0U)
#define DMA_INT_STAT_TFR_Msk                (0x1UL << DMA_INT_STAT_TFR_Pos)
#define DMA_INT_STAT_TFR                    DMA_INT_STAT_TFR_Msk
/** @brief DMA_INT_STAT[1]: OR of the contents of STAT_BLOCK register */
#define DMA_INT_STAT_BLOCK_Pos              (1U)
#define DMA_INT_STAT_BLOCK_Msk              (0x1UL << DMA_INT_STAT_BLOCK_Pos)
#define DMA_INT_STAT_BLOCK                  DMA_INT_STAT_BLOCK_Msk
/** @brief DMA_INT_STAT[2]: OR of the contents of STAT_SRC_TRAN register */
#define DMA_INT_STAT_SRC_TRAN_Pos           (2U)
#define DMA_INT_STAT_SRC_TRAN_Msk           (0x1UL << DMA_INT_STAT_SRC_TRAN_Pos)
#define DMA_INT_STAT_SRC_TRAN               DMA_INT_STAT_SRC_TRAN_Msk
/** @brief DMA_INT_STAT[3]: OR of the contents of STAT_DST_TRAN register */
#define DMA_INT_STAT_DST_TRAN_Pos           (3U)
#define DMA_INT_STAT_DST_TRAN_Msk           (0x1UL << DMA_INT_STAT_DST_TRAN_Pos)
#define DMA_INT_STAT_DST_TRAN               DMA_INT_STAT_DST_TRAN_Msk
/** @brief DMA_INT_STAT[4]: OR of the contents of STAT_ERR register */
#define DMA_INT_STAT_ERR_Pos                (4U)
#define DMA_INT_STAT_ERR_Msk                (0x1UL << DMA_INT_STAT_ERR_Pos)
#define DMA_INT_STAT_ERR                    DMA_INT_STAT_ERR_Msk

/****************  Bit definition for DMA_HS_REQ_SRC register  ****************/

/** @brief DMA_HS_REQ_SRC[0]: Channel 0 source software transaction request */
#define DMA_HS_REQ_SRC_CH0_Pos              (0U)
#define DMA_HS_REQ_SRC_CH0_Msk              (0x1UL << DMA_HS_REQ_SRC_CH0_Pos)
#define DMA_HS_REQ_SRC_CH0                  DMA_HS_REQ_SRC_CH0_Msk
/** @brief DMA_HS_REQ_SRC[1]: Channel 1 source software transaction request */
#define DMA_HS_REQ_SRC_CH1_Pos              (1U)
#define DMA_HS_REQ_SRC_CH1_Msk              (0x1UL << DMA_HS_REQ_SRC_CH1_Pos)
#define DMA_HS_REQ_SRC_CH1                  DMA_HS_REQ_SRC_CH1_Msk
/** @brief DMA_HS_REQ_SRC[2]: Channel 2 source software transaction request */
#define DMA_HS_REQ_SRC_CH2_Pos              (2U)
#define DMA_HS_REQ_SRC_CH2_Msk              (0x1UL << DMA_HS_REQ_SRC_CH2_Pos)
#define DMA_HS_REQ_SRC_CH2                  DMA_HS_REQ_SRC_CH2_Msk
/** @brief DMA_HS_REQ_SRC[3]: Channel 3 source software transaction request */
#define DMA_HS_REQ_SRC_CH3_Pos              (3U)
#define DMA_HS_REQ_SRC_CH3_Msk              (0x1UL << DMA_HS_REQ_SRC_CH3_Pos)
#define DMA_HS_REQ_SRC_CH3                  DMA_HS_REQ_SRC_CH3_Msk
/** @brief DMA_HS_REQ_SRC[4]: Channel 4 source software transaction request */
#define DMA_HS_REQ_SRC_CH4_Pos              (4U)
#define DMA_HS_REQ_SRC_CH4_Msk              (0x1UL << DMA_HS_REQ_SRC_CH4_Pos)
#define DMA_HS_REQ_SRC_CH4                  DMA_HS_REQ_SRC_CH4_Msk
/** @brief DMA_HS_REQ_SRC[5]: Channel 5 source software transaction request */
#define DMA_HS_REQ_SRC_CH5_Pos              (5U)
#define DMA_HS_REQ_SRC_CH5_Msk              (0x1UL << DMA_HS_REQ_SRC_CH5_Pos)
#define DMA_HS_REQ_SRC_CH5                  DMA_HS_REQ_SRC_CH5_Msk
/** @brief DMA_HS_REQ_SRC[6]: Channel 6 source software transaction request */
#define DMA_HS_REQ_SRC_CH6_Pos              (6U)
#define DMA_HS_REQ_SRC_CH6_Msk              (0x1UL << DMA_HS_REQ_SRC_CH6_Pos)
#define DMA_HS_REQ_SRC_CH6                  DMA_HS_REQ_SRC_CH6_Msk
/** @brief DMA_HS_REQ_SRC[7]: Channel 7 source software transaction request */
#define DMA_HS_REQ_SRC_CH7_Pos              (7U)
#define DMA_HS_REQ_SRC_CH7_Msk              (0x1UL << DMA_HS_REQ_SRC_CH7_Pos)
#define DMA_HS_REQ_SRC_CH7                  DMA_HS_REQ_SRC_CH7_Msk
/** @brief DMA_HS_REQ_SRC_WE[8]: Channel 0 source software transaction request write enable */
#define DMA_HS_REQ_SRC_WE_CH0_Pos           (8U)
#define DMA_HS_REQ_SRC_WE_CH0_Msk           (0x1UL << DMA_HS_REQ_SRC_WE_CH0_Pos)
#define DMA_HS_REQ_SRC_WE_CH0               DMA_HS_REQ_SRC_WE_CH0_Msk
/** @brief DMA_HS_REQ_SRC_WE[9]: Channel 1 source software transaction request write enable */
#define DMA_HS_REQ_SRC_WE_CH1_Pos           (9U)
#define DMA_HS_REQ_SRC_WE_CH1_Msk           (0x1UL << DMA_HS_REQ_SRC_WE_CH1_Pos)
#define DMA_HS_REQ_SRC_WE_CH1               DMA_HS_REQ_SRC_WE_CH1_Msk
/** @brief DMA_HS_REQ_SRC_WE[10]: Channel 2 source software transaction request write enable */
#define DMA_HS_REQ_SRC_WE_CH2_Pos           (10U)
#define DMA_HS_REQ_SRC_WE_CH2_Msk           (0x1UL << DMA_HS_REQ_SRC_WE_CH2_Pos)
#define DMA_HS_REQ_SRC_WE_CH2               DMA_HS_REQ_SRC_WE_CH2_Msk
/** @brief DMA_HS_REQ_SRC_WE[11]: Channel 3 source software transaction request write enable */
#define DMA_HS_REQ_SRC_WE_CH3_Pos           (11U)
#define DMA_HS_REQ_SRC_WE_CH3_Msk           (0x1UL << DMA_HS_REQ_SRC_WE_CH3_Pos)
#define DMA_HS_REQ_SRC_WE_CH3               DMA_HS_REQ_SRC_WE_CH3_Msk
/** @brief DMA_HS_REQ_SRC_WE[12]: Channel 4 source software transaction request write enable */
#define DMA_HS_REQ_SRC_WE_CH4_Pos           (12U)
#define DMA_HS_REQ_SRC_WE_CH4_Msk           (0x1UL << DMA_HS_REQ_SRC_WE_CH4_Pos)
#define DMA_HS_REQ_SRC_WE_CH4               DMA_HS_REQ_SRC_WE_CH4_Msk
/** @brief DMA_HS_REQ_SRC_WE[13]: Channel 5 source software transaction request write enable */
#define DMA_HS_REQ_SRC_WE_CH5_Pos           (13U)
#define DMA_HS_REQ_SRC_WE_CH5_Msk           (0x1UL << DMA_HS_REQ_SRC_WE_CH5_Pos)
#define DMA_HS_REQ_SRC_WE_CH5               DMA_HS_REQ_SRC_WE_CH5_Msk
/** @brief DMA_HS_REQ_SRC_WE[14]: Channel 6 source software transaction request write enable */
#define DMA_HS_REQ_SRC_WE_CH6_Pos           (14U)
#define DMA_HS_REQ_SRC_WE_CH6_Msk           (0x1UL << DMA_HS_REQ_SRC_WE_CH6_Pos)
#define DMA_HS_REQ_SRC_WE_CH6               DMA_HS_REQ_SRC_WE_CH6_Msk
/** @brief DMA_HS_REQ_SRC_WE[15]: Channel 7 source software transaction request write enable */
#define DMA_HS_REQ_SRC_WE_CH7_Pos           (15U)
#define DMA_HS_REQ_SRC_WE_CH7_Msk           (0x1UL << DMA_HS_REQ_SRC_WE_CH7_Pos)
#define DMA_HS_REQ_SRC_WE_CH7               DMA_HS_REQ_SRC_WE_CH7_Msk

/****************  Bit definition for DMA_HS_REQ_DST register  ****************/

/** @brief DMA_HS_REQ_DST[0]: Channel 0 destination software transaction request */
#define DMA_HS_REQ_DST_CH0_Pos              (0U)
#define DMA_HS_REQ_DST_CH0_Msk              (0x1UL << DMA_HS_REQ_DST_CH0_Pos)
#define DMA_HS_REQ_DST_CH0                  DMA_HS_REQ_DST_CH0_Msk
/** @brief DMA_HS_REQ_DST[1]: Channel 1 destination software transaction request */
#define DMA_HS_REQ_DST_CH1_Pos              (1U)
#define DMA_HS_REQ_DST_CH1_Msk              (0x1UL << DMA_HS_REQ_DST_CH1_Pos)
#define DMA_HS_REQ_DST_CH1                  DMA_HS_REQ_DST_CH1_Msk
/** @brief DMA_HS_REQ_DST[2]: Channel 2 destination software transaction request */
#define DMA_HS_REQ_DST_CH2_Pos              (2U)
#define DMA_HS_REQ_DST_CH2_Msk              (0x1UL << DMA_HS_REQ_DST_CH2_Pos)
#define DMA_HS_REQ_DST_CH2                  DMA_HS_REQ_DST_CH2_Msk
/** @brief DMA_HS_REQ_DST[3]: Channel 3 destination software transaction request */
#define DMA_HS_REQ_DST_CH3_Pos              (3U)
#define DMA_HS_REQ_DST_CH3_Msk              (0x1UL << DMA_HS_REQ_DST_CH3_Pos)
#define DMA_HS_REQ_DST_CH3                  DMA_HS_REQ_DST_CH3_Msk
/** @brief DMA_HS_REQ_DST[4]: Channel 4 destination software transaction request */
#define DMA_HS_REQ_DST_CH4_Pos              (4U)
#define DMA_HS_REQ_DST_CH4_Msk              (0x1UL << DMA_HS_REQ_DST_CH4_Pos)
#define DMA_HS_REQ_DST_CH4                  DMA_HS_REQ_DST_CH4_Msk
/** @brief DMA_HS_REQ_DST[5]: Channel 5 destination software transaction request */
#define DMA_HS_REQ_DST_CH5_Pos              (5U)
#define DMA_HS_REQ_DST_CH5_Msk              (0x1UL << DMA_HS_REQ_DST_CH5_Pos)
#define DMA_HS_REQ_DST_CH5                  DMA_HS_REQ_DST_CH5_Msk
/** @brief DMA_HS_REQ_DST[6]: Channel 6 destination software transaction request */
#define DMA_HS_REQ_DST_CH6_Pos              (6U)
#define DMA_HS_REQ_DST_CH6_Msk              (0x1UL << DMA_HS_REQ_DST_CH6_Pos)
#define DMA_HS_REQ_DST_CH6                  DMA_HS_REQ_DST_CH6_Msk
/** @brief DMA_HS_REQ_DST[7]: Channel 7 destination software transaction request */
#define DMA_HS_REQ_DST_CH7_Pos              (7U)
#define DMA_HS_REQ_DST_CH7_Msk              (0x1UL << DMA_HS_REQ_DST_CH7_Pos)
#define DMA_HS_REQ_DST_CH7                  DMA_HS_REQ_DST_CH7_Msk
/** @brief DMA_HS_REQ_DST_WE[8]: Channel 0 destination software transaction request write enable */
#define DMA_HS_REQ_DST_WE_CH0_Pos           (8U)
#define DMA_HS_REQ_DST_WE_CH0_Msk           (0x1UL << DMA_HS_REQ_DST_WE_CH0_Pos)
#define DMA_HS_REQ_DST_WE_CH0               DMA_HS_REQ_DST_WE_CH0_Msk
/** @brief DMA_HS_REQ_DST_WE[9]: Channel 1 destination software transaction request write enable */
#define DMA_HS_REQ_DST_WE_CH1_Pos           (9U)
#define DMA_HS_REQ_DST_WE_CH1_Msk           (0x1UL << DMA_HS_REQ_DST_WE_CH1_Pos)
#define DMA_HS_REQ_DST_WE_CH1               DMA_HS_REQ_DST_WE_CH1_Msk
/** @brief DMA_HS_REQ_DST_WE[10]: Channel 2 destination software transaction request write enable */
#define DMA_HS_REQ_DST_WE_CH2_Pos           (10U)
#define DMA_HS_REQ_DST_WE_CH2_Msk           (0x1UL << DMA_HS_REQ_DST_WE_CH2_Pos)
#define DMA_HS_REQ_DST_WE_CH2               DMA_HS_REQ_DST_WE_CH2_Msk
/** @brief DMA_HS_REQ_DST_WE[11]: Channel 3 destination software transaction request write enable */
#define DMA_HS_REQ_DST_WE_CH3_Pos           (11U)
#define DMA_HS_REQ_DST_WE_CH3_Msk           (0x1UL << DMA_HS_REQ_DST_WE_CH3_Pos)
#define DMA_HS_REQ_DST_WE_CH3               DMA_HS_REQ_DST_WE_CH3_Msk
/** @brief DMA_HS_REQ_DST_WE[12]: Channel 4 destination software transaction request write enable */
#define DMA_HS_REQ_DST_WE_CH4_Pos           (12U)
#define DMA_HS_REQ_DST_WE_CH4_Msk           (0x1UL << DMA_HS_REQ_DST_WE_CH4_Pos)
#define DMA_HS_REQ_DST_WE_CH4               DMA_HS_REQ_DST_WE_CH4_Msk
/** @brief DMA_HS_REQ_DST_WE[13]: Channel 5 destination software transaction request write enable */
#define DMA_HS_REQ_DST_WE_CH5_Pos           (13U)
#define DMA_HS_REQ_DST_WE_CH5_Msk           (0x1UL << DMA_HS_REQ_DST_WE_CH5_Pos)
#define DMA_HS_REQ_DST_WE_CH5               DMA_HS_REQ_DST_WE_CH5_Msk
/** @brief DMA_HS_REQ_DST_WE[14]: Channel 6 destination software transaction request write enable */
#define DMA_HS_REQ_DST_WE_CH6_Pos           (14U)
#define DMA_HS_REQ_DST_WE_CH6_Msk           (0x1UL << DMA_HS_REQ_DST_WE_CH6_Pos)
#define DMA_HS_REQ_DST_WE_CH6               DMA_HS_REQ_DST_WE_CH6_Msk
/** @brief DMA_HS_REQ_DST_WE[15]: Channel 7 destination software transaction request write enable */
#define DMA_HS_REQ_DST_WE_CH7_Pos           (15U)
#define DMA_HS_REQ_DST_WE_CH7_Msk           (0x1UL << DMA_HS_REQ_DST_WE_CH7_Pos)
#define DMA_HS_REQ_DST_WE_CH7               DMA_HS_REQ_DST_WE_CH7_Msk

/**************  Bit definition for DMA_HS_SGL_REQ_SRC register  **************/

/** @brief DMA_HS_SGL_REQ_SRC[0]: Channel 0 source single transaction request */
#define DMA_HS_SGL_REQ_SRC_CH0_Pos          (0U)
#define DMA_HS_SGL_REQ_SRC_CH0_Msk          (0x1UL << DMA_HS_SGL_REQ_SRC_CH0_Pos)
#define DMA_HS_SGL_REQ_SRC_CH0              DMA_HS_SGL_REQ_SRC_CH0_Msk
/** @brief DMA_HS_SGL_REQ_SRC[1]: Channel 1 source single transaction request */
#define DMA_HS_SGL_REQ_SRC_CH1_Pos          (1U)
#define DMA_HS_SGL_REQ_SRC_CH1_Msk          (0x1UL << DMA_HS_SGL_REQ_SRC_CH1_Pos)
#define DMA_HS_SGL_REQ_SRC_CH1              DMA_HS_SGL_REQ_SRC_CH1_Msk
/** @brief DMA_HS_SGL_REQ_SRC[2]: Channel 2 source single transaction request */
#define DMA_HS_SGL_REQ_SRC_CH2_Pos          (2U)
#define DMA_HS_SGL_REQ_SRC_CH2_Msk          (0x1UL << DMA_HS_SGL_REQ_SRC_CH2_Pos)
#define DMA_HS_SGL_REQ_SRC_CH2              DMA_HS_SGL_REQ_SRC_CH2_Msk
/** @brief DMA_HS_SGL_REQ_SRC[3]: Channel 3 source single transaction request */
#define DMA_HS_SGL_REQ_SRC_CH3_Pos          (3U)
#define DMA_HS_SGL_REQ_SRC_CH3_Msk          (0x1UL << DMA_HS_SGL_REQ_SRC_CH3_Pos)
#define DMA_HS_SGL_REQ_SRC_CH3              DMA_HS_SGL_REQ_SRC_CH3_Msk
/** @brief DMA_HS_SGL_REQ_SRC[4]: Channel 4 source single transaction request */
#define DMA_HS_SGL_REQ_SRC_CH4_Pos          (4U)
#define DMA_HS_SGL_REQ_SRC_CH4_Msk          (0x1UL << DMA_HS_SGL_REQ_SRC_CH4_Pos)
#define DMA_HS_SGL_REQ_SRC_CH4              DMA_HS_SGL_REQ_SRC_CH4_Msk
/** @brief DMA_HS_SGL_REQ_SRC[5]: Channel 5 source single transaction request */
#define DMA_HS_SGL_REQ_SRC_CH5_Pos          (5U)
#define DMA_HS_SGL_REQ_SRC_CH5_Msk          (0x1UL << DMA_HS_SGL_REQ_SRC_CH5_Pos)
#define DMA_HS_SGL_REQ_SRC_CH5              DMA_HS_SGL_REQ_SRC_CH5_Msk
/** @brief DMA_HS_SGL_REQ_SRC[6]: Channel 6 source single transaction request */
#define DMA_HS_SGL_REQ_SRC_CH6_Pos          (6U)
#define DMA_HS_SGL_REQ_SRC_CH6_Msk          (0x1UL << DMA_HS_SGL_REQ_SRC_CH6_Pos)
#define DMA_HS_SGL_REQ_SRC_CH6              DMA_HS_SGL_REQ_SRC_CH6_Msk
/** @brief DMA_HS_SGL_REQ_SRC[7]: Channel 7 source single transaction request */
#define DMA_HS_SGL_REQ_SRC_CH7_Pos          (7U)
#define DMA_HS_SGL_REQ_SRC_CH7_Msk          (0x1UL << DMA_HS_SGL_REQ_SRC_CH7_Pos)
#define DMA_HS_SGL_REQ_SRC_CH7              DMA_HS_SGL_REQ_SRC_CH7_Msk
/** @brief DMA_HS_SGL_REQ_SRC_WE[8]: Channel 0 source single transaction request write enable */
#define DMA_HS_SGL_REQ_SRC_WE_CH0_Pos       (8U)
#define DMA_HS_SGL_REQ_SRC_WE_CH0_Msk       (0x1UL << DMA_HS_SGL_REQ_SRC_WE_CH0_Pos)
#define DMA_HS_SGL_REQ_SRC_WE_CH0           DMA_HS_SGL_REQ_SRC_WE_CH0_Msk
/** @brief DMA_HS_SGL_REQ_SRC_WE[9]: Channel 1 source single transaction request write enable */
#define DMA_HS_SGL_REQ_SRC_WE_CH1_Pos       (9U)
#define DMA_HS_SGL_REQ_SRC_WE_CH1_Msk       (0x1UL << DMA_HS_SGL_REQ_SRC_WE_CH1_Pos)
#define DMA_HS_SGL_REQ_SRC_WE_CH1           DMA_HS_SGL_REQ_SRC_WE_CH1_Msk
/** @brief DMA_HS_SGL_REQ_SRC_WE[10]: Channel 2 source single transaction request write enable */
#define DMA_HS_SGL_REQ_SRC_WE_CH2_Pos       (10U)
#define DMA_HS_SGL_REQ_SRC_WE_CH2_Msk       (0x1UL << DMA_HS_SGL_REQ_SRC_WE_CH2_Pos)
#define DMA_HS_SGL_REQ_SRC_WE_CH2           DMA_HS_SGL_REQ_SRC_WE_CH2_Msk
/** @brief DMA_HS_SGL_REQ_SRC_WE[11]: Channel 3 source single transaction request write enable */
#define DMA_HS_SGL_REQ_SRC_WE_CH3_Pos       (11U)
#define DMA_HS_SGL_REQ_SRC_WE_CH3_Msk       (0x1UL << DMA_HS_SGL_REQ_SRC_WE_CH3_Pos)
#define DMA_HS_SGL_REQ_SRC_WE_CH3           DMA_HS_SGL_REQ_SRC_WE_CH3_Msk
/** @brief DMA_HS_SGL_REQ_SRC_WE[12]: Channel 4 source single transaction request write enable */
#define DMA_HS_SGL_REQ_SRC_WE_CH4_Pos       (12U)
#define DMA_HS_SGL_REQ_SRC_WE_CH4_Msk       (0x1UL << DMA_HS_SGL_REQ_SRC_WE_CH4_Pos)
#define DMA_HS_SGL_REQ_SRC_WE_CH4           DMA_HS_SGL_REQ_SRC_WE_CH4_Msk
/** @brief DMA_HS_SGL_REQ_SRC_WE[13]: Channel 5 source single transaction request write enable */
#define DMA_HS_SGL_REQ_SRC_WE_CH5_Pos       (13U)
#define DMA_HS_SGL_REQ_SRC_WE_CH5_Msk       (0x1UL << DMA_HS_SGL_REQ_SRC_WE_CH5_Pos)
#define DMA_HS_SGL_REQ_SRC_WE_CH5           DMA_HS_SGL_REQ_SRC_WE_CH5_Msk
/** @brief DMA_HS_SGL_REQ_SRC_WE[14]: Channel 6 source single transaction request write enable */
#define DMA_HS_SGL_REQ_SRC_WE_CH6_Pos       (14U)
#define DMA_HS_SGL_REQ_SRC_WE_CH6_Msk       (0x1UL << DMA_HS_SGL_REQ_SRC_WE_CH6_Pos)
#define DMA_HS_SGL_REQ_SRC_WE_CH6           DMA_HS_SGL_REQ_SRC_WE_CH6_Msk
/** @brief DMA_HS_SGL_REQ_SRC_WE[15]: Channel 7 source single transaction request write enable */
#define DMA_HS_SGL_REQ_SRC_WE_CH7_Pos       (15U)
#define DMA_HS_SGL_REQ_SRC_WE_CH7_Msk       (0x1UL << DMA_HS_SGL_REQ_SRC_WE_CH7_Pos)
#define DMA_HS_SGL_REQ_SRC_WE_CH7           DMA_HS_SGL_REQ_SRC_WE_CH7_Msk

/**************  Bit definition for DMA_HS_SGL_REQ_DST register  **************/

/** @brief DMA_HS_SGL_REQ_DST[0]: Channel 0 destination single transaction request */
#define DMA_HS_SGL_REQ_DST_CH0_Pos          (0U)
#define DMA_HS_SGL_REQ_DST_CH0_Msk          (0x1UL << DMA_HS_SGL_REQ_DST_CH0_Pos)
#define DMA_HS_SGL_REQ_DST_CH0              DMA_HS_SGL_REQ_DST_CH0_Msk
/** @brief DMA_HS_SGL_REQ_DST[1]: Channel 1 destination single transaction request */
#define DMA_HS_SGL_REQ_DST_CH1_Pos          (1U)
#define DMA_HS_SGL_REQ_DST_CH1_Msk          (0x1UL << DMA_HS_SGL_REQ_DST_CH1_Pos)
#define DMA_HS_SGL_REQ_DST_CH1              DMA_HS_SGL_REQ_DST_CH1_Msk
/** @brief DMA_HS_SGL_REQ_DST[2]: Channel 2 destination single transaction request */
#define DMA_HS_SGL_REQ_DST_CH2_Pos          (2U)
#define DMA_HS_SGL_REQ_DST_CH2_Msk          (0x1UL << DMA_HS_SGL_REQ_DST_CH2_Pos)
#define DMA_HS_SGL_REQ_DST_CH2              DMA_HS_SGL_REQ_DST_CH2_Msk
/** @brief DMA_HS_SGL_REQ_DST[3]: Channel 3 destination single transaction request */
#define DMA_HS_SGL_REQ_DST_CH3_Pos          (3U)
#define DMA_HS_SGL_REQ_DST_CH3_Msk          (0x1UL << DMA_HS_SGL_REQ_DST_CH3_Pos)
#define DMA_HS_SGL_REQ_DST_CH3              DMA_HS_SGL_REQ_DST_CH3_Msk
/** @brief DMA_HS_SGL_REQ_DST[4]: Channel 4 destination single transaction request */
#define DMA_HS_SGL_REQ_DST_CH4_Pos          (4U)
#define DMA_HS_SGL_REQ_DST_CH4_Msk          (0x1UL << DMA_HS_SGL_REQ_DST_CH4_Pos)
#define DMA_HS_SGL_REQ_DST_CH4              DMA_HS_SGL_REQ_DST_CH4_Msk
/** @brief DMA_HS_SGL_REQ_DST[5]: Channel 5 destination single transaction request */
#define DMA_HS_SGL_REQ_DST_CH5_Pos          (5U)
#define DMA_HS_SGL_REQ_DST_CH5_Msk          (0x1UL << DMA_HS_SGL_REQ_DST_CH5_Pos)
#define DMA_HS_SGL_REQ_DST_CH5              DMA_HS_SGL_REQ_DST_CH5_Msk
/** @brief DMA_HS_SGL_REQ_DST[6]: Channel 6 destination single transaction request */
#define DMA_HS_SGL_REQ_DST_CH6_Pos          (6U)
#define DMA_HS_SGL_REQ_DST_CH6_Msk          (0x1UL << DMA_HS_SGL_REQ_DST_CH6_Pos)
#define DMA_HS_SGL_REQ_DST_CH6              DMA_HS_SGL_REQ_DST_CH6_Msk
/** @brief DMA_HS_SGL_REQ_DST[7]: Channel 7 destination single transaction request */
#define DMA_HS_SGL_REQ_DST_CH7_Pos          (7U)
#define DMA_HS_SGL_REQ_DST_CH7_Msk          (0x1UL << DMA_HS_SGL_REQ_DST_CH7_Pos)
#define DMA_HS_SGL_REQ_DST_CH7              DMA_HS_SGL_REQ_DST_CH7_Msk
/** @brief DMA_HS_SGL_REQ_DST_WE[8]: Channel 0 destination single transaction request write enable */
#define DMA_HS_SGL_REQ_DST_WE_CH0_Pos       (8U)
#define DMA_HS_SGL_REQ_DST_WE_CH0_Msk       (0x1UL << DMA_HS_SGL_REQ_DST_WE_CH0_Pos)
#define DMA_HS_SGL_REQ_DST_WE_CH0           DMA_HS_SGL_REQ_DST_WE_CH0_Msk
/** @brief DMA_HS_SGL_REQ_DST_WE[9]: Channel 1 destination single transaction request write enable */
#define DMA_HS_SGL_REQ_DST_WE_CH1_Pos       (9U)
#define DMA_HS_SGL_REQ_DST_WE_CH1_Msk       (0x1UL << DMA_HS_SGL_REQ_DST_WE_CH1_Pos)
#define DMA_HS_SGL_REQ_DST_WE_CH1           DMA_HS_SGL_REQ_DST_WE_CH1_Msk
/** @brief DMA_HS_SGL_REQ_DST_WE[10]: Channel 2 destination single transaction request write enable */
#define DMA_HS_SGL_REQ_DST_WE_CH2_Pos       (10U)
#define DMA_HS_SGL_REQ_DST_WE_CH2_Msk       (0x1UL << DMA_HS_SGL_REQ_DST_WE_CH2_Pos)
#define DMA_HS_SGL_REQ_DST_WE_CH2           DMA_HS_SGL_REQ_DST_WE_CH2_Msk
/** @brief DMA_HS_SGL_REQ_DST_WE[11]: Channel 3 destination single transaction request write enable */
#define DMA_HS_SGL_REQ_DST_WE_CH3_Pos       (11U)
#define DMA_HS_SGL_REQ_DST_WE_CH3_Msk       (0x1UL << DMA_HS_SGL_REQ_DST_WE_CH3_Pos)
#define DMA_HS_SGL_REQ_DST_WE_CH3           DMA_HS_SGL_REQ_DST_WE_CH3_Msk
/** @brief DMA_HS_SGL_REQ_DST_WE[12]: Channel 4 destination single transaction request write enable */
#define DMA_HS_SGL_REQ_DST_WE_CH4_Pos       (12U)
#define DMA_HS_SGL_REQ_DST_WE_CH4_Msk       (0x1UL << DMA_HS_SGL_REQ_DST_WE_CH4_Pos)
#define DMA_HS_SGL_REQ_DST_WE_CH4           DMA_HS_SGL_REQ_DST_WE_CH4_Msk
/** @brief DMA_HS_SGL_REQ_DST_WE[13]: Channel 5 destination single transaction request write enable */
#define DMA_HS_SGL_REQ_DST_WE_CH5_Pos       (13U)
#define DMA_HS_SGL_REQ_DST_WE_CH5_Msk       (0x1UL << DMA_HS_SGL_REQ_DST_WE_CH5_Pos)
#define DMA_HS_SGL_REQ_DST_WE_CH5           DMA_HS_SGL_REQ_DST_WE_CH5_Msk
/** @brief DMA_HS_SGL_REQ_DST_WE[14]: Channel 6 destination single transaction request write enable */
#define DMA_HS_SGL_REQ_DST_WE_CH6_Pos       (14U)
#define DMA_HS_SGL_REQ_DST_WE_CH6_Msk       (0x1UL << DMA_HS_SGL_REQ_DST_WE_CH6_Pos)
#define DMA_HS_SGL_REQ_DST_WE_CH6           DMA_HS_SGL_REQ_DST_WE_CH6_Msk
/** @brief DMA_HS_SGL_REQ_DST_WE[15]: Channel 7 destination single transaction request write enable */
#define DMA_HS_SGL_REQ_DST_WE_CH7_Pos       (15U)
#define DMA_HS_SGL_REQ_DST_WE_CH7_Msk       (0x1UL << DMA_HS_SGL_REQ_DST_WE_CH7_Pos)
#define DMA_HS_SGL_REQ_DST_WE_CH7           DMA_HS_SGL_REQ_DST_WE_CH7_Msk

/****************  Bit definition for DMA_HS_LST_SRC register  ****************/

/** @brief DMA_HS_LST_SRC[0]: Channel 0 source last transaction request */
#define DMA_HS_LST_SRC_CH0_Pos              (0U)
#define DMA_HS_LST_SRC_CH0_Msk              (0x1UL << DMA_HS_LST_SRC_CH0_Pos)
#define DMA_HS_LST_SRC_CH0                  DMA_HS_LST_SRC_CH0_Msk
/** @brief DMA_HS_LST_SRC[1]: Channel 1 source last transaction request */
#define DMA_HS_LST_SRC_CH1_Pos              (1U)
#define DMA_HS_LST_SRC_CH1_Msk              (0x1UL << DMA_HS_LST_SRC_CH1_Pos)
#define DMA_HS_LST_SRC_CH1                  DMA_HS_LST_SRC_CH1_Msk
/** @brief DMA_HS_LST_SRC[2]: Channel 2 source last transaction request */
#define DMA_HS_LST_SRC_CH2_Pos              (2U)
#define DMA_HS_LST_SRC_CH2_Msk              (0x1UL << DMA_HS_LST_SRC_CH2_Pos)
#define DMA_HS_LST_SRC_CH2                  DMA_HS_LST_SRC_CH2_Msk
/** @brief DMA_HS_LST_SRC[3]: Channel 3 source last transaction request */
#define DMA_HS_LST_SRC_CH3_Pos              (3U)
#define DMA_HS_LST_SRC_CH3_Msk              (0x1UL << DMA_HS_LST_SRC_CH3_Pos)
#define DMA_HS_LST_SRC_CH3                  DMA_HS_LST_SRC_CH3_Msk
/** @brief DMA_HS_LST_SRC[4]: Channel 4 source last transaction request */
#define DMA_HS_LST_SRC_CH4_Pos              (4U)
#define DMA_HS_LST_SRC_CH4_Msk              (0x1UL << DMA_HS_LST_SRC_CH4_Pos)
#define DMA_HS_LST_SRC_CH4                  DMA_HS_LST_SRC_CH4_Msk
/** @brief DMA_HS_LST_SRC[5]: Channel 5 source last transaction request */
#define DMA_HS_LST_SRC_CH5_Pos              (5U)
#define DMA_HS_LST_SRC_CH5_Msk              (0x1UL << DMA_HS_LST_SRC_CH5_Pos)
#define DMA_HS_LST_SRC_CH5                  DMA_HS_LST_SRC_CH5_Msk
/** @brief DMA_HS_LST_SRC[6]: Channel 6 source last transaction request */
#define DMA_HS_LST_SRC_CH6_Pos              (6U)
#define DMA_HS_LST_SRC_CH6_Msk              (0x1UL << DMA_HS_LST_SRC_CH6_Pos)
#define DMA_HS_LST_SRC_CH6                  DMA_HS_LST_SRC_CH6_Msk
/** @brief DMA_HS_LST_SRC[7]: Channel 7 source last transaction request */
#define DMA_HS_LST_SRC_CH7_Pos              (7U)
#define DMA_HS_LST_SRC_CH7_Msk              (0x1UL << DMA_HS_LST_SRC_CH7_Pos)
#define DMA_HS_LST_SRC_CH7                  DMA_HS_LST_SRC_CH7_Msk
/** @brief DMA_HS_LST_SRC_WE[8]: Channel 0 source last transaction request write enable */
#define DMA_HS_LST_SRC_WE_CH0_Pos           (8U)
#define DMA_HS_LST_SRC_WE_CH0_Msk           (0x1UL << DMA_HS_LST_SRC_WE_CH0_Pos)
#define DMA_HS_LST_SRC_WE_CH0               DMA_HS_LST_SRC_WE_CH0_Msk
/** @brief DMA_HS_LST_SRC_WE[9]: Channel 1 source last transaction request write enable */
#define DMA_HS_LST_SRC_WE_CH1_Pos           (9U)
#define DMA_HS_LST_SRC_WE_CH1_Msk           (0x1UL << DMA_HS_LST_SRC_WE_CH1_Pos)
#define DMA_HS_LST_SRC_WE_CH1               DMA_HS_LST_SRC_WE_CH1_Msk
/** @brief DMA_HS_LST_SRC_WE[10]: Channel 2 source last transaction request write enable */
#define DMA_HS_LST_SRC_WE_CH2_Pos           (10U)
#define DMA_HS_LST_SRC_WE_CH2_Msk           (0x1UL << DMA_HS_LST_SRC_WE_CH2_Pos)
#define DMA_HS_LST_SRC_WE_CH2               DMA_HS_LST_SRC_WE_CH2_Msk
/** @brief DMA_HS_LST_SRC_WE[11]: Channel 3 source last transaction request write enable */
#define DMA_HS_LST_SRC_WE_CH3_Pos           (11U)
#define DMA_HS_LST_SRC_WE_CH3_Msk           (0x1UL << DMA_HS_LST_SRC_WE_CH3_Pos)
#define DMA_HS_LST_SRC_WE_CH3               DMA_HS_LST_SRC_WE_CH3_Msk
/** @brief DMA_HS_LST_SRC_WE[12]: Channel 4 source last transaction request write enable */
#define DMA_HS_LST_SRC_WE_CH4_Pos           (12U)
#define DMA_HS_LST_SRC_WE_CH4_Msk           (0x1UL << DMA_HS_LST_SRC_WE_CH4_Pos)
#define DMA_HS_LST_SRC_WE_CH4               DMA_HS_LST_SRC_WE_CH4_Msk
/** @brief DMA_HS_LST_SRC_WE[13]: Channel 5 source last transaction request write enable */
#define DMA_HS_LST_SRC_WE_CH5_Pos           (13U)
#define DMA_HS_LST_SRC_WE_CH5_Msk           (0x1UL << DMA_HS_LST_SRC_WE_CH5_Pos)
#define DMA_HS_LST_SRC_WE_CH5               DMA_HS_LST_SRC_WE_CH5_Msk
/** @brief DMA_HS_LST_SRC_WE[14]: Channel 6 source last transaction request write enable */
#define DMA_HS_LST_SRC_WE_CH6_Pos           (14U)
#define DMA_HS_LST_SRC_WE_CH6_Msk           (0x1UL << DMA_HS_LST_SRC_WE_CH6_Pos)
#define DMA_HS_LST_SRC_WE_CH6               DMA_HS_LST_SRC_WE_CH6_Msk
/** @brief DMA_HS_LST_SRC_WE[15]: Channel 7 source last transaction request write enable */
#define DMA_HS_LST_SRC_WE_CH7_Pos           (15U)
#define DMA_HS_LST_SRC_WE_CH7_Msk           (0x1UL << DMA_HS_LST_SRC_WE_CH7_Pos)
#define DMA_HS_LST_SRC_WE_CH7               DMA_HS_LST_SRC_WE_CH7_Msk

/****************  Bit definition for DMA_HS_LST_DST register  ****************/

/** @brief DMA_HS_LST_DST[0]: Channel 0 destination last transaction request */
#define DMA_HS_LST_DST_CH0_Pos              (0U)
#define DMA_HS_LST_DST_CH0_Msk              (0x1UL << DMA_HS_LST_DST_CH0_Pos)
#define DMA_HS_LST_DST_CH0                  DMA_HS_LST_DST_CH0_Msk
/** @brief DMA_HS_LST_DST[1]: Channel 1 destination last transaction request */
#define DMA_HS_LST_DST_CH1_Pos              (1U)
#define DMA_HS_LST_DST_CH1_Msk              (0x1UL << DMA_HS_LST_DST_CH1_Pos)
#define DMA_HS_LST_DST_CH1                  DMA_HS_LST_DST_CH1_Msk
/** @brief DMA_HS_LST_DST[2]: Channel 2 destination last transaction request */
#define DMA_HS_LST_DST_CH2_Pos              (2U)
#define DMA_HS_LST_DST_CH2_Msk              (0x1UL << DMA_HS_LST_DST_CH2_Pos)
#define DMA_HS_LST_DST_CH2                  DMA_HS_LST_DST_CH2_Msk
/** @brief DMA_HS_LST_DST[3]: Channel 3 destination last transaction request */
#define DMA_HS_LST_DST_CH3_Pos              (3U)
#define DMA_HS_LST_DST_CH3_Msk              (0x1UL << DMA_HS_LST_DST_CH3_Pos)
#define DMA_HS_LST_DST_CH3                  DMA_HS_LST_DST_CH3_Msk
/** @brief DMA_HS_LST_DST[4]: Channel 4 destination last transaction request */
#define DMA_HS_LST_DST_CH4_Pos              (4U)
#define DMA_HS_LST_DST_CH4_Msk              (0x1UL << DMA_HS_LST_DST_CH4_Pos)
#define DMA_HS_LST_DST_CH4                  DMA_HS_LST_DST_CH4_Msk
/** @brief DMA_HS_LST_DST[5]: Channel 5 destination last transaction request */
#define DMA_HS_LST_DST_CH5_Pos              (5U)
#define DMA_HS_LST_DST_CH5_Msk              (0x1UL << DMA_HS_LST_DST_CH5_Pos)
#define DMA_HS_LST_DST_CH5                  DMA_HS_LST_DST_CH5_Msk
/** @brief DMA_HS_LST_DST[6]: Channel 6 destination last transaction request */
#define DMA_HS_LST_DST_CH6_Pos              (6U)
#define DMA_HS_LST_DST_CH6_Msk              (0x1UL << DMA_HS_LST_DST_CH6_Pos)
#define DMA_HS_LST_DST_CH6                  DMA_HS_LST_DST_CH6_Msk
/** @brief DMA_HS_LST_DST[7]: Channel 7 destination last transaction request */
#define DMA_HS_LST_DST_CH7_Pos              (7U)
#define DMA_HS_LST_DST_CH7_Msk              (0x1UL << DMA_HS_LST_DST_CH7_Pos)
#define DMA_HS_LST_DST_CH7                  DMA_HS_LST_DST_CH7_Msk
/** @brief DMA_HS_LST_DST_WE[8]: Channel 0 destination last transaction request write enable */
#define DMA_HS_LST_DST_WE_CH0_Pos           (8U)
#define DMA_HS_LST_DST_WE_CH0_Msk           (0x1UL << DMA_HS_LST_DST_WE_CH0_Pos)
#define DMA_HS_LST_DST_WE_CH0               DMA_HS_LST_DST_WE_CH0_Msk
/** @brief DMA_HS_LST_DST_WE[9]: Channel 1 destination last transaction request write enable */
#define DMA_HS_LST_DST_WE_CH1_Pos           (9U)
#define DMA_HS_LST_DST_WE_CH1_Msk           (0x1UL << DMA_HS_LST_DST_WE_CH1_Pos)
#define DMA_HS_LST_DST_WE_CH1               DMA_HS_LST_DST_WE_CH1_Msk
/** @brief DMA_HS_LST_DST_WE[10]: Channel 2 destination last transaction request write enable */
#define DMA_HS_LST_DST_WE_CH2_Pos           (10U)
#define DMA_HS_LST_DST_WE_CH2_Msk           (0x1UL << DMA_HS_LST_DST_WE_CH2_Pos)
#define DMA_HS_LST_DST_WE_CH2               DMA_HS_LST_DST_WE_CH2_Msk
/** @brief DMA_HS_LST_DST_WE[11]: Channel 3 destination last transaction request write enable */
#define DMA_HS_LST_DST_WE_CH3_Pos           (11U)
#define DMA_HS_LST_DST_WE_CH3_Msk           (0x1UL << DMA_HS_LST_DST_WE_CH3_Pos)
#define DMA_HS_LST_DST_WE_CH3               DMA_HS_LST_DST_WE_CH3_Msk
/** @brief DMA_HS_LST_DST_WE[12]: Channel 4 destination last transaction request write enable */
#define DMA_HS_LST_DST_WE_CH4_Pos           (12U)
#define DMA_HS_LST_DST_WE_CH4_Msk           (0x1UL << DMA_HS_LST_DST_WE_CH4_Pos)
#define DMA_HS_LST_DST_WE_CH4               DMA_HS_LST_DST_WE_CH4_Msk
/** @brief DMA_HS_LST_DST_WE[13]: Channel 5 destination last transaction request write enable */
#define DMA_HS_LST_DST_WE_CH5_Pos           (13U)
#define DMA_HS_LST_DST_WE_CH5_Msk           (0x1UL << DMA_HS_LST_DST_WE_CH5_Pos)
#define DMA_HS_LST_DST_WE_CH5               DMA_HS_LST_DST_WE_CH5_Msk
/** @brief DMA_HS_LST_DST_WE[14]: Channel 6 destination last transaction request write enable */
#define DMA_HS_LST_DST_WE_CH6_Pos           (14U)
#define DMA_HS_LST_DST_WE_CH6_Msk           (0x1UL << DMA_HS_LST_DST_WE_CH6_Pos)
#define DMA_HS_LST_DST_WE_CH6               DMA_HS_LST_DST_WE_CH6_Msk
/** @brief DMA_HS_LST_DST_WE[15]: Channel 7 destination last transaction request write enable */
#define DMA_HS_LST_DST_WE_CH7_Pos           (15U)
#define DMA_HS_LST_DST_WE_CH7_Msk           (0x1UL << DMA_HS_LST_DST_WE_CH7_Pos)
#define DMA_HS_LST_DST_WE_CH7               DMA_HS_LST_DST_WE_CH7_Msk

/*****************  Bit definition for DMA_MSC_CFG register  ******************/

/** @brief DMA_MSC_CFG[0]: DMA controller enable */
#define DMA_MSC_CFG_EN_Pos                  (0U)
#define DMA_MSC_CFG_EN_Msk                  (0x1UL << DMA_MSC_CFG_EN_Pos)
#define DMA_MSC_CFG_EN                      DMA_MSC_CFG_EN_Msk

/****************  Bit definition for DMA_MSC_CH_EN register  *****************/

/** @brief DMA_MSC_CH_EN[0]: Channel 0 enable */
#define DMA_MSC_CH_EN_CH0_Pos               (0U)
#define DMA_MSC_CH_EN_CH0_Msk               (0x1UL << DMA_MSC_CH_EN_CH0_Pos)
#define DMA_MSC_CH_EN_CH0                   DMA_MSC_CH_EN_CH0_Msk
/** @brief DMA_MSC_CH_EN[1]: Channel 1 enable */
#define DMA_MSC_CH_EN_CH1_Pos               (1U)
#define DMA_MSC_CH_EN_CH1_Msk               (0x1UL << DMA_MSC_CH_EN_CH1_Pos)
#define DMA_MSC_CH_EN_CH1                   DMA_MSC_CH_EN_CH1_Msk
/** @brief DMA_MSC_CH_EN[2]: Channel 2 enable */
#define DMA_MSC_CH_EN_CH2_Pos               (2U)
#define DMA_MSC_CH_EN_CH2_Msk               (0x1UL << DMA_MSC_CH_EN_CH2_Pos)
#define DMA_MSC_CH_EN_CH2                   DMA_MSC_CH_EN_CH2_Msk
/** @brief DMA_MSC_CH_EN[3]: Channel 3 enable */
#define DMA_MSC_CH_EN_CH3_Pos               (3U)
#define DMA_MSC_CH_EN_CH3_Msk               (0x1UL << DMA_MSC_CH_EN_CH3_Pos)
#define DMA_MSC_CH_EN_CH3                   DMA_MSC_CH_EN_CH3_Msk
/** @brief DMA_MSC_CH_EN[4]: Channel 4 enable */
#define DMA_MSC_CH_EN_CH4_Pos               (4U)
#define DMA_MSC_CH_EN_CH4_Msk               (0x1UL << DMA_MSC_CH_EN_CH4_Pos)
#define DMA_MSC_CH_EN_CH4                   DMA_MSC_CH_EN_CH4_Msk
/** @brief DMA_MSC_CH_EN[5]: Channel 5 enable */
#define DMA_MSC_CH_EN_CH5_Pos               (5U)
#define DMA_MSC_CH_EN_CH5_Msk               (0x1UL << DMA_MSC_CH_EN_CH5_Pos)
#define DMA_MSC_CH_EN_CH5                   DMA_MSC_CH_EN_CH5_Msk
/** @brief DMA_MSC_CH_EN[6]: Channel 6 enable */
#define DMA_MSC_CH_EN_CH6_Pos               (6U)
#define DMA_MSC_CH_EN_CH6_Msk               (0x1UL << DMA_MSC_CH_EN_CH6_Pos)
#define DMA_MSC_CH_EN_CH6                   DMA_MSC_CH_EN_CH6_Msk
/** @brief DMA_MSC_CH_EN[7]: Channel 7 enable */
#define DMA_MSC_CH_EN_CH7_Pos               (7U)
#define DMA_MSC_CH_EN_CH7_Msk               (0x1UL << DMA_MSC_CH_EN_CH7_Pos)
#define DMA_MSC_CH_EN_CH7                   DMA_MSC_CH_EN_CH7_Msk
/** @brief DMA_MSC_CH_EN[8]: Channel 0 enable write enable */
#define DMA_MSC_CH_EN_WE_CH0_Pos            (8U)
#define DMA_MSC_CH_EN_WE_CH0_Msk            (0x1UL << DMA_MSC_CH_EN_WE_CH0_Pos)
#define DMA_MSC_CH_EN_WE_CH0                DMA_MSC_CH_EN_WE_CH0_Msk
/** @brief DMA_MSC_CH_EN[9]: Channel 1 enable write enable */
#define DMA_MSC_CH_EN_WE_CH1_Pos            (9U)
#define DMA_MSC_CH_EN_WE_CH1_Msk            (0x1UL << DMA_MSC_CH_EN_WE_CH1_Pos)
#define DMA_MSC_CH_EN_WE_CH1                DMA_MSC_CH_EN_WE_CH1_Msk
/** @brief DMA_MSC_CH_EN[10]: Channel 2 enable write enable */
#define DMA_MSC_CH_EN_WE_CH2_Pos            (10U)
#define DMA_MSC_CH_EN_WE_CH2_Msk            (0x1UL << DMA_MSC_CH_EN_WE_CH2_Pos)
#define DMA_MSC_CH_EN_WE_CH2                DMA_MSC_CH_EN_WE_CH2_Msk
/** @brief DMA_MSC_CH_EN[11]: Channel 3 enable write enable */
#define DMA_MSC_CH_EN_WE_CH3_Pos            (11U)
#define DMA_MSC_CH_EN_WE_CH3_Msk            (0x1UL << DMA_MSC_CH_EN_WE_CH3_Pos)
#define DMA_MSC_CH_EN_WE_CH3                DMA_MSC_CH_EN_WE_CH3_Msk
/** @brief DMA_MSC_CH_EN[12]: Channel 4 enable write enable */
#define DMA_MSC_CH_EN_WE_CH4_Pos            (12U)
#define DMA_MSC_CH_EN_WE_CH4_Msk            (0x1UL << DMA_MSC_CH_EN_WE_CH4_Pos)
#define DMA_MSC_CH_EN_WE_CH4                DMA_MSC_CH_EN_WE_CH4_Msk
/** @brief DMA_MSC_CH_EN[13]: Channel 5 enable write enable */
#define DMA_MSC_CH_EN_WE_CH5_Pos            (13U)
#define DMA_MSC_CH_EN_WE_CH5_Msk            (0x1UL << DMA_MSC_CH_EN_WE_CH5_Pos)
#define DMA_MSC_CH_EN_WE_CH5                DMA_MSC_CH_EN_WE_CH5_Msk
/** @brief DMA_MSC_CH_EN[14]: Channel 6 enable write enable */
#define DMA_MSC_CH_EN_WE_CH6_Pos            (14U)
#define DMA_MSC_CH_EN_WE_CH6_Msk            (0x1UL << DMA_MSC_CH_EN_WE_CH6_Pos)
#define DMA_MSC_CH_EN_WE_CH6                DMA_MSC_CH_EN_WE_CH6_Msk
/** @brief DMA_MSC_CH_EN[15]: Channel 7 enable write enable */
#define DMA_MSC_CH_EN_WE_CH7_Pos            (15U)
#define DMA_MSC_CH_EN_WE_CH7_Msk            (0x1UL << DMA_MSC_CH_EN_WE_CH7_Pos)
#define DMA_MSC_CH_EN_WE_CH7                DMA_MSC_CH_EN_WE_CH7_Msk

/******************  Bit definition for DMA_MSC_ID register  ******************/

/** @brief DMA_MSC_ID[31:0]: DMA controller peripheral ID */
#define DMA_MSC_ID_Pos                      (0U)
#define DMA_MSC_ID_Msk                      (0xFFFFFFFFUL << DMA_MSC_ID_Pos)
#define DMA_MSC_ID                          DMA_MSC_ID_Msk

/*****************  Bit definition for DMA_MSC_TEST register  *****************/

/** @brief DMA_MSC_TEST[0]: DMA test mode */
#define DMA_MSC_TEST_SLV_IF_Pos             (0U)
#define DMA_MSC_TEST_SLV_IF_Msk             (0x1UL << DMA_MSC_TEST_SLV_IF_Pos)
#define DMA_MSC_TEST_SLV_IF                 DMA_MSC_TEST_SLV_IF_Msk

/**************  Bit definition for DMA_MSC_COMP_TYPE register  ***************/

/** @brief DMA_MSC_COMP_TYPE[31:0]: DMA component type */
#define DMA_MSC_COMP_TYPE_Pos               (0U)
#define DMA_MSC_COMP_TYPE_Msk               (0xFFFFFFFFUL << DMA_MSC_COMP_TYPE_Pos)
#define DMA_MSC_COMP_TYPE                   DMA_MSC_COMP_TYPE_Msk

/*************  Bit definition for DMA_MSC_COMP_VERSION register  *************/

/** @brief DMA_MSC_COMP_VERSION[31:0]: DMA component version */
#define DMA_MSC_COMP_VERSION_Pos            (0U)
#define DMA_MSC_COMP_VERSION_Msk            (0xFFFFFFFFUL << DMA_MSC_COMP_VERSION_Pos)
#define DMA_MSC_COMP_VERSION                DMA_MSC_COMP_VERSION_Msk

/******************************************************************************/
/*                               Embedded Flash                               */
/******************************************************************************/

/******************  Bit definition for EFLASH_CTRL register  *****************/

/** @brief CTRL[0]: Run command */
#define EFLASH_CTRL_RUN_Pos                 (0U)
#define EFLASH_CTRL_RUN_Msk                 (0x1UL << EFLASH_CTRL_RUN_Pos)
#define EFLASH_CTRL_RUN                     EFLASH_CTRL_RUN_Msk

/** @brief CTRL[3:1]: Operation code */
#define EFLASH_CTRL_OP_CODE_Pos             (1U)
#define EFLASH_CTRL_OP_CODE_Msk             (0x7UL << EFLASH_CTRL_OP_CODE_Pos)
#define EFLASH_CTRL_OP_CODE                 EFLASH_CTRL_OP_CODE_Msk

#define EFLASH_CTRL_OP_CODE_CFG             (0x0UL << EFLASH_CTRL_OP_CODE_Pos)  /*!< CFG: The flash memory configuration */
#define EFLASH_CTRL_OP_CODE_WRITE           (0x1UL << EFLASH_CTRL_OP_CODE_Pos)  /*!< WRITE: Write a word to the flash memory */
#define EFLASH_CTRL_OP_CODE_READ            (0x2UL << EFLASH_CTRL_OP_CODE_Pos)  /*!< READ: Read a word from the flash memory */
#define EFLASH_CTRL_OP_CODE_CHIP_ERASE      (0x3UL << EFLASH_CTRL_OP_CODE_Pos)  /*!< CHIP_ERASE: Chip erase */
#define EFLASH_CTRL_OP_CODE_BLOCK_ERASE     (0x4UL << EFLASH_CTRL_OP_CODE_Pos)  /*!< BLOCK_ERASE: Block erase*/
#define EFLASH_CTRL_OP_CODE_SECTOR_ERASE    (0x5UL << EFLASH_CTRL_OP_CODE_Pos)  /*!< SECTOR_ERASE: Sector erase */
#define EFLASH_CTRL_OP_CODE_ENTER_DPD       (0x6UL << EFLASH_CTRL_OP_CODE_Pos)  /*!< ENTER_DPD: Enter Deep Power Down mode */
#define EFLASH_CTRL_OP_CODE_EXIT_DPD        (0x7UL << EFLASH_CTRL_OP_CODE_Pos)  /*!< EXIT_DPD: Exit Deep Power Down mode */

/** @brief CTRL[4]: Run OP_CODE in NVR array */
#define EFLASH_CTRL_NVR_Pos                 (4U)
#define EFLASH_CTRL_NVR_Msk                 (0x1UL << EFLASH_CTRL_NVR_Pos)
#define EFLASH_CTRL_NVR                     EFLASH_CTRL_NVR_Msk
/** @brief CTRL[5]: Switch to manual control */
#define EFLASH_CTRL_MANUAL_MODE_Pos         (5U)
#define EFLASH_CTRL_MANUAL_MODE_Msk         (0x1UL << EFLASH_CTRL_MANUAL_MODE_Pos)
#define EFLASH_CTRL_MANUAL_MODE             EFLASH_CTRL_MANUAL_MODE_Msk

/******************  Bit definition for EFLASH_ADDR register  *****************/

/** @brief ADDR[15:0]: Address for a command */
#define EFLASH_ADDR_Pos                     (0U)
#define EFLASH_ADDR_Msk                     (0xFFFFUL << EFLASH_ADDR_Pos)
#define EFLASH_ADDR                         EFLASH_ADDR_Msk

/*****************  Bit definition for EFLASH_WDATA register  *****************/

/** @brief WDATA[31:0]: Data to WRITE */
#define EFLASH_WDATA_Pos                    (0U)
#define EFLASH_WDATA_Msk                    (0xFFFFFFFFUL << EFLASH_WDATA_Pos)
#define EFLASH_WDATA                        EFLASH_WDATA_Msk

/*****************  Bit definition for EFLASH_RDATA register  *****************/

/** @brief RDATA[31:0]: READ data */
#define EFLASH_RDATA_Pos                    (0U)
#define EFLASH_RDATA_Msk                    (0xFFFFFFFFUL << EFLASH_RDATA_Pos)
#define EFLASH_RDATA                        EFLASH_RDATA_Msk

/*****************  Bit definition for EFLASH_STATUS register  ****************/

/** @brief STATUS[0]: Busy flag */
#define EFLASH_STATUS_BUSY_Pos              (0U)
#define EFLASH_STATUS_BUSY_Msk              (0x1UL << EFLASH_STATUS_BUSY_Pos)
#define EFLASH_STATUS_BUSY                  EFLASH_STATUS_BUSY_Msk

/*****************  Bit definition for EFLASH_TIME0 register  *****************/

/** @brief TIME0[3:0]: Data to PROG2 High setup time (min. 15ns) */
#define EFLASH_TIME0_DS_T_Pos               (0U)
#define EFLASH_TIME0_DS_T_Msk               (0xFUL << EFLASH_TIME0_DS_T_Pos)
#define EFLASH_TIME0_DS_T                   EFLASH_TIME0_DS_T_Msk
/** @brief TIME0[26:4]: Chip Erase time (8ms to 10ms) */
#define EFLASH_TIME0_SCE_T_Pos              (4U)
#define EFLASH_TIME0_SCE_T_Msk              (0x7FFFFFUL << EFLASH_TIME0_SCE_T_Pos)
#define EFLASH_TIME0_SCE_T                  EFLASH_TIME0_SCE_T_Msk
/** @brief TIME0[31:27]: AE access time (max. 25ns) */
#define EFLASH_TIME0_AA_T_Pos               (27U)
#define EFLASH_TIME0_AA_T_Msk               (0x1FUL << EFLASH_TIME0_AA_T_Pos)
#define EFLASH_TIME0_AA_T                   EFLASH_TIME0_AA_T_Msk

/*****************  Bit definition for EFLASH_TIME1 register  *****************/

/** @brief TIME1[1:0]: Address and ARRDN/NVR setup time (min. 2ns) */
#define EFLASH_TIME1_AS_T_Pos               (0U)
#define EFLASH_TIME1_AS_T_Msk               (0x3UL << EFLASH_TIME1_AS_T_Pos)
#define EFLASH_TIME1_AS_T                   EFLASH_TIME1_AS_T_Msk
/** @brief TIME1[15:2]: PORb High to any operation time (min. 10us) */
#define EFLASH_TIME1_RHR_T_Pos              (2U)
#define EFLASH_TIME1_RHR_T_Msk              (0x3FFFUL << EFLASH_TIME1_RHR_T_Pos)
#define EFLASH_TIME1_RHR_T                  EFLASH_TIME1_RHR_T_Msk
/** @brief TIME1[31:16]: Erase recovery time (min. 50us) */
#define EFLASH_TIME1_RCV_T_Pos              (16U)
#define EFLASH_TIME1_RCV_T_Msk              (0xFFFFUL << EFLASH_TIME1_RCV_T_Pos)
#define EFLASH_TIME1_RCV_T                  EFLASH_TIME1_RCV_T_Msk

/*****************  Bit definition for EFLASH_TIME2 register  *****************/

/** @brief TIME2[6:0]: Latency to next operation after PROG/ERASE Low time (min. 100ns) */
#define EFLASH_TIME2_RW_T_Pos               (0U)
#define EFLASH_TIME2_RW_T_Msk               (0x7FUL << EFLASH_TIME2_RW_T_Pos)
#define EFLASH_TIME2_RW_T                   EFLASH_TIME2_RW_T_Msk
/** @brief TIME2[19:7]: Program time (8us to 10us) */
#define EFLASH_TIME2_PROG_T_Pos             (7U)
#define EFLASH_TIME2_PROG_T_Msk             (0x1FFFUL << EFLASH_TIME2_PROG_T_Pos)
#define EFLASH_TIME2_PROG_T                 EFLASH_TIME2_PROG_T_Msk
/** @brief TIME2[31:20]: Program First AE / Sector Erase AE / Block Erase AE to WEb setup time (min. 4us) */
#define EFLASH_TIME2_NVS_T_Pos              (20U)
#define EFLASH_TIME2_NVS_T_Msk              (0xFFFUL << EFLASH_TIME2_NVS_T_Pos)
#define EFLASH_TIME2_NVS_T                  EFLASH_TIME2_NVS_T_Msk

/*****************  Bit definition for EFLASH_TIME3 register  *****************/

/** @brief TIME3[11:0]: Pre-Program time (2.5us to 3.5us) */
#define EFLASH_TIME3_PREPROG_T_Pos          (0U)
#define EFLASH_TIME3_PREPROG_T_Msk          (0xFFFUL << EFLASH_TIME3_PREPROG_T_Pos)
#define EFLASH_TIME3_PREPROG_T              EFLASH_TIME3_PREPROG_T_Msk
/** @brief TIME3[16:12]: Configuration register data setup time (max. 20ns) */
#define EFLASH_TIME3_CBD_T_Pos              (12U)
#define EFLASH_TIME3_CBD_T_Msk              (0x1FUL << EFLASH_TIME3_CBD_T_Pos)
#define EFLASH_TIME3_CBD_T                  EFLASH_TIME3_CBD_T_Msk
/** @brief TIME3[31:17]: WEb Low to PROG2 High setup time (min. 20us) */
#define EFLASH_TIME3_PGS_T_Pos              (17U)
#define EFLASH_TIME3_PGS_T_Msk              (0x7FFFUL << EFLASH_TIME3_PGS_T_Pos)
#define EFLASH_TIME3_PGS_T                  EFLASH_TIME3_PGS_T_Msk

/*****************  Bit definition for EFLASH_TIME4 register  *****************/

/** @brief TIME4[3:0]: RECALL to AE time (min. 10ns) */
#define EFLASH_TIME4_RS_T_Pos               (0U)
#define EFLASH_TIME4_RS_T_Msk               (0xFUL << EFLASH_TIME4_RS_T_Pos)
#define EFLASH_TIME4_RS_T                   EFLASH_TIME4_RS_T_Msk
/** @brief TIME4[25:4]: Block Erase time (4ms to 6ms) */
#define EFLASH_TIME4_SBE_T_Pos              (4U)
#define EFLASH_TIME4_SBE_T_Msk              (0x3FFFFFUL << EFLASH_TIME4_SBE_T_Pos)
#define EFLASH_TIME4_SBE_T                  EFLASH_TIME4_SBE_T_Msk
/** @brief TIME4[31:26]: Test mode inputs setup time (min. 50ns) */
#define EFLASH_TIME4_TEST_T_Pos             (26U)
#define EFLASH_TIME4_TEST_T_Msk             (0x3FUL << EFLASH_TIME4_TEST_T_Pos)
#define EFLASH_TIME4_TEST_T                 EFLASH_TIME4_TEST_T_Msk

/*****************  Bit definition for EFLASH_TIME5 register  *****************/

/** @brief TIME5[21:0]: Sector Erase time (3.2ms to 4ms) */
#define EFLASH_TIME5_ERASE_T_Pos            (0U)
#define EFLASH_TIME5_ERASE_T_Msk            (0x3FFFFFUL << EFLASH_TIME5_ERASE_T_Pos)
#define EFLASH_TIME5_ERASE_T                EFLASH_TIME5_ERASE_T_Msk
/** @brief TIME5[29:22]: Recall Read cycle time (min. 200ns) */
#define EFLASH_TIME5_RCR_T_Pos              (22U)
#define EFLASH_TIME5_RCR_T_Msk              (0xFFUL << EFLASH_TIME5_RCR_T_Pos)
#define EFLASH_TIME5_RCR_T                  EFLASH_TIME5_RCR_T_Msk

/*****************  Bit definition for EFLASH_TIME6 register  *****************/

/** @brief TIME6[12:0]: Exit Pre-Program to PROG2 High setup time (min. 5us) */
#define EFLASH_TIME6_PREPGS_T_Pos           (0U)
#define EFLASH_TIME6_PREPGS_T_Msk           (0x1FFFUL << EFLASH_TIME6_PREPGS_T_Pos)
#define EFLASH_TIME6_PREPGS_T               EFLASH_TIME6_PREPGS_T_Msk

/*****************  Bit definition for EFLASH_MANUAL register  ****************/

/** @brief MANUAL[0]: AE input control */
#define EFLASH_MANUAL_AE_Pos                (0U)
#define EFLASH_MANUAL_AE_Msk                (0x1UL << EFLASH_MANUAL_AE_Pos)
#define EFLASH_MANUAL_AE                    EFLASH_MANUAL_AE_Msk
/** @brief MANUAL[1]: RECALL input control */
#define EFLASH_MANUAL_RECALL_Pos            (1U)
#define EFLASH_MANUAL_RECALL_Msk            (0x1UL << EFLASH_MANUAL_RECALL_Pos)
#define EFLASH_MANUAL_RECALL                EFLASH_MANUAL_RECALL_Msk
/** @brief MANUAL[2]: NVR input control */
#define EFLASH_MANUAL_NVR_Pos               (2U)
#define EFLASH_MANUAL_NVR_Msk               (0x1UL << EFLASH_MANUAL_NVR_Pos)
#define EFLASH_MANUAL_NVR                   EFLASH_MANUAL_NVR_Msk
/** @brief MANUAL[3]: CEB input control */
#define EFLASH_MANUAL_CEB_Pos               (3U)
#define EFLASH_MANUAL_CEB_Msk               (0x1UL << EFLASH_MANUAL_CEB_Pos)
#define EFLASH_MANUAL_CEB                   EFLASH_MANUAL_CEB_Msk
/** @brief MANUAL[4]: WEB input control */
#define EFLASH_MANUAL_WEB_Pos               (4U)
#define EFLASH_MANUAL_WEB_Msk               (0x1UL << EFLASH_MANUAL_WEB_Pos)
#define EFLASH_MANUAL_WEB                   EFLASH_MANUAL_WEB_Msk
/** @brief MANUAL[5]: PROG input control */
#define EFLASH_MANUAL_PROG_Pos              (5U)
#define EFLASH_MANUAL_PROG_Msk              (0x1UL << EFLASH_MANUAL_PROG_Pos)
#define EFLASH_MANUAL_PROG                  EFLASH_MANUAL_PROG_Msk
/** @brief MANUAL[6]: PROG2 input control */
#define EFLASH_MANUAL_PROG2_Pos             (6U)
#define EFLASH_MANUAL_PROG2_Msk             (0x1UL << EFLASH_MANUAL_PROG2_Pos)
#define EFLASH_MANUAL_PROG2                 EFLASH_MANUAL_PROG2_Msk
/** @brief MANUAL[7]: ERASE input control */
#define EFLASH_MANUAL_ERASE_Pos             (7U)
#define EFLASH_MANUAL_ERASE_Msk             (0x1UL << EFLASH_MANUAL_ERASE_Pos)
#define EFLASH_MANUAL_ERASE                 EFLASH_MANUAL_ERASE_Msk
/** @brief MANUAL[8]: CHIP input control */
#define EFLASH_MANUAL_CHIP_Pos              (8U)
#define EFLASH_MANUAL_CHIP_Msk              (0x1UL << EFLASH_MANUAL_CHIP_Pos)
#define EFLASH_MANUAL_CHIP                  EFLASH_MANUAL_CHIP_Msk
/** @brief MANUAL[9]: CONFEN input control */
#define EFLASH_MANUAL_CONFEN_Pos            (9U)
#define EFLASH_MANUAL_CONFEN_Msk            (0x1UL << EFLASH_MANUAL_CONFEN_Pos)
#define EFLASH_MANUAL_CONFEN                EFLASH_MANUAL_CONFEN_Msk
/** @brief MANUAL[10]: DPD input control */
#define EFLASH_MANUAL_DPD_Pos               (10U)
#define EFLASH_MANUAL_DPD_Msk               (0x1UL << EFLASH_MANUAL_DPD_Pos)
#define EFLASH_MANUAL_DPD                   EFLASH_MANUAL_DPD_Msk
/** @brief MANUAL[11]: PORB input control */
#define EFLASH_MANUAL_PORB_Pos              (11U)
#define EFLASH_MANUAL_PORB_Msk              (0x1UL << EFLASH_MANUAL_PORB_Pos)
#define EFLASH_MANUAL_PORB                  EFLASH_MANUAL_PORB_Msk
/** @brief MANUAL[12]: ARRDN input control */
#define EFLASH_MANUAL_ARRDN_Pos             (12U)
#define EFLASH_MANUAL_ARRDN_Msk             (0x1UL << EFLASH_MANUAL_ARRDN_Pos)
#define EFLASH_MANUAL_ARRDN                 EFLASH_MANUAL_ARRDN_Msk
/** @brief MANUAL[13]: TMEN input control */
#define EFLASH_MANUAL_TMEN_Pos              (13U)
#define EFLASH_MANUAL_TMEN_Msk              (0x1UL << EFLASH_MANUAL_TMEN_Pos)
#define EFLASH_MANUAL_TMEN                  EFLASH_MANUAL_TMEN_Msk
/** @brief MANUAL[14]: VREAD0 input control */
#define EFLASH_MANUAL_VREAD0_Pos            (14U)
#define EFLASH_MANUAL_VREAD0_Msk            (0x1UL << EFLASH_MANUAL_VREAD0_Pos)
#define EFLASH_MANUAL_VREAD0                EFLASH_MANUAL_VREAD0_Msk
/** @brief MANUAL[15]: VREAD1 input control */
#define EFLASH_MANUAL_VREAD1_Pos            (15U)
#define EFLASH_MANUAL_VREAD1_Msk            (0x1UL << EFLASH_MANUAL_VREAD1_Pos)
#define EFLASH_MANUAL_VREAD1                EFLASH_MANUAL_VREAD1_Msk
/** @brief MANUAL[16]: PREPG input control */
#define EFLASH_MANUAL_PREPG_Pos             (16U)
#define EFLASH_MANUAL_PREPG_Msk             (0x1UL << EFLASH_MANUAL_PREPG_Pos)
#define EFLASH_MANUAL_PREPG                 EFLASH_MANUAL_PREPG_Msk
/** @brief MANUAL[17]: BLOK input control */
#define EFLASH_MANUAL_BLOK_Pos              (17U)
#define EFLASH_MANUAL_BLOK_Msk              (0x1UL << EFLASH_MANUAL_BLOK_Pos)
#define EFLASH_MANUAL_BLOK                  EFLASH_MANUAL_BLOK_Msk

/***************  Bit definition for EFLASH_BAD_ADDR0 register  ***************/

/** @brief BAD_ADDR0[7:0]: EFLASH 0 bad sector address 0 */
#define EFLASH_BAD_ADDR0_0_Pos              (0U)
#define EFLASH_BAD_ADDR0_0_Msk              (0xFFUL << EFLASH_BAD_ADDR0_0_Pos)
#define EFLASH_BAD_ADDR0_0                  EFLASH_BAD_ADDR0_0_Msk
/** @brief BAD_ADDR0[15:8]: EFLASH 0 bad sector address 1 */
#define EFLASH_BAD_ADDR0_1_Pos              (8U)
#define EFLASH_BAD_ADDR0_1_Msk              (0xFFUL << EFLASH_BAD_ADDR0_1_Pos)
#define EFLASH_BAD_ADDR0_1                  EFLASH_BAD_ADDR0_1_Msk
/** @brief BAD_ADDR0[23:16]: EFLASH 0 bad sector address 2 */
#define EFLASH_BAD_ADDR0_2_Pos              (16U)
#define EFLASH_BAD_ADDR0_2_Msk              (0xFFUL << EFLASH_BAD_ADDR0_2_Pos)
#define EFLASH_BAD_ADDR0_2                  EFLASH_BAD_ADDR0_2_Msk
/** @brief BAD_ADDR0[31:24]: EFLASH 0 bad sector address 3 */
#define EFLASH_BAD_ADDR0_3_Pos              (24U)
#define EFLASH_BAD_ADDR0_3_Msk              (0xFFUL << EFLASH_BAD_ADDR0_3_Pos)
#define EFLASH_BAD_ADDR0_3                  EFLASH_BAD_ADDR0_3_Msk

/***************  Bit definition for EFLASH_BAD_ADDR1 register  ***************/

/** @brief BAD_ADDR1[7:0]: EFLASH 1 bad sector address 0 */
#define EFLASH_BAD_ADDR1_0_Pos              (0U)
#define EFLASH_BAD_ADDR1_0_Msk              (0xFFUL << EFLASH_BAD_ADDR1_0_Pos)
#define EFLASH_BAD_ADDR1_0                  EFLASH_BAD_ADDR1_0_Msk
/** @brief BAD_ADDR1[15:8]: EFLASH 1 bad sector address 1 */
#define EFLASH_BAD_ADDR1_1_Pos              (8U)
#define EFLASH_BAD_ADDR1_1_Msk              (0xFFUL << EFLASH_BAD_ADDR1_1_Pos)
#define EFLASH_BAD_ADDR1_1                  EFLASH_BAD_ADDR1_1_Msk
/** @brief BAD_ADDR1[23:16]: EFLASH 1 bad sector address 2 */
#define EFLASH_BAD_ADDR1_2_Pos              (16U)
#define EFLASH_BAD_ADDR1_2_Msk              (0xFFUL << EFLASH_BAD_ADDR1_2_Pos)
#define EFLASH_BAD_ADDR1_2                  EFLASH_BAD_ADDR1_2_Msk
/** @brief BAD_ADDR1[31:24]: EFLASH 1 bad sector address 3 */
#define EFLASH_BAD_ADDR1_3_Pos              (24U)
#define EFLASH_BAD_ADDR1_3_Msk              (0xFFUL << EFLASH_BAD_ADDR1_3_Pos)
#define EFLASH_BAD_ADDR1_3                  EFLASH_BAD_ADDR1_3_Msk

/***************  Bit definition for EFLASH_CFG_FLAGS register  ***************/

/** @brief CFG_FLAGS[0]: EFLASH 0 bad sector address 0 validity flag */
#define EFLASH_CFG_FLAGS_BAD_ADDR0_VAL0_Pos (0U)
#define EFLASH_CFG_FLAGS_BAD_ADDR0_VAL0_Msk (0x1UL << EFLASH_CFG_FLAGS_BAD_ADDR0_VAL0_Pos)
#define EFLASH_CFG_FLAGS_BAD_ADDR0_VAL0     EFLASH_CFG_FLAGS_BAD_ADDR0_VAL0_Msk
/** @brief CFG_FLAGS[1]: EFLASH 0 bad sector address 1 validity flag */
#define EFLASH_CFG_FLAGS_BAD_ADDR0_VAL1_Pos (1U)
#define EFLASH_CFG_FLAGS_BAD_ADDR0_VAL1_Msk (0x1UL << EFLASH_CFG_FLAGS_BAD_ADDR0_VAL1_Pos)
#define EFLASH_CFG_FLAGS_BAD_ADDR0_VAL1     EFLASH_CFG_FLAGS_BAD_ADDR0_VAL1_Msk
/** @brief CFG_FLAGS[2]: EFLASH 0 bad sector address 2 validity flag */
#define EFLASH_CFG_FLAGS_BAD_ADDR0_VAL2_Pos (2U)
#define EFLASH_CFG_FLAGS_BAD_ADDR0_VAL2_Msk (0x1UL << EFLASH_CFG_FLAGS_BAD_ADDR0_VAL2_Pos)
#define EFLASH_CFG_FLAGS_BAD_ADDR0_VAL2     EFLASH_CFG_FLAGS_BAD_ADDR0_VAL2_Msk
/** @brief CFG_FLAGS[3]: EFLASH 0 bad sector address 3 validity flag */
#define EFLASH_CFG_FLAGS_BAD_ADDR0_VAL3_Pos (3U)
#define EFLASH_CFG_FLAGS_BAD_ADDR0_VAL3_Msk (0x1UL << EFLASH_CFG_FLAGS_BAD_ADDR0_VAL3_Pos)
#define EFLASH_CFG_FLAGS_BAD_ADDR0_VAL3     EFLASH_CFG_FLAGS_BAD_ADDR0_VAL3_Msk
/** @brief CFG_FLAGS[4]: EFLASH 1 bad sector address 0 validity flag */
#define EFLASH_CFG_FLAGS_BAD_ADDR1_VAL0_Pos (4U)
#define EFLASH_CFG_FLAGS_BAD_ADDR1_VAL0_Msk (0x1UL << EFLASH_CFG_FLAGS_BAD_ADDR1_VAL0_Pos)
#define EFLASH_CFG_FLAGS_BAD_ADDR1_VAL0     EFLASH_CFG_FLAGS_BAD_ADDR1_VAL0_Msk
/** @brief CFG_FLAGS[5]: EFLASH 1 bad sector address 1 validity flag */
#define EFLASH_CFG_FLAGS_BAD_ADDR1_VAL1_Pos (5U)
#define EFLASH_CFG_FLAGS_BAD_ADDR1_VAL1_Msk (0x1UL << EFLASH_CFG_FLAGS_BAD_ADDR1_VAL1_Pos)
#define EFLASH_CFG_FLAGS_BAD_ADDR1_VAL1     EFLASH_CFG_FLAGS_BAD_ADDR1_VAL1_Msk
/** @brief CFG_FLAGS[6]: EFLASH 1 bad sector address 2 validity flag */
#define EFLASH_CFG_FLAGS_BAD_ADDR1_VAL2_Pos (6U)
#define EFLASH_CFG_FLAGS_BAD_ADDR1_VAL2_Msk (0x1UL << EFLASH_CFG_FLAGS_BAD_ADDR1_VAL2_Pos)
#define EFLASH_CFG_FLAGS_BAD_ADDR1_VAL2     EFLASH_CFG_FLAGS_BAD_ADDR1_VAL2_Msk
/** @brief CFG_FLAGS[7]: EFLASH 1 bad sector address 3 validity flag */
#define EFLASH_CFG_FLAGS_BAD_ADDR1_VAL3_Pos (7U)
#define EFLASH_CFG_FLAGS_BAD_ADDR1_VAL3_Msk (0x1UL << EFLASH_CFG_FLAGS_BAD_ADDR1_VAL3_Pos)
#define EFLASH_CFG_FLAGS_BAD_ADDR1_VAL3     EFLASH_CFG_FLAGS_BAD_ADDR1_VAL3_Msk
/** @brief CFG_FLAGS[8]: EFLASH 0 configuration flag error */
#define EFLASH_CFG_FLAGS_CFG_FLAG_ERR0_Pos  (8U)
#define EFLASH_CFG_FLAGS_CFG_FLAG_ERR0_Msk  (0x1UL << EFLASH_CFG_FLAGS_CFG_FLAG_ERR0_Pos)
#define EFLASH_CFG_FLAGS_CFG_FLAG_ERR0      EFLASH_CFG_FLAGS_CFG_FLAG_ERR0_Msk
/** @brief CFG_FLAGS[9]: EFLASH 1 configuration flag error */
#define EFLASH_CFG_FLAGS_CFG_FLAG_ERR1_Pos  (9U)
#define EFLASH_CFG_FLAGS_CFG_FLAG_ERR1_Msk  (0x1UL << EFLASH_CFG_FLAGS_CFG_FLAG_ERR1_Pos)
#define EFLASH_CFG_FLAGS_CFG_FLAG_ERR1      EFLASH_CFG_FLAGS_CFG_FLAG_ERR1_Msk
/** @brief CFG_FLAGS[10]: EFLASH 0 configuration register error */
#define EFLASH_CFG_FLAGS_CFG_READ_ERR0_Pos  (10U)
#define EFLASH_CFG_FLAGS_CFG_READ_ERR0_Msk  (0x1UL << EFLASH_CFG_FLAGS_CFG_READ_ERR0_Pos)
#define EFLASH_CFG_FLAGS_CFG_READ_ERR0      EFLASH_CFG_FLAGS_CFG_READ_ERR0_Msk
/** @brief CFG_FLAGS[11]: EFLASH 1 configuration register error */
#define EFLASH_CFG_FLAGS_CFG_READ_ERR1_Pos  (11U)
#define EFLASH_CFG_FLAGS_CFG_READ_ERR1_Msk  (0x1UL << EFLASH_CFG_FLAGS_CFG_READ_ERR1_Pos)
#define EFLASH_CFG_FLAGS_CFG_READ_ERR1      EFLASH_CFG_FLAGS_CFG_READ_ERR1_Msk

/******************************************************************************/
/*                            General Purpose I/O                             */
/******************************************************************************/

/*** Bits definition for DR register ******************************************/
/* @brief DR[15:0]: Port data value */
#define GPIO_DR_Pos                         (0U)
#define GPIO_DR_Msk                         (0xFFFFUL << GPIO_DR_Pos)
#define GPIO_DR                             GPIO_DR_Msk

/*** Bits definition for DDR register *****************************************/
/* @brief DDR[15:0]: Port data direction */
#define GPIO_DDR_Pos                        (0U)
#define GPIO_DDR_Msk                        (0xFFFFUL << GPIO_DDR_Pos)
#define GPIO_DDR                            GPIO_DDR_Msk

/*** Bits definition for INTEN register **************************************/
/* @brief INTEN[15:0]: Port bits interrupts enable */
#define GPIO_INTEN_Pos                      (0U)
#define GPIO_INTEN_Msk                      (0xFFFFUL << GPIO_INTEN_Pos)
#define GPIO_INTEN                          GPIO_INTEN_Msk

/*** Bits definition for INTMSK register ************************************/
/* @brief INTMSK[15:0]: Port bits interrupt masking */
#define GPIO_INTMSK_Pos                     (0U)
#define GPIO_INTMSK_Msk                     (0xFFFFUL << GPIO_INTMSK_Pos)
#define GPIO_INTMSK                         GPIO_INTMSK_Msk

/*** Bits definition for INTLVL register ***********************************/
/* @brief INTLVL[15:0]: Port bits interrupt edge/level mode */
#define GPIO_INTTYPELVL_Pos                 (0U)
#define GPIO_INTTYPELVL_Msk                 (0xFFFFUL << GPIO_INTTYPELVL_Pos)
#define GPIO_INTTYPELVL                     GPIO_INTTYPELVL_Msk

/*** Bits definition for INTPOLARITY register ********************************/
/* @brief INTPOLARITY[15:0]: Polarity of edge or level sensitivity */
#define GPIO_INTPOLARITY_Pos                (0U)
#define GPIO_INTPOLARITY_Msk                (0xFFFFUL << GPIO_INTPOLARITY_Pos)
#define GPIO_INTPOLARITY                    GPIO_INTPOLARITY_Msk

/*** Bits definition for INTSTAT register ***********************************/
/* @brief INTSTAT[15:0]: Interrupts status */
#define GPIO_INTSTAT_Pos                    (0U)
#define GPIO_INTSTAT_Msk                    (0xFFFFUL << GPIO_INTSTAT_Pos)
#define GPIO_INTSTAT                        GPIO_INTSTAT_Msk

/*** Bits definition for INTSTATRAW register ******************************/
/* @brief INTSTATRAW[15:0]: Raw interrupts status */
#define GPIO_INTSTATRAW_Pos                 (0U)
#define GPIO_INTSTATRAW_Msk                 (0xFFFFUL << GPIO_INTSTATRAW_Pos)
#define GPIO_INTSTATRAW                     GPIO_INTSTATRAW_Msk

/*** Bits definition for DEBOUNCE register ************************************/
/* @brief DEBOUNCE[15:0]: Debounce enable */
#define GPIO_DEBOUNCE_Pos                   (0U)
#define GPIO_DEBOUNCE_Msk                   (0xFFFFUL << GPIO_DEBOUNCE_Pos)
#define GPIO_DEBOUNCE                       GPIO_DEBOUNCE_Msk

/*** Bits definition for EOI register *****************************************/
/* @brief EOI[15:0]: Clear interrupts */
#define GPIO_EOI_Pos                        (0U)
#define GPIO_EOI_Msk                        (0xFFFFUL << GPIO_EOI_Pos)
#define GPIO_EOI                            GPIO_EOI_Msk

/*** Bits definition for EXT register *****************************************/
/* @brief EXT[15:0]: Eternal port */
#define GPIO_EXT_Pos                        (0U)
#define GPIO_EXT_Msk                        (0xFFFFUL << GPIO_EXT_Pos)
#define GPIO_EXT                            GPIO_EXT_Msk

/*** Bits definition for SYNC register ****************************************/
/* @brief SYNC[1:0]: Synchronization level */
#define GPIO_SYNC_Pos                       (0U)
#define GPIO_SYNC_Msk                       (0x1UL << GPIO_SYNC_Pos)
#define GPIO_SYNC                           GPIO_SYNC_Msk

/*** Bits definition for BOTHEDGE register ************************************/
/* @brief BOTHEDGE[15:0]: Both Edge type */
#define GPIO_BOTHEDGE_Pos                   (0U)
#define GPIO_BOTHEDGE_Msk                   (0xFFFFUL << GPIO_BOTHEDGE_Pos)
#define GPIO_BOTHEDGE                       GPIO_BOTHEDGE_Msk

/******************************************************************************/
/*                     Inter Integrated Circuit Interface                     */
/******************************************************************************/

/********************  Bit definition for I2C_CON register  *******************/

/** @brief CON[0]: Master mode enable */
#define I2C_CON_MASTER_MODE_Pos             (0U)
#define I2C_CON_MASTER_MODE_Msk             (0x1UL << I2C_CON_MASTER_MODE_Pos)
#define I2C_CON_MASTER_MODE                 I2C_CON_MASTER_MODE_Msk

/** @brief CON[2:1]: Bus speed */
#define I2C_CON_SPEED_Pos                   (1U)
#define I2C_CON_SPEED_Msk                   (0x3UL << I2C_CON_SPEED_Pos)
#define I2C_CON_SPEED                       I2C_CON_SPEED_Msk

#define I2C_CON_SPEED_STANDARD              (0x1UL << I2C_CON_SPEED_Pos)    /*!< Standard speed mode of operation */
#define I2C_CON_SPEED_FAST                  (0x2UL << I2C_CON_SPEED_Pos)    /*!< Fast or Fast Plus speed mode of operation */
#define I2C_CON_SPEED_HIGH                  (0x3UL << I2C_CON_SPEED_Pos)    /*!< High speed mode of operation */

/** @brief CON[3]: Slave addressing mode */
#define I2C_CON_10BITADDR_SLAVE_Pos         (3U)
#define I2C_CON_10BITADDR_SLAVE_Msk         (0x1UL << I2C_CON_10BITADDR_SLAVE_Pos)
#define I2C_CON_10BITADDR_SLAVE             I2C_CON_10BITADDR_SLAVE_Msk
/** @brief CON[4]: Master addressing mode */
#define I2C_CON_10BITADDR_MASTER_Pos        (4U)
#define I2C_CON_10BITADDR_MASTER_Msk        (0x1UL << I2C_CON_10BITADDR_MASTER_Pos)
#define I2C_CON_10BITADDR_MASTER            I2C_CON_10BITADDR_MASTER_Msk
/** @brief CON[5]: Master restart enable */
#define I2C_CON_RESTART_EN_Pos              (5U)
#define I2C_CON_RESTART_EN_Msk              (0x1UL << I2C_CON_RESTART_EN_Pos)
#define I2C_CON_RESTART_EN                  I2C_CON_RESTART_EN_Msk
/** @brief CON[6]: Slave mode disable */
#define I2C_CON_SLAVE_DISABLE_Pos           (6U)
#define I2C_CON_SLAVE_DISABLE_Msk           (0x1UL << I2C_CON_SLAVE_DISABLE_Pos)
#define I2C_CON_SLAVE_DISABLE               I2C_CON_SLAVE_DISABLE_Msk
/** @brief CON[7]: STOP_DET interrupt mode if addressed */
#define I2C_CON_STOP_DET_IFADDR_Pos         (7U)
#define I2C_CON_STOP_DET_IFADDR_Msk         (0x1UL << I2C_CON_STOP_DET_IFADDR_Pos)
#define I2C_CON_STOP_DET_IFADDR             I2C_CON_STOP_DET_IFADDR_Msk
/** @brief CON[8]: TX_EMPTY interrupt control */
#define I2C_CON_TX_EMPTY_CTRL_Pos           (8U)
#define I2C_CON_TX_EMPTY_CTRL_Msk           (0x1UL << I2C_CON_TX_EMPTY_CTRL_Pos)
#define I2C_CON_TX_EMPTY_CTRL               I2C_CON_TX_EMPTY_CTRL_Msk
/** @brief CON[9]: Hold bus when RX_FIFO is full */
#define I2C_CON_RX_FIFO_FULL_HLD_CTRL_Pos   (9U)
#define I2C_CON_RX_FIFO_FULL_HLD_CTRL_Msk   (0x1UL << I2C_CON_RX_FIFO_FULL_HLD_CTRL_Pos)
#define I2C_CON_RX_FIFO_FULL_HLD_CTRL       I2C_CON_RX_FIFO_FULL_HLD_CTRL_Msk
/** @brief CON[10]: STOP_DET interrupt mode if master active */
#define I2C_CON_STOP_DET_IFMACT_Pos         (10U)
#define I2C_CON_STOP_DET_IFMACT_Msk         (0x1UL << I2C_CON_STOP_DET_IFMACT_Pos)
#define I2C_CON_STOP_DET_IFMACT             I2C_CON_STOP_DET_IFMACT_Msk
/** @brief CON[11]: Bus clear feature control */
#define I2C_CON_BUS_CLR_FEAT_CTRL_Pos       (11U)
#define I2C_CON_BUS_CLR_FEAT_CTRL_Msk       (0x1UL << I2C_CON_BUS_CLR_FEAT_CTRL_Pos)
#define I2C_CON_BUS_CLR_FEAT_CTRL           I2C_CON_BUS_CLR_FEAT_CTRL_Msk

/********************  Bit definition for I2C_TAR register  *******************/

/** @brief TAR[9:0]: Target address */
#define I2C_TAR_TARGET_ADDR_Pos             (0U)
#define I2C_TAR_TARGET_ADDR_Msk             (0x3FFUL << I2C_TAR_TARGET_ADDR_Pos)
#define I2C_TAR_TARGET_ADDR                 I2C_TAR_TARGET_ADDR_Msk
/** @brief TAR[10]: Select GENERAL_CALL or START byte transmission */
#define I2C_TAR_GC_OR_START_Pos             (10U)
#define I2C_TAR_GC_OR_START_Msk             (0x1UL << I2C_TAR_GC_OR_START_Pos)
#define I2C_TAR_GC_OR_START                 I2C_TAR_GC_OR_START_Msk
/** @brief TAR[11]: Select Device_ID or GC_OR_START to perform */
#define I2C_TAR_SPECIAL_Pos                 (11U)
#define I2C_TAR_SPECIAL_Msk                 (0x1UL << I2C_TAR_SPECIAL_Pos)
#define I2C_TAR_SPECIAL                     I2C_TAR_SPECIAL_Msk
/** @brief TAR[12]: Master addressing mode */
#define I2C_TAR_10BITADDR_MASTER_Pos        (12U)
#define I2C_TAR_10BITADDR_MASTER_Msk        (0x1UL << I2C_TAR_10BITADDR_MASTER_Pos)
#define I2C_TAR_10BITADDR_MASTER            I2C_TAR_10BITADDR_MASTER_Msk

/********************  Bit definition for I2C_SAR register  *******************/

/** @brief SAR[9:0]: Slave address */
#define I2C_TAR_SLAVE_ADDR_Pos              (0U)
#define I2C_TAR_SLAVE_ADDR_Msk              (0x3FFUL << I2C_TAR_SLAVE_ADDR_Pos)
#define I2C_TAR_SLAVE_ADDR                  I2C_TAR_SLAVE_ADDR_Msk

/*****************  Bit definition for I2C_HS_MADDR register  *****************/

/** @brief HS_MADDR[2:0]: HS mode master code */
#define I2C_HS_MADDR_MASTER_CODE_Pos        (0U)
#define I2C_HS_MADDR_MASTER_CODE_Msk        (0x7UL << I2C_HS_MADDR_MASTER_CODE_Pos)
#define I2C_HS_MADDR_MASTER_CODE            I2C_HS_MADDR_MASTER_CODE_Msk

/*****************  Bit definition for I2C_DATA_CMD register  *****************/

/** @brief DATA_CMD[7:0]: The data to be transmitted or received */
#define I2C_DATA_CMD_DAT_Pos                (0U)
#define I2C_DATA_CMD_DAT_Msk                (0xFFUL << I2C_DATA_CMD_DAT_Pos)
#define I2C_DATA_CMD_DAT                    I2C_DATA_CMD_DAT_Msk

/** @brief DATA_CMD[8]: Read or write select */
#define I2C_DATA_CMD_DIR_Pos                (8U)
#define I2C_DATA_CMD_DIR_Msk                (0x1UL << I2C_DATA_CMD_DIR_Pos)
#define I2C_DATA_CMD_DIR                    I2C_DATA_CMD_DIR_Msk

#define I2C_DATA_CMD_DIR_WRITE              (0x0UL << I2C_DATA_CMD_DIR_Pos)     /*!< Write command */
#define I2C_DATA_CMD_DIR_READ               (0x1UL << I2C_DATA_CMD_DIR_Pos)     /*!< Read command */

/** @brief DATA_CMD[11]: Indicates the first data byte received after the address phase for receive transfer */
#define I2C_DATA_CMD_FIRST_DATA_BYTE_Pos    (11U)
#define I2C_DATA_CMD_FIRST_DATA_BYTE_Msk    (0x1UL << I2C_DATA_CMD_FIRST_DATA_BYTE_Pos)
#define I2C_DATA_CMD_FIRST_DATA_BYTE        I2C_DATA_CMD_FIRST_DATA_BYTE_Msk

/****************  Bit definition for I2C_SS_SCL_HCNT register  ***************/

/** @brief SS_SCL_HCNT[15:0]: SCL clock high period count for standard speed */
#define I2C_SS_SCL_HCNT_Pos                 (0U)
#define I2C_SS_SCL_HCNT_Msk                 (0xFFFFUL << I2C_SS_SCL_HCNT_Pos)
#define I2C_SS_SCL_HCNT                     I2C_SS_SCL_HCNT_Msk

/****************  Bit definition for I2C_SS_SCL_LCNT register  ***************/

/** @brief SS_SCL_LCNT[15:0]: SCL clock low period count for standard speed */
#define I2C_SS_SCL_LCNT_Pos                 (0U)
#define I2C_SS_SCL_LCNT_Msk                 (0xFFFFUL << I2C_SS_SCL_LCNT_Pos)
#define I2C_SS_SCL_LCNT                     I2C_SS_SCL_LCNT_Msk

/****************  Bit definition for I2C_FS_SCL_HCNT register  ***************/

/** @brief FS_SCL_HCNT[15:0]: SCL clock high period count for fast speed */
#define I2C_FS_SCL_HCNT_Pos                 (0U)
#define I2C_FS_SCL_HCNT_Msk                 (0xFFFFUL << I2C_FS_SCL_HCNT_Pos)
#define I2C_FS_SCL_HCNT                     I2C_FS_SCL_HCNT_Msk

/****************  Bit definition for I2C_FS_SCL_LCNT register  ***************/

/** @brief FS_SCL_LCNT[15:0]: SCL clock low period count for fast speed */
#define I2C_FS_SCL_LCNT_Pos                 (0U)
#define I2C_FS_SCL_LCNT_Msk                 (0xFFFFUL << I2C_FS_SCL_LCNT_Pos)
#define I2C_FS_SCL_LCNT                     I2C_FS_SCL_LCNT_Msk

/****************  Bit definition for I2C_HS_SCL_HCNT register  ***************/

/** @brief HS_SCL_HCNT[15:0]: SCL clock high period count for high speed */
#define I2C_HS_SCL_HCNT_Pos                 (0U)
#define I2C_HS_SCL_HCNT_Msk                 (0xFFFFUL << I2C_HS_SCL_HCNT_Pos)
#define I2C_HS_SCL_HCNT                     I2C_HS_SCL_HCNT_Msk

/****************  Bit definition for I2C_HS_SCL_LCNT register  ***************/

/** @brief HS_SCL_LCNT[15:0]: SCL clock low period count for high speed */
#define I2C_HS_SCL_LCNT_Pos                 (0U)
#define I2C_HS_SCL_LCNT_Msk                 (0xFFFFUL << I2C_HS_SCL_LCNT_Pos)
#define I2C_HS_SCL_LCNT                     I2C_HS_SCL_LCNT_Msk

/*****************  Bit definition for I2C_INTR_STAT register  ****************/

/** @brief INTR_STAT[0]: RX_UNDER interrupt status */
#define I2C_INTR_STAT_RX_UNDER_Pos          (0U)
#define I2C_INTR_STAT_RX_UNDER_Msk          (0x1UL << I2C_INTR_STAT_RX_UNDER_Pos)
#define I2C_INTR_STAT_RX_UNDER              I2C_INTR_STAT_RX_UNDER_Msk
/** @brief INTR_STAT[1]: RX_OVER interrupt status */
#define I2C_INTR_STAT_RX_OVER_Pos           (1U)
#define I2C_INTR_STAT_RX_OVER_Msk           (0x1UL << I2C_INTR_STAT_RX_OVER_Pos)
#define I2C_INTR_STAT_RX_OVER               I2C_INTR_STAT_RX_OVER_Msk
/** @brief INTR_STAT[2]: RX_FULL interrupt status */
#define I2C_INTR_STAT_RX_FULL_Pos           (2U)
#define I2C_INTR_STAT_RX_FULL_Msk           (0x1UL << I2C_INTR_STAT_RX_FULL_Pos)
#define I2C_INTR_STAT_RX_FULL               I2C_INTR_STAT_RX_FULL_Msk
/** @brief INTR_STAT[3]: TX_OVER interrupt status */
#define I2C_INTR_STAT_TX_OVER_Pos           (3U)
#define I2C_INTR_STAT_TX_OVER_Msk           (0x1UL << I2C_INTR_STAT_TX_OVER_Pos)
#define I2C_INTR_STAT_TX_OVER               I2C_INTR_STAT_TX_OVER_Msk
/** @brief INTR_STAT[4]: TX_EMPTY interrupt status */
#define I2C_INTR_STAT_TX_EMPTY_Pos          (4U)
#define I2C_INTR_STAT_TX_EMPTY_Msk          (0x1UL << I2C_INTR_STAT_TX_EMPTY_Pos)
#define I2C_INTR_STAT_TX_EMPTY              I2C_INTR_STAT_TX_EMPTY_Msk
/** @brief INTR_STAT[5]: RD_REQ interrupt status */
#define I2C_INTR_STAT_RD_REQ_Pos            (5U)
#define I2C_INTR_STAT_RD_REQ_Msk            (0x1UL << I2C_INTR_STAT_RD_REQ_Pos)
#define I2C_INTR_STAT_RD_REQ                I2C_INTR_STAT_RD_REQ_Msk
/** @brief INTR_STAT[6]: TX_ABRT interrupt status */
#define I2C_INTR_STAT_TX_ABRT_Pos           (6U)
#define I2C_INTR_STAT_TX_ABRT_Msk           (0x1UL << I2C_INTR_STAT_TX_ABRT_Pos)
#define I2C_INTR_STAT_TX_ABRT               I2C_INTR_STAT_TX_ABRT_Msk
/** @brief INTR_STAT[7]: RX_DONE interrupt status */
#define I2C_INTR_STAT_RX_DONE_Pos           (7U)
#define I2C_INTR_STAT_RX_DONE_Msk           (0x1UL << I2C_INTR_STAT_RX_DONE_Pos)
#define I2C_INTR_STAT_RX_DONE               I2C_INTR_STAT_RX_DONE_Msk
/** @brief INTR_STAT[8]: ACTIVITY interrupt status */
#define I2C_INTR_STAT_ACTIVITY_Pos          (8U)
#define I2C_INTR_STAT_ACTIVITY_Msk          (0x1UL << I2C_INTR_STAT_ACTIVITY_Pos)
#define I2C_INTR_STAT_ACTIVITY              I2C_INTR_STAT_ACTIVITY_Msk
/** @brief INTR_STAT[9]: STOP_DET interrupt status */
#define I2C_INTR_STAT_STOP_DET_Pos          (9U)
#define I2C_INTR_STAT_STOP_DET_Msk          (0x1UL << I2C_INTR_STAT_STOP_DET_Pos)
#define I2C_INTR_STAT_STOP_DET              I2C_INTR_STAT_STOP_DET_Msk
/** @brief INTR_STAT[10]: START_DET interrupt status */
#define I2C_INTR_STAT_START_DET_Pos         (10U)
#define I2C_INTR_STAT_START_DET_Msk         (0x1UL << I2C_INTR_STAT_START_DET_Pos)
#define I2C_INTR_STAT_START_DET             I2C_INTR_STAT_START_DET_Msk
/** @brief INTR_STAT[11]: GEN_CALL interrupt status */
#define I2C_INTR_STAT_GEN_CALL_Pos          (11U)
#define I2C_INTR_STAT_GEN_CALL_Msk          (0x1UL << I2C_INTR_STAT_GEN_CALL_Pos)
#define I2C_INTR_STAT_GEN_CALL              I2C_INTR_STAT_GEN_CALL_Msk
/** @brief INTR_STAT[12]: RESTART_DET interrupt status */
#define I2C_INTR_STAT_RESTART_DET_Pos       (12U)
#define I2C_INTR_STAT_RESTART_DET_Msk       (0x1UL << I2C_INTR_STAT_RESTART_DET_Pos)
#define I2C_INTR_STAT_RESTART_DET           I2C_INTR_STAT_RESTART_DET_Msk
/** @brief INTR_STAT[13]: MASTER_ON_HOLD interrupt status */
#define I2C_INTR_STAT_MST_ON_HOLD_Pos       (13U)
#define I2C_INTR_STAT_MST_ON_HOLD_Msk       (0x1UL << I2C_INTR_STAT_MST_ON_HOLD_Pos)
#define I2C_INTR_STAT_MST_ON_HOLD           I2C_INTR_STAT_MST_ON_HOLD_Msk
/** @brief INTR_STAT[14]: SCL_STUCK_AT_LOW interrupt status */
#define I2C_INTR_STAT_SCL_STUCK_LOW_Pos     (14U)
#define I2C_INTR_STAT_SCL_STUCK_LOW_Msk     (0x1UL << I2C_INTR_STAT_SCL_STUCK_LOW_Pos)
#define I2C_INTR_STAT_SCL_STUCK_LOW         I2C_INTR_STAT_SCL_STUCK_LOW_Msk

/*****************  Bit definition for I2C_INTR_MASK register  ****************/

/** @brief INTR_MASK[0]: RX_UNDER interrupt mask */
#define I2C_INTR_MASK_RX_UNDER_Pos          (0U)
#define I2C_INTR_MASK_RX_UNDER_Msk          (0x1UL << I2C_INTR_MASK_RX_UNDER_Pos)
#define I2C_INTR_MASK_RX_UNDER              I2C_INTR_MASK_RX_UNDER_Msk
/** @brief INTR_MASK[1]: RX_OVER interrupt mask */
#define I2C_INTR_MASK_RX_OVER_Pos           (1U)
#define I2C_INTR_MASK_RX_OVER_Msk           (0x1UL << I2C_INTR_MASK_RX_OVER_Pos)
#define I2C_INTR_MASK_RX_OVER               I2C_INTR_MASK_RX_OVER_Msk
/** @brief INTR_MASK[2]: RX_FULL interrupt mask */
#define I2C_INTR_MASK_RX_FULL_Pos           (2U)
#define I2C_INTR_MASK_RX_FULL_Msk           (0x1UL << I2C_INTR_MASK_RX_FULL_Pos)
#define I2C_INTR_MASK_RX_FULL               I2C_INTR_MASK_RX_FULL_Msk
/** @brief INTR_MASK[3]: TX_OVER interrupt mask */
#define I2C_INTR_MASK_TX_OVER_Pos           (3U)
#define I2C_INTR_MASK_TX_OVER_Msk           (0x1UL << I2C_INTR_MASK_TX_OVER_Pos)
#define I2C_INTR_MASK_TX_OVER               I2C_INTR_MASK_TX_OVER_Msk
/** @brief INTR_MASK[4]: TX_EMPTY interrupt mask */
#define I2C_INTR_MASK_TX_EMPTY_Pos          (4U)
#define I2C_INTR_MASK_TX_EMPTY_Msk          (0x1UL << I2C_INTR_MASK_TX_EMPTY_Pos)
#define I2C_INTR_MASK_TX_EMPTY              I2C_INTR_MASK_TX_EMPTY_Msk
/** @brief INTR_MASK[5]: RD_REQ interrupt mask */
#define I2C_INTR_MASK_RD_REQ_Pos            (5U)
#define I2C_INTR_MASK_RD_REQ_Msk            (0x1UL << I2C_INTR_MASK_RD_REQ_Pos)
#define I2C_INTR_MASK_RD_REQ                I2C_INTR_MASK_RD_REQ_Msk
/** @brief INTR_MASK[6]: TX_ABRT interrupt mask */
#define I2C_INTR_MASK_TX_ABRT_Pos           (6U)
#define I2C_INTR_MASK_TX_ABRT_Msk           (0x1UL << I2C_INTR_MASK_TX_ABRT_Pos)
#define I2C_INTR_MASK_TX_ABRT               I2C_INTR_MASK_TX_ABRT_Msk
/** @brief INTR_MASK[7]: RX_DONE interrupt mask */
#define I2C_INTR_MASK_RX_DONE_Pos           (7U)
#define I2C_INTR_MASK_RX_DONE_Msk           (0x1UL << I2C_INTR_MASK_RX_DONE_Pos)
#define I2C_INTR_MASK_RX_DONE               I2C_INTR_MASK_RX_DONE_Msk
/** @brief INTR_MASK[8]: ACTIVITY interrupt mask */
#define I2C_INTR_MASK_ACTIVITY_Pos          (8U)
#define I2C_INTR_MASK_ACTIVITY_Msk          (0x1UL << I2C_INTR_MASK_ACTIVITY_Pos)
#define I2C_INTR_MASK_ACTIVITY              I2C_INTR_MASK_ACTIVITY_Msk
/** @brief INTR_MASK[9]: STOP_DET interrupt mask */
#define I2C_INTR_MASK_STOP_DET_Pos          (9U)
#define I2C_INTR_MASK_STOP_DET_Msk          (0x1UL << I2C_INTR_MASK_STOP_DET_Pos)
#define I2C_INTR_MASK_STOP_DET              I2C_INTR_MASK_STOP_DET_Msk
/** @brief INTR_MASK[10]: START_DET interrupt mask */
#define I2C_INTR_MASK_START_DET_Pos         (10U)
#define I2C_INTR_MASK_START_DET_Msk         (0x1UL << I2C_INTR_MASK_START_DET_Pos)
#define I2C_INTR_MASK_START_DET             I2C_INTR_MASK_START_DET_Msk
/** @brief INTR_MASK[11]: GEN_CALL interrupt mask */
#define I2C_INTR_MASK_GEN_CALL_Pos          (11U)
#define I2C_INTR_MASK_GEN_CALL_Msk          (0x1UL << I2C_INTR_MASK_GEN_CALL_Pos)
#define I2C_INTR_MASK_GEN_CALL              I2C_INTR_MASK_GEN_CALL_Msk
/** @brief INTR_MASK[14]: SCL_STUCK_AT_LOW interrupt mask */
#define I2C_INTR_MASK_SCL_STUCK_LOW_Pos     (14U)
#define I2C_INTR_MASK_SCL_STUCK_LOW_Msk     (0x1UL << I2C_INTR_MASK_SCL_STUCK_LOW_Pos)
#define I2C_INTR_MASK_SCL_STUCK_LOW         I2C_INTR_MASK_SCL_STUCK_LOW_Msk

/***************  Bit definition for I2C_RAW_INTR_STAT register  **************/

/** @brief RAW_INTR_STAT[0]: RX_UNDER interrupt status */
#define I2C_RAW_INTR_STAT_RX_UNDER_Pos      (0U)
#define I2C_RAW_INTR_STAT_RX_UNDER_Msk      (0x1UL << I2C_RAW_INTR_STAT_RX_UNDER_Pos)
#define I2C_RAW_INTR_STAT_RX_UNDER          I2C_RAW_INTR_STAT_RX_UNDER_Msk
/** @brief RAW_INTR_STAT[1]: RX_OVER interrupt status */
#define I2C_RAW_INTR_STAT_RX_OVER_Pos       (1U)
#define I2C_RAW_INTR_STAT_RX_OVER_Msk       (0x1UL << I2C_RAW_INTR_STAT_RX_OVER_Pos)
#define I2C_RAW_INTR_STAT_RX_OVER           I2C_RAW_INTR_STAT_RX_OVER_Msk
/** @brief RAW_INTR_STAT[2]: RX_FULL interrupt status */
#define I2C_RAW_INTR_STAT_RX_FULL_Pos       (2U)
#define I2C_RAW_INTR_STAT_RX_FULL_Msk       (0x1UL << I2C_RAW_INTR_STAT_RX_FULL_Pos)
#define I2C_RAW_INTR_STAT_RX_FULL           I2C_RAW_INTR_STAT_RX_FULL_Msk
/** @brief RAW_INTR_STAT[3]: TX_OVER interrupt status */
#define I2C_RAW_INTR_STAT_TX_OVER_Pos       (3U)
#define I2C_RAW_INTR_STAT_TX_OVER_Msk       (0x1UL << I2C_RAW_INTR_STAT_TX_OVER_Pos)
#define I2C_RAW_INTR_STAT_TX_OVER           I2C_RAW_INTR_STAT_TX_OVER_Msk
/** @brief RAW_INTR_STAT[4]: TX_EMPTY interrupt status */
#define I2C_RAW_INTR_STAT_TX_EMPTY_Pos      (4U)
#define I2C_RAW_INTR_STAT_TX_EMPTY_Msk      (0x1UL << I2C_RAW_INTR_STAT_TX_EMPTY_Pos)
#define I2C_RAW_INTR_STAT_TX_EMPTY          I2C_RAW_INTR_STAT_TX_EMPTY_Msk
/** @brief RAW_INTR_STAT[5]: RD_REQ interrupt status */
#define I2C_RAW_INTR_STAT_RD_REQ_Pos        (5U)
#define I2C_RAW_INTR_STAT_RD_REQ_Msk        (0x1UL << I2C_RAW_INTR_STAT_RD_REQ_Pos)
#define I2C_RAW_INTR_STAT_RD_REQ            I2C_RAW_INTR_STAT_RD_REQ_Msk
/** @brief RAW_INTR_STAT[6]: TX_ABRT interrupt status */
#define I2C_RAW_INTR_STAT_TX_ABRT_Pos       (6U)
#define I2C_RAW_INTR_STAT_TX_ABRT_Msk       (0x1UL << I2C_RAW_INTR_STAT_TX_ABRT_Pos)
#define I2C_RAW_INTR_STAT_TX_ABRT           I2C_RAW_INTR_STAT_TX_ABRT_Msk
/** @brief RAW_INTR_STAT[7]: RX_DONE interrupt status */
#define I2C_RAW_INTR_STAT_RX_DONE_Pos       (7U)
#define I2C_RAW_INTR_STAT_RX_DONE_Msk       (0x1UL << I2C_RAW_INTR_STAT_RX_DONE_Pos)
#define I2C_RAW_INTR_STAT_RX_DONE           I2C_RAW_INTR_STAT_RX_DONE_Msk
/** @brief RAW_INTR_STAT[8]: ACTIVITY interrupt status */
#define I2C_RAW_INTR_STAT_ACTIVITY_Pos      (8U)
#define I2C_RAW_INTR_STAT_ACTIVITY_Msk      (0x1UL << I2C_RAW_INTR_STAT_ACTIVITY_Pos)
#define I2C_RAW_INTR_STAT_ACTIVITY          I2C_RAW_INTR_STAT_ACTIVITY_Msk
/** @brief RAW_INTR_STAT[9]: STOP_DET interrupt status */
#define I2C_RAW_INTR_STAT_STOP_DET_Pos      (9U)
#define I2C_RAW_INTR_STAT_STOP_DET_Msk      (0x1UL << I2C_RAW_INTR_STAT_STOP_DET_Pos)
#define I2C_RAW_INTR_STAT_STOP_DET          I2C_RAW_INTR_STAT_STOP_DET_Msk
/** @brief RAW_INTR_STAT[10]: START_DET interrupt status */
#define I2C_RAW_INTR_STAT_START_DET_Pos     (10U)
#define I2C_RAW_INTR_STAT_START_DET_Msk     (0x1UL << I2C_RAW_INTR_STAT_START_DET_Pos)
#define I2C_RAW_INTR_STAT_START_DET         I2C_RAW_INTR_STAT_START_DET_Msk
/** @brief RAW_INTR_STAT[11]: GEN_CALL interrupt status */
#define I2C_RAW_INTR_STAT_GEN_CALL_Pos      (11U)
#define I2C_RAW_INTR_STAT_GEN_CALL_Msk      (0x1UL << I2C_RAW_INTR_STAT_GEN_CALL_Pos)
#define I2C_RAW_INTR_STAT_GEN_CALL          I2C_RAW_INTR_STAT_GEN_CALL_Msk
/** @brief RAW_INTR_STAT[14]: SCL_STUCK_AT_LOW interrupt status */
#define I2C_RAW_INTR_STAT_SCL_STUCK_LOW_Pos (14U)
#define I2C_RAW_INTR_STAT_SCL_STUCK_LOW_Msk (0x1UL << I2C_RAW_INTR_STAT_SCL_STUCK_LOW_Pos)
#define I2C_RAW_INTR_STAT_SCL_STUCK_LOW     I2C_RAW_INTR_STAT_SCL_STUCK_LOW_Msk

/*******************  Bit definition for I2C_RX_TL register  ******************/

/** @brief RX_TL[7:0]: Rx FIFO threshold level */
#define I2C_RX_TL_Pos                       (0U)
#define I2C_RX_TL_Msk                       (0xFFUL << I2C_RX_TL_Pos)
#define I2C_RX_TL                           I2C_RX_TL_Msk

/*******************  Bit definition for I2C_TX_TL register  ******************/

/** @brief TX_TL[7:0]: Tx FIFO threshold level */
#define I2C_TX_TL_Pos                       (0U)
#define I2C_TX_TL_Msk                       (0xFFUL << I2C_TX_TL_Pos)
#define I2C_TX_TL                           I2C_TX_TL_Msk

/******************  Bit definition for I2C_CLR_INTR register  ****************/

/** @brief CLR_INTR[0]: Clear the combined interrupt, all individual interrupts and TX_ABRT_SOURCE register */
#define I2C_CLR_INTR_Pos                    (0U)
#define I2C_CLR_INTR_Msk                    (0x1UL << I2C_CLR_INTR_Pos)
#define I2C_CLR_INTR                        I2C_CLR_INTR_Msk

/****************  Bit definition for I2C_CLR_RX_UNDER register  **************/

/** @brief CLR_RX_UNDER[0]: Clear RX_UNDER interrupt */
#define I2C_CLR_RX_UNDER_Pos                (0U)
#define I2C_CLR_RX_UNDER_Msk                (0x1UL << I2C_CLR_RX_UNDER_Pos)
#define I2C_CLR_RX_UNDER                    I2C_CLR_RX_UNDER_Msk

/****************  Bit definition for I2C_CLR_RX_OVER register  ***************/

/** @brief CLR_RX_OVER[0]: Clear RX_OVER interrupt */
#define I2C_CLR_RX_OVER_Pos                 (0U)
#define I2C_CLR_RX_OVER_Msk                 (0x1UL << I2C_CLR_RX_OVER_Pos)
#define I2C_CLR_RX_OVER                     I2C_CLR_RX_OVER_Msk

/****************  Bit definition for I2C_CLR_TX_OVER register  ***************/

/** @brief CLR_TX_OVER[0]: Clear TX_OVER interrupt */
#define I2C_CLR_TX_OVER_Pos                 (0U)
#define I2C_CLR_TX_OVER_Msk                 (0x1UL << I2C_CLR_TX_OVER_Pos)
#define I2C_CLR_TX_OVER                     I2C_CLR_TX_OVER_Msk

/****************  Bit definition for I2C_CLR_RD_REQ register  ****************/

/** @brief CLR_RD_REQ[0]: Clear RD_REQ interrupt */
#define I2C_CLR_RD_REQ_Pos                  (0U)
#define I2C_CLR_RD_REQ_Msk                  (0x1UL << I2C_CLR_RD_REQ_Pos)
#define I2C_CLR_RD_REQ                      I2C_CLR_RD_REQ_Msk

/****************  Bit definition for I2C_CLR_TX_ABRT register  ***************/

/** @brief CLR_TX_ABRT[0]: Clear TX_ABRT interrupt and TX_ABRT_SOURCE register */
#define I2C_CLR_TX_ABRT_Pos                 (0U)
#define I2C_CLR_TX_ABRT_Msk                 (0x1UL << I2C_CLR_TX_ABRT_Pos)
#define I2C_CLR_TX_ABRT                     I2C_CLR_TX_ABRT_Msk

/****************  Bit definition for I2C_CLR_RX_DONE register  ***************/

/** @brief CLR_RX_DONE[0]: Clear RX_DONE interrupt */
#define I2C_CLR_RX_DONE_Pos                 (0U)
#define I2C_CLR_RX_DONE_Msk                 (0x1UL << I2C_CLR_RX_DONE_Pos)
#define I2C_CLR_RX_DONE                     I2C_CLR_RX_DONE_Msk

/***************  Bit definition for I2C_CLR_ACTIVITY register  ***************/

/** @brief CLR_ACTIVITY[0]: Clear ACTIVITY interrupt */
#define I2C_CLR_ACTIVITY_Pos                (0U)
#define I2C_CLR_ACTIVITY_Msk                (0x1UL << I2C_CLR_ACTIVITY_Pos)
#define I2C_CLR_ACTIVITY                    I2C_CLR_ACTIVITY_Msk

/***************  Bit definition for I2C_CLR_STOP_DET register  ***************/

/** @brief CLR_STOP_DET[0]: Clear STOP_DET interrupt */
#define I2C_CLR_STOP_DET_Pos                (0U)
#define I2C_CLR_STOP_DET_Msk                (0x1UL << I2C_CLR_STOP_DET_Pos)
#define I2C_CLR_STOP_DET                    I2C_CLR_STOP_DET_Msk

/***************  Bit definition for I2C_CLR_START_DET register  **************/

/** @brief CLR_START_DET[0]: Clear START_DET interrupt */
#define I2C_CLR_START_DET_Pos               (0U)
#define I2C_CLR_START_DET_Msk               (0x1UL << I2C_CLR_START_DET_Pos)
#define I2C_CLR_START_DET                   I2C_CLR_START_DET_Msk

/***************  Bit definition for I2C_CLR_GEN_CALL register  ***************/

/** @brief CLR_GEN_CALL[0]: Clear GEN_CALL interrupt */
#define I2C_CLR_GEN_CALL_Pos                (0U)
#define I2C_CLR_GEN_CALL_Msk                (0x1UL << I2C_CLR_GEN_CALL_Pos)
#define I2C_CLR_GEN_CALL                    I2C_CLR_GEN_CALL_Msk

/******************  Bit definition for I2C_ENABLE register  ******************/

/** @brief ENABLE[0]: Enable I2C */
#define I2C_ENABLE_EN_Pos                   (0U)
#define I2C_ENABLE_EN_Msk                   (0x1UL << I2C_ENABLE_EN_Pos)
#define I2C_ENABLE_EN                       I2C_ENABLE_EN_Msk
/** @brief ENABLE[1]: Initiate the transfer abort */
#define I2C_ENABLE_ABORT_Pos                (1U)
#define I2C_ENABLE_ABORT_Msk                (0x1UL << I2C_ENABLE_ABORT_Pos)
#define I2C_ENABLE_ABORT                    I2C_ENABLE_ABORT_Msk
/** @brief ENABLE[2]: Block the transmission of data */
#define I2C_ENABLE_TX_CMD_BLOCK_Pos         (2U)
#define I2C_ENABLE_TX_CMD_BLOCK_Msk         (0x1UL << I2C_ENABLE_TX_CMD_BLOCK_Pos)
#define I2C_ENABLE_TX_CMD_BLOCK             I2C_ENABLE_TX_CMD_BLOCK_Msk
/** @brief ENABLE[3]: Initiate SDA recovery mechanism */
#define I2C_ENABLE_SDA_STUCK_REC_EN_Pos     (3U)
#define I2C_ENABLE_SDA_STUCK_REC_EN_Msk     (0x1UL << I2C_ENABLE_SDA_STUCK_REC_EN_Pos)
#define I2C_ENABLE_SDA_STUCK_REC_EN         I2C_ENABLE_SDA_STUCK_REC_EN_Msk

/******************  Bit definition for I2C_STATUS register  ******************/

/** @brief STATUS[0]: I2C activity status */
#define I2C_STATUS_ACTIVITY_Pos             (0U)
#define I2C_STATUS_ACTIVITY_Msk             (0x1UL << I2C_STATUS_ACTIVITY_Pos)
#define I2C_STATUS_ACTIVITY                 I2C_STATUS_ACTIVITY_Msk
/** @brief STATUS[1]: Tx FIFO not full */
#define I2C_STATUS_TFNF_Pos                 (1U)
#define I2C_STATUS_TFNF_Msk                 (0x1UL << I2C_STATUS_TFNF_Pos)
#define I2C_STATUS_TFNF                     I2C_STATUS_TFNF_Msk
/** @brief STATUS[2]: Tx FIFO completely empty */
#define I2C_STATUS_TFE_Pos                  (2U)
#define I2C_STATUS_TFE_Msk                  (0x1UL << I2C_STATUS_TFE_Pos)
#define I2C_STATUS_TFE                      I2C_STATUS_TFE_Msk
/** @brief STATUS[3]: Rx FIFO not empty */
#define I2C_STATUS_RFNE_Pos                 (3U)
#define I2C_STATUS_RFNE_Msk                 (0x1UL << I2C_STATUS_RFNE_Pos)
#define I2C_STATUS_RFNE                     I2C_STATUS_RFNE_Msk
/** @brief STATUS[4]: Rx FIFO completely full */
#define I2C_STATUS_RFF_Pos                  (4U)
#define I2C_STATUS_RFF_Msk                  (0x1UL << I2C_STATUS_RFF_Pos)
#define I2C_STATUS_RFF                      I2C_STATUS_RFF_Msk
/** @brief STATUS[5]: Master FSM activity status */
#define I2C_STATUS_MST_ACTIVITY_Pos         (5U)
#define I2C_STATUS_MST_ACTIVITY_Msk         (0x1UL << I2C_STATUS_MST_ACTIVITY_Pos)
#define I2C_STATUS_MST_ACTIVITY             I2C_STATUS_MST_ACTIVITY_Msk
/** @brief STATUS[6]: Slave FSM activity status */
#define I2C_STATUS_SLV_ACTIVITY_Pos         (6U)
#define I2C_STATUS_SLV_ACTIVITY_Msk         (0x1UL << I2C_STATUS_SLV_ACTIVITY_Pos)
#define I2C_STATUS_SLV_ACTIVITY             I2C_STATUS_SLV_ACTIVITY_Msk
/** @brief STATUS[7]: Stall the write transfer when Tx FIFO is empty, no stop bit after the last byte */
#define I2C_STATUS_MST_HOLD_TX_FIFOE_Pos    (7U)
#define I2C_STATUS_MST_HOLD_TX_FIFOE_Msk    (0x1UL << I2C_STATUS_MST_HOLD_TX_FIFOE_Pos)
#define I2C_STATUS_MST_HOLD_TX_FIFOE        I2C_STATUS_MST_HOLD_TX_FIFOE_Msk
/** @brief STATUS[8]: BUS Hold due to Rx FIFO is full and an additional byte received */
#define I2C_STATUS_MST_HOLD_RX_FIFOF_Pos    (8U)
#define I2C_STATUS_MST_HOLD_RX_FIFOF_Msk    (0x1UL << I2C_STATUS_MST_HOLD_RX_FIFOF_Pos)
#define I2C_STATUS_MST_HOLD_RX_FIFOF        I2C_STATUS_MST_HOLD_RX_FIFOF_Msk
/** @brief STATUS[9]: BUS Hold for the read request when Tx FIFO is empty */
#define I2C_STATUS_SLV_HOLD_TX_FIFOE_Pos    (9U)
#define I2C_STATUS_SLV_HOLD_TX_FIFOE_Msk    (0x1UL << I2C_STATUS_SLV_HOLD_TX_FIFOE_Pos)
#define I2C_STATUS_SLV_HOLD_TX_FIFOE        I2C_STATUS_SLV_HOLD_TX_FIFOE_Msk
/** @brief STATUS[10]: BUS Hold due to Rx FIFO is full and an additional byte received */
#define I2C_STATUS_SLV_HOLD_RX_FIFOF_Pos    (10U)
#define I2C_STATUS_SLV_HOLD_RX_FIFOF_Msk    (0x1UL << I2C_STATUS_SLV_HOLD_RX_FIFOF_Pos)
#define I2C_STATUS_SLV_HOLD_RX_FIFOF        I2C_STATUS_SLV_HOLD_RX_FIFOF_Msk
/** @brief STATUS[11]: SDA stuck at low is not recovered after the recovery mechanism */
#define I2C_STATUS_SDA_STUCK_NOT_REC_Pos    (11U)
#define I2C_STATUS_SDA_STUCK_NOT_REC_Msk    (0x1UL << I2C_STATUS_SDA_STUCK_NOT_REC_Pos)
#define I2C_STATUS_SDA_STUCK_NOT_REC        I2C_STATUS_SDA_STUCK_NOT_REC_Msk

/*******************  Bit definition for I2C_TXFLR register  ******************/

/** @brief TXFLR[3:0]: Tx FIFO level */
#define I2C_TXFLR_Pos                       (0U)
#define I2C_TXFLR_Msk                       (0xFUL << I2C_TXFLR_Pos)
#define I2C_TXFLR                           I2C_TXFLR_Msk

/*******************  Bit definition for I2C_RXFLR register  ******************/

/** @brief RXFLR[3:0]: Rx FIFO level */
#define I2C_RXFLR_Pos                       (0U)
#define I2C_RXFLR_Msk                       (0xFUL << I2C_RXFLR_Pos)
#define I2C_RXFLR                           I2C_RXFLR_Msk

/******************  Bit definition for I2C_SDA_HOLD register  ****************/

/** @brief SDA_HOLD[15:0]: SDA hold time when transmit */
#define I2C_SDA_HOLD_TX_HOLD_Pos            (0U)
#define I2C_SDA_HOLD_TX_HOLD_Msk            (0xFFFFUL << I2C_SDA_HOLD_TX_HOLD_Pos)
#define I2C_SDA_HOLD_TX_HOLD                I2C_SDA_HOLD_TX_HOLD_Msk

/** @brief SDA_HOLD[23:16]: SDA hold time when receive */
#define I2C_SDA_HOLD_RX_HOLD_Pos            (16U)
#define I2C_SDA_HOLD_RX_HOLD_Msk            (0xFFUL << I2C_SDA_HOLD_RX_HOLD_Pos)
#define I2C_SDA_HOLD_RX_HOLD                I2C_SDA_HOLD_RX_HOLD_Msk

/**************  Bit definition for I2C_TX_ABRT_SOURCE register  **************/

/** @brief TX_ABRT_SOURCE[0]: The address sent was not acknowledged in 7-bit addressing mode */
#define I2C_TX_ABRT_SOURCE_7ADDR_NACK_Pos   (0U)
#define I2C_TX_ABRT_SOURCE_7ADDR_NACK_Msk   (0x1UL << I2C_TX_ABRT_SOURCE_7ADDR_NACK_Pos)
#define I2C_TX_ABRT_SOURCE_7ADDR_NACK       I2C_TX_ABRT_SOURCE_7ADDR_NACK_Msk
/** @brief TX_ABRT_SOURCE[1]: The first address byte was not acknowledged in 10-bit addressing mode */
#define I2C_TX_ABRT_SOURCE_10ADDR1_NACK_Pos (1U)
#define I2C_TX_ABRT_SOURCE_10ADDR1_NACK_Msk (0x1UL << I2C_TX_ABRT_SOURCE_10ADDR1_NACK_Pos)
#define I2C_TX_ABRT_SOURCE_10ADDR1_NACK     I2C_TX_ABRT_SOURCE_10ADDR1_NACK_Msk
/** @brief TX_ABRT_SOURCE[2]: The second address byte was not acknowledged in 10-bit addressing mode */
#define I2C_TX_ABRT_SOURCE_10ADDR2_NACK_Pos (2U)
#define I2C_TX_ABRT_SOURCE_10ADDR2_NACK_Msk (0x1UL << I2C_TX_ABRT_SOURCE_10ADDR2_NACK_Pos)
#define I2C_TX_ABRT_SOURCE_10ADDR2_NACK     I2C_TX_ABRT_SOURCE_10ADDR2_NACK_Msk
/** @brief TX_ABRT_SOURCE[3]: No acknowledgement of the data following the address */
#define I2C_TX_ABRT_SOURCE_TXDATA_NACK_Pos  (3U)
#define I2C_TX_ABRT_SOURCE_TXDATA_NACK_Msk  (0x1UL << I2C_TX_ABRT_SOURCE_TXDATA_NACK_Pos)
#define I2C_TX_ABRT_SOURCE_TXDATA_NACK      I2C_TX_ABRT_SOURCE_TXDATA_NACK_Msk
/** @brief TX_ABRT_SOURCE[4]: No slave acknowledged the General Call */
#define I2C_TX_ABRT_SOURCE_GCALL_NACK_Pos   (4U)
#define I2C_TX_ABRT_SOURCE_GCALL_NACK_Msk   (0x1UL << I2C_TX_ABRT_SOURCE_GCALL_NACK_Pos)
#define I2C_TX_ABRT_SOURCE_GCALL_NACK       I2C_TX_ABRT_SOURCE_GCALL_NACK_Msk
/** @brief TX_ABRT_SOURCE[5]: General Call is followed by the read from the bus */
#define I2C_TX_ABRT_SOURCE_GCALL_READ_Pos   (5U)
#define I2C_TX_ABRT_SOURCE_GCALL_READ_Msk   (0x1UL << I2C_TX_ABRT_SOURCE_GCALL_READ_Pos)
#define I2C_TX_ABRT_SOURCE_GCALL_READ       I2C_TX_ABRT_SOURCE_GCALL_READ_Msk
/** @brief TX_ABRT_SOURCE[6]: High speed master code was acknowledged */
#define I2C_TX_ABRT_SOURCE_HS_ACKDET_Pos    (6U)
#define I2C_TX_ABRT_SOURCE_HS_ACKDET_Msk    (0x1UL << I2C_TX_ABRT_SOURCE_HS_ACKDET_Pos)
#define I2C_TX_ABRT_SOURCE_HS_ACKDET        I2C_TX_ABRT_SOURCE_HS_ACKDET_Msk
/** @brief TX_ABRT_SOURCE[7]: START byte was acknowledged */
#define I2C_TX_ABRT_SOURCE_SBYTE_ACKDET_Pos (7U)
#define I2C_TX_ABRT_SOURCE_SBYTE_ACKDET_Msk (0x1UL << I2C_TX_ABRT_SOURCE_SBYTE_ACKDET_Pos)
#define I2C_TX_ABRT_SOURCE_SBYTE_ACKDET     I2C_TX_ABRT_SOURCE_SBYTE_ACKDET_Msk
/** @brief TX_ABRT_SOURCE[8]: Trying to switch master to HS mode when RESTART disabled */
#define I2C_TX_ABRT_SOURCE_HS_NR_Pos        (8U)
#define I2C_TX_ABRT_SOURCE_HS_NR_Msk        (0x1UL << I2C_TX_ABRT_SOURCE_HS_NR_Pos)
#define I2C_TX_ABRT_SOURCE_HS_NR            I2C_TX_ABRT_SOURCE_HS_NR_Msk
/** @brief TX_ABRT_SOURCE[9]: Trying to send START when RESTART disabled */
#define I2C_TX_ABRT_SOURCE_SBYTE_NR_Pos     (9U)
#define I2C_TX_ABRT_SOURCE_SBYTE_NR_Msk     (0x1UL << I2C_TX_ABRT_SOURCE_SBYTE_NR_Pos)
#define I2C_TX_ABRT_SOURCE_SBYTE_NR         I2C_TX_ABRT_SOURCE_SBYTE_NR_Msk
/** @brief TX_ABRT_SOURCE[10]: Trying to read in 10-bit addressing mode when RESTART disabled */
#define I2C_TX_ABRT_SOURCE_10B_RD_NR_Pos    (10U)
#define I2C_TX_ABRT_SOURCE_10B_RD_NR_Msk    (0x1UL << I2C_TX_ABRT_SOURCE_10B_RD_NR_Pos)
#define I2C_TX_ABRT_SOURCE_10B_RD_NR        I2C_TX_ABRT_SOURCE_10B_RD_NR_Msk
/** @brief TX_ABRT_SOURCE[11]: Initiating master operation when master mode disabled */
#define I2C_TX_ABRT_SOURCE_MASTER_DIS_Pos   (11U)
#define I2C_TX_ABRT_SOURCE_MASTER_DIS_Msk   (0x1UL << I2C_TX_ABRT_SOURCE_MASTER_DIS_Pos)
#define I2C_TX_ABRT_SOURCE_MASTER_DIS       I2C_TX_ABRT_SOURCE_MASTER_DIS_Msk
/** @brief TX_ABRT_SOURCE[12]: Master or slave transmitter lost arbitration */
#define I2C_TX_ABRT_SOURCE_ARB_LOST_Pos     (12U)
#define I2C_TX_ABRT_SOURCE_ARB_LOST_Msk     (0x1UL << I2C_TX_ABRT_SOURCE_ARB_LOST_Pos)
#define I2C_TX_ABRT_SOURCE_ARB_LOST         I2C_TX_ABRT_SOURCE_ARB_LOST_Msk
/** @brief TX_ABRT_SOURCE[13]: Slave flushes existing data in Tx FIFO upon getting read */
#define I2C_TX_ABRT_SOURCE_SLV_FL_FIFO_Pos  (13U)
#define I2C_TX_ABRT_SOURCE_SLV_FL_FIFO_Msk  (0x1UL << I2C_TX_ABRT_SOURCE_SLV_FL_FIFO_Pos)
#define I2C_TX_ABRT_SOURCE_SLV_FL_FIFO      I2C_TX_ABRT_SOURCE_SLV_FL_FIFO_Msk
/** @brief TX_ABRT_SOURCE[14]: Slave lost arbitration */
#define I2C_TX_ABRT_SOURCE_SLV_ARB_LOST_Pos (14U)
#define I2C_TX_ABRT_SOURCE_SLV_ARB_LOST_Msk (0x1UL << I2C_TX_ABRT_SOURCE_SLV_ARB_LOST_Pos)
#define I2C_TX_ABRT_SOURCE_SLV_ARB_LOST     I2C_TX_ABRT_SOURCE_SLV_ARB_LOST_Msk
/** @brief TX_ABRT_SOURCE[15]: Slave trying to transmit in read mode */
#define I2C_TX_ABRT_SOURCE_SLV_RD_INTX_Pos  (15U)
#define I2C_TX_ABRT_SOURCE_SLV_RD_INTX_Msk  (0x1UL << I2C_TX_ABRT_SOURCE_SLV_RD_INTX_Pos)
#define I2C_TX_ABRT_SOURCE_SLV_RD_INTX      I2C_TX_ABRT_SOURCE_SLV_RD_INTX_Msk
/** @brief TX_ABRT_SOURCE[16]: Master has detected the transfer abort */
#define I2C_TX_ABRT_SOURCE_USER_Pos         (16U)
#define I2C_TX_ABRT_SOURCE_USER_Msk         (0x1UL << I2C_TX_ABRT_SOURCE_USER_Pos)
#define I2C_TX_ABRT_SOURCE_USER             I2C_TX_ABRT_SOURCE_USER_Msk
/** @brief TX_ABRT_SOURCE[17]: SDA stuck at low */
#define I2C_TX_ABRT_SOURCE_STUCK_LOW_Pos    (17U)
#define I2C_TX_ABRT_SOURCE_STUCK_LOW_Msk    (0x1UL << I2C_TX_ABRT_SOURCE_STUCK_LOW_Pos)
#define I2C_TX_ABRT_SOURCE_STUCK_LOW        I2C_TX_ABRT_SOURCE_STUCK_LOW_Msk
/** @brief TX_ABRT_SOURCE[31:23]: The number of Tx FIFO data commands flushed due to TX_ABRT interrupt */
#define I2C_TX_ABRT_SOURCE_FLUSH_CNT_Pos    (23U)
#define I2C_TX_ABRT_SOURCE_FLUSH_CNT_Msk    (0x1FFUL << I2C_TX_ABRT_SOURCE_FLUSH_CNT_Pos)
#define I2C_TX_ABRT_SOURCE_FLUSH_CNT        I2C_TX_ABRT_SOURCE_FLUSH_CNT_Msk

/******************  Bit definition for I2C_DMA_CR register  ******************/

/** @brief DMA_CR[0]: Receive DMA enable */
#define I2C_DMA_CR_RDMAE_Pos                (0U)
#define I2C_DMA_CR_RDMAE_Msk                (0x1UL << I2C_DMA_CR_RDMAE_Pos)
#define I2C_DMA_CR_RDMAE                    I2C_DMA_CR_RDMAE_Msk
/** @brief DMA_CR[1]: Transmit DMA enable */
#define I2C_DMA_CR_TDMAE_Pos                (1U)
#define I2C_DMA_CR_TDMAE_Msk                (0x1UL << I2C_DMA_CR_TDMAE_Pos)
#define I2C_DMA_CR_TDMAE                    I2C_DMA_CR_TDMAE_Msk

/*****************  Bit definition for I2C_DMA_TDLR register  *****************/

/** @brief DMA_TDLR[2:0]: Transmit data level */
#define I2C_DMA_TDLR_Pos                    (0U)
#define I2C_DMA_TDLR_Msk                    (0x7UL << I2C_DMA_TDLR_Pos)
#define I2C_DMA_TDLR                        I2C_DMA_TDLR_Msk

/*****************  Bit definition for I2C_DMA_RDLR register  *****************/

/** @brief DMA_RDLR[2:0]: Receive data level */
#define I2C_DMA_RDLR_Pos                    (0U)
#define I2C_DMA_RDLR_Msk                    (0x7UL << I2C_DMA_RDLR_Pos)
#define I2C_DMA_RDLR                        I2C_DMA_RDLR_Msk

/*****************  Bit definition for I2C_SDA_SETUP register  ****************/

/** @brief SDA_SETUP[7:0]: Time delay introduced in the rising edge of SCL */
#define I2C_SDA_SETUP_Pos                   (0U)
#define I2C_SDA_SETUP_Msk                   (0xFFUL << I2C_SDA_SETUP_Pos)
#define I2C_SDA_SETUP                       I2C_SDA_SETUP_Msk

/*************  Bit definition for I2C_ACK_GENERAL_CALL register  *************/

/** @brief ACK_GENERAL_CALL[0]: ACK for General Call */
#define I2C_ACK_GENERAL_CALL_Pos            (0U)
#define I2C_ACK_GENERAL_CALL_Msk            (0x1UL << I2C_ACK_GENERAL_CALL_Pos)
#define I2C_ACK_GENERAL_CALL                I2C_ACK_GENERAL_CALL_Msk

/***************  Bit definition for I2C_ENABLE_STATUS register  **************/

/** @brief ENABLE_STATUS[0]: IC enable status */
#define I2C_ENABLE_STATUS_IC_EN_Pos         (0U)
#define I2C_ENABLE_STATUS_IC_EN_Msk         (0x1UL << I2C_ENABLE_STATUS_IC_EN_Pos)
#define I2C_ENABLE_STATUS_IC_EN             I2C_ENABLE_STATUS_IC_EN_Msk
/** @brief ENABLE_STATUS[1]: Slave disabled while busy */
#define I2C_ENABLE_STATUS_SLV_DIS_BUSY_Pos  (1U)
#define I2C_ENABLE_STATUS_SLV_DIS_BUSY_Msk  (0x1UL << I2C_ENABLE_STATUS_SLV_DIS_BUSY_Pos)
#define I2C_ENABLE_STATUS_SLV_DIS_BUSY      I2C_ENABLE_STATUS_SLV_DIS_BUSY_Msk
/** @brief ENABLE_STATUS[2]: Slave received data lost */
#define I2C_ENABLE_STATUS_SLV_DATA_LOST_Pos (2U)
#define I2C_ENABLE_STATUS_SLV_DATA_LOST_Msk (0x1UL << I2C_ENABLE_STATUS_SLV_DATA_LOST_Pos)
#define I2C_ENABLE_STATUS_SLV_DATA_LOST     I2C_ENABLE_STATUS_SLV_DATA_LOST_Msk

/*****************  Bit definition for I2C_FS_SPKLEN register  ****************/

/** @brief FS_SPKLEN[7:0]: SS, FS or FM+ spike suppression limit */
#define I2C_FS_SPKLEN_Pos                   (0U)
#define I2C_FS_SPKLEN_Msk                   (0xFFUL << I2C_FS_SPKLEN_Pos)
#define I2C_FS_SPKLEN                       I2C_FS_SPKLEN_Msk

/*****************  Bit definition for I2C_HS_SPKLEN register  ****************/

/** @brief HS_SPKLEN[7:0]: HS spike suppression limit */
#define I2C_HS_SPKLEN_Pos                   (0U)
#define I2C_HS_SPKLEN_Msk                   (0xFFUL << I2C_HS_SPKLEN_Pos)
#define I2C_HS_SPKLEN                       I2C_HS_SPKLEN_Msk

/***************  Bit definition for I2C_SCL_STUCK_LT register  ***************/

/** @brief SCL_STUCK_LT[31:0]: SCL stuck at low timeout */
#define I2C_SCL_STUCK_LT_Pos                (0U)
#define I2C_SCL_STUCK_LT_Msk                (0xFFFFFFFFUL << I2C_SCL_STUCK_LT_Pos)
#define I2C_SCL_STUCK_LT                    I2C_SCL_STUCK_LT_Msk

/***************  Bit definition for I2C_SDA_STUCK_LT register  ***************/

/** @brief SDA_STUCK_LT[31:0]: SDA stuck at low timeout */
#define I2C_SDA_STUCK_LT_Pos                (0U)
#define I2C_SDA_STUCK_LT_Msk                (0xFFFFFFFFUL << I2C_SDA_STUCK_LT_Pos)
#define I2C_SDA_STUCK_LT                    I2C_SDA_STUCK_LT_Msk

/*************  Bit definition for I2C_CLR_SCL_STUCK_DET register  ************/

/** @brief CLR_SCL_STUCK_DET[0]: Clear SCL_STUCK_AT_LOW interrupt */
#define I2C_CLR_SCL_STUCK_DET_Pos           (0U)
#define I2C_CLR_SCL_STUCK_DET_Msk           (0x1UL << I2C_CLR_SCL_STUCK_DET_Pos)
#define I2C_CLR_SCL_STUCK_DET               I2C_CLR_SCL_STUCK_DET_Msk

/**************  Bit definition for I2C_REG_TIMEOUT_RST register  *************/

/** @brief REG_TIMEOUT_RST[7:0]: The reset value of REG_TIMEOUT counter register */
#define I2C_REG_TIMEOUT_RST_Pos             (0U)
#define I2C_REG_TIMEOUT_RST_Msk             (0xFFUL << I2C_REG_TIMEOUT_RST_Pos)
#define I2C_REG_TIMEOUT_RST                 I2C_REG_TIMEOUT_RST_Msk

/***************  Bit definition for I2C_COMP_PARAM_1 register  ***************/

/** @brief COMP_PARAM_1[1:0]: APB data bus width */
#define I2C_COMP_PARAM_1_APB_WIDTH_Pos      (0U)
#define I2C_COMP_PARAM_1_APB_WIDTH_Msk      (0x3UL << I2C_COMP_PARAM_1_APB_WIDTH_Pos)
#define I2C_COMP_PARAM_1_APB_WIDTH          I2C_COMP_PARAM_1_APB_WIDTH_Msk

#define I2C_COMP_PARAM_1_APB_WIDTH_8BITS    (0x0UL << I2C_COMP_PARAM_1_APB_WIDTH_Pos)   /*!< APB data bus width is 8 bits */
#define I2C_COMP_PARAM_1_APB_WIDTH_16BITS   (0x1UL << I2C_COMP_PARAM_1_APB_WIDTH_Pos)   /*!< APB data bus width is 16 bits */
#define I2C_COMP_PARAM_1_APB_WIDTH_32BITS   (0x2UL << I2C_COMP_PARAM_1_APB_WIDTH_Pos)   /*!< APB data bus width is 32 bits */

/** @brief COMP_PARAM_1[3:2]: Max speed mode */
#define I2C_COMP_PARAM_1_MAX_SPEED_Pos      (2U)
#define I2C_COMP_PARAM_1_MAX_SPEED_Msk      (0x3UL << I2C_COMP_PARAM_1_MAX_SPEED_Pos)
#define I2C_COMP_PARAM_1_MAX_SPEED          I2C_COMP_PARAM_1_MAX_SPEED_Msk

#define I2C_COMP_PARAM_1_MAX_SPEED_STD      (0x1UL << I2C_COMP_PARAM_1_MAX_SPEED_Pos)   /*!< Max speed is Standard mode */
#define I2C_COMP_PARAM_1_MAX_SPEED_FAST     (0x2UL << I2C_COMP_PARAM_1_MAX_SPEED_Pos)   /*!< Max speed is Fast mode */
#define I2C_COMP_PARAM_1_MAX_SPEED_HIGH     (0x3UL << I2C_COMP_PARAM_1_MAX_SPEED_Pos)   /*!< Max speed is High mode */

/** @brief COMP_PARAM_1[4]: Count values setting mode */
#define I2C_COMP_PARAM_1_HC_CNT_VAL_Pos     (4U)
#define I2C_COMP_PARAM_1_HC_CNT_VAL_Msk     (0x1UL << I2C_COMP_PARAM_1_HC_CNT_VAL_Pos)
#define I2C_COMP_PARAM_1_HC_CNT_VAL         I2C_COMP_PARAM_1_HC_CNT_VAL_Msk

#define I2C_COMP_PARAM_1_HC_CNT_VAL_DISABLE (0x0UL << I2C_COMP_PARAM_1_HC_CNT_VAL_Pos)  /*!< Programmable count values */
#define I2C_COMP_PARAM_1_HC_CNT_VAL_ENABLE  (0x1UL << I2C_COMP_PARAM_1_HC_CNT_VAL_Pos)  /*!< Hard code the count values */

/** @brief COMP_PARAM_1[5]: Interrupt outputs mode */
#define I2C_COMP_PARAM_1_INTR_IO_Pos        (5U)
#define I2C_COMP_PARAM_1_INTR_IO_Msk        (0x1UL << I2C_COMP_PARAM_1_INTR_IO_Pos)
#define I2C_COMP_PARAM_1_INTR_IO            I2C_COMP_PARAM_1_INTR_IO_Msk

#define I2C_COMP_PARAM_1_INTR_IO_INDIVIDUAL (0x0UL << I2C_COMP_PARAM_1_INTR_IO_Pos) /*!< Individual interrupt outputs */
#define I2C_COMP_PARAM_1_INTR_IO_COMBINED   (0x1UL << I2C_COMP_PARAM_1_INTR_IO_Pos) /*!< Combined interrupt outputs */

/** @brief COMP_PARAM_1[6]: DMA handshaking signals enable status */
#define I2C_COMP_PARAM_1_HAS_DMA_Pos        (6U)
#define I2C_COMP_PARAM_1_HAS_DMA_Msk        (0x1UL << I2C_COMP_PARAM_1_HAS_DMA_Pos)
#define I2C_COMP_PARAM_1_HAS_DMA            I2C_COMP_PARAM_1_HAS_DMA_Msk

#define I2C_COMP_PARAM_1_HAS_DMA_DISABLE    (0x0UL << I2C_COMP_PARAM_1_HAS_DMA_Pos) /*!< DMA handshaking signals are disabled */
#define I2C_COMP_PARAM_1_HAS_DMA_ENABLE     (0x1UL << I2C_COMP_PARAM_1_HAS_DMA_Pos) /*!< DMA handshaking signals are enabled */

/** @brief COMP_PARAM_1[7]: Reading encoded parameters capability enable status */
#define I2C_COMP_PARAM_1_RD_ENC_PAR_Pos     (7U)
#define I2C_COMP_PARAM_1_RD_ENC_PAR_Msk     (0x1UL << I2C_COMP_PARAM_1_RD_ENC_PAR_Pos)
#define I2C_COMP_PARAM_1_RD_ENC_PAR         I2C_COMP_PARAM_1_RD_ENC_PAR_Msk

#define I2C_COMP_PARAM_1_RD_ENC_PAR_DISABLE (0x0UL << I2C_COMP_PARAM_1_ADD_ENC_P_Pos)   /*!< Capability of reading encoded parameters is disabled */
#define I2C_COMP_PARAM_1_RD_ENC_PAR_ENABLE  (0x1UL << I2C_COMP_PARAM_1_ADD_ENC_P_Pos)   /*!< Capability of reading encoded parameters is enabled */

/** @brief COMP_PARAM_1[15:8]: Depth of receive buffer */
#define I2C_COMP_PARAM_1_RX_BUF_DEPTH_Pos   (8U)
#define I2C_COMP_PARAM_1_RX_BUF_DEPTH_Msk   (0xFFUL << I2C_COMP_PARAM_1_RX_BUF_DEPTH_Pos)
#define I2C_COMP_PARAM_1_RX_BUF_DEPTH       I2C_COMP_PARAM_1_RX_BUF_DEPTH_Msk
/** @brief COMP_PARAM_1[23:16]: Depth of transmit buffer */
#define I2C_COMP_PARAM_1_TX_BUF_DEPTH_Pos   (16U)
#define I2C_COMP_PARAM_1_TX_BUF_DEPTH_Msk   (0xFFUL << I2C_COMP_PARAM_1_TX_BUF_DEPTH_Pos)
#define I2C_COMP_PARAM_1_TX_BUF_DEPTH       I2C_COMP_PARAM_1_TX_BUF_DEPTH_Msk

/***************  Bit definition for I2C_COMP_VERSION register  ***************/

/** @brief COMP_VERSION[31:0]: I2C component version */
#define I2C_COMP_VERSION_Pos                (0U)
#define I2C_COMP_VERSION_Msk                (0xFFFFFFFFUL << I2C_COMP_VERSION_Pos)
#define I2C_COMP_VERSION                    I2C_COMP_VERSION_Msk

/*****************  Bit definition for I2C_COMP_TYPE register  ****************/

/** @brief COMP_TYPE[31:0]: I2C component type */
#define I2C_COMP_TYPE_Pos                   (0U)
#define I2C_COMP_TYPE_Msk                   (0xFFFFFFFFUL << I2C_COMP_TYPE_Pos)
#define I2C_COMP_TYPE                       I2C_COMP_TYPE_Msk

/******************************************************************************/
/*                  Inter Integrated Circuit Sound Interface                  */
/******************************************************************************/

/********************  Bit definition for I2S_IER register  *******************/

/** @brief IER[0]: I2S enable */
#define I2S_IER_IEN_Pos                     (0U)
#define I2S_IER_IEN_Msk                     (0x1UL << I2S_IER_IEN_Pos)
#define I2S_IER_IEN                         I2S_IER_IEN_Msk

/*******************  Bit definition for I2S_IRER register  *******************/

/** @brief IRER[0]: Receiver enable */
#define I2S_IRER_RXEN_Pos                   (0U)
#define I2S_IRER_RXEN_Msk                   (0x1UL << I2S_IRER_RXEN_Pos)
#define I2S_IRER_RXEN                       I2S_IRER_RXEN_Msk

/*******************  Bit definition for I2S_ITER register  *******************/

/** @brief ITER[0]: Transmitter enable */
#define I2S_ITER_TXEN_Pos                   (0U)
#define I2S_ITER_TXEN_Msk                   (0x1UL << I2S_ITER_TXEN_Pos)
#define I2S_ITER_TXEN                       I2S_ITER_TXEN_Msk

/*******************  Bit definition for I2S_RXFFR register  ******************/

/** @brief RXFFR[0]: Receiver FIFO reset */
#define I2S_RXFFR_Pos                       (0U)
#define I2S_RXFFR_Msk                       (0x1UL << I2S_RXFFR_Pos)
#define I2S_RXFFR                           I2S_RXFFR_Msk

/*******************  Bit definition for I2S_TXFFR register  ******************/

/** @brief TXFFR[0]: Transmitter FIFO reset */
#define I2S_TXFFR_Pos                       (0U)
#define I2S_TXFFR_Msk                       (0x1UL << I2S_TXFFR_Pos)
#define I2S_TXFFR                           I2S_TXFFR_Msk

/*******************  Bit definition for I2S_LRBR0 register  ******************/

/** @brief LRBR0[31:0]: The left stereo data received */
#define I2S_LRBR0_Pos                       (0U)
#define I2S_LRBR0_Msk                       (0xFFFFFFFFUL << I2S_LRBR0_Pos)
#define I2S_LRBR0                           I2S_LRBR0_Msk

/*******************  Bit definition for I2S_LTHR0 register  ******************/

/** @brief LTHR0[31:0]: The left stereo data to be transmitted */
#define I2S_LTHR0_Pos                       (0U)
#define I2S_LTHR0_Msk                       (0xFFFFFFFFUL << I2S_LTHR0_Pos)
#define I2S_LTHR0                           I2S_LTHR0_Msk

/*******************  Bit definition for I2S_RRBR0 register  ******************/

/** @brief RRBR0[31:0]: The right stereo data received */
#define I2S_RRBR0_Pos                       (0U)
#define I2S_RRBR0_Msk                       (0xFFFFFFFFUL << I2S_RRBR0_Pos)
#define I2S_RRBR0                           I2S_RRBR0_Msk

/*******************  Bit definition for I2S_RTHR0 register  ******************/

/** @brief RTHR0[31:0]: The right stereo data to be transmitted */
#define I2S_RTHR0_Pos                       (0U)
#define I2S_RTHR0_Msk                       (0xFFFFFFFFUL << I2S_RTHR0_Pos)
#define I2S_RTHR0                           I2S_RTHR0_Msk

/*******************  Bit definition for I2S_RER0 register  *******************/

/** @brief RER0[0]: Receive channel 0 enable */
#define I2S_RER0_RXCHEN0_Pos                (0U)
#define I2S_RER0_RXCHEN0_Msk                (0x1UL << I2S_RER0_RXCHEN0_Pos)
#define I2S_RER0_RXCHEN0                    I2S_RER0_RXCHEN0_Msk

/*******************  Bit definition for I2S_TER0 register  *******************/

/** @brief TER0[0]: Transmit channel 0 enable */
#define I2S_TER0_TXCHEN0_Pos                (0U)
#define I2S_TER0_TXCHEN0_Msk                (0x1UL << I2S_TER0_TXCHEN0_Pos)
#define I2S_TER0_TXCHEN0                    I2S_TER0_TXCHEN0_Msk

/*******************  Bit definition for I2S_RCR0 register  *******************/

/** @brief RCR0[2:0]: Receiver data resolution */
#define I2S_RCR0_WLEN_Pos                   (0U)
#define I2S_RCR0_WLEN_Msk                   (0x7UL << I2S_RCR0_WLEN_Pos)
#define I2S_RCR0_WLEN                       I2S_RCR0_WLEN_Msk

#define I2S_RCR0_WLEN_IGNORE                (0x0UL << I2S_RCR0_WLEN_Pos)    /*!< Ignore the word length */
#define I2S_RCR0_WLEN_12BIT                 (0x1UL << I2S_RCR0_WLEN_Pos)    /*!< 12-bit data resolution */
#define I2S_RCR0_WLEN_16BIT                 (0x2UL << I2S_RCR0_WLEN_Pos)    /*!< 16-bit data resolution */
#define I2S_RCR0_WLEN_20BIT                 (0x3UL << I2S_RCR0_WLEN_Pos)    /*!< 20-bit data resolution */
#define I2S_RCR0_WLEN_24BIT                 (0x4UL << I2S_RCR0_WLEN_Pos)    /*!< 24-bit data resolution */
#define I2S_RCR0_WLEN_32BIT                 (0x5UL << I2S_RCR0_WLEN_Pos)    /*!< 32-bit data resolution */

/*******************  Bit definition for I2S_TCR0 register  *******************/

/** @brief TCR0[2:0]: Transmitter data resolution */
#define I2S_TCR0_WLEN_Pos                   (0U)
#define I2S_TCR0_WLEN_Msk                   (0x7UL << I2S_TCR0_WLEN_Pos)
#define I2S_TCR0_WLEN                       I2S_TCR0_WLEN_Msk

#define I2S_TCR0_WLEN_IGNORE                (0x0UL << I2S_TCR0_WLEN_Pos)    /*!< Ignore the word length */
#define I2S_TCR0_WLEN_12BIT                 (0x1UL << I2S_TCR0_WLEN_Pos)    /*!< 12-bit data resolution */
#define I2S_TCR0_WLEN_16BIT                 (0x2UL << I2S_TCR0_WLEN_Pos)    /*!< 16-bit data resolution */
#define I2S_TCR0_WLEN_20BIT                 (0x3UL << I2S_TCR0_WLEN_Pos)    /*!< 20-bit data resolution */
#define I2S_TCR0_WLEN_24BIT                 (0x4UL << I2S_TCR0_WLEN_Pos)    /*!< 24-bit data resolution */
#define I2S_TCR0_WLEN_32BIT                 (0x5UL << I2S_TCR0_WLEN_Pos)    /*!< 32-bit data resolution */

/*******************  Bit definition for I2S_ISR0 register  *******************/

/** @brief ISR0[0]: Status of Receive Data Available interrupt */
#define I2S_ISR0_RXDA_Pos                   (0U)
#define I2S_ISR0_RXDA_Msk                   (0x1UL << I2S_ISR0_RXDA_Pos)
#define I2S_ISR0_RXDA                       I2S_ISR0_RXDA_Msk
/** @brief ISR0[1]: Status of Receive Data Overrun interrupt */
#define I2S_ISR0_RXFO_Pos                   (1U)
#define I2S_ISR0_RXFO_Msk                   (0x1UL << I2S_ISR0_RXFO_Pos)
#define I2S_ISR0_RXFO                       I2S_ISR0_RXFO_Msk
/** @brief ISR0[4]: Status of Transmit Empty Trigger interrupt */
#define I2S_ISR0_TXFE_Pos                   (4U)
#define I2S_ISR0_TXFE_Msk                   (0x1UL << I2S_ISR0_TXFE_Pos)
#define I2S_ISR0_TXFE                       I2S_ISR0_TXFE_Msk
/** @brief ISR0[5]: Status of Transmit Data Overrun interrupt */
#define I2S_ISR0_TXFO_Pos                   (5U)
#define I2S_ISR0_TXFO_Msk                   (0x1UL << I2S_ISR0_TXFO_Pos)
#define I2S_ISR0_TXFO                       I2S_ISR0_TXFO_Msk

/*******************  Bit definition for I2S_IMR0 register  *******************/

/** @brief IMR0[0]: Mask Receive Data Available interrupt */
#define I2S_IMR0_RXDAM_Pos                  (0U)
#define I2S_IMR0_RXDAM_Msk                  (0x1UL << I2S_IMR0_RXDAM_Pos)
#define I2S_IMR0_RXDAM                      I2S_IMR0_RXDAM_Msk
/** @brief IMR0[1]: Mask Receive Data Overrun interrupt */
#define I2S_IMR0_RXFOM_Pos                  (1U)
#define I2S_IMR0_RXFOM_Msk                  (0x1UL << I2S_IMR0_RXFOM_Pos)
#define I2S_IMR0_RXFOM                      I2S_IMR0_RXFOM_Msk
/** @brief IMR0[4]: Mask Transmit Empty Trigger interrupt */
#define I2S_IMR0_TXFEM_Pos                  (4U)
#define I2S_IMR0_TXFEM_Msk                  (0x1UL << I2S_IMR0_TXFEM_Pos)
#define I2S_IMR0_TXFEM                      I2S_IMR0_TXFEM_Msk
/** @brief IMR0[5]: Mask Transmit Data Overrun interrupt */
#define I2S_IMR0_TXFOM_Pos                  (5U)
#define I2S_IMR0_TXFOM_Msk                  (0x1UL << I2S_IMR0_TXFOM_Pos)
#define I2S_IMR0_TXFOM                      I2S_IMR0_TXFOM_Msk

/*******************  Bit definition for I2S_ROR0 register  *******************/

/** @brief ROR0[0]: Clear Rx FIFO Data Overrun interrupt */
#define I2S_ROR0_RXCHO_Pos                  (0U)
#define I2S_ROR0_RXCHO_Msk                  (0x1UL << I2S_ROR0_RXCHO_Pos)
#define I2S_ROR0_RXCHO                      I2S_ROR0_RXCHO_Msk

/*******************  Bit definition for I2S_TOR0 register  *******************/

/** @brief TOR0[0]: Clear Tx FIFO Data Overrun interrupt */
#define I2S_TOR0_TXCHO_Pos                  (0U)
#define I2S_TOR0_TXCHO_Msk                  (0x1UL << I2S_TOR0_TXCHO_Pos)
#define I2S_TOR0_TXCHO                      I2S_TOR0_TXCHO_Msk

/*******************  Bit definition for I2S_RFCR0 register  ******************/

/** @brief RFCR0[3:0]: Rx FIFO trigger level */
#define I2S_RFCR0_RXCHDT_Pos                (0U)
#define I2S_RFCR0_RXCHDT_Msk                (0xFUL << I2S_RFCR0_RXCHDT_Pos)
#define I2S_RFCR0_RXCHDT                    I2S_RFCR0_RXCHDT_Msk

#define I2S_RFCR0_RXCHDT_LVL1               (0x0UL << I2S_RFCR0_RXCHDT_Pos)     /*!< Rx FIFO trigger level 1 */
#define I2S_RFCR0_RXCHDT_LVL2               (0x1UL << I2S_RFCR0_RXCHDT_Pos)     /*!< Rx FIFO trigger level 2 */
#define I2S_RFCR0_RXCHDT_LVL3               (0x2UL << I2S_RFCR0_RXCHDT_Pos)     /*!< Rx FIFO trigger level 3 */
#define I2S_RFCR0_RXCHDT_LVL4               (0x3UL << I2S_RFCR0_RXCHDT_Pos)     /*!< Rx FIFO trigger level 4 */
#define I2S_RFCR0_RXCHDT_LVL5               (0x4UL << I2S_RFCR0_RXCHDT_Pos)     /*!< Rx FIFO trigger level 5 */
#define I2S_RFCR0_RXCHDT_LVL6               (0x5UL << I2S_RFCR0_RXCHDT_Pos)     /*!< Rx FIFO trigger level 6 */
#define I2S_RFCR0_RXCHDT_LVL7               (0x6UL << I2S_RFCR0_RXCHDT_Pos)     /*!< Rx FIFO trigger level 7 */
#define I2S_RFCR0_RXCHDT_LVL8               (0x7UL << I2S_RFCR0_RXCHDT_Pos)     /*!< Rx FIFO trigger level 8 */
#define I2S_RFCR0_RXCHDT_LVL9               (0x8UL << I2S_RFCR0_RXCHDT_Pos)     /*!< Rx FIFO trigger level 9 */
#define I2S_RFCR0_RXCHDT_LVL10              (0x9UL << I2S_RFCR0_RXCHDT_Pos)     /*!< Rx FIFO trigger level 10 */
#define I2S_RFCR0_RXCHDT_LVL11              (0xAUL << I2S_RFCR0_RXCHDT_Pos)     /*!< Rx FIFO trigger level 11 */
#define I2S_RFCR0_RXCHDT_LVL12              (0xBUL << I2S_RFCR0_RXCHDT_Pos)     /*!< Rx FIFO trigger level 12 */
#define I2S_RFCR0_RXCHDT_LVL13              (0xCUL << I2S_RFCR0_RXCHDT_Pos)     /*!< Rx FIFO trigger level 13 */
#define I2S_RFCR0_RXCHDT_LVL14              (0xDUL << I2S_RFCR0_RXCHDT_Pos)     /*!< Rx FIFO trigger level 14 */
#define I2S_RFCR0_RXCHDT_LVL15              (0xEUL << I2S_RFCR0_RXCHDT_Pos)     /*!< Rx FIFO trigger level 15 */
#define I2S_RFCR0_RXCHDT_LVL16              (0xFUL << I2S_RFCR0_RXCHDT_Pos)     /*!< Rx FIFO trigger level 16 */

/*******************  Bit definition for I2S_TFCR0 register  ******************/

/** @brief TFCR0[3:0]: Tx FIFO trigger level */
#define I2S_TFCR0_TXCHET_Pos                (0U)
#define I2S_TFCR0_TXCHET_Msk                (0xFUL << I2S_TFCR0_TXCHET_Pos)
#define I2S_TFCR0_TXCHET                    I2S_TFCR0_TXCHET_Msk

#define I2S_TFCR0_TXCHET_LVL1               (0x0UL << I2S_TFCR0_TXCHET_Pos)     /*!< Tx FIFO trigger level 1 */
#define I2S_TFCR0_TXCHET_LVL2               (0x1UL << I2S_TFCR0_TXCHET_Pos)     /*!< Tx FIFO trigger level 2 */
#define I2S_TFCR0_TXCHET_LVL3               (0x2UL << I2S_TFCR0_TXCHET_Pos)     /*!< Tx FIFO trigger level 3 */
#define I2S_TFCR0_TXCHET_LVL4               (0x3UL << I2S_TFCR0_TXCHET_Pos)     /*!< Tx FIFO trigger level 4 */
#define I2S_TFCR0_TXCHET_LVL5               (0x4UL << I2S_TFCR0_TXCHET_Pos)     /*!< Tx FIFO trigger level 5 */
#define I2S_TFCR0_TXCHET_LVL6               (0x5UL << I2S_TFCR0_TXCHET_Pos)     /*!< Tx FIFO trigger level 6 */
#define I2S_TFCR0_TXCHET_LVL7               (0x6UL << I2S_TFCR0_TXCHET_Pos)     /*!< Tx FIFO trigger level 7 */
#define I2S_TFCR0_TXCHET_LVL8               (0x7UL << I2S_TFCR0_TXCHET_Pos)     /*!< Tx FIFO trigger level 8 */
#define I2S_TFCR0_TXCHET_LVL9               (0x8UL << I2S_TFCR0_TXCHET_Pos)     /*!< Tx FIFO trigger level 9 */
#define I2S_TFCR0_TXCHET_LVL10              (0x9UL << I2S_TFCR0_TXCHET_Pos)     /*!< Tx FIFO trigger level 10 */
#define I2S_TFCR0_TXCHET_LVL11              (0xAUL << I2S_TFCR0_TXCHET_Pos)     /*!< Tx FIFO trigger level 11 */
#define I2S_TFCR0_TXCHET_LVL12              (0xBUL << I2S_TFCR0_TXCHET_Pos)     /*!< Tx FIFO trigger level 12 */
#define I2S_TFCR0_TXCHET_LVL13              (0xCUL << I2S_TFCR0_TXCHET_Pos)     /*!< Tx FIFO trigger level 13 */
#define I2S_TFCR0_TXCHET_LVL14              (0xDUL << I2S_TFCR0_TXCHET_Pos)     /*!< Tx FIFO trigger level 14 */
#define I2S_TFCR0_TXCHET_LVL15              (0xEUL << I2S_TFCR0_TXCHET_Pos)     /*!< Tx FIFO trigger level 15 */
#define I2S_TFCR0_TXCHET_LVL16              (0xFUL << I2S_TFCR0_TXCHET_Pos)     /*!< Tx FIFO trigger level 16 */

/*******************  Bit definition for I2S_RFF0 register  *******************/

/** @brief RFF0[0]: Receive Channel 0 FIFO reset */
#define I2S_RFF0_RXCHFR_Pos                 (0U)
#define I2S_RFF0_RXCHFR_Msk                 (0x1UL << I2S_RFF0_RXCHFR_Pos)
#define I2S_RFF0_RXCHFR                     I2S_RFF0_RXCHFR_Msk

/*******************  Bit definition for I2S_TFF0 register  *******************/

/** @brief TFF0[0]: Transmit Channel 0 FIFO reset */
#define I2S_TFF0_TXCHFR_Pos                 (0U)
#define I2S_TFF0_TXCHFR_Msk                 (0x1UL << I2S_TFF0_TXCHFR_Pos)
#define I2S_TFF0_TXCHFR                     I2S_TFF0_TXCHFR_Msk

/*******************  Bit definition for I2S_RXDMA register  ******************/

/** @brief RXDMA[31:0]: Receiver DMA register */
#define I2S_RXDMA_Pos                       (0U)
#define I2S_RXDMA_Msk                       (0xFFFFFFFFUL << I2S_RXDMA_Pos)
#define I2S_RXDMA                           I2S_RXDMA_Msk

/*******************  Bit definition for I2S_TXDMA register  ******************/

/** @brief TXDMA[31:0]: Transmitter DMA register */
#define I2S_TXDMA_Pos                       (0U)
#define I2S_TXDMA_Msk                       (0xFFFFFFFFUL << I2S_TXDMA_Pos)
#define I2S_TXDMA                           I2S_TXDMA_Msk

/***************  Bit definition for I2S_COMP_PARAM_2 register  ***************/

/** @brief COMP_PARAM_2[2:0]: Rx resolution for WORDSIZE_0 */
#define I2S_COMP_PARAM_2_RX_WORDSIZE0_Pos   (0U)
#define I2S_COMP_PARAM_2_RX_WORDSIZE0_Msk   (0x7UL << I2S_COMP_PARAM_2_RX_WORDSIZE0_Pos)
#define I2S_COMP_PARAM_2_RX_WORDSIZE0       I2S_COMP_PARAM_2_RX_WORDSIZE0_Msk

#define I2S_COMP_PARAM_2_RX_WORDSIZE0_12BIT (0x0UL << I2S_COMP_PARAM_2_RX_WORDSIZE0_Pos)    /*!< 12-bit resolution */
#define I2S_COMP_PARAM_2_RX_WORDSIZE0_16BIT (0x1UL << I2S_COMP_PARAM_2_RX_WORDSIZE0_Pos)    /*!< 16-bit resolution */
#define I2S_COMP_PARAM_2_RX_WORDSIZE0_20BIT (0x2UL << I2S_COMP_PARAM_2_RX_WORDSIZE0_Pos)    /*!< 20-bit resolution */
#define I2S_COMP_PARAM_2_RX_WORDSIZE0_24BIT (0x3UL << I2S_COMP_PARAM_2_RX_WORDSIZE0_Pos)    /*!< 24-bit resolution */
#define I2S_COMP_PARAM_2_RX_WORDSIZE0_32BIT (0x4UL << I2S_COMP_PARAM_2_RX_WORDSIZE0_Pos)    /*!< 32-bit resolution */

/** @brief COMP_PARAM_2[5:3]: Rx resolution for WORDSIZE_1 */
#define I2S_COMP_PARAM_2_RX_WORDSIZE1_Pos   (3U)
#define I2S_COMP_PARAM_2_RX_WORDSIZE1_Msk   (0x7UL << I2S_COMP_PARAM_2_RX_WORDSIZE1_Pos)
#define I2S_COMP_PARAM_2_RX_WORDSIZE1       I2S_COMP_PARAM_2_RX_WORDSIZE1_Msk

#define I2S_COMP_PARAM_2_RX_WORDSIZE1_12BIT (0x0UL << I2S_COMP_PARAM_2_RX_WORDSIZE1_Pos)    /*!< 12-bit resolution */
#define I2S_COMP_PARAM_2_RX_WORDSIZE1_16BIT (0x1UL << I2S_COMP_PARAM_2_RX_WORDSIZE1_Pos)    /*!< 16-bit resolution */
#define I2S_COMP_PARAM_2_RX_WORDSIZE1_20BIT (0x2UL << I2S_COMP_PARAM_2_RX_WORDSIZE1_Pos)    /*!< 20-bit resolution */
#define I2S_COMP_PARAM_2_RX_WORDSIZE1_24BIT (0x3UL << I2S_COMP_PARAM_2_RX_WORDSIZE1_Pos)    /*!< 24-bit resolution */
#define I2S_COMP_PARAM_2_RX_WORDSIZE1_32BIT (0x4UL << I2S_COMP_PARAM_2_RX_WORDSIZE1_Pos)    /*!< 32-bit resolution */

/** @brief COMP_PARAM_2[9:7]: Rx resolution for WORDSIZE_2 */
#define I2S_COMP_PARAM_2_RX_WORDSIZE2_Pos   (7U)
#define I2S_COMP_PARAM_2_RX_WORDSIZE2_Msk   (0x7UL << I2S_COMP_PARAM_2_RX_WORDSIZE2_Pos)
#define I2S_COMP_PARAM_2_RX_WORDSIZE2       I2S_COMP_PARAM_2_RX_WORDSIZE2_Msk

#define I2S_COMP_PARAM_2_RX_WORDSIZE2_12BIT (0x0UL << I2S_COMP_PARAM_2_RX_WORDSIZE2_Pos)    /*!< 12-bit resolution */
#define I2S_COMP_PARAM_2_RX_WORDSIZE2_16BIT (0x1UL << I2S_COMP_PARAM_2_RX_WORDSIZE2_Pos)    /*!< 16-bit resolution */
#define I2S_COMP_PARAM_2_RX_WORDSIZE2_20BIT (0x2UL << I2S_COMP_PARAM_2_RX_WORDSIZE2_Pos)    /*!< 20-bit resolution */
#define I2S_COMP_PARAM_2_RX_WORDSIZE2_24BIT (0x3UL << I2S_COMP_PARAM_2_RX_WORDSIZE2_Pos)    /*!< 24-bit resolution */
#define I2S_COMP_PARAM_2_RX_WORDSIZE2_32BIT (0x4UL << I2S_COMP_PARAM_2_RX_WORDSIZE2_Pos)    /*!< 32-bit resolution */

/** @brief COMP_PARAM_2[12:10]: Rx resolution for WORDSIZE_3 */
#define I2S_COMP_PARAM_2_RX_WORDSIZE3_Pos   (10U)
#define I2S_COMP_PARAM_2_RX_WORDSIZE3_Msk   (0x7UL << I2S_COMP_PARAM_2_RX_WORDSIZE3_Pos)
#define I2S_COMP_PARAM_2_RX_WORDSIZE3       I2S_COMP_PARAM_2_RX_WORDSIZE3_Msk

#define I2S_COMP_PARAM_2_RX_WORDSIZE3_12BIT (0x0UL << I2S_COMP_PARAM_2_RX_WORDSIZE3_Pos)    /*!< 12-bit resolution */
#define I2S_COMP_PARAM_2_RX_WORDSIZE3_16BIT (0x1UL << I2S_COMP_PARAM_2_RX_WORDSIZE3_Pos)    /*!< 16-bit resolution */
#define I2S_COMP_PARAM_2_RX_WORDSIZE3_20BIT (0x2UL << I2S_COMP_PARAM_2_RX_WORDSIZE3_Pos)    /*!< 20-bit resolution */
#define I2S_COMP_PARAM_2_RX_WORDSIZE3_24BIT (0x3UL << I2S_COMP_PARAM_2_RX_WORDSIZE3_Pos)    /*!< 24-bit resolution */
#define I2S_COMP_PARAM_2_RX_WORDSIZE3_32BIT (0x4UL << I2S_COMP_PARAM_2_RX_WORDSIZE3_Pos)    /*!< 32-bit resolution */

/***************  Bit definition for I2S_COMP_PARAM_1 register  ***************/

/** @brief COMP_PARAM_1[1:0]: APB data width */
#define I2S_COMP_PARAM_1_APB_DW_Pos         (0U)
#define I2S_COMP_PARAM_1_APB_DW_Msk         (0x3UL << I2S_COMP_PARAM_1_APB_DW_Pos)
#define I2S_COMP_PARAM_1_APB_DW             I2S_COMP_PARAM_1_APB_DW_Msk

#define I2S_COMP_PARAM_1_APB_DW_8BIT        (0x0UL << I2S_COMP_PARAM_1_APB_DW_Pos)  /*!< 8-bit APB data width */
#define I2S_COMP_PARAM_1_APB_DW_16BIT       (0x1UL << I2S_COMP_PARAM_1_APB_DW_Pos)  /*!< 16-bit APB data width */
#define I2S_COMP_PARAM_1_APB_DW_32BIT       (0x2UL << I2S_COMP_PARAM_1_APB_DW_Pos)  /*!< 32-bit APB data width */

/** @brief COMP_PARAM_1[3:2]: FIFO depth for Tx and Rx channels */
#define I2S_COMP_PARAM_1_FIFO_DEPTH_Pos     (2U)
#define I2S_COMP_PARAM_1_FIFO_DEPTH_Msk     (0x3UL << I2S_COMP_PARAM_1_FIFO_DEPTH_Pos)
#define I2S_COMP_PARAM_1_FIFO_DEPTH         I2S_COMP_PARAM_1_FIFO_DEPTH_Msk

#define I2S_COMP_PARAM_1_FIFO_DEPTH_2       (0x0UL << I2S_COMP_PARAM_1_FIFO_DEPTH_Pos)  /*!< FIFO depth is 2 */
#define I2S_COMP_PARAM_1_FIFO_DEPTH_4       (0x1UL << I2S_COMP_PARAM_1_FIFO_DEPTH_Pos)  /*!< FIFO depth is 4 */
#define I2S_COMP_PARAM_1_FIFO_DEPTH_8       (0x2UL << I2S_COMP_PARAM_1_FIFO_DEPTH_Pos)  /*!< FIFO depth is 8 */
#define I2S_COMP_PARAM_1_FIFO_DEPTH_16      (0x3UL << I2S_COMP_PARAM_1_FIFO_DEPTH_Pos)  /*!< FIFO depth is 16 */

/** @brief COMP_PARAM_1[4]: Master mode enable */
#define I2S_COMP_PARAM_1_MODE_EN_Pos        (4U)
#define I2S_COMP_PARAM_1_MODE_EN_Msk        (0x1UL << I2S_COMP_PARAM_1_MODE_EN_Pos)
#define I2S_COMP_PARAM_1_MODE_EN            I2S_COMP_PARAM_1_MODE_EN_Msk
/** @brief COMP_PARAM_1[5]: Transmitter enable */
#define I2S_COMP_PARAM_1_TX_EN_Pos          (5U)
#define I2S_COMP_PARAM_1_TX_EN_Msk          (0x1UL << I2S_COMP_PARAM_1_TX_EN_Pos)
#define I2S_COMP_PARAM_1_TX_EN              I2S_COMP_PARAM_1_TX_EN_Msk
/** @brief COMP_PARAM_1[6]: Receiver enable */
#define I2S_COMP_PARAM_1_RX_EN_Pos          (6U)
#define I2S_COMP_PARAM_1_RX_EN_Msk          (0x1UL << I2S_COMP_PARAM_1_RX_EN_Pos)
#define I2S_COMP_PARAM_1_RX_EN              I2S_COMP_PARAM_1_RX_EN_Msk

/** @brief COMP_PARAM_1[8:7]: The number of Rx channels */
#define I2S_COMP_PARAM_1_RX_CHANNELS_Pos    (7U)
#define I2S_COMP_PARAM_1_RX_CHANNELS_Msk    (0x3UL << I2S_COMP_PARAM_1_RX_CHANNELS_Pos)
#define I2S_COMP_PARAM_1_RX_CHANNELS        I2S_COMP_PARAM_1_RX_CHANNELS_Msk

#define I2S_COMP_PARAM_1_RX_CHANNELS_1      (0x0UL << I2S_COMP_PARAM_1_RX_CHANNELS_Pos)     /*!< 1 receive channel */
#define I2S_COMP_PARAM_1_RX_CHANNELS_2      (0x1UL << I2S_COMP_PARAM_1_RX_CHANNELS_Pos)     /*!< 2 receive channels */
#define I2S_COMP_PARAM_1_RX_CHANNELS_3      (0x2UL << I2S_COMP_PARAM_1_RX_CHANNELS_Pos)     /*!< 3 receive channels */
#define I2S_COMP_PARAM_1_RX_CHANNELS_4      (0x3UL << I2S_COMP_PARAM_1_RX_CHANNELS_Pos)     /*!< 4 receive channels */

/** @brief COMP_PARAM_1[10:9]: The number of Tx channels */
#define I2S_COMP_PARAM_1_TX_CHANNELS_Pos    (9U)
#define I2S_COMP_PARAM_1_TX_CHANNELS_Msk    (0x3UL << I2S_COMP_PARAM_1_TX_CHANNELS_Pos)
#define I2S_COMP_PARAM_1_TX_CHANNELS        I2S_COMP_PARAM_1_TX_CHANNELS_Msk

#define I2S_COMP_PARAM_1_TX_CHANNELS_1      (0x0UL << I2S_COMP_PARAM_1_TX_CHANNELS_Pos)     /*!< 1 transmit channel */
#define I2S_COMP_PARAM_1_TX_CHANNELS_2      (0x1UL << I2S_COMP_PARAM_1_TX_CHANNELS_Pos)     /*!< 2 transmit channels */
#define I2S_COMP_PARAM_1_TX_CHANNELS_3      (0x2UL << I2S_COMP_PARAM_1_TX_CHANNELS_Pos)     /*!< 3 transmit channels */
#define I2S_COMP_PARAM_1_TX_CHANNELS_4      (0x3UL << I2S_COMP_PARAM_1_TX_CHANNELS_Pos)     /*!< 4 transmit channels */

/** @brief COMP_PARAM_1[18:16]: Tx resolution for WORDSIZE_0 */
#define I2S_COMP_PARAM_1_TX_WORDSIZE0_Pos   (16U)
#define I2S_COMP_PARAM_1_TX_WORDSIZE0_Msk   (0x7UL << I2S_COMP_PARAM_1_TX_WORDSIZE0_Pos)
#define I2S_COMP_PARAM_1_TX_WORDSIZE0       I2S_COMP_PARAM_1_TX_WORDSIZE0_Msk

#define I2S_COMP_PARAM_1_TX_WORDSIZE0_12BIT (0x0UL << I2S_COMP_PARAM_1_TX_WORDSIZE0_Pos)    /*!< 12-bit resolution */
#define I2S_COMP_PARAM_1_TX_WORDSIZE0_16BIT (0x1UL << I2S_COMP_PARAM_1_TX_WORDSIZE0_Pos)    /*!< 16-bit resolution */
#define I2S_COMP_PARAM_1_TX_WORDSIZE0_20BIT (0x2UL << I2S_COMP_PARAM_1_TX_WORDSIZE0_Pos)    /*!< 20-bit resolution */
#define I2S_COMP_PARAM_1_TX_WORDSIZE0_24BIT (0x3UL << I2S_COMP_PARAM_1_TX_WORDSIZE0_Pos)    /*!< 24-bit resolution */
#define I2S_COMP_PARAM_1_TX_WORDSIZE0_32BIT (0x4UL << I2S_COMP_PARAM_1_TX_WORDSIZE0_Pos)    /*!< 32-bit resolution */

/** @brief COMP_PARAM_1[21:19]: Tx resolution for WORDSIZE_1 */
#define I2S_COMP_PARAM_1_TX_WORDSIZE1_Pos   (19U)
#define I2S_COMP_PARAM_1_TX_WORDSIZE1_Msk   (0x7UL << I2S_COMP_PARAM_1_TX_WORDSIZE1_Pos)
#define I2S_COMP_PARAM_1_TX_WORDSIZE1       I2S_COMP_PARAM_1_TX_WORDSIZE1_Msk

#define I2S_COMP_PARAM_1_TX_WORDSIZE1_12BIT (0x0UL << I2S_COMP_PARAM_1_TX_WORDSIZE1_Pos)    /*!< 12-bit resolution */
#define I2S_COMP_PARAM_1_TX_WORDSIZE1_16BIT (0x1UL << I2S_COMP_PARAM_1_TX_WORDSIZE1_Pos)    /*!< 16-bit resolution */
#define I2S_COMP_PARAM_1_TX_WORDSIZE1_20BIT (0x2UL << I2S_COMP_PARAM_1_TX_WORDSIZE1_Pos)    /*!< 20-bit resolution */
#define I2S_COMP_PARAM_1_TX_WORDSIZE1_24BIT (0x3UL << I2S_COMP_PARAM_1_TX_WORDSIZE1_Pos)    /*!< 24-bit resolution */
#define I2S_COMP_PARAM_1_TX_WORDSIZE1_32BIT (0x4UL << I2S_COMP_PARAM_1_TX_WORDSIZE1_Pos)    /*!< 32-bit resolution */

/** @brief COMP_PARAM_1[24:22]: Tx resolution for WORDSIZE_2 */
#define I2S_COMP_PARAM_1_TX_WORDSIZE2_Pos   (22U)
#define I2S_COMP_PARAM_1_TX_WORDSIZE2_Msk   (0x7UL << I2S_COMP_PARAM_1_TX_WORDSIZE2_Pos)
#define I2S_COMP_PARAM_1_TX_WORDSIZE2       I2S_COMP_PARAM_1_TX_WORDSIZE2_Msk

#define I2S_COMP_PARAM_1_TX_WORDSIZE2_12BIT (0x0UL << I2S_COMP_PARAM_1_TX_WORDSIZE2_Pos)    /*!< 12-bit resolution */
#define I2S_COMP_PARAM_1_TX_WORDSIZE2_16BIT (0x1UL << I2S_COMP_PARAM_1_TX_WORDSIZE2_Pos)    /*!< 16-bit resolution */
#define I2S_COMP_PARAM_1_TX_WORDSIZE2_20BIT (0x2UL << I2S_COMP_PARAM_1_TX_WORDSIZE2_Pos)    /*!< 20-bit resolution */
#define I2S_COMP_PARAM_1_TX_WORDSIZE2_24BIT (0x3UL << I2S_COMP_PARAM_1_TX_WORDSIZE2_Pos)    /*!< 24-bit resolution */
#define I2S_COMP_PARAM_1_TX_WORDSIZE2_32BIT (0x4UL << I2S_COMP_PARAM_1_TX_WORDSIZE2_Pos)    /*!< 32-bit resolution */

/** @brief COMP_PARAM_1[27:25]: Tx resolution for WORDSIZE_3 */
#define I2S_COMP_PARAM_1_TX_WORDSIZE3_Pos   (25U)
#define I2S_COMP_PARAM_1_TX_WORDSIZE3_Msk   (0x7UL << I2S_COMP_PARAM_1_TX_WORDSIZE3_Pos)
#define I2S_COMP_PARAM_1_TX_WORDSIZE3       I2S_COMP_PARAM_1_TX_WORDSIZE3_Msk

#define I2S_COMP_PARAM_1_TX_WORDSIZE3_12BIT (0x0UL << I2S_COMP_PARAM_1_TX_WORDSIZE3_Pos)    /*!< 12-bit resolution */
#define I2S_COMP_PARAM_1_TX_WORDSIZE3_16BIT (0x1UL << I2S_COMP_PARAM_1_TX_WORDSIZE3_Pos)    /*!< 16-bit resolution */
#define I2S_COMP_PARAM_1_TX_WORDSIZE3_20BIT (0x2UL << I2S_COMP_PARAM_1_TX_WORDSIZE3_Pos)    /*!< 20-bit resolution */
#define I2S_COMP_PARAM_1_TX_WORDSIZE3_24BIT (0x3UL << I2S_COMP_PARAM_1_TX_WORDSIZE3_Pos)    /*!< 24-bit resolution */
#define I2S_COMP_PARAM_1_TX_WORDSIZE3_32BIT (0x4UL << I2S_COMP_PARAM_1_TX_WORDSIZE3_Pos)    /*!< 32-bit resolution */

/***************  Bit definition for I2S_COMP_VERSION register  ***************/

/** @brief COMP_VERSION[31:0]: I2S component version */
#define I2S_COMP_VERSION_Pos                (0U)
#define I2S_COMP_VERSION_Msk                (0xFFFFFFFFUL << I2S_COMP_VERSION_Pos)
#define I2S_COMP_VERSION                    I2S_COMP_VERSION_Msk

/*****************  Bit definition for I2S_COMP_TYPE register  ****************/

/** @brief COMP_TYPE[31:0]: I2S component type */
#define I2S_COMP_TYPE_Pos                   (0U)
#define I2S_COMP_TYPE_Msk                   (0xFFFFFFFFUL << I2S_COMP_TYPE_Pos)
#define I2S_COMP_TYPE                       I2S_COMP_TYPE_Msk

/*******************  Bit definition for I2S_DMACR register  ******************/

/** @brief DMACR[16]: Receiver DMA enable */
#define I2S_DMACR_DMAEN_RX_Pos              (16U)
#define I2S_DMACR_DMAEN_RX_Msk              (0x1UL << I2S_DMACR_DMAEN_RX_Pos)
#define I2S_DMACR_DMAEN_RX                  I2S_DMACR_DMAEN_RX_Msk
/** @brief DMACR[17]: Transmitter DMA enable */
#define I2S_DMACR_DMAEN_TX_Pos              (17U)
#define I2S_DMACR_DMAEN_TX_Msk              (0x1UL << I2S_DMACR_DMAEN_TX_Pos)
#define I2S_DMACR_DMAEN_TX                  I2S_DMACR_DMAEN_TX_Msk

/******************************************************************************/
/*                       Quad Serial Peripheral Interface                     */
/******************************************************************************/

/*** Bits definition for CTRLR0 register  *********************************************/

/*** It is impossible to write to this register when the QSPI is enabled (QSPIENR) ****/

/** @brief CTRLR0[5:4]: Frame Format */
#define QSPI_CTRLR0_FRF_Pos                 (4U)
#define QSPI_CTRLR0_FRF_Msk                 (0x3UL << QSPI_CTRLR0_FRF_Pos)

#define QSPI_CTRLR0_FRF_SPI                 (0x0UL << QSPI_CTRLR0_FRF_Pos)
#define QSPI_CTRLR0_FRF_SSP                 (0x1UL << QSPI_CTRLR0_FRF_Pos)
#define QSPI_CTRLR0_FRF_MW                  (0x2UL << QSPI_CTRLR0_FRF_Pos)

/** @brief CTRLR0[6]: Serial Clock Phase */
#define QSPI_CTRLR0_SCPH_Pos                (6U)
#define QSPI_CTRLR0_SCPH_Msk                (0x1UL << QSPI_CTRLR0_SCPH_Pos)

#define QSPI_CTRLR0_SCPH_MIDDLE             (0x0UL << QSPI_CTRLR0_SCPH_Pos)
#define QSPI_CTRLR0_SCPH_START              (0x1UL << QSPI_CTRLR0_SCPH_Pos)

/** @brief CTRLR0[7]: Serial Clock Polarity */
#define QSPI_CTRLR0_SCPOL_Pos               (7U)
#define QSPI_CTRLR0_SCPOL_Msk               (0x1UL << QSPI_CTRLR0_SCPOL_Pos)

#define QSPI_CTRLR0_SCPOL_LOW               (0x0UL << QSPI_CTRLR0_SCPOL_Pos)
#define QSPI_CTRLR0_SCPOL_HIGH              (0x1UL << QSPI_CTRLR0_SCPOL_Pos)

/** @brief CTRLR0[9:8]: Transfer Mode */
#define QSPI_CTRLR0_TMOD_Pos                (8U)
#define QSPI_CTRLR0_TMOD_Msk                (0x3UL << QSPI_CTRLR0_TMOD_Pos)

#define QSPI_CTRLR0_TMOD_TX_RX              (0x0UL << QSPI_CTRLR0_TMOD_Pos)
#define QSPI_CTRLR0_TMOD_TX                 (0x1UL << QSPI_CTRLR0_TMOD_Pos)
#define QSPI_CTRLR0_TMOD_RX                 (0x2UL << QSPI_CTRLR0_TMOD_Pos)
#define QSPI_CTRLR0_TMOD_EEPROM_READ        (0x3UL << QSPI_CTRLR0_TMOD_Pos)

/** @brief CTRLR0[11]: Shift Register Loop */
#define QSPI_CTRLR0_SRL_Pos                 (11U)
#define QSPI_CTRLR0_SRL_Msk                 (0x1UL << QSPI_CTRLR0_SRL_Pos)

#define QSPI_CTRLR0_SRL_NORMAL              (0x0UL << QSPI_CTRLR0_SRL_Pos)
#define QSPI_CTRLR0_SRL_TESTING             (0x1UL << QSPI_CTRLR0_SRL_Pos)

/** @brief CTRLR0[15:12]: Control Frame Size */
#define QSPI_CTRLR0_CFS_Pos                 (12U)
#define QSPI_CTRLR0_CFS_Msk                 (0xFUL << QSPI_CTRLR0_CFS_Pos)

#define QSPI_CTRLR0_CFS_1                   (0x0UL << QSPI_CTRLR0_CFS_Pos)
#define QSPI_CTRLR0_CFS_2                   (0x1UL << QSPI_CTRLR0_CFS_Pos)
#define QSPI_CTRLR0_CFS_3                   (0x2UL << QSPI_CTRLR0_CFS_Pos)
#define QSPI_CTRLR0_CFS_4                   (0x3UL << QSPI_CTRLR0_CFS_Pos)
#define QSPI_CTRLR0_CFS_5                   (0x4UL << QSPI_CTRLR0_CFS_Pos)
#define QSPI_CTRLR0_CFS_6                   (0x5UL << QSPI_CTRLR0_CFS_Pos)
#define QSPI_CTRLR0_CFS_7                   (0x6UL << QSPI_CTRLR0_CFS_Pos)
#define QSPI_CTRLR0_CFS_8                   (0x7UL << QSPI_CTRLR0_CFS_Pos)
#define QSPI_CTRLR0_CFS_9                   (0x8UL << QSPI_CTRLR0_CFS_Pos)
#define QSPI_CTRLR0_CFS_10                  (0x9UL << QSPI_CTRLR0_CFS_Pos)
#define QSPI_CTRLR0_CFS_11                  (0xAUL << QSPI_CTRLR0_CFS_Pos)
#define QSPI_CTRLR0_CFS_12                  (0xBUL << QSPI_CTRLR0_CFS_Pos)
#define QSPI_CTRLR0_CFS_13                  (0xDUL << QSPI_CTRLR0_CFS_Pos)
#define QSPI_CTRLR0_CFS_14                  (0xDUL << QSPI_CTRLR0_CFS_Pos)
#define QSPI_CTRLR0_CFS_15                  (0xEUL << QSPI_CTRLR0_CFS_Pos)
#define QSPI_CTRLR0_CFS_16                  (0xFUL << QSPI_CTRLR0_CFS_Pos)

/** @brief CTRLR0[20:16]: Data Frame Size */
#define QSPI_CTRLR0_DFS_32_Pos              (16U)
#define QSPI_CTRLR0_DFS_32_Msk              (0x1FU << QSPI_CTRLR0_DFS_32_Pos)

#define QSPI_CTRLR0_DFS_32_04_BIT           (0x03UL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_05_BIT           (0x04UL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_06_BIT           (0x05UL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_07_BIT           (0x06UL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_08_BIT           (0x07UL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_09_BIT           (0x08UL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_10_BIT           (0x09UL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_11_BIT           (0x0AUL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_12_BIT           (0x0BUL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_13_BIT           (0x0CUL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_14_BIT           (0x0DUL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_15_BIT           (0x0EUL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_16_BIT           (0x0FUL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_17_BIT           (0x10UL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_18_BIT           (0x11UL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_19_BIT           (0x12UL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_20_BIT           (0x13UL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_21_BIT           (0x14UL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_22_BIT           (0x15UL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_23_BIT           (0x16UL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_24_BIT           (0x17UL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_25_BIT           (0x18UL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_26_BIT           (0x19UL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_27_BIT           (0x1AUL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_28_BIT           (0x1BUL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_29_BIT           (0x1CUL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_30_BIT           (0x1DUL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_31_BIT           (0x1EUL << QSPI_CTRLR0_DFS_32_Pos)
#define QSPI_CTRLR0_DFS_32_32_BIT           (0x1FUL << QSPI_CTRLR0_DFS_32_Pos)

/** @brief CTRLR0[22:21]: SPI Frame Format */
#define QSPI_CTRLR0_SPI_FRF_Pos             (21U)
#define QSPI_CTRLR0_SPI_FRF_Msk             (0x3UL << QSPI_CTRLR0_SPI_FRF_Pos)

#define QSPI_CTRLR0_SPI_FRF_STD             (0x0UL << QSPI_CTRLR0_SPI_FRF_Pos)
#define QSPI_CTRLR0_SPI_FRF_DUAL            (0x1UL << QSPI_CTRLR0_SPI_FRF_Pos)
#define QSPI_CTRLR0_SPI_FRF_QUAD            (0x2UL << QSPI_CTRLR0_SPI_FRF_Pos)

/** @brief CTRLR0[24]: Slave Select Toggle Enable */
#define QSPI_CTRLR0_SSTE_Pos                (24U)
#define QSPI_CTRLR0_SSTE_Msk                (0x1UL << QSPI_CTRLR0_SSTE_Pos)

#define QSPI_CTRLR0_SSTE_DISABLED           (0x0UL << QSPI_CTRLR0_SSTE_Pos)
#define QSPI_CTRLR0_SSTE_ENABLED            (0x1UL << QSPI_CTRLR0_SSTE_Pos)

/** @brief CTRLR0[25]: Set the Endianness for XIP and data register reads */
#define QSPI_CTRLR0_SECONV_Pos              (25U)
#define QSPI_CTRLR0_SECONV_Msk              (0x1UL << QSPI_CTRLR0_SECONV_Pos)

#define QSPI_CTRLR0_SECONV_DISABLED         (0x0UL << QSPI_CTRLR0_SECONV_Pos)
#define QSPI_CTRLR0_SECONV_ENABLED          (0x1UL << QSPI_CTRLR0_SECONV_Pos)

/***  Bits definition for CTRLR1 register  ********************************************/

/*** It is impossible to write to this register when the QSPI is enabled (QSPIENR) ****/

/** @brief CTRLR1[15:0]: Number of Data Frames */
#define QSPI_CTRLR1_NDF_Pos                 (0U)
#define QSPI_CTRLR1_NDF_Msk                 (0xFFFFUL << QSPI_CTRLR1_NDF_Pos)

/***  Bits definition for QSPIENR register  *******************************************/

/** @brief QSPIENR[0]: QSPI Enable */
#define QSPI_QSPIENR_QSPI_EN_Pos            (0U)
#define QSPI_QSPIENR_QSPI_EN_Msk            (0x1UL << QSPI_QSPIENR_QSPI_EN_Pos)

#define QSPI_QSPIENR_QSPI_EN_DISABLED       (0x0UL << QSPI_QSPIENR_QSPI_EN_Pos)
#define QSPI_QSPIENR_QSPI_EN_ENABLED        (0x1UL << QSPI_QSPIENR_QSPI_EN_Pos)

/*** Bits definition for MWCR register  ***********************************************/

/*** It is impossible to write to this register when the QSPI is enabled (QSPIENR) ****/

/** @brief MWCR[0]: Microwire Transfer Mode */
#define QSPI_MWCR_MWMOD_Pos                 (0U)
#define QSPI_MWCR_MWMOD_Msk                 (0x1UL << QSPI_MWCR_MWMOD_Pos)

#define QSPI_MWCR_MWMOD_NON_SEQUENTIAL      (0x0UL << QSPI_MWCR_MWMOD_Pos)
#define QSPI_MWCR_MWMOD_SEQUENTIAL          (0x1UL << QSPI_MWCR_MWMOD_Pos)

/** @brief MWCR[1]: Microwire Control */
#define QSPI_MWCR_MDD_Pos                   (1U)
#define QSPI_MWCR_MDD_Msk                   (0x1UL << QSPI_MWCR_MDD_Pos)

#define QSPI_MWCR_MDD_RECEIVE               (0x0UL << QSPI_MWCR_MDD_Pos)
#define QSPI_MWCR_MDD_TRANSMIT              (0x1UL << QSPI_MWCR_MDD_Pos)

/** @brief MWCR[2]: Microwire Handshaking */
#define QSPI_MWCR_MHS_Pos                   (2U)
#define QSPI_MWCR_MHS_Msk                   (0x1UL << QSPI_MWCR_MHS_Pos)

#define QSPI_MWCR_MHS_DISABLE               (0x0UL << QSPI_MWCR_MHS_Pos)
#define QSPI_MWCR_MHS_ENABLE                (0x1UL << QSPI_MWCR_MHS_Pos)

/*** Bits definition for SER register  ************************************************/

/*** It is impossible to write to this register when the QSPI is enabled (QSPIENR) ****/

/** @brief SER[2:0]: Slave Select Enable Flag */
#define QSPI_SER_SER_Pos                    (0U)
#define QSPI_SER_SER_Msk                    (0x7UL << QSPI_SER_SER_Pos)

#define QSPI_SER_SER_NOT_SELECTED           (0x0UL << QSPI_SER_SER_Pos)
#define QSPI_SER_SER_0                      (0x1UL << QSPI_SER_SER_Pos)
#define QSPI_SER_SER_1                      (0x2UL << QSPI_SER_SER_Pos)
#define QSPI_SER_SER_2                      (0x4UL << QSPI_SER_SER_Pos)

/*** Bits definition for BAUDR register  **********************************************/

/*** It is impossible to write to this register when the QSPI is enabled (QSPIENR) ****/

/** @brief BAUDR[15:0]: SSI Clock Divider */
#define QSPI_BAUDR_SCKDV_Pos                (0U)
#define QSPI_BAUDR_SCKDV_Msk                (0xFFFFUL << QSPI_BAUDR_SCKDV_Pos)

/*** Bits definition for TXFTLR register  *********************************************/

/*** It is impossible to write to this register when the QSPI is enabled (QSPIENR) ****/

/** @brief TXFTLR[2:0]: Transmit FIFO Threshold */
#define QSPI_TXFTLR_TFT_Pos                 (0U)
#define QSPI_TXFTLR_TFT_Msk                 (0x7UL << QSPI_TXFTLR_TFT_Pos)

/*** Bits definition for RXFTLR register  *********************************************/

/*** It is impossible to write to this register when the QSPI is enabled (QSPIENR) ****/

/** @brief RXFTLR[2:0]: Receive FIFO Threshold */
#define QSPI_RXFTLR_RFT_Pos                 (0U)
#define QSPI_RXFTLR_RFT_Msk                 (0x7UL << QSPI_RXFTLR_RFT_Pos)

/*** Bits definition for TXFLR register  **********************************************/

/** @brief TXFLR[2:0]: Transmit FIFO Level */
#define QSPI_TXFLR_TXTFL_Pos                (0U)
#define QSPI_TXFLR_TXTFL_Msk                (0x7UL << QSPI_TXFLR_TXTFL_Pos)

/*** Bits definition for RXFLR register  **********************************************/

/** @brief RXFLR[2:0]: Receive FIFO Level */
#define QSPI_RXFLR_RXTFL_Pos                (0U)
#define QSPI_RXFLR_RXTFL_Msk                (0x7UL << QSPI_RXFLR_RXTFL_Pos)

/*** Bits definition for SR register  *************************************************/

/** @brief SR[0]: QSPI Busy Flag */
#define QSPI_SR_BUSY_Pos                    (0U)
#define QSPI_SR_BUSY_Msk                    (0x1UL << QSPI_SR_BUSY_Pos)

#define QSPI_SR_BUSY_INACTIVE               (0x0UL << QSPI_SR_BUSY_Pos)
#define QSPI_SR_BUSY_ACTIVE                 (0x1UL << QSPI_SR_BUSY_Pos)

/** @brief SR[1]: Transmit FIFO Not Full */
#define QSPI_SR_TFNF_Pos                    (1U)
#define QSPI_SR_TFNF_Msk                    (0x1UL << QSPI_SR_TFNF_Pos)

#define QSPI_SR_TFNF_FULL                   (0x0UL << QSPI_SR_TFNF_Pos)
#define QSPI_SR_TFNF_NON_FULL               (0x1UL << QSPI_SR_TFNF_Pos)

/** @brief SR[2]: Transmit FIFO Empty */
#define QSPI_SR_TFE_Pos                     (2U)
#define QSPI_SR_TFE_Msk                     (0x1UL << QSPI_SR_TFE_Pos)

#define QSPI_SR_TFE_NOT_EMPTY               (0x0UL << QSPI_SR_TFE_Pos)
#define QSPI_SR_TFE_EMPTY                   (0x1UL << QSPI_SR_TFE_Pos)

/** @brief SR[3]: Receive FIFO Not Empty */
#define QSPI_SR_RFNE_Pos                    (3U)
#define QSPI_SR_RFNE_Msk                    (0x1UL << QSPI_SR_RFNE_Pos)

#define QSPI_SR_RFNE_EMPTY                  (0x0UL << QSPI_SR_RFNE_Pos)
#define QSPI_SR_RFNE_NOT_EMPTY              (0x1UL << QSPI_SR_RFNE_Pos)

/** @brief SR[4]: Receive FIFO Full */
#define QSPI_SR_RFF_Pos                     (4U)
#define QSPI_SR_RFF_Msk                     (0x1UL << QSPI_SR_RFF_Pos)

#define QSPI_SR_RFF_NOT_FULL                (0x0UL << QSPI_SR_RFF_Pos)
#define QSPI_SR_RFF_FULL                    (0x1UL << QSPI_SR_RFF_Pos)

/** @brief SR[5]: Transmission Error */
#define QSPI_SR_TXE_Pos                     (5U)
#define QSPI_SR_TXE_Msk                     (0x1UL << QSPI_SR_TXE_Pos)

#define QSPI_SR_TXE_NO_ERROR                (0x0UL << QSPI_SR_TXE_Pos)
#define QSPI_SR_TXE_TX_ERROR                (0x1UL << QSPI_SR_TXE_Pos)

/** @brief SR[6]: Data Collision Error */
#define QSPI_SR_DCOL_Pos                    (6U)
#define QSPI_SR_DCOL_Msk                    (0x1UL << QSPI_SR_DCOL_Pos)

#define QSPI_SR_DCOL_NO_ERROR_CONDITION     (0x0UL << QSPI_SR_DCOL_Pos)
#define QSPI_SR_DCOL_TX_COLLISION_ERROR     (0x1UL << QSPI_SR_DCOL_Pos)

/*** Bits definition for IMR register  ************************************************/

/** @brief IMR[0]: Transmit FIFO Empty Interrupt Mask */
#define QSPI_IMR_TXEIM_Pos                  (0U)
#define QSPI_IMR_TXEIM_Msk                  (0x1UL << QSPI_IMR_TXEIM_Pos)

#define QSPI_IMR_TXEIM_MASKED               (0x0UL << QSPI_IMR_TXEIM_Pos)
#define QSPI_IMR_TXEIM_UNMASKED             (0x1UL << QSPI_IMR_TXEIM_Pos)

/** @brief IMR[1]: Transmit FIFO Overflow Interrupt Mask */
#define QSPI_IMR_TXOIM_Pos                  (1U)
#define QSPI_IMR_TXOIM_Msk                  (0x1UL << QSPI_IMR_TXOIM_Pos)

#define QSPI_IMR_TXOIM_MASKED               (0x0UL << QSPI_IMR_TXOIM_Pos)
#define QSPI_IMR_TXOIM_UNMASKED             (0x1UL << QSPI_IMR_TXOIM_Pos)

/** @brief IMR[2]: Receive FIFO Underflow Interrupt Mask */
#define QSPI_IMR_RXUIM_Pos                  (2U)
#define QSPI_IMR_RXUIM_Msk                  (0x1UL << QSPI_IMR_RXUIM_Pos)

#define QSPI_IMR_RXUIM_MASKED               (0x0UL << QSPI_IMR_RXUIM_Pos)
#define QSPI_IMR_RXUIM_UNMASKED             (0x1UL << QSPI_IMR_RXUIM_Pos)

/** @brief IMR[3]: Receive FIFO Overflow Interrupt Mask */
#define QSPI_IMR_RXOIM_Pos                  (3U)
#define QSPI_IMR_RXOIM_Msk                  (0x1UL << QSPI_IMR_RXOIM_Pos)

#define QSPI_IMR_RXOIM_MASKED               (0x0UL << QSPI_IMR_RXOIM_Pos)
#define QSPI_IMR_RXOIM_UNMASKED             (0x1UL << QSPI_IMR_RXOIM_Pos)

/** @brief IMR[4]: Receive FIFO Full Interrupt Mask */
#define QSPI_IMR_RXFIM_Pos                  (4U)
#define QSPI_IMR_RXFIM_Msk                  (0x1UL << QSPI_IMR_RXFIM_Pos)

#define QSPI_IMR_RXFIM_MASKED               (0x0UL << QSPI_IMR_RXFIM_Pos)
#define QSPI_IMR_RXFIM_UNMASKED             (0x1UL << QSPI_IMR_RXFIM_Pos)

/** @brief IMR[5]: Multi-Master Contention Interrupt Mask */
#define QSPI_IMR_MSTIM_Pos                  (5U)
#define QSPI_IMR_MSTIM_Msk                  (0x1UL << QSPI_IMR_MSTIM_Pos)

#define QSPI_IMR_MSTIM_MASKED               (0x0UL << QSPI_IMR_MSTIM_Pos)
#define QSPI_IMR_MSTIM_UNMASKED             (0x1UL << QSPI_IMR_MSTIM_Pos)

/*** Bits definition for ISR register  ************************************************/

/** @brief ISR[0]: Transmit FIFO Empty Interrupt Status */
#define QSPI_ISR_TXEIS_Pos                  (0U)
#define QSPI_ISR_TXEIS_Msk                  (0x1UL << QSPI_ISR_TXEIS_Pos)

#define QSPI_ISR_TXEIS_INACTIVE             (0x0UL << QSPI_ISR_TXEIS_Pos)
#define QSPI_ISR_TXEIS_ACTIVE               (0x1UL << QSPI_ISR_TXEIS_Pos)

/** @brief ISR[1]: Transmit FIFO Overflow Interrupt Status */
#define QSPI_ISR_TXOIS_Pos                  (1U)
#define QSPI_ISR_TXOIS_Msk                  (0x1UL << QSPI_ISR_TXOIS_Pos)

#define QSPI_ISR_TXOIS_INACTIVE             (0x0UL << QSPI_ISR_TXOIS_Pos)
#define QSPI_ISR_TXOIS_ACTIVE               (0x1UL << QSPI_ISR_TXOIS_Pos)

/** @brief ISR[2]: Receive FIFO Underflow Interrupt Status */
#define QSPI_ISR_RXUIS_Pos                  (2U)
#define QSPI_ISR_RXUIS_Msk                  (0x1UL << QSPI_ISR_RXUIS_Pos)

#define QSPI_ISR_RXUIS_INACTIVE             (0x0UL << QSPI_ISR_RXUIS_Pos)
#define QSPI_ISR_RXUIS_ACTIVE               (0x1UL << QSPI_ISR_RXUIS_Pos)

/** @brief ISR[3]: Receive FIFO Overflow Interrupt Status */
#define QSPI_ISR_RXOIS_Pos                  (3U)
#define QSPI_ISR_RXOIS_Msk                  (0x1UL << QSPI_ISR_RXOIS_Pos)

#define QSPI_ISR_RXOIS_INACTIVE             (0x0UL << QSPI_ISR_RXOIS_Pos)
#define QSPI_ISR_RXOIS_ACTIVE               (0x1UL << QSPI_ISR_RXOIS_Pos)

/** @brief ISR[4]: Receive FIFO Full Interrupt Status */
#define QSPI_ISR_RXFIS_Pos                  (4U)
#define QSPI_ISR_RXFIS_Msk                  (0x1UL << QSPI_ISR_RXFIS_Pos)

#define QSPI_ISR_RXFIS_INACTIVE             (0x0UL << QSPI_ISR_RXFIS_Pos)
#define QSPI_ISR_RXFIS_ACTIVE               (0x1UL << QSPI_ISR_RXFIS_Pos)

/** @brief ISR[5]: Multi-Master Contention Interrupt Status */
#define QSPI_ISR_MSTIS_Pos                  (5U)
#define QSPI_ISR_MSTIS_Msk                  (0x1UL << QSPI_ISR_MSTIS_Pos)

#define QSPI_ISR_MSTIS_INACTIVE             (0x0UL << QSPI_ISR_MSTIS_Pos)
#define QSPI_ISR_MSTIS_ACTIVE               (0x1UL << QSPI_ISR_MSTIS_Pos)

/*** Bits definition for RISR register  ***********************************************/

/** @brief RISR[0]: Transmit FIFO Empty Raw Interrupt Status */
#define QSPI_RISR_TXEIR_Pos                 (0U)
#define QSPI_RISR_TXEIR_Msk                 (0x1UL << QSPI_RISR_TXEIR_Pos)

#define QSPI_RISR_TXEIR_INACTIVE            (0x0UL << QSPI_RISR_TXEIR_Pos)
#define QSPI_RISR_TXEIR_ACTIVE              (0x1UL << QSPI_RISR_TXEIR_Pos)

/** @brief RISR[1]: Transmit FIFO Overflow Raw Interrupt Status */
#define QSPI_RISR_TXOIR_Pos                 (1U)
#define QSPI_RISR_TXOIR_Msk                 (0x1UL << QSPI_RISR_TXOIR_Pos)

#define QSPI_RISR_TXOIR_INACTIVE            (0x0UL << QSPI_RISR_TXOIR_Pos)
#define QSPI_RISR_TXOIR_ACTIVE              (0x1UL << QSPI_RISR_TXOIR_Pos)

/** @brief RISR[2]: Receive FIFO Underflow Raw Interrupt Status */
#define QSPI_RISR_RXUIR_Pos                 (2U)
#define QSPI_RISR_RXUIR_Msk                 (0x1UL << QSPI_RISR_RXUIR_Pos)

#define QSPI_RISR_RXUIR_INACTIVE            (0x0UL << QSPI_RISR_RXUIR_Pos)
#define QSPI_RISR_RXUIR_ACTIVE              (0x1UL << QSPI_RISR_RXUIR_Pos)

/** @brief RISR[3]: Receive FIFO Overflow Raw Interrupt Status */
#define QSPI_RISR_RXOIR_Pos                 (3U)
#define QSPI_RISR_RXOIR_Msk                 (0x1UL << QSPI_RISR_RXOIR_Pos)

#define QSPI_RISR_RXOIR_INACTIVE            (0x0UL << QSPI_RISR_RXOIR_Pos)
#define QSPI_RISR_RXOIR_ACTIVE              (0x1UL << QSPI_RISR_RXOIR_Pos)

/** @brief RISR[4]: Receive FIFO Full Raw Interrupt Status */
#define QSPI_RISR_RXFIR_Pos                 (4U)
#define QSPI_RISR_RXFIR_Msk                 (0x1UL << QSPI_RISR_RXFIR_Pos)

#define QSPI_RISR_RXFIR_INACTIVE            (0x0UL << QSPI_RISR_RXFIR_Pos)
#define QSPI_RISR_RXFIR_ACTIVE              (0x1UL << QSPI_RISR_RXFIR_Pos)

/** @brief RISR[5]: Multi-Master Contention Raw Interrupt Status */
#define QSPI_RISR_MSTIR_Pos                 (5U)
#define QSPI_RISR_MSTIR_Msk                 (0x1UL << QSPI_RISR_MSTIR_Pos)

#define QSPI_RISR_MSTIR_INACTIVE            (0x0UL << QSPI_RISR_MSTIR_Pos)
#define QSPI_RISR_MSTIR_ACTIVE              (0x1UL << QSPI_RISR_MSTIR_Pos)

/*** Bits definition for TXOICR register  *********************************************/

/** @brief TXOICR[0]: Clear Transmit FIFO Overflow Interrupt */
#define QSPI_TXOICR_TXOICR_Pos              (0U)
#define QSPI_TXOICR_TXOICR_Msk              (0x1UL << QSPI_TXOICR_TXOICR_Pos)

/*** Bits definition for RXOICR register  *********************************************/

/** @brief RXOICR[0]: Clear Receive FIFO Overflow Interrupt */
#define QSPI_RXOICR_RXOICR_Pos              (0U)
#define QSPI_RXOICR_RXOICR_Msk              (0x1UL << QSPI_RXOICR_RXOICR_Pos)

/*** Bits definition for RXUICR register  *********************************************/

/** @brief RXUICR[0]: Clear Receive FIFO Underflow Interrupt */
#define QSPI_RXUICR_RXUICR_Pos              (0U)
#define QSPI_RXUICR_RXUICR_Msk              (0x1UL << QSPI_RXUICR_RXUICR_Pos)

/*** Bits definition for MSTICR register  *********************************************/

/** @brief MSTICR[0]: Clear Multi-Master Contention Interrupt */
#define QSPI_MSTICR_MSTICR_Pos              (0U)
#define QSPI_MSTICR_MSTICR_Msk              (0x1UL << QSPI_MSTICR_MSTICR_Pos)

/*** Bits definition for ICR register  ************************************************/

/** @brief ICR[0]: Clear Interrupts */
#define QSPI_ICR_ICR_Pos                    (0U)
#define QSPI_ICR_ICR_Msk                    (0x1UL << QSPI_ICR_ICR_Pos)

/*** Bits definition for DMACR register  **********************************************/

/** @brief DMACR[0]: Receive DMA Enable */
#define QSPI_DMACR_RDMAE_Pos                (0U)
#define QSPI_DMACR_RDMAE_Msk                (0x1UL << QSPI_DMACR_RDMAE_Pos)

#define QSPI_DMACR_RDMAE_DISABLE            (0x0UL << QSPI_DMACR_RDMAE_Pos)
#define QSPI_DMACR_RDMAE_ENABLE             (0x1UL << QSPI_DMACR_RDMAE_Pos)

/** @brief DMACR[1]: Transmit DMA Enable */
#define QSPI_DMACR_TDMAE_Pos                (1U)
#define QSPI_DMACR_TDMAE_Msk                (0x1UL << QSPI_DMACR_TDMAE_Pos)

#define QSPI_DMACR_TDMAE_DISABLE            (0x0UL << QSPI_DMACR_TDMAE_Pos)
#define QSPI_DMACR_TDMAE_ENABLE             (0x1UL << QSPI_DMACR_TDMAE_Pos)

/*** Bits definition for DMATDLR register  ********************************************/

/** @brief DMATDLR[2:0]: Transmit Data Level */
#define QSPI_DMATDLR_DMATDL_Pos             (0U)
#define QSPI_DMATDLR_DMATDL_Msk             (0x7UL << QSPI_DMATDLR_DMATDL_Pos)

/*** Bits definition for DMARDLR register  ********************************************/

/** @brief DMARDLR[2:0]: Receive Data Level */
#define QSPI_DMARDLR_DMARDL_Pos             (0U)
#define QSPI_DMARDLR_DMARDL_Msk             (0x7UL << QSPI_DMARDLR_DMARDL_Pos)

/*** Bits definition for IDR register  ************************************************/

/** @brief IDR[31:0]: Identification code */
#define QSPI_IDR_IDCODE_Pos                 (0U)
#define QSPI_IDR_IDCODE_Msk                 (0xFFFFFFFFUL << QSPI_IDR_IDCODE_Pos)

/*** Bits definition for VER_ID register  *********************************************/

/** @brief VER_ID[31:0]: Hex representation of component version */
#define QSPI_VER_ID_COMP_VER_Pos            (0U)
#define QSPI_VER_ID_COMP_VER_Msk            (0xFFFFFFFFUL << QSPI_VER_ID_COMP_VER_Pos)

/*** Bits definition for DR register  *************************************************/

/** @brief DR[31:0]: Data Register */
#define QSPI_DR_DR_Pos                      (0U)
#define QSPI_DR_DR_Msk                      (0xFFFFFFFFUL << QSPI_DR_DR_Pos)

/*** Bits definition for RX_SAMPLE_DLY registers  *************************************/

/*** It is impossible to write to this register when the QSPI is enabled (QSPIENR) ****/

/** @brief RX_SAMPLE_DLY[7:0]: Rxd Sample Delay */
#define QSPI_RX_SAMPLE_DLY_RSD_Pos          (0U)
#define QSPI_RX_SAMPLE_DLY_RSD_Msk          (0xFFUL << QSPI_RX_SAMPLE_DLY_RSD_Pos)

/*** Bits definition for SPI_CTRLR0 registers  ****************************************/

/*** It is impossible to write to this register when the QSPI is enabled (QSPIENR) ****/

/** @brief SPI_CTRLR0[1:0]: Address and instruction transfer format */
#define QSPI_SPI_CTRLR0_TRANS_TYPE_Pos      (0U)
#define QSPI_SPI_CTRLR0_TRANS_TYPE_Msk      (0x3UL << QSPI_SPI_CTRLR0_TRANS_TYPE_Pos)

#define QSPI_SPI_CTRLR0_TRANS_TYPE_STD      (0x0UL << QSPI_SPI_CTRLR0_TRANS_TYPE_Pos)
#define QSPI_SPI_CTRLR0_TRANS_TYPE_INST     (0x1UL << QSPI_SPI_CTRLR0_TRANS_TYPE_Pos)
#define QSPI_SPI_CTRLR0_TRANS_TYPE_BOTH     (0x2UL << QSPI_SPI_CTRLR0_TRANS_TYPE_Pos)

/** @brief SPI_CTRLR0[5:2]: Address Length */
#define QSPI_SPI_CTRLR0_ADDR_L_Pos          (2U)
#define QSPI_SPI_CTRLR0_ADDR_L_Msk          (0xFUL << QSPI_SPI_CTRLR0_ADDR_L_Pos)

#define QSPI_SPI_CTRLR0_ADDR_L_0            (0x0UL << QSPI_SPI_CTRLR0_ADDR_L_Pos)
#define QSPI_SPI_CTRLR0_ADDR_L_4            (0x1UL << QSPI_SPI_CTRLR0_ADDR_L_Pos)
#define QSPI_SPI_CTRLR0_ADDR_L_8            (0x2UL << QSPI_SPI_CTRLR0_ADDR_L_Pos)
#define QSPI_SPI_CTRLR0_ADDR_L_12           (0x3UL << QSPI_SPI_CTRLR0_ADDR_L_Pos)
#define QSPI_SPI_CTRLR0_ADDR_L_16           (0x4UL << QSPI_SPI_CTRLR0_ADDR_L_Pos)
#define QSPI_SPI_CTRLR0_ADDR_L_20           (0x5UL << QSPI_SPI_CTRLR0_ADDR_L_Pos)
#define QSPI_SPI_CTRLR0_ADDR_L_24           (0x6UL << QSPI_SPI_CTRLR0_ADDR_L_Pos)
#define QSPI_SPI_CTRLR0_ADDR_L_28           (0x7UL << QSPI_SPI_CTRLR0_ADDR_L_Pos)
#define QSPI_SPI_CTRLR0_ADDR_L_32           (0x8UL << QSPI_SPI_CTRLR0_ADDR_L_Pos)
#define QSPI_SPI_CTRLR0_ADDR_L_36           (0x9UL << QSPI_SPI_CTRLR0_ADDR_L_Pos)
#define QSPI_SPI_CTRLR0_ADDR_L_40           (0xAUL << QSPI_SPI_CTRLR0_ADDR_L_Pos)
#define QSPI_SPI_CTRLR0_ADDR_L_44           (0xBUL << QSPI_SPI_CTRLR0_ADDR_L_Pos)
#define QSPI_SPI_CTRLR0_ADDR_L_48           (0xCUL << QSPI_SPI_CTRLR0_ADDR_L_Pos)
#define QSPI_SPI_CTRLR0_ADDR_L_52           (0xDUL << QSPI_SPI_CTRLR0_ADDR_L_Pos)
#define QSPI_SPI_CTRLR0_ADDR_L_56           (0xEUL << QSPI_SPI_CTRLR0_ADDR_L_Pos)
#define QSPI_SPI_CTRLR0_ADDR_L_60           (0xFUL << QSPI_SPI_CTRLR0_ADDR_L_Pos)

/** @brief SPI_CTRLR0[9:8]: Instruction Length */
#define QSPI_SPI_CTRLR0_INST_L_Pos          (8U)
#define QSPI_SPI_CTRLR0_INST_L_Msk          (0x3UL << QSPI_SPI_CTRLR0_INST_L_Pos)

#define QSPI_SPI_CTRLR0_INST_L_0            (0x0UL << QSPI_SPI_CTRLR0_INST_L_Pos)
#define QSPI_SPI_CTRLR0_INST_L_4            (0x1UL << QSPI_SPI_CTRLR0_INST_L_Pos)
#define QSPI_SPI_CTRLR0_INST_L_8            (0x2UL << QSPI_SPI_CTRLR0_INST_L_Pos)
#define QSPI_SPI_CTRLR0_INST_L_16           (0x3UL << QSPI_SPI_CTRLR0_INST_L_Pos)

/** @brief SPI_CTRLR0[15:11]: Wait cycles */
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos     (11U)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_Msk     (0x1FUL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)

#define QSPI_SPI_CTRLR0_WAIT_CYCLES_0       (0x00UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_1       (0x01UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_2       (0x02UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_3       (0x03UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_4       (0x04UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_5       (0x05UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_6       (0x06UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_7       (0x07UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_8       (0x08UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_9       (0x09UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_10      (0x0AUL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_11      (0x0BUL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_12      (0x0CUL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_13      (0x0DUL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_14      (0x0EUL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_15      (0x0FUL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_16      (0x10UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_17      (0x11UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_18      (0x12UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_19      (0x13UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_20      (0x14UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_21      (0x15UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_22      (0x16UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_23      (0x17UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_24      (0x18UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_25      (0x19UL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_26      (0x1AUL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_27      (0x1BUL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_28      (0x1CUL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_29      (0x1DUL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_30      (0x1EUL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)
#define QSPI_SPI_CTRLR0_WAIT_CYCLES_31      (0x1FUL << QSPI_SPI_CTRLR0_WAIT_CYCLES_Pos)

/** @brief SPI_CTRLR0[16]: SPI DDR Enable bit */
#define QSPI_SPI_CTRLR0_SPI_DDR_EN_Pos      (16U)
#define QSPI_SPI_CTRLR0_SPI_DDR_EN_Msk      (0x1UL << QSPI_SPI_CTRLR0_SPI_DDR_EN_Pos)

#define QSPI_SPI_CTRLR0_SPI_DDR_EN_DISABLE  (0x0UL << QSPI_SPI_CTRLR0_SPI_DDR_EN_Pos)
#define QSPI_SPI_CTRLR0_SPI_DDR_EN_ENABLE   (0x1UL << QSPI_SPI_CTRLR0_SPI_DDR_EN_Pos)

/** @brief SPI_CTRLR0[17]: Instruction DDR Enable bit */
#define QSPI_SPI_CTRLR0_INST_DDR_EN_Pos     (17U)
#define QSPI_SPI_CTRLR0_INST_DDR_EN_Msk     (0x1UL << QSPI_SPI_CTRLR0_INST_DDR_EN_Pos)

#define QSPI_SPI_CTRLR0_INST_DDR_EN_DISABLE (0x0UL << QSPI_SPI_CTRLR0_INST_DDR_EN_Pos)
#define QSPI_SPI_CTRLR0_INST_DDR_EN_ENABLE  (0x1UL << QSPI_SPI_CTRLR0_INST_DDR_EN_Pos)

/** @brief SPI_CTRLR0[18]: Read data strobe enable bit */
#define QSPI_SPI_CTRLR0_SPI_RXDS_EN_Pos     (18U)
#define QSPI_SPI_CTRLR0_SPI_RXDS_EN_Msk     (0x1UL << QSPI_SPI_CTRLR0_SPI_RXDS_EN_Pos)

/*** Bits definition for TXD_DRIVE_EDGE registers  ************************************/

/*** It is impossible to write to this register when the QSPI is enabled (QSPIENR) ****/

/** @brief TXD_DRIVE_EDGE[7:0]: TXD Drive edge */
#define QSPI_TXD_DRIVE_EDGE_TDE_Pos         (0U)
#define QSPI_TXD_DRIVE_EDGE_TDE_Msk         (0xFFUL << QSPI_TXD_DRIVE_EDGE_TDE_Pos)

/******************************************************************************/
/*                      Serial Peripheral Interface (SPI)                     */
/******************************************************************************/

/*** Bits definition for CTRLR0 register  *********************************************/

/** @brief CTRLR0[5:4]: Frame Format */
#define SPI_CTRLR0_FRF_Pos                  (4U)
#define SPI_CTRLR0_FRF_Msk                  (0x3UL << SPI_CTRLR0_FRF_Pos)

#define SPI_CTRLR0_FRF_SPI                  (0x0UL << SPI_CTRLR0_FRF_Pos)
#define SPI_CTRLR0_FRF_SSP                  (0x1UL << SPI_CTRLR0_FRF_Pos)
#define SPI_CTRLR0_FRF_MW                   (0x2UL << SPI_CTRLR0_FRF_Pos)

/** @brief CTRLR0[6]: Serial Clock Phase */
#define SPI_CTRLR0_SCPH_Pos                 (6U)
#define SPI_CTRLR0_SCPH_Msk                 (0x1UL << SPI_CTRLR0_SCPH_Pos)

#define SPI_CTRLR0_SCPH_MIDDLE              (0x0UL << SPI_CTRLR0_SCPH_Pos)
#define SPI_CTRLR0_SCPH_START               (0x1UL << SPI_CTRLR0_SCPH_Pos)

/** @brief CTRLR0[7]: Serial Clock Polarity */
#define SPI_CTRLR0_SCPOL_Pos                (7U)
#define SPI_CTRLR0_SCPOL_Msk                (0x1UL << SPI_CTRLR0_SCPOL_Pos)

#define SPI_CTRLR0_SCPOL_LOW                (0x0UL << SPI_CTRLR0_SCPOL_Pos)
#define SPI_CTRLR0_SCPOL_HIGH               (0x1UL << SPI_CTRLR0_SCPOL_Pos)

/** @brief CTRLR0[9:8]: Transfer Mode */
#define SPI_CTRLR0_TMOD_Pos                 (8U)
#define SPI_CTRLR0_TMOD_Msk                 (0x3UL << SPI_CTRLR0_TMOD_Pos)

#define SPI_CTRLR0_TMOD_TX_RX               (0x0UL << SPI_CTRLR0_TMOD_Pos)
#define SPI_CTRLR0_TMOD_TX                  (0x1UL << SPI_CTRLR0_TMOD_Pos)
#define SPI_CTRLR0_TMOD_RX                  (0x2UL << SPI_CTRLR0_TMOD_Pos)
#define SPI_CTRLR0_TMOD_EEPROM_READ         (0x3UL << SPI_CTRLR0_TMOD_Pos)

/** @brief CTRLR0[10]: Slave Output Enable */
#define SPI_CTRLR0_SLV_OE_Pos               (10U)
#define SPI_CTRLR0_SLV_OE_Msk               (0x1UL << SPI_CTRLR0_SLV_OE_Pos)

#define SPI_CTRLR0_SLV_OE_DISABLED          (0x0UL << SPI_CTRLR0_SLV_OE_Pos)
#define SPI_CTRLR0_SLV_OE_ENABLED           (0x1UL << SPI_CTRLR0_SLV_OE_Pos)

/** @brief CTRLR0[11]: Shift Register Loop */
#define SPI_CTRLR0_SRL_Pos                  (11U)
#define SPI_CTRLR0_SRL_Msk                  (0x1UL << SPI_CTRLR0_SRL_Pos)

#define SPI_CTRLR0_SRL_NORMAL               (0x0UL << SPI_CTRLR0_SRL_Pos)
#define SPI_CTRLR0_SRL_TESTING              (0x1UL << SPI_CTRLR0_SRL_Pos)

/** @brief CTRLR0[15:12]: Control Frame Size for Microwire frame format */
#define SPI_CTRLR0_CFS_Pos                  (12U)
#define SPI_CTRLR0_CFS_Msk                  (0xFUL << SPI_CTRLR0_CFS_Pos)

#define SPI_CTRLR0_CFS_01_BIT               (0x0UL << SPI_CTRLR0_CFS_Pos)
#define SPI_CTRLR0_CFS_02_BIT               (0x1UL << SPI_CTRLR0_CFS_Pos)
#define SPI_CTRLR0_CFS_03_BIT               (0x2UL << SPI_CTRLR0_CFS_Pos)
#define SPI_CTRLR0_CFS_04_BIT               (0x3UL << SPI_CTRLR0_CFS_Pos)
#define SPI_CTRLR0_CFS_05_BIT               (0x4UL << SPI_CTRLR0_CFS_Pos)
#define SPI_CTRLR0_CFS_06_BIT               (0x5UL << SPI_CTRLR0_CFS_Pos)
#define SPI_CTRLR0_CFS_07_BIT               (0x6UL << SPI_CTRLR0_CFS_Pos)
#define SPI_CTRLR0_CFS_08_BIT               (0x7UL << SPI_CTRLR0_CFS_Pos)
#define SPI_CTRLR0_CFS_09_BIT               (0x8UL << SPI_CTRLR0_CFS_Pos)
#define SPI_CTRLR0_CFS_10_BIT               (0x9UL << SPI_CTRLR0_CFS_Pos)
#define SPI_CTRLR0_CFS_11_BIT               (0xAUL << SPI_CTRLR0_CFS_Pos)
#define SPI_CTRLR0_CFS_12_BIT               (0xBUL << SPI_CTRLR0_CFS_Pos)
#define SPI_CTRLR0_CFS_13_BIT               (0xCUL << SPI_CTRLR0_CFS_Pos)
#define SPI_CTRLR0_CFS_14_BIT               (0xDUL << SPI_CTRLR0_CFS_Pos)
#define SPI_CTRLR0_CFS_15_BIT               (0xEUL << SPI_CTRLR0_CFS_Pos)
#define SPI_CTRLR0_CFS_16_BIT               (0xFUL << SPI_CTRLR0_CFS_Pos)

/** @brief CTRLR0[20:16]: Data Frame Size */
#define SPI_CTRLR0_DFS_32_Pos               (16U)
#define SPI_CTRLR0_DFS_32_Msk               (0x1FU << SPI_CTRLR0_DFS_32_Pos)

#define SPI_CTRLR0_DFS_32_04_BIT            (0x03UL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_05_BIT            (0x04UL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_06_BIT            (0x05UL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_07_BIT            (0x06UL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_08_BIT            (0x07UL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_09_BIT            (0x08UL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_10_BIT            (0x09UL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_11_BIT            (0x0AUL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_12_BIT            (0x0BUL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_13_BIT            (0x0CUL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_14_BIT            (0x0DUL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_15_BIT            (0x0EUL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_16_BIT            (0x0FUL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_17_BIT            (0x10UL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_18_BIT            (0x11UL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_19_BIT            (0x12UL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_20_BIT            (0x13UL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_21_BIT            (0x14UL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_22_BIT            (0x15UL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_23_BIT            (0x16UL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_24_BIT            (0x17UL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_25_BIT            (0x18UL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_26_BIT            (0x19UL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_27_BIT            (0x1AUL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_28_BIT            (0x1BUL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_29_BIT            (0x1CUL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_30_BIT            (0x1DUL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_31_BIT            (0x1EUL << SPI_CTRLR0_DFS_32_Pos)
#define SPI_CTRLR0_DFS_32_32_BIT            (0x1FUL << SPI_CTRLR0_DFS_32_Pos)

/** @brief CTRLR0[22:21]: SPI Frame Format */
#define SPI_CTRLR0_SPI_FRF_Pos              (21U)
#define SPI_CTRLR0_SPI_FRF_Msk              (0x3UL << SPI_CTRLR0_SPI_FRF_Pos)

#define SPI_CTRLR0_SPI_FRF_STD              (0x0UL << SPI_CTRLR0_SPI_FRF_Pos)
#define SPI_CTRLR0_SPI_FRF_DUAL             (0x1UL << SPI_CTRLR0_SPI_FRF_Pos)
#define SPI_CTRLR0_SPI_FRF_QUAD             (0x2UL << SPI_CTRLR0_SPI_FRF_Pos)
#define SPI_CTRLR0_SPI_FRF_OCTAL            (0x3UL << SPI_CTRLR0_SPI_FRF_Pos)

/** @brief CTRLR0[24]: Slave Select Toggle Enable */
#define SPI_CTRLR0_SSTE_Pos                 (24U)
#define SPI_CTRLR0_SSTE_Msk                 (0x1UL << SPI_CTRLR0_SSTE_Pos)

#define SPI_CTRLR0_SSTE_DISABLED            (0x0UL << SPI_CTRLR0_SSTE_Pos)
#define SPI_CTRLR0_SSTE_ENABLED             (0x1UL << SPI_CTRLR0_SSTE_Pos)

/** @brief CTRLR0[25]: Set the Endianness for data register reads */
#define SPI_CTRLR0_SECONV_Pos               (25U)
#define SPI_CTRLR0_SECONV_Msk               (0x1UL << SPI_CTRLR0_SECONV_Pos)

#define SPI_CTRLR0_SECONV_DISABLED          (0x0UL << SPI_CTRLR0_SECONV_Pos)
#define SPI_CTRLR0_SECONV_ENABLED           (0x1UL << SPI_CTRLR0_SECONV_Pos)

/*** Bits definition for CTRLR1 register  *********************************************/

/*  Valid only for SPI Master instances.
    It is impossible to write to this register when the SPI is enabled */

/** @brief CTRLR1[15:0]: Number of Data Frames */
#define SPI_CTRLR1_NDF_Pos                  (0U)
#define SPI_CTRLR1_NDF_Msk                  (0xFFFFUL << SPI_CTRLR1_NDF_Pos)

/*** Bits definition for SPIENR register  *********************************************/

/** @brief SPIENR[0]: SPI Enable */
#define SPI_SPIENR_SPI_EN_Pos               (0U)
#define SPI_SPIENR_SPI_EN_Msk               (0x1UL << SPI_SPIENR_SPI_EN_Pos)

#define SPI_SPIENR_SPI_EN_DISABLED          (0x0UL << SPI_SPIENR_SPI_EN_Pos)
#define SPI_SPIENR_SPI_EN_ENABLED           (0x1UL << SPI_SPIENR_SPI_EN_Pos)

/*** Bits definition for MWCR register  ***********************************************/

/*  Control the direction of the data word for the half-duplex Microwire serial
    protocol. It is impossible to write to this register when
    the SPI is enabled. */

/** @brief MWCR[0]: Microwire Transfer Mode */
#define SPI_MWCR_MWMOD_Pos                  (0U)
#define SPI_MWCR_MWMOD_Msk                  (0x1UL << SPI_MWCR_MWMOD_Pos)

#define SPI_MWCR_MWMOD_NON_SEQUENTIAL       (0x0UL << SPI_MWCR_MWMOD_Pos)
#define SPI_MWCR_MWMOD_SEQUENTIAL           (0x1UL << SPI_MWCR_MWMOD_Pos)

/** @brief MWCR[1]: Microwire Control */
#define SPI_MWCR_MDD_Pos                    (1U)
#define SPI_MWCR_MDD_Msk                    (0x1UL << SPI_MWCR_MDD_Pos)

#define SPI_MWCR_MDD_RECEIVE                (0x0UL << SPI_MWCR_MDD_Pos)
#define SPI_MWCR_MDD_TRANSMIT               (0x1UL << SPI_MWCR_MDD_Pos)

/** @brief MWCR[2]: Microwire Handshaking */
#define SPI_MWCR_MHS_Pos                    (2U)
#define SPI_MWCR_MHS_Msk                    (0x1UL << SPI_MWCR_MHS_Pos)

#define SPI_MWCR_MHS_DISABLE                (0x0UL << SPI_MWCR_MHS_Pos)
#define SPI_MWCR_MHS_ENABLE                 (0x1UL << SPI_MWCR_MHS_Pos)

/*** Bits definition for SER register  ************************************************/

/*  Valid only for SPI Master instances.
    Enables the individual slave select output line from the SPI Master. */

/** @brief SER[0]: Slave Select Enable Flag */
#define SPI_SER_SER_Pos                     (0U)
#define SPI_SER_SER_Msk                     (0x7UL << SPI_SER_SER_Pos)

#define SPI_SER_SER_NOT_SELECTED            (0x0UL << SPI_SER_SER_Pos)
#define SPI_SER_SER_SELECTED                (0x1UL << SPI_SER_SER_Pos)

/*** Bits definition for BAUDR register  **********************************************/

/*  Valid only for SPI Master instances.
    Derives the frequency of the serial clock that regulates the data transfer.
    The 16-bit field in this register defines the clock divider value.
    It is impossible to write to this register when the SPI is enabled. */

/** @brief BAUDR[15:0]: SSI Clock Divider */
#define SPI_BAUDR_SCKDV_Pos                 (0U)
#define SPI_BAUDR_SCKDV_Msk                 (0xFFFFUL << SPI_BAUDR_SCKDV_Pos)

/*** Bits definition for TXFTLR register  ************************************************/

/** @brief TXFTLR[5:0]: Transmit FIFO Threshold */
#define SPI_TXFTLR_TFT_Pos                  (0U)
#define SPI_TXFTLR_TFT_Msk                  (0x3FUL << SPI_TXFTLR_TFT_Pos)

/*** Bits definition for RXFTLR register  ************************************************/

/** @brief RXFTLR[2:0]: Receive FIFO Threshold */
#define SPI_RXFTLR_RFT_Pos                  (0U)
#define SPI_RXFTLR_RFT_Msk                  (0x7UL << SPI_RXFTLR_RFT_Pos)

/*** Bits definition for TXFLR register  ************************************************/

/** @brief TXFLR[5:0]: Transmit FIFO Level */
#define SPI_TXFLR_TXTFL_Pos                 (0U)
#define SPI_TXFLR_TXTFL_Msk                 (0x3FUL << SPI_TXFLR_TXTFL_Pos)

/*** Bits definition for RXFLR register  ************************************************/

/** @brief RXFLR[2:0]: Receive FIFO Level */
#define SPI_RXFLR_RXTFL_Pos                 (0U)
#define SPI_RXFLR_RXTFL_Msk                 (0x7UL << SPI_RXFLR_RXTFL_Pos)

/*** Bits definition for SR register  ************************************************/

/** @brief SR[0]: SPI Busy Flag */
#define SPI_SR_BUSY_Pos                     (0U)
#define SPI_SR_BUSY_Msk                     (0x1UL << SPI_SR_BUSY_Pos)

#define SPI_SR_BUSY_INACTIVE                (0x0UL << SPI_SR_BUSY_Pos)
#define SPI_SR_BUSY_ACTIVE                  (0x1UL << SPI_SR_BUSY_Pos)

/** @brief SR[1]: Transmit FIFO Not Full */
#define SPI_SR_TFNF_Pos                     (1U)
#define SPI_SR_TFNF_Msk                     (0x1UL << SPI_SR_TFNF_Pos)

#define SPI_SR_TFNF_FULL                    (0x0UL << SPI_SR_TFNF_Pos)
#define SPI_SR_TFNF_NON_FULL                (0x1UL << SPI_SR_TFNF_Pos)

/** @brief SR[2]: Transmit FIFO Empty */
#define SPI_SR_TFE_Pos                      (2U)
#define SPI_SR_TFE_Msk                      (0x1UL << SPI_SR_TFE_Pos)

#define SPI_SR_TFE_NOT_EMPTY                (0x0UL << SPI_SR_TFE_Pos)
#define SPI_SR_TFE_EMPTY                    (0x1UL << SPI_SR_TFE_Pos)

/** @brief SR[3]: Receive FIFO Not Empty */
#define SPI_SR_RFNE_Pos                     (3U)
#define SPI_SR_RFNE_Msk                     (0x1UL << SPI_SR_RFNE_Pos)

#define SPI_SR_RFNE_EMPTY                   (0x0UL << SPI_SR_RFNE_Pos)
#define SPI_SR_RFNE_NOT_EMPTY               (0x1UL << SPI_SR_RFNE_Pos)

/** @brief SR[4]: Receive FIFO Full */
#define SPI_SR_RFF_Pos                      (4U)
#define SPI_SR_RFF_Msk                      (0x1UL << SPI_SR_RFF_Pos)

#define SPI_SR_RFF_NOT_FULL                 (0x0UL << SPI_SR_RFF_Pos)
#define SPI_SR_RFF_FULL                     (0x1UL << SPI_SR_RFF_Pos)

/** @brief SR[5]: Transmission Error */
#define SPI_SR_TXE_Pos                      (5U)
#define SPI_SR_TXE_Msk                      (0x1UL << SPI_SR_TXE_Pos)

#define SPI_SR_TXE_NO_ERROR                 (0x0UL << SPI_SR_TXE_Pos)
#define SPI_SR_TXE_TX_ERROR                 (0x1UL << SPI_SR_TXE_Pos)

/** @brief SR[6]: Data Collision Error */
#define SPI_SR_DCOL_Pos                     (6U)
#define SPI_SR_DCOL_Msk                     (0x1UL << SPI_SR_DCOL_Pos)

#define SPI_SR_DCOL_NO_ERROR_CONDITION      (0x0UL << SPI_SR_DCOL_Pos)
#define SPI_SR_DCOL_TX_COLLISION_ERROR      (0x1UL << SPI_SR_DCOL_Pos)

/*** Bits definition for IMR register  ***********************************************/

/** @brief IMR[0]: Transmit FIFO Empty Interrupt Mask */
#define SPI_IMR_TXEIM_Pos                   (0U)
#define SPI_IMR_TXEIM_Msk                   (0x1UL << SPI_IMR_TXEIM_Pos)

#define SPI_IMR_TXEIM_MASKED                (0x0UL << SPI_IMR_TXEIM_Pos)
#define SPI_IMR_TXEIM_UNMASKED              (0x1UL << SPI_IMR_TXEIM_Pos)

/** @brief IMR[1]: Transmit FIFO Overflow Interrupt Mask */
#define SPI_IMR_TXOIM_Pos                   (1U)
#define SPI_IMR_TXOIM_Msk                   (0x1UL << SPI_IMR_TXOIM_Pos)

#define SPI_IMR_TXOIM_MASKED                (0x0UL << SPI_IMR_TXOIM_Pos)
#define SPI_IMR_TXOIM_UNMASKED              (0x1UL << SPI_IMR_TXOIM_Pos)

/** @brief IMR[2]: Receive FIFO Underflow Interrupt Mask */
#define SPI_IMR_RXUIM_Pos                   (2U)
#define SPI_IMR_RXUIM_Msk                   (0x1UL << SPI_IMR_RXUIM_Pos)

#define SPI_IMR_RXUIM_MASKED                (0x0UL << SPI_IMR_RXUIM_Pos)
#define SPI_IMR_RXUIM_UNMASKED              (0x1UL << SPI_IMR_RXUIM_Pos)

/** @brief IMR[3]: Receive FIFO Overflow Interrupt Mask */
#define SPI_IMR_RXOIM_Pos                   (3U)
#define SPI_IMR_RXOIM_Msk                   (0x1UL << SPI_IMR_RXOIM_Pos)

#define SPI_IMR_RXOIM_MASKED                (0x0UL << SPI_IMR_RXOIM_Pos)
#define SPI_IMR_RXOIM_UNMASKED              (0x1UL << SPI_IMR_RXOIM_Pos)

/** @brief IMR[4]: Receive FIFO Full Interrupt Mask */
#define SPI_IMR_RXFIM_Pos                   (4U)
#define SPI_IMR_RXFIM_Msk                   (0x1UL << SPI_IMR_RXFIM_Pos)

#define SPI_IMR_RXFIM_MASKED                (0x0UL << SPI_IMR_RXFIM_Pos)
#define SPI_IMR_RXFIM_UNMASKED              (0x1UL << SPI_IMR_RXFIM_Pos)

/** @brief IMR[5]: Multi-Master Contention Interrupt Mask */
#define SPI_IMR_MSTIM_Pos                   (5U)
#define SPI_IMR_MSTIM_Msk                   (0x1UL << SPI_IMR_MSTIM_Pos)

#define SPI_IMR_MSTIM_MASKED                (0x0UL << SPI_IMR_MSTIM_Pos)
#define SPI_IMR_MSTIM_UNMASKED              (0x1UL << SPI_IMR_MSTIM_Pos)

/*** Bits definition for ISR register  ***********************************************/

/** @brief ISR[0]: Transmit FIFO Empty Interrupt Status */
#define SPI_ISR_TXEIS_Pos                   (0U)
#define SPI_ISR_TXEIS_Msk                   (0x1UL << SPI_ISR_TXEIS_Pos)

#define SPI_ISR_TXEIS_INACTIVE              (0x0UL << SPI_ISR_TXEIS_Pos)
#define SPI_ISR_TXEIS_ACTIVE                (0x1UL << SPI_ISR_TXEIS_Pos)

/** @brief ISR[1]: Transmit FIFO Overflow Interrupt Status */
#define SPI_ISR_TXOIS_Pos                   (1U)
#define SPI_ISR_TXOIS_Msk                   (0x1UL << SPI_ISR_TXOIS_Pos)

#define SPI_ISR_TXOIS_INACTIVE              (0x0UL << SPI_ISR_TXOIS_Pos)
#define SPI_ISR_TXOIS_ACTIVE                (0x1UL << SPI_ISR_TXOIS_Pos)

/** @brief ISR[2]: Receive FIFO Underflow Interrupt Status */
#define SPI_ISR_RXUIS_Pos                   (2U)
#define SPI_ISR_RXUIS_Msk                   (0x1UL << SPI_ISR_RXUIS_Pos)

#define SPI_ISR_RXUIS_INACTIVE              (0x0UL << SPI_ISR_RXUIS_Pos)
#define SPI_ISR_RXUIS_ACTIVE                (0x1UL << SPI_ISR_RXUIS_Pos)

/** @brief ISR[3]: Receive FIFO Overflow Interrupt Status */
#define SPI_ISR_RXOIS_Pos                   (3U)
#define SPI_ISR_RXOIS_Msk                   (0x1UL << SPI_ISR_RXOIS_Pos)

#define SPI_ISR_RXOIS_INACTIVE              (0x0UL << SPI_ISR_RXOIS_Pos)
#define SPI_ISR_RXOIS_ACTIVE                (0x1UL << SPI_ISR_RXOIS_Pos)

/** @brief ISR[4]: Receive FIFO Full Interrupt Status */
#define SPI_ISR_RXFIS_Pos                   (4U)
#define SPI_ISR_RXFIS_Msk                   (0x1UL << SPI_ISR_RXFIS_Pos)

#define SPI_ISR_RXFIS_INACTIVE              (0x0UL << SPI_ISR_RXFIS_Pos)
#define SPI_ISR_RXFIS_ACTIVE                (0x1UL << SPI_ISR_RXFIS_Pos)

/** @brief ISR[5]: Multi-Master Contention Interrupt Status */
#define SPI_ISR_MSTIS_Pos                   (5U)
#define SPI_ISR_MSTIS_Msk                   (0x1UL << SPI_ISR_MSTIS_Pos)

#define SPI_ISR_MSTIS_INACTIVE              (0x0UL << SPI_ISR_MSTIS_Pos)
#define SPI_ISR_MSTIS_ACTIVE                (0x1UL << SPI_ISR_MSTIS_Pos)

/*** Bits definition for RISR register  **********************************************/

/** @brief RISR[0]: Transmit FIFO Empty Raw Interrupt Status */
#define SPI_RISR_TXEIR_Pos                  (0U)
#define SPI_RISR_TXEIR_Msk                  (0x1UL << SPI_RISR_TXEIR_Pos)

#define SPI_RISR_TXEIR_INACTIVE             (0x0UL << SPI_RISR_TXEIR_Pos)
#define SPI_RISR_TXEIR_ACTIVE               (0x1UL << SPI_RISR_TXEIR_Pos)

/** @brief RISR[1]: Transmit FIFO Overflow Raw Interrupt Status */
#define SPI_RISR_TXOIR_Pos                  (1U)
#define SPI_RISR_TXOIR_Msk                  (0x1UL << SPI_RISR_TXOIR_Pos)

#define SPI_RISR_TXOIR_INACTIVE             (0x0UL << SPI_RISR_TXOIR_Pos)
#define SPI_RISR_TXOIR_ACTIVE               (0x1UL << SPI_RISR_TXOIR_Pos)

/** @brief RISR[2]: Receive FIFO Underflow Raw Interrupt Status */
#define SPI_RISR_RXUIR_Pos                  (2U)
#define SPI_RISR_RXUIR_Msk                  (0x1UL << SPI_RISR_RXUIR_Pos)

#define SPI_RISR_RXUIR_INACTIVE             (0x0UL << SPI_RISR_RXUIR_Pos)
#define SPI_RISR_RXUIR_ACTIVE               (0x1UL << SPI_RISR_RXUIR_Pos)

/** @brief RISR[3]: Receive FIFO Overflow Raw Interrupt Status */
#define SPI_RISR_RXOIR_Pos                  (3U)
#define SPI_RISR_RXOIR_Msk                  (0x1UL << SPI_RISR_RXOIR_Pos)

#define SPI_RISR_RXOIR_INACTIVE             (0x0UL << SPI_RISR_RXOIR_Pos)
#define SPI_RISR_RXOIR_ACTIVE               (0x1UL << SPI_RISR_RXOIR_Pos)

/** @brief RISR[4]: Receive FIFO Full Raw Interrupt Status */
#define SPI_RISR_RXFIR_Pos                  (4U)
#define SPI_RISR_RXFIR_Msk                  (0x1UL << SPI_RISR_RXFIR_Pos)

#define SPI_RISR_RXFIR_INACTIVE             (0x0UL << SPI_RISR_RXFIR_Pos)
#define SPI_RISR_RXFIR_ACTIVE               (0x1UL << SPI_RISR_RXFIR_Pos)

/** @brief RISR[5]: Multi-Master Contention Raw Interrupt Status */
#define SPI_RISR_MSTIR_Pos                  (5U)
#define SPI_RISR_MSTIR_Msk                  (0x1UL << SPI_RISR_MSTIR_Pos)

#define SPI_RISR_MSTIR_INACTIVE             (0x0UL << SPI_RISR_MSTIR_Pos)
#define SPI_RISR_MSTIR_ACTIVE               (0x1UL << SPI_RISR_MSTIR_Pos)

/*** Bits definition for TXOICR register  *********************************************/

/** @brief TXOICR[0]: Clear Transmit FIFO Overflow Interrupt */
#define SPI_TXOICR_TXOICR_Pos               (0U)
#define SPI_TXOICR_TXOICR_Msk               (0x1UL << SPI_TXOICR_TXOICR_Pos)

/*** Bits definition for RXOICR register  *********************************************/

/** @brief RXOICR[0]: Clear Receive FIFO Overflow Interrupt */
#define SPI_RXOICR_RXOICR_Pos               (0U)
#define SPI_RXOICR_RXOICR_Msk               (0x1UL << SPI_RXOICR_RXOICR_Pos)

/*** Bits definition for RXUICR register  *********************************************/

/** @brief RXUICR[0]: Clear Receive FIFO Underflow Interrupt */
#define SPI_RXUICR_RXUICR_Pos               (0U)
#define SPI_RXUICR_RXUICR_Msk               (0x1UL << SPI_RXUICR_RXUICR_Pos)

/*** Bits definition for MSTICR register  *********************************************/

/** @brief MSTICR[0]: Clear Multi-Master Contention Interrupt */
#define SPI_MSTICR_MSTICR_Pos               (0U)
#define SPI_MSTICR_MSTICR_Msk               (0x1UL << SPI_MSTICR_MSTICR_Pos)

/*** Bits definition for ICR register  ************************************************/

/** @brief ICR[0]: Clear Interrupts */
#define SPI_ICR_ICR_Pos                     (0U)
#define SPI_ICR_ICR_Msk                     (0x1UL << SPI_ICR_ICR_Pos)

/*** Bits definition for DMACR register  ************************************************/

/** @brief DMACR[0]: Receive DMA Enable */
#define SPI_DMACR_RDMAE_Pos                 (0U)
#define SPI_DMACR_RDMAE_Msk                 (0x1UL << SPI_DMACR_RDMAE_Pos)

#define SPI_DMACR_RDMAE_DISABLE             (0x0UL << SPI_DMACR_RDMAE_Pos)
#define SPI_DMACR_RDMAE_ENABLE              (0x1UL << SPI_DMACR_RDMAE_Pos)

/** @brief DMACR[1]: Transmit DMA Enable */
#define SPI_DMACR_TDMAE_Pos                 (1U)
#define SPI_DMACR_TDMAE_Msk                 (0x1UL << SPI_DMACR_TDMAE_Pos)

#define SPI_DMACR_TDMAE_DISABLE             (0x0UL << SPI_DMACR_TDMAE_Pos)
#define SPI_DMACR_TDMAE_ENABLE              (0x1UL << SPI_DMACR_TDMAE_Pos)

/*** Bits definition for DMATDLR register  ********************************************/

/** @brief DMATDLR[2:0]: Transmit Data Level */
#define SPI_DMATDLR_DMATDL_Pos              (0U)
#define SPI_DMATDLR_DMATDL_Msk              (0x3FUL << SPI_DMATDLR_DMATDL_Pos)

/*** Bits definition for DMARDLR register  ********************************************/

/** @brief DMARDLR[2:0]: Receive Data Level */
#define SPI_DMARDLR_DMARDL_Pos              (0U)
#define SPI_DMARDLR_DMARDL_Msk              (0x7UL << SPI_DMARDLR_DMARDL_Pos)

/*** Bits definition for IDR register  ************************************************/

/** @brief IDR[31:0]: Identification code */
#define SPI_IDR_IDCODE_Pos                  (0U)
#define SPI_IDR_IDCODE_Msk                  (0xFFFFFFFFUL << SPI_IDR_IDCODE_Pos)

/*** Bits definition for VERSION_ID register  ************************************************/

/** @brief VERSION_ID[31:0]: Hex representation of component version */
#define SPI_VERSION_ID_COMP_VERSION_Pos     (0U)
#define SPI_VERSION_ID_COMP_VERSION_Msk     (0xFFFFFFFFUL << SPI_VERSION_ID_COMP_VERSION_Pos)

/*** Bits definition for DR register  *************************************************/

/** @brief DR[31:0]: Data Register */
#define SPI_DR_DR_Pos                       (0U)
#define SPI_DR_DR_Msk                       (0xFFFFFFFFUL << SPI_DR_DR_Pos)

/*** Bits definition for RX_SAMPLE_DLY register  **************************************/

/*  Control the number of SPI clock cycles that are delayed (from the default
    sample time) before the actual sample of the Rx input occurs.
    It is impossible to write to this register when the SPI is enabled. */

/** @brief RX_SAMPLE_DLY[7:0]: Rxd Sample Delay */
#define SPI_RX_SAMPLE_DLY_RSD_Pos           (0U)
#define SPI_RX_SAMPLE_DLY_RSD_Msk           (0xFFUL << SPI_RX_SAMPLE_DLY_RSD_Pos)

/*** Bits definition for TXD_DRIVE_EDGE register  **************************************/

/*  Control the driving edge of TXD register in DDR mode.
    It is not possible to write to this register when the SPI is enabled. */

/** @brief TXD_DRIVE_EDGE[7:0]: TXD Drive edge */
#define SPI_TXD_DRIVE_EDGE_TDE_Pos          (0U)
#define SPI_TXD_DRIVE_EDGE_TDE_Msk          (0xFFUL << SPI_TXD_DRIVE_EDGE_TDE_Pos)

/******************************************************************************/
/*                Universal Asynchronous Receiver/Transmitter                 */
/******************************************************************************/

/*******************  Bit definition for UART_RBR register  *******************/

/** @brief RBR[7:0]: Received data (8-bit mode) */
#define UART_RBR_8BIT_Pos                   (0U)
#define UART_RBR_8BIT_Msk                   (0xFFUL << UART_RBR_8BIT_Pos)
#define UART_RBR_8BIT                       UART_RBR_8BIT_Msk

/** @brief RBR[8:0]: Received data (9-bit mode) */
#define UART_RBR_9BIT_Pos                   (0U)
#define UART_RBR_9BIT_Msk                   (0x1FFUL << UART_RBR_9BIT_Pos)
#define UART_RBR_9BIT                       UART_RBR_9BIT_Msk

/*******************  Bit definition for UART_THR register  *******************/

/** @brief THR[7:0]: Data to be transmitted (8-bit mode) */
#define UART_THR_8BIT_Pos                   (0U)
#define UART_THR_8BIT_Msk                   (0xFFUL << UART_THR_8BIT_Pos)
#define UART_THR_8BIT                       UART_THR_8BIT_Msk

/** @brief THR[8:0]: Data to be transmitted (9-bit mode) */
#define UART_THR_9BIT_Pos                   (0U)
#define UART_THR_9BIT_Msk                   (0x1FFUL << UART_THR_9BIT_Pos)
#define UART_THR_9BIT                       UART_THR_9BIT_Msk

/*******************  Bit definition for UART_DLL register  *******************/

/** @brief DLL[7:0]: Lower 8 bits of the baudrate divisor */
#define UART_DLL_Pos                        (0U)
#define UART_DLL_Msk                        (0xFFUL << UART_DLL_Pos)
#define UART_DLL                            UART_DLL_Msk

/*******************  Bit definition for UART_IER register  *******************/

/** @brief IER[0]: Enable received data available interrupt */
#define UART_IER_ERBFI_Pos                  (0U)
#define UART_IER_ERBFI_Msk                  (0x1UL << UART_IER_ERBFI_Pos)
#define UART_IER_ERBFI                      UART_IER_ERBFI_Msk

#define UART_IER_ERBFI_DISABLE              (0x0UL << UART_IER_ERBFI_Pos)   /*!< Disable Received Data Available interrupt */
#define UART_IER_ERBFI_ENABLE               (0x1UL << UART_IER_ERBFI_Pos)   /*!< Enable Received Data Available interrupt */

/** @brief IER[1]: Enable transmit holding register empty interrupt */
#define UART_IER_ETBEI_Pos                  (1U)
#define UART_IER_ETBEI_Msk                  (0x1UL << UART_IER_ETBEI_Pos)
#define UART_IER_ETBEI                      UART_IER_ETBEI_Msk

#define UART_IER_ETBEI_DISABLE              (0x0UL << UART_IER_ETBEI_Pos)   /*!< Disable Transmit Holding Register Empty interrupt */
#define UART_IER_ETBEI_ENABLE               (0x1UL << UART_IER_ETBEI_Pos)   /*!< Enable Transmit Holding Register Empty interrupt */

/** @brief IER[2]: Enable receiver line status interrupt */
#define UART_IER_ELSI_Pos                   (2U)
#define UART_IER_ELSI_Msk                   (0x1UL << UART_IER_ELSI_Pos)
#define UART_IER_ELSI                       UART_IER_ELSI_Msk

#define UART_IER_ELSI_DISABLE               (0x0UL << UART_IER_ELSI_Pos)    /*!< Disable Receiver Line Status interrupt */
#define UART_IER_ELSI_ENABLE                (0x1UL << UART_IER_ELSI_Pos)    /*!< Enable Receiver Line Status interrupt */

/** @brief IER[3]: Enable modem status interrupt */
#define UART_IER_EDSSI_Pos                  (3U)
#define UART_IER_EDSSI_Msk                  (0x1UL << UART_IER_EDSSI_Pos)
#define UART_IER_EDSSI                      UART_IER_EDSSI_Msk

#define UART_IER_EDSSI_DISABLE              (0x0UL << UART_IER_EDSSI_Pos)   /*!< Disable Modem Status interrupt */
#define UART_IER_EDSSI_ENABLE               (0x1UL << UART_IER_EDSSI_Pos)   /*!< Enable Modem Status interrupt */

/** @brief IER[4]: The method for clearing the status in the LSR register */
#define UART_IER_ELCOLR_Pos                 (4U)
#define UART_IER_ELCOLR_Msk                 (0x1UL << UART_IER_ELCOLR_Pos)
#define UART_IER_ELCOLR                     UART_IER_ELCOLR_Msk

#define UART_IER_ELCOLR_DISABLE             (0x0UL << UART_IER_ELCOLR_Pos)  /*!< LSR status bits are cleared either on reading Rx FIFO (RBR read) or on reading LSR register */
#define UART_IER_ELCOLR_ENABLE              (0x1UL << UART_IER_ELCOLR_Pos)  /*!< LSR status bits are cleared only on reading LSR register */

/** @brief IER[7]: Programmable THRE interrupt mode enable */
#define UART_IER_PTIME_Pos                  (7U)
#define UART_IER_PTIME_Msk                  (0x1UL << UART_IER_PTIME_Pos)
#define UART_IER_PTIME                      UART_IER_PTIME_Msk

#define UART_IER_PTIME_DISABLE              (0x0UL << UART_IER_PTIME_Pos)   /*!< Disable Programmable THRE Interrupt Mode */
#define UART_IER_PTIME_ENABLE               (0x1UL << UART_IER_PTIME_Pos)   /*!< Enable Programmable THRE Interrupt Mode */

/*******************  Bit definition for UART_DLH register  *******************/

/** @brief DLH[7:0]: Upper 8 bits of the baudrate divisor */
#define UART_DLH_Pos                        (0U)
#define UART_DLH_Msk                        (0xFFUL << UART_DLH_Pos)
#define UART_DLH                            UART_DLH_Msk

/*******************  Bit definition for UART_IIR register  *******************/

/** @brief IIR[3:0]: Interrupt ID */
#define UART_IIR_IID_Pos                    (0U)
#define UART_IIR_IID_Msk                    (0xFUL << UART_IIR_IID_Pos)
#define UART_IIR_IID                        UART_IIR_IID_Msk

#define UART_IIR_IID_MODEM_STATUS           (0x0UL << UART_IIR_IID_Pos)     /*!< Modem status */
#define UART_IIR_IID_NO_INT_PENDING         (0x1UL << UART_IIR_IID_Pos)     /*!< No interrupt pending */
#define UART_IIR_IID_THR_EMPTY              (0x2UL << UART_IIR_IID_Pos)     /*!< THR empty */
#define UART_IIR_IID_RX_DATA_AVAILABLE      (0x4UL << UART_IIR_IID_Pos)     /*!< Received data available */
#define UART_IIR_IID_RX_LINE_STATUS         (0x6UL << UART_IIR_IID_Pos)     /*!< Receiver line status */
#define UART_IIR_IID_BUSY_DETECT            (0x7UL << UART_IIR_IID_Pos)     /*!< Busy detect */
#define UART_IIR_IID_CHAR_TIMEOUT           (0xCUL << UART_IIR_IID_Pos)     /*!< Character timeout */

/** @brief IIR[7:6]: FIFOs enabled */
#define UART_IIR_FIFOSE_Pos                 (6U)
#define UART_IIR_FIFOSE_Msk                 (0x3UL << UART_IIR_FIFOSE_Pos)
#define UART_IIR_FIFOSE                     UART_IIR_FIFOSE_Msk

#define UART_IIR_FIFOSE_DISABLE             (0x0UL << UART_IIR_FIFOSE_Pos)  /*!< FIFOs are disabled */
#define UART_IIR_FIFOSE_ENABLE              (0x3UL << UART_IIR_FIFOSE_Pos)  /*!< FIFOs are enabled */

/*******************  Bit definition for UART_FCR register  *******************/

/** @brief FCR[0]: FIFO enable */
#define UART_FCR_FIFOE_Pos                  (0U)
#define UART_FCR_FIFOE_Msk                  (0x1UL << UART_FCR_FIFOE_Pos)
#define UART_FCR_FIFOE                      UART_FCR_FIFOE_Msk

#define UART_FCR_FIFOE_DISABLE              (0x0UL << UART_FCR_FIFOE_Pos)   /*!< FIFO disabled */
#define UART_FCR_FIFOE_ENABLE               (0x1UL << UART_FCR_FIFOE_Pos)   /*!< FIFO enabled */

/** @brief FCR[1]: RX FIFO reset */
#define UART_FCR_RFIFOR_Pos                 (1U)
#define UART_FCR_RFIFOR_Msk                 (0x1UL << UART_FCR_RFIFOR_Pos)
#define UART_FCR_RFIFOR                     UART_FCR_RFIFOR_Msk
/** @brief FCR[2]: TX FIFO reset */
#define UART_FCR_XFIFOR_Pos                 (2U)
#define UART_FCR_XFIFOR_Msk                 (0x1UL << UART_FCR_XFIFOR_Pos)
#define UART_FCR_XFIFOR                     UART_FCR_XFIFOR_Msk

/** @brief FCR[3]: DMA mode */
#define UART_FCR_DMAM_Pos                   (3U)
#define UART_FCR_DMAM_Msk                   (0x1UL << UART_FCR_DMAM_Pos)
#define UART_FCR_DMAM                       UART_FCR_DMAM_Msk

#define UART_FCR_DMAM_MODE0                 (0x0UL << UART_FCR_DMAM_Pos)    /*!< DMA Mode 0 */
#define UART_FCR_DMAM_MODE1                 (0x1UL << UART_FCR_DMAM_Pos)    /*!< DMA Mode 1 */

/** @brief FCR[5:4]: TX FIFO empty trigger */
#define UART_FCR_TET_Pos                    (4U)
#define UART_FCR_TET_Msk                    (0x3UL << UART_FCR_TET_Pos)
#define UART_FCR_TET                        UART_FCR_TET_Msk

#define UART_FCR_TET_FIFO_EMPTY             (0x0UL << UART_FCR_TET_Pos)     /*!< FIFO empty */
#define UART_FCR_TET_FIFO_CHAR_2            (0x1UL << UART_FCR_TET_Pos)     /*!< 2 characters in FIFO */
#define UART_FCR_TET_FIFO_QUARTER_FULL      (0x2UL << UART_FCR_TET_Pos)     /*!< FIFO 1/4 full */
#define UART_FCR_TET_FIFO_HALF_FULL         (0x3UL << UART_FCR_TET_Pos)     /*!< FIFO 1/2 full */

/** @brief FCR[7:6]: RX FIFO trigger */
#define UART_FCR_RT_Pos                     (6U)
#define UART_FCR_RT_Msk                     (0x3UL << UART_FCR_RT_Pos)
#define UART_FCR_RT                         UART_FCR_RT_Msk

#define UART_FCR_RT_FIFO_CHAR_1             (0x0UL << UART_FCR_RT_Pos)      /*!< 1 character in FIFO */
#define UART_FCR_RT_FIFO_QUARTER_FULL       (0x1UL << UART_FCR_RT_Pos)      /*!< FIFO 1/4 full */
#define UART_FCR_RT_FIFO_HALF_FULL          (0x2UL << UART_FCR_RT_Pos)      /*!< FIFO 1/2 full */
#define UART_FCR_RT_FIFO_FULL_2             (0x3UL << UART_FCR_RT_Pos)      /*!< FIFO 2 less than full */

/*******************  Bit definition for UART_LCR register  *******************/

/** @brief LCR[1:0]: Data length select */
#define UART_LCR_DLS_Pos                    (0U)
#define UART_LCR_DLS_Msk                    (0x3UL << UART_LCR_DLS_Pos)
#define UART_LCR_DLS                        UART_LCR_DLS_Msk

#define UART_LCR_DLS_CHAR_5BITS             (0x0UL << UART_LCR_DLS_Pos)     /*!< 5 data bits per character */
#define UART_LCR_DLS_CHAR_6BITS             (0x1UL << UART_LCR_DLS_Pos)     /*!< 6 data bits per character */
#define UART_LCR_DLS_CHAR_7BITS             (0x2UL << UART_LCR_DLS_Pos)     /*!< 7 data bits per character */
#define UART_LCR_DLS_CHAR_8BITS             (0x3UL << UART_LCR_DLS_Pos)     /*!< 8 data bits per character */

/** @brief LCR[2]: Number of stop bits */
#define UART_LCR_STOP_Pos                   (2U)
#define UART_LCR_STOP_Msk                   (0x1UL << UART_LCR_STOP_Pos)
#define UART_LCR_STOP                       UART_LCR_STOP_Msk

#define UART_LCR_STOP_1BIT                  (0x0UL << UART_LCR_STOP_Pos)    /*!< 1 stop bit */
#define UART_LCR_STOP_1_5BIT_OR_2BIT        (0x1UL << UART_LCR_STOP_Pos)    /*!< 1.5 stop bits when DLS is 0, otherwise 2 stop bits */

/** @brief LCR[3]: Parity enable */
#define UART_LCR_PEN_Pos                    (3U)
#define UART_LCR_PEN_Msk                    (0x1UL << UART_LCR_PEN_Pos)
#define UART_LCR_PEN                        UART_LCR_PEN_Msk

#define UART_LCR_PEN_DISABLE                (0x0UL << UART_LCR_PEN_Pos)     /*!< Disable parity */
#define UART_LCR_PEN_ENABLE                 (0x1UL << UART_LCR_PEN_Pos)     /*!< Enable parity */

/** @brief LCR[4]: Even parity select */
#define UART_LCR_EPS_Pos                    (4U)
#define UART_LCR_EPS_Msk                    (0x1UL << UART_LCR_EPS_Pos)
#define UART_LCR_EPS                        UART_LCR_EPS_Msk

#define UART_LCR_EPS_ODD                    (0x0UL << UART_LCR_EPS_Pos)     /*!< Odd parity */
#define UART_LCR_EPS_EVEN                   (0x1UL << UART_LCR_EPS_Pos)     /*!< Even parity */

/** @brief LCR[5]: Stick parity */
#define UART_LCR_SP_Pos                     (5U)
#define UART_LCR_SP_Msk                     (0x1UL << UART_LCR_SP_Pos)
#define UART_LCR_SP                         UART_LCR_SP_Msk

#define UART_LCR_SP_DISABLE                 (0x0UL << UART_LCR_SP_Pos)      /*!< Stick parity disabled */
#define UART_LCR_SP_ENABLE                  (0x1UL << UART_LCR_SP_Pos)      /*!< Stick parity enabled */

/** @brief LCR[6]: Break control */
#define UART_LCR_BC_Pos                     (6U)
#define UART_LCR_BC_Msk                     (0x1UL << UART_LCR_BC_Pos)
#define UART_LCR_BC                         UART_LCR_BC_Msk

#define UART_LCR_BC_DISABLE                 (0x0UL << UART_LCR_BC_Pos)      /*!< Serial output is released for data transmission */
#define UART_LCR_BC_ENABLE                  (0x1UL << UART_LCR_BC_Pos)      /*!< Serial output is forced to spacing state */

/** @brief LCR[7]: Divisor latch access */
#define UART_LCR_DLAB_Pos                   (7U)
#define UART_LCR_DLAB_Msk                   (0x1UL << UART_LCR_DLAB_Pos)
#define UART_LCR_DLAB                       UART_LCR_DLAB_Msk

#define UART_LCR_DLAB_DISABLE               (0x0UL << UART_LCR_DLAB_Pos)    /*!< Divisor Latch is writable only when UART not busy */
#define UART_LCR_DLAB_ENABLE                (0x1UL << UART_LCR_DLAB_Pos)    /*!< Divisor Latch is always readable and writable */

/*******************  Bit definition for UART_MCR register  *******************/

/** @brief MCR[0]: Data terminal ready */
#define UART_MCR_DTR_Pos                    (0U)
#define UART_MCR_DTR_Msk                    (0x1UL << UART_MCR_DTR_Pos)
#define UART_MCR_DTR                        UART_MCR_DTR_Msk
/** @brief MCR[1]: Request to send */
#define UART_MCR_RTS_Pos                    (1U)
#define UART_MCR_RTS_Msk                    (0x1UL << UART_MCR_RTS_Pos)
#define UART_MCR_RTS                        UART_MCR_RTS_Msk
/** @brief MCR[2]: OUT1 control */
#define UART_MCR_OUT1_Pos                   (2U)
#define UART_MCR_OUT1_Msk                   (0x1UL << UART_MCR_OUT1_Pos)
#define UART_MCR_OUT1                       UART_MCR_OUT1_Msk
/** @brief MCR[3]: OUT2 control */
#define UART_MCR_OUT2_Pos                   (3U)
#define UART_MCR_OUT2_Msk                   (0x1UL << UART_MCR_OUT2_Pos)
#define UART_MCR_OUT2                       UART_MCR_OUT2_Msk
/** @brief MCR[4]: Loopback mode */
#define UART_MCR_LOOPBACK_Pos               (4U)
#define UART_MCR_LOOPBACK_Msk               (0x1UL << UART_MCR_LOOPBACK_Pos)
#define UART_MCR_LOOPBACK                   UART_MCR_LOOPBACK_Msk
/** @brief MCR[5]: Auto flow control enable */
#define UART_MCR_AFCE_Pos                   (5U)
#define UART_MCR_AFCE_Msk                   (0x1UL << UART_MCR_AFCE_Pos)
#define UART_MCR_AFCE                       UART_MCR_AFCE_Msk
/** @brief MCR[6]: SIR mode enable */
#define UART_MCR_SIRE_Pos                   (6U)
#define UART_MCR_SIRE_Msk                   (0x1UL << UART_MCR_SIRE_Pos)
#define UART_MCR_SIRE                       UART_MCR_SIRE_Msk

/*******************  Bit definition for UART_LSR register  *******************/

/** @brief LSR[0]: Data ready */
#define UART_LSR_DR_Pos                     (0U)
#define UART_LSR_DR_Msk                     (0x1UL << UART_LSR_DR_Pos)
#define UART_LSR_DR                         UART_LSR_DR_Msk
/** @brief LSR[1]: Overrun error */
#define UART_LSR_OE_Pos                     (1U)
#define UART_LSR_OE_Msk                     (0x1UL << UART_LSR_OE_Pos)
#define UART_LSR_OE                         UART_LSR_OE_Msk
/** @brief LSR[2]: Parity error */
#define UART_LSR_PE_Pos                     (2U)
#define UART_LSR_PE_Msk                     (0x1UL << UART_LSR_PE_Pos)
#define UART_LSR_PE                         UART_LSR_PE_Msk
/** @brief LSR[3]: Framing error */
#define UART_LSR_FE_Pos                     (3U)
#define UART_LSR_FE_Msk                     (0x1UL << UART_LSR_FE_Pos)
#define UART_LSR_FE                         UART_LSR_FE_Msk
/** @brief LSR[4]: Break interrupt */
#define UART_LSR_BI_Pos                     (4U)
#define UART_LSR_BI_Msk                     (0x1UL << UART_LSR_BI_Pos)
#define UART_LSR_BI                         UART_LSR_BI_Msk
/** @brief LSR[5]: Transmit holding register empty */
#define UART_LSR_THRE_Pos                   (5U)
#define UART_LSR_THRE_Msk                   (0x1UL << UART_LSR_THRE_Pos)
#define UART_LSR_THRE                       UART_LSR_THRE_Msk
/** @brief LSR[6]: Transmitter empty */
#define UART_LSR_TEMT_Pos                   (6U)
#define UART_LSR_TEMT_Msk                   (0x1UL << UART_LSR_TEMT_Pos)
#define UART_LSR_TEMT                       UART_LSR_TEMT_Msk
/** @brief LSR[7]: Receiver FIFO error */
#define UART_LSR_RFE_Pos                    (7U)
#define UART_LSR_RFE_Msk                    (0x1UL << UART_LSR_RFE_Pos)
#define UART_LSR_RFE                        UART_LSR_RFE_Msk
/** @brief LSR[8]: Address received */
#define UART_LSR_ADDR_RCVD_Pos              (8U)
#define UART_LSR_ADDR_RCVD_Msk              (0x1UL << UART_LSR_ADDR_RCVD_Pos)
#define UART_LSR_ADDR_RCVD                  UART_LSR_ADDR_RCVD_Msk

/*******************  Bit definition for UART_MSR register  *******************/

/** @brief MSR[0]: Delta clear to send */
#define UART_MSR_DCTS_Pos                   (0U)
#define UART_MSR_DCTS_Msk                   (0x1UL << UART_MSR_DCTS_Pos)
#define UART_MSR_DCTS                       UART_MSR_DCTS_Msk
/** @brief MSR[1]: Delta data set ready */
#define UART_MSR_DDSR_Pos                   (1U)
#define UART_MSR_DDSR_Msk                   (0x1UL << UART_MSR_DDSR_Pos)
#define UART_MSR_DDSR                       UART_MSR_DDSR_Msk
/** @brief MSR[2]: Trailing edge of ring indicator */
#define UART_MSR_TERI_Pos                   (2U)
#define UART_MSR_TERI_Msk                   (0x1UL << UART_MSR_TERI_Pos)
#define UART_MSR_TERI                       UART_MSR_TERI_Msk
/** @brief MSR[3]: Delta data carrier detect */
#define UART_MSR_DDCD_Pos                   (3U)
#define UART_MSR_DDCD_Msk                   (0x1UL << UART_MSR_DDCD_Pos)
#define UART_MSR_DDCD                       UART_MSR_DDCD_Msk
/** @brief MSR[4]: Clear to send */
#define UART_MSR_CTS_Pos                    (4U)
#define UART_MSR_CTS_Msk                    (0x1UL << UART_MSR_CTS_Pos)
#define UART_MSR_CTS                        UART_MSR_CTS_Msk
/** @brief MSR[5]: Data set ready */
#define UART_MSR_DSR_Pos                    (5U)
#define UART_MSR_DSR_Msk                    (0x1UL << UART_MSR_DSR_Pos)
#define UART_MSR_DSR                        UART_MSR_DSR_Msk
/** @brief MSR[6]: Ring indicator */
#define UART_MSR_RI_Pos                     (6U)
#define UART_MSR_RI_Msk                     (0x1UL << UART_MSR_RI_Pos)
#define UART_MSR_RI                         UART_MSR_RI_Msk
/** @brief MSR[7]: Data carrier detect */
#define UART_MSR_DCD_Pos                    (7U)
#define UART_MSR_DCD_Msk                    (0x1UL << UART_MSR_DCD_Pos)
#define UART_MSR_DCD                        UART_MSR_DCD_Msk

/*******************  Bit definition for UART_SCR register  *******************/

/** @brief SCR[7:0]: Temporary storage space */
#define UART_SCR_Pos                        (0U)
#define UART_SCR_Msk                        (0xFFUL << UART_SCR_Pos)
#define UART_SCR                            UART_SCR_Msk

/*******************  Bit definition for UART_SRBR register  ******************/

/** @brief SRBR[7:0]: Received data (8-bit mode) */
#define UART_SRBR_8BIT_Pos                  (0U)
#define UART_SRBR_8BIT_Msk                  (0xFFUL << UART_SRBR_8BIT_Pos)
#define UART_SRBR_8BIT                      UART_SRBR_8BIT_Msk

/** @brief SRBR[8:0]: Received data (9-bit mode) */
#define UART_SRBR_9BIT_Pos                  (0U)
#define UART_SRBR_9BIT_Msk                  (0x1FFUL << UART_SRBR_9BIT_Pos)
#define UART_SRBR_9BIT                      UART_SRBR_9BIT_Msk

/*******************  Bit definition for UART_STHR register  ******************/

/** @brief STHR[7:0]: Data to be transmitted (8-bit mode) */
#define UART_STHR_8BIT_Pos                  (0U)
#define UART_STHR_8BIT_Msk                  (0xFFUL << UART_STHR_8BIT_Pos)
#define UART_STHR_8BIT                      UART_STHR_8BIT_Msk

/** @brief STHR[8:0]: Data to be transmitted (9-bit mode) */
#define UART_STHR_9BIT_Pos                  (0U)
#define UART_STHR_9BIT_Msk                  (0x1FFUL << UART_STHR_9BIT_Pos)
#define UART_STHR_9BIT                      UART_STHR_9BIT_Msk

/*******************  Bit definition for UART_FAR register  *******************/

/** @brief FAR[0]: FIFO access mode enable */
#define UART_FAR_Pos                        (0U)
#define UART_FAR_Msk                        (0x1UL << UART_FAR_Pos)
#define UART_FAR                            UART_FAR_Msk

/*******************  Bit definition for UART_TFR register  *******************/

/** @brief TFR[7:0]: TX FIFO read data */
#define UART_TFR_Pos                        (0U)
#define UART_TFR_Msk                        (0xFFUL << UART_TFR_Pos)
#define UART_TFR                            UART_TFR_Msk

/*******************  Bit definition for UART_RFW register  *******************/

/** @brief RFW[7:0]: RX FIFO write data */
#define UART_RFW_RFWD_Pos                   (0U)
#define UART_RFW_RFWD_Msk                   (0xFFUL << UART_RFW_RFWD_Pos)
#define UART_RFW_RFWD                       UART_RFW_RFWD_Msk
/** @brief RFW[8]: RX FIFO parity error */
#define UART_RFW_RFPE_Pos                   (8U)
#define UART_RFW_RFPE_Msk                   (0x1UL << UART_RFW_RFPE_Pos)
#define UART_RFW_RFPE                       UART_RFW_RFPE_Msk
/** @brief RFW[9]: RX FIFO framing error */
#define UART_RFW_RFFE_Pos                   (9U)
#define UART_RFW_RFFE_Msk                   (0x1UL << UART_RFW_RFFE_Pos)
#define UART_RFW_RFFE                       UART_RFW_RFFE_Msk

/*******************  Bit definition for UART_USR register  *******************/

/** @brief USR[0]: UART busy */
#define UART_USR_BUSY_Pos                   (0U)
#define UART_USR_BUSY_Msk                   (0x1UL << UART_USR_BUSY_Pos)
#define UART_USR_BUSY                       UART_USR_BUSY_Msk
/** @brief USR[1]: TX FIFO not full */
#define UART_USR_TFNF_Pos                   (1U)
#define UART_USR_TFNF_Msk                   (0x1UL << UART_USR_TFNF_Pos)
#define UART_USR_TFNF                       UART_USR_TFNF_Msk
/** @brief USR[2]: TX FIFO empty */
#define UART_USR_TFE_Pos                    (2U)
#define UART_USR_TFE_Msk                    (0x1UL << UART_USR_TFE_Pos)
#define UART_USR_TFE                        UART_USR_TFE_Msk
/** @brief USR[3]: RX FIFO not empty */
#define UART_USR_RFNE_Pos                   (3U)
#define UART_USR_RFNE_Msk                   (0x1UL << UART_USR_RFNE_Pos)
#define UART_USR_RFNE                       UART_USR_RFNE_Msk
/** @brief USR[4]: RX FIFO full */
#define UART_USR_RFF_Pos                    (4U)
#define UART_USR_RFF_Msk                    (0x1UL << UART_USR_RFF_Pos)
#define UART_USR_RFF                        UART_USR_RFF_Msk

/*******************  Bit definition for UART_TFL register  *******************/

/** @brief TFL[4:0]: TX FIFO level */
#define UART_TFL_Pos                        (0U)
#define UART_TFL_Msk                        (0x1FUL << UART_TFL_Pos)
#define UART_TFL                            UART_TFL_Msk

/*******************  Bit definition for UART_RFL register  *******************/

/** @brief RFL[4:0]: RX FIFO level */
#define UART_RFL_Pos                        (0U)
#define UART_RFL_Msk                        (0x1FUL << UART_RFL_Pos)
#define UART_RFL                            UART_RFL_Msk

/*******************  Bit definition for UART_SRR register  *******************/

/** @brief SRR[0]: UART reset */
#define UART_SRR_UR_Pos                     (0U)
#define UART_SRR_UR_Msk                     (0x1UL << UART_SRR_UR_Pos)
#define UART_SRR_UR                         UART_SRR_UR_Msk
/** @brief SRR[1]: RX FIFO reset */
#define UART_SRR_RFR_Pos                    (1U)
#define UART_SRR_RFR_Msk                    (0x1UL << UART_SRR_RFR_Pos)
#define UART_SRR_RFR                        UART_SRR_RFR_Msk
/** @brief SRR[2]: TX FIFO reset */
#define UART_SRR_XFR_Pos                    (2U)
#define UART_SRR_XFR_Msk                    (0x1UL << UART_SRR_XFR_Pos)
#define UART_SRR_XFR                        UART_SRR_XFR_Msk

/*******************  Bit definition for UART_SRTS register  ******************/

/** @brief SRTS[0]: Shadow request to send */
#define UART_SRTS_Pos                       (0U)
#define UART_SRTS_Msk                       (0x1UL << UART_SRTS_Pos)
#define UART_SRTS                           UART_SRTS_Msk

/*******************  Bit definition for UART_SBCR register  ******************/

/** @brief SBCR[0]: Shadow break control */
#define UART_SBCR_SBCB_Pos                  (0U)
#define UART_SBCR_SBCB_Msk                  (0x1UL << UART_SBCR_SBCB_Pos)
#define UART_SBCR_SBCB                      UART_SBCR_SBCB_Msk

/******************  Bit definition for UART_SDMAM register  ******************/

/** @brief SDMAM[0]: Shadow DMA mode */
#define UART_SDMAM_Pos                      (0U)
#define UART_SDMAM_Msk                      (0x1UL << UART_SDMAM_Pos)
#define UART_SDMAM                          UART_SDMAM_Msk

/*******************  Bit definition for UART_SFE register  *******************/

/** @brief SFE[0]: Shadow FIFO enable */
#define UART_SFE_Pos                        (0U)
#define UART_SFE_Msk                        (0x1UL << UART_SFE_Pos)
#define UART_SFE                            UART_SFE_Msk

/*******************  Bit definition for UART_SRT register  *******************/

/** @brief SRT[1:0]: Shadow RX FIFO trigger */
#define UART_SRT_Pos                        (0U)
#define UART_SRT_Msk                        (0x3UL << UART_SRT_Pos)
#define UART_SRT                            UART_SRT_Msk

#define UART_SRT_SRT_FIFO_CHAR_1            (0x0UL << UART_SRT_Pos)     /*!< 1 character in FIFO */
#define UART_SRT_SRT_FIFO_QUARTER_FULL      (0x1UL << UART_SRT_Pos)     /*!< FIFO 1/4 full */
#define UART_SRT_SRT_FIFO_HALF_FULL         (0x2UL << UART_SRT_Pos)     /*!< FIFO 1/2 full */
#define UART_SRT_SRT_FIFO_FULL_2            (0x3UL << UART_SRT_Pos)     /*!< FIFO 2 less than full */

/*******************  Bit definition for UART_STET register  ******************/

/** @brief STET[1:0]: Shadow TX FIFO empty trigger */
#define UART_STET_Pos                       (0U)
#define UART_STET_Msk                       (0x3UL << UART_STET_Pos)
#define UART_STET                           UART_STET_Msk

#define UART_STET_STET_FIFO_EMPTY           (0x0UL << UART_STET_Pos)    /*!< FIFO empty */
#define UART_STET_STET_FIFO_CHAR_2          (0x1UL << UART_STET_Pos)    /*!< 2 characters in FIFO */
#define UART_STET_STET_FIFO_QUARTER_FULL    (0x2UL << UART_STET_Pos)    /*!< FIFO 1/4 full */
#define UART_STET_STET_FIFO_HALF_FULL       (0x3UL << UART_STET_Pos)    /*!< FIFO 1/2 full */

/*******************  Bit definition for UART_HTX register  *******************/

/** @brief HTX[0]: Halt TX */
#define UART_HTX_Pos                        (0U)
#define UART_HTX_Msk                        (0x1UL << UART_HTX_Pos)
#define UART_HTX                            UART_HTX_Msk

/******************  Bit definition for UART_DMASA register  ******************/

/** @brief DMASA[0]: DMA software acknowledge */
#define UART_DMASA_Pos                      (0U)
#define UART_DMASA_Msk                      (0x1UL << UART_DMASA_Pos)
#define UART_DMASA                          UART_DMASA_Msk

/*******************  Bit definition for UART_TCR register  *******************/

/** @brief TCR[0]: RS485 transfer enable */
#define UART_TCR_RS485_EN_Pos               (0U)
#define UART_TCR_RS485_EN_Msk               (0x1UL << UART_TCR_RS485_EN_Pos)
#define UART_TCR_RS485_EN                   UART_TCR_RS485_EN_Msk
/** @brief TCR[1]: Receiver enable polarity */
#define UART_TCR_RE_POL_Pos                 (1U)
#define UART_TCR_RE_POL_Msk                 (0x1UL << UART_TCR_RE_POL_Pos)
#define UART_TCR_RE_POL                     UART_TCR_RE_POL_Msk
/** @brief TCR[2]: Driver enable polarity */
#define UART_TCR_DE_POL_Pos                 (2U)
#define UART_TCR_DE_POL_Msk                 (0x1UL << UART_TCR_DE_POL_Pos)
#define UART_TCR_DE_POL                     UART_TCR_DE_POL_Msk

/** @brief TCR[4:3]: Transfer mode */
#define UART_TCR_XFER_MODE_Pos              (3U)
#define UART_TCR_XFER_MODE_Msk              (0x3UL << UART_TCR_XFER_MODE_Pos)
#define UART_TCR_XFER_MODE                  UART_TCR_XFER_MODE_Msk

#define UART_TCR_XFER_MODE0                 (0x0UL << UART_TCR_XFER_MODE_Pos)   /*!< Mode 0 */
#define UART_TCR_XFER_MODE1                 (0x1UL << UART_TCR_XFER_MODE_Pos)   /*!< Mode 1 */
#define UART_TCR_XFER_MODE2                 (0x2UL << UART_TCR_XFER_MODE_Pos)   /*!< Mode 2 */

/******************  Bit definition for UART_DE_EN register  ******************/

/** @brief DE_EN[0]: Driver enable */
#define UART_DE_EN_Pos                      (0U)
#define UART_DE_EN_Msk                      (0x1UL << UART_DE_EN_Pos)
#define UART_DE_EN                          UART_DE_EN_Msk

/******************  Bit definition for UART_RE_EN register  ******************/

/** @brief RE_EN[0]: Receiver enable */
#define UART_RE_EN_Pos                      (0U)
#define UART_RE_EN_Msk                      (0x1UL << UART_RE_EN_Pos)
#define UART_RE_EN                          UART_RE_EN_Msk

/*******************  Bit definition for UART_DET register  *******************/

/** @brief DET[7:0]: Driver enable assertion time */
#define UART_DET_DE_ASSERT_TIME_Pos         (0U)
#define UART_DET_DE_ASSERT_TIME_Msk         (0xFFUL << UART_DET_DE_ASSERT_TIME_Pos)
#define UART_DET_DE_ASSERT_TIME             UART_DET_DE_ASSERT_TIME_Msk
/** @brief DET[23:16]: Driver enable deassertion time */
#define UART_DET_DE_DEASSERT_TIME_Pos       (16U)
#define UART_DET_DE_DEASSERT_TIME_Msk       (0xFFUL << UART_DET_DE_DEASSERT_TIME_Pos)
#define UART_DET_DE_DEASSERT_TIME           UART_DET_DE_DEASSERT_TIME_Msk

/*******************  Bit definition for UART_TAT register  *******************/

/** @brief TAT[15:0]: Driver enable to receiver enable turnaround time */
#define UART_TAT_DE_TO_RE_Pos               (0U)
#define UART_TAT_DE_TO_RE_Msk               (0xFFFFUL << UART_TAT_DE_TO_RE_Pos)
#define UART_TAT_DE_TO_RE                   UART_TAT_DE_TO_RE_Msk
/** @brief TAT[31:16]: Receiver enable to driver enable turnaround time */
#define UART_TAT_RE_TO_DE_Pos               (16U)
#define UART_TAT_RE_TO_DE_Msk               (0xFFFFUL << UART_TAT_RE_TO_DE_Pos)
#define UART_TAT_RE_TO_DE                   UART_TAT_RE_TO_DE_Msk

/*******************  Bit definition for UART_DLF register  *******************/

/** @brief DLF[3:0]: Fractional part of baudrate divisor */
#define UART_DLF_Pos                        (0U)
#define UART_DLF_Msk                        (0xFUL << UART_DLF_Pos)
#define UART_DLF                            UART_DLF_Msk

/*******************  Bit definition for UART_RAR register  *******************/

/** @brief RAR[7:0]: Receive address */
#define UART_RAR_Pos                        (0U)
#define UART_RAR_Msk                        (0xFFUL << UART_RAR_Pos)
#define UART_RAR                            UART_RAR_Msk

/*******************  Bit definition for UART_TAR register  *******************/

/** @brief TAR[7:0]: Transmit address */
#define UART_TAR_Pos                        (0U)
#define UART_TAR_Msk                        (0xFFUL << UART_TAR_Pos)
#define UART_TAR                            UART_TAR_Msk

/*****************  Bit definition for UART_LCR_EXT register  *****************/

/** @brief LCR_EXT[0]: Enable 9-bit data for transmit and receive transfers */
#define UART_LCR_EXT_DLS_E_Pos              (0U)
#define UART_LCR_EXT_DLS_E_Msk              (0x1UL << UART_LCR_EXT_DLS_E_Pos)
#define UART_LCR_EXT_DLS_E                  UART_LCR_EXT_DLS_E_Msk
/** @brief LCR_EXT[1]: Address match mode */
#define UART_LCR_EXT_ADDR_MATCH_Pos         (1U)
#define UART_LCR_EXT_ADDR_MATCH_Msk         (0x1UL << UART_LCR_EXT_ADDR_MATCH_Pos)
#define UART_LCR_EXT_ADDR_MATCH             UART_LCR_EXT_ADDR_MATCH_Msk
/** @brief LCR_EXT[2]: Send address control */
#define UART_LCR_EXT_SEND_ADDR_Pos          (2U)
#define UART_LCR_EXT_SEND_ADDR_Msk          (0x1UL << UART_LCR_EXT_SEND_ADDR_Pos)
#define UART_LCR_EXT_SEND_ADDR              UART_LCR_EXT_SEND_ADDR_Msk
/** @brief LCR_EXT[3]: Transmit mode control */
#define UART_LCR_EXT_TX_MODE_Pos            (3U)
#define UART_LCR_EXT_TX_MODE_Msk            (0x1UL << UART_LCR_EXT_TX_MODE_Pos)
#define UART_LCR_EXT_TX_MODE                UART_LCR_EXT_TX_MODE_Msk

/****************  Bit definition for UART_PROT_LEVEL register  ***************/

/** @brief PROT_LEVEL[2:0]: Protection level */
#define UART_PROT_LEVEL_Pos                 (0U)
#define UART_PROT_LEVEL_Msk                 (0x7UL << UART_PROT_LEVEL_Pos)
#define UART_PROT_LEVEL                     UART_PROT_LEVEL_Msk

/*************  Bit definition for UART_REG_TIMEOUT_RST register  *************/

/** @brief REG_TIMEOUT_RST[7:0]: reset value of REG_TIMEOUT register */
#define UART_REG_TIMEOUT_RST_Pos            (0U)
#define UART_REG_TIMEOUT_RST_Msk            (0xFFUL << UART_REG_TIMEOUT_RST_Pos)
#define UART_REG_TIMEOUT_RST                UART_REG_TIMEOUT_RST_Msk

/*******************  Bit definition for UART_UCV register  *******************/

/** @brief UCV[31:0]: UART component version */
#define UART_UCV_Pos                        (0U)
#define UART_UCV_Msk                        (0xFFFFFFFFUL << UART_UCV_Pos)
#define UART_UCV                            UART_UCV_Msk

/*******************  Bit definition for UART_CTR register  *******************/

/** @brief CTR[31:0]: UART component type */
#define UART_CTR_Pos                        (0U)
#define UART_CTR_Msk                        (0xFFFFFFFFUL << UART_CTR_Pos)
#define UART_CTR                            UART_CTR_Msk

/******************************************************************************/
/*                                    USB                                     */
/******************************************************************************/

/*******************  Bit definition for USB_FADDR register  ******************/

/** @brief FADDR[6:0]: The function address */
#define USB_FADDR_FUNC_Pos                  (0U)
#define USB_FADDR_FUNC_Msk                  (0x7FUL << USB_FADDR_FUNC_Pos)
#define USB_FADDR_FUNC                      USB_FADDR_FUNC_Msk

/*******************  Bit definition for USB_POWER register  ******************/

/** @brief POWER[0]: Suspend mode enable */
#define USB_POWER_SUSPEN_Pos                (0U)
#define USB_POWER_SUSPEN_Msk                (0x1UL << USB_POWER_SUSPEN_Pos)
#define USB_POWER_SUSPEN                    USB_POWER_SUSPEN_Msk
/** @brief POWER[1]: Suspend mode status */
#define USB_POWER_SUSPMODE_Pos              (1U)
#define USB_POWER_SUSPMODE_Msk              (0x1UL << USB_POWER_SUSPMODE_Pos)
#define USB_POWER_SUSPMODE                  USB_POWER_SUSPMODE_Msk
/** @brief POWER[2]: Resume from Suspend control */
#define USB_POWER_RESUME_Pos                (2U)
#define USB_POWER_RESUME_Msk                (0x1UL << USB_POWER_RESUME_Pos)
#define USB_POWER_RESUME                    USB_POWER_RESUME_Msk
/** @brief POWER[3]: Reset status */
#define USB_POWER_RESET_Pos                 (3U)
#define USB_POWER_RESET_Msk                 (0x1UL << USB_POWER_RESET_Pos)
#define USB_POWER_RESET                     USB_POWER_RESET_Msk
/** @brief POWER[4]: High-speed mode status */
#define USB_POWER_HSMODE_Pos                (4U)
#define USB_POWER_HSMODE_Msk                (0x1UL << USB_POWER_HSMODE_Pos)
#define USB_POWER_HSMODE                    USB_POWER_HSMODE_Msk
/** @brief POWER[5]: High-speed mode control */
#define USB_POWER_HSEN_Pos                  (5U)
#define USB_POWER_HSEN_Msk                  (0x1UL << USB_POWER_HSEN_Pos)
#define USB_POWER_HSEN                      USB_POWER_HSEN_Msk
/** @brief POWER[6]: Soft Connect/Disconnect feature selection */
#define USB_POWER_SOFTCONN_Pos              (6U)
#define USB_POWER_SOFTCONN_Msk              (0x1UL << USB_POWER_SOFTCONN_Pos)
#define USB_POWER_SOFTCONN                  USB_POWER_SOFTCONN_Msk
/** @brief POWER[7]: ISO Update */
#define USB_POWER_ISOUPD_Pos                (7U)
#define USB_POWER_ISOUPD_Msk                (0x1UL << USB_POWER_ISOUPD_Pos)
#define USB_POWER_ISOUPD                    USB_POWER_ISOUPD_Msk

/*******************  Bit definition for USB_INTRTX register  *****************/

/** @brief INTRTX[0]: Endpoint 0 Interrupt Flag */
#define USB_INTRTX_EP0IF_Pos                (0U)
#define USB_INTRTX_EP0IF_Msk                (0x1UL << USB_INTRTX_EP0IF_Pos)
#define USB_INTRTX_EP0IF                    USB_INTRTX_EP0IF_Msk
/** @brief INTRTX[1]: Endpoint 1 TX Interrupt Flag */
#define USB_INTRTX_EP1TXIF_Pos              (1U)
#define USB_INTRTX_EP1TXIF_Msk              (0x1UL << USB_INTRTX_EP1TXIF_Pos)
#define USB_INTRTX_EP1TXIF                  USB_INTRTX_EP1TXIF_Msk
/** @brief INTRTX[2]: Endpoint 2 TX Interrupt Flag */
#define USB_INTRTX_EP2TXIF_Pos              (2U)
#define USB_INTRTX_EP2TXIF_Msk              (0x1UL << USB_INTRTX_EP2TXIF_Pos)
#define USB_INTRTX_EP2TXIF                  USB_INTRTX_EP2TXIF_Msk
/** @brief INTRTX[3]: Endpoint 3 TX Interrupt Flag */
#define USB_INTRTX_EP3TXIF_Pos              (3U)
#define USB_INTRTX_EP3TXIF_Msk              (0x1UL << USB_INTRTX_EP3TXIF_Pos)
#define USB_INTRTX_EP3TXIF                  USB_INTRTX_EP3TXIF_Msk
/** @brief INTRTX[4]: Endpoint 4 TX Interrupt Flag */
#define USB_INTRTX_EP4TXIF_Pos              (4U)
#define USB_INTRTX_EP4TXIF_Msk              (0x1UL << USB_INTRTX_EP4TXIF_Pos)
#define USB_INTRTX_EP4TXIF                  USB_INTRTX_EP4TXIF_Msk
/** @brief INTRTX[5]: Endpoint 5 TX Interrupt Flag */
#define USB_INTRTX_EP5TXIF_Pos              (5U)
#define USB_INTRTX_EP5TXIF_Msk              (0x1UL << USB_INTRTX_EP5TXIF_Pos)
#define USB_INTRTX_EP5TXIF                  USB_INTRTX_EP5TXIF_Msk
/** @brief INTRTX[6]: Endpoint 6 TX Interrupt Flag */
#define USB_INTRTX_EP6TXIF_Pos              (6U)
#define USB_INTRTX_EP6TXIF_Msk              (0x1UL << USB_INTRTX_EP6TXIF_Pos)
#define USB_INTRTX_EP6TXIF                  USB_INTRTX_EP6TXIF_Msk
/** @brief INTRTX[7]: Endpoint 7 TX Interrupt Flag */
#define USB_INTRTX_EP7TXIF_Pos              (7U)
#define USB_INTRTX_EP7TXIF_Msk              (0x1UL << USB_INTRTX_EP7TXIF_Pos)
#define USB_INTRTX_EP7TXIF                  USB_INTRTX_EP7TXIF_Msk
/** @brief INTRTX[8]: Endpoint 8 TX Interrupt Flag */
#define USB_INTRTX_EP8TXIF_Pos              (8U)
#define USB_INTRTX_EP8TXIF_Msk              (0x1UL << USB_INTRTX_EP8TXIF_Pos)
#define USB_INTRTX_EP8TXIF                  USB_INTRTX_EP8TXIF_Msk
/** @brief INTRTX[9]: Endpoint 9 TX Interrupt Flag */
#define USB_INTRTX_EP9TXIF_Pos              (9U)
#define USB_INTRTX_EP9TXIF_Msk              (0x1UL << USB_INTRTX_EP9TXIF_Pos)
#define USB_INTRTX_EP9TXIF                  USB_INTRTX_EP9TXIF_Msk
/** @brief INTRTX[10]: Endpoint 10 TX Interrupt Flag */
#define USB_INTRTX_EP10TXIF_Pos             (10U)
#define USB_INTRTX_EP10TXIF_Msk             (0x1UL << USB_INTRTX_EP10TXIF_Pos)
#define USB_INTRTX_EP10TXIF                 USB_INTRTX_EP10TXIF_Msk
/** @brief INTRTX[11]: Endpoint 11 TX Interrupt Flag */
#define USB_INTRTX_EP11TXIF_Pos             (11U)
#define USB_INTRTX_EP11TXIF_Msk             (0x1UL << USB_INTRTX_EP11TXIF_Pos)
#define USB_INTRTX_EP11TXIF                 USB_INTRTX_EP11TXIF_Msk
/** @brief INTRTX[12]: Endpoint 12 TX Interrupt Flag */
#define USB_INTRTX_EP12TXIF_Pos             (12U)
#define USB_INTRTX_EP12TXIF_Msk             (0x1UL << USB_INTRTX_EP12TXIF_Pos)
#define USB_INTRTX_EP12TXIF                 USB_INTRTX_EP12TXIF_Msk
/** @brief INTRTX[13]: Endpoint 13 TX Interrupt Flag */
#define USB_INTRTX_EP13TXIF_Pos             (13U)
#define USB_INTRTX_EP13TXIF_Msk             (0x1UL << USB_INTRTX_EP13TXIF_Pos)
#define USB_INTRTX_EP13TXIF                 USB_INTRTX_EP13TXIF_Msk
/** @brief INTRTX[14]: Endpoint 14 TX Interrupt Flag */
#define USB_INTRTX_EP14TXIF_Pos             (14U)
#define USB_INTRTX_EP14TXIF_Msk             (0x1UL << USB_INTRTX_EP14TXIF_Pos)
#define USB_INTRTX_EP14TXIF                 USB_INTRTX_EP14TXIF_Msk
/** @brief INTRTX[15]: Endpoint 15 TX Interrupt Flag */
#define USB_INTRTX_EP15TXIF_Pos             (15U)
#define USB_INTRTX_EP15TXIF_Msk             (0x1UL << USB_INTRTX_EP15TXIF_Pos)
#define USB_INTRTX_EP15TXIF                 USB_INTRTX_EP15TXIF_Msk

/*******************  Bit definition for USB_INTRRX register  *****************/

/** @brief INTRRX[1]: Endpoint 1 RX Interrupt Flag */
#define USB_INTRRX_EP1RXIF_Pos              (1U)
#define USB_INTRRX_EP1RXIF_Msk              (0x1UL << USB_INTRRX_EP1RXIF_Pos)
#define USB_INTRRX_EP1RXIF                  USB_INTRRX_EP1RXIF_Msk
/** @brief INTRRX[2]: Endpoint 2 RX Interrupt Flag */
#define USB_INTRRX_EP2RXIF_Pos              (2U)
#define USB_INTRRX_EP2RXIF_Msk              (0x1UL << USB_INTRRX_EP2RXIF_Pos)
#define USB_INTRRX_EP2RXIF                  USB_INTRRX_EP2RXIF_Msk
/** @brief INTRRX[3]: Endpoint 3 RX Interrupt Flag */
#define USB_INTRRX_EP3RXIF_Pos              (3U)
#define USB_INTRRX_EP3RXIF_Msk              (0x1UL << USB_INTRRX_EP3RXIF_Pos)
#define USB_INTRRX_EP3RXIF                  USB_INTRRX_EP3RXIF_Msk
/** @brief INTRRX[4]: Endpoint 4 RX Interrupt Flag */
#define USB_INTRRX_EP4RXIF_Pos              (4U)
#define USB_INTRRX_EP4RXIF_Msk              (0x1UL << USB_INTRRX_EP4RXIF_Pos)
#define USB_INTRRX_EP4RXIF                  USB_INTRRX_EP4RXIF_Msk
/** @brief INTRRX[5]: Endpoint 5 RX Interrupt Flag */
#define USB_INTRRX_EP5RXIF_Pos              (5U)
#define USB_INTRRX_EP5RXIF_Msk              (0x1UL << USB_INTRRX_EP5RXIF_Pos)
#define USB_INTRRX_EP5RXIF                  USB_INTRRX_EP5RXIF_Msk
/** @brief INTRRX[6]: Endpoint 6 RX Interrupt Flag */
#define USB_INTRRX_EP6RXIF_Pos              (6U)
#define USB_INTRRX_EP6RXIF_Msk              (0x1UL << USB_INTRRX_EP6RXIF_Pos)
#define USB_INTRRX_EP6RXIF                  USB_INTRRX_EP6RXIF_Msk
/** @brief INTRRX[7]: Endpoint 7 RX Interrupt Flag */
#define USB_INTRRX_EP7RXIF_Pos              (7U)
#define USB_INTRRX_EP7RXIF_Msk              (0x1UL << USB_INTRRX_EP7RXIF_Pos)
#define USB_INTRRX_EP7RXIF                  USB_INTRRX_EP7RXIF_Msk
/** @brief INTRRX[8]: Endpoint 8 RX Interrupt Flag */
#define USB_INTRRX_EP8RXIF_Pos              (8U)
#define USB_INTRRX_EP8RXIF_Msk              (0x1UL << USB_INTRRX_EP8RXIF_Pos)
#define USB_INTRRX_EP8RXIF                  USB_INTRRX_EP8RXIF_Msk
/** @brief INTRRX[9]: Endpoint 9 RX Interrupt Flag */
#define USB_INTRRX_EP9RXIF_Pos              (9U)
#define USB_INTRRX_EP9RXIF_Msk              (0x1UL << USB_INTRRX_EP9RXIF_Pos)
#define USB_INTRRX_EP9RXIF                  USB_INTRRX_EP9RXIF_Msk
/** @brief INTRRX[10]: Endpoint 10 RX Interrupt Flag */
#define USB_INTRRX_EP10RXIF_Pos             (10U)
#define USB_INTRRX_EP10RXIF_Msk             (0x1UL << USB_INTRRX_EP10RXIF_Pos)
#define USB_INTRRX_EP10RXIF                 USB_INTRRX_EP10RXIF_Msk
/** @brief INTRRX[11]: Endpoint 11 RX Interrupt Flag */
#define USB_INTRRX_EP11RXIF_Pos             (11U)
#define USB_INTRRX_EP11RXIF_Msk             (0x1UL << USB_INTRRX_EP11RXIF_Pos)
#define USB_INTRRX_EP11RXIF                 USB_INTRRX_EP11RXIF_Msk
/** @brief INTRRX[12]: Endpoint 12 RX Interrupt Flag */
#define USB_INTRRX_EP12RXIF_Pos             (12U)
#define USB_INTRRX_EP12RXIF_Msk             (0x1UL << USB_INTRRX_EP12RXIF_Pos)
#define USB_INTRRX_EP12RXIF                 USB_INTRRX_EP12RXIF_Msk
/** @brief INTRRX[13]: Endpoint 13 RX Interrupt Flag */
#define USB_INTRRX_EP13RXIF_Pos             (13U)
#define USB_INTRRX_EP13RXIF_Msk             (0x1UL << USB_INTRRX_EP13RXIF_Pos)
#define USB_INTRRX_EP13RXIF                 USB_INTRRX_EP13RXIF_Msk
/** @brief INTRRX[14]: Endpoint 14 RX Interrupt Flag */
#define USB_INTRRX_EP14RXIF_Pos             (14U)
#define USB_INTRRX_EP14RXIF_Msk             (0x1UL << USB_INTRRX_EP14RXIF_Pos)
#define USB_INTRRX_EP14RXIF                 USB_INTRRX_EP14RXIF_Msk
/** @brief INTRRX[15]: Endpoint 15 RX Interrupt Flag */
#define USB_INTRRX_EP15RXIF_Pos             (15U)
#define USB_INTRRX_EP15RXIF_Msk             (0x1UL << USB_INTRRX_EP15RXIF_Pos)
#define USB_INTRRX_EP15RXIF                 USB_INTRRX_EP15RXIF_Msk

/******************  Bit definition for USB_INTRTXE register  *****************/

/** @brief INTRTXE[0]: Endpoint 0 Interrupt Enable */
#define USB_INTRTXE_EP0IE_Pos               (0U)
#define USB_INTRTXE_EP0IE_Msk               (0x1UL << USB_INTRTXE_EP0IE_Pos)
#define USB_INTRTXE_EP0IE                   USB_INTRTXE_EP0IE_Msk
/** @brief INTRTXE[1]: Endpoint 1 TX Interrupt Enable */
#define USB_INTRTXE_EP1TXIE_Pos             (1U)
#define USB_INTRTXE_EP1TXIE_Msk             (0x1UL << USB_INTRTXE_EP1TXIE_Pos)
#define USB_INTRTXE_EP1TXIE                 USB_INTRTXE_EP1TXIE_Msk
/** @brief INTRTXE[2]: Endpoint 2 TX Interrupt Enable */
#define USB_INTRTXE_EP2TXIE_Pos             (2U)
#define USB_INTRTXE_EP2TXIE_Msk             (0x1UL << USB_INTRTXE_EP2TXIE_Pos)
#define USB_INTRTXE_EP2TXIE                 USB_INTRTXE_EP2TXIE_Msk
/** @brief INTRTXE[3]: Endpoint 3 TX Interrupt Enable */
#define USB_INTRTXE_EP3TXIE_Pos             (3U)
#define USB_INTRTXE_EP3TXIE_Msk             (0x1UL << USB_INTRTXE_EP3TXIE_Pos)
#define USB_INTRTXE_EP3TXIE                 USB_INTRTXE_EP3TXIE_Msk
/** @brief INTRTXE[4]: Endpoint 4 TX Interrupt Enable */
#define USB_INTRTXE_EP4TXIE_Pos             (4U)
#define USB_INTRTXE_EP4TXIE_Msk             (0x1UL << USB_INTRTXE_EP4TXIE_Pos)
#define USB_INTRTXE_EP4TXIE                 USB_INTRTXE_EP4TXIE_Msk
/** @brief INTRTXE[5]: Endpoint 5 TX Interrupt Enable */
#define USB_INTRTXE_EP5TXIE_Pos             (5U)
#define USB_INTRTXE_EP5TXIE_Msk             (0x1UL << USB_INTRTXE_EP5TXIE_Pos)
#define USB_INTRTXE_EP5TXIE                 USB_INTRTXE_EP5TXIE_Msk
/** @brief INTRTXE[6]: Endpoint 6 TX Interrupt Enable */
#define USB_INTRTXE_EP6TXIE_Pos             (6U)
#define USB_INTRTXE_EP6TXIE_Msk             (0x1UL << USB_INTRTXE_EP6TXIE_Pos)
#define USB_INTRTXE_EP6TXIE                 USB_INTRTXE_EP6TXIE_Msk
/** @brief INTRTXE[7]: Endpoint 7 TX Interrupt Enable */
#define USB_INTRTXE_EP7TXIE_Pos             (7U)
#define USB_INTRTXE_EP7TXIE_Msk             (0x1UL << USB_INTRTXE_EP7TXIE_Pos)
#define USB_INTRTXE_EP7TXIE                 USB_INTRTXE_EP7TXIE_Msk
/** @brief INTRTXE[8]: Endpoint 8 TX Interrupt Enable */
#define USB_INTRTXE_EP8TXIE_Pos             (8U)
#define USB_INTRTXE_EP8TXIE_Msk             (0x1UL << USB_INTRTXE_EP8TXIE_Pos)
#define USB_INTRTXE_EP8TXIE                 USB_INTRTXE_EP8TXIE_Msk
/** @brief INTRTXE[9]: Endpoint 9 TX Interrupt Enable */
#define USB_INTRTXE_EP9TXIE_Pos             (9U)
#define USB_INTRTXE_EP9TXIE_Msk             (0x1UL << USB_INTRTXE_EP9TXIE_Pos)
#define USB_INTRTXE_EP9TXIE                 USB_INTRTXE_EP9TXIE_Msk
/** @brief INTRTXE[10]: Endpoint 10 TX Interrupt Enable */
#define USB_INTRTXE_EP10TXIE_Pos            (10U)
#define USB_INTRTXE_EP10TXIE_Msk            (0x1UL << USB_INTRTXE_EP10TXIE_Pos)
#define USB_INTRTXE_EP10TXIE                USB_INTRTXE_EP10TXIE_Msk
/** @brief INTRTXE[11]: Endpoint 11 TX Interrupt Enable */
#define USB_INTRTXE_EP11TXIE_Pos            (11U)
#define USB_INTRTXE_EP11TXIE_Msk            (0x1UL << USB_INTRTXE_EP11TXIE_Pos)
#define USB_INTRTXE_EP11TXIE                USB_INTRTXE_EP11TXIE_Msk
/** @brief INTRTXE[12]: Endpoint 12 TX Interrupt Enable */
#define USB_INTRTXE_EP12TXIE_Pos            (12U)
#define USB_INTRTXE_EP12TXIE_Msk            (0x1UL << USB_INTRTXE_EP12TXIE_Pos)
#define USB_INTRTXE_EP12TXIE                USB_INTRTXE_EP12TXIE_Msk
/** @brief INTRTXE[13]: Endpoint 13 TX Interrupt Enable */
#define USB_INTRTXE_EP13TXIE_Pos            (13U)
#define USB_INTRTXE_EP13TXIE_Msk            (0x1UL << USB_INTRTXE_EP13TXIE_Pos)
#define USB_INTRTXE_EP13TXIE                USB_INTRTXE_EP13TXIE_Msk
/** @brief INTRTXE[14]: Endpoint 14 TX Interrupt Enable */
#define USB_INTRTXE_EP14TXIE_Pos            (14U)
#define USB_INTRTXE_EP14TXIE_Msk            (0x1UL << USB_INTRTXE_EP14TXIE_Pos)
#define USB_INTRTXE_EP14TXIE                USB_INTRTXE_EP14TXIE_Msk
/** @brief INTRTXE[15]: Endpoint 15 TX Interrupt Enable */
#define USB_INTRTXE_EP15TXIE_Pos            (15U)
#define USB_INTRTXE_EP15TXIE_Msk            (0x1UL << USB_INTRTXE_EP15TXIE_Pos)
#define USB_INTRTXE_EP15TXIE                USB_INTRTXE_EP15TXIE_Msk

/******************  Bit definition for USB_INTRRXE register  *****************/

/** @brief INTRRXE[1]: Endpoint 1 RX Interrupt Enable */
#define USB_INTRRXE_EP1RXIE_Pos             (1U)
#define USB_INTRRXE_EP1RXIE_Msk             (0x1UL << USB_INTRRXE_EP1RXIE_Pos)
#define USB_INTRRXE_EP1RXIE                 USB_INTRRXE_EP1RXIE_Msk
/** @brief INTRRXE[2]: Endpoint 2 RX Interrupt Enable */
#define USB_INTRRXE_EP2RXIE_Pos             (2U)
#define USB_INTRRXE_EP2RXIE_Msk             (0x1UL << USB_INTRRXE_EP2RXIE_Pos)
#define USB_INTRRXE_EP2RXIE                 USB_INTRRXE_EP2RXIE_Msk
/** @brief INTRRXE[3]: Endpoint 3 RX Interrupt Enable */
#define USB_INTRRXE_EP3RXIE_Pos             (3U)
#define USB_INTRRXE_EP3RXIE_Msk             (0x1UL << USB_INTRRXE_EP3RXIE_Pos)
#define USB_INTRRXE_EP3RXIE                 USB_INTRRXE_EP3RXIE_Msk
/** @brief INTRRXE[4]: Endpoint 4 RX Interrupt Enable */
#define USB_INTRRXE_EP4RXIE_Pos             (4U)
#define USB_INTRRXE_EP4RXIE_Msk             (0x1UL << USB_INTRRXE_EP4RXIE_Pos)
#define USB_INTRRXE_EP4RXIE                 USB_INTRRXE_EP4RXIE_Msk
/** @brief INTRRXE[5]: Endpoint 5 RX Interrupt Enable */
#define USB_INTRRXE_EP5RXIE_Pos             (5U)
#define USB_INTRRXE_EP5RXIE_Msk             (0x1UL << USB_INTRRXE_EP5RXIE_Pos)
#define USB_INTRRXE_EP5RXIE                 USB_INTRRXE_EP5RXIE_Msk
/** @brief INTRRXE[6]: Endpoint 6 RX Interrupt Enable */
#define USB_INTRRXE_EP6RXIE_Pos             (6U)
#define USB_INTRRXE_EP6RXIE_Msk             (0x1UL << USB_INTRRXE_EP6RXIE_Pos)
#define USB_INTRRXE_EP6RXIE                 USB_INTRRXE_EP6RXIE_Msk
/** @brief INTRRXE[7]: Endpoint 7 RX Interrupt Enable */
#define USB_INTRRXE_EP7RXIE_Pos             (7U)
#define USB_INTRRXE_EP7RXIE_Msk             (0x1UL << USB_INTRRXE_EP7RXIE_Pos)
#define USB_INTRRXE_EP7RXIE                 USB_INTRRXE_EP7RXIE_Msk
/** @brief INTRRXE[8]: Endpoint 8 RX Interrupt Enable */
#define USB_INTRRXE_EP8RXIE_Pos             (8U)
#define USB_INTRRXE_EP8RXIE_Msk             (0x1UL << USB_INTRRXE_EP8RXIE_Pos)
#define USB_INTRRXE_EP8RXIE                 USB_INTRRXE_EP8RXIE_Msk
/** @brief INTRRXE[9]: Endpoint 9 RX Interrupt Enable */
#define USB_INTRRXE_EP9RXIE_Pos             (9U)
#define USB_INTRRXE_EP9RXIE_Msk             (0x1UL << USB_INTRRXE_EP9RXIE_Pos)
#define USB_INTRRXE_EP9RXIE                 USB_INTRRXE_EP9RXIE_Msk
/** @brief INTRRXE[10]: Endpoint 10 RX Interrupt Enable */
#define USB_INTRRXE_EP10RXIE_Pos            (10U)
#define USB_INTRRXE_EP10RXIE_Msk            (0x1UL << USB_INTRRXE_EP10RXIE_Pos)
#define USB_INTRRXE_EP10RXIE                USB_INTRRXE_EP10RXIE_Msk
/** @brief INTRRXE[11]: Endpoint 11 RX Interrupt Enable */
#define USB_INTRRXE_EP11RXIE_Pos            (11U)
#define USB_INTRRXE_EP11RXIE_Msk            (0x1UL << USB_INTRRXE_EP11RXIE_Pos)
#define USB_INTRRXE_EP11RXIE                USB_INTRRXE_EP11RXIE_Msk
/** @brief INTRRXE[12]: Endpoint 12 RX Interrupt Enable */
#define USB_INTRRXE_EP12RXIE_Pos            (12U)
#define USB_INTRRXE_EP12RXIE_Msk            (0x1UL << USB_INTRRXE_EP12RXIE_Pos)
#define USB_INTRRXE_EP12RXIE                USB_INTRRXE_EP12RXIE_Msk
/** @brief INTRRXE[13]: Endpoint 13 RX Interrupt Enable */
#define USB_INTRRXE_EP13RXIE_Pos            (13U)
#define USB_INTRRXE_EP13RXIE_Msk            (0x1UL << USB_INTRRXE_EP13RXIE_Pos)
#define USB_INTRRXE_EP13RXIE                USB_INTRRXE_EP13RXIE_Msk
/** @brief INTRRXE[14]: Endpoint 14 RX Interrupt Enable */
#define USB_INTRRXE_EP14RXIE_Pos            (14U)
#define USB_INTRRXE_EP14RXIE_Msk            (0x1UL << USB_INTRRXE_EP14RXIE_Pos)
#define USB_INTRRXE_EP14RXIE                USB_INTRRXE_EP14RXIE_Msk
/** @brief INTRRXE[15]: Endpoint 15 RX Interrupt Enable */
#define USB_INTRRXE_EP15RXIE_Pos            (15U)
#define USB_INTRRXE_EP15RXIE_Msk            (0x1UL << USB_INTRRXE_EP15RXIE_Pos)
#define USB_INTRRXE_EP15RXIE                USB_INTRRXE_EP15RXIE_Msk

/******************  Bit definition for USB_INTRUSB register  *****************/

/** @brief INTRUSB[0]: Suspend Interrupt Flag */
#define USB_INTRUSB_SUSPIF_Pos              (0U)
#define USB_INTRUSB_SUSPIF_Msk              (0x1UL << USB_INTRUSB_SUSPIF_Pos)
#define USB_INTRUSB_SUSPIF                  USB_INTRUSB_SUSPIF_Msk
/** @brief INTRUSB[1]: Resume Interrupt Flag */
#define USB_INTRUSB_RESUMEIF_Pos            (1U)
#define USB_INTRUSB_RESUMEIF_Msk            (0x1UL << USB_INTRUSB_RESUMEIF_Pos)
#define USB_INTRUSB_RESUMEIF                USB_INTRUSB_RESUMEIF_Msk
/** @brief INTRUSB[2]: Reset/Babble Interrupt Flag */
#define USB_INTRUSB_RESETIF_Pos             (2U)
#define USB_INTRUSB_RESETIF_Msk             (0x1UL << USB_INTRUSB_RESETIF_Pos)
#define USB_INTRUSB_RESETIF                 USB_INTRUSB_RESETIF_Msk
/** @brief INTRUSB[3]: Start of Frame Interrupt Flag */
#define USB_INTRUSB_SOFIF_Pos               (3U)
#define USB_INTRUSB_SOFIF_Msk               (0x1UL << USB_INTRUSB_SOFIF_Pos)
#define USB_INTRUSB_SOFIF                   USB_INTRUSB_SOFIF_Msk
/** @brief INTRUSB[4]: Device Connection Interrupt Flag */
#define USB_INTRUSB_CONNIF_Pos              (4U)
#define USB_INTRUSB_CONNIF_Msk              (0x1UL << USB_INTRUSB_CONNIF_Pos)
#define USB_INTRUSB_CONNIF                  USB_INTRUSB_CONNIF_Msk
/** @brief INTRUSB[5]: Device Disconnect Interrupt Flag */
#define USB_INTRUSB_DISCONIF_Pos            (5U)
#define USB_INTRUSB_DISCONIF_Msk            (0x1UL << USB_INTRUSB_DISCONIF_Pos)
#define USB_INTRUSB_DISCONIF                USB_INTRUSB_DISCONIF_Msk
/** @brief INTRUSB[6]: Session Request Interrupt Flag */
#define USB_INTRUSB_SESSRQIF_Pos            (6U)
#define USB_INTRUSB_SESSRQIF_Msk            (0x1UL << USB_INTRUSB_SESSRQIF_Pos)
#define USB_INTRUSB_SESSRQIF                USB_INTRUSB_SESSRQIF_Msk
/** @brief INTRUSB[7]: VBUS Error Interrupt Flag */
#define USB_INTRUSB_VBUSERRIF_Pos           (7U)
#define USB_INTRUSB_VBUSERRIF_Msk           (0x1UL << USB_INTRUSB_VBUSERRIF_Pos)
#define USB_INTRUSB_VBUSERRIF               USB_INTRUSB_VBUSERRIF_Msk

/******************  Bit definition for USB_INTRUSBE register  ****************/

/** @brief INTRUSBE[0]: Suspend Interrupt Enable */
#define USB_INTRUSBE_SUSPIE_Pos              (0U)
#define USB_INTRUSBE_SUSPIE_Msk              (0x1UL << USB_INTRUSBE_SUSPIE_Pos)
#define USB_INTRUSBE_SUSPIE                  USB_INTRUSBE_SUSPIE_Msk
/** @brief INTRUSBE[1]: Resume Interrupt Enable */
#define USB_INTRUSBE_RESUMEIE_Pos            (1U)
#define USB_INTRUSBE_RESUMEIE_Msk            (0x1UL << USB_INTRUSBE_RESUMEIE_Pos)
#define USB_INTRUSBE_RESUMEIE                USB_INTRUSBE_RESUMEIE_Msk
/** @brief INTRUSBE[2]: Reset/Babble Interrupt Enable */
#define USB_INTRUSBE_RESETIE_Pos             (2U)
#define USB_INTRUSBE_RESETIE_Msk             (0x1UL << USB_INTRUSBE_RESETIE_Pos)
#define USB_INTRUSBE_RESETIE                 USB_INTRUSBE_RESETIE_Msk
/** @brief INTRUSBE[3]: Start of Frame Interrupt Enable */
#define USB_INTRUSBE_SOFIE_Pos               (3U)
#define USB_INTRUSBE_SOFIE_Msk               (0x1UL << USB_INTRUSBE_SOFIE_Pos)
#define USB_INTRUSBE_SOFIE                   USB_INTRUSBE_SOFIE_Msk
/** @brief INTRUSBE[4]: Device Connection Interrupt Enable */
#define USB_INTRUSBE_CONNIE_Pos              (4U)
#define USB_INTRUSBE_CONNIE_Msk              (0x1UL << USB_INTRUSBE_CONNIE_Pos)
#define USB_INTRUSBE_CONNIE                  USB_INTRUSBE_CONNIE_Msk
/** @brief INTRUSBE[5]: Device Disconnect Interrupt Enable */
#define USB_INTRUSBE_DISCONIE_Pos            (5U)
#define USB_INTRUSBE_DISCONIE_Msk            (0x1UL << USB_INTRUSBE_DISCONIE_Pos)
#define USB_INTRUSBE_DISCONIE                USB_INTRUSBE_DISCONIE_Msk
/** @brief INTRUSBE[6]: Session Request Interrupt Enable */
#define USB_INTRUSBE_SESSRQIE_Pos            (6U)
#define USB_INTRUSBE_SESSRQIE_Msk            (0x1UL << USB_INTRUSBE_SESSRQIE_Pos)
#define USB_INTRUSBE_SESSRQIE                USB_INTRUSBE_SESSRQIE_Msk
/** @brief INTRUSBE[7]: VBUS Error Interrupt Enable */
#define USB_INTRUSBE_VBUSERRIE_Pos           (7U)
#define USB_INTRUSBE_VBUSERRIE_Msk           (0x1UL << USB_INTRUSBE_VBUSERRIE_Pos)
#define USB_INTRUSBE_VBUSERRIE               USB_INTRUSBE_VBUSERRIE_Msk

/*******************  Bit definition for USB_FRAME register  ******************/

/** @brief FRAME[10:0]: Last received frame number */
#define USB_FRAME_RFRMNUM_Pos               (0U)
#define USB_FRAME_RFRMNUM_Msk               (0x7FFUL << USB_FRAME_RFRMNUM_Pos)
#define USB_FRAME_RFRMNUM                   USB_FRAME_RFRMNUM_Msk

/*******************  Bit definition for USB_INDEX register  ******************/

/** @brief INDEX[3:0]: Endpoint registers select */
#define USB_INDEX_ENDPOINT_Pos              (0U)
#define USB_INDEX_ENDPOINT_Msk              (0xFUL << USB_INDEX_ENDPOINT_Pos)
#define USB_INDEX_ENDPOINT                  USB_INDEX_ENDPOINT_Msk

/*****************  Bit definition for USB_TESTMODE register  *****************/

/** @brief TESTMODE[0]: Test_SE0_NAK test mode select */
#define USB_TESTMODE_NAK_Pos                (0U)
#define USB_TESTMODE_NAK_Msk                (0x1UL << USB_TESTMODE_NAK_Pos)
#define USB_TESTMODE_NAK                    USB_TESTMODE_NAK_Msk
/** @brief TESTMODE[1]: Test_J test mode select */
#define USB_TESTMODE_TESTJ_Pos              (1U)
#define USB_TESTMODE_TESTJ_Msk              (0x1UL << USB_TESTMODE_TESTJ_Pos)
#define USB_TESTMODE_TESTJ                  USB_TESTMODE_TESTJ_Msk
/** @brief TESTMODE[2]: Test_K test mode select */
#define USB_TESTMODE_TESTK_Pos              (2U)
#define USB_TESTMODE_TESTK_Msk              (0x1UL << USB_TESTMODE_TESTK_Pos)
#define USB_TESTMODE_TESTK                  USB_TESTMODE_TESTK_Msk
/** @brief TESTMODE[3]: Test_Packet test mode select */
#define USB_TESTMODE_PACKET_Pos             (3U)
#define USB_TESTMODE_PACKET_Msk             (0x1UL << USB_TESTMODE_PACKET_Pos)
#define USB_TESTMODE_PACKET                 USB_TESTMODE_PACKET_Msk
/** @brief TESTMODE[4]: Force High-Speed test mode select */
#define USB_TESTMODE_FORCEHS_Pos            (4U)
#define USB_TESTMODE_FORCEHS_Msk            (0x1UL << USB_TESTMODE_FORCEHS_Pos)
#define USB_TESTMODE_FORCEHS                USB_TESTMODE_FORCEHS_Msk
/** @brief TESTMODE[5]: Force Full-Speed test mode select */
#define USB_TESTMODE_FORCEFS_Pos            (5U)
#define USB_TESTMODE_FORCEFS_Msk            (0x1UL << USB_TESTMODE_FORCEFS_Pos)
#define USB_TESTMODE_FORCEFS                USB_TESTMODE_FORCEFS_Msk
/** @brief TESTMODE[6]: Endpoint 0 FIFO transfer test mode select */
#define USB_TESTMODE_FIFOACC_Pos            (6U)
#define USB_TESTMODE_FIFOACC_Msk            (0x1UL << USB_TESTMODE_FIFOACC_Pos)
#define USB_TESTMODE_FIFOACC                USB_TESTMODE_FIFOACC_Msk
/** @brief TESTMODE[7]: Force host mode select */
#define USB_TESTMODE_FORCEHST_Pos           (7U)
#define USB_TESTMODE_FORCEHST_Msk           (0x1UL << USB_TESTMODE_FORCEHST_Pos)
#define USB_TESTMODE_FORCEHST               USB_TESTMODE_FORCEHST_Msk

/*******************  Bit definition for USB_TXMAXP register  *****************/

/** @brief TXMAXP[10:0]: Maximum payload transmitted in a single transaction */
#define USB_TXMAXP_MAXP_Pos                 (0U)
#define USB_TXMAXP_MAXP_Msk                 (0x7FFUL << USB_TXMAXP_MAXP_Pos)
#define USB_TXMAXP_MAXP                     USB_TXMAXP_MAXP_Msk
/** @brief TXMAXP[15:11]: Maximum number of packets into which a single data packet should be split */
#define USB_TXMAXP_MULT_Pos                 (11U)
#define USB_TXMAXP_MULT_Msk                 (0x1FUL << USB_TXMAXP_MULT_Pos)
#define USB_TXMAXP_MULT                     USB_TXMAXP_MULT_Msk

/*******************  Bit definition for USB_CSR0L register  ******************/

/* Peripheral mode */

/** @brief CSR0L[0]: Data packet received */
#define USB_CSR0L_PERIPH_RXPKTRDY_Pos       (0U)
#define USB_CSR0L_PERIPH_RXPKTRDY_Msk       (0x1UL << USB_CSR0L_PERIPH_RXPKTRDY_Pos)
#define USB_CSR0L_PERIPH_RXPKTRDY           USB_CSR0L_PERIPH_RXPKTRDY_Msk
/** @brief CSR0L[1]: Data packet loaded into FIFO */
#define USB_CSR0L_PERIPH_TXPKTRDY_Pos       (1U)
#define USB_CSR0L_PERIPH_TXPKTRDY_Msk       (0x1UL << USB_CSR0L_PERIPH_TXPKTRDY_Pos)
#define USB_CSR0L_PERIPH_TXPKTRDY           USB_CSR0L_PERIPH_TXPKTRDY_Msk
/** @brief CSR0L[2]: STALL handshake is transmitted */
#define USB_CSR0L_PERIPH_SENTSTALL_Pos      (2U)
#define USB_CSR0L_PERIPH_SENTSTALL_Msk      (0x1UL << USB_CSR0L_PERIPH_SENTSTALL_Pos)
#define USB_CSR0L_PERIPH_SENTSTALL          USB_CSR0L_PERIPH_SENTSTALL_Msk
/** @brief CSR0L[3]: TxPktRdy bit set / RxPktRdy bit cleared */
#define USB_CSR0L_PERIPH_DATAEND_Pos        (3U)
#define USB_CSR0L_PERIPH_DATAEND_Msk        (0x1UL << USB_CSR0L_PERIPH_DATAEND_Pos)
#define USB_CSR0L_PERIPH_DATAEND            USB_CSR0L_PERIPH_DATAEND_Msk
/** @brief CSR0L[4]: Control transaction ended before DataEnd bit has been set */
#define USB_CSR0L_PERIPH_SETUPEND_Pos       (4U)
#define USB_CSR0L_PERIPH_SETUPEND_Msk       (0x1UL << USB_CSR0L_PERIPH_SETUPEND_Pos)
#define USB_CSR0L_PERIPH_SETUPEND           USB_CSR0L_PERIPH_SETUPEND_Msk
/** @brief CSR0L[5]: Terminate current transaction */
#define USB_CSR0L_PERIPH_SENDSTALL_Pos      (5U)
#define USB_CSR0L_PERIPH_SENDSTALL_Msk      (0x1UL << USB_CSR0L_PERIPH_SENDSTALL_Pos)
#define USB_CSR0L_PERIPH_SENDSTALL          USB_CSR0L_PERIPH_SENDSTALL_Msk
/** @brief CSR0L[6]: Clear RxPktRdy bit */
#define USB_CSR0L_PERIPH_SVCRPR_Pos         (6U)
#define USB_CSR0L_PERIPH_SVCRPR_Msk         (0x1UL << USB_CSR0L_PERIPH_SVCRPR_Pos)
#define USB_CSR0L_PERIPH_SVCRPR             USB_CSR0L_PERIPH_SVCRPR_Msk
/** @brief CSR0L[7]: Clear SetupEnd bit */
#define USB_CSR0L_PERIPH_SVSSETEND_Pos      (7U)
#define USB_CSR0L_PERIPH_SVSSETEND_Msk      (0x1UL << USB_CSR0L_PERIPH_SVSSETEND_Pos)
#define USB_CSR0L_PERIPH_SVSSETEND          USB_CSR0L_PERIPH_SVSSETEND_Msk

/* Host mode */

/** @brief CSR0L[0]: Data packet received */
#define USB_CSR0L_HOST_RXPKTRDY_Pos         (0U)
#define USB_CSR0L_HOST_RXPKTRDY_Msk         (0x1UL << USB_CSR0L_HOST_RXPKTRDY_Pos)
#define USB_CSR0L_HOST_RXPKTRDY             USB_CSR0L_HOST_RXPKTRDY_Msk
/** @brief CSR0L[1]: Data packet loaded into FIFO */
#define USB_CSR0L_HOST_TXPKTRDY_Pos         (1U)
#define USB_CSR0L_HOST_TXPKTRDY_Msk         (0x1UL << USB_CSR0L_HOST_TXPKTRDY_Pos)
#define USB_CSR0L_HOST_TXPKTRDY             USB_CSR0L_HOST_TXPKTRDY_Msk
/** @brief CSR0L[2]: STALL handshake is received */
#define USB_CSR0L_HOST_RXSTALL_Pos          (2U)
#define USB_CSR0L_HOST_RXSTALL_Msk          (0x1UL << USB_CSR0L_HOST_RXSTALL_Pos)
#define USB_CSR0L_HOST_RXSTALL              USB_CSR0L_HOST_RXSTALL_Msk
/** @brief CSR0L[3]: Send SETUP token instead of OUT token */
#define USB_CSR0L_HOST_SETUPPKT_Pos         (3U)
#define USB_CSR0L_HOST_SETUPPKT_Msk         (0x1UL << USB_CSR0L_HOST_SETUPPKT_Pos)
#define USB_CSR0L_HOST_SETUPPKT             USB_CSR0L_HOST_SETUPPKT_Msk
/** @brief CSR0L[4]: Three attempts to perform transaction with no response */
#define USB_CSR0L_HOST_ERROR_Pos            (4U)
#define USB_CSR0L_HOST_ERROR_Msk            (0x1UL << USB_CSR0L_HOST_ERROR_Pos)
#define USB_CSR0L_HOST_ERROR                USB_CSR0L_HOST_ERROR_Msk
/** @brief CSR0L[5]: IN transaction request */
#define USB_CSR0L_HOST_REQPKT_Pos           (5U)
#define USB_CSR0L_HOST_REQPKT_Msk           (0x1UL << USB_CSR0L_HOST_REQPKT_Pos)
#define USB_CSR0L_HOST_REQPKT               USB_CSR0L_HOST_REQPKT_Msk
/** @brief CSR0L[6]: Perform a status stage transaction */
#define USB_CSR0L_HOST_STATPKT_Pos          (6U)
#define USB_CSR0L_HOST_STATPKT_Msk          (0x1UL << USB_CSR0L_HOST_STATPKT_Pos)
#define USB_CSR0L_HOST_STATPKT              USB_CSR0L_HOST_STATPKT_Msk
/** @brief CSR0L[7]: EP 0 is halted following NAK timeout */
#define USB_CSR0L_HOST_NAKTMOUT_Pos         (7U)
#define USB_CSR0L_HOST_NAKTMOUT_Msk         (0x1UL << USB_CSR0L_HOST_NAKTMOUT_Pos)
#define USB_CSR0L_HOST_NAKTMOUT             USB_CSR0L_HOST_NAKTMOUT_Msk

/*******************  Bit definition for USB_CSR0H register  ******************/

/* Peripheral mode */

/** @brief CSR0H[0]: Flush the next packet from the EP 0 FIFO */
#define USB_CSR0H_PERIPH_FLSHFIFO_Pos       (0U)
#define USB_CSR0H_PERIPH_FLSHFIFO_Msk       (0x1UL << USB_CSR0H_PERIPH_FLSHFIFO_Pos)
#define USB_CSR0H_PERIPH_FLSHFIFO           USB_CSR0H_PERIPH_FLSHFIFO_Msk

/* Host mode */

/** @brief CSR0H[0]: Flush the next packet from the EP 0 FIFO */
#define USB_CSR0H_HOST_FLSHFIFO_Pos         (0U)
#define USB_CSR0H_HOST_FLSHFIFO_Msk         (0x1UL << USB_CSR0H_HOST_FLSHFIFO_Pos)
#define USB_CSR0H_HOST_FLSHFIFO             USB_CSR0H_HOST_FLSHFIFO_Msk
/** @brief CSR0H[1]: EP 0 data toggle state */
#define USB_CSR0H_HOST_DATATGGL_Pos         (1U)
#define USB_CSR0H_HOST_DATATGGL_Msk         (0x1UL << USB_CSR0H_HOST_DATATGGL_Pos)
#define USB_CSR0H_HOST_DATATGGL             USB_CSR0H_HOST_DATATGGL_Msk
/** @brief CSR0H[2]: Enable EP 0 data toggle state to be written */
#define USB_CSR0H_HOST_DTWREN_Pos           (2U)
#define USB_CSR0H_HOST_DTWREN_Msk           (0x1UL << USB_CSR0H_HOST_DTWREN_Pos)
#define USB_CSR0H_HOST_DTWREN               USB_CSR0H_HOST_DTWREN_Msk
/** @brief CSR0H[3]: Prevent issue of PING tokens in data and status phases */
#define USB_CSR0H_HOST_DISPING_Pos          (3U)
#define USB_CSR0H_HOST_DISPING_Msk          (0x1UL << USB_CSR0H_HOST_DISPING_Pos)
#define USB_CSR0H_HOST_DISPING              USB_CSR0H_HOST_DISPING_Msk

/******************  Bit definition for USB_TXCSRL register  ******************/

/* Peripheral mode */

/** @brief TXCSRL[0]: Data packet loaded into FIFO */
#define USB_TXCSRL_PERIPH_TXPKTRDY_Pos      (0U)
#define USB_TXCSRL_PERIPH_TXPKTRDY_Msk      (0x1UL << USB_TXCSRL_PERIPH_TXPKTRDY_Pos)
#define USB_TXCSRL_PERIPH_TXPKTRDY          USB_TXCSRL_PERIPH_TXPKTRDY_Msk
/** @brief TXCSRL[1]: At least 1 packet in the TX FIFO */
#define USB_TXCSRL_PERIPH_FIFONE_Pos        (1U)
#define USB_TXCSRL_PERIPH_FIFONE_Msk        (0x1UL << USB_TXCSRL_PERIPH_FIFONE_Pos)
#define USB_TXCSRL_PERIPH_FIFONE            USB_TXCSRL_PERIPH_FIFONE_Msk
/** @brief TXCSRL[2]: IN token received when TxPktRdy not set */
#define USB_TXCSRL_PERIPH_UNDERRUN_Pos      (2U)
#define USB_TXCSRL_PERIPH_UNDERRUN_Msk      (0x1UL << USB_TXCSRL_PERIPH_UNDERRUN_Pos)
#define USB_TXCSRL_PERIPH_UNDERRUN          USB_TXCSRL_PERIPH_UNDERRUN_Msk
/** @brief TXCSRL[3]: Flush the latest packet from the endpoint TX FIFO */
#define USB_TXCSRL_PERIPH_FLSHFIFO_Pos      (3U)
#define USB_TXCSRL_PERIPH_FLSHFIFO_Msk      (0x1UL << USB_TXCSRL_PERIPH_FLSHFIFO_Pos)
#define USB_TXCSRL_PERIPH_FLSHFIFO          USB_TXCSRL_PERIPH_FLSHFIFO_Msk
/** @brief TXCSRL[4]: Issue STALL handshake to IN token */
#define USB_TXCSRL_PERIPH_SENDSTALL_Pos     (4U)
#define USB_TXCSRL_PERIPH_SENDSTALL_Msk     (0x1UL << USB_TXCSRL_PERIPH_SENDSTALL_Pos)
#define USB_TXCSRL_PERIPH_SENDSTALL         USB_TXCSRL_PERIPH_SENDSTALL_Msk
/** @brief TXCSRL[5]: STALL handshake is transmitted */
#define USB_TXCSRL_PERIPH_SENTSTALL_Pos     (5U)
#define USB_TXCSRL_PERIPH_SENTSTALL_Msk     (0x1UL << USB_TXCSRL_PERIPH_SENTSTALL_Pos)
#define USB_TXCSRL_PERIPH_SENTSTALL         USB_TXCSRL_PERIPH_SENTSTALL_Msk
/** @brief TXCSRL[6]: Reset the endpoint data toggle to 0 */
#define USB_TXCSRL_PERIPH_CLRDT_Pos         (6U)
#define USB_TXCSRL_PERIPH_CLRDT_Msk         (0x1UL << USB_TXCSRL_PERIPH_CLRDT_Pos)
#define USB_TXCSRL_PERIPH_CLRDT             USB_TXCSRL_PERIPH_CLRDT_Msk
/** @brief TXCSRL[7]: Insufficient IN tokens for a split large packet */
#define USB_TXCSRL_PERIPH_INCOMPTX_Pos      (7U)
#define USB_TXCSRL_PERIPH_INCOMPTX_Msk      (0x1UL << USB_TXCSRL_PERIPH_INCOMPTX_Pos)
#define USB_TXCSRL_PERIPH_INCOMPTX          USB_TXCSRL_PERIPH_INCOMPTX_Msk

/* Host mode */

/** @brief TXCSRL[0]: Data packet loaded into FIFO */
#define USB_TXCSRL_HOST_TXPKTRDY_Pos        (0U)
#define USB_TXCSRL_HOST_TXPKTRDY_Msk        (0x1UL << USB_TXCSRL_HOST_TXPKTRDY_Pos)
#define USB_TXCSRL_HOST_TXPKTRDY            USB_TXCSRL_HOST_TXPKTRDY_Msk
/** @brief TXCSRL[1]: At least 1 packet in the TX FIFO */
#define USB_TXCSRL_HOST_FIFONE_Pos          (1U)
#define USB_TXCSRL_HOST_FIFONE_Msk          (0x1UL << USB_TXCSRL_HOST_FIFONE_Pos)
#define USB_TXCSRL_HOST_FIFONE              USB_TXCSRL_HOST_FIFONE_Msk
/** @brief TXCSRL[2]: Three attempts to send a packet with no handshake received */
#define USB_TXCSRL_HOST_ERROR_Pos           (2U)
#define USB_TXCSRL_HOST_ERROR_Msk           (0x1UL << USB_TXCSRL_HOST_ERROR_Pos)
#define USB_TXCSRL_HOST_ERROR               USB_TXCSRL_HOST_ERROR_Msk
/** @brief TXCSRL[3]: Flush the latest packet from the endpoint TX FIFO */
#define USB_TXCSRL_HOST_FLSHFIFO_Pos        (3U)
#define USB_TXCSRL_HOST_FLSHFIFO_Msk        (0x1UL << USB_TXCSRL_HOST_FLSHFIFO_Pos)
#define USB_TXCSRL_HOST_FLSHFIFO            USB_TXCSRL_HOST_FLSHFIFO_Msk
/** @brief TXCSRL[4]: Send SETUP token instead of OUT token */
#define USB_TXCSRL_HOST_SETUPPKT_Pos        (4U)
#define USB_TXCSRL_HOST_SETUPPKT_Msk        (0x1UL << USB_TXCSRL_HOST_SETUPPKT_Pos)
#define USB_TXCSRL_HOST_SETUPPKT            USB_TXCSRL_HOST_SETUPPKT_Msk
/** @brief TXCSRL[5]: STALL handshake is received */
#define USB_TXCSRL_HOST_RXSTALL_Pos         (5U)
#define USB_TXCSRL_HOST_RXSTALL_Msk         (0x1UL << USB_TXCSRL_HOST_RXSTALL_Pos)
#define USB_TXCSRL_HOST_RXSTALL             USB_TXCSRL_HOST_RXSTALL_Msk
/** @brief TXCSRL[6]: Reset the endpoint data toggle to 0 */
#define USB_TXCSRL_HOST_CLRDT_Pos           (6U)
#define USB_TXCSRL_HOST_CLRDT_Msk           (0x1UL << USB_TXCSRL_HOST_CLRDT_Pos)
#define USB_TXCSRL_HOST_CLRDT               USB_TXCSRL_HOST_CLRDT_Msk
/** @brief TXCSRL[7]: No response received from the device */
#define USB_TXCSRL_HOST_INCOMPTX_Pos        (7U)
#define USB_TXCSRL_HOST_INCOMPTX_Msk        (0x1UL << USB_TXCSRL_HOST_INCOMPTX_Pos)
#define USB_TXCSRL_HOST_INCOMPTX            USB_TXCSRL_HOST_INCOMPTX_Msk

/******************  Bit definition for USB_TXCSRH register  ******************/

/* Peripheral mode */

/** @brief TXCSRH[2]: Select DMA Request Mode */
#define USB_TXCSRH_PERIPH_DMAREQMD_Pos      (2U)
#define USB_TXCSRH_PERIPH_DMAREQMD_Msk      (0x1UL << USB_TXCSRH_PERIPH_DMAREQMD_Pos)
#define USB_TXCSRH_PERIPH_DMAREQMD          USB_TXCSRH_PERIPH_DMAREQMD_Msk
/** @brief TXCSRH[3]: Force the endpoint data toggle to switch */
#define USB_TXCSRH_PERIPH_FRCDATTG_Pos      (3U)
#define USB_TXCSRH_PERIPH_FRCDATTG_Msk      (0x1UL << USB_TXCSRH_PERIPH_FRCDATTG_Pos)
#define USB_TXCSRH_PERIPH_FRCDATTG          USB_TXCSRH_PERIPH_FRCDATTG_Msk
/** @brief TXCSRH[4]: Enable the DMA request for the TX endpoint */
#define USB_TXCSRH_PERIPH_DMAREQEN_Pos      (4U)
#define USB_TXCSRH_PERIPH_DMAREQEN_Msk      (0x1UL << USB_TXCSRH_PERIPH_DMAREQEN_Pos)
#define USB_TXCSRH_PERIPH_DMAREQEN          USB_TXCSRH_PERIPH_DMAREQEN_Msk
/** @brief TXCSRH[5]: Set the endpoint direction */
#define USB_TXCSRH_PERIPH_MODE_Pos          (5U)
#define USB_TXCSRH_PERIPH_MODE_Msk          (0x1UL << USB_TXCSRH_PERIPH_MODE_Pos)
#define USB_TXCSRH_PERIPH_MODE              USB_TXCSRH_PERIPH_MODE_Msk
/** @brief TXCSRH[6]: Set the endpoint transfer type */
#define USB_TXCSRH_PERIPH_ISO_Pos           (6U)
#define USB_TXCSRH_PERIPH_ISO_Msk           (0x1UL << USB_TXCSRH_PERIPH_ISO_Pos)
#define USB_TXCSRH_PERIPH_ISO               USB_TXCSRH_PERIPH_ISO_Msk
/** @brief TXCSRH[7]: Set TxPktRdy bit automatically */
#define USB_TXCSRH_PERIPH_AUTOSET_Pos       (7U)
#define USB_TXCSRH_PERIPH_AUTOSET_Msk       (0x1UL << USB_TXCSRH_PERIPH_AUTOSET_Pos)
#define USB_TXCSRH_PERIPH_AUTOSET           USB_TXCSRH_PERIPH_AUTOSET_Msk

/* Host mode */

/** @brief TXCSRH[0]: TX endpoint data toggle state */
#define USB_TXCSRH_HOST_DATATGGL_Pos        (0U)
#define USB_TXCSRH_HOST_DATATGGL_Msk        (0x1UL << USB_TXCSRH_HOST_DATATGGL_Pos)
#define USB_TXCSRH_HOST_DATATGGL            USB_TXCSRH_HOST_DATATGGL_Msk
/** @brief TXCSRH[1]: TX endpoint data toggle state write enable */
#define USB_TXCSRH_HOST_DTWREN_Pos          (1U)
#define USB_TXCSRH_HOST_DTWREN_Msk          (0x1UL << USB_TXCSRH_HOST_DTWREN_Pos)
#define USB_TXCSRH_HOST_DTWREN              USB_TXCSRH_HOST_DTWREN_Msk
/** @brief TXCSRH[2]: Select DMA Request Mode */
#define USB_TXCSRH_HOST_DMAREQMD_Pos        (2U)
#define USB_TXCSRH_HOST_DMAREQMD_Msk        (0x1UL << USB_TXCSRH_HOST_DMAREQMD_Pos)
#define USB_TXCSRH_HOST_DMAREQMD            USB_TXCSRH_HOST_DMAREQMD_Msk
/** @brief TXCSRH[3]: Force the endpoint data toggle to switch */
#define USB_TXCSRH_HOST_FRCDATTG_Pos        (3U)
#define USB_TXCSRH_HOST_FRCDATTG_Msk        (0x1UL << USB_TXCSRH_HOST_FRCDATTG_Pos)
#define USB_TXCSRH_HOST_FRCDATTG            USB_TXCSRH_HOST_FRCDATTG_Msk
/** @brief TXCSRH[4]: Enable the DMA request for the TX endpoint */
#define USB_TXCSRH_HOST_DMAREQEN_Pos        (4U)
#define USB_TXCSRH_HOST_DMAREQEN_Msk        (0x1UL << USB_TXCSRH_HOST_DMAREQEN_Pos)
#define USB_TXCSRH_HOST_DMAREQEN            USB_TXCSRH_HOST_DMAREQEN_Msk
/** @brief TXCSRH[5]: Set the endpoint direction */
#define USB_TXCSRH_HOST_MODE_Pos            (5U)
#define USB_TXCSRH_HOST_MODE_Msk            (0x1UL << USB_TXCSRH_HOST_MODE_Pos)
#define USB_TXCSRH_HOST_MODE                USB_TXCSRH_HOST_MODE_Msk
/** @brief TXCSRH[7]: Set TxPktRdy bit automatically */
#define USB_TXCSRH_HOST_AUTOSET_Pos         (7U)
#define USB_TXCSRH_HOST_AUTOSET_Msk         (0x1UL << USB_TXCSRH_HOST_AUTOSET_Pos)
#define USB_TXCSRH_HOST_AUTOSET             USB_TXCSRH_HOST_AUTOSET_Msk

/*******************  Bit definition for USB_RXMAXP register  *****************/

/** @brief RXMAXP[10:0]: Maximum payload transmitted in a single transaction */
#define USB_RXMAXP_MAXP_Pos                 (0U)
#define USB_RXMAXP_MAXP_Msk                 (0x7FFUL << USB_RXMAXP_MAXP_Pos)
#define USB_RXMAXP_MAXP                     USB_RXMAXP_MAXP_Msk
/** @brief RXMAXP[15:11]: The number of packets to be amalgamated into a single data packet */
#define USB_RXMAXP_MULT_Pos                 (11U)
#define USB_RXMAXP_MULT_Msk                 (0x1FUL << USB_RXMAXP_MULT_Pos)
#define USB_RXMAXP_MULT                     USB_RXMAXP_MULT_Msk

/******************  Bit definition for USB_RXCSRL register  ******************/

/* Peripheral mode */

/** @brief RXCSRL[0]: Data packet has been received */
#define USB_RXCSRL_PERIPH_RXPKTRDY_Pos      (0U)
#define USB_RXCSRL_PERIPH_RXPKTRDY_Msk      (0x1UL << USB_RXCSRL_PERIPH_RXPKTRDY_Pos)
#define USB_RXCSRL_PERIPH_RXPKTRDY          USB_RXCSRL_PERIPH_RXPKTRDY_Msk
/** @brief RXCSRL[1]: No more packets can be loaded into the RX FIFO */
#define USB_RXCSRL_PERIPH_FIFOFULL_Pos      (1U)
#define USB_RXCSRL_PERIPH_FIFOFULL_Msk      (0x1UL << USB_RXCSRL_PERIPH_FIFOFULL_Pos)
#define USB_RXCSRL_PERIPH_FIFOFULL          USB_RXCSRL_PERIPH_FIFOFULL_Msk
/** @brief RXCSRL[2]: OUT packet cannot be loaded into the RX FIFO */
#define USB_RXCSRL_PERIPH_OVERRUN_Pos       (2U)
#define USB_RXCSRL_PERIPH_OVERRUN_Msk       (0x1UL << USB_RXCSRL_PERIPH_OVERRUN_Pos)
#define USB_RXCSRL_PERIPH_OVERRUN           USB_RXCSRL_PERIPH_OVERRUN_Msk
/** @brief RXCSRL[3]: Data packet has CRC or bit-stuff error */
#define USB_RXCSRL_PERIPH_DATAERR_Pos       (3U)
#define USB_RXCSRL_PERIPH_DATAERR_Msk       (0x1UL << USB_RXCSRL_PERIPH_DATAERR_Pos)
#define USB_RXCSRL_PERIPH_DATAERR           USB_RXCSRL_PERIPH_DATAERR_Msk
/** @brief RXCSRL[4]: Flush the next packet to be read from the EP RX FIFO */
#define USB_RXCSRL_PERIPH_FLSHFIFO_Pos      (4U)
#define USB_RXCSRL_PERIPH_FLSHFIFO_Msk      (0x1UL << USB_RXCSRL_PERIPH_FLSHFIFO_Pos)
#define USB_RXCSRL_PERIPH_FLSHFIFO          USB_RXCSRL_PERIPH_FLSHFIFO_Msk
/** @brief RXCSRL[5]: Issue STALL handshake */
#define USB_RXCSRL_PERIPH_SENDSTALL_Pos     (5U)
#define USB_RXCSRL_PERIPH_SENDSTALL_Msk     (0x1UL << USB_RXCSRL_PERIPH_SENDSTALL_Pos)
#define USB_RXCSRL_PERIPH_SENDSTALL         USB_RXCSRL_PERIPH_SENDSTALL_Msk
/** @brief RXCSRL[6]: STALL handshake is transmitted */
#define USB_RXCSRL_PERIPH_SENTSTALL_Pos     (6U)
#define USB_RXCSRL_PERIPH_SENTSTALL_Msk     (0x1UL << USB_RXCSRL_PERIPH_SENTSTALL_Pos)
#define USB_RXCSRL_PERIPH_SENTSTALL         USB_RXCSRL_PERIPH_SENTSTALL_Msk
/** @brief RXCSRL[7]: Reset the endpoint data toggle to 0 */
#define USB_RXCSRL_PERIPH_CLRDT_Pos         (7U)
#define USB_RXCSRL_PERIPH_CLRDT_Msk         (0x1UL << USB_RXCSRL_PERIPH_CLRDT_Pos)
#define USB_RXCSRL_PERIPH_CLRDT             USB_RXCSRL_PERIPH_CLRDT_Msk

/* Host mode */

/** @brief RXCSRL[0]: Data packet has been received */
#define USB_RXCSRL_HOST_RXPKTRDY_Pos        (0U)
#define USB_RXCSRL_HOST_RXPKTRDY_Msk        (0x1UL << USB_RXCSRL_HOST_RXPKTRDY_Pos)
#define USB_RXCSRL_HOST_RXPKTRDY            USB_RXCSRL_HOST_RXPKTRDY_Msk
/** @brief RXCSRL[1]: No more packets can be loaded into the RX FIFO */
#define USB_RXCSRL_HOST_FIFOFULL_Pos        (1U)
#define USB_RXCSRL_HOST_FIFOFULL_Msk        (0x1UL << USB_RXCSRL_HOST_FIFOFULL_Pos)
#define USB_RXCSRL_HOST_FIFOFULL            USB_RXCSRL_HOST_FIFOFULL_Msk
/** @brief RXCSRL[2]: No data packet has been received after three attempts */
#define USB_RXCSRL_HOST_ERROR_Pos           (2U)
#define USB_RXCSRL_HOST_ERROR_Msk           (0x1UL << USB_RXCSRL_HOST_ERROR_Pos)
#define USB_RXCSRL_HOST_ERROR               USB_RXCSRL_HOST_ERROR_Msk
/** @brief RXCSRL[3]: CRC or bit-stuff error / NAK timeout */
#define USB_RXCSRL_HOST_DERRNAKT_Pos        (3U)
#define USB_RXCSRL_HOST_DERRNAKT_Msk        (0x1UL << USB_RXCSRL_HOST_DERRNAKT_Pos)
#define USB_RXCSRL_HOST_DERRNAKT            USB_RXCSRL_HOST_DERRNAKT_Msk
/** @brief RXCSRL[4]: Flush the next packet to be read from the EP RX FIFO */
#define USB_RXCSRL_HOST_FLSHFIFO_Pos        (4U)
#define USB_RXCSRL_HOST_FLSHFIFO_Msk        (0x1UL << USB_RXCSRL_HOST_FLSHFIFO_Pos)
#define USB_RXCSRL_HOST_FLSHFIFO            USB_RXCSRL_HOST_FLSHFIFO_Msk
/** @brief RXCSRL[5]: IN transaction request */
#define USB_RXCSRL_HOST_REQPKT_Pos          (5U)
#define USB_RXCSRL_HOST_REQPKT_Msk          (0x1UL << USB_RXCSRL_HOST_REQPKT_Pos)
#define USB_RXCSRL_HOST_REQPKT              USB_RXCSRL_HOST_REQPKT_Msk
/** @brief RXCSRL[6]: STALL handshake is received */
#define USB_RXCSRL_HOST_RXSTALL_Pos         (6U)
#define USB_RXCSRL_HOST_RXSTALL_Msk         (0x1UL << USB_RXCSRL_HOST_RXSTALL_Pos)
#define USB_RXCSRL_HOST_RXSTALL             USB_RXCSRL_HOST_RXSTALL_Msk
/** @brief RXCSRL[7]: Reset the endpoint data toggle to 0 */
#define USB_RXCSRL_HOST_CLRDT_Pos           (7U)
#define USB_RXCSRL_HOST_CLRDT_Msk           (0x1UL << USB_RXCSRL_HOST_CLRDT_Pos)
#define USB_RXCSRL_HOST_CLRDT               USB_RXCSRL_HOST_CLRDT_Msk

/******************  Bit definition for USB_RXCSRH register  ******************/

/* Peripheral mode */

/** @brief RXCSRH[0]: The packet in RX FIFO is incomplete */
#define USB_RXCSRH_PERIPH_INCOMPRX_Pos      (0U)
#define USB_RXCSRH_PERIPH_INCOMPRX_Msk      (0x1UL << USB_RXCSRH_PERIPH_INCOMPRX_Pos)
#define USB_RXCSRH_PERIPH_INCOMPRX          USB_RXCSRH_PERIPH_INCOMPRX_Msk
/** @brief RXCSRH[3]: Select DMA Request Mode */
#define USB_RXCSRH_PERIPH_DMAREQMD_Pos      (3U)
#define USB_RXCSRH_PERIPH_DMAREQMD_Msk      (0x1UL << USB_RXCSRH_PERIPH_DMAREQMD_Pos)
#define USB_RXCSRH_PERIPH_DMAREQMD          USB_RXCSRH_PERIPH_DMAREQMD_Msk
/** @brief RXCSRH[4]: Disable sending of NYET handshakes */
#define USB_RXCSRH_PERIPH_DISNYET_Pos       (4U)
#define USB_RXCSRH_PERIPH_DISNYET_Msk       (0x1UL << USB_RXCSRH_PERIPH_DISNYET_Pos)
#define USB_RXCSRH_PERIPH_DISNYET           USB_RXCSRH_PERIPH_DISNYET_Msk
/** @brief RXCSRH[5]: Enable DMA request for the RX endpoint */
#define USB_RXCSRH_PERIPH_DMAREQEN_Pos      (5U)
#define USB_RXCSRH_PERIPH_DMAREQEN_Msk      (0x1UL << USB_RXCSRH_PERIPH_DMAREQEN_Pos)
#define USB_RXCSRH_PERIPH_DMAREQEN          USB_RXCSRH_PERIPH_DMAREQEN_Msk
/** @brief RXCSRH[6]: Set the endpoint transfer type */
#define USB_RXCSRH_PERIPH_ISO_Pos           (6U)
#define USB_RXCSRH_PERIPH_ISO_Msk           (0x1UL << USB_RXCSRH_PERIPH_ISO_Pos)
#define USB_RXCSRH_PERIPH_ISO               USB_RXCSRH_PERIPH_ISO_Msk
/** @brief RXCSRH[7]: Clear RxPktRdy bit automatically */
#define USB_RXCSRH_PERIPH_AUTOCLR_Pos       (7U)
#define USB_RXCSRH_PERIPH_AUTOCLR_Msk       (0x1UL << USB_RXCSRH_PERIPH_AUTOCLR_Pos)
#define USB_RXCSRH_PERIPH_AUTOCLR           USB_RXCSRH_PERIPH_AUTOCLR_Msk

/* Host mode */

/** @brief RXCSRH[0]: The packet received is incomplete */
#define USB_RXCSRH_HOST_INCOMPRX_Pos        (0U)
#define USB_RXCSRH_HOST_INCOMPRX_Msk        (0x1UL << USB_RXCSRH_HOST_INCOMPRX_Pos)
#define USB_RXCSRH_HOST_INCOMPRX            USB_RXCSRH_HOST_INCOMPRX_Msk
/** @brief RXCSRH[1]: State of the EP 0 data toggle */
#define USB_RXCSRH_HOST_DATATGGL_Pos        (1U)
#define USB_RXCSRH_HOST_DATATGGL_Msk        (0x1UL << USB_RXCSRH_HOST_DATATGGL_Pos)
#define USB_RXCSRH_HOST_DATATGGL            USB_RXCSRH_HOST_DATATGGL_Msk
/** @brief RXCSRH[2]: Enable state of the EP 0 data toggle to be written */
#define USB_RXCSRH_HOST_DATATWEN_Pos        (2U)
#define USB_RXCSRH_HOST_DATATWEN_Msk        (0x1UL << USB_RXCSRH_HOST_DATATWEN_Pos)
#define USB_RXCSRH_HOST_DATATWEN            USB_RXCSRH_HOST_DATATWEN_Msk
/** @brief RXCSRH[3]: Select DMA Request Mode */
#define USB_RXCSRH_HOST_DMAREQMD_Pos        (3U)
#define USB_RXCSRH_HOST_DMAREQMD_Msk        (0x1UL << USB_RXCSRH_HOST_DMAREQMD_Pos)
#define USB_RXCSRH_HOST_DMAREQMD            USB_RXCSRH_HOST_DMAREQMD_Msk
/** @brief RXCSRH[4]: PID error in the received packet */
#define USB_RXCSRH_HOST_PIDERR_Pos          (4U)
#define USB_RXCSRH_HOST_PIDERR_Msk          (0x1UL << USB_RXCSRH_HOST_PIDERR_Pos)
#define USB_RXCSRH_HOST_PIDERR              USB_RXCSRH_HOST_PIDERR_Msk
/** @brief RXCSRH[5]: Enable DMA request for the RX endpoint */
#define USB_RXCSRH_HOST_DMAREQEN_Pos        (5U)
#define USB_RXCSRH_HOST_DMAREQEN_Msk        (0x1UL << USB_RXCSRH_HOST_DMAREQEN_Pos)
#define USB_RXCSRH_HOST_DMAREQEN            USB_RXCSRH_HOST_DMAREQEN_Msk
/** @brief RXCSRH[6]: Set ReqPkt bit automatically */
#define USB_RXCSRH_HOST_AUTOREQ_Pos         (6U)
#define USB_RXCSRH_HOST_AUTOREQ_Msk         (0x1UL << USB_RXCSRH_HOST_AUTOREQ_Pos)
#define USB_RXCSRH_HOST_AUTOREQ             USB_RXCSRH_HOST_AUTOREQ_Msk
/** @brief RXCSRH[7]: Clear RxPktRdy bit automatically */
#define USB_RXCSRH_HOST_AUTOCLR_Pos         (7U)
#define USB_RXCSRH_HOST_AUTOCLR_Msk         (0x1UL << USB_RXCSRH_HOST_AUTOCLR_Pos)
#define USB_RXCSRH_HOST_AUTOCLR             USB_RXCSRH_HOST_AUTOCLR_Msk

/******************  Bit definition for USB_COUNT0 register  ******************/

/** @brief COUNT0[6:0]: The number of received data bytes in the EP 0 FIFO */
#define USB_COUNT0_EP0RXCNT_Pos             (0U)
#define USB_COUNT0_EP0RXCNT_Msk             (0x7FUL << USB_COUNT0_EP0RXCNT_Pos)
#define USB_COUNT0_EP0RXCNT                 USB_COUNT0_EP0RXCNT_Msk

/******************  Bit definition for USB_RXCOUNT register  *****************/

/** @brief RXCOUNT[13:0]: The number of data bytes to be read from RX FIFO */
#define USB_RXCOUNT_EPRXCNT_Pos             (0U)
#define USB_RXCOUNT_EPRXCNT_Msk             (0x3FFFUL << USB_RXCOUNT_EPRXCNT_Pos)
#define USB_RXCOUNT_EPRXCNT                 USB_RXCOUNT_EPRXCNT_Msk

/*******************  Bit definition for USB_TYPE0 register  ******************/

/** @brief TYPE0[7:6]: The operating speed of the targeted device */
#define USB_TYPE0_SPEED_Pos                 (6U)
#define USB_TYPE0_SPEED_Msk                 (0x3UL << USB_TYPE0_SPEED_Pos)
#define USB_TYPE0_SPEED                     USB_TYPE0_SPEED_Msk

/******************  Bit definition for USB_TXTYPE register  ******************/

/** @brief TXTYPE[3:0]: EP number from the TX EP descriptor returned during enumeration */
#define USB_TXTYPE_TEP_Pos                  (0U)
#define USB_TXTYPE_TEP_Msk                  (0xFUL << USB_TXTYPE_TEP_Pos)
#define USB_TXTYPE_TEP                      USB_TXTYPE_TEP_Msk
/** @brief TXTYPE[5:4]: Protocol for the TX endpoint */
#define USB_TXTYPE_PROTOCOL_Pos             (4U)
#define USB_TXTYPE_PROTOCOL_Msk             (0x3UL << USB_TXTYPE_PROTOCOL_Pos)
#define USB_TXTYPE_PROTOCOL                 USB_TXTYPE_PROTOCOL_Msk
/** @brief TXTYPE[7:6]: The operating speed of the target device */
#define USB_TXTYPE_SPEED_Pos                (6U)
#define USB_TXTYPE_SPEED_Msk                (0x3UL << USB_TXTYPE_SPEED_Pos)
#define USB_TXTYPE_SPEED                    USB_TXTYPE_SPEED_Msk

/*****************  Bit definition for USB_NAKLIMIT0 register  ****************/

/** @brief NAKLIMIT0[4:0]: The number of NAK responses received for the EP 0 to timeout */
#define USB_NAKLIMIT0_EP0NAKLIM_Pos         (0U)
#define USB_NAKLIMIT0_EP0NAKLIM_Msk         (0x1FUL << USB_NAKLIMIT0_EP0NAKLIM_Pos)
#define USB_NAKLIMIT0_EP0NAKLIM             USB_NAKLIMIT0_EP0NAKLIM_Msk

/****************  Bit definition for USB_TXINTERVAL register  ****************/

/** @brief TXINTERVAL[7:0]: Polling interval for TX endpoint */
#define USB_TXINTERVAL_TXINTERV_Pos         (0U)
#define USB_TXINTERVAL_TXINTERV_Msk         (0xFFUL << USB_TXINTERVAL_TXINTERV_Pos)
#define USB_TXINTERVAL_TXINTERV             USB_TXINTERVAL_TXINTERV_Msk

/******************  Bit definition for USB_RXTYPE register  ******************/

/** @brief RXTYPE[3:0]: EP number from the RX EP descriptor returned during enumeration */
#define USB_RXTYPE_TEP_Pos                  (0U)
#define USB_RXTYPE_TEP_Msk                  (0xFUL << USB_RXTYPE_TEP_Pos)
#define USB_RXTYPE_TEP                      USB_RXTYPE_TEP_Msk
/** @brief RXTYPE[5:4]: Protocol for the RX endpoint */
#define USB_RXTYPE_PROTOCOL_Pos             (4U)
#define USB_RXTYPE_PROTOCOL_Msk             (0x3UL << USB_RXTYPE_PROTOCOL_Pos)
#define USB_RXTYPE_PROTOCOL                 USB_RXTYPE_PROTOCOL_Msk
/** @brief RXTYPE[7:6]: The operating speed of the target device */
#define USB_RXTYPE_SPEED_Pos                (6U)
#define USB_RXTYPE_SPEED_Msk                (0x3UL << USB_RXTYPE_SPEED_Pos)
#define USB_RXTYPE_SPEED                    USB_RXTYPE_SPEED_Msk

/****************  Bit definition for USB_RXINTERVAL register  ****************/

/** @brief RXINTERVAL[7:0]: Polling interval for RX endpoint */
#define USB_RXINTERVAL_RXINTERV_Pos         (0U)
#define USB_RXINTERVAL_RXINTERV_Msk         (0xFFUL << USB_RXINTERVAL_RXINTERV_Pos)
#define USB_RXINTERVAL_RXINTERV             USB_RXINTERVAL_RXINTERV_Msk

/****************  Bit definition for USB_CONFIGDATA register  ****************/

/** @brief CONFIGDATA[0]: UTMI data width */
#define USB_CONFIGDATA_UTMIDW_Pos           (0U)
#define USB_CONFIGDATA_UTMIDW_Msk           (0x1UL << USB_CONFIGDATA_UTMIDW_Pos)
#define USB_CONFIGDATA_UTMIDW               USB_CONFIGDATA_UTMIDW_Msk
/** @brief CONFIGDATA[1]: Soft Connect/Disconnect */
#define USB_CONFIGDATA_SOFTCONE_Pos         (1U)
#define USB_CONFIGDATA_SOFTCONE_Msk         (0x1UL << USB_CONFIGDATA_SOFTCONE_Pos)
#define USB_CONFIGDATA_SOFTCONE             USB_CONFIGDATA_SOFTCONE_Msk
/** @brief CONFIGDATA[2]: Dynamic FIFO Sizing option */
#define USB_CONFIGDATA_DYNFIFOSZ_Pos        (2U)
#define USB_CONFIGDATA_DYNFIFOSZ_Msk        (0x1UL << USB_CONFIGDATA_DYNFIFOSZ_Pos)
#define USB_CONFIGDATA_DYNFIFOSZ            USB_CONFIGDATA_DYNFIFOSZ_Msk
/** @brief CONFIGDATA[3]: High-bandwidth TX ISO Endpoint Support */
#define USB_CONFIGDATA_HBTXE_Pos            (3U)
#define USB_CONFIGDATA_HBTXE_Msk            (0x1UL << USB_CONFIGDATA_HBTXE_Pos)
#define USB_CONFIGDATA_HBTXE                USB_CONFIGDATA_HBTXE_Msk
/** @brief CONFIGDATA[4]: High-bandwidth RX ISO Endpoint Support */
#define USB_CONFIGDATA_HBRXE_Pos            (4U)
#define USB_CONFIGDATA_HBRXE_Msk            (0x1UL << USB_CONFIGDATA_HBRXE_Pos)
#define USB_CONFIGDATA_HBRXE                USB_CONFIGDATA_HBRXE_Msk
/** @brief CONFIGDATA[5]: Indicates Little Endian ordering. Always 0. */
#define USB_CONFIGDATA_BIGENDIAN_Pos        (5U)
#define USB_CONFIGDATA_BIGENDIAN_Msk        (0x1UL << USB_CONFIGDATA_BIGENDIAN_Pos)
#define USB_CONFIGDATA_BIGENDIAN            USB_CONFIGDATA_BIGENDIAN_Msk
/** @brief CONFIGDATA[6]: Automatic splitting of bulk packets */
#define USB_CONFIGDATA_MPTXE_Pos            (6U)
#define USB_CONFIGDATA_MPTXE_Msk            (0x1UL << USB_CONFIGDATA_MPTXE_Pos)
#define USB_CONFIGDATA_MPTXE                USB_CONFIGDATA_MPTXE_Msk
/** @brief CONFIGDATA[7]: Automatic amalgamation of bulk packets */
#define USB_CONFIGDATA_MPRXE_Pos            (7U)
#define USB_CONFIGDATA_MPRXE_Msk            (0x1UL << USB_CONFIGDATA_MPRXE_Pos)
#define USB_CONFIGDATA_MPRXE                USB_CONFIGDATA_MPRXE_Msk

/*****************  Bit definition for USB_FIFOSIZE register  *****************/

/** @brief FIFOSIZE[3:0]: TX endpoint FIFO size */
#define USB_FIFOSIZE_TXFIFOSZ_Pos           (0U)
#define USB_FIFOSIZE_TXFIFOSZ_Msk           (0xFUL << USB_FIFOSIZE_TXFIFOSZ_Pos)
#define USB_FIFOSIZE_TXFIFOSZ               USB_FIFOSIZE_TXFIFOSZ_Msk
/** @brief FIFOSIZE[7:4]: RX endpoint FIFO size */
#define USB_FIFOSIZE_RXFIFOSZ_Pos           (4U)
#define USB_FIFOSIZE_RXFIFOSZ_Msk           (0xFUL << USB_FIFOSIZE_RXFIFOSZ_Pos)
#define USB_FIFOSIZE_RXFIFOSZ               USB_FIFOSIZE_RXFIFOSZ_Msk

/******************  Bit definition for USB_DEVCTL register  ******************/

/** @brief DEVCTL[0]: Start/End of session */
#define USB_DEVCTL_SESSION_Pos              (0U)
#define USB_DEVCTL_SESSION_Msk              (0x1UL << USB_DEVCTL_SESSION_Pos)
#define USB_DEVCTL_SESSION                  USB_DEVCTL_SESSION_Msk
/** @brief DEVCTL[1]: Initiate Host Negotiation */
#define USB_DEVCTL_HOSTREQ_Pos              (1U)
#define USB_DEVCTL_HOSTREQ_Msk              (0x1UL << USB_DEVCTL_HOSTREQ_Pos)
#define USB_DEVCTL_HOSTREQ                  USB_DEVCTL_HOSTREQ_Msk
/** @brief DEVCTL[2]: Host Mode indication */
#define USB_DEVCTL_HOSTMODE_Pos             (2U)
#define USB_DEVCTL_HOSTMODE_Msk             (0x1UL << USB_DEVCTL_HOSTMODE_Pos)
#define USB_DEVCTL_HOSTMODE                 USB_DEVCTL_HOSTMODE_Msk
/** @brief DEVCTL[4:3]: VBus level */
#define USB_DEVCTL_VBUS_Pos                 (3U)
#define USB_DEVCTL_VBUS_Msk                 (0x3UL << USB_DEVCTL_VBUS_Pos)
#define USB_DEVCTL_VBUS                     USB_DEVCTL_VBUS_Msk

#define USB_DEVCTL_VBUS_LEV0                (0x0UL << USB_DEVCTL_VBUS_Pos)  /*!< Below SessionEnd */
#define USB_DEVCTL_VBUS_LEV1                (0x1UL << USB_DEVCTL_VBUS_Pos)  /*!< Above SessionEnd, below AValid */
#define USB_DEVCTL_VBUS_LEV2                (0x2UL << USB_DEVCTL_VBUS_Pos)  /*!< Above AValid, below VBusValid */
#define USB_DEVCTL_VBUS_LEV3                (0x3UL << USB_DEVCTL_VBUS_Pos)  /*!< Above VBusValid */

/** @brief DEVCTL[5]: Low-speed device detected */
#define USB_DEVCTL_LSDEV_Pos                (5U)
#define USB_DEVCTL_LSDEV_Msk                (0x1UL << USB_DEVCTL_LSDEV_Pos)
#define USB_DEVCTL_LSDEV                    USB_DEVCTL_LSDEV_Msk
/** @brief DEVCTL[6]: Full-speed or high-speed device detected */
#define USB_DEVCTL_FSDEV_Pos                (6U)
#define USB_DEVCTL_FSDEV_Msk                (0x1UL << USB_DEVCTL_FSDEV_Pos)
#define USB_DEVCTL_FSDEV                    USB_DEVCTL_FSDEV_Msk
/** @brief DEVCTL[7]: A device or B device indication */
#define USB_DEVCTL_BDEV_Pos                 (7U)
#define USB_DEVCTL_BDEV_Msk                 (0x1UL << USB_DEVCTL_BDEV_Pos)
#define USB_DEVCTL_BDEV                     USB_DEVCTL_BDEV_Msk

/*******************  Bit definition for USB_MISC register  *******************/

/** @brief MISC[0]: DMA_REQ signal mode for OUT endpoints */
#define USB_MISC_RXEDMA_Pos                 (0U)
#define USB_MISC_RXEDMA_Msk                 (0x1UL << USB_MISC_RXEDMA_Pos)
#define USB_MISC_RXEDMA                     USB_MISC_RXEDMA_Msk
/** @brief MISC[1]: DMA_REQ signal mode for IN endpoints */
#define USB_MISC_TXEDMA_Pos                 (1U)
#define USB_MISC_TXEDMA_Msk                 (0x1UL << USB_MISC_TXEDMA_Pos)
#define USB_MISC_TXEDMA                     USB_MISC_TXEDMA_Msk

/******************  Bit definition for USB_TXFIFOSZ register  ****************/

/** @brief TXFIFOSZ[3:0]: The size of the selected TX EP FIFO */
#define USB_TXFIFOSZ_SZ_Pos                 (0U)
#define USB_TXFIFOSZ_SZ_Msk                 (0xFUL << USB_TXFIFOSZ_SZ_Pos)
#define USB_TXFIFOSZ_SZ                     USB_TXFIFOSZ_SZ_Msk

#define USB_TXFIFOSZ_SZ_8BYTES              (0x0UL << USB_TXFIFOSZ_SZ_Pos)  /*!< 8 bytes */
#define USB_TXFIFOSZ_SZ_16BYTES             (0x1UL << USB_TXFIFOSZ_SZ_Pos)  /*!< 16 bytes */
#define USB_TXFIFOSZ_SZ_32BYTES             (0x2UL << USB_TXFIFOSZ_SZ_Pos)  /*!< 32 bytes */
#define USB_TXFIFOSZ_SZ_64BYTES             (0x3UL << USB_TXFIFOSZ_SZ_Pos)  /*!< 64 bytes */
#define USB_TXFIFOSZ_SZ_128BYTES            (0x4UL << USB_TXFIFOSZ_SZ_Pos)  /*!< 128 bytes */
#define USB_TXFIFOSZ_SZ_256BYTES            (0x5UL << USB_TXFIFOSZ_SZ_Pos)  /*!< 256 bytes */
#define USB_TXFIFOSZ_SZ_512BYTES            (0x6UL << USB_TXFIFOSZ_SZ_Pos)  /*!< 512 bytes */
#define USB_TXFIFOSZ_SZ_1024BYTES           (0x7UL << USB_TXFIFOSZ_SZ_Pos)  /*!< 1024 bytes */
#define USB_TXFIFOSZ_SZ_2048BYTES           (0x8UL << USB_TXFIFOSZ_SZ_Pos)  /*!< 2048 bytes */
#define USB_TXFIFOSZ_SZ_4096BYTES           (0x9UL << USB_TXFIFOSZ_SZ_Pos)  /*!< 4096 bytes */

/** @brief TXFIFOSZ[4]: Double-packet buffering support */
#define USB_TXFIFOSZ_DPB_Pos                (4U)
#define USB_TXFIFOSZ_DPB_Msk                (0x1UL << USB_TXFIFOSZ_DPB_Pos)
#define USB_TXFIFOSZ_DPB                    USB_TXFIFOSZ_DPB_Msk

/******************  Bit definition for USB_RXFIFOSZ register  ****************/

/** @brief RXFIFOSZ[3:0]: The size of the selected RX EP FIFO */
#define USB_RXFIFOSZ_SZ_Pos                 (0U)
#define USB_RXFIFOSZ_SZ_Msk                 (0xFUL << USB_RXFIFOSZ_SZ_Pos)
#define USB_RXFIFOSZ_SZ                     USB_RXFIFOSZ_SZ_Msk

#define USB_RXFIFOSZ_SZ_8BYTES              (0x0UL << USB_RXFIFOSZ_SZ_Pos)  /*!< 8 bytes */
#define USB_RXFIFOSZ_SZ_16BYTES             (0x1UL << USB_RXFIFOSZ_SZ_Pos)  /*!< 16 bytes */
#define USB_RXFIFOSZ_SZ_32BYTES             (0x2UL << USB_RXFIFOSZ_SZ_Pos)  /*!< 32 bytes */
#define USB_RXFIFOSZ_SZ_64BYTES             (0x3UL << USB_RXFIFOSZ_SZ_Pos)  /*!< 64 bytes */
#define USB_RXFIFOSZ_SZ_128BYTES            (0x4UL << USB_RXFIFOSZ_SZ_Pos)  /*!< 128 bytes */
#define USB_RXFIFOSZ_SZ_256BYTES            (0x5UL << USB_RXFIFOSZ_SZ_Pos)  /*!< 256 bytes */
#define USB_RXFIFOSZ_SZ_512BYTES            (0x6UL << USB_RXFIFOSZ_SZ_Pos)  /*!< 512 bytes */
#define USB_RXFIFOSZ_SZ_1024BYTES           (0x7UL << USB_RXFIFOSZ_SZ_Pos)  /*!< 1024 bytes */
#define USB_RXFIFOSZ_SZ_2048BYTES           (0x8UL << USB_RXFIFOSZ_SZ_Pos)  /*!< 2048 bytes */
#define USB_RXFIFOSZ_SZ_4096BYTES           (0x9UL << USB_RXFIFOSZ_SZ_Pos)  /*!< 4096 bytes */

/** @brief RXFIFOSZ[4]: Double-packet buffering support */
#define USB_RXFIFOSZ_DPB_Pos                (4U)
#define USB_RXFIFOSZ_DPB_Msk                (0x1UL << USB_RXFIFOSZ_DPB_Pos)
#define USB_RXFIFOSZ_DPB                    USB_RXFIFOSZ_DPB_Msk

/******************  Bit definition for USB_TXFIFOADD register  ***************/

/** @brief TXFIFOADD[12:0]: The start address of the selected TX EP FIFO */
#define USB_TXFIFOADD_AD_Pos                (0U)
#define USB_TXFIFOADD_AD_Msk                (0x1FFFUL << USB_TXFIFOADD_AD_Pos)
#define USB_TXFIFOADD_AD                    USB_TXFIFOADD_AD_Msk

/******************  Bit definition for USB_RXFIFOADD register  ***************/

/** @brief RXFIFOADD[12:0]: The start address of the selected RX EP FIFO */
#define USB_RXFIFOADD_AD_Pos                (0U)
#define USB_RXFIFOADD_AD_Msk                (0x1FFFUL << USB_RXFIFOADD_AD_Pos)
#define USB_RXFIFOADD_AD                    USB_RXFIFOADD_AD_Msk

/*******************  Bit definition for USB_HWVERS register  *****************/

/** @brief HWVERS[9:0]: Minor Version Number (Range 0-999) */
#define USB_HWVERS_YYY_Pos                  (0U)
#define USB_HWVERS_YYY_Msk                  (0x3FFUL << USB_HWVERS_YYY_Pos)
#define USB_HWVERS_YYY                      USB_HWVERS_YYY_Msk
/** @brief HWVERS[14:10]: Major Version Number (Range 0-31) */
#define USB_HWVERS_XX_Pos                   (10U)
#define USB_HWVERS_XX_Msk                   (0x1FUL << USB_HWVERS_XX_Pos)
#define USB_HWVERS_XX                       USB_HWVERS_XX_Msk
/** @brief HWVERS[15]: Release Candidate flag */
#define USB_HWVERS_RC_Pos                   (15U)
#define USB_HWVERS_RC_Msk                   (0x1UL << USB_HWVERS_RC_Pos)
#define USB_HWVERS_RC                       USB_HWVERS_RC_Msk

/**************  Bit definition for USB_ULPIVBUSCONTROL register  *************/

/** @brief ULPIVBUSCONTROL[0]: The use of an external charghe pump */
#define USB_ULPIVBUSCONTROL_USEEXTVBUS_Pos          (0U)
#define USB_ULPIVBUSCONTROL_USEEXTVBUS_Msk          (0x1UL << USB_ULPIVBUSCONTROL_USEEXTVBUS_Pos)
#define USB_ULPIVBUSCONTROL_USEEXTVBUS              USB_ULPIVBUSCONTROL_USEEXTVBUS_Msk
/** @brief ULPIVBUSCONTROL[1]: The use of an external VBus indicator */
#define USB_ULPIVBUSCONTROL_USEEXTVBUSIND_Pos       (1U)
#define USB_ULPIVBUSCONTROL_USEEXTVBUSIND_Msk       (0x1UL << USB_ULPIVBUSCONTROL_USEEXTVBUSIND_Pos)
#define USB_ULPIVBUSCONTROL_USEEXTVBUSIND           USB_ULPIVBUSCONTROL_USEEXTVBUSIND_Msk

/*************  Bit definition for USB_ULPICARKITCONTROL register  ************/

/** @brief ULPICARKITCONTROL[0]: Decouple the reconstituted UTMI signals from the USB controller prior to entering CarKit mode */
#define USB_ULPICARKITCONTROL_DISABLEUTMI_Pos       (0U)
#define USB_ULPICARKITCONTROL_DISABLEUTMI_Msk       (0x1UL << USB_ULPICARKITCONTROL_DISABLEUTMI_Pos)
#define USB_ULPICARKITCONTROL_DISABLEUTMI           USB_ULPICARKITCONTROL_DISABLEUTMI_Msk
/** @brief ULPICARKITCONTROL[1]: The CarKit mode is entered after DIR goes high */
#define USB_ULPICARKITCONTROL_CARKITACTIVE_Pos      (1U)
#define USB_ULPICARKITCONTROL_CARKITACTIVE_Msk      (0x1UL << USB_ULPICARKITCONTROL_CARKITACTIVE_Pos)
#define USB_ULPICARKITCONTROL_CARKITACTIVE          USB_ULPICARKITCONTROL_CARKITACTIVE_Msk
/** @brief ULPICARKITCONTROL[2]: An alt_int event occured */
#define USB_ULPICARKITCONTROL_ALTINTEVENT_Pos       (2U)
#define USB_ULPICARKITCONTROL_ALTINTEVENT_Msk       (0x1UL << USB_ULPICARKITCONTROL_ALTINTEVENT_Pos)
#define USB_ULPICARKITCONTROL_ALTINTEVENT           USB_ULPICARKITCONTROL_ALTINTEVENT_Msk
/** @brief ULPICARKITCONTROL[3]: An alt_int event occured */
#define USB_ULPICARKITCONTROL_CANCELCARKIT_Pos      (3U)
#define USB_ULPICARKITCONTROL_CANCELCARKIT_Msk      (0x1UL << USB_ULPICARKITCONTROL_CANCELCARKIT_Pos)
#define USB_ULPICARKITCONTROL_CANCELCARKIT          USB_ULPICARKITCONTROL_CANCELCARKIT_Msk
/** @brief ULPICARKITCONTROL[4]: A RxCmd has been latched */
#define USB_ULPICARKITCONTROL_RXCMDEVENT_Pos        (4U)
#define USB_ULPICARKITCONTROL_RXCMDEVENT_Msk        (0x1UL << USB_ULPICARKITCONTROL_RXCMDEVENT_Pos)
#define USB_ULPICARKITCONTROL_RXCMDEVENT            USB_ULPICARKITCONTROL_RXCMDEVENT_Msk
/** @brief ULPICARKITCONTROL[5]: CarKitActive is cleared */
#define USB_ULPICARKITCONTROL_CARKITACTIVEEND_Pos   (5U)
#define USB_ULPICARKITCONTROL_CARKITACTIVEEND_Msk   (0x1UL << USB_ULPICARKITCONTROL_CARKITACTIVEEND_Pos)
#define USB_ULPICARKITCONTROL_CARKITACTIVEEND       USB_ULPICARKITCONTROL_CARKITACTIVEEND_Msk

/***************  Bit definition for USB_ULPIINTMASK register  ****************/

/** @brief ULPIINTMASK[0]: Assert MC_NINT if ULPIIntSrc.RegInt is set */
#define USB_ULPIINTMASK_REGINTEN_Pos                (0U)
#define USB_ULPIINTMASK_REGINTEN_Msk                (0x1UL << USB_ULPIINTMASK_REGINTEN_Pos)
#define USB_ULPIINTMASK_REGINTEN                    USB_ULPIINTMASK_REGINTEN_Msk
/** @brief ULPIINTMASK[1]: Assert MC_NINT if ULPIIntSrc.AltInt is set */
#define USB_ULPIINTMASK_ALTINTEN_Pos                (1U)
#define USB_ULPIINTMASK_ALTINTEN_Msk                (0x1UL << USB_ULPIINTMASK_ALTINTEN_Pos)
#define USB_ULPIINTMASK_ALTINTEN                    USB_ULPIINTMASK_ALTINTEN_Msk
/** @brief ULPIINTMASK[2]: Assert MC_NINT if ULPIIntSrc.ActiveEndInt is set */
#define USB_ULPIINTMASK_ACTIVEENDINTEN_Pos          (2U)
#define USB_ULPIINTMASK_ACTIVEENDINTEN_Msk          (0x1UL << USB_ULPIINTMASK_ACTIVEENDINTEN_Pos)
#define USB_ULPIINTMASK_ACTIVEENDINTEN              USB_ULPIINTMASK_ACTIVEENDINTEN_Msk
/** @brief ULPIINTMASK[3]: Assert MC_NINT if ULPIIntSrc.RxCmdInt is set */
#define USB_ULPIINTMASK_RXCMDINTEN_Pos              (3U)
#define USB_ULPIINTMASK_RXCMDINTEN_Msk              (0x1UL << USB_ULPIINTMASK_RXCMDINTEN_Pos)
#define USB_ULPIINTMASK_RXCMDINTEN                  USB_ULPIINTMASK_RXCMDINTEN_Msk

/****************  Bit definition for USB_ULPIINTSRC register  ****************/

/** @brief ULPIINTSRC[0]: Asserted if ULPIRegControl.ULPIRegCmplt is set */
#define USB_ULPIINTSRC_REGINT_Pos                   (0U)
#define USB_ULPIINTSRC_REGINT_Msk                   (0x1UL << USB_ULPIINTSRC_REGINT_Pos)
#define USB_ULPIINTSRC_REGINT                       USB_ULPIINTSRC_REGINT_Msk
/** @brief ULPIINTSRC[1]: Asserted if ULPICarKitControl.AltIntEvent is set */
#define USB_ULPIINTSRC_ALTINT_Pos                   (1U)
#define USB_ULPIINTSRC_ALTINT_Msk                   (0x1UL << USB_ULPIINTSRC_ALTINT_Pos)
#define USB_ULPIINTSRC_ALTINT                       USB_ULPIINTSRC_ALTINT_Msk
/** @brief ULPIINTSRC[2]: Asserted if ULPICarKitControl.CarKitActiveEnd is set */
#define USB_ULPIINTSRC_ACTIVEENDINT_Pos             (2U)
#define USB_ULPIINTSRC_ACTIVEENDINT_Msk             (0x1UL << USB_ULPIINTSRC_ACTIVEENDINT_Pos)
#define USB_ULPIINTSRC_ACTIVEENDINT                 USB_ULPIINTSRC_ACTIVEENDINT_Msk
/** @brief ULPIINTSRC[3]: Asserted if ULPICarKitControl.RxCmdEvent is set */
#define USB_ULPIINTSRC_RXCMDINT_Pos                 (3U)
#define USB_ULPIINTSRC_RXCMDINT_Msk                 (0x1UL << USB_ULPIINTSRC_RXCMDINT_Pos)
#define USB_ULPIINTSRC_RXCMDINT                     USB_ULPIINTSRC_RXCMDINT_Msk

/**************  Bit definition for USB_ULPIREGCONTROL register  **************/

/** @brief ULPIREGCONTROL[0]: Initiate register access */
#define USB_ULPIREGCONTROL_ULPIREGREQ_Pos           (0U)
#define USB_ULPIREGCONTROL_ULPIREGREQ_Msk           (0x1UL << USB_ULPIREGCONTROL_ULPIREGREQ_Pos)
#define USB_ULPIREGCONTROL_ULPIREGREQ               USB_ULPIREGCONTROL_ULPIREGREQ_Msk
/** @brief ULPIREGCONTROL[1]: Register access is complete */
#define USB_ULPIREGCONTROL_ULPIREGCMPLT_Pos         (1U)
#define USB_ULPIREGCONTROL_ULPIREGCMPLT_Msk         (0x1UL << USB_ULPIREGCONTROL_ULPIREGCMPLT_Pos)
#define USB_ULPIREGCONTROL_ULPIREGCMPLT             USB_ULPIREGCONTROL_ULPIREGCMPLT_Msk
/** @brief ULPIREGCONTROL[2]: Register read/write access */
#define USB_ULPIREGCONTROL_ULPIRDNWR_Pos            (2U)
#define USB_ULPIREGCONTROL_ULPIRDNWR_Msk            (0x1UL << USB_ULPIREGCONTROL_ULPIRDNWR_Pos)
#define USB_ULPIREGCONTROL_ULPIRDNWR                USB_ULPIREGCONTROL_ULPIRDNWR_Msk

/***************  Bit definition for USB_ULPIRAWDATA register  ****************/

/* Asynchronous mode */

/** @brief ULPIRAWDATA[0]: Active high transmit enable (tx_enable) */
#define USB_ULPIRAWDATA_ASYNC_DATA0_Pos             (0U)
#define USB_ULPIRAWDATA_ASYNC_DATA0_Msk             (0x1UL << USB_ULPIRAWDATA_ASYNC_DATA0_Pos)
#define USB_ULPIRAWDATA_ASYNC_DATA0                 USB_ULPIRAWDATA_ASYNC_DATA0_Msk
/** @brief ULPIRAWDATA[1]: Differential data (dat) */
#define USB_ULPIRAWDATA_ASYNC_DATA1_Pos             (1U)
#define USB_ULPIRAWDATA_ASYNC_DATA1_Msk             (0x1UL << USB_ULPIRAWDATA_ASYNC_DATA1_Pos)
#define USB_ULPIRAWDATA_ASYNC_DATA1                 USB_ULPIRAWDATA_ASYNC_DATA1_Msk
/** @brief ULPIRAWDATA[2]: Single-ended zero (se0) */
#define USB_ULPIRAWDATA_ASYNC_DATA2_Pos             (2U)
#define USB_ULPIRAWDATA_ASYNC_DATA2_Msk             (0x1UL << USB_ULPIRAWDATA_ASYNC_DATA2_Pos)
#define USB_ULPIRAWDATA_ASYNC_DATA2                 USB_ULPIRAWDATA_ASYNC_DATA2_Msk
/** @brief ULPIRAWDATA[3]: Active high interrupt indication (int) */
#define USB_ULPIRAWDATA_ASYNC_DATA3_Pos             (3U)
#define USB_ULPIRAWDATA_ASYNC_DATA3_Msk             (0x1UL << USB_ULPIRAWDATA_ASYNC_DATA3_Pos)
#define USB_ULPIRAWDATA_ASYNC_DATA3                 USB_ULPIRAWDATA_ASYNC_DATA3_Msk

/* Synchronous mode */

/** @brief ULPIRAWDATA[1:0]: UTMI+ LineState signals */
#define USB_ULPIRAWDATA_SYNC_LINESTATE_Pos          (0U)
#define USB_ULPIRAWDATA_SYNC_LINESTATE_Msk          (0x3UL << USB_ULPIRAWDATA_SYNC_LINESTATE_Pos)
#define USB_ULPIRAWDATA_SYNC_LINESTATE              USB_ULPIRAWDATA_SYNC_LINESTATE_Msk
/** @brief ULPIRAWDATA[3:2]: VBus voltage state */
#define USB_ULPIRAWDATA_SYNC_VBUSSTATE_Pos          (2U)
#define USB_ULPIRAWDATA_SYNC_VBUSSTATE_Msk          (0x3UL << USB_ULPIRAWDATA_SYNC_VBUSSTATE_Pos)
#define USB_ULPIRAWDATA_SYNC_VBUSSTATE              USB_ULPIRAWDATA_SYNC_VBUSSTATE_Msk

#define USB_ULPIRAWDATA_SYNC_VBUSSTATE0             (0x0UL << USB_ULPIRAWDATA_SYNC_VBUSSTATE_Pos)   /*!< VBus < VB_Sess_END */
#define USB_ULPIRAWDATA_SYNC_VBUSSTATE1             (0x1UL << USB_ULPIRAWDATA_SYNC_VBUSSTATE_Pos)   /*!< VB_Sess_END <= VBus < V_Sess_VLD */
#define USB_ULPIRAWDATA_SYNC_VBUSSTATE2             (0x2UL << USB_ULPIRAWDATA_SYNC_VBUSSTATE_Pos)   /*!< V_Sess_VLD <= VBus < VA_VBus_VLD */
#define USB_ULPIRAWDATA_SYNC_VBUSSTATE3             (0x3UL << USB_ULPIRAWDATA_SYNC_VBUSSTATE_Pos)   /*!< VA_VBus_VLD < VBus */

/** @brief ULPIRAWDATA[5:4]: Encoded UTMI event signals */
#define USB_ULPIRAWDATA_SYNC_RXEVENT_Pos            (4U)
#define USB_ULPIRAWDATA_SYNC_RXEVENT_Msk            (0x3UL << USB_ULPIRAWDATA_SYNC_RXEVENT_Pos)
#define USB_ULPIRAWDATA_SYNC_RXEVENT                USB_ULPIRAWDATA_SYNC_RXEVENT_Msk

/** @brief ULPIRAWDATA[6]: The value of the IDDIG (valid 50ms after IDPULLUP is asserted) */
#define USB_ULPIRAWDATA_SYNC_ID_Pos                 (6U)
#define USB_ULPIRAWDATA_SYNC_ID_Msk                 (0x1UL << USB_ULPIRAWDATA_SYNC_ID_Pos)
#define USB_ULPIRAWDATA_SYNC_ID                     USB_ULPIRAWDATA_SYNC_ID_Msk

/** @brief ULPIRAWDATA[7]: A non-USB interrupt occured */
#define USB_ULPIRAWDATA_SYNC_ALT_INT_Pos            (7U)
#define USB_ULPIRAWDATA_SYNC_ALT_INT_Msk            (0x1UL << USB_ULPIRAWDATA_SYNC_ALT_INT_Pos)
#define USB_ULPIRAWDATA_SYNC_ALT_INT                USB_ULPIRAWDATA_SYNC_ALT_INT_Msk

/******************  Bit definition for USB_EPINFO register  ******************/

/** @brief EPINFO[3:0]: The number of TX endpoints implemented */
#define USB_EPINFO_TXEP_Pos                 (0U)
#define USB_EPINFO_TXEP_Msk                 (0xFUL << USB_EPINFO_TXEP_Pos)
#define USB_EPINFO_TXEP                     USB_EPINFO_TXEP_Msk
/** @brief EPINFO[7:4]: The number of RX endpoints implemented */
#define USB_EPINFO_RXEP_Pos                 (4U)
#define USB_EPINFO_RXEP_Msk                 (0xFUL << USB_EPINFO_RXEP_Pos)
#define USB_EPINFO_RXEP                     USB_EPINFO_RXEP_Msk

/******************  Bit definition for USB_RAMINFO register  *****************/

/** @brief RAMINFO[3:0]: The width of the RAM address bus */
#define USB_RAMINFO_RAMBITS_Pos             (0U)
#define USB_RAMINFO_RAMBITS_Msk             (0xFUL << USB_RAMINFO_RAMBITS_Pos)
#define USB_RAMINFO_RAMBITS                 USB_RAMINFO_RAMBITS_Msk
/** @brief RAMINFO[7:4]: The number of DMA channels implemented */
#define USB_RAMINFO_DMACHNLS_Pos            (4U)
#define USB_RAMINFO_DMACHNLS_Msk            (0xFUL << USB_RAMINFO_DMACHNLS_Pos)
#define USB_RAMINFO_DMACHNLS                USB_RAMINFO_DMACHNLS_Msk

/*****************  Bit definition for USB_LINKINFO register  *****************/

/** @brief LINKINFO[3:0]: The delay from IDPULLUP asserted to IDDIG considered valid */
#define USB_LINKINFO_WTID_Pos               (0U)
#define USB_LINKINFO_WTID_Msk               (0xFUL << USB_LINKINFO_WTID_Pos)
#define USB_LINKINFO_WTID                   USB_LINKINFO_WTID_Msk
/** @brief LINKINFO[7:4]: The delay to allow for user's connect/disconnect filter */
#define USB_LINKINFO_WTCON_Pos              (4U)
#define USB_LINKINFO_WTCON_Msk              (0xFUL << USB_LINKINFO_WTCON_Pos)
#define USB_LINKINFO_WTCON                  USB_LINKINFO_WTCON_Msk

/*****************  Bit definition for USB_SOFT_RST register  *****************/

/** @brief SOFT_RST[0]: NRSTO signal control */
#define USB_SOFT_RST_NRST_Pos               (0U)
#define USB_SOFT_RST_NRST_Msk               (0x1UL << USB_SOFT_RST_NRST_Pos)
#define USB_SOFT_RST_NRST                   USB_SOFT_RST_NRST_Msk
/** @brief SOFT_RST[0]: NRSTOX signal control */
#define USB_SOFT_RST_NRSTX_Pos              (1U)
#define USB_SOFT_RST_NRSTX_Msk              (0x1UL << USB_SOFT_RST_NRSTX_Pos)
#define USB_SOFT_RST_NRSTX                  USB_SOFT_RST_NRSTX_Msk

/****************  Bit definition for USB_TXFUNCADDR register  ****************/

/** @brief TXFUNCADDR[6:0]: The address of the target function */
#define USB_TXFUNCADDR_ADDR_Pos             (0U)
#define USB_TXFUNCADDR_ADDR_Msk             (0x7FUL << USB_TXFUNCADDR_ADDR_Pos)
#define USB_TXFUNCADDR_ADDR                 USB_TXFUNCADDR_ADDR_Msk

/*****************  Bit definition for USB_TXHUBADDR register  ****************/

/** @brief TXHUBADDR[6:0]: The address of the USB hub */
#define USB_TXHUBADDR_ADDR_Pos              (0U)
#define USB_TXHUBADDR_ADDR_Msk              (0x7FUL << USB_TXHUBADDR_ADDR_Pos)
#define USB_TXHUBADDR_ADDR                  USB_TXHUBADDR_ADDR_Msk
/** @brief TXHUBADDR[7]: Hub multiple transaction translators flag */
#define USB_TXHUBADDR_MULTTRAN_Pos          (7U)
#define USB_TXHUBADDR_MULTTRAN_Msk          (0x1UL << USB_TXHUBADDR_MULTTRAN_Pos)
#define USB_TXHUBADDR_MULTTRAN              USB_TXHUBADDR_MULTTRAN_Msk

/****************  Bit definition for USB_TXHUBPORT register  *****************/

/** @brief TXHUBPORT[6:0]: The port of the USB hub */
#define USB_TXHUBPORT_PORT_Pos              (0U)
#define USB_TXHUBPORT_PORT_Msk              (0x7FUL << USB_TXHUBPORT_PORT_Pos)
#define USB_TXHUBPORT_PORT                  USB_TXHUBPORT_PORT_Msk

/****************  Bit definition for USB_RXFUNCADDR register  ****************/

/** @brief RXFUNCADDR[6:0]: The address of the target function */
#define USB_RXFUNCADDR_ADDR_Pos             (0U)
#define USB_RXFUNCADDR_ADDR_Msk             (0x7FUL << USB_RXFUNCADDR_ADDR_Pos)
#define USB_RXFUNCADDR_ADDR                 USB_RXFUNCADDR_ADDR_Msk

/*****************  Bit definition for USB_RXHUBADDR register  ****************/

/** @brief RXHUBADDR[6:0]: The address of the USB hub */
#define USB_RXHUBADDR_ADDR_Pos              (0U)
#define USB_RXHUBADDR_ADDR_Msk              (0x7FUL << USB_RXHUBADDR_ADDR_Pos)
#define USB_RXHUBADDR_ADDR                  USB_RXHUBADDR_ADDR_Msk
/** @brief RXHUBADDR[7]: Hub multiple transaction translators flag */
#define USB_RXHUBADDR_MULTTRAN_Pos          (7U)
#define USB_RXHUBADDR_MULTTRAN_Msk          (0x1UL << USB_RXHUBADDR_MULTTRAN_Pos)
#define USB_RXHUBADDR_MULTTRAN              USB_RXHUBADDR_MULTTRAN_Msk

/****************  Bit definition for USB_RXHUBPORT register  *****************/

/** @brief RXHUBPORT[6:0]: The port of the USB hub */
#define USB_RXHUBPORT_PORT_Pos              (0U)
#define USB_RXHUBPORT_PORT_Msk              (0x7FUL << USB_RXHUBPORT_PORT_Pos)
#define USB_RXHUBPORT_PORT                  USB_RXHUBPORT_PORT_Msk

/*****************  Bit definition for USB_DMA_INTR register  *****************/

/** @brief DMA_INTR[0]: Channel 1 DMA interrupt */
#define USB_DMA_CNTL_INTR_CH1_Pos           (0U)
#define USB_DMA_CNTL_INTR_CH1_Msk           (0x1UL << USB_DMA_CNTL_INTR_CH1_Pos)
#define USB_DMA_CNTL_INTR_CH1               USB_DMA_CNTL_INTR_CH1_Msk
/** @brief DMA_INTR[1]: Channel 2 DMA interrupt */
#define USB_DMA_CNTL_INTR_CH2_Pos           (1U)
#define USB_DMA_CNTL_INTR_CH2_Msk           (0x1UL << USB_DMA_CNTL_INTR_CH2_Pos)
#define USB_DMA_CNTL_INTR_CH2               USB_DMA_CNTL_INTR_CH2_Msk
/** @brief DMA_INTR[2]: Channel 3 DMA interrupt */
#define USB_DMA_CNTL_INTR_CH3_Pos           (2U)
#define USB_DMA_CNTL_INTR_CH3_Msk           (0x1UL << USB_DMA_CNTL_INTR_CH3_Pos)
#define USB_DMA_CNTL_INTR_CH3               USB_DMA_CNTL_INTR_CH3_Msk
/** @brief DMA_INTR[3]: Channel 4 DMA interrupt */
#define USB_DMA_CNTL_INTR_CH4_Pos           (3U)
#define USB_DMA_CNTL_INTR_CH4_Msk           (0x1UL << USB_DMA_CNTL_INTR_CH4_Pos)
#define USB_DMA_CNTL_INTR_CH4               USB_DMA_CNTL_INTR_CH4_Msk
/** @brief DMA_INTR[4]: Channel 5 DMA interrupt */
#define USB_DMA_CNTL_INTR_CH5_Pos           (4U)
#define USB_DMA_CNTL_INTR_CH5_Msk           (0x1UL << USB_DMA_CNTL_INTR_CH5_Pos)
#define USB_DMA_CNTL_INTR_CH5               USB_DMA_CNTL_INTR_CH5_Msk
/** @brief DMA_INTR[5]: Channel 6 DMA interrupt */
#define USB_DMA_CNTL_INTR_CH6_Pos           (5U)
#define USB_DMA_CNTL_INTR_CH6_Msk           (0x1UL << USB_DMA_CNTL_INTR_CH6_Pos)
#define USB_DMA_CNTL_INTR_CH6               USB_DMA_CNTL_INTR_CH6_Msk
/** @brief DMA_INTR[6]: Channel 7 DMA interrupt */
#define USB_DMA_CNTL_INTR_CH7_Pos           (6U)
#define USB_DMA_CNTL_INTR_CH7_Msk           (0x1UL << USB_DMA_CNTL_INTR_CH7_Pos)
#define USB_DMA_CNTL_INTR_CH7               USB_DMA_CNTL_INTR_CH7_Msk
/** @brief DMA_INTR[7]: Channel 8 DMA interrupt */
#define USB_DMA_CNTL_INTR_CH8_Pos           (7U)
#define USB_DMA_CNTL_INTR_CH8_Msk           (0x1UL << USB_DMA_CNTL_INTR_CH8_Pos)
#define USB_DMA_CNTL_INTR_CH8               USB_DMA_CNTL_INTR_CH8_Msk

/*****************  Bit definition for USB_DMA_CNTL register  *****************/

/** @brief DMA_CNTL[0]: DMA transfer enable */
#define USB_DMA_CNTL_DMAEN_Pos              (0U)
#define USB_DMA_CNTL_DMAEN_Msk              (0x1UL << USB_DMA_CNTL_DMAEN_Pos)
#define USB_DMA_CNTL_DMAEN                  USB_DMA_CNTL_DMAEN_Msk

#define USB_DMA_CNTL_DMAEN_DISABLE          (0x0UL << USB_DMA_CNTL_DMAEN_Pos)   /*!< DMA transfer disabled */
#define USB_DMA_CNTL_DMAEN_ENABLE           (0x1UL << USB_DMA_CNTL_DMAEN_Pos)   /*!< DMA transfer enabled */

/** @brief DMA_CNTL[1]: DMA transfer direction */
#define USB_DMA_CNTL_DMADIR_Pos             (1U)
#define USB_DMA_CNTL_DMADIR_Msk             (0x1UL << USB_DMA_CNTL_DMADIR_Pos)
#define USB_DMA_CNTL_DMADIR                 USB_DMA_CNTL_DMADIR_Msk

#define USB_DMA_CNTL_DMADIR_WRITE           (0x0UL << USB_DMA_CNTL_DMADIR_Pos)  /*!< DMA Write (RX Endpoint) */
#define USB_DMA_CNTL_DMADIR_READ            (0x1UL << USB_DMA_CNTL_DMADIR_Pos)  /*!< DMA Read (TX Endpoint) */

/** @brief DMA_CNTL[2]: DMA transfer mode */
#define USB_DMA_CNTL_DMAMODE_Pos            (2U)
#define USB_DMA_CNTL_DMAMODE_Msk            (0x1UL << USB_DMA_CNTL_DMAMODE_Pos)
#define USB_DMA_CNTL_DMAMODE                USB_DMA_CNTL_DMAMODE_Msk

#define USB_DMA_CNTL_DMAMODE_MODE0          (0x0UL << USB_DMA_CNTL_DMAMODE_Pos) /*!< DMA Mode0 Transfer */
#define USB_DMA_CNTL_DMAMODE_MODE1          (0x1UL << USB_DMA_CNTL_DMAMODE_Pos) /*!< DMA Mode1 Transfer */

/** @brief DMA_CNTL[3]: DMA interrupt enable */
#define USB_DMA_CNTL_DMAIE_Pos              (3U)
#define USB_DMA_CNTL_DMAIE_Msk              (0x1UL << USB_DMA_CNTL_DMAIE_Pos)
#define USB_DMA_CNTL_DMAIE                  USB_DMA_CNTL_DMAIE_Msk

#define USB_DMA_CNTL_DMAIE_DISABLE          (0x0UL << USB_DMA_CNTL_DMAIE_Pos)   /*!< DMA interrupt disabled */
#define USB_DMA_CNTL_DMAIE_ENABLE           (0x1UL << USB_DMA_CNTL_DMAIE_Pos)   /*!< DMA interrupt enabled */

/** @brief DMA_CNTL[7:4]: The endpoint number the channel is assigned to */
#define USB_DMA_CNTL_DMAEP_Pos              (4U)
#define USB_DMA_CNTL_DMAEP_Msk              (0xFUL << USB_DMA_CNTL_DMAEP_Pos)
#define USB_DMA_CNTL_DMAEP                  USB_DMA_CNTL_DMAEP_Msk

#define USB_DMA_CNTL_DMAEP_EP0              (0x0UL << USB_DMA_CNTL_DMAEP_Pos)   /*!< Endpoint 0 */
#define USB_DMA_CNTL_DMAEP_EP1              (0x1UL << USB_DMA_CNTL_DMAEP_Pos)   /*!< Endpoint 1 */
#define USB_DMA_CNTL_DMAEP_EP2              (0x2UL << USB_DMA_CNTL_DMAEP_Pos)   /*!< Endpoint 2 */
#define USB_DMA_CNTL_DMAEP_EP3              (0x3UL << USB_DMA_CNTL_DMAEP_Pos)   /*!< Endpoint 3 */
#define USB_DMA_CNTL_DMAEP_EP4              (0x4UL << USB_DMA_CNTL_DMAEP_Pos)   /*!< Endpoint 4 */
#define USB_DMA_CNTL_DMAEP_EP5              (0x5UL << USB_DMA_CNTL_DMAEP_Pos)   /*!< Endpoint 5 */
#define USB_DMA_CNTL_DMAEP_EP6              (0x6UL << USB_DMA_CNTL_DMAEP_Pos)   /*!< Endpoint 6 */
#define USB_DMA_CNTL_DMAEP_EP7              (0x7UL << USB_DMA_CNTL_DMAEP_Pos)   /*!< Endpoint 7 */
#define USB_DMA_CNTL_DMAEP_EP8              (0x8UL << USB_DMA_CNTL_DMAEP_Pos)   /*!< Endpoint 8 */
#define USB_DMA_CNTL_DMAEP_EP9              (0x9UL << USB_DMA_CNTL_DMAEP_Pos)   /*!< Endpoint 9 */
#define USB_DMA_CNTL_DMAEP_EP10             (0xAUL << USB_DMA_CNTL_DMAEP_Pos)   /*!< Endpoint 10 */
#define USB_DMA_CNTL_DMAEP_EP11             (0xBUL << USB_DMA_CNTL_DMAEP_Pos)   /*!< Endpoint 11 */
#define USB_DMA_CNTL_DMAEP_EP12             (0xCUL << USB_DMA_CNTL_DMAEP_Pos)   /*!< Endpoint 12 */
#define USB_DMA_CNTL_DMAEP_EP13             (0xDUL << USB_DMA_CNTL_DMAEP_Pos)   /*!< Endpoint 13 */
#define USB_DMA_CNTL_DMAEP_EP14             (0xEUL << USB_DMA_CNTL_DMAEP_Pos)   /*!< Endpoint 14 */
#define USB_DMA_CNTL_DMAEP_EP15             (0xFUL << USB_DMA_CNTL_DMAEP_Pos)   /*!< Endpoint 15 */

/** @brief DMA_CNTL[8]: Bus Error Bit */
#define USB_DMA_CNTL_DMAERR_Pos             (8U)
#define USB_DMA_CNTL_DMAERR_Msk             (0x1UL << USB_DMA_CNTL_DMAERR_Pos)
#define USB_DMA_CNTL_DMAERR                 USB_DMA_CNTL_DMAERR_Msk

/** @brief DMA_CNTL[10:9]: Burst Mode */
#define USB_DMA_CNTL_DMABRSTM_Pos           (9U)
#define USB_DMA_CNTL_DMABRSTM_Msk           (0x3UL << USB_DMA_CNTL_DMABRSTM_Pos)
#define USB_DMA_CNTL_DMABRSTM               USB_DMA_CNTL_DMABRSTM_Msk

#define USB_DMA_CNTL_DMABRSTM_MODE0         (0x0UL << USB_DMA_CNTL_DMABRSTM_Pos)    /*!< Burst Mode 0: Bursts of unspecified length */
#define USB_DMA_CNTL_DMABRSTM_MODE1         (0x1UL << USB_DMA_CNTL_DMABRSTM_Pos)    /*!< Burst Mode 1: INCR4 or unspecified length */
#define USB_DMA_CNTL_DMABRSTM_MODE2         (0x2UL << USB_DMA_CNTL_DMABRSTM_Pos)    /*!< Burst Mode 2: INCR8, INCR4 or unspecified length */
#define USB_DMA_CNTL_DMABRSTM_MODE3         (0x3UL << USB_DMA_CNTL_DMABRSTM_Pos)    /*!< Burst Mode 3: INCR16, INCR8, INCR4 or unspecified length */

/***************  Bit definition for USB_RXDPKTBUFDIS register  ***************/

/** @brief RXDPKTBUFDIS[1]: Rx Double Packet Buffer Disable for EP 1 */
#define USB_RXDPKTBUFDIS_EP1_Pos            (1U)
#define USB_RXDPKTBUFDIS_EP1_Msk            (0x1UL << USB_RXDPKTBUFDIS_EP1_Pos)
#define USB_RXDPKTBUFDIS_EP1                USB_RXDPKTBUFDIS_EP1_Msk
/** @brief RXDPKTBUFDIS[2]: Rx Double Packet Buffer Disable for EP 2 */
#define USB_RXDPKTBUFDIS_EP2_Pos            (2U)
#define USB_RXDPKTBUFDIS_EP2_Msk            (0x1UL << USB_RXDPKTBUFDIS_EP2_Pos)
#define USB_RXDPKTBUFDIS_EP2                USB_RXDPKTBUFDIS_EP2_Msk
/** @brief RXDPKTBUFDIS[3]: Rx Double Packet Buffer Disable for EP 3 */
#define USB_RXDPKTBUFDIS_EP3_Pos            (3U)
#define USB_RXDPKTBUFDIS_EP3_Msk            (0x1UL << USB_RXDPKTBUFDIS_EP3_Pos)
#define USB_RXDPKTBUFDIS_EP3                USB_RXDPKTBUFDIS_EP3_Msk
/** @brief RXDPKTBUFDIS[4]: Rx Double Packet Buffer Disable for EP 4 */
#define USB_RXDPKTBUFDIS_EP4_Pos            (4U)
#define USB_RXDPKTBUFDIS_EP4_Msk            (0x1UL << USB_RXDPKTBUFDIS_EP4_Pos)
#define USB_RXDPKTBUFDIS_EP4                USB_RXDPKTBUFDIS_EP4_Msk
/** @brief RXDPKTBUFDIS[5]: Rx Double Packet Buffer Disable for EP 5 */
#define USB_RXDPKTBUFDIS_EP5_Pos            (5U)
#define USB_RXDPKTBUFDIS_EP5_Msk            (0x1UL << USB_RXDPKTBUFDIS_EP5_Pos)
#define USB_RXDPKTBUFDIS_EP5                USB_RXDPKTBUFDIS_EP5_Msk
/** @brief RXDPKTBUFDIS[6]: Rx Double Packet Buffer Disable for EP 6 */
#define USB_RXDPKTBUFDIS_EP6_Pos            (6U)
#define USB_RXDPKTBUFDIS_EP6_Msk            (0x1UL << USB_RXDPKTBUFDIS_EP6_Pos)
#define USB_RXDPKTBUFDIS_EP6                USB_RXDPKTBUFDIS_EP6_Msk
/** @brief RXDPKTBUFDIS[7]: Rx Double Packet Buffer Disable for EP 7 */
#define USB_RXDPKTBUFDIS_EP7_Pos            (7U)
#define USB_RXDPKTBUFDIS_EP7_Msk            (0x1UL << USB_RXDPKTBUFDIS_EP7_Pos)
#define USB_RXDPKTBUFDIS_EP7                USB_RXDPKTBUFDIS_EP7_Msk
/** @brief RXDPKTBUFDIS[8]: Rx Double Packet Buffer Disable for EP 8 */
#define USB_RXDPKTBUFDIS_EP8_Pos            (8U)
#define USB_RXDPKTBUFDIS_EP8_Msk            (0x1UL << USB_RXDPKTBUFDIS_EP8_Pos)
#define USB_RXDPKTBUFDIS_EP8                USB_RXDPKTBUFDIS_EP8_Msk
/** @brief RXDPKTBUFDIS[9]: Rx Double Packet Buffer Disable for EP 9 */
#define USB_RXDPKTBUFDIS_EP9_Pos            (9U)
#define USB_RXDPKTBUFDIS_EP9_Msk            (0x1UL << USB_RXDPKTBUFDIS_EP9_Pos)
#define USB_RXDPKTBUFDIS_EP9                USB_RXDPKTBUFDIS_EP9_Msk
/** @brief RXDPKTBUFDIS[10]: Rx Double Packet Buffer Disable for EP 10 */
#define USB_RXDPKTBUFDIS_EP10_Pos           (10U)
#define USB_RXDPKTBUFDIS_EP10_Msk           (0x1UL << USB_RXDPKTBUFDIS_EP10_Pos)
#define USB_RXDPKTBUFDIS_EP10               USB_RXDPKTBUFDIS_EP10_Msk
/** @brief RXDPKTBUFDIS[11]: Rx Double Packet Buffer Disable for EP 11 */
#define USB_RXDPKTBUFDIS_EP11_Pos           (11U)
#define USB_RXDPKTBUFDIS_EP11_Msk           (0x1UL << USB_RXDPKTBUFDIS_EP11_Pos)
#define USB_RXDPKTBUFDIS_EP11               USB_RXDPKTBUFDIS_EP11_Msk
/** @brief RXDPKTBUFDIS[12]: Rx Double Packet Buffer Disable for EP 12 */
#define USB_RXDPKTBUFDIS_EP12_Pos           (12U)
#define USB_RXDPKTBUFDIS_EP12_Msk           (0x1UL << USB_RXDPKTBUFDIS_EP12_Pos)
#define USB_RXDPKTBUFDIS_EP12               USB_RXDPKTBUFDIS_EP12_Msk
/** @brief RXDPKTBUFDIS[13]: Rx Double Packet Buffer Disable for EP 13 */
#define USB_RXDPKTBUFDIS_EP13_Pos           (13U)
#define USB_RXDPKTBUFDIS_EP13_Msk           (0x1UL << USB_RXDPKTBUFDIS_EP13_Pos)
#define USB_RXDPKTBUFDIS_EP13               USB_RXDPKTBUFDIS_EP13_Msk
/** @brief RXDPKTBUFDIS[14]: Rx Double Packet Buffer Disable for EP 14 */
#define USB_RXDPKTBUFDIS_EP14_Pos           (14U)
#define USB_RXDPKTBUFDIS_EP14_Msk           (0x1UL << USB_RXDPKTBUFDIS_EP14_Pos)
#define USB_RXDPKTBUFDIS_EP14               USB_RXDPKTBUFDIS_EP14_Msk
/** @brief RXDPKTBUFDIS[15]: Rx Double Packet Buffer Disable for EP 15 */
#define USB_RXDPKTBUFDIS_EP15_Pos           (15U)
#define USB_RXDPKTBUFDIS_EP15_Msk           (0x1UL << USB_RXDPKTBUFDIS_EP15_Pos)
#define USB_RXDPKTBUFDIS_EP15               USB_RXDPKTBUFDIS_EP15_Msk

/***************  Bit definition for USB_TXDPKTBUFDIS register  ***************/

/** @brief TXDPKTBUFDIS[1]: Tx Double Packet Buffer Disable for EP 1 */
#define USB_TXDPKTBUFDIS_EP1_Pos            (1U)
#define USB_TXDPKTBUFDIS_EP1_Msk            (0x1UL << USB_TXDPKTBUFDIS_EP1_Pos)
#define USB_TXDPKTBUFDIS_EP1                USB_TXDPKTBUFDIS_EP1_Msk
/** @brief TXDPKTBUFDIS[2]: Tx Double Packet Buffer Disable for EP 2 */
#define USB_TXDPKTBUFDIS_EP2_Pos            (2U)
#define USB_TXDPKTBUFDIS_EP2_Msk            (0x1UL << USB_TXDPKTBUFDIS_EP2_Pos)
#define USB_TXDPKTBUFDIS_EP2                USB_TXDPKTBUFDIS_EP2_Msk
/** @brief TXDPKTBUFDIS[3]: Tx Double Packet Buffer Disable for EP 3 */
#define USB_TXDPKTBUFDIS_EP3_Pos            (3U)
#define USB_TXDPKTBUFDIS_EP3_Msk            (0x1UL << USB_TXDPKTBUFDIS_EP3_Pos)
#define USB_TXDPKTBUFDIS_EP3                USB_TXDPKTBUFDIS_EP3_Msk
/** @brief TXDPKTBUFDIS[4]: Tx Double Packet Buffer Disable for EP 4 */
#define USB_TXDPKTBUFDIS_EP4_Pos            (4U)
#define USB_TXDPKTBUFDIS_EP4_Msk            (0x1UL << USB_TXDPKTBUFDIS_EP4_Pos)
#define USB_TXDPKTBUFDIS_EP4                USB_TXDPKTBUFDIS_EP4_Msk
/** @brief TXDPKTBUFDIS[5]: Tx Double Packet Buffer Disable for EP 5 */
#define USB_TXDPKTBUFDIS_EP5_Pos            (5U)
#define USB_TXDPKTBUFDIS_EP5_Msk            (0x1UL << USB_TXDPKTBUFDIS_EP5_Pos)
#define USB_TXDPKTBUFDIS_EP5                USB_TXDPKTBUFDIS_EP5_Msk
/** @brief TXDPKTBUFDIS[6]: Tx Double Packet Buffer Disable for EP 6 */
#define USB_TXDPKTBUFDIS_EP6_Pos            (6U)
#define USB_TXDPKTBUFDIS_EP6_Msk            (0x1UL << USB_TXDPKTBUFDIS_EP6_Pos)
#define USB_TXDPKTBUFDIS_EP6                USB_TXDPKTBUFDIS_EP6_Msk
/** @brief TXDPKTBUFDIS[7]: Tx Double Packet Buffer Disable for EP 7 */
#define USB_TXDPKTBUFDIS_EP7_Pos            (7U)
#define USB_TXDPKTBUFDIS_EP7_Msk            (0x1UL << USB_TXDPKTBUFDIS_EP7_Pos)
#define USB_TXDPKTBUFDIS_EP7                USB_TXDPKTBUFDIS_EP7_Msk
/** @brief TXDPKTBUFDIS[8]: Tx Double Packet Buffer Disable for EP 8 */
#define USB_TXDPKTBUFDIS_EP8_Pos            (8U)
#define USB_TXDPKTBUFDIS_EP8_Msk            (0x1UL << USB_TXDPKTBUFDIS_EP8_Pos)
#define USB_TXDPKTBUFDIS_EP8                USB_TXDPKTBUFDIS_EP8_Msk
/** @brief TXDPKTBUFDIS[9]: Tx Double Packet Buffer Disable for EP 9 */
#define USB_TXDPKTBUFDIS_EP9_Pos            (9U)
#define USB_TXDPKTBUFDIS_EP9_Msk            (0x1UL << USB_TXDPKTBUFDIS_EP9_Pos)
#define USB_TXDPKTBUFDIS_EP9                USB_TXDPKTBUFDIS_EP9_Msk
/** @brief TXDPKTBUFDIS[10]: Tx Double Packet Buffer Disable for EP 10 */
#define USB_TXDPKTBUFDIS_EP10_Pos           (10U)
#define USB_TXDPKTBUFDIS_EP10_Msk           (0x1UL << USB_TXDPKTBUFDIS_EP10_Pos)
#define USB_TXDPKTBUFDIS_EP10               USB_TXDPKTBUFDIS_EP10_Msk
/** @brief TXDPKTBUFDIS[11]: Tx Double Packet Buffer Disable for EP 11 */
#define USB_TXDPKTBUFDIS_EP11_Pos           (11U)
#define USB_TXDPKTBUFDIS_EP11_Msk           (0x1UL << USB_TXDPKTBUFDIS_EP11_Pos)
#define USB_TXDPKTBUFDIS_EP11               USB_TXDPKTBUFDIS_EP11_Msk
/** @brief TXDPKTBUFDIS[12]: Tx Double Packet Buffer Disable for EP 12 */
#define USB_TXDPKTBUFDIS_EP12_Pos           (12U)
#define USB_TXDPKTBUFDIS_EP12_Msk           (0x1UL << USB_TXDPKTBUFDIS_EP12_Pos)
#define USB_TXDPKTBUFDIS_EP12               USB_TXDPKTBUFDIS_EP12_Msk
/** @brief TXDPKTBUFDIS[13]: Tx Double Packet Buffer Disable for EP 13 */
#define USB_TXDPKTBUFDIS_EP13_Pos           (13U)
#define USB_TXDPKTBUFDIS_EP13_Msk           (0x1UL << USB_TXDPKTBUFDIS_EP13_Pos)
#define USB_TXDPKTBUFDIS_EP13               USB_TXDPKTBUFDIS_EP13_Msk
/** @brief TXDPKTBUFDIS[14]: Tx Double Packet Buffer Disable for EP 14 */
#define USB_TXDPKTBUFDIS_EP14_Pos           (14U)
#define USB_TXDPKTBUFDIS_EP14_Msk           (0x1UL << USB_TXDPKTBUFDIS_EP14_Pos)
#define USB_TXDPKTBUFDIS_EP14               USB_TXDPKTBUFDIS_EP14_Msk
/** @brief TXDPKTBUFDIS[15]: Tx Double Packet Buffer Disable for EP 15 */
#define USB_TXDPKTBUFDIS_EP15_Pos           (15U)
#define USB_TXDPKTBUFDIS_EP15_Msk           (0x1UL << USB_TXDPKTBUFDIS_EP15_Pos)
#define USB_TXDPKTBUFDIS_EP15               USB_TXDPKTBUFDIS_EP15_Msk

/*****************  Bit definition for USB_LPM_ATTR register  *****************/

/** @brief LPM_ATTR[3:0]: The state that the peripheral must transit to after LPM transaction */
#define USB_LPM_ATTR_LINKSTATE_Pos          (0U)
#define USB_LPM_ATTR_LINKSTATE_Msk          (0xFUL << USB_LPM_ATTR_LINKSTATE_Pos)
#define USB_LPM_ATTR_LINKSTATE              USB_LPM_ATTR_LINKSTATE_Msk
/** @brief LPM_ATTR[7:4]: Host Initiated Resume Duration */
#define USB_LPM_ATTR_HIRD_Pos               (4U)
#define USB_LPM_ATTR_HIRD_Msk               (0xFUL << USB_LPM_ATTR_HIRD_Pos)
#define USB_LPM_ATTR_HIRD                   USB_LPM_ATTR_HIRD_Msk
/** @brief LPM_ATTR[8]: Remote wakeup enable */
#define USB_LPM_ATTR_RMTWAK_Pos             (8U)
#define USB_LPM_ATTR_RMTWAK_Msk             (0x1UL << USB_LPM_ATTR_RMTWAK_Pos)
#define USB_LPM_ATTR_RMTWAK                 USB_LPM_ATTR_RMTWAK_Msk
/** @brief LPM_ATTR[15:12]: EP in the Token Packet of the LPM transaction */
#define USB_LPM_ATTR_ENDPNT_Pos             (12U)
#define USB_LPM_ATTR_ENDPNT_Msk             (0xFUL << USB_LPM_ATTR_ENDPNT_Pos)
#define USB_LPM_ATTR_ENDPNT                 USB_LPM_ATTR_ENDPNT_Msk

/*****************  Bit definition for USB_LPM_CNTRL register  ****************/

/** @brief LPM_CNTRL[0]: Peripheral: Transit to L1 state on receipt of the next LPM transaction \
                         Host: Transmit an LPM transaction */
#define USB_LPM_CNTRL_LPMXMT_Pos            (0U)
#define USB_LPM_CNTRL_LPMXMT_Msk            (0x1UL << USB_LPM_CNTRL_LPMXMT_Pos)
#define USB_LPM_CNTRL_LPMXMT                USB_LPM_CNTRL_LPMXMT_Msk
/** @brief LPM_CNTRL[1]: Peripheral: Initiate resume (remote wakeup) \
                         Host: Initiate resume from the L1 state */
#define USB_LPM_CNTRL_LPMRES_Pos            (1U)
#define USB_LPM_CNTRL_LPMRES_Msk            (0x1UL << USB_LPM_CNTRL_LPMRES_Pos)
#define USB_LPM_CNTRL_LPMRES                USB_LPM_CNTRL_LPMRES_Msk
/** @brief LPM_CNTRL[3:2]: Peripheral: Enable LPM */
#define USB_LPM_CNTRL_LPMEN_Pos             (2U)
#define USB_LPM_CNTRL_LPMEN_Msk             (0x3UL << USB_LPM_CNTRL_LPMEN_Pos)
#define USB_LPM_CNTRL_LPMEN                 USB_LPM_CNTRL_LPMEN_Msk
/** @brief LPM_CNTRL[4]: Peripheral: Set all EPs to response NAK to all transactions except LPM */
#define USB_LPM_CNTRL_LPMNAK_Pos            (4U)
#define USB_LPM_CNTRL_LPMNAK_Msk            (0x1UL << USB_LPM_CNTRL_LPMNAK_Pos)
#define USB_LPM_CNTRL_LPMNAK                USB_LPM_CNTRL_LPMNAK_Msk

/****************  Bit definition for USB_LPM_INTREN register  ****************/

/** @brief LPM_INTREN[0]: Enable LPMST interrupt */
#define USB_LPM_INTREN_LPMSTEN_Pos          (0U)
#define USB_LPM_INTREN_LPMSTEN_Msk          (0x1UL << USB_LPM_INTREN_LPMSTEN_Pos)
#define USB_LPM_INTREN_LPMSTEN              USB_LPM_INTREN_LPMSTEN_Msk
/** @brief LPM_INTREN[1]: Enable LPMNY interrupt */
#define USB_LPM_INTREN_LPMNYEN_Pos          (1U)
#define USB_LPM_INTREN_LPMNYEN_Msk          (0x1UL << USB_LPM_INTREN_LPMNYEN_Pos)
#define USB_LPM_INTREN_LPMNYEN              USB_LPM_INTREN_LPMNYEN_Msk
/** @brief LPM_INTREN[2]: Enable LPMACK interrupt */
#define USB_LPM_INTREN_LPMACKEN_Pos         (2U)
#define USB_LPM_INTREN_LPMACKEN_Msk         (0x1UL << USB_LPM_INTREN_LPMACKEN_Pos)
#define USB_LPM_INTREN_LPMACKEN             USB_LPM_INTREN_LPMACKEN_Msk
/** @brief LPM_INTREN[3]: Enable LPMNC interrupt */
#define USB_LPM_INTREN_LPMNCEN_Pos          (3U)
#define USB_LPM_INTREN_LPMNCEN_Msk          (0x1UL << USB_LPM_INTREN_LPMNCEN_Pos)
#define USB_LPM_INTREN_LPMNCEN              USB_LPM_INTREN_LPMNCEN_Msk
/** @brief LPM_INTREN[4]: Enable LPMRES interrupt */
#define USB_LPM_INTREN_LPMRESEN_Pos         (4U)
#define USB_LPM_INTREN_LPMRESEN_Msk         (0x1UL << USB_LPM_INTREN_LPMRESEN_Pos)
#define USB_LPM_INTREN_LPMRESEN             USB_LPM_INTREN_LPMRESEN_Msk
/** @brief LPM_INTREN[5]: Enable LPMERR interrupt */
#define USB_LPM_INTREN_LPMERREN_Pos         (5U)
#define USB_LPM_INTREN_LPMERREN_Msk         (0x1UL << USB_LPM_INTREN_LPMERREN_Pos)
#define USB_LPM_INTREN_LPMERREN             USB_LPM_INTREN_LPMERREN_Msk

/*****************  Bit definition for USB_LPM_INTR register  *****************/

/** @brief LPM_INTR[0]: Peripheral: LPM transaction received and responded with STALL \
                        Host: LPM transaction transmitted and STALL received */
#define USB_LPM_INTR_LPMST_Pos              (0U)
#define USB_LPM_INTR_LPMST_Msk              (0x1UL << USB_LPM_INTR_LPMST_Pos)
#define USB_LPM_INTR_LPMST                  USB_LPM_INTR_LPMST_Msk
/** @brief LPM_INTR[1]: Peripheral: LPM transaction received and responded with NYET \
                        Host: LPM transaction transmitted and NYET received */
#define USB_LPM_INTR_LPMNY_Pos              (1U)
#define USB_LPM_INTR_LPMNY_Msk              (0x1UL << USB_LPM_INTR_LPMNY_Pos)
#define USB_LPM_INTR_LPMNY                  USB_LPM_INTR_LPMNY_Msk
/** @brief LPM_INTR[2]: Peripheral: LPM transaction received and responded with ACK \
                        Host: LPM transaction transmitted and ACK received */
#define USB_LPM_INTR_LPMACK_Pos             (2U)
#define USB_LPM_INTR_LPMACK_Msk             (0x1UL << USB_LPM_INTR_LPMACK_Pos)
#define USB_LPM_INTR_LPMACK                 USB_LPM_INTR_LPMACK_Msk
/** @brief LPM_INTR[3]: Peripheral: LPM transaction received and responded with NYET due to data pending in RX FIFO \
                        Host: LPM transaction transmitted and failed to complete */
#define USB_LPM_INTR_LPMNC_Pos              (3U)
#define USB_LPM_INTR_LPMNC_Msk              (0x1UL << USB_LPM_INTR_LPMNC_Pos)
#define USB_LPM_INTR_LPMNC                  USB_LPM_INTR_LPMNC_Msk
/** @brief LPM_INTR[4]: Peripheral: USB has been resumed for any reason \
                        Host: USB has been resumed for any reason */
#define USB_LPM_INTR_LPMRES_Pos             (4U)
#define USB_LPM_INTR_LPMRES_Msk             (0x1UL << USB_LPM_INTR_LPMRES_Pos)
#define USB_LPM_INTR_LPMRES                 USB_LPM_INTR_LPMRES_Msk
/** @brief LPM_INTR[5]: Peripheral: LPM transaction received with LinkState field that is not supported \
                        Host: Response to the LPM transaction received with BitStuff or PID error */
#define USB_LPM_INTR_LPMERR_Pos             (5U)
#define USB_LPM_INTR_LPMERR_Msk             (0x1UL << USB_LPM_INTR_LPMERR_Pos)
#define USB_LPM_INTR_LPMERR                 USB_LPM_INTR_LPMERR_Msk

/*****************  Bit definition for USB_LPM_FADDR register  ****************/

/** @brief LPM_FADDR[6:0]: The LPM function address */
#define USB_LPM_FADDR_ADDR_Pos              (0U)
#define USB_LPM_FADDR_ADDR_Msk              (0x7FUL << USB_LPM_FADDR_ADDR_Pos)
#define USB_LPM_FADDR_ADDR                  USB_LPM_FADDR_ADDR_Msk

/******************************************************************************/
/*                                    TIM                                     */
/******************************************************************************/

/************** Bit definition for TIM_TIMER0LOADCOUNT register ***************/

/** @brief TIMER0LOADCOUNT[31:0]: Timer 1 Counter */
#define TIM_TIMER0LOADCOUNT_CNT_Pos         (0U)
#define TIM_TIMER0LOADCOUNT_CNT_Msk         (0xFFFFFFFFUL << TIM_TIMER0LOADCOUNT_CNT_Pos)
#define TIM_TIMER0LOADCOUNT_CNT             TIM_TIMER0LOADCOUNT_CNT_Msk


/************** Bit definition for TIM_TIMER0CURRENTVAL register **************/

/** @brief TIMER0CURRENTVAL[31:0]: Timer 0 Current Value */
#define TIM_TIMER0CURRENTVAL_CNT_Pos        (0U)
#define TIM_TIMER0CURRENTVAL_CNT_Msk        (0xFFFFFFFFUL << TIM_TIMER0CURRENTVAL_CNT_Pos)
#define TIM_TIMER0CURRENTVAL_CNT            TIM_TIMER0CURRENTVAL_CNT_Msk

/************** Bit definition for TIM_TIMER0CONTROLREG register **************/

/** @brief TIMER0CONTROLREG[0]: Enable Timer 0 */
#define TIM_TIMER0CONTROLREG_TIMER_ENABLE_Pos (0U)
#define TIM_TIMER0CONTROLREG_TIMER_ENABLE_Msk (0x1UL << TIM_TIMER0CONTROLREG_TIMER_ENABLE_Pos)
#define TIM_TIMER0CONTROLREG_TIMER_ENABLE   TIM_TIMER0CONTROLREG_TIMER_ENABLE_Msk

#define TIM_TIMER0CONTROLREG_TIMER_ENABLE_FALSE (0x0UL << TIM_TIMER0CONTROLREG_TIMER_ENABLE_Pos) /*!< False */
#define TIM_TIMER0CONTROLREG_TIMER_ENABLE_TRUE (0x1UL << TIM_TIMER0CONTROLREG_TIMER_ENABLE_Pos) /*!< True */

/** @brief TIMER0CONTROLREG[1]: Timer 0 mode */
#define TIM_TIMER0CONTROLREG_TIMER_MODE_Pos (1U)
#define TIM_TIMER0CONTROLREG_TIMER_MODE_Msk (0x1UL << TIM_TIMER0CONTROLREG_TIMER_MODE_Pos)
#define TIM_TIMER0CONTROLREG_TIMER_MODE     TIM_TIMER0CONTROLREG_TIMER_MODE_Msk

#define TIM_TIMER0CONTROLREG_TIMER_MODE_0   (0x0UL << TIM_TIMER0CONTROLREG_TIMER_MODE_Pos) /*!< Free Running mode */
#define TIM_TIMER0CONTROLREG_TIMER_MODE_1   (0x1UL << TIM_TIMER0CONTROLREG_TIMER_MODE_Pos) /*!< User-Defined mode */

/** @brief TIMER0CONTROLREG[2]: Mask Timer 0 interrupt */
#define TIM_TIMER0CONTROLREG_TIMER_INTERRUPT_MASK_Pos (2U)
#define TIM_TIMER0CONTROLREG_TIMER_INTERRUPT_MASK_Msk (0x1UL << TIM_TIMER0CONTROLREG_TIMER_INTERRUPT_MASK_Pos)
#define TIM_TIMER0CONTROLREG_TIMER_INTERRUPT_MASK TIM_TIMER0CONTROLREG_TIMER_INTERRUPT_MASK_Msk

#define TIM_TIMER0CONTROLREG_TIMER_INTERRUPT_MASK_FALSE (0x0UL << TIM_TIMER0CONTROLREG_TIMER_INTERRUPT_MASK_Pos) /*!< False */
#define TIM_TIMER0CONTROLREG_TIMER_INTERRUPT_MASK_TRUE (0x1UL << TIM_TIMER0CONTROLREG_TIMER_INTERRUPT_MASK_Pos) /*!< True */

/** @brief TIMER0CONTROLREG[3]: Enable PWM for timer_0_toggle output */
#define TIM_TIMER0CONTROLREG_TIMER_PWM_Pos  (3U)
#define TIM_TIMER0CONTROLREG_TIMER_PWM_Msk  (0x1UL << TIM_TIMER0CONTROLREG_TIMER_PWM_Pos)
#define TIM_TIMER0CONTROLREG_TIMER_PWM      TIM_TIMER0CONTROLREG_TIMER_PWM_Msk

#define TIM_TIMER0CONTROLREG_TIMER_PWM_FALSE (0x0UL << TIM_TIMER0CONTROLREG_TIMER_PWM_Pos) /*!< False */
#define TIM_TIMER0CONTROLREG_TIMER_PWM_TRUE (0x1UL << TIM_TIMER0CONTROLREG_TIMER_PWM_Pos) /*!< True */

/** @brief TIMER0CONTROLREG[4]: Enable Timer 0 0% and 100% PWM duty cycle mode */
#define TIM_TIMER0CONTROLREG_TIMER_0N100PWM_EN_Pos (4U)
#define TIM_TIMER0CONTROLREG_TIMER_0N100PWM_EN_Msk (0x1UL << TIM_TIMER0CONTROLREG_TIMER_0N100PWM_EN_Pos)
#define TIM_TIMER0CONTROLREG_TIMER_0N100PWM_EN TIM_TIMER0CONTROLREG_TIMER_0N100PWM_EN_Msk

#define TIM_TIMER0CONTROLREG_TIMER_0N100PWM_EN_FALSE (0x0UL << TIM_TIMER0CONTROLREG_TIMER_0N100PWM_EN_Pos) /*!< False */
#define TIM_TIMER0CONTROLREG_TIMER_0N100PWM_EN_TRUE (0x1UL << TIM_TIMER0CONTROLREG_TIMER_0N100PWM_EN_Pos) /*!< True */

/***************** Bit definition for TIM_TIMER0EOI register ******************/

/** @brief TIMER0EOI[0]: Timer 0 End Of Interrupt Register */
#define TIM_TIMER0EOI_INTR_CLR_Pos          (0U)
#define TIM_TIMER0EOI_INTR_CLR_Msk          (0x1UL << TIM_TIMER0EOI_INTR_CLR_Pos)
#define TIM_TIMER0EOI_INTR_CLR              TIM_TIMER0EOI_INTR_CLR_Msk

/*************** Bit definition for TIM_TIMER0INTSTAT register ****************/

/** @brief TIMER0INTSTAT[0]: Timer 0 Interrupt flag */
#define TIM_TIMER0INTSTAT_INTSTAT_Pos       (0U)
#define TIM_TIMER0INTSTAT_INTSTAT_Msk       (0x1UL << TIM_TIMER0INTSTAT_INTSTAT_Pos)
#define TIM_TIMER0INTSTAT_INTSTAT           TIM_TIMER0INTSTAT_INTSTAT_Msk

#define TIM_TIMER0INTSTAT_INTSTAT_FALSE     (0x0UL << TIM_TIMER0INTSTAT_INTSTAT_Pos) /*!< False */
#define TIM_TIMER0INTSTAT_INTSTAT_TRUE      (0x1UL << TIM_TIMER0INTSTAT_INTSTAT_Pos) /*!< True */

/************** Bit definition for TIM_TIMER1LOADCOUNT register ***************/

/** @brief TIMER1LOADCOUNT[31:0]: Timer 1 Counter */
#define TIM_TIMER1LOADCOUNT_CNT_Pos         (0U)
#define TIM_TIMER1LOADCOUNT_CNT_Msk         (0xFFFFFFFFUL << TIM_TIMER1LOADCOUNT_CNT_Pos)
#define TIM_TIMER1LOADCOUNT_CNT             TIM_TIMER1LOADCOUNT_CNT_Msk

/************** Bit definition for TIM_TIMER1CURRENTVAL register **************/

/** @brief TIMER1CURRENTVAL[31:0]: Timer 1 Current Value */
#define TIM_TIMER1CURRENTVAL_CNT_Pos        (0U)
#define TIM_TIMER1CURRENTVAL_CNT_Msk        (0xFFFFFFFFUL << TIM_TIMER1CURRENTVAL_CNT_Pos)
#define TIM_TIMER1CURRENTVAL_CNT            TIM_TIMER1CURRENTVAL_CNT_Msk

/************** Bit definition for TIM_TIMER1CONTROLREG register **************/

/** @brief TIMER1CONTROLREG[0]: Enable Timer 1 */
#define TIM_TIMER1CONTROLREG_TIMER_ENABLE_Pos (0U)
#define TIM_TIMER1CONTROLREG_TIMER_ENABLE_Msk (0x1UL << TIM_TIMER1CONTROLREG_TIMER_ENABLE_Pos)
#define TIM_TIMER1CONTROLREG_TIMER_ENABLE   TIM_TIMER1CONTROLREG_TIMER_ENABLE_Msk

#define TIM_TIMER1CONTROLREG_TIMER_ENABLE_FALSE (0x0UL << TIM_TIMER1CONTROLREG_TIMER_ENABLE_Pos) /*!< False */
#define TIM_TIMER1CONTROLREG_TIMER_ENABLE_TRUE (0x1UL << TIM_TIMER1CONTROLREG_TIMER_ENABLE_Pos) /*!< True */

/** @brief TIMER1CONTROLREG[1]: Timer 1 mode */
#define TIM_TIMER1CONTROLREG_TIMER_MODE_Pos (1U)
#define TIM_TIMER1CONTROLREG_TIMER_MODE_Msk (0x1UL << TIM_TIMER1CONTROLREG_TIMER_MODE_Pos)
#define TIM_TIMER1CONTROLREG_TIMER_MODE     TIM_TIMER1CONTROLREG_TIMER_MODE_Msk

#define TIM_TIMER1CONTROLREG_TIMER_MODE_0   (0x0UL << TIM_TIMER1CONTROLREG_TIMER_MODE_Pos) /*!< Free Running mode */
#define TIM_TIMER1CONTROLREG_TIMER_MODE_1   (0x1UL << TIM_TIMER1CONTROLREG_TIMER_MODE_Pos) /*!< User-Defined mode */

/** @brief TIMER1CONTROLREG[2]: Mask Timer 1 interrupt */
#define TIM_TIMER1CONTROLREG_TIMER_INTERRUPT_MASK_Pos (2U)
#define TIM_TIMER1CONTROLREG_TIMER_INTERRUPT_MASK_Msk (0x1UL << TIM_TIMER1CONTROLREG_TIMER_INTERRUPT_MASK_Pos)
#define TIM_TIMER1CONTROLREG_TIMER_INTERRUPT_MASK TIM_TIMER1CONTROLREG_TIMER_INTERRUPT_MASK_Msk

#define TIM_TIMER1CONTROLREG_TIMER_INTERRUPT_MASK_FALSE (0x0UL << TIM_TIMER1CONTROLREG_TIMER_INTERRUPT_MASK_Pos) /*!< False */
#define TIM_TIMER1CONTROLREG_TIMER_INTERRUPT_MASK_TRUE (0x1UL << TIM_TIMER1CONTROLREG_TIMER_INTERRUPT_MASK_Pos) /*!< True */

/** @brief TIMER1CONTROLREG[3]: Enable PWM for timer_1_toggle output */
#define TIM_TIMER1CONTROLREG_TIMER_PWM_Pos  (3U)
#define TIM_TIMER1CONTROLREG_TIMER_PWM_Msk  (0x1UL << TIM_TIMER1CONTROLREG_TIMER_PWM_Pos)
#define TIM_TIMER1CONTROLREG_TIMER_PWM      TIM_TIMER1CONTROLREG_TIMER_PWM_Msk

#define TIM_TIMER1CONTROLREG_TIMER_PWM_FALSE (0x0UL << TIM_TIMER1CONTROLREG_TIMER_PWM_Pos) /*!< False */
#define TIM_TIMER1CONTROLREG_TIMER_PWM_TRUE (0x1UL << TIM_TIMER1CONTROLREG_TIMER_PWM_Pos) /*!< True */

/** @brief TIMER1CONTROLREG[4]: Enable Timer 1 0% and 100% PWM duty cycle mode */
#define TIM_TIMER1CONTROLREG_TIMER_0N100PWM_EN_Pos (4U)
#define TIM_TIMER1CONTROLREG_TIMER_0N100PWM_EN_Msk (0x1UL << TIM_TIMER1CONTROLREG_TIMER_0N100PWM_EN_Pos)
#define TIM_TIMER1CONTROLREG_TIMER_0N100PWM_EN TIM_TIMER1CONTROLREG_TIMER_0N100PWM_EN_Msk

#define TIM_TIMER1CONTROLREG_TIMER_0N100PWM_EN_FALSE (0x0UL << TIM_TIMER1CONTROLREG_TIMER_0N100PWM_EN_Pos) /*!< False */
#define TIM_TIMER1CONTROLREG_TIMER_0N100PWM_EN_TRUE (0x1UL << TIM_TIMER1CONTROLREG_TIMER_0N100PWM_EN_Pos) /*!< True */

/***************** Bit definition for TIM_TIMER1EOI register ******************/

/** @brief TIMER1EOI[0]: Timer 1 End Of Interrupt Register */
#define TIM_TIMER1EOI_INTR_CLR_Pos          (0U)
#define TIM_TIMER1EOI_INTR_CLR_Msk          (0x1UL << TIM_TIMER1EOI_INTR_CLR_Pos)
#define TIM_TIMER1EOI_INTR_CLR              TIM_TIMER1EOI_INTR_CLR_Msk

/*************** Bit definition for TIM_TIMER1INTSTAT register ****************/

/** @brief TIMER1INTSTAT[0]: Timer 1 Interrupt flag */
#define TIM_TIMER1INTSTAT_INTSTAT_Pos       (0U)
#define TIM_TIMER1INTSTAT_INTSTAT_Msk       (0x1UL << TIM_TIMER1INTSTAT_INTSTAT_Pos)
#define TIM_TIMER1INTSTAT_INTSTAT           TIM_TIMER1INTSTAT_INTSTAT_Msk

#define TIM_TIMER1INTSTAT_INTSTAT_FALSE     (0x0UL << TIM_TIMER1INTSTAT_INTSTAT_Pos) /*!< False */
#define TIM_TIMER1INTSTAT_INTSTAT_TRUE      (0x1UL << TIM_TIMER1INTSTAT_INTSTAT_Pos) /*!< True */

/************** Bit definition for TIM_TIMER2LOADCOUNT register ***************/

/** @brief TIMER2LOADCOUNT[31:0]: Timer 2 Counter */
#define TIM_TIMER2LOADCOUNT_CNT_Pos         (0U)
#define TIM_TIMER2LOADCOUNT_CNT_Msk         (0xFFFFFFFFUL << TIM_TIMER2LOADCOUNT_CNT_Pos)
#define TIM_TIMER2LOADCOUNT_CNT             TIM_TIMER2LOADCOUNT_CNT_Msk

/************** Bit definition for TIM_TIMER2CURRENTVAL register **************/

/** @brief TIMER2CURRENTVAL[31:0]: Timer 2 Current Value */
#define TIM_TIMER2CURRENTVAL_CNT_Pos        (0U)
#define TIM_TIMER2CURRENTVAL_CNT_Msk        (0xFFFFFFFFUL << TIM_TIMER2CURRENTVAL_CNT_Pos)
#define TIM_TIMER2CURRENTVAL_CNT            TIM_TIMER2CURRENTVAL_CNT_Msk

/************** Bit definition for TIM_TIMER2CONTROLREG register **************/

/** @brief TIMER2CONTROLREG[0]: Enable Timer 2 */
#define TIM_TIMER2CONTROLREG_TIMER_ENABLE_Pos (0U)
#define TIM_TIMER2CONTROLREG_TIMER_ENABLE_Msk (0x1UL << TIM_TIMER2CONTROLREG_TIMER_ENABLE_Pos)
#define TIM_TIMER2CONTROLREG_TIMER_ENABLE   TIM_TIMER2CONTROLREG_TIMER_ENABLE_Msk

#define TIM_TIMER2CONTROLREG_TIMER_ENABLE_FALSE (0x0UL << TIM_TIMER2CONTROLREG_TIMER_ENABLE_Pos) /*!< False */
#define TIM_TIMER2CONTROLREG_TIMER_ENABLE_TRUE (0x1UL << TIM_TIMER2CONTROLREG_TIMER_ENABLE_Pos) /*!< True */

/** @brief TIMER2CONTROLREG[1]: Timer 2 mode */
#define TIM_TIMER2CONTROLREG_TIMER_MODE_Pos (1U)
#define TIM_TIMER2CONTROLREG_TIMER_MODE_Msk (0x1UL << TIM_TIMER2CONTROLREG_TIMER_MODE_Pos)
#define TIM_TIMER2CONTROLREG_TIMER_MODE     TIM_TIMER2CONTROLREG_TIMER_MODE_Msk

#define TIM_TIMER2CONTROLREG_TIMER_MODE_0   (0x0UL << TIM_TIMER2CONTROLREG_TIMER_MODE_Pos) /*!< Free Running mode */
#define TIM_TIMER2CONTROLREG_TIMER_MODE_1   (0x1UL << TIM_TIMER2CONTROLREG_TIMER_MODE_Pos) /*!< User-Defined mode */

/** @brief TIMER2CONTROLREG[2]: Mask Timer 2 interrupt */
#define TIM_TIMER2CONTROLREG_TIMER_INTERRUPT_MASK_Pos (2U)
#define TIM_TIMER2CONTROLREG_TIMER_INTERRUPT_MASK_Msk (0x1UL << TIM_TIMER2CONTROLREG_TIMER_INTERRUPT_MASK_Pos)
#define TIM_TIMER2CONTROLREG_TIMER_INTERRUPT_MASK TIM_TIMER2CONTROLREG_TIMER_INTERRUPT_MASK_Msk

#define TIM_TIMER2CONTROLREG_TIMER_INTERRUPT_MASK_FALSE (0x0UL << TIM_TIMER2CONTROLREG_TIMER_INTERRUPT_MASK_Pos) /*!< False */
#define TIM_TIMER2CONTROLREG_TIMER_INTERRUPT_MASK_TRUE (0x1UL << TIM_TIMER2CONTROLREG_TIMER_INTERRUPT_MASK_Pos) /*!< True */

/** @brief TIMER2CONTROLREG[3]: Enable PWM for timer_2_toggle output */
#define TIM_TIMER2CONTROLREG_TIMER_PWM_Pos  (3U)
#define TIM_TIMER2CONTROLREG_TIMER_PWM_Msk  (0x1UL << TIM_TIMER2CONTROLREG_TIMER_PWM_Pos)
#define TIM_TIMER2CONTROLREG_TIMER_PWM      TIM_TIMER2CONTROLREG_TIMER_PWM_Msk

#define TIM_TIMER2CONTROLREG_TIMER_PWM_FALSE (0x0UL << TIM_TIMER2CONTROLREG_TIMER_PWM_Pos) /*!< False */
#define TIM_TIMER2CONTROLREG_TIMER_PWM_TRUE (0x1UL << TIM_TIMER2CONTROLREG_TIMER_PWM_Pos) /*!< True */

/** @brief TIMER2CONTROLREG[4]: Enable Timer 2 0% and 100% PWM duty cycle mode */
#define TIM_TIMER2CONTROLREG_TIMER_0N100PWM_EN_Pos (4U)
#define TIM_TIMER2CONTROLREG_TIMER_0N100PWM_EN_Msk (0x1UL << TIM_TIMER2CONTROLREG_TIMER_0N100PWM_EN_Pos)
#define TIM_TIMER2CONTROLREG_TIMER_0N100PWM_EN TIM_TIMER2CONTROLREG_TIMER_0N100PWM_EN_Msk

#define TIM_TIMER2CONTROLREG_TIMER_0N100PWM_EN_FALSE (0x0UL << TIM_TIMER2CONTROLREG_TIMER_0N100PWM_EN_Pos) /*!< False */
#define TIM_TIMER2CONTROLREG_TIMER_0N100PWM_EN_TRUE (0x1UL << TIM_TIMER2CONTROLREG_TIMER_0N100PWM_EN_Pos) /*!< True */

/***************** Bit definition for TIM_TIMER2EOI register ******************/

/** @brief TIMER2EOI[0]: Timer 2 End Of Interrupt Register */
#define TIM_TIMER2EOI_INTR_CLR_Pos          (0U)
#define TIM_TIMER2EOI_INTR_CLR_Msk          (0x1UL << TIM_TIMER2EOI_INTR_CLR_Pos)
#define TIM_TIMER2EOI_INTR_CLR              TIM_TIMER2EOI_INTR_CLR_Msk

/*************** Bit definition for TIM_TIMER2INTSTAT register ****************/

/** @brief TIMER2INTSTAT[0]: Timer 2 Interrupt flag */
#define TIM_TIMER2INTSTAT_INTSTAT_Pos       (0U)
#define TIM_TIMER2INTSTAT_INTSTAT_Msk       (0x1UL << TIM_TIMER2INTSTAT_INTSTAT_Pos)
#define TIM_TIMER2INTSTAT_INTSTAT           TIM_TIMER2INTSTAT_INTSTAT_Msk

#define TIM_TIMER2INTSTAT_INTSTAT_FALSE     (0x0UL << TIM_TIMER2INTSTAT_INTSTAT_Pos) /*!< False */
#define TIM_TIMER2INTSTAT_INTSTAT_TRUE      (0x1UL << TIM_TIMER2INTSTAT_INTSTAT_Pos) /*!< True */

/************** Bit definition for TIM_TIMER3LOADCOUNT register ***************/

/** @brief TIMER3LOADCOUNT[31:0]: Timer 3 Counter */
#define TIM_TIMER3LOADCOUNT_CNT_Pos         (0U)
#define TIM_TIMER3LOADCOUNT_CNT_Msk         (0xFFFFFFFFUL << TIM_TIMER3LOADCOUNT_CNT_Pos)
#define TIM_TIMER3LOADCOUNT_CNT             TIM_TIMER3LOADCOUNT_CNT_Msk

/************** Bit definition for TIM_TIMER3CURRENTVAL register **************/

/** @brief TIMER3CURRENTVAL[31:0]: Timer 3 Current Value */
#define TIM_TIMER3CURRENTVAL_CNT_Pos        (0U)
#define TIM_TIMER3CURRENTVAL_CNT_Msk        (0xFFFFFFFFUL << TIM_TIMER3CURRENTVAL_CNT_Pos)
#define TIM_TIMER3CURRENTVAL_CNT            TIM_TIMER3CURRENTVAL_CNT_Msk

/************** Bit definition for TIM_TIMER3CONTROLREG register **************/

/** @brief TIMER3CONTROLREG[0]: Enable Timer 3 */
#define TIM_TIMER3CONTROLREG_TIMER_ENABLE_Pos (0U)
#define TIM_TIMER3CONTROLREG_TIMER_ENABLE_Msk (0x1UL << TIM_TIMER3CONTROLREG_TIMER_ENABLE_Pos)
#define TIM_TIMER3CONTROLREG_TIMER_ENABLE   TIM_TIMER3CONTROLREG_TIMER_ENABLE_Msk

#define TIM_TIMER3CONTROLREG_TIMER_ENABLE_FALSE (0x0UL << TIM_TIMER3CONTROLREG_TIMER_ENABLE_Pos) /*!< False */
#define TIM_TIMER3CONTROLREG_TIMER_ENABLE_TRUE (0x1UL << TIM_TIMER3CONTROLREG_TIMER_ENABLE_Pos) /*!< True */

/** @brief TIMER3CONTROLREG[1]: Timer 3 mode */
#define TIM_TIMER3CONTROLREG_TIMER_MODE_Pos (1U)
#define TIM_TIMER3CONTROLREG_TIMER_MODE_Msk (0x1UL << TIM_TIMER3CONTROLREG_TIMER_MODE_Pos)
#define TIM_TIMER3CONTROLREG_TIMER_MODE     TIM_TIMER3CONTROLREG_TIMER_MODE_Msk

#define TIM_TIMER3CONTROLREG_TIMER_MODE_0   (0x0UL << TIM_TIMER3CONTROLREG_TIMER_MODE_Pos) /*!< Free Running mode */
#define TIM_TIMER3CONTROLREG_TIMER_MODE_1   (0x1UL << TIM_TIMER3CONTROLREG_TIMER_MODE_Pos) /*!< User-Defined mode */

/** @brief TIMER3CONTROLREG[2]: Mask Timer 3 interrupt */
#define TIM_TIMER3CONTROLREG_TIMER_INTERRUPT_MASK_Pos (2U)
#define TIM_TIMER3CONTROLREG_TIMER_INTERRUPT_MASK_Msk (0x1UL << TIM_TIMER3CONTROLREG_TIMER_INTERRUPT_MASK_Pos)
#define TIM_TIMER3CONTROLREG_TIMER_INTERRUPT_MASK TIM_TIMER3CONTROLREG_TIMER_INTERRUPT_MASK_Msk

#define TIM_TIMER3CONTROLREG_TIMER_INTERRUPT_MASK_FALSE (0x0UL << TIM_TIMER3CONTROLREG_TIMER_INTERRUPT_MASK_Pos) /*!< False */
#define TIM_TIMER3CONTROLREG_TIMER_INTERRUPT_MASK_TRUE (0x1UL << TIM_TIMER3CONTROLREG_TIMER_INTERRUPT_MASK_Pos) /*!< True */

/** @brief TIMER3CONTROLREG[3]: Enable PWM for timer_3_toggle output */
#define TIM_TIMER3CONTROLREG_TIMER_PWM_Pos  (3U)
#define TIM_TIMER3CONTROLREG_TIMER_PWM_Msk  (0x1UL << TIM_TIMER3CONTROLREG_TIMER_PWM_Pos)
#define TIM_TIMER3CONTROLREG_TIMER_PWM      TIM_TIMER3CONTROLREG_TIMER_PWM_Msk

#define TIM_TIMER3CONTROLREG_TIMER_PWM_FALSE (0x0UL << TIM_TIMER3CONTROLREG_TIMER_PWM_Pos) /*!< False */
#define TIM_TIMER3CONTROLREG_TIMER_PWM_TRUE (0x1UL << TIM_TIMER3CONTROLREG_TIMER_PWM_Pos) /*!< True */

/** @brief TIMER3CONTROLREG[4]: Enable Timer 3 0% and 100% PWM duty cycle mode */
#define TIM_TIMER3CONTROLREG_TIMER_0N100PWM_EN_Pos (4U)
#define TIM_TIMER3CONTROLREG_TIMER_0N100PWM_EN_Msk (0x1UL << TIM_TIMER3CONTROLREG_TIMER_0N100PWM_EN_Pos)
#define TIM_TIMER3CONTROLREG_TIMER_0N100PWM_EN TIM_TIMER3CONTROLREG_TIMER_0N100PWM_EN_Msk

#define TIM_TIMER3CONTROLREG_TIMER_0N100PWM_EN_FALSE (0x0UL << TIM_TIMER3CONTROLREG_TIMER_0N100PWM_EN_Pos) /*!< False */
#define TIM_TIMER3CONTROLREG_TIMER_0N100PWM_EN_TRUE (0x1UL << TIM_TIMER3CONTROLREG_TIMER_0N100PWM_EN_Pos) /*!< True */

/***************** Bit definition for TIM_TIMER3EOI register ******************/

/** @brief TIMER3EOI[0]: Timer 3 End Of Interrupt Register */
#define TIM_TIMER3EOI_INTR_CLR_Pos          (0U)
#define TIM_TIMER3EOI_INTR_CLR_Msk          (0x1UL << TIM_TIMER3EOI_INTR_CLR_Pos)
#define TIM_TIMER3EOI_INTR_CLR              TIM_TIMER3EOI_INTR_CLR_Msk

/*************** Bit definition for TIM_TIMER3INTSTAT register ****************/

/** @brief TIMER3INTSTAT[0]: Timer 3 Interrupt flag */
#define TIM_TIMER3INTSTAT_INTSTAT_Pos       (0U)
#define TIM_TIMER3INTSTAT_INTSTAT_Msk       (0x1UL << TIM_TIMER3INTSTAT_INTSTAT_Pos)
#define TIM_TIMER3INTSTAT_INTSTAT           TIM_TIMER3INTSTAT_INTSTAT_Msk

#define TIM_TIMER3INTSTAT_INTSTAT_FALSE     (0x0UL << TIM_TIMER3INTSTAT_INTSTAT_Pos) /*!< False */
#define TIM_TIMER3INTSTAT_INTSTAT_TRUE      (0x1UL << TIM_TIMER3INTSTAT_INTSTAT_Pos) /*!< True */

/************** Bit definition for TIM_TIMERSINTSTATUS register ***************/

/** @brief TIMERSINTSTATUS[3:0]: Contains the interrupt status of all timers */
#define TIM_TIMERSINTSTATUS_INTSTATUS_Pos   (0U)
#define TIM_TIMERSINTSTATUS_INTSTATUS_Msk   (0xFUL << TIM_TIMERSINTSTATUS_INTSTATUS_Pos)
#define TIM_TIMERSINTSTATUS_INTSTATUS       TIM_TIMERSINTSTATUS_INTSTATUS_Msk

/***************** Bit definition for TIM_TIMERSEOI register ******************/

/** @brief TIMERSEOI[3:0]: Contains the end-of-interrupt of all timers */
#define TIM_TIMERSEOI_INTR_CLR_Pos          (0U)
#define TIM_TIMERSEOI_INTR_CLR_Msk          (0xFUL << TIM_TIMERSEOI_INTR_CLR_Pos)
#define TIM_TIMERSEOI_INTR_CLR              TIM_TIMERSEOI_INTR_CLR_Msk

/************** Bit definition for TIM_TIMERSRAWINTSTAT register **************/

/** @brief TIMERSRAWINTSTAT[3:0]: Contains raw value of the end-of-interrupt of all timers */
#define TIM_TIMERSRAWINTSTAT_RAWINTSTAT_Pos (0U)
#define TIM_TIMERSRAWINTSTAT_RAWINTSTAT_Msk (0xFUL << TIM_TIMERSRAWINTSTAT_RAWINTSTAT_Pos)
#define TIM_TIMERSRAWINTSTAT_RAWINTSTAT     TIM_TIMERSRAWINTSTAT_RAWINTSTAT_Msk

/************* Bit definition for TIM_TIMERSCOMPVERSION register **************/

/** @brief TIMERSCOMPVERSION[31:0]: Component Version */
#define TIM_TIMERSCOMPVERSION_COMPVERSION_Pos (0U)
#define TIM_TIMERSCOMPVERSION_COMPVERSION_Msk (0xFFFFFFFFUL << TIM_TIMERSCOMPVERSION_COMPVERSION_Pos)
#define TIM_TIMERSCOMPVERSION_COMPVERSION   TIM_TIMERSCOMPVERSION_COMPVERSION_Msk

/************** Bit definition for TIM_TIMER0LOADCOUNT2 register **************/

/** @brief TIMER0LOADCOUNT2[31:0]: Timer 0 Counter2 */
#define TIM_TIMER0LOADCOUNT2_CNT_Pos        (0U)
#define TIM_TIMER0LOADCOUNT2_CNT_Msk        (0xFFFFFFFFUL << TIM_TIMER0LOADCOUNT2_CNT_Pos)
#define TIM_TIMER0LOADCOUNT2_CNT            TIM_TIMER0LOADCOUNT2_CNT_Msk

/************** Bit definition for TIM_TIMER1LOADCOUNT2 register **************/

/** @brief TIMER1LOADCOUNT2[31:0]: Timer 1 Counter2 */
#define TIM_TIMER1LOADCOUNT2_CNT_Pos        (0U)
#define TIM_TIMER1LOADCOUNT2_CNT_Msk        (0xFFFFFFFFUL << TIM_TIMER1LOADCOUNT2_CNT_Pos)
#define TIM_TIMER1LOADCOUNT2_CNT            TIM_TIMER1LOADCOUNT2_CNT_Msk

/************** Bit definition for TIM_TIMER2LOADCOUNT2 register **************/

/** @brief TIMER2LOADCOUNT2[31:0]: Timer 2 Counter2 */
#define TIM_TIMER2LOADCOUNT2_CNT_Pos        (0U)
#define TIM_TIMER2LOADCOUNT2_CNT_Msk        (0xFFFFFFFFUL << TIM_TIMER2LOADCOUNT2_CNT_Pos)
#define TIM_TIMER2LOADCOUNT2_CNT            TIM_TIMER2LOADCOUNT2_CNT_Msk

/************** Bit definition for TIM_TIMER3LOADCOUNT2 register **************/

/** @brief TIMER3LOADCOUNT2[31:0]: Timer 3 Counter2 */
#define TIM_TIMER3LOADCOUNT2_CNT_Pos        (0U)
#define TIM_TIMER3LOADCOUNT2_CNT_Msk        (0xFFFFFFFFUL << TIM_TIMER3LOADCOUNT2_CNT_Pos)
#define TIM_TIMER3LOADCOUNT2_CNT            TIM_TIMER3LOADCOUNT2_CNT_Msk

/******************************************************************************/
/*                                    PWMA                                    */
/******************************************************************************/

/******************** Bit definition for PWMA_CR1 register ********************/

/** @brief CR1[0]: Counter enable */
#define PWMA_CR1_CEN_Pos                    (0U)
#define PWMA_CR1_CEN_Msk                    (0x1UL << PWMA_CR1_CEN_Pos)
#define PWMA_CR1_CEN                        PWMA_CR1_CEN_Msk                    /*!<Counter enable        */

/** @brief CR1[1]: Update disable */
#define PWMA_CR1_UDIS_Pos                   (1U)
#define PWMA_CR1_UDIS_Msk                   (0x1UL << PWMA_CR1_UDIS_Pos)
#define PWMA_CR1_UDIS                       PWMA_CR1_UDIS_Msk                   /*!<Update disable        */

/** @brief CR1[3]: One pulse mode */
#define PWMA_CR1_OPM_Pos                    (3U)
#define PWMA_CR1_OPM_Msk                    (0x1UL << PWMA_CR1_OPM_Pos)
#define PWMA_CR1_OPM                        PWMA_CR1_OPM_Msk

#define PWMA_CR1_OPM_REPETITIVE             (0x0UL << PWMA_CR1_OPM_Pos)         /*!< One pulse mode disable. Counter is not stopped at update event*/
#define PWMA_CR1_OPM_SINGLE                 (0x1UL << PWMA_CR1_OPM_Pos)         /*!< One pulse mode enable. Counter stops counting at the next update event (clearing the bit CEN) */

/** @brief CR1[4]: Direction */
#define PWMA_CR1_DIR_Pos                    (4U)
#define PWMA_CR1_DIR_Msk                    (0x1UL << PWMA_CR1_DIR_Pos)
#define PWMA_CR1_DIR                        PWMA_CR1_DIR_Msk

#define PWMA_CR1_DIR_UP                     (0x0UL << PWMA_CR1_DIR_Pos)         /*!< Count up */
#define PWMA_CR1_DIR_DOWN                   (0x1UL << PWMA_CR1_DIR_Pos)         /*!< Count down */

/** @brief CR1[6:5]: Center-aligned mode selection */
#define PWMA_CR1_CMS_Pos                    (5U)
#define PWMA_CR1_CMS_Msk                    (0x3UL << PWMA_CR1_CMS_Pos)
#define PWMA_CR1_CMS                        PWMA_CR1_CMS_Msk

#define PWMA_CR1_CMS_00                     (0x0UL << PWMA_CR1_CMS_Pos)         /*!< "Edge-aligned mode" */
#define PWMA_CR1_CMS_01                     (0x1UL << PWMA_CR1_CMS_Pos)         /*!< "Mode 1 with center alignment" */
#define PWMA_CR1_CMS_10                     (0x2UL << PWMA_CR1_CMS_Pos)         /*!< "Mode 2 with center alignment" */
#define PWMA_CR1_CMS_11                     (0x3UL << PWMA_CR1_CMS_Pos)         /*!< "Mode 3 with center alignment" */

/** @brief CR1[7]: Auto-reload preload enable */
#define PWMA_CR1_ARPE_Pos                   (7U)
#define PWMA_CR1_ARPE_Msk                   (0x1UL << PWMA_CR1_ARPE_Pos)
#define PWMA_CR1_ARPE                       PWMA_CR1_ARPE_Msk                   /*!<Auto-reload preload enable     */

/** @brief CR1[9:8]: Clock division */
#define PWMA_CR1_CKD_Pos                    (8U)
#define PWMA_CR1_CKD_Msk                    (0x3UL << PWMA_CR1_CKD_Pos)
#define PWMA_CR1_CKD                        PWMA_CR1_CKD_Msk

#define PWMA_CR1_CKD_00                     (0x0UL << PWMA_CR1_CKD_Pos)         /*!< tDTS=tCK_INT   */
#define PWMA_CR1_CKD_01                     (0x1UL << PWMA_CR1_CKD_Pos)         /*!< tDTS=2*tCK_INT */
#define PWMA_CR1_CKD_10                     (0x2UL << PWMA_CR1_CKD_Pos)         /*!< tDTS=4*tCK_INT */

/******************** Bit definition for PWMA_CR2 register ********************/

/** @brief CR2[0]: Capture/Compare preloaded control */
#define PWMA_CR2_CCPC_Pos                   (0U)
#define PWMA_CR2_CCPC_Msk                   (0x1UL << PWMA_CR2_CCPC_Pos)
#define PWMA_CR2_CCPC                       PWMA_CR2_CCPC_Msk

#define PWMA_CR2_CCPC_DISABLE               (0x0UL << PWMA_CR2_CCPC_Pos)        /*!< CCxE, CCxNE and OCxM bits are not preloaded */
#define PWMA_CR2_CCPC_ENABLE                (0x1UL << PWMA_CR2_CCPC_Pos)        /*!< CCxE, CCxNE and OCxM bits are preloaded */

/** @brief CR2[2]: Capture/compare control update selection */
#define PWMA_CR2_CCUS_Pos                   (2U)
#define PWMA_CR2_CCUS_Msk                   (0x1UL << PWMA_CR2_CCUS_Pos)
#define PWMA_CR2_CCUS                       PWMA_CR2_CCUS_Msk

#define PWMA_CR2_CCUS_0                     (0x0UL << PWMA_CR2_CCUS_Pos)        /*!< If buffering of capture/compare control bits
                                                                                    (CCxE, CCxNE, OCxM) is used;
                                                                                    Update occurs when CR2_CCPC=1 and COMG=1 */
#define PWMA_CR2_CCUS_1                     (0x1UL << PWMA_CR2_CCUS_Pos)        /*!< If buffering of capture/compare control bits
                                                                                    (CCxE, CCxNE, OCxM) is used;
                                                                                    The update occurs when CR2_CCPC=1 and (COMG=1 or posedge(TRGI)) */

/** @brief CR2[6:4]: Master mode selection */
#define PWMA_CR2_MMS_Pos                    (4U)
#define PWMA_CR2_MMS_Msk                    (0x7UL << PWMA_CR2_MMS_Pos)
#define PWMA_CR2_MMS                        PWMA_CR2_MMS_Msk

#define PWMA_CR2_MMS_000                    (0x0UL << PWMA_CR2_MMS_Pos)         /*!< Reset; TRGO=EGR_UG */
#define PWMA_CR2_MMS_001                    (0x1UL << PWMA_CR2_MMS_Pos)         /*!< Enable; TRGO=CEN or TRGI */
#define PWMA_CR2_MMS_010                    (0x2UL << PWMA_CR2_MMS_Pos)         /*!< Update; TRGO=EUV */
#define PWMA_CR2_MMS_011                    (0x3UL << PWMA_CR2_MMS_Pos)         /*!< Compare Pulse; TRGO=CC0IF */
#define PWMA_CR2_MMS_100                    (0x4UL << PWMA_CR2_MMS_Pos)         /*!< Compare; TRGO=OC0REF */
#define PWMA_CR2_MMS_101                    (0x5UL << PWMA_CR2_MMS_Pos)         /*!< Compare; TRGO=OC1REF */
#define PWMA_CR2_MMS_110                    (0x6UL << PWMA_CR2_MMS_Pos)         /*!< Compare; TRGO=OC2REF */
#define PWMA_CR2_MMS_111                    (0x7UL << PWMA_CR2_MMS_Pos)         /*!< Compare; TRGO=OC3REF */

/** @brief CR2[7]: TI0 selection */
#define PWMA_CR2_TI0S_Pos                   (7U)
#define PWMA_CR2_TI0S_Msk                   (0x1UL << PWMA_CR2_TI0S_Pos)
#define PWMA_CR2_TI0S                       PWMA_CR2_TI0S_Msk

/** @brief CR2[8]: Output Idle state 0 (OC0 output) */
#define PWMA_CR2_OIS0_Pos                   (8U)
#define PWMA_CR2_OIS0_Msk                   (0x1UL << PWMA_CR2_OIS0_Pos)
#define PWMA_CR2_OIS0                       PWMA_CR2_OIS0_Msk

#define PWMA_CR2_OIS0_IDLE_LOW              (0x0UL << PWMA_CR2_OIS0_Pos)        /*!< OC0=0 (after a dead-time if OC is implemented) when MOE=0 */
#define PWMA_CR2_OIS0_IDLE_HIGH             (0x1UL << PWMA_CR2_OIS0_Pos)        /*!< OC0=1 (after a dead-time if OC is implemented) when MOE=0 */

/** @brief CR2[9]: Output Idle state 0 (OC0N output) */
#define PWMA_CR2_OIS0N_Pos                  (9U)
#define PWMA_CR2_OIS0N_Msk                  (0x1UL << PWMA_CR2_OIS0N_Pos)
#define PWMA_CR2_OIS0N                      PWMA_CR2_OIS0N_Msk

#define PWMA_CR2_OIS0N_IDLE_LOW             (0x0UL << PWMA_CR2_OIS0N_Pos)       /*!< OC0N=0 after a dead-time when MOE=0 */
#define PWMA_CR2_OIS0N_IDLE_HIGH            (0x1UL << PWMA_CR2_OIS0N_Pos)       /*!< OC0N=1 after a dead-time when MOE=0 */

/** @brief CR2[10]: Output Idle state 1 (OC1 output) */
#define PWMA_CR2_OIS1_Pos                   (10U)
#define PWMA_CR2_OIS1_Msk                   (0x1UL << PWMA_CR2_OIS1_Pos)
#define PWMA_CR2_OIS1                       PWMA_CR2_OIS1_Msk

#define PWMA_CR2_OIS1_IDLE_LOW              (0x0UL << PWMA_CR2_OIS1_Pos)        /*!< OC1=0 (after a dead-time if OC is implemented) when MOE=0 */
#define PWMA_CR2_OIS1_IDLE_HIGH             (0x1UL << PWMA_CR2_OIS1_Pos)        /*!< OC1=1 (after a dead-time if OC is implemented) when MOE=0 */

/** @brief CR2[11]: Output Idle state 1 (OC1N output) */
#define PWMA_CR2_OIS1N_Pos                  (11U)
#define PWMA_CR2_OIS1N_Msk                  (0x1UL << PWMA_CR2_OIS1N_Pos)
#define PWMA_CR2_OIS1N                      PWMA_CR2_OIS1N_Msk

#define PWMA_CR2_OIS1N_IDLE_LOW             (0x0UL << PWMA_CR2_OIS1N_Pos)       /*!< OC1N=0 after a dead-time when MOE=0 */
#define PWMA_CR2_OIS1N_IDLE_HIGH            (0x1UL << PWMA_CR2_OIS1N_Pos)       /*!< OC1N=1 after a dead-time when MOE=0 */

/** @brief CR2[12]: Output Idle state 2 (OC2 output) */
#define PWMA_CR2_OIS2_Pos                   (12U)
#define PWMA_CR2_OIS2_Msk                   (0x1UL << PWMA_CR2_OIS2_Pos)
#define PWMA_CR2_OIS2                       PWMA_CR2_OIS2_Msk

#define PWMA_CR2_OIS2_IDLE_LOW              (0x0UL << PWMA_CR2_OIS2_Pos)        /*!< OC2=0 (after a dead-time if OC is implemented) when MOE=0 */
#define PWMA_CR2_OIS2_IDLE_HIGH             (0x1UL << PWMA_CR2_OIS2_Pos)        /*!< OC2=1 (after a dead-time if OC is implemented) when MOE=0 */

/** @brief CR2[13]: Output Idle state 2 (OC2N output) */
#define PWMA_CR2_OIS2N_Pos                  (13U)
#define PWMA_CR2_OIS2N_Msk                  (0x1UL << PWMA_CR2_OIS2N_Pos)
#define PWMA_CR2_OIS2N                      PWMA_CR2_OIS2N_Msk

#define PWMA_CR2_OIS2N_IDLE_LOW             (0x0UL << PWMA_CR2_OIS2N_Pos)       /*!< OC2N=1 after a dead-time when MOE=0 */
#define PWMA_CR2_OIS2N_IDLE_HIGH            (0x1UL << PWMA_CR2_OIS2N_Pos)       /*!< OC2N=1 after a dead-time when MOE=0 */


/******************* Bit definition for PWMA_SMCR register ********************/

/** @brief SMCR[2:0]: Slave mode selection */
#define PWMA_SMCR_SMS_Pos                   (0U)
#define PWMA_SMCR_SMS_Msk                   (0x7UL << PWMA_SMCR_SMS_Pos)
#define PWMA_SMCR_SMS                       PWMA_SMCR_SMS_Msk

#define PWMA_SMCR_SMS_000                   (0x0UL << PWMA_SMCR_SMS_Pos)        /*!< Slave mode disabled; At prescaler input: CEN=1 and "internal clock signal" */
#define PWMA_SMCR_SMS_100                   (0x4UL << PWMA_SMCR_SMS_Pos)        /*!< Reset mode; posedge(TRGI) resets the counter and updates the registers */
#define PWMA_SMCR_SMS_101                   (0x5UL << PWMA_SMCR_SMS_Pos)        /*!< Gated mode; The counter is started by posedge  (TRGI);
                                                                                    The counter is stopped by negedge(TRGI); The counter is not reset. */
#define PWMA_SMCR_SMS_110                   (0x6UL << PWMA_SMCR_SMS_Pos)        /*!< Trigger mode; The counter is started by posedge(TRGI); Stopping the counter
                                                                                    is not controlled. The counter is not reset. */
#define PWMA_SMCR_SMS_111                   (0x7UL << PWMA_SMCR_SMS_Pos)        /*!< External  Clock Mode 1; The counter is clocked by posedge(TRGI). */

/** @brief SMCR[6:4]: Trigger selection */
#define PWMA_SMCR_TS_Pos                    (4U)
#define PWMA_SMCR_TS_Msk                    (0x7UL << PWMA_SMCR_TS_Pos)
#define PWMA_SMCR_TS                        PWMA_SMCR_TS_Msk

#define PWMA_SMCR_TS_000                    (0x0UL << PWMA_SMCR_TS_Pos)         /*!< Internal Trigger 0 (ITR0) */
#define PWMA_SMCR_TS_001                    (0x1UL << PWMA_SMCR_TS_Pos)         /*!< Internal Trigger 1 (ITR1) */
#define PWMA_SMCR_TS_010                    (0x2UL << PWMA_SMCR_TS_Pos)         /*!< Internal Trigger 2 (ITR2) */
#define PWMA_SMCR_TS_100                    (0x4UL << PWMA_SMCR_TS_Pos)         /*!< TI1 Edge Detector (TI1F_ED) */
#define PWMA_SMCR_TS_101                    (0x5UL << PWMA_SMCR_TS_Pos)         /*!< Filtered Timer Input 1 (TI1FP1) */
#define PWMA_SMCR_TS_110                    (0x6UL << PWMA_SMCR_TS_Pos)         /*!< Filtered Timer Input 2 (TI1FP2) */
#define PWMA_SMCR_TS_111                    (0x7UL << PWMA_SMCR_TS_Pos)         /*!< External Trigger input (ETRF) */

/** @brief SMCR[11:8]: External trigger filter */
#define PWMA_SMCR_ETF_Pos                   (8U)
#define PWMA_SMCR_ETF_Msk                   (0xFUL << PWMA_SMCR_ETF_Pos)
#define PWMA_SMCR_ETF                       PWMA_SMCR_ETF_Msk

#define PWMA_SMCR_ETF_0000                  (0x0UL << PWMA_SMCR_ETF_Pos)        /*!< f_sampling=f_dts; Filter not used */
#define PWMA_SMCR_ETF_0001                  (0x1UL << PWMA_SMCR_ETF_Pos)        /*!< f_sampling=f_ck_int, N=2 */
#define PWMA_SMCR_ETF_0010                  (0x2UL << PWMA_SMCR_ETF_Pos)        /*!< f_sampling=f_ck_int, N=4 */
#define PWMA_SMCR_ETF_0011                  (0x3UL << PWMA_SMCR_ETF_Pos)        /*!< f_sampling=f_ck_int, N=8 */
#define PWMA_SMCR_ETF_0100                  (0x4UL << PWMA_SMCR_ETF_Pos)        /*!< f_sampling=f_dts/2, N=6 */
#define PWMA_SMCR_ETF_0101                  (0x5UL << PWMA_SMCR_ETF_Pos)        /*!< f_sampling=f_dts/2, N=8 */
#define PWMA_SMCR_ETF_0110                  (0x6UL << PWMA_SMCR_ETF_Pos)        /*!< f_sampling=f_dts/4, N=6 */
#define PWMA_SMCR_ETF_0111                  (0x7UL << PWMA_SMCR_ETF_Pos)        /*!< f_sampling=f_dts/4, N=8 */
#define PWMA_SMCR_ETF_1000                  (0x8UL << PWMA_SMCR_ETF_Pos)        /*!< f_sampling=f_dts/8, N=6 */
#define PWMA_SMCR_ETF_1001                  (0x9UL << PWMA_SMCR_ETF_Pos)        /*!< f_sampling=f_dts/8, N=8 */
#define PWMA_SMCR_ETF_1010                  (0xAUL << PWMA_SMCR_ETF_Pos)        /*!< f_sampling=f_dts/16, N=5 */
#define PWMA_SMCR_ETF_1011                  (0xBUL << PWMA_SMCR_ETF_Pos)        /*!< f_sampling=f_dts/16, N=6 */
#define PWMA_SMCR_ETF_1100                  (0xCUL << PWMA_SMCR_ETF_Pos)        /*!< f_sampling=f_dts/16, N=8 */
#define PWMA_SMCR_ETF_1101                  (0xDUL << PWMA_SMCR_ETF_Pos)        /*!< f_sampling=f_dts/32, N=5 */
#define PWMA_SMCR_ETF_1110                  (0xEUL << PWMA_SMCR_ETF_Pos)        /*!< f_sampling=f_dts/32, N=6 */
#define PWMA_SMCR_ETF_1111                  (0xFUL << PWMA_SMCR_ETF_Pos)        /*!< f_sampling=f_dts/32, N=8 */

/** @brief SMCR[13:12]: External trigger prescaler */
#define PWMA_SMCR_ETPS_Pos                  (12U)
#define PWMA_SMCR_ETPS_Msk                  (0x3UL << PWMA_SMCR_ETPS_Pos)
#define PWMA_SMCR_ETPS                      PWMA_SMCR_ETPS_Msk

#define PWMA_SMCR_ETPS_00                   (0x0UL << PWMA_SMCR_ETPS_Pos)       /*!< 1 */
#define PWMA_SMCR_ETPS_01                   (0x1UL << PWMA_SMCR_ETPS_Pos)       /*!< 2 */
#define PWMA_SMCR_ETPS_10                   (0x2UL << PWMA_SMCR_ETPS_Pos)       /*!< 4 */
#define PWMA_SMCR_ETPS_11                   (0x3UL << PWMA_SMCR_ETPS_Pos)       /*!< 8 */

/** @brief SMCR[14]: External clock enable */
#define PWMA_SMCR_ECE_Pos                   (14U)
#define PWMA_SMCR_ECE_Msk                   (0x1UL << PWMA_SMCR_ECE_Pos)
#define PWMA_SMCR_ECE                       PWMA_SMCR_ECE_Msk

/** @brief SMCR[15]: External trigger polarity */
#define PWMA_SMCR_ETP_Pos                   (15U)
#define PWMA_SMCR_ETP_Msk                   (0x1UL << PWMA_SMCR_ETP_Pos)
#define PWMA_SMCR_ETP                       PWMA_SMCR_ETP_Msk

#define PWMA_SMCR_ETP_NONINV                (0x0UL << PWMA_SMCR_ETP_Pos)        /*!< ETR is non-inverted, active at high level or rising edge */
#define PWMA_SMCR_ETP_INV                   (0x1UL << PWMA_SMCR_ETP_Pos)        /*!< ETR is inverted, active at low level or falling edge */


/******************* Bit definition for PWMA_DIER register ********************/

/** @brief DIER[0]: Update interrupt enable */
#define PWMA_DIER_UIE_Pos                   (0U)
#define PWMA_DIER_UIE_Msk                   (0x1UL << PWMA_DIER_UIE_Pos)
#define PWMA_DIER_UIE                       PWMA_DIER_UIE_Msk

/** @brief DIER[1]: Capture/Compare 0 interrupt enable */
#define PWMA_DIER_CC0IE_Pos                 (1U)
#define PWMA_DIER_CC0IE_Msk                 (0x1UL << PWMA_DIER_CC0IE_Pos)
#define PWMA_DIER_CC0IE                     PWMA_DIER_CC0IE_Msk

/** @brief DIER[2]: Capture/Compare 1 interrupt enable */
#define PWMA_DIER_CC1IE_Pos                 (2U)
#define PWMA_DIER_CC1IE_Msk                 (0x1UL << PWMA_DIER_CC1IE_Pos)
#define PWMA_DIER_CC1IE                     PWMA_DIER_CC1IE_Msk

/** @brief DIER[3]: Capture/Compare 2 interrupt enable */
#define PWMA_DIER_CC2IE_Pos                 (3U)
#define PWMA_DIER_CC2IE_Msk                 (0x1UL << PWMA_DIER_CC2IE_Pos)
#define PWMA_DIER_CC2IE                     PWMA_DIER_CC2IE_Msk

/** @brief DIER[4]: Capture/Compare 3 interrupt enable */
#define PWMA_DIER_CC3IE_Pos                 (4U)
#define PWMA_DIER_CC3IE_Msk                 (0x1UL << PWMA_DIER_CC3IE_Pos)
#define PWMA_DIER_CC3IE                     PWMA_DIER_CC3IE_Msk

/** @brief DIER[5]: COM interrupt enable */
#define PWMA_DIER_COMIE_Pos                 (5U)
#define PWMA_DIER_COMIE_Msk                 (0x1UL << PWMA_DIER_COMIE_Pos)
#define PWMA_DIER_COMIE                     PWMA_DIER_COMIE_Msk

/** @brief DIER[6]: Trigger interrupt enable */
#define PWMA_DIER_TIE_Pos                   (6U)
#define PWMA_DIER_TIE_Msk                   (0x1UL << PWMA_DIER_TIE_Pos)
#define PWMA_DIER_TIE                       PWMA_DIER_TIE_Msk

/** @brief DIER[7]: Break interrupt enable */
#define PWMA_DIER_BIE_Pos                   (7U)
#define PWMA_DIER_BIE_Msk                   (0x1UL << PWMA_DIER_BIE_Pos)
#define PWMA_DIER_BIE                       PWMA_DIER_BIE_Msk

/** @brief DIER[8]: Update DMA request enable */
#define PWMA_DIER_UDE_Pos                   (8U)
#define PWMA_DIER_UDE_Msk                   (0x1UL << PWMA_DIER_UDE_Pos)
#define PWMA_DIER_UDE                       PWMA_DIER_UDE_Msk

/** @brief DIER[9]: Capture/Compare 0 DMA request enable */
#define PWMA_DIER_CC0DE_Pos                 (9U)
#define PWMA_DIER_CC0DE_Msk                 (0x1UL << PWMA_DIER_CC0DE_Pos)
#define PWMA_DIER_CC0DE                     PWMA_DIER_CC0DE_Msk

/** @brief DIER[10]: Capture/Compare 1 DMA request enable */
#define PWMA_DIER_CC1DE_Pos                 (10U)
#define PWMA_DIER_CC1DE_Msk                 (0x1UL << PWMA_DIER_CC1DE_Pos)
#define PWMA_DIER_CC1DE                     PWMA_DIER_CC1DE_Msk

/** @brief DIER[11]: Capture/Compare 2 DMA request enable */
#define PWMA_DIER_CC2DE_Pos                 (11U)
#define PWMA_DIER_CC2DE_Msk                 (0x1UL << PWMA_DIER_CC2DE_Pos)
#define PWMA_DIER_CC2DE                     PWMA_DIER_CC2DE_Msk

/** @brief DIER[12]: Capture/Compare 3 DMA request enable */
#define PWMA_DIER_CC3DE_Pos                 (12U)
#define PWMA_DIER_CC3DE_Msk                 (0x1UL << PWMA_DIER_CC3DE_Pos)
#define PWMA_DIER_CC3DE                     PWMA_DIER_CC3DE_Msk

/** @brief DIER[13]: COM DMA request enable */
#define PWMA_DIER_COMDE_Pos                 (13U)
#define PWMA_DIER_COMDE_Msk                 (0x1UL << PWMA_DIER_COMDE_Pos)
#define PWMA_DIER_COMDE                     PWMA_DIER_COMDE_Msk

/** @brief DIER[14]: Trigger DMA request enable */
#define PWMA_DIER_TDE_Pos                   (14U)
#define PWMA_DIER_TDE_Msk                   (0x1UL << PWMA_DIER_TDE_Pos)
#define PWMA_DIER_TDE                       PWMA_DIER_TDE_Msk

/** @brief DIER[15]: Interrupt clear */
#define PWMA_DIER_INTR_CLEAR_Pos            (15U)
#define PWMA_DIER_INTR_CLEAR_Msk            (0x1UL << PWMA_DIER_INTR_CLEAR_Pos)
#define PWMA_DIER_INTR_CLEAR                PWMA_DIER_INTR_CLEAR_Msk

/******************** Bit definition for PWMA_SR register *********************/

/** @brief SR[0]: Update interrupt flag */
#define PWMA_SR_UIF_Pos                     (0U)
#define PWMA_SR_UIF_Msk                     (0x1UL << PWMA_SR_UIF_Pos)
#define PWMA_SR_UIF                         PWMA_SR_UIF_Msk

/** @brief SR[1]: Capture/Compare 0 interrupt flag */
#define PWMA_SR_CC0IF_Pos                   (1U)
#define PWMA_SR_CC0IF_Msk                   (0x1UL << PWMA_SR_CC0IF_Pos)
#define PWMA_SR_CC0IF                       PWMA_SR_CC0IF_Msk

/** @brief SR[2]: Capture/Compare 1 interrupt flag */
#define PWMA_SR_CC1IF_Pos                   (2U)
#define PWMA_SR_CC1IF_Msk                   (0x1UL << PWMA_SR_CC1IF_Pos)
#define PWMA_SR_CC1IF                       PWMA_SR_CC1IF_Msk

/** @brief SR[3]: Capture/Compare 2 interrupt flag */
#define PWMA_SR_CC2IF_Pos                   (3U)
#define PWMA_SR_CC2IF_Msk                   (0x1UL << PWMA_SR_CC2IF_Pos)
#define PWMA_SR_CC2IF                       PWMA_SR_CC2IF_Msk

/** @brief SR[4]: Capture/Compare 3 interrupt flag */
#define PWMA_SR_CC3IF_Pos                   (4U)
#define PWMA_SR_CC3IF_Msk                   (0x1UL << PWMA_SR_CC3IF_Pos)
#define PWMA_SR_CC3IF                       PWMA_SR_CC3IF_Msk

/** @brief SR[5]: COM interrupt flag */
#define PWMA_SR_COMIF_Pos                   (5U)
#define PWMA_SR_COMIF_Msk                   (0x1UL << PWMA_SR_COMIF_Pos)
#define PWMA_SR_COMIF                       PWMA_SR_COMIF_Msk

/** @brief SR[6]: Trigger interrupt flag */
#define PWMA_SR_TIF_Pos                     (6U)
#define PWMA_SR_TIF_Msk                     (0x1UL << PWMA_SR_TIF_Pos)
#define PWMA_SR_TIF                         PWMA_SR_TIF_Msk

/** @brief SR[7]: Break interrupt flag */
#define PWMA_SR_BIF_Pos                     (7U)
#define PWMA_SR_BIF_Msk                     (0x1UL << PWMA_SR_BIF_Pos)
#define PWMA_SR_BIF                         PWMA_SR_BIF_Msk

/** @brief SR[9]: Capture/Compare 0 overcapture flag */
#define PWMA_SR_CC0OF_Pos                   (9U)
#define PWMA_SR_CC0OF_Msk                   (0x1UL << PWMA_SR_CC0OF_Pos)
#define PWMA_SR_CC0OF                       PWMA_SR_CC0OF_Msk

/** @brief SR[10]: Capture/Compare 1 overcapture flag */
#define PWMA_SR_CC1OF_Pos                   (10U)
#define PWMA_SR_CC1OF_Msk                   (0x1UL << PWMA_SR_CC1OF_Pos)
#define PWMA_SR_CC1OF                       PWMA_SR_CC1OF_Msk

/** @brief SR[11]: Capture/Compare 2 overcapture flag */
#define PWMA_SR_CC2OF_Pos                   (11U)
#define PWMA_SR_CC2OF_Msk                   (0x1UL << PWMA_SR_CC2OF_Pos)
#define PWMA_SR_CC2OF                       PWMA_SR_CC2OF_Msk

/** @brief SR[12]: Capture/Compare 3 overcapture flag */
#define PWMA_SR_CC3OF_Pos                   (12U)
#define PWMA_SR_CC3OF_Msk                   (0x1UL << PWMA_SR_CC3OF_Pos)
#define PWMA_SR_CC3OF                       PWMA_SR_CC3OF_Msk

/******************** Bit definition for PWMA_EGR register ********************/

/** @brief EGR[0]: Update generation */
#define PWMA_EGR_UG_Pos                     (0U)
#define PWMA_EGR_UG_Msk                     (0x1UL << PWMA_EGR_UG_Pos)
#define PWMA_EGR_UG                         PWMA_EGR_UG_Msk

/** @brief EGR[1]: Capture/Compare 0 generation */
#define PWMA_EGR_CC0G_Pos                   (1U)
#define PWMA_EGR_CC0G_Msk                   (0x1UL << PWMA_EGR_CC0G_Pos)
#define PWMA_EGR_CC0G                       PWMA_EGR_CC0G_Msk

/** @brief EGR[2]: Capture/Compare 1 generation */
#define PWMA_EGR_CC1G_Pos                   (2U)
#define PWMA_EGR_CC1G_Msk                   (0x1UL << PWMA_EGR_CC1G_Pos)
#define PWMA_EGR_CC1G                       PWMA_EGR_CC1G_Msk

/** @brief EGR[3]: Capture/Compare 2 generation */
#define PWMA_EGR_CC2G_Pos                   (3U)
#define PWMA_EGR_CC2G_Msk                   (0x1UL << PWMA_EGR_CC2G_Pos)
#define PWMA_EGR_CC2G                       PWMA_EGR_CC2G_Msk

/** @brief EGR[4]: Capture/Compare 3 generation */
#define PWMA_EGR_CC3G_Pos                   (4U)
#define PWMA_EGR_CC3G_Msk                   (0x1UL << PWMA_EGR_CC3G_Pos)
#define PWMA_EGR_CC3G                       PWMA_EGR_CC3G_Msk

/** @brief EGR[5]: Capture/Compare control update generation */
#define PWMA_EGR_COMG_Pos                   (5U)
#define PWMA_EGR_COMG_Msk                   (0x1UL << PWMA_EGR_COMG_Pos)
#define PWMA_EGR_COMG                       PWMA_EGR_COMG_Msk

/** @brief EGR[6]: Trigger generation */
#define PWMA_EGR_TG_Pos                     (6U)
#define PWMA_EGR_TG_Msk                     (0x1UL << PWMA_EGR_TG_Pos)
#define PWMA_EGR_TG                         PWMA_EGR_TG_Msk

/** @brief EGR[7]: Break generation */
#define PWMA_EGR_BG_Pos                     (7U)
#define PWMA_EGR_BG_Msk                     (0x1UL << PWMA_EGR_BG_Pos)
#define PWMA_EGR_BG                         PWMA_EGR_BG_Msk

/**************** Bit definition for PWMA_CCMR1 register ****************/

/** @brief CCMR1[1:0]: Capture/Compare 0 selection */
#define PWMA_CCMR1_CC0S_Pos                 (0U)
#define PWMA_CCMR1_CC0S_Msk                 (0x3UL << PWMA_CCMR1_CC0S_Pos)
#define PWMA_CCMR1_CC0S                     PWMA_CCMR1_CC0S_Msk

#define PWMA_CCMR1_CC0S_00                  (0x0UL << PWMA_CCMR1_CC0S_Pos)      /*!< Channel 0 configured as output */
#define PWMA_CCMR1_CC0S_01                  (0x1UL << PWMA_CCMR1_CC0S_Pos)      /*!< Channel 0 configured as input; IC0 signal connected to TI0 */
#define PWMA_CCMR1_CC0S_10                  (0x2UL << PWMA_CCMR1_CC0S_Pos)      /*!< Channel 0 configured as input; IC0 signal connected to TI1 */
#define PWMA_CCMR1_CC0S_11                  (0x3UL << PWMA_CCMR1_CC0S_Pos)      /*!< Channel 0 configured as input; IC0 signal connected to TRC when SMCR_TS=1 */

/** @brief CCMR1[3]: Output Compare 0 preload enable */
#define PWMA_CCMR1_OC0PE_Pos                (3U)
#define PWMA_CCMR1_OC0PE_Msk                (0x1UL << PWMA_CCMR1_OC0PE_Pos)
#define PWMA_CCMR1_OC0PE                    PWMA_CCMR1_OC0PE_Msk                /*!< Preload register on PWMAx_CCR0 enabled. Read/Write operations access the preload register. PWMAx_CCR0 preload value is loaded in the active register at each update event.*/

/** @brief CCMR1[3:2]: Input capture 0 prescaler */
#define PWMA_CCMR1_IC0PSC_Pos               (2U)
#define PWMA_CCMR1_IC0PSC_Msk               (0x3UL << PWMA_CCMR1_IC0PSC_Pos)
#define PWMA_CCMR1_IC0PSC                   PWMA_CCMR1_IC0PSC_Msk

#define PWMA_CCMR1_IC0PSC_00                (0x0UL << PWMA_CCMR1_IC0PSC_Pos)    /*!< 1 */
#define PWMA_CCMR1_IC0PSC_01                (0x1UL << PWMA_CCMR1_IC0PSC_Pos)    /*!< 2 */
#define PWMA_CCMR1_IC0PSC_10                (0x2UL << PWMA_CCMR1_IC0PSC_Pos)    /*!< 4 */
#define PWMA_CCMR1_IC0PSC_11                (0x3UL << PWMA_CCMR1_IC0PSC_Pos)    /*!< 8 */

/** @brief CCMR1[6:4]: Output Compare 0 mode */
#define PWMA_CCMR1_OC0M_Pos                 (4U)
#define PWMA_CCMR1_OC0M_Msk                 (0x7UL << PWMA_CCMR1_OC0M_Pos)
#define PWMA_CCMR1_OC0M                     PWMA_CCMR1_OC0M_Msk

#define PWMA_CCMR1_OC0M_000                 (0x0UL << PWMA_CCMR1_OC0M_Pos)      /*!< Frozen; When CNT=CCR0, the output states do not change. */
#define PWMA_CCMR1_OC0M_001                 (0x1UL << PWMA_CCMR1_OC0M_Pos)      /*!< Set the Channel 0 output to the active level when the comparison circuit is triggered; When CNT=CCR0, OC0REF=1 is set. */
#define PWMA_CCMR1_OC0M_010                 (0x2UL << PWMA_CCMR1_OC0M_Pos)      /*!< Set the Channel 0 output to the inactive level when the comparison circuit is triggered; When CNT=CCR0, OC0REF=0 is set. */
#define PWMA_CCMR1_OC0M_011                 (0x3UL << PWMA_CCMR1_OC0M_Pos)      /*!< Toggle; When CNT=CCR0, OC0REF = not OC0REF is set. */
#define PWMA_CCMR1_OC0M_100                 (0x4UL << PWMA_CCMR1_OC0M_Pos)      /*!< Force switch to the inactive state; OC0REF=0. */
#define PWMA_CCMR1_OC0M_101                 (0x5UL << PWMA_CCMR1_OC0M_Pos)      /*!< Force switch to the active state; OC0REF=1. */
#define PWMA_CCMR1_OC0M_110                 (0x6UL << PWMA_CCMR1_OC0M_Pos)      /*!< "PWM mode 1"; When counting up, Channel 0 is active (OC0REF=1)
                                                                                     until (CNT less-than-or-equal CCR0), otherwise Channel 0 is inactive (OC0REF=0);
                                                                                     When counting down, Channel 0 is inactive (OC0REF=0)
                                                                                     until (CNT greater-than-or-equal CCR0), otherwise Channel 0 is active (OC0REF=1). */
#define PWMA_CCMR1_OC0M_111                 (0x7UL << PWMA_CCMR1_OC0M_Pos)      /*!< "PWM Mode 2"; When counting up, Channel 0 is inactive (OC0REF=0)
                                                                                     until (CNT less-than-or-equal CCR0), otherwise Channel 0 is active (OC0REF=1);
                                                                                     When counting down, Channel 0 is active (OC0REF=1)
                                                                                     until (CNT greater-than-or-equal CCR0), otherwise Channel 0 is inactive
                                                                                     (OC0REF=1). */

/** @brief CCMR1[7]: Output Compare 0 clear enable */
#define PWMA_CCMR1_OC0CE_Pos                (7U)
#define PWMA_CCMR1_OC0CE_Msk                (0x1UL << PWMA_CCMR1_OC0CE_Pos)
#define PWMA_CCMR1_OC0CE                    PWMA_CCMR1_OC0CE_Msk

/** @brief CCMR1[7:4]: Input capture 0 filter */
#define PWMA_CCMR1_IC0F_Pos                 (4U)
#define PWMA_CCMR1_IC0F_Msk                 (0xFUL << PWMA_CCMR1_IC0F_Pos)
#define PWMA_CCMR1_IC0F                     PWMA_CCMR1_IC0F_Msk

#define PWMA_CCMR1_IC0F_0000                (0x0UL << PWMA_CCMR1_IC0F_Pos)      /*!< f_sampling=f_dts; Filter not used */
#define PWMA_CCMR1_IC0F_0001                (0x1UL << PWMA_CCMR1_IC0F_Pos)      /*!< f_sampling=f_ck_int, N=2 */
#define PWMA_CCMR1_IC0F_0010                (0x2UL << PWMA_CCMR1_IC0F_Pos)      /*!< f_sampling=f_ck_int, N=4 */
#define PWMA_CCMR1_IC0F_0011                (0x3UL << PWMA_CCMR1_IC0F_Pos)      /*!< f_sampling=f_ck_int, N=8 */
#define PWMA_CCMR1_IC0F_0100                (0x4UL << PWMA_CCMR1_IC0F_Pos)      /*!< f_sampling=f_dts/2, N=6 */
#define PWMA_CCMR1_IC0F_0101                (0x5UL << PWMA_CCMR1_IC0F_Pos)      /*!< f_sampling=f_dts/2, N=8 */
#define PWMA_CCMR1_IC0F_0110                (0x6UL << PWMA_CCMR1_IC0F_Pos)      /*!< f_sampling=f_dts/4, N=6 */
#define PWMA_CCMR1_IC0F_0111                (0x7UL << PWMA_CCMR1_IC0F_Pos)      /*!< f_sampling=f_dts/4, N=8 */
#define PWMA_CCMR1_IC0F_1000                (0x8UL << PWMA_CCMR1_IC0F_Pos)      /*!< f_sampling=f_dts/8, N=6 */
#define PWMA_CCMR1_IC0F_1001                (0x9UL << PWMA_CCMR1_IC0F_Pos)      /*!< f_sampling=f_dts/8, N=8 */
#define PWMA_CCMR1_IC0F_1010                (0xAUL << PWMA_CCMR1_IC0F_Pos)      /*!< f_sampling=f_dts/16, N=5 */
#define PWMA_CCMR1_IC0F_1011                (0xBUL << PWMA_CCMR1_IC0F_Pos)      /*!< f_sampling=f_dts/16, N=6 */
#define PWMA_CCMR1_IC0F_1100                (0xCUL << PWMA_CCMR1_IC0F_Pos)      /*!< f_sampling=f_dts/16, N=8 */
#define PWMA_CCMR1_IC0F_1101                (0xDUL << PWMA_CCMR1_IC0F_Pos)      /*!< f_sampling=f_dts/32, N=5 */
#define PWMA_CCMR1_IC0F_1110                (0xEUL << PWMA_CCMR1_IC0F_Pos)      /*!< f_sampling=f_dts/32, N=6 */
#define PWMA_CCMR1_IC0F_1111                (0xFUL << PWMA_CCMR1_IC0F_Pos)      /*!< f_sampling=f_dts/32, N=8 */

/** @brief CCMR1[9:8]: Capture/Compare 1 selection */
#define PWMA_CCMR1_CC1S_Pos                 (8U)
#define PWMA_CCMR1_CC1S_Msk                 (0x3UL << PWMA_CCMR1_CC1S_Pos)
#define PWMA_CCMR1_CC1S                     PWMA_CCMR1_CC1S_Msk

#define PWMA_CCMR1_CC1S_00                  (0x0UL << PWMA_CCMR1_CC1S_Pos)      /*!< Channel 1 configured as output */
#define PWMA_CCMR1_CC1S_01                  (0x1UL << PWMA_CCMR1_CC1S_Pos)      /*!< Channel 1 configured as input; IC1 signal connected to TI1 */
#define PWMA_CCMR1_CC1S_10                  (0x2UL << PWMA_CCMR1_CC1S_Pos)      /*!< Channel 1 configured as input; IC1 signal connected to TI0 */
#define PWMA_CCMR1_CC1S_11                  (0x3UL << PWMA_CCMR1_CC1S_Pos)      /*!< Channel 1 configured as input; IC1 signal connected to TRC when SMCR_TS=1 */

/** @brief CCMR1[11]: Output Compare 1 preload enable */
#define PWMA_CCMR1_OC1PE_Pos                (11U)
#define PWMA_CCMR1_OC1PE_Msk                (0x1UL << PWMA_CCMR1_OC1PE_Pos)
#define PWMA_CCMR1_OC1PE                    PWMA_CCMR1_OC1PE_Msk

/** @brief CCMR1[11:10]: Input capture 1 prescaler */
#define PWMA_CCMR1_IC1PSC_Pos               (10U)
#define PWMA_CCMR1_IC1PSC_Msk               (0x3UL << PWMA_CCMR1_IC1PSC_Pos)
#define PWMA_CCMR1_IC1PSC                   PWMA_CCMR1_IC1PSC_Msk

#define PWMA_CCMR1_IC1PSC_00                (0x0UL << PWMA_CCMR1_IC1PSC_Pos)    /*!< 1 */
#define PWMA_CCMR1_IC1PSC_01                (0x1UL << PWMA_CCMR1_IC1PSC_Pos)    /*!< 2 */
#define PWMA_CCMR1_IC1PSC_10                (0x2UL << PWMA_CCMR1_IC1PSC_Pos)    /*!< 4 */
#define PWMA_CCMR1_IC1PSC_11                (0x3UL << PWMA_CCMR1_IC1PSC_Pos)    /*!< 8 */

/** @brief CCMR1[14:12]: Output Compare 1 mode */
#define PWMA_CCMR1_OC1M_Pos                 (12U)
#define PWMA_CCMR1_OC1M_Msk                 (0x7UL << PWMA_CCMR1_OC1M_Pos)
#define PWMA_CCMR1_OC1M                     PWMA_CCMR1_OC1M_Msk

#define PWMA_CCMR1_OC1M_000                 (0x0UL << PWMA_CCMR1_OC1M_Pos)      /*!< Frozen; When CNT=CCR1 the output states do not change. */
#define PWMA_CCMR1_OC1M_001                 (0x1UL << PWMA_CCMR1_OC1M_Pos)      /*!< Set the Channel 1 output to the active level when the comparison circuit
                                                                                           is triggered; When CNT=CCR1, OC1REF=1 is set. */
#define PWMA_CCMR1_OC1M_010                 (0x2UL << PWMA_CCMR1_OC1M_Pos)      /*!< Set the Channel 1 output to the inactive level when the comparison circuit
                                                                                           is triggered; When CNT=CCR1, OC1REF=0 is set. */
#define PWMA_CCMR1_OC1M_011                 (0x3UL << PWMA_CCMR1_OC1M_Pos)      /*!< Toggle; When CNT=CCR1, OC1REF = not OC1REF is set. */
#define PWMA_CCMR1_OC1M_100                 (0x4UL << PWMA_CCMR1_OC1M_Pos)      /*!< Force switch to the inactive state; OC1REF=0. */
#define PWMA_CCMR1_OC1M_101                 (0x5UL << PWMA_CCMR1_OC1M_Pos)      /*!< Force switch to the active state; OC1REF=1. */
#define PWMA_CCMR1_OC1M_110                 (0x6UL << PWMA_CCMR1_OC1M_Pos)      /*!< "PWM mode 1"; When counting up, Channel 1 is active (OC1REF=1)  
                                                                                     until (CNT less-than-or-equal CCR1), otherwise Channel 1 is inactive (OC1REF=0);
                                                                                     When counting down, Channel 1 is inactive (OC1REF=0)
                                                                                     until (CNT greater-than-or-equal CCR1), otherwise Channel 1 is active
                                                                                     (OC1REF=1). */
#define PWMA_CCMR1_OC1M_111                 (0x7UL << PWMA_CCMR1_OC1M_Pos)      /*!< "PWM Mode 2"; When counting up, Channel 1 is inactive (OC1REF=0)
                                                                                      until (CNT less-than-or-equal CCR1), otherwise Channel 1 is active (OC1REF=1);
                                                                                      When counting down, Channel 1 is active (OC1REF=1)
                                                                                      until (CNT greater-than-or-equal CCR1), otherwise Channel 1 is inactive
                                                                                      (OC1REF=1). */

/** @brief CCMR1[15]: Output Compare 1 clear enable */
#define PWMA_CCMR1_OC1CE_Pos                (15U)
#define PWMA_CCMR1_OC1CE_Msk                (0x1UL << PWMA_CCMR1_OC1CE_Pos)
#define PWMA_CCMR1_OC1CE                    PWMA_CCMR1_OC1CE_Msk

/** @brief CCMR1[15:12]: Input capture 1 filter */
#define PWMA_CCMR1_IC1F_Pos                 (12U)
#define PWMA_CCMR1_IC1F_Msk                 (0xFUL << PWMA_CCMR1_IC1F_Pos)
#define PWMA_CCMR1_IC1F                     PWMA_CCMR1_IC1F_Msk

#define PWMA_CCMR1_IC1F_0000                (0x0UL << PWMA_CCMR1_IC1F_Pos)      /*!< f_sampling=f_dts; Filter not used */
#define PWMA_CCMR1_IC1F_0001                (0x1UL << PWMA_CCMR1_IC1F_Pos)      /*!< f_sampling=f_ck_int, N=2 */
#define PWMA_CCMR1_IC1F_0010                (0x2UL << PWMA_CCMR1_IC1F_Pos)      /*!< f_sampling=f_ck_int, N=4 */
#define PWMA_CCMR1_IC1F_0011                (0x3UL << PWMA_CCMR1_IC1F_Pos)      /*!< f_sampling=f_ck_int, N=8 */
#define PWMA_CCMR1_IC1F_0100                (0x4UL << PWMA_CCMR1_IC1F_Pos)      /*!< f_sampling=f_dts/2, N=6 */
#define PWMA_CCMR1_IC1F_0101                (0x5UL << PWMA_CCMR1_IC1F_Pos)      /*!< f_sampling=f_dts/2, N=8 */
#define PWMA_CCMR1_IC1F_0110                (0x6UL << PWMA_CCMR1_IC1F_Pos)      /*!< f_sampling=f_dts/4, N=6 */
#define PWMA_CCMR1_IC1F_0111                (0x7UL << PWMA_CCMR1_IC1F_Pos)      /*!< f_sampling=f_dts/4, N=8 */
#define PWMA_CCMR1_IC1F_1000                (0x8UL << PWMA_CCMR1_IC1F_Pos)      /*!< f_sampling=f_dts/8, N=6 */
#define PWMA_CCMR1_IC1F_1001                (0x9UL << PWMA_CCMR1_IC1F_Pos)      /*!< f_sampling=f_dts/8, N=8 */
#define PWMA_CCMR1_IC1F_1010                (0xAUL << PWMA_CCMR1_IC1F_Pos)      /*!< f_sampling=f_dts/16, N=5 */
#define PWMA_CCMR1_IC1F_1011                (0xBUL << PWMA_CCMR1_IC1F_Pos)      /*!< f_sampling=f_dts/16, N=6 */
#define PWMA_CCMR1_IC1F_1100                (0xCUL << PWMA_CCMR1_IC1F_Pos)      /*!< f_sampling=f_dts/16, N=8 */
#define PWMA_CCMR1_IC1F_1101                (0xDUL << PWMA_CCMR1_IC1F_Pos)      /*!< f_sampling=f_dts/32, N=5 */
#define PWMA_CCMR1_IC1F_1110                (0xEUL << PWMA_CCMR1_IC1F_Pos)      /*!< f_sampling=f_dts/32, N=6 */
#define PWMA_CCMR1_IC1F_1111                (0xFUL << PWMA_CCMR1_IC1F_Pos)      /*!< f_sampling=f_dts/32, N=8 */

/**************** Bit definition for PWMA_CCMR2 register ****************/

/** @brief CCMR2[1:0]: Capture/Compare 2 selection */
#define PWMA_CCMR2_CC2S_Pos                 (0U)
#define PWMA_CCMR2_CC2S_Msk                 (0x3UL << PWMA_CCMR2_CC2S_Pos)
#define PWMA_CCMR2_CC2S                     PWMA_CCMR2_CC2S_Msk

#define PWMA_CCMR2_CC2S_00                  (0x0UL << PWMA_CCMR2_CC2S_Pos)      /*!< Channel 2 configured as output */
#define PWMA_CCMR2_CC2S_01                  (0x1UL << PWMA_CCMR2_CC2S_Pos)      /*!< Channel 2 configured as input; IC2 signal connected to TI2 */
#define PWMA_CCMR2_CC2S_10                  (0x2UL << PWMA_CCMR2_CC2S_Pos)      /*!< Channel 2 configured as input; IC2 signal connected to TI3 */
#define PWMA_CCMR2_CC2S_11                  (0x3UL << PWMA_CCMR2_CC2S_Pos)      /*!< Channel 2 configured as input; IC2 signal connected to TRC
                                                                                                 when SMCR_TS=1 */

/** @brief CCMR2[3:2]: Input capture 2 prescaler */
#define PWMA_CCMR2_IC2PSC_Pos               (2U)
#define PWMA_CCMR2_IC2PSC_Msk               (0x3UL << PWMA_CCMR2_IC2PSC_Pos)
#define PWMA_CCMR2_IC2PSC                   PWMA_CCMR2_IC2PSC_Msk

#define PWMA_CCMR2_IC2PSC_00                (0x0UL << PWMA_CCMR2_IC2PSC_Pos)     /*!< 1 */
#define PWMA_CCMR2_IC2PSC_01                (0x1UL << PWMA_CCMR2_IC2PSC_Pos)     /*!< 2 */
#define PWMA_CCMR2_IC2PSC_10                (0x2UL << PWMA_CCMR2_IC2PSC_Pos)     /*!< 4 */
#define PWMA_CCMR2_IC2PSC_11                (0x3UL << PWMA_CCMR2_IC2PSC_Pos)     /*!< 8 */                                                                                                 

/** @brief CCMR2[3]: Output Compare 2 preload enable */
#define PWMA_CCMR2_OC2PE_Pos                (3U)
#define PWMA_CCMR2_OC2PE_Msk                (0x1UL << PWMA_CCMR2_OC2PE_Pos)
#define PWMA_CCMR2_OC2PE                    PWMA_CCMR2_OC2PE_Msk

/** @brief CCMR2[6:4]: Output Compare 2 mode */
#define PWMA_CCMR2_OC2M_Pos                 (4U)
#define PWMA_CCMR2_OC2M_Msk                 (0x7UL << PWMA_CCMR2_OC2M_Pos)
#define PWMA_CCMR2_OC2M                     PWMA_CCMR2_OC2M_Msk

#define PWMA_CCMR2_OC2M_000                 (0x0UL << PWMA_CCMR2_OC2M_Pos)      /*!< Frozen; When CNT=CCR2 the output states do not change. */
#define PWMA_CCMR2_OC2M_001                 (0x1UL << PWMA_CCMR2_OC2M_Pos)      /*!< Set the Channel 2 output to the active level when the comparison circuit
                                                                                                 is triggered; When CNT=CCR2, OC2REF=1 is set. */
#define PWMA_CCMR2_OC2M_010                 (0x2UL << PWMA_CCMR2_OC2M_Pos)      /*!< Set the Channel 2 output to the inactive level when the comparison circuit
                                                                                                 is triggered; When CNT=CCR2, OC2REF=0 is set. */
#define PWMA_CCMR2_OC2M_011                 (0x3UL << PWMA_CCMR2_OC2M_Pos)      /*!< Toggle; When CNT=CCR2, OC2REF = not OC2REF is set. */
#define PWMA_CCMR2_OC2M_100                 (0x4UL << PWMA_CCMR2_OC2M_Pos)      /*!< Force switch to the inactive state; OC2REF=0. */
#define PWMA_CCMR2_OC2M_101                 (0x5UL << PWMA_CCMR2_OC2M_Pos)      /*!< Force switch to the active state; OC2REF=1. */
#define PWMA_CCMR2_OC2M_110                 (0x6UL << PWMA_CCMR2_OC2M_Pos)      /*!< "PWM mode 1"; When counting up, Channel 2 is active (OC2REF=1)
                                                                                                 until (CNT less-than-or-equal CCR2), otherwise Channel 2 is inactive (OC2REF=0);
                                                                                                 When counting down, Channel 2 is inactive (OC2REF=0)
                                                                                                 until (CNT greater-than-or-equal CCR2), otherwise Channel 2 is active
                                                                                                 (OC2REF=1). */
#define PWMA_CCMR2_OC2M_111                 (0x7UL << PWMA_CCMR2_OC2M_Pos)      /*!< "PWM Mode 2"; When counting up, Channel 2 is inactive (OC2REF=0)
                                                                                                 until (CNT less-than-or-equal CCR2), otherwise Channel 2 is active (OC2REF=1);
                                                                                                 When counting down, Channel 2 is active (OC2REF=1)
                                                                                                 until (CNT greater-than-or-equal CCR2), otherwise Channel 2 is inactive
                                                                                                 (OC2REF=1). */

/** @brief CCMR2[7]: Output Compare 2 clear enable */
#define PWMA_CCMR2_OC2CE_Pos                (7U)
#define PWMA_CCMR2_OC2CE_Msk                (0x1UL << PWMA_CCMR2_OC2CE_Pos)
#define PWMA_CCMR2_OC2CE                    PWMA_CCMR2_OC2CE_Msk

/** @brief CCMR2[7:4]: Input capture 2 filter */
#define PWMA_CCMR2_IC2F_Pos                 (4U)
#define PWMA_CCMR2_IC2F_Msk                 (0xFUL << PWMA_CCMR2_IC2F_Pos)
#define PWMA_CCMR2_IC2F                     PWMA_CCMR2_IC2F_Msk

#define PWMA_CCMR2_IC2F_0000                (0x0UL << PWMA_CCMR2_IC2F_Pos)      /*!< f_sampling=f_dts; Filter not used */
#define PWMA_CCMR2_IC2F_0001                (0x1UL << PWMA_CCMR2_IC2F_Pos)      /*!< f_sampling=f_ck_int, N=2 */
#define PWMA_CCMR2_IC2F_0010                (0x2UL << PWMA_CCMR2_IC2F_Pos)      /*!< f_sampling=f_ck_int, N=4 */
#define PWMA_CCMR2_IC2F_0011                (0x3UL << PWMA_CCMR2_IC2F_Pos)      /*!< f_sampling=f_ck_int, N=8 */
#define PWMA_CCMR2_IC2F_0100                (0x4UL << PWMA_CCMR2_IC2F_Pos)      /*!< f_sampling=f_dts/2, N=6 */
#define PWMA_CCMR2_IC2F_0101                (0x5UL << PWMA_CCMR2_IC2F_Pos)      /*!< f_sampling=f_dts/2, N=8 */
#define PWMA_CCMR2_IC2F_0110                (0x6UL << PWMA_CCMR2_IC2F_Pos)      /*!< f_sampling=f_dts/4, N=6 */
#define PWMA_CCMR2_IC2F_0111                (0x7UL << PWMA_CCMR2_IC2F_Pos)      /*!< f_sampling=f_dts/4, N=8 */
#define PWMA_CCMR2_IC2F_1000                (0x8UL << PWMA_CCMR2_IC2F_Pos)      /*!< f_sampling=f_dts/8, N=6 */
#define PWMA_CCMR2_IC2F_1001                (0x9UL << PWMA_CCMR2_IC2F_Pos)      /*!< f_sampling=f_dts/8, N=8 */
#define PWMA_CCMR2_IC2F_1010                (0xAUL << PWMA_CCMR2_IC2F_Pos)      /*!< f_sampling=f_dts/16, N=5 */
#define PWMA_CCMR2_IC2F_1011                (0xBUL << PWMA_CCMR2_IC2F_Pos)      /*!< f_sampling=f_dts/16, N=6 */
#define PWMA_CCMR2_IC2F_1100                (0xCUL << PWMA_CCMR2_IC2F_Pos)      /*!< f_sampling=f_dts/16, N=8 */
#define PWMA_CCMR2_IC2F_1101                (0xDUL << PWMA_CCMR2_IC2F_Pos)      /*!< f_sampling=f_dts/32, N=5 */
#define PWMA_CCMR2_IC2F_1110                (0xEUL << PWMA_CCMR2_IC2F_Pos)      /*!< f_sampling=f_dts/32, N=6 */
#define PWMA_CCMR2_IC2F_1111                (0xFUL << PWMA_CCMR2_IC2F_Pos)      /*!< f_sampling=f_dts/32, N=8 */

/** @brief CCMR2[9:8]: Capture/Compare 3 selection */
#define PWMA_CCMR2_CC3S_Pos                 (8U)
#define PWMA_CCMR2_CC3S_Msk                 (0x3UL << PWMA_CCMR2_CC3S_Pos)
#define PWMA_CCMR2_CC3S                     PWMA_CCMR2_CC3S_Msk

#define PWMA_CCMR2_CC3S_00                  (0x0UL << PWMA_CCMR2_CC3S_Pos)      /*!< Channel 3 configured as output */
#define PWMA_CCMR2_CC3S_01                  (0x1UL << PWMA_CCMR2_CC3S_Pos)      /*!< Channel 3 configured as input; IC3 signal connected to TI3 */
#define PWMA_CCMR2_CC3S_10                  (0x2UL << PWMA_CCMR2_CC3S_Pos)      /*!< Channel 3 configured as input; IC3 signal connected to TI2 */
#define PWMA_CCMR2_CC3S_11                  (0x3UL << PWMA_CCMR2_CC3S_Pos)      /*!< Channel 3 configured as input; IC3 signal connected to TRC
                                                                                                 when SMCR_TS=1 */

/** @brief CCMR2[11]: Output Compare 3 preload enable */
#define PWMA_CCMR2_OC3PE_Pos                (11U)
#define PWMA_CCMR2_OC3PE_Msk                (0x1UL << PWMA_CCMR2_OC3PE_Pos)
#define PWMA_CCMR2_OC3PE                    PWMA_CCMR2_OC3PE_Msk

/** @brief IC3PSC[11:10]: Input capture 3 prescaler */
#define PWMA_CCMR2_IC3PSC_Pos               (10U)
#define PWMA_CCMR2_IC3PSC_Msk               (0x3UL << PWMA_CCMR2_IC3PSC_Pos)
#define PWMA_CCMR2_IC3PSC                   PWMA_CCMR2_IC3PSC_Msk

#define PWMA_CCMR2_IC3PSC_00                (0x0UL << PWMA_CCMR2_IC3PSC_Pos) /*!< 1 */
#define PWMA_CCMR2_IC3PSC_01                (0x1UL << PWMA_CCMR2_IC3PSC_Pos) /*!< 2 */
#define PWMA_CCMR2_IC3PSC_10                (0x2UL << PWMA_CCMR2_IC3PSC_Pos) /*!< 4 */
#define PWMA_CCMR2_IC3PSC_11                (0x3UL << PWMA_CCMR2_IC3PSC_Pos) /*!< 8 */

/** @brief CCMR2[14:12]: Output Compare 3 mode */
#define PWMA_CCMR2_OC3M_Pos                 (12U)
#define PWMA_CCMR2_OC3M_Msk                 (0x7UL << PWMA_CCMR2_OC3M_Pos)
#define PWMA_CCMR2_OC3M                     PWMA_CCMR2_OC3M_Msk

#define PWMA_CCMR2_OC3M_000                 (0x0UL << PWMA_CCMR2_OC3M_Pos)      /*!< Frozen; When CNT=CCR3 the output states do not change. */
#define PWMA_CCMR2_OC3M_001                 (0x1UL << PWMA_CCMR2_OC3M_Pos)      /*!< Set the Channel 3 output to the active level when the comparison circuit
                                                                                                 is triggered; When CNT=CCR3, OC3REF=1 is set. */
#define PWMA_CCMR2_OC3M_010                 (0x2UL << PWMA_CCMR2_OC3M_Pos)      /*!< Set the Channel 3 output to the inactive level when the comparison circuit
                                                                                                 is triggered; When CNT=CCR3, OC3REF=0 is set. */
#define PWMA_CCMR2_OC3M_011                 (0x3UL << PWMA_CCMR2_OC3M_Pos)      /*!< Toggle; When CNT=CCR3, OC3REF = not OC3REF is set. */
#define PWMA_CCMR2_OC3M_100                 (0x4UL << PWMA_CCMR2_OC3M_Pos)      /*!< Force switch to the inactive state; OC3REF=0. */
#define PWMA_CCMR2_OC3M_101                 (0x5UL << PWMA_CCMR2_OC3M_Pos)      /*!< Force switch to the active state; OC3REF=1. */
#define PWMA_CCMR2_OC3M_110                 (0x6UL << PWMA_CCMR2_OC3M_Pos)      /*!< "PWM mode 1"; When counting up, Channel 3 is active (OC3REF=1)
                                                                                                 until (CNT less-than-or-equal CCR3), otherwise Channel 3 is inactive (OC3REF=0);
                                                                                                 When counting down, Channel 3 is inactive (OC3REF=0)
                                                                                                 until (CNT greater-than-or-equal CCR3), otherwise Channel 3 is active
                                                                                                 (OC3REF=1). */
#define PWMA_CCMR2_OC3M_111                 (0x7UL << PWMA_CCMR2_OC3M_Pos)      /*!< "PWM Mode 2"; When counting up, Channel 3 is inactive (OC3REF=0)
                                                                                                 until (CNT less-than-or-equal CCR3), otherwise Channel 3 is active (OC3REF=1);
                                                                                                 When counting down, Channel 3 is active (OC3REF=1)
                                                                                                 until (CNT greater-than-or-equal CCR3), otherwise Channel 3 is inactive
                                                                                                 (OC3REF=1). */

/** @brief CCMR2[15]: Output Compare 3 clear enable */
#define PWMA_CCMR2_OC3CE_Pos                (15U)
#define PWMA_CCMR2_OC3CE_Msk                (0x1UL << PWMA_CCMR2_OC3CE_Pos)
#define PWMA_CCMR2_OC3CE                    PWMA_CCMR2_OC3CE_Msk

/** @brief CCMR2[15:12]: Input capture 3 filter */
#define PWMA_CCMR2_IC3F_Pos                 (12U)
#define PWMA_CCMR2_IC3F_Msk                 (0xFUL << PWMA_CCMR2_IC3F_Pos)
#define PWMA_CCMR2_IC3F                     PWMA_CCMR2_IC3F_Msk

#define PWMA_CCMR2_IC3F_0000                (0x0UL << PWMA_CCMR2_IC3F_Pos)      /*!< f_sampling=f_dts; Filter not used */
#define PWMA_CCMR2_IC3F_0001                (0x1UL << PWMA_CCMR2_IC3F_Pos)      /*!< f_sampling=f_ck_int, N=2 */
#define PWMA_CCMR2_IC3F_0010                (0x2UL << PWMA_CCMR2_IC3F_Pos)      /*!< f_sampling=f_ck_int, N=4 */
#define PWMA_CCMR2_IC3F_0011                (0x3UL << PWMA_CCMR2_IC3F_Pos)      /*!< f_sampling=f_ck_int, N=8 */
#define PWMA_CCMR2_IC3F_0100                (0x4UL << PWMA_CCMR2_IC3F_Pos)      /*!< f_sampling=f_dts/2, N=6 */
#define PWMA_CCMR2_IC3F_0101                (0x5UL << PWMA_CCMR2_IC3F_Pos)      /*!< f_sampling=f_dts/2, N=8 */
#define PWMA_CCMR2_IC3F_0110                (0x6UL << PWMA_CCMR2_IC3F_Pos)      /*!< f_sampling=f_dts/4, N=6 */
#define PWMA_CCMR2_IC3F_0111                (0x7UL << PWMA_CCMR2_IC3F_Pos)      /*!< f_sampling=f_dts/4, N=8 */
#define PWMA_CCMR2_IC3F_1000                (0x8UL << PWMA_CCMR2_IC3F_Pos)      /*!< f_sampling=f_dts/8, N=6 */
#define PWMA_CCMR2_IC3F_1001                (0x9UL << PWMA_CCMR2_IC3F_Pos)      /*!< f_sampling=f_dts/8, N=8 */
#define PWMA_CCMR2_IC3F_1010                (0xAUL << PWMA_CCMR2_IC3F_Pos)      /*!< f_sampling=f_dts/16, N=5 */
#define PWMA_CCMR2_IC3F_1011                (0xBUL << PWMA_CCMR2_IC3F_Pos)      /*!< f_sampling=f_dts/16, N=6 */
#define PWMA_CCMR2_IC3F_1100                (0xCUL << PWMA_CCMR2_IC3F_Pos)      /*!< f_sampling=f_dts/16, N=8 */
#define PWMA_CCMR2_IC3F_1101                (0xDUL << PWMA_CCMR2_IC3F_Pos)      /*!< f_sampling=f_dts/32, N=5 */
#define PWMA_CCMR2_IC3F_1110                (0xEUL << PWMA_CCMR2_IC3F_Pos)      /*!< f_sampling=f_dts/32, N=6 */
#define PWMA_CCMR2_IC3F_1111                (0xFUL << PWMA_CCMR2_IC3F_Pos)      /*!< f_sampling=f_dts/32, N=8 */

/******************* Bit definition for PWMA_CCER register ********************/

/** @brief CCER[0]: Capture/Compare 0 output enable */
#define PWMA_CCER_CC0E_Pos                  (0U)
#define PWMA_CCER_CC0E_Msk                  (0x1UL << PWMA_CCER_CC0E_Pos)
#define PWMA_CCER_CC0E                      PWMA_CCER_CC0E_Msk

#define PWMA_CCER_CC0E_DISABLE              (0x0UL << PWMA_CCER_CC0E_Pos)     /*!< OC0 is not active */
#define PWMA_CCER_CC0E_ENABLE               (0x1UL << PWMA_CCER_CC0E_Pos)     /*!< OC0 signal is output on the corresponding output pin */

/** @brief CCER[1]: Capture/Compare 0 output polarity */
#define PWMA_CCER_CC0P_Pos                  (1U)
#define PWMA_CCER_CC0P_Msk                  (0x1UL << PWMA_CCER_CC0P_Pos)
#define PWMA_CCER_CC0P                      PWMA_CCER_CC0P_Msk

#define PWMA_CCER_CC0P_HIGH                 (0x0UL << PWMA_CCER_CC0P_Pos)     /*!< Channel 0 configured as output:  active high */
#define PWMA_CCER_CC0P_LOW                  (0x1UL << PWMA_CCER_CC0P_Pos)     /*!< Channel 0 configured as output:  active low */
#define PWMA_CCER_CC0P_NONINV               (0x0UL << PWMA_CCER_CC0P_Pos)     /*!< Channel 0 configured as input:  non-inverted/rising edge */
#define PWMA_CCER_CC0P_INV                  (0x1UL << PWMA_CCER_CC0P_Pos)     /*!< Channel 0 configured as input:  inverted/falling edge */

/** @brief CCER[2]: Capture/Compare 0 complementary output enable */
#define PWMA_CCER_CC0NE_Pos                 (2U)
#define PWMA_CCER_CC0NE_Msk                 (0x1UL << PWMA_CCER_CC0NE_Pos)
#define PWMA_CCER_CC0NE                     PWMA_CCER_CC0NE_Msk

#define PWMA_CCER_CC0NE_DISABLE             (0x0UL << PWMA_CCER_CC0NE_Pos)     /*!< OC0N is not active */
#define PWMA_CCER_CC0NE_ENABLE              (0x1UL << PWMA_CCER_CC0NE_Pos)     /*!< OC0N signal is output on the corresponding output pin */

/** @brief CCER[3]: Capture/Compare 0 complementary output polarity */
#define PWMA_CCER_CC0NP_Pos                 (3U)
#define PWMA_CCER_CC0NP_Msk                 (0x1UL << PWMA_CCER_CC0NP_Pos)
#define PWMA_CCER_CC0NP                     PWMA_CCER_CC0NP_Msk

#define PWMA_CCER_CC0NP_HIGH                (0x0UL << PWMA_CCER_CC0NP_Pos)     /*!< Channel 0 configured as output:  active high */
#define PWMA_CCER_CC0NP_LOW                 (0x1UL << PWMA_CCER_CC0NP_Pos)     /*!< Channel 0 configured as output:  active low */
#define PWMA_CCER_CC0NP_NONINV              (0x0UL << PWMA_CCER_CC0NP_Pos)     /*!< This bit is used in conjunction with CC0P to define the polarity of TI0FP1 and TI1FP1. Refer to CC0P description. Channel 0 configured as input:  non-inverted/rising edge */
#define PWMA_CCER_CC0NP_INV                 (0x1UL << PWMA_CCER_CC0NP_Pos)     /*!< Channel 0 configured as input:  inverted/falling edge */

/** @brief CCER[4]: Capture/Compare 1 output enable */
#define PWMA_CCER_CC1E_Pos                  (4U)
#define PWMA_CCER_CC1E_Msk                  (0x1UL << PWMA_CCER_CC1E_Pos)
#define PWMA_CCER_CC1E                      PWMA_CCER_CC1E_Msk

#define PWMA_CCER_CC1E_DISABLE              (0x0UL << PWMA_CCER_CC1E_Pos)     /*!< OC1 is not active */
#define PWMA_CCER_CC1E_ENABLE               (0x1UL << PWMA_CCER_CC1E_Pos)     /*!< OC1 signal is output on the corresponding output pin */

/** @brief CCER[5]: Capture/Compare 1 output polarity */
#define PWMA_CCER_CC1P_Pos                  (5U)
#define PWMA_CCER_CC1P_Msk                  (0x1UL << PWMA_CCER_CC1P_Pos)
#define PWMA_CCER_CC1P                      PWMA_CCER_CC1P_Msk

#define PWMA_CCER_CC1P_HIGH                 (0x0UL << PWMA_CCER_CC1P_Pos)     /*!< Channel 1 configured as output:  active high */
#define PWMA_CCER_CC1P_LOW                  (0x1UL << PWMA_CCER_CC1P_Pos)     /*!< Channel 1 configured as output:  active low */
#define PWMA_CCER_CC1P_NONINV               (0x0UL << PWMA_CCER_CC1P_Pos)     /*!< Channel 1 configured as input:  non-inverted/rising edge */
#define PWMA_CCER_CC1P_INV                  (0x1UL << PWMA_CCER_CC1P_Pos)     /*!< Channel 1 configured as input:  inverted/falling edge */

/** @brief CCER[6]: Capture/Compare 1 complementary output enable */
#define PWMA_CCER_CC1NE_Pos                 (6U)
#define PWMA_CCER_CC1NE_Msk                 (0x1UL << PWMA_CCER_CC1NE_Pos)
#define PWMA_CCER_CC1NE                     PWMA_CCER_CC1NE_Msk

#define PWMA_CCER_CC1NE_DISABLE             (0x0UL << PWMA_CCER_CC1NE_Pos)     /*!< OC1N is not active */
#define PWMA_CCER_CC1NE_ENABLE              (0x1UL << PWMA_CCER_CC1NE_Pos)     /*!< OC1N signal is output on the corresponding output pin */

/** @brief CCER[7]: Capture/Compare 1 complementary output polarity */
#define PWMA_CCER_CC1NP_Pos                 (7U)
#define PWMA_CCER_CC1NP_Msk                 (0x1UL << PWMA_CCER_CC1NP_Pos)
#define PWMA_CCER_CC1NP                     PWMA_CCER_CC1NP_Msk

#define PWMA_CCER_CC1NP_HIGH                (0x0UL << PWMA_CCER_CC1NP_Pos)     /*!< Channel 1 configured as output:  active high */
#define PWMA_CCER_CC1NP_LOW                 (0x1UL << PWMA_CCER_CC1NP_Pos)     /*!< Channel 1 configured as output:  active low */
#define PWMA_CCER_CC1NP_NONINV              (0x0UL << PWMA_CCER_CC1NP_Pos)     /*!< This bit is used in conjunction with CC1P to define the polarity of TI0FP1 and TI1FP1. Refer to CC1P description. Channel 1 configured as input:  non-inverted/rising edge */
#define PWMA_CCER_CC1NP_INV                 (0x1UL << PWMA_CCER_CC1NP_Pos)     /*!< Channel 1 configured as input:  inverted/falling edge */

/** @brief CCER[8]: Capture/Compare 2 output enable */
#define PWMA_CCER_CC2E_Pos                  (8U)
#define PWMA_CCER_CC2E_Msk                  (0x1UL << PWMA_CCER_CC2E_Pos)
#define PWMA_CCER_CC2E                      PWMA_CCER_CC2E_Msk

#define PWMA_CCER_CC2E_DISABLE              (0x0UL << PWMA_CCER_CC2E_Pos)     /*!< OC2 is not active */
#define PWMA_CCER_CC2E_ENABLE               (0x1UL << PWMA_CCER_CC2E_Pos)     /*!< OC2 signal is output on the corresponding output pin */

/** @brief CCER[9]: Capture/Compare 2 output polarity */
#define PWMA_CCER_CC2P_Pos                  (9U)
#define PWMA_CCER_CC2P_Msk                  (0x1UL << PWMA_CCER_CC2P_Pos)
#define PWMA_CCER_CC2P                      PWMA_CCER_CC2P_Msk

#define PWMA_CCER_CC2P_HIGH                 (0x0UL << PWMA_CCER_CC2P_Pos)     /*!< Channel 2 configured as output:  active high */
#define PWMA_CCER_CC2P_LOW                  (0x1UL << PWMA_CCER_CC2P_Pos)     /*!< Channel 2 configured as output:  active low */
#define PWMA_CCER_CC2P_NONINV               (0x0UL << PWMA_CCER_CC2P_Pos)     /*!< Channel 2 configured as input:  non-inverted/rising edge */
#define PWMA_CCER_CC2P_INV                  (0x1UL << PWMA_CCER_CC2P_Pos)     /*!< Channel 2 configured as input:  inverted/falling edge */

/** @brief CCER[10]: Capture/Compare 2 complementary output enable */
#define PWMA_CCER_CC2NE_Pos                 (10U)
#define PWMA_CCER_CC2NE_Msk                 (0x1UL << PWMA_CCER_CC2NE_Pos)
#define PWMA_CCER_CC2NE                     PWMA_CCER_CC2NE_Msk

#define PWMA_CCER_CC2NE_DISABLE             (0x0UL << PWMA_CCER_CC2NE_Pos)     /*!< OC2N is not active */
#define PWMA_CCER_CC2NE_ENABLE              (0x1UL << PWMA_CCER_CC2NE_Pos)     /*!< OC2N signal is output on the corresponding output pin */

/** @brief CCER[11]: Capture/Compare 2 complementary output polarity */
#define PWMA_CCER_CC2NP_Pos                 (11U)
#define PWMA_CCER_CC2NP_Msk                 (0x1UL << PWMA_CCER_CC2NP_Pos)
#define PWMA_CCER_CC2NP                     PWMA_CCER_CC2NP_Msk

#define PWMA_CCER_CC2NP_HIGH                (0x0UL << PWMA_CCER_CC2NP_Pos)     /*!< Channel 2 configured as output:  active high */
#define PWMA_CCER_CC2NP_LOW                 (0x1UL << PWMA_CCER_CC2NP_Pos)     /*!< Channel 2 configured as output:  active low */
#define PWMA_CCER_CC2NP_NONINV              (0x0UL << PWMA_CCER_CC2NP_Pos)     /*!< This bit is used in conjunction with CC2P to define the polarity of TI0FP1 and TI1FP1. Refer to CC2P description. Channel 2 configured as input:  non-inverted/rising edge */
#define PWMA_CCER_CC2NP_INV                 (0x1UL << PWMA_CCER_CC2NP_Pos)     /*!< Channel 2 configured as input:  inverted/falling edge */

/** @brief CCER[12]: Capture/Compare 3 output enable */
#define PWMA_CCER_CC3E_Pos                  (12U)
#define PWMA_CCER_CC3E_Msk                  (0x1UL << PWMA_CCER_CC3E_Pos)
#define PWMA_CCER_CC3E                      PWMA_CCER_CC3E_Msk

#define PWMA_CCER_CC3E_DISABLE              (0x0UL << PWMA_CCER_CC3E_Pos)     /*!< OC3 is not active */
#define PWMA_CCER_CC3E_ENABLE               (0x1UL << PWMA_CCER_CC3E_Pos)     /*!< OC3 signal is output on the corresponding output pin */

/** @brief CCER[13]: Capture/Compare 3 output polarity */
#define PWMA_CCER_CC3P_Pos                  (13U)
#define PWMA_CCER_CC3P_Msk                  (0x1UL << PWMA_CCER_CC3P_Pos)
#define PWMA_CCER_CC3P                      PWMA_CCER_CC3P_Msk

#define PWMA_CCER_CC3P_HIGH                 (0x0UL << PWMA_CCER_CC3P_Pos)     /*!< Channel 3 configured as output:  active high */
#define PWMA_CCER_CC3P_LOW                  (0x1UL << PWMA_CCER_CC3P_Pos)     /*!< Channel 3 configured as output:  active low */
#define PWMA_CCER_CC3P_NONINV               (0x0UL << PWMA_CCER_CC3P_Pos)     /*!< Channel 3 configured as input:  non-inverted/rising edge */
#define PWMA_CCER_CC3P_INV                  (0x1UL << PWMA_CCER_CC3P_Pos)     /*!< Channel 3 configured as input:  inverted/falling edge */

/******************** Bit definition for PWMA_CNT register ********************/

/** @brief CNT[15:0]: Counter */
#define PWMA_CNT_CNT_Pos                    (0U)
#define PWMA_CNT_CNT_Msk                    (0xFFFFUL << PWMA_CNT_CNT_Pos)
#define PWMA_CNT_CNT                        PWMA_CNT_CNT_Msk

/******************** Bit definition for PWMA_PSC register ********************/

/** @brief PSC[15:0]: Prescaler */
#define PWMA_PSC_PSC_Pos                    (0U)
#define PWMA_PSC_PSC_Msk                    (0xFFFFUL << PWMA_PSC_PSC_Pos)
#define PWMA_PSC_PSC                        PWMA_PSC_PSC_Msk

/******************** Bit definition for PWMA_ARR register ********************/

/** @brief ARR[15:0]: Auto-reload register */
#define PWMA_ARR_ARR_Pos                    (0U)
#define PWMA_ARR_ARR_Msk                    (0xFFFFUL << PWMA_ARR_ARR_Pos)
#define PWMA_ARR_ARR                        PWMA_ARR_ARR_Msk

/******************** Bit definition for PWMA_RCR register ********************/

/** @brief RCR[7:0]: Repetition counter register */
#define PWMA_RCR_RCR_Pos                    (0U)
#define PWMA_RCR_RCR_Msk                    (0xFFUL << PWMA_RCR_RCR_Pos)
#define PWMA_RCR_RCR                        PWMA_RCR_RCR_Msk

/******************* Bit definition for PWMA_CCR0 register ********************/

/** @brief CCR0[15:0]: Capture/Compare register 0 */
#define PWMA_CCR0_CCR0_Pos                  (0U)
#define PWMA_CCR0_CCR0_Msk                  (0xFFFFUL << PWMA_CCR0_CCR0_Pos)
#define PWMA_CCR0_CCR0                      PWMA_CCR0_CCR0_Msk

/******************* Bit definition for PWMA_CCR1 register ********************/

/** @brief CCR1[15:0]: Capture/Compare register 1 */
#define PWMA_CCR1_CCR1_Pos                  (0U)
#define PWMA_CCR1_CCR1_Msk                  (0xFFFFUL << PWMA_CCR1_CCR1_Pos)
#define PWMA_CCR1_CCR1                      PWMA_CCR1_CCR1_Msk

/******************* Bit definition for PWMA_CCR2 register ********************/

/** @brief CCR2[15:0]: Capture/Compare register 2 */
#define PWMA_CCR2_CCR2_Pos                  (0U)
#define PWMA_CCR2_CCR2_Msk                  (0xFFFFUL << PWMA_CCR2_CCR2_Pos)
#define PWMA_CCR2_CCR2                      PWMA_CCR2_CCR2_Msk

/******************* Bit definition for PWMA_CCR3 register ********************/

/** @brief CCR3[15:0]: Capture/Compare register 3 */
#define PWMA_CCR3_CCR3_Pos                  (0U)
#define PWMA_CCR3_CCR3_Msk                  (0xFFFFUL << PWMA_CCR3_CCR3_Pos)
#define PWMA_CCR3_CCR3                      PWMA_CCR3_CCR3_Msk

/******************* Bit definition for PWMA_BDTR register ********************/

/** @brief BDTR[7:0]: Dead-time generator setup */
#define PWMA_BDTR_DTG_Pos                   (0U)
#define PWMA_BDTR_DTG_Msk                   (0xFFUL << PWMA_BDTR_DTG_Pos)
#define PWMA_BDTR_DTG                       PWMA_BDTR_DTG_Msk

/** @brief OSSR[11]: Off-state selection for Run mode */
#define PWMA_BDTR_OSSR_Pos                  (11U)
#define PWMA_BDTR_OSSR_Msk                  (0x1UL << PWMA_BDTR_OSSR_Pos)
#define PWMA_BDTR_OSSR                      PWMA_BDTR_OSSR_Msk

#define PWMA_BDTR_OSSR_DISABLE              (0x0UL << PWMA_BDTR_OSSR_Pos)       /*!< When inactive, OCx/OCxN outputs are disabled */
#define PWMA_BDTR_OSSR_ENABLE               (0x1UL << PWMA_BDTR_OSSR_Pos)       /*!< When inactive, OC/OCN outputs are enabled with their inactive level as soon as CCxE=1 or CCxNE=1 */

/** @brief BDTR[12]: Break enable */
#define PWMA_BDTR_BKE_Pos                   (12U)
#define PWMA_BDTR_BKE_Msk                   (0x1UL << PWMA_BDTR_BKE_Pos)
#define PWMA_BDTR_BKE                       PWMA_BDTR_BKE_Msk

#define PWMA_BDTR_BKE_DISABLE              (0x0UL << PWMA_BDTR_BKE_Pos)       /*!< Break function disabled */
#define PWMA_BDTR_BKE_ENABLE               (0x1UL << PWMA_BDTR_BKE_Pos)       /*!< Break function enabled */

/** @brief BDTR[13]: Break polarity */
#define PWMA_BDTR_BKP_Pos                   (13U)
#define PWMA_BDTR_BKP_Msk                   (0x1UL << PWMA_BDTR_BKP_Pos)
#define PWMA_BDTR_BKP                       PWMA_BDTR_BKP_Msk

#define PWMA_BDTR_BKP_LOW                   (0x0UL << PWMA_BDTR_BKP_Pos)        /*!< Break input BRK is active low */
#define PWMA_BDTR_BKP_HIGH                  (0x1UL << PWMA_BDTR_BKP_Pos)        /*!< Break input BRK is active high */

/** @brief BDTR[14]: Automatic output enable */
#define PWMA_BDTR_AOE_Pos                   (14U)
#define PWMA_BDTR_AOE_Msk                   (0x1UL << PWMA_BDTR_AOE_Pos)
#define PWMA_BDTR_AOE                       PWMA_BDTR_AOE_Msk

#define PWMA_BDTR_AOE_DISABLE              (0x0UL << PWMA_BDTR_AOE_Pos)       /*!< MOE can be set only by software */
#define PWMA_BDTR_AOE_ENABLE               (0x1UL << PWMA_BDTR_AOE_Pos)       /*!< MOE can be set by software or automatically at the next update event  */

/** @brief BDTR[15]: Main output enable */
#define PWMA_BDTR_MOE_Pos                   (15U)
#define PWMA_BDTR_MOE_Msk                   (0x1UL << PWMA_BDTR_MOE_Pos)
#define PWMA_BDTR_MOE                       PWMA_BDTR_MOE_Msk

/******************** Bit definition for PWMA_DCR register ********************/

/** @brief DCR[4:0]: DMA base address */
#define PWMA_DCR_DBA_Pos                    (0U)
#define PWMA_DCR_DBA_Msk                    (0x1FUL << PWMA_DCR_DBA_Pos)
#define PWMA_DCR_DBA                        PWMA_DCR_DBA_Msk

#define PWMA_DCR_DBA_00000                  (0x0UL << PWMA_DCR_DBA_Pos)         /*!< (0x00) CR1 */
#define PWMA_DCR_DBA_00001                  (0x1UL << PWMA_DCR_DBA_Pos)         /*!< (0x04) CR2 */
#define PWMA_DCR_DBA_00010                  (0x2UL << PWMA_DCR_DBA_Pos)         /*!< (0x08) SMCR */
#define PWMA_DCR_DBA_00011                  (0x3UL << PWMA_DCR_DBA_Pos)         /*!< (0x0C) DIER */
#define PWMA_DCR_DBA_00100                  (0x4UL << PWMA_DCR_DBA_Pos)         /*!< (0x10) SR */
#define PWMA_DCR_DBA_00101                  (0x5UL << PWMA_DCR_DBA_Pos)         /*!< (0x14) EGR */
#define PWMA_DCR_DBA_00110                  (0x6UL << PWMA_DCR_DBA_Pos)         /*!< (0x18) CCMR1 */
#define PWMA_DCR_DBA_00111                  (0x7UL << PWMA_DCR_DBA_Pos)         /*!< (0x1C) CCMR2 */
#define PWMA_DCR_DBA_01000                  (0x8UL << PWMA_DCR_DBA_Pos)         /*!< (0x20) CCER */
#define PWMA_DCR_DBA_01001                  (0x9UL << PWMA_DCR_DBA_Pos)         /*!< (0x24) CNT */
#define PWMA_DCR_DBA_01010                  (0xAUL << PWMA_DCR_DBA_Pos)         /*!< (0x28) PSC */
#define PWMA_DCR_DBA_01011                  (0xBUL << PWMA_DCR_DBA_Pos)         /*!< (0x2C) ARR */
#define PWMA_DCR_DBA_01100                  (0xCUL << PWMA_DCR_DBA_Pos)         /*!< (0x30) RCR */
#define PWMA_DCR_DBA_01101                  (0xDUL << PWMA_DCR_DBA_Pos)         /*!< (0x34) CCR0 */
#define PWMA_DCR_DBA_01110                  (0xEUL << PWMA_DCR_DBA_Pos)         /*!< (0x38) CCR1 */
#define PWMA_DCR_DBA_01111                  (0xFUL << PWMA_DCR_DBA_Pos)         /*!< (0x3C) CCR2 */
#define PWMA_DCR_DBA_10000                  (0x10UL << PWMA_DCR_DBA_Pos)        /*!< (0x40) CCR3 */
#define PWMA_DCR_DBA_10001                  (0x11UL << PWMA_DCR_DBA_Pos)        /*!< (0x44) BDTR */
#define PWMA_DCR_DBA_10010                  (0x12UL << PWMA_DCR_DBA_Pos)        /*!< (0x48) DCR */

/** @brief DCR[12:8]: DMA burst length */
#define PWMA_DCR_DBL_Pos                    (8U)
#define PWMA_DCR_DBL_Msk                    (0x1FUL << PWMA_DCR_DBL_Pos)
#define PWMA_DCR_DBL                        PWMA_DCR_DBL_Msk

#define PWMA_DCR_DBL_00000                  (0x0UL << PWMA_DCR_DBL_Pos)         /*!< Transfer is done to 1 register starting from the DMA burst base address */
#define PWMA_DCR_DBL_00001                  (0x1UL << PWMA_DCR_DBL_Pos)         /*!< Transfer is done to 1 registers starting from the DMA burst base address */
#define PWMA_DCR_DBL_00010                  (0x2UL << PWMA_DCR_DBL_Pos)         /*!< Transfer is done to 2 registers starting from the DMA burst base address */
#define PWMA_DCR_DBL_00011                  (0x3UL << PWMA_DCR_DBL_Pos)         /*!< Transfer is done to 3 registers starting from the DMA burst base address */
#define PWMA_DCR_DBL_00100                  (0x4UL << PWMA_DCR_DBL_Pos)         /*!< Transfer is done to 4 registers starting from the DMA burst base address*/
#define PWMA_DCR_DBL_00101                  (0x5UL << PWMA_DCR_DBL_Pos)         /*!< Transfer is done to 5 registers starting from the DMA burst base address */
#define PWMA_DCR_DBL_00110                  (0x6UL << PWMA_DCR_DBL_Pos)         /*!< Transfer is done to 6 registers starting from the DMA burst base address */
#define PWMA_DCR_DBL_00111                  (0x7UL << PWMA_DCR_DBL_Pos)         /*!< Transfer is done to 7 registers starting from the DMA burst base address */
#define PWMA_DCR_DBL_01000                  (0x8UL << PWMA_DCR_DBL_Pos)         /*!< Transfer is done to 8 registers starting from the DMA burst base address */
#define PWMA_DCR_DBL_01001                  (0x9UL << PWMA_DCR_DBL_Pos)         /*!< Transfer is done to 9 registers starting from the DMA burst base address */
#define PWMA_DCR_DBL_01010                  (0xAUL << PWMA_DCR_DBL_Pos)         /*!< Transfer is done to 10 registers starting from the DMA burst base address */
#define PWMA_DCR_DBL_01011                  (0xBUL << PWMA_DCR_DBL_Pos)         /*!< Transfer is done to 11 registers starting from the DMA burst base address */
#define PWMA_DCR_DBL_01100                  (0xCUL << PWMA_DCR_DBL_Pos)         /*!< Transfer is done to 12 registers starting from the DMA burst base address */
#define PWMA_DCR_DBL_01101                  (0xDUL << PWMA_DCR_DBL_Pos)         /*!< Transfer is done to 13 registers starting from the DMA burst base address */
#define PWMA_DCR_DBL_01110                  (0xEUL << PWMA_DCR_DBL_Pos)         /*!< Transfer is done to 14 registers starting from the DMA burst base address */
#define PWMA_DCR_DBL_01111                  (0xFUL << PWMA_DCR_DBL_Pos)         /*!< Transfer is done to 15 registers starting from the DMA burst base address */
#define PWMA_DCR_DBL_10000                  (0x10UL << PWMA_DCR_DBL_Pos)        /*!< Transfer is done to 16 registers starting from the DMA burst base address */
#define PWMA_DCR_DBL_10001                  (0x11UL << PWMA_DCR_DBL_Pos)        /*!< Transfer is done to 17 registers starting from the DMA burst base address */
#define PWMA_DCR_DBL_10010                  (0x12UL << PWMA_DCR_DBL_Pos)        /*!< Transfer is done to 18 registers starting from the DMA burst base address */

/******************* Bit definition for PWMA_DMAR register ********************/

/** @brief DMAR[15:0]: DMA burst access base address */
#define PWMA_DMAR_DBAB_Pos                  (0U)
#define PWMA_DMAR_DBAB_Msk                  (0xFFFFUL << PWMA_DMAR_DBAB_Pos)
#define PWMA_DMAR_DBAB                      PWMA_DMAR_DBAB_Msk

/******************************************************************************/
/*                                     QE                                     */
/******************************************************************************/

/******************** Bit definition for QE_QESET register ********************/

/** @brief QESET[0]: Enable */
#define QE_QESET_QEN_Pos                    (0U)
#define QE_QESET_QEN_Msk                    (0x1UL << QE_QESET_QEN_Pos)
#define QE_QESET_QEN                        QE_QESET_QEN_Msk

#define QE_QESET_QEN_FALSE                  (0x0UL << QE_QESET_QEN_Pos)         /*!< False */
#define QE_QESET_QEN_TRUE                   (0x1UL << QE_QESET_QEN_Pos)         /*!< True */

/** @brief QESET[1]: Counting mode */
#define QE_QESET_QEMODE_Pos                 (1U)
#define QE_QESET_QEMODE_Msk                 (0x1UL << QE_QESET_QEMODE_Pos)
#define QE_QESET_QEMODE                     QE_QESET_QEMODE_Msk

#define QE_QESET_QEMODE_0                   (0x0UL << QE_QESET_QEMODE_Pos)      /*!< Quadrature mode */
#define QE_QESET_QEMODE_1                   (0x1UL << QE_QESET_QEMODE_Pos)      /*!< Counter is triggered by posedge(QA);
                                                                                    You can select the direction of QB */

/** @brief QESET[2]: QA changed with QB placed */
#define QE_QESET_QESWAP_Pos                 (2U)
#define QE_QESET_QESWAP_Msk                 (0x1UL << QE_QESET_QESWAP_Pos)
#define QE_QESET_QESWAP                     QE_QESET_QESWAP_Msk

#define QE_QESET_QESWAP_FALSE               (0x0UL << QE_QESET_QESWAP_Pos)      /*!< False */
#define QE_QESET_QESWAP_TRUE                (0x1UL << QE_QESET_QESWAP_Pos)      /*!< True */

/** @brief QESET[3]: Enable INDXF signal sampling */
#define QE_QESET_INDF_Pos                   (3U)
#define QE_QESET_INDF_Msk                   (0x1UL << QE_QESET_INDF_Pos)
#define QE_QESET_INDF                       QE_QESET_INDF_Msk

#define QE_QESET_INDF_FALSE                 (0x0UL << QE_QESET_INDF_Pos)        /*!< False */
#define QE_QESET_INDF_TRUE                  (0x1UL << QE_QESET_INDF_Pos)        /*!< True */

/** @brief QESET[4]: Enable QA signal sampling */
#define QE_QESET_QAF_Pos                    (4U)
#define QE_QESET_QAF_Msk                    (0x1UL << QE_QESET_QAF_Pos)
#define QE_QESET_QAF                        QE_QESET_QAF_Msk

#define QE_QESET_QAF_FALSE                  (0x0UL << QE_QESET_QAF_Pos)         /*!< False */
#define QE_QESET_QAF_TRUE                   (0x1UL << QE_QESET_QAF_Pos)         /*!< True */

/** @brief QESET[5]: Enable QB signal sampling */
#define QE_QESET_QBF_Pos                    (5U)
#define QE_QESET_QBF_Msk                    (0x1UL << QE_QESET_QBF_Pos)
#define QE_QESET_QBF                        QE_QESET_QBF_Msk

#define QE_QESET_QBF_FALSE                  (0x0UL << QE_QESET_QBF_Pos)         /*!< False */
#define QE_QESET_QBF_TRUE                   (0x1UL << QE_QESET_QBF_Pos)         /*!< True */

/** @brief QESET[6]: Enable QA signal inversion */
#define QE_QESET_QAPOL_Pos                  (6U)
#define QE_QESET_QAPOL_Msk                  (0x1UL << QE_QESET_QAPOL_Pos)
#define QE_QESET_QAPOL                      QE_QESET_QAPOL_Msk

#define QE_QESET_QAPOL_FALSE                (0x0UL << QE_QESET_QAPOL_Pos)       /*!< False */
#define QE_QESET_QAPOL_TRUE                 (0x1UL << QE_QESET_QAPOL_Pos)       /*!< True */

/** @brief QESET[7]: Enable QB signal inversion */
#define QE_QESET_QBPOL_Pos                  (7U)
#define QE_QESET_QBPOL_Msk                  (0x1UL << QE_QESET_QBPOL_Pos)
#define QE_QESET_QBPOL                      QE_QESET_QBPOL_Msk

#define QE_QESET_QBPOL_FALSE                (0x0UL << QE_QESET_QBPOL_Pos)       /*!< False */
#define QE_QESET_QBPOL_TRUE                 (0x1UL << QE_QESET_QBPOL_Pos)       /*!< True */

/** @brief QESET[8]: Enable INDX signal inversion */
#define QE_QESET_INDXPOL_Pos                (8U)
#define QE_QESET_INDXPOL_Msk                (0x1UL << QE_QESET_INDXPOL_Pos)
#define QE_QESET_INDXPOL                    QE_QESET_INDXPOL_Msk

#define QE_QESET_INDXPOL_FALSE              (0x0UL << QE_QESET_INDXPOL_Pos)     /*!< False */
#define QE_QESET_INDXPOL_TRUE               (0x1UL << QE_QESET_INDXPOL_Pos)     /*!< True */

/** @brief QESET[9]: Single mode */
#define QE_QESET_SINGLE_MODE_Pos            (9U)
#define QE_QESET_SINGLE_MODE_Msk            (0x1UL << QE_QESET_SINGLE_MODE_Pos)
#define QE_QESET_SINGLE_MODE                QE_QESET_SINGLE_MODE_Msk

#define QE_QESET_SINGLE_MODE_0              (0x0UL << QE_QESET_SINGLE_MODE_Pos) /*!< Multiple mode; When the maximum value QEMAX is reached,
                                                                                    the counter is reset (CNT=0) */
#define QE_QESET_SINGLE_MODE_1              (0x1UL << QE_QESET_SINGLE_MODE_Pos) /*!< Single mode; When the maximum value QEMAX is reached,
                                                                                    the counter is not incremented; When the minimum value 0 is reached,
                                                                                    the counter is not decremented. */

/** @brief QESET[10]: Select the counter initialization mode */
#define QE_QESET_INIT_MODE_Pos              (10U)
#define QE_QESET_INIT_MODE_Msk              (0x1UL << QE_QESET_INIT_MODE_Pos)
#define QE_QESET_INIT_MODE                  QE_QESET_INIT_MODE_Msk

#define QE_QESET_INIT_MODE_0                (0x0UL << QE_QESET_INIT_MODE_Pos)   /*!< On posedge(INDX) the counter is reset (CNT=0) */
#define QE_QESET_INIT_MODE_1                (0x1UL << QE_QESET_INIT_MODE_Pos)   /*!< On posedge(INDX) the counter is initialized from INITSET (CNT=INITSET) */

/** @brief QESET[11]: Overflow interrupt enable */
#define QE_QESET_OFIE_Pos                   (11U)
#define QE_QESET_OFIE_Msk                   (0x1UL << QE_QESET_OFIE_Pos)
#define QE_QESET_OFIE                       QE_QESET_OFIE_Msk

#define QE_QESET_OFIE_FALSE                 (0x0UL << QE_QESET_OFIE_Pos)        /*!< False */
#define QE_QESET_OFIE_TRUE                  (0x1UL << QE_QESET_OFIE_Pos)        /*!< True */

/** @brief QESET[12]: INDX interrupt enable */
#define QE_QESET_INIE_Pos                   (12U)
#define QE_QESET_INIE_Msk                   (0x1UL << QE_QESET_INIE_Pos)
#define QE_QESET_INIE                       QE_QESET_INIE_Msk

#define QE_QESET_INIE_FALSE                 (0x0UL << QE_QESET_INIE_Pos)        /*!< False */
#define QE_QESET_INIE_TRUE                  (0x1UL << QE_QESET_INIE_Pos)        /*!< True */

/** @brief QESET[13]: Interrupt clear */
#define QE_QESET_INTRPT_CLR_Pos             (13U)
#define QE_QESET_INTRPT_CLR_Msk             (0x1UL << QE_QESET_INTRPT_CLR_Pos)
#define QE_QESET_INTRPT_CLR                 QE_QESET_INTRPT_CLR_Msk

#define QE_QESET_INTRPT_CLR_FALSE           (0x0UL << QE_QESET_INTRPT_CLR_Pos)  /*!< False */
#define QE_QESET_INTRPT_CLR_TRUE            (0x1UL << QE_QESET_INTRPT_CLR_Pos)  /*!< True */

/******************* Bit definition for QE_FILTSET register *******************/

/** @brief FILTSET[3:0]: QA filter setting */
#define QE_FILTSET_QASF_Pos                 (0U)
#define QE_FILTSET_QASF_Msk                 (0xFUL << QE_FILTSET_QASF_Pos)
#define QE_FILTSET_QASF                     QE_FILTSET_QASF_Msk

#define QE_FILTSET_QASF_0000                (0x0UL << QE_FILTSET_QASF_Pos)      /*!< f_sampling=f_dts; Filter not used */
#define QE_FILTSET_QASF_0001                (0x1UL << QE_FILTSET_QASF_Pos)      /*!< f_sampling=f_ck_int, N=2 */
#define QE_FILTSET_QASF_0010                (0x2UL << QE_FILTSET_QASF_Pos)      /*!< f_sampling=f_ck_int, N=4 */
#define QE_FILTSET_QASF_0011                (0x3UL << QE_FILTSET_QASF_Pos)      /*!< f_sampling=f_ck_int, N=8 */
#define QE_FILTSET_QASF_0100                (0x4UL << QE_FILTSET_QASF_Pos)      /*!< f_sampling=f_dts/2, N=6 */
#define QE_FILTSET_QASF_0101                (0x5UL << QE_FILTSET_QASF_Pos)      /*!< f_sampling=f_dts/2, N=8 */
#define QE_FILTSET_QASF_0110                (0x6UL << QE_FILTSET_QASF_Pos)      /*!< f_sampling=f_dts/4, N=6 */
#define QE_FILTSET_QASF_0111                (0x7UL << QE_FILTSET_QASF_Pos)      /*!< f_sampling=f_dts/4, N=8 */
#define QE_FILTSET_QASF_1000                (0x8UL << QE_FILTSET_QASF_Pos)      /*!< f_sampling=f_dts/8, N=6 */
#define QE_FILTSET_QASF_1001                (0x9UL << QE_FILTSET_QASF_Pos)      /*!< f_sampling=f_dts/8, N=8 */
#define QE_FILTSET_QASF_1010                (0xAUL << QE_FILTSET_QASF_Pos)      /*!< f_sampling=f_dts/16, N=5 */
#define QE_FILTSET_QASF_1011                (0xBUL << QE_FILTSET_QASF_Pos)      /*!< f_sampling=f_dts/16, N=6 */
#define QE_FILTSET_QASF_1100                (0xCUL << QE_FILTSET_QASF_Pos)      /*!< f_sampling=f_dts/16, N=8 */
#define QE_FILTSET_QASF_1101                (0xDUL << QE_FILTSET_QASF_Pos)      /*!< f_sampling=f_dts/32, N=5 */
#define QE_FILTSET_QASF_1110                (0xEUL << QE_FILTSET_QASF_Pos)      /*!< f_sampling=f_dts/32, N=6 */
#define QE_FILTSET_QASF_1111                (0xFUL << QE_FILTSET_QASF_Pos)      /*!< f_sampling=f_dts/32, N=8 */

/** @brief FILTSET[7:4]: QB filter setting */
#define QE_FILTSET_QBSF_Pos                 (4U)
#define QE_FILTSET_QBSF_Msk                 (0xFUL << QE_FILTSET_QBSF_Pos)
#define QE_FILTSET_QBSF                     QE_FILTSET_QBSF_Msk

#define QE_FILTSET_QBSF_0000                (0x0UL << QE_FILTSET_QBSF_Pos)      /*!< f_sampling=f_dts; Filter not used */
#define QE_FILTSET_QBSF_0001                (0x1UL << QE_FILTSET_QBSF_Pos)      /*!< f_sampling=f_ck_int, N=2 */
#define QE_FILTSET_QBSF_0010                (0x2UL << QE_FILTSET_QBSF_Pos)      /*!< f_sampling=f_ck_int, N=4 */
#define QE_FILTSET_QBSF_0011                (0x3UL << QE_FILTSET_QBSF_Pos)      /*!< f_sampling=f_ck_int, N=8 */
#define QE_FILTSET_QBSF_0100                (0x4UL << QE_FILTSET_QBSF_Pos)      /*!< f_sampling=f_dts/2, N=6 */
#define QE_FILTSET_QBSF_0101                (0x5UL << QE_FILTSET_QBSF_Pos)      /*!< f_sampling=f_dts/2, N=8 */
#define QE_FILTSET_QBSF_0110                (0x6UL << QE_FILTSET_QBSF_Pos)      /*!< f_sampling=f_dts/4, N=6 */
#define QE_FILTSET_QBSF_0111                (0x7UL << QE_FILTSET_QBSF_Pos)      /*!< f_sampling=f_dts/4, N=8 */
#define QE_FILTSET_QBSF_1000                (0x8UL << QE_FILTSET_QBSF_Pos)      /*!< f_sampling=f_dts/8, N=6 */
#define QE_FILTSET_QBSF_1001                (0x9UL << QE_FILTSET_QBSF_Pos)      /*!< f_sampling=f_dts/8, N=8 */
#define QE_FILTSET_QBSF_1010                (0xAUL << QE_FILTSET_QBSF_Pos)      /*!< f_sampling=f_dts/16, N=5 */
#define QE_FILTSET_QBSF_1011                (0xBUL << QE_FILTSET_QBSF_Pos)      /*!< f_sampling=f_dts/16, N=6 */
#define QE_FILTSET_QBSF_1100                (0xCUL << QE_FILTSET_QBSF_Pos)      /*!< f_sampling=f_dts/16, N=8 */
#define QE_FILTSET_QBSF_1101                (0xDUL << QE_FILTSET_QBSF_Pos)      /*!< f_sampling=f_dts/32, N=5 */
#define QE_FILTSET_QBSF_1110                (0xEUL << QE_FILTSET_QBSF_Pos)      /*!< f_sampling=f_dts/32, N=6 */
#define QE_FILTSET_QBSF_1111                (0xFUL << QE_FILTSET_QBSF_Pos)      /*!< f_sampling=f_dts/32, N=8 */

/** @brief FILTSET[11:8]: INDF filter setting */
#define QE_FILTSET_INDXSF_Pos               (8U)
#define QE_FILTSET_INDXSF_Msk               (0xFUL << QE_FILTSET_INDXSF_Pos)
#define QE_FILTSET_INDXSF                   QE_FILTSET_INDXSF_Msk

#define QE_FILTSET_INDXSF_0000              (0x0UL << QE_FILTSET_INDXSF_Pos)    /*!< f_sampling=f_dts; Filter not used */
#define QE_FILTSET_INDXSF_0001              (0x1UL << QE_FILTSET_INDXSF_Pos)    /*!< f_sampling=f_ck_int, N=2 */
#define QE_FILTSET_INDXSF_0010              (0x2UL << QE_FILTSET_INDXSF_Pos)    /*!< f_sampling=f_ck_int, N=4 */
#define QE_FILTSET_INDXSF_0011              (0x3UL << QE_FILTSET_INDXSF_Pos)    /*!< f_sampling=f_ck_int, N=8 */
#define QE_FILTSET_INDXSF_0100              (0x4UL << QE_FILTSET_INDXSF_Pos)    /*!< f_sampling=f_dts/2, N=6 */
#define QE_FILTSET_INDXSF_0101              (0x5UL << QE_FILTSET_INDXSF_Pos)    /*!< f_sampling=f_dts/2, N=8 */
#define QE_FILTSET_INDXSF_0110              (0x6UL << QE_FILTSET_INDXSF_Pos)    /*!< f_sampling=f_dts/4, N=6 */
#define QE_FILTSET_INDXSF_0111              (0x7UL << QE_FILTSET_INDXSF_Pos)    /*!< f_sampling=f_dts/4, N=8 */
#define QE_FILTSET_INDXSF_1000              (0x8UL << QE_FILTSET_INDXSF_Pos)    /*!< f_sampling=f_dts/8, N=6 */
#define QE_FILTSET_INDXSF_1001              (0x9UL << QE_FILTSET_INDXSF_Pos)    /*!< f_sampling=f_dts/8, N=8 */
#define QE_FILTSET_INDXSF_1010              (0xAUL << QE_FILTSET_INDXSF_Pos)    /*!< f_sampling=f_dts/16, N=5 */
#define QE_FILTSET_INDXSF_1011              (0xBUL << QE_FILTSET_INDXSF_Pos)    /*!< f_sampling=f_dts/16, N=6 */
#define QE_FILTSET_INDXSF_1100              (0xCUL << QE_FILTSET_INDXSF_Pos)    /*!< f_sampling=f_dts/16, N=8 */
#define QE_FILTSET_INDXSF_1101              (0xDUL << QE_FILTSET_INDXSF_Pos)    /*!< f_sampling=f_dts/32, N=5 */
#define QE_FILTSET_INDXSF_1110              (0xEUL << QE_FILTSET_INDXSF_Pos)    /*!< f_sampling=f_dts/32, N=6 */
#define QE_FILTSET_INDXSF_1111              (0xFUL << QE_FILTSET_INDXSF_Pos)    /*!< f_sampling=f_dts/32, N=8 */

/******************** Bit definition for QE_QEMAX register ********************/

/** @brief QEMAX[15:0]: Max counter value register */
#define QE_QEMAX_QEMAX_Pos                  (0U)
#define QE_QEMAX_QEMAX_Msk                  (0xFFFFUL << QE_QEMAX_QEMAX_Pos)
#define QE_QEMAX_QEMAX                      QE_QEMAX_QEMAX_Msk

/******************* Bit definition for QE_INITSET register *******************/

/** @brief INITSET[15:0]: Init counter value register */
#define QE_INITSET_INITSET_Pos              (0U)
#define QE_INITSET_INITSET_Msk              (0xFFFFUL << QE_INITSET_INITSET_Pos)
#define QE_INITSET_INITSET                  QE_INITSET_INITSET_Msk

/******************** Bit definition for QE_QECNT register ********************/

/** @brief QECNT[15:0]: Current counter value register */
#define QE_QECNT_QECNT_Pos                  (0U)
#define QE_QECNT_QECNT_Msk                  (0xFFFFUL << QE_QECNT_QECNT_Pos)
#define QE_QECNT_QECNT                      QE_QECNT_QECNT_Msk

/******************************************************************************/
/*                                    PWMG                                    */
/******************************************************************************/

/******************** Bit definition for PWMG_CR1 register ********************/

/** @brief CR1[0]: Counter enable */
#define PWMG_CR1_CEN_Pos                    (0U)
#define PWMG_CR1_CEN_Msk                    (0x1UL << PWMG_CR1_CEN_Pos)
#define PWMG_CR1_CEN                        PWMG_CR1_CEN_Msk

#define PWMG_CR1_CEN_FALSE                  (0x0UL << PWMG_CR1_CEN_Pos)         /*!< False */
#define PWMG_CR1_CEN_TRUE                   (0x1UL << PWMG_CR1_CEN_Pos)         /*!< True */

/** @brief CR1[1]: Update disable */
#define PWMG_CR1_UDIS_INV_Pos               (1U)
#define PWMG_CR1_UDIS_INV_Msk               (0x1UL << PWMG_CR1_UDIS_INV_Pos)
#define PWMG_CR1_UDIS_INV                   PWMG_CR1_UDIS_INV_Msk

#define PWMG_CR1_UDIS_INV_FALSE             (0x0UL << PWMG_CR1_UDIS_INV_Pos)    /*!< False */
#define PWMG_CR1_UDIS_INV_TRUE              (0x1UL << PWMG_CR1_UDIS_INV_Pos)    /*!< True */

/** @brief CR1[3]: One pulse mode */
#define PWMG_CR1_OPM_Pos                    (3U)
#define PWMG_CR1_OPM_Msk                    (0x1UL << PWMG_CR1_OPM_Pos)
#define PWMG_CR1_OPM                        PWMG_CR1_OPM_Msk

#define PWMG_CR1_OPM_FALSE                  (0x0UL << PWMG_CR1_OPM_Pos)         /*!< False */
#define PWMG_CR1_OPM_TRUE                   (0x1UL << PWMG_CR1_OPM_Pos)         /*!< True */

/** @brief CR1[7]: Auto-reload preload enable */
#define PWMG_CR1_ARPE_Pos                   (7U)
#define PWMG_CR1_ARPE_Msk                   (0x1UL << PWMG_CR1_ARPE_Pos)
#define PWMG_CR1_ARPE                       PWMG_CR1_ARPE_Msk

#define PWMG_CR1_ARPE_FALSE                 (0x0UL << PWMG_CR1_ARPE_Pos)        /*!< False */
#define PWMG_CR1_ARPE_TRUE                  (0x1UL << PWMG_CR1_ARPE_Pos)        /*!< True */

/** @brief CR1[9:8]: Clock division */
#define PWMG_CR1_CKD_Pos                    (8U)
#define PWMG_CR1_CKD_Msk                    (0x3UL << PWMG_CR1_CKD_Pos)
#define PWMG_CR1_CKD                        PWMG_CR1_CKD_Msk

#define PWMG_CR1_CKD_00                     (0x0UL << PWMG_CR1_CKD_Pos)         /*!< 1 */
#define PWMG_CR1_CKD_01                     (0x1UL << PWMG_CR1_CKD_Pos)         /*!< 2 */
#define PWMG_CR1_CKD_10                     (0x2UL << PWMG_CR1_CKD_Pos)         /*!< 4 */

/******************* Bit definition for PWMG_DIER register ********************/

/** @brief DIER[0]: Update interrupt enable */
#define PWMG_DIER_UIE_Pos                   (0U)
#define PWMG_DIER_UIE_Msk                   (0x1UL << PWMG_DIER_UIE_Pos)
#define PWMG_DIER_UIE                       PWMG_DIER_UIE_Msk

#define PWMG_DIER_UIE_FALSE                 (0x0UL << PWMG_DIER_UIE_Pos)        /*!< False */
#define PWMG_DIER_UIE_TRUE                  (0x1UL << PWMG_DIER_UIE_Pos)        /*!< True */

/** @brief DIER[1]: Capture/Compare 0 interrupt enable */
#define PWMG_DIER_CC0IE_Pos                 (1U)
#define PWMG_DIER_CC0IE_Msk                 (0x1UL << PWMG_DIER_CC0IE_Pos)
#define PWMG_DIER_CC0IE                     PWMG_DIER_CC0IE_Msk

#define PWMG_DIER_CC0IE_FALSE               (0x0UL << PWMG_DIER_CC0IE_Pos)      /*!< False */
#define PWMG_DIER_CC0IE_TRUE                (0x1UL << PWMG_DIER_CC0IE_Pos)      /*!< True */

/** @brief DIER[15]: Interrupt clear */
#define PWMG_DIER_INTR_CLEAR_Pos            (15U)
#define PWMG_DIER_INTR_CLEAR_Msk            (0x1UL << PWMG_DIER_INTR_CLEAR_Pos)
#define PWMG_DIER_INTR_CLEAR                PWMG_DIER_INTR_CLEAR_Msk

#define PWMG_DIER_INTR_CLEAR_FALSE          (0x0UL << PWMG_DIER_INTR_CLEAR_Pos) /*!< False */
#define PWMG_DIER_INTR_CLEAR_TRUE           (0x1UL << PWMG_DIER_INTR_CLEAR_Pos) /*!< True */

/******************** Bit definition for PWMG_SR register *********************/

/** @brief SR[0]: Update interrupt flag */
#define PWMG_SR_UIF_Pos                     (0U)
#define PWMG_SR_UIF_Msk                     (0x1UL << PWMG_SR_UIF_Pos)
#define PWMG_SR_UIF                         PWMG_SR_UIF_Msk

#define PWMG_SR_UIF_FALSE                   (0x0UL << PWMG_SR_UIF_Pos)          /*!< False */
#define PWMG_SR_UIF_TRUE                    (0x1UL << PWMG_SR_UIF_Pos)          /*!< True */

/** @brief SR[1]: Capture/Compare 0 interrupt flag */
#define PWMG_SR_CC0IF_Pos                   (1U)
#define PWMG_SR_CC0IF_Msk                   (0x1UL << PWMG_SR_CC0IF_Pos)
#define PWMG_SR_CC0IF                       PWMG_SR_CC0IF_Msk

#define PWMG_SR_CC0IF_FALSE                 (0x0UL << PWMG_SR_CC0IF_Pos)        /*!< False */
#define PWMG_SR_CC0IF_TRUE                  (0x1UL << PWMG_SR_CC0IF_Pos)        /*!< True */

/** @brief SR[9]: Capture/Compare 0 overcapture flag */
#define PWMG_SR_CC0OF_Pos                   (9U)
#define PWMG_SR_CC0OF_Msk                   (0x1UL << PWMG_SR_CC0OF_Pos)
#define PWMG_SR_CC0OF                       PWMG_SR_CC0OF_Msk

#define PWMG_SR_CC0OF_FALSE                 (0x0UL << PWMG_SR_CC0OF_Pos)        /*!< False */
#define PWMG_SR_CC0OF_TRUE                  (0x1UL << PWMG_SR_CC0OF_Pos)        /*!< True */

/******************** Bit definition for PWMG_EGR register ********************/

/** @brief EGR[0]: Update generation */
#define PWMG_EGR_UG_Pos                     (0U)
#define PWMG_EGR_UG_Msk                     (0x1UL << PWMG_EGR_UG_Pos)
#define PWMG_EGR_UG                         PWMG_EGR_UG_Msk

#define PWMG_EGR_UG_FALSE                   (0x0UL << PWMG_EGR_UG_Pos)          /*!< False */
#define PWMG_EGR_UG_TRUE                    (0x1UL << PWMG_EGR_UG_Pos)          /*!< True */

/** @brief EGR[1]: Capture/Compare 0 generation */
#define PWMG_EGR_CC0G_Pos                   (1U)
#define PWMG_EGR_CC0G_Msk                   (0x1UL << PWMG_EGR_CC0G_Pos)
#define PWMG_EGR_CC0G                       PWMG_EGR_CC0G_Msk

#define PWMG_EGR_CC0G_FALSE                 (0x0UL << PWMG_EGR_CC0G_Pos)        /*!< False */
#define PWMG_EGR_CC0G_TRUE                  (0x1UL << PWMG_EGR_CC0G_Pos)        /*!< True */

/**************** Bit definition for PWMG_CCMR1_MODE0 register ****************/

/** @brief CCMR1_MODE0[1:0]: Capture/Compare 0 selection */
#define PWMG_CCMR1_MODE0_CC0S_MODE0_Pos     (0U)
#define PWMG_CCMR1_MODE0_CC0S_MODE0_Msk     (0x3UL << PWMG_CCMR1_MODE0_CC0S_MODE0_Pos)
#define PWMG_CCMR1_MODE0_CC0S_MODE0         PWMG_CCMR1_MODE0_CC0S_MODE0_Msk

#define PWMG_CCMR1_MODE0_CC0S_MODE0_00      (0x0UL << PWMG_CCMR1_MODE0_CC0S_MODE0_Pos) /*!< Channel 0 is configured as output */
#define PWMG_CCMR1_MODE0_CC0S_MODE0_11      (0x3UL << PWMG_CCMR1_MODE0_CC0S_MODE0_Pos) /*!< Channel 0 is configured as input;
                                                                                            IC0 signal is connected to TI0 when SMCR_TS=1 */

/** @brief CCMR1_MODE0[3]: Output Compare 0 preload enable */
#define PWMG_CCMR1_MODE0_OC0PE_Pos          (3U)
#define PWMG_CCMR1_MODE0_OC0PE_Msk          (0x1UL << PWMG_CCMR1_MODE0_OC0PE_Pos)
#define PWMG_CCMR1_MODE0_OC0PE              PWMG_CCMR1_MODE0_OC0PE_Msk

#define PWMG_CCMR1_MODE0_OC0PE_FALSE        (0x0UL << PWMG_CCMR1_MODE0_OC0PE_Pos) /*!< False */
#define PWMG_CCMR1_MODE0_OC0PE_TRUE         (0x1UL << PWMG_CCMR1_MODE0_OC0PE_Pos) /*!< True */

/** @brief CCMR1_MODE0[6:4]: Output Compare 0 mode */
#define PWMG_CCMR1_MODE0_OC0M_Pos           (4U)
#define PWMG_CCMR1_MODE0_OC0M_Msk           (0x7UL << PWMG_CCMR1_MODE0_OC0M_Pos)
#define PWMG_CCMR1_MODE0_OC0M               PWMG_CCMR1_MODE0_OC0M_Msk

#define PWMG_CCMR1_MODE0_OC0M_000           (0x0UL << PWMG_CCMR1_MODE0_OC0M_Pos) /*!< Frozen; When CNT=CCR0, the output states do not change. */
#define PWMG_CCMR1_MODE0_OC0M_001           (0x1UL << PWMG_CCMR1_MODE0_OC0M_Pos) /*!< Set the channel 0 output to the active level when
                                                                                    the comparison circuit is triggered; When CNT=CCR0, OC0REF=1 is set. */
#define PWMG_CCMR1_MODE0_OC0M_010           (0x2UL << PWMG_CCMR1_MODE0_OC0M_Pos) /*!< Set the channel 0 output to the inactive level when
                                                                                    the comparison circuit is triggered; When CNT=CCR0, OC0REF=0 is set. */
#define PWMG_CCMR1_MODE0_OC0M_011           (0x3UL << PWMG_CCMR1_MODE0_OC0M_Pos) /*!< Toggle; When CNT=CCR0, OC0REF = not OC0REF is set. */
#define PWMG_CCMR1_MODE0_OC0M_100           (0x4UL << PWMG_CCMR1_MODE0_OC0M_Pos) /*!< Force switch to the inactive state; OC0REF=0. */
#define PWMG_CCMR1_MODE0_OC0M_101           (0x5UL << PWMG_CCMR1_MODE0_OC0M_Pos) /*!< Force switch to the active state; OC0REF=1. */
#define PWMG_CCMR1_MODE0_OC0M_110           (0x6UL << PWMG_CCMR1_MODE0_OC0M_Pos) /*!< "PWM mode 1"; When counting up, channel 0 is active (OC0REF=1)
                                                                                    until (CNT less-than-or-equal CCR0), otherwise channel 0 is inactive (OC0REF=0);
                                                                                    When counting down, channel 0 is inactive (OC0REF=0)
                                                                                    until (CNT greater-than-or-equal CCR0), otherwise channel 0 is active (OC0REF=1). */
#define PWMG_CCMR1_MODE0_OC0M_111           (0x7UL << PWMG_CCMR1_MODE0_OC0M_Pos) /*!< "PWM Mode 2"; When counting up, channel 0 is inactive (OC0REF=0)
                                                                                    until (CNT less-than-or-equal CCR0), otherwise channel 0 is active (OC0REF=1);
                                                                                    When counting down, channel 0 is active (OC0REF=1)
                                                                                    until (CNT greater-than-or-equal CCR0), otherwise channel 0 is inactive (OC0REF=1). */

/**************** Bit definition for PWMG_CCMR1_MODE1 register ****************/

/** @brief CCMR1_MODE1[1:0]: Capture/Compare 0 selection */
#define PWMG_CCMR1_MODE1_CC0S_MODE1_Pos     (0U)
#define PWMG_CCMR1_MODE1_CC0S_MODE1_Msk     (0x3UL << PWMG_CCMR1_MODE1_CC0S_MODE1_Pos)
#define PWMG_CCMR1_MODE1_CC0S_MODE1         PWMG_CCMR1_MODE1_CC0S_MODE1_Msk

#define PWMG_CCMR1_MODE1_CC0S_MODE1_00      (0x0UL << PWMG_CCMR1_MODE1_CC0S_MODE1_Pos) /*!< Channel 0 is configured as output */
#define PWMG_CCMR1_MODE1_CC0S_MODE1_11      (0x3UL << PWMG_CCMR1_MODE1_CC0S_MODE1_Pos) /*!< Channel 0 is configured as input;
                                                                                            IC0 signal is connected to TI0 when SMCR_TS=1 */

/** @brief CCMR1_MODE1[3:2]: Input capture 0 prescaler */
#define PWMG_CCMR1_MODE1_IC0PSC_Pos         (2U)
#define PWMG_CCMR1_MODE1_IC0PSC_Msk         (0x3UL << PWMG_CCMR1_MODE1_IC0PSC_Pos)
#define PWMG_CCMR1_MODE1_IC0PSC             PWMG_CCMR1_MODE1_IC0PSC_Msk

#define PWMG_CCMR1_MODE1_IC0PSC_00          (0x0UL << PWMG_CCMR1_MODE1_IC0PSC_Pos) /*!< 1 */
#define PWMG_CCMR1_MODE1_IC0PSC_01          (0x1UL << PWMG_CCMR1_MODE1_IC0PSC_Pos) /*!< 2 */
#define PWMG_CCMR1_MODE1_IC0PSC_10          (0x2UL << PWMG_CCMR1_MODE1_IC0PSC_Pos) /*!< 4 */
#define PWMG_CCMR1_MODE1_IC0PSC_11          (0x3UL << PWMG_CCMR1_MODE1_IC0PSC_Pos) /*!< 8 */

/** @brief CCMR1_MODE1[7:4]: Input capture 0 filter */
#define PWMG_CCMR1_MODE1_IC0F_Pos           (4U)
#define PWMG_CCMR1_MODE1_IC0F_Msk           (0xFUL << PWMG_CCMR1_MODE1_IC0F_Pos)
#define PWMG_CCMR1_MODE1_IC0F               PWMG_CCMR1_MODE1_IC0F_Msk

#define PWMG_CCMR1_MODE1_IC0F_0000          (0x0UL << PWMG_CCMR1_MODE1_IC0F_Pos) /*!< f_sampling=f_dts; Filter not used */
#define PWMG_CCMR1_MODE1_IC0F_0001          (0x1UL << PWMG_CCMR1_MODE1_IC0F_Pos) /*!< f_sampling=f_ck_int, N=2 */
#define PWMG_CCMR1_MODE1_IC0F_0010          (0x2UL << PWMG_CCMR1_MODE1_IC0F_Pos) /*!< f_sampling=f_ck_int, N=4 */
#define PWMG_CCMR1_MODE1_IC0F_0011          (0x3UL << PWMG_CCMR1_MODE1_IC0F_Pos) /*!< f_sampling=f_ck_int, N=8 */
#define PWMG_CCMR1_MODE1_IC0F_0100          (0x4UL << PWMG_CCMR1_MODE1_IC0F_Pos) /*!< f_sampling=f_dts/2, N=6 */
#define PWMG_CCMR1_MODE1_IC0F_0101          (0x5UL << PWMG_CCMR1_MODE1_IC0F_Pos) /*!< f_sampling=f_dts/2, N=8 */
#define PWMG_CCMR1_MODE1_IC0F_0110          (0x6UL << PWMG_CCMR1_MODE1_IC0F_Pos) /*!< f_sampling=f_dts/4, N=6 */
#define PWMG_CCMR1_MODE1_IC0F_0111          (0x7UL << PWMG_CCMR1_MODE1_IC0F_Pos) /*!< f_sampling=f_dts/4, N=8 */
#define PWMG_CCMR1_MODE1_IC0F_1000          (0x8UL << PWMG_CCMR1_MODE1_IC0F_Pos) /*!< f_sampling=f_dts/8, N=6 */
#define PWMG_CCMR1_MODE1_IC0F_1001          (0x9UL << PWMG_CCMR1_MODE1_IC0F_Pos) /*!< f_sampling=f_dts/8, N=8 */
#define PWMG_CCMR1_MODE1_IC0F_1010          (0xAUL << PWMG_CCMR1_MODE1_IC0F_Pos) /*!< f_sampling=f_dts/16, N=5 */
#define PWMG_CCMR1_MODE1_IC0F_1011          (0xBUL << PWMG_CCMR1_MODE1_IC0F_Pos) /*!< f_sampling=f_dts/16, N=6 */
#define PWMG_CCMR1_MODE1_IC0F_1100          (0xCUL << PWMG_CCMR1_MODE1_IC0F_Pos) /*!< f_sampling=f_dts/16, N=8 */
#define PWMG_CCMR1_MODE1_IC0F_1101          (0xDUL << PWMG_CCMR1_MODE1_IC0F_Pos) /*!< f_sampling=f_dts/32, N=5 */
#define PWMG_CCMR1_MODE1_IC0F_1110          (0xEUL << PWMG_CCMR1_MODE1_IC0F_Pos) /*!< f_sampling=f_dts/32, N=6 */
#define PWMG_CCMR1_MODE1_IC0F_1111          (0xFUL << PWMG_CCMR1_MODE1_IC0F_Pos) /*!< f_sampling=f_dts/32, N=8 */

/******************* Bit definition for PWMG_CCER register ********************/

/** @brief CCER[0]: Capture/Compare 0 output enable */
#define PWMG_CCER_CC0E_Pos                  (0U)
#define PWMG_CCER_CC0E_Msk                  (0x1UL << PWMG_CCER_CC0E_Pos)
#define PWMG_CCER_CC0E                      PWMG_CCER_CC0E_Msk

#define PWMG_CCER_CC0E_FALSE                (0x0UL << PWMG_CCER_CC0E_Pos)       /*!< False */
#define PWMG_CCER_CC0E_TRUE                 (0x1UL << PWMG_CCER_CC0E_Pos)       /*!< True */

/** @brief CCER[1]: Capture/Compare 0 output polarity */
#define PWMG_CCER_CC0P_Pos                  (1U)
#define PWMG_CCER_CC0P_Msk                  (0x1UL << PWMG_CCER_CC0P_Pos)
#define PWMG_CCER_CC0P                      PWMG_CCER_CC0P_Msk

#define PWMG_CCER_CC0P_FALSE                (0x0UL << PWMG_CCER_CC0P_Pos)       /*!< False */
#define PWMG_CCER_CC0P_TRUE                 (0x1UL << PWMG_CCER_CC0P_Pos)       /*!< True */

/******************** Bit definition for PWMG_CNT register ********************/

/** @brief CNT[15:0]: Counter */
#define PWMG_CNT_CNT_Pos                    (0U)
#define PWMG_CNT_CNT_Msk                    (0xFFFFUL << PWMG_CNT_CNT_Pos)
#define PWMG_CNT_CNT                        PWMG_CNT_CNT_Msk

/******************** Bit definition for PWMG_PSC register ********************/

/** @brief PSC[15:0]: Prescaler */
#define PWMG_PSC_PSC_Pos                    (0U)
#define PWMG_PSC_PSC_Msk                    (0xFFFFUL << PWMG_PSC_PSC_Pos)
#define PWMG_PSC_PSC                        PWMG_PSC_PSC_Msk

/******************** Bit definition for PWMG_ARR register ********************/

/** @brief ARR[15:0]: Auto-reload register */
#define PWMG_ARR_ARR_Pos                    (0U)
#define PWMG_ARR_ARR_Msk                    (0xFFFFUL << PWMG_ARR_ARR_Pos)
#define PWMG_ARR_ARR                        PWMG_ARR_ARR_Msk

/******************* Bit definition for PWMG_CCR0 register ********************/

/** @brief CCR0[15:0]: Capture/Compare register 0 */
#define PWMG_CCR0_CCR0_Pos                  (0U)
#define PWMG_CCR0_CCR0_Msk                  (0xFFFFUL << PWMG_CCR0_CCR0_Pos)
#define PWMG_CCR0_CCR0                      PWMG_CCR0_CCR0_Msk

/******************************************************************************/
/*                          WatchDog Timer (WDT)                              */
/******************************************************************************/

/******************* Bit definition for CR register ********************/

/** @brief CR[0]: Enable and disable the WDT */
#define WDT_CR_WDT_EN_Pos                   (0U)
#define WDT_CR_WDT_EN_Msk                   (0x1UL << WDT_CR_WDT_EN_Pos)
#define WDT_CR_WDT_EN                       WDT_CR_WDT_EN_Msk

#define WDT_CR_WDT_EN_DISABLED              (0x0UL << WDT_CR_WDT_EN_Pos)        /*!< Watchdog timer disabled */
#define WDT_CR_WDT_EN_ENABLED               (0x1UL << WDT_CR_WDT_EN_Pos)        /*!< Watchdog timer enabled */

/** @brief CR[1]: Response mode */
#define WDT_CR_RMOD_Pos                     (1U)
#define WDT_CR_RMOD_Msk                     (0x1UL << WDT_CR_RMOD_Pos)
#define WDT_CR_RMOD                         WDT_CR_RMOD_Msk

#define WDT_CR_RMOD_RESET                   (0x0UL << WDT_CR_RMOD_Pos)          /*!< Generate a system reset */
#define WDT_CR_RMOD_INTERRUPT               (0x1UL << WDT_CR_RMOD_Pos)          /*!< First generate an interrupt, then a system reset */

/** @brief CR[4:2]: Reset pulse length */
#define WDT_CR_RPL_Pos                      (2U)
#define WDT_CR_RPL_Msk                      (0x7UL << WDT_CR_RPL_Pos)
#define WDT_CR_RPL                          WDT_CR_RPL_Msk

#define WDT_CR_RPL_CYCLES_2                 (0x0UL << WDT_CR_RPL_Pos)           /*!< 2 clock cycles */
#define WDT_CR_RPL_CYCLES_4                 (0x1UL << WDT_CR_RPL_Pos)           /*!< 4 clock cycles */
#define WDT_CR_RPL_CYCLES_8                 (0x2UL << WDT_CR_RPL_Pos)           /*!< 8 clock cycles */
#define WDT_CR_RPL_CYCLES_16                (0x3UL << WDT_CR_RPL_Pos)           /*!< 16 clock cycles */
#define WDT_CR_RPL_CYCLES_32                (0x4UL << WDT_CR_RPL_Pos)           /*!< 32 clock cycles */
#define WDT_CR_RPL_CYCLES_64                (0x5UL << WDT_CR_RPL_Pos)           /*!< 64 clock cycles */
#define WDT_CR_RPL_CYCLES_128               (0x6UL << WDT_CR_RPL_Pos)           /*!< 128 clock cycles */
#define WDT_CR_RPL_CYCLES_256               (0x7UL << WDT_CR_RPL_Pos)           /*!< 256 clock cycles */

/******************* Bit definition for TORR register ********************/

/** @brief TORR[3:0]: Timeout period */
#define WDT_TORR_TOP_Pos                    (0U)
#define WDT_TORR_TOP_Msk                    (0xFUL << WDT_TORR_TOP_Pos)
#define WDT_TORR_TOP                        WDT_TORR_TOP_Msk

#define WDT_TORR_TOP_USER_0                 (0x0UL << WDT_TORR_TOP_Pos)         /*!< 0xFF cycles */
#define WDT_TORR_TOP_USER_1                 (0x1UL << WDT_TORR_TOP_Pos)         /*!< 0x1FF cycles */
#define WDT_TORR_TOP_USER_2                 (0x2UL << WDT_TORR_TOP_Pos)         /*!< 0x3FF cycles */
#define WDT_TORR_TOP_USER_3                 (0x3UL << WDT_TORR_TOP_Pos)         /*!< 0x7FF cycles */
#define WDT_TORR_TOP_USER_4                 (0x4UL << WDT_TORR_TOP_Pos)         /*!< 0xFFFF cycles */
#define WDT_TORR_TOP_USER_5                 (0x5UL << WDT_TORR_TOP_Pos)         /*!< 0x1FFFF cycles */
#define WDT_TORR_TOP_USER_6                 (0x6UL << WDT_TORR_TOP_Pos)         /*!< 0x3FFFF cycles */
#define WDT_TORR_TOP_USER_7                 (0x7UL << WDT_TORR_TOP_Pos)         /*!< 0x7FFFF cycles */
#define WDT_TORR_TOP_USER_8                 (0x8UL << WDT_TORR_TOP_Pos)         /*!< 0xFFFFF cycles */
#define WDT_TORR_TOP_USER_9                 (0x9UL << WDT_TORR_TOP_Pos)         /*!< 0x1FFFFF cycles */
#define WDT_TORR_TOP_USER_10                (0xAUL << WDT_TORR_TOP_Pos)         /*!< 0x3FFFFF cycles */
#define WDT_TORR_TOP_USER_11                (0xBUL << WDT_TORR_TOP_Pos)         /*!< 0x7FFFFF cycles */
#define WDT_TORR_TOP_USER_12                (0xCUL << WDT_TORR_TOP_Pos)         /*!< 0xFFFFFF cycles */
#define WDT_TORR_TOP_USER_13                (0xDUL << WDT_TORR_TOP_Pos)         /*!< 0x1FFFFFF cycles */
#define WDT_TORR_TOP_USER_14                (0xEUL << WDT_TORR_TOP_Pos)         /*!< 0x3FFFFFF cycles */
#define WDT_TORR_TOP_USER_15                (0xFUL << WDT_TORR_TOP_Pos)         /*!< 0x7FFFFFF cycles */

/******************* Bit definition for CCVR register ********************/

/** @brief CCVR[31:0]: WDT Current Counter Value Register */
#define WDT_CCVR_Pos                        (0U)
#define WDT_CCVR_Msk                        (0xFFFFFFFFUL << WDT_CCVR_Pos)
#define WDT_CCVR                            WDT_CCVR_Msk

/******************* Bit definition for CRR register ********************/

/** @brief CRR[7:0]: Counter Restart Register */

#define WDT_CRR_Pos                         (0U)
#define WDT_CRR_Msk                         (0xFFUL << WDT_CRR_Pos)
#define WDT_CRR                             WDT_CRR_Msk

/******************* Bit definition for STAT register ********************/

/** @brief STAT[0]: Interrupt status of the WDT */
#define WDT_STAT_Pos                        (0U)
#define WDT_STAT_Msk                        (0x1UL << WDT_STAT_Pos)
#define WDT_STAT                            WDT_STAT_Msk

#define WDT_STAT_INACTIVE                   (0x0UL << WDT_STAT_Pos)            /*!< Interrupt is inactive */
#define WDT_STAT_ACTIVE                     (0x1UL << WDT_STAT_Pos)            /*!< Interrupt is active regardless of polarity */

/******************* Bit definition for EOI register ********************/

/** @brief EOI[0]: Clears the watchdog interrupt */
#define WDT_EOI_Pos                         (0U)
#define WDT_EOI_Msk                         (0x1UL << WDT_EOI_Pos)
#define WDT_EOI                             WDT_EOI_Msk

/******************************************************************************/
/*                             CORE2 subsystem, INT                           */
/******************************************************************************/

/***************** Bit definition for CORE2_INT_IER register ******************/

/** @brief IER[0]: CORE2 interrupt enable */
#define CORE2_INT_IER_EN_Pos                (0U)
#define CORE2_INT_IER_EN_Msk                (0x1UL << CORE2_INT_IER_EN_Pos)
#define CORE2_INT_IER_EN                    CORE2_INT_IER_EN_Msk

/******************************************************************************/
/*                           CORE2 subsystem, PIO                             */
/******************************************************************************/

/****************** Bit definition for CORE2_PIO_IN register ******************/

/** @brief IN[0]: PIO0 input data */
#define CORE2_PIO_IN_IN0_Pos                (0U)
#define CORE2_PIO_IN_IN0_Msk                (0x1UL << CORE2_PIO_IN_IN0_Pos)
#define CORE2_PIO_IN_IN0                    CORE2_PIO_IN_IN0_Msk

/** @brief IN[1]: PIO1 input data */
#define CORE2_PIO_IN_IN1_Pos                (1U)
#define CORE2_PIO_IN_IN1_Msk                (0x1UL << CORE2_PIO_IN_IN1_Pos)
#define CORE2_PIO_IN_IN1                    CORE2_PIO_IN_IN1_Msk

/** @brief IN[2]: PIO2 input data */
#define CORE2_PIO_IN_IN2_Pos                (2U)
#define CORE2_PIO_IN_IN2_Msk                (0x1UL << CORE2_PIO_IN_IN2_Pos)
#define CORE2_PIO_IN_IN2                    CORE2_PIO_IN_IN2_Msk

/** @brief IN[3]: PIO3 input data */
#define CORE2_PIO_IN_IN3_Pos                (3U)
#define CORE2_PIO_IN_IN3_Msk                (0x1UL << CORE2_PIO_IN_IN3_Pos)
#define CORE2_PIO_IN_IN3                    CORE2_PIO_IN_IN3_Msk

/** @brief IN[4]: PIO4 input data */
#define CORE2_PIO_IN_IN4_Pos                (4U)
#define CORE2_PIO_IN_IN4_Msk                (0x1UL << CORE2_PIO_IN_IN4_Pos)
#define CORE2_PIO_IN_IN4                    CORE2_PIO_IN_IN4_Msk

/** @brief IN[5]: PIO5 input data */
#define CORE2_PIO_IN_IN5_Pos                (5U)
#define CORE2_PIO_IN_IN5_Msk                (0x1UL << CORE2_PIO_IN_IN5_Pos)
#define CORE2_PIO_IN_IN5                    CORE2_PIO_IN_IN5_Msk

/** @brief IN[6]: PIO6 input data */
#define CORE2_PIO_IN_IN6_Pos               (6U)
#define CORE2_PIO_IN_IN6_Msk               (0x1UL << CORE2_PIO_IN_IN6_Pos)
#define CORE2_PIO_IN_IN6                   CORE2_PIO_IN_IN6_Msk

/** @brief IN[7]: PIO7 input data */
#define CORE2_PIO_IN_IN7_Pos               (7U)
#define CORE2_PIO_IN_IN7_Msk               (0x1UL << CORE2_PIO_IN_IN7_Pos)
#define CORE2_PIO_IN_IN7                   CORE2_PIO_IN_IN7_Msk

/** @brief IN[8]: PIO8 input data */
#define CORE2_PIO_IN_IN8_Pos               (8U)
#define CORE2_PIO_IN_IN8_Msk               (0x1UL << CORE2_PIO_IN_IN8_Pos)
#define CORE2_PIO_IN_IN8                   CORE2_PIO_IN_IN8_Msk

/** @brief IN[9]: PIO9 input data */
#define CORE2_PIO_IN_IN9_Pos               (9U)
#define CORE2_PIO_IN_IN9_Msk               (0x1UL << CORE2_PIO_IN_IN9_Pos)
#define CORE2_PIO_IN_IN9                   CORE2_PIO_IN_IN9_Msk

/** @brief IN[10]: PIO10 input data */
#define CORE2_PIO_IN_IN10_Pos              (10U)
#define CORE2_PIO_IN_IN10_Msk              (0x1UL << CORE2_PIO_IN_IN10_Pos)
#define CORE2_PIO_IN_IN10                  CORE2_PIO_IN_IN10_Msk

/** @brief IN[11]: PIO11 input data */
#define CORE2_PIO_IN_IN11_Pos              (11U)
#define CORE2_PIO_IN_IN11_Msk              (0x1UL << CORE2_PIO_IN_IN11_Pos)
#define CORE2_PIO_IN_IN11                  CORE2_PIO_IN_IN11_Msk

/** @brief IN[12]: PIO12 input data */
#define CORE2_PIO_IN_IN12_Pos              (12U)
#define CORE2_PIO_IN_IN12_Msk              (0x1UL << CORE2_PIO_IN_IN12_Pos)
#define CORE2_PIO_IN_IN12                  CORE2_PIO_IN_IN12_Msk

/** @brief IN[13]: PIO13 input data */
#define CORE2_PIO_IN_IN13_Pos              (13U)
#define CORE2_PIO_IN_IN13_Msk              (0x1UL << CORE2_PIO_IN_IN13_Pos)
#define CORE2_PIO_IN_IN13                  CORE2_PIO_IN_IN13_Msk

/** @brief IN[14]: PIO14 input data */
#define CORE2_PIO_IN_IN14_Pos              (14U)
#define CORE2_PIO_IN_IN14_Msk              (0x1UL << CORE2_PIO_IN_IN14_Pos)
#define CORE2_PIO_IN_IN14                  CORE2_PIO_IN_IN14_Msk

/** @brief IN[15]: PIO15 input data */
#define CORE2_PIO_IN_IN15_Pos              (15U)
#define CORE2_PIO_IN_IN15_Msk              (0x1UL << CORE2_PIO_IN_IN15_Pos)
#define CORE2_PIO_IN_IN15                  CORE2_PIO_IN_IN15_Msk

/****************** Bit definition for CORE2_PIO_EN register ******************/

/** @brief EN[0]: PIO0 direction control */
#define CORE2_PIO_EN_EN0_Pos               (0U)
#define CORE2_PIO_EN_EN0_Msk               (0x1UL << CORE2_PIO_EN_EN0_Pos)
#define CORE2_PIO_EN_EN0                   CORE2_PIO_EN_EN0_Msk

/** @brief EN[1]: PIO1 direction control */
#define CORE2_PIO_EN_EN1_Pos               (1U)
#define CORE2_PIO_EN_EN1_Msk               (0x1UL << CORE2_PIO_EN_EN1_Pos)
#define CORE2_PIO_EN_EN1                   CORE2_PIO_EN_EN1_Msk

/** @brief EN[2]: PIO2 direction control */
#define CORE2_PIO_EN_EN2_Pos               (2U)
#define CORE2_PIO_EN_EN2_Msk               (0x1UL << CORE2_PIO_EN_EN2_Pos)
#define CORE2_PIO_EN_EN2                   CORE2_PIO_EN_EN2_Msk

/** @brief EN[3]: PIO3 direction control */
#define CORE2_PIO_EN_EN3_Pos               (3U)
#define CORE2_PIO_EN_EN3_Msk               (0x1UL << CORE2_PIO_EN_EN3_Pos)
#define CORE2_PIO_EN_EN3                   CORE2_PIO_EN_EN3_Msk

/** @brief EN[4]: PIO4 direction control */
#define CORE2_PIO_EN_EN4_Pos               (4U)
#define CORE2_PIO_EN_EN4_Msk               (0x1UL << CORE2_PIO_EN_EN4_Pos)
#define CORE2_PIO_EN_EN4                   CORE2_PIO_EN_EN4_Msk

/** @brief EN[5]: PIO5 direction control */
#define CORE2_PIO_EN_EN5_Pos               (5U)
#define CORE2_PIO_EN_EN5_Msk               (0x1UL << CORE2_PIO_EN_EN5_Pos)
#define CORE2_PIO_EN_EN5                   CORE2_PIO_EN_EN5_Msk

/** @brief EN[6]: PIO6 direction control */
#define CORE2_PIO_EN_EN6_Pos               (6U)
#define CORE2_PIO_EN_EN6_Msk               (0x1UL << CORE2_PIO_EN_EN6_Pos)
#define CORE2_PIO_EN_EN6                   CORE2_PIO_EN_EN6_Msk

/** @brief EN[7]: PIO7 direction control */
#define CORE2_PIO_EN_EN7_Pos               (7U)
#define CORE2_PIO_EN_EN7_Msk               (0x1UL << CORE2_PIO_EN_EN7_Pos)
#define CORE2_PIO_EN_EN7                   CORE2_PIO_EN_EN7_Msk

/** @brief EN[8]: PIO8 direction control */
#define CORE2_PIO_EN_EN8_Pos               (8U)
#define CORE2_PIO_EN_EN8_Msk               (0x1UL << CORE2_PIO_EN_EN8_Pos)
#define CORE2_PIO_EN_EN8                   CORE2_PIO_EN_EN8_Msk

/** @brief EN[9]: PIO9 direction control */
#define CORE2_PIO_EN_EN9_Pos               (9U)
#define CORE2_PIO_EN_EN9_Msk               (0x1UL << CORE2_PIO_EN_EN9_Pos)
#define CORE2_PIO_EN_EN9                   CORE2_PIO_EN_EN9_Msk

/** @brief EN[10]: PIO10 direction control */
#define CORE2_PIO_EN_EN10_Pos              (10U)
#define CORE2_PIO_EN_EN10_Msk              (0x1UL << CORE2_PIO_EN_EN10_Pos)
#define CORE2_PIO_EN_EN10                  CORE2_PIO_EN_EN10_Msk

/** @brief EN[11]: PIO11 direction control */
#define CORE2_PIO_EN_EN11_Pos              (11U)
#define CORE2_PIO_EN_EN11_Msk              (0x1UL << CORE2_PIO_EN_EN11_Pos)
#define CORE2_PIO_EN_EN11                  CORE2_PIO_EN_EN11_Msk

/** @brief EN[12]: PIO12 direction control */
#define CORE2_PIO_EN_EN12_Pos              (12U)
#define CORE2_PIO_EN_EN12_Msk              (0x1UL << CORE2_PIO_EN_EN12_Pos)
#define CORE2_PIO_EN_EN12                  CORE2_PIO_EN_EN12_Msk

/** @brief EN[13]: PIO13 direction control */
#define CORE2_PIO_EN_EN13_Pos              (13U)
#define CORE2_PIO_EN_EN13_Msk              (0x1UL << CORE2_PIO_EN_EN13_Pos)
#define CORE2_PIO_EN_EN13                  CORE2_PIO_EN_EN13_Msk

/** @brief EN[14]: PIO14 direction control */
#define CORE2_PIO_EN_EN14_Pos              (14U)
#define CORE2_PIO_EN_EN14_Msk              (0x1UL << CORE2_PIO_EN_EN14_Pos)
#define CORE2_PIO_EN_EN14                  CORE2_PIO_EN_EN14_Msk

/** @brief EN[15]: PIO15 direction control */
#define CORE2_PIO_EN_EN15_Pos              (15U)
#define CORE2_PIO_EN_EN15_Msk              (0x1UL << CORE2_PIO_EN_EN15_Pos)
#define CORE2_PIO_EN_EN15                  CORE2_PIO_EN_EN15_Msk

/****************** Bit definition for CORE2_PIO_OU register ******************/

/** @brief OU[0]: PIO0 output data */
#define CORE2_PIO_OU_OU0_Pos               (0U)
#define CORE2_PIO_OU_OU0_Msk               (0x1UL << CORE2_PIO_OU_OU0_Pos)
#define CORE2_PIO_OU_OU0                   CORE2_PIO_OU_OU0_Msk

/** @brief OU[1]: PIO1 output data */
#define CORE2_PIO_OU_OU1_Pos               (1U)
#define CORE2_PIO_OU_OU1_Msk               (0x1UL << CORE2_PIO_OU_OU1_Pos)
#define CORE2_PIO_OU_OU1                   CORE2_PIO_OU_OU1_Msk

/** @brief OU[2]: PIO2 output data */
#define CORE2_PIO_OU_OU2_Pos               (2U)
#define CORE2_PIO_OU_OU2_Msk               (0x1UL << CORE2_PIO_OU_OU2_Pos)
#define CORE2_PIO_OU_OU2                   CORE2_PIO_OU_OU2_Msk

/** @brief OU[3]: PIO3 output data */
#define CORE2_PIO_OU_OU3_Pos               (3U)
#define CORE2_PIO_OU_OU3_Msk               (0x1UL << CORE2_PIO_OU_OU3_Pos)
#define CORE2_PIO_OU_OU3                   CORE2_PIO_OU_OU3_Msk

/** @brief OU[4]: PIO4 output data */
#define CORE2_PIO_OU_OU4_Pos               (4U)
#define CORE2_PIO_OU_OU4_Msk               (0x1UL << CORE2_PIO_OU_OU4_Pos)
#define CORE2_PIO_OU_OU4                   CORE2_PIO_OU_OU4_Msk

/** @brief OU[5]: PIO5 output data */
#define CORE2_PIO_OU_OU5_Pos               (5U)
#define CORE2_PIO_OU_OU5_Msk               (0x1UL << CORE2_PIO_OU_OU5_Pos)
#define CORE2_PIO_OU_OU5                   CORE2_PIO_OU_OU5_Msk

/** @brief OU[6]: PIO6 output data */
#define CORE2_PIO_OU_OU6_Pos               (6U)
#define CORE2_PIO_OU_OU6_Msk               (0x1UL << CORE2_PIO_OU_OU6_Pos)
#define CORE2_PIO_OU_OU6                   CORE2_PIO_OU_OU6_Msk

/** @brief OU[7]: PIO7 output data */
#define CORE2_PIO_OU_OU7_Pos               (7U)
#define CORE2_PIO_OU_OU7_Msk               (0x1UL << CORE2_PIO_OU_OU7_Pos)
#define CORE2_PIO_OU_OU7                   CORE2_PIO_OU_OU7_Msk

/** @brief OU[8]: PIO8 output data */
#define CORE2_PIO_OU_OU8_Pos               (8U)
#define CORE2_PIO_OU_OU8_Msk               (0x1UL << CORE2_PIO_OU_OU8_Pos)
#define CORE2_PIO_OU_OU8                   CORE2_PIO_OU_OU8_Msk

/** @brief OU[9]: PIO9 output data */
#define CORE2_PIO_OU_OU9_Pos               (9U)
#define CORE2_PIO_OU_OU9_Msk               (0x1UL << CORE2_PIO_OU_OU9_Pos)
#define CORE2_PIO_OU_OU9                   CORE2_PIO_OU_OU9_Msk

/** @brief OU[10]: PIO10 output data */
#define CORE2_PIO_OU_OU10_Pos              (10U)
#define CORE2_PIO_OU_OU10_Msk              (0x1UL << CORE2_PIO_OU_OU10_Pos)
#define CORE2_PIO_OU_OU10                  CORE2_PIO_OU_OU10_Msk

/** @brief OU[11]: PIO11 output data */
#define CORE2_PIO_OU_OU11_Pos              (11U)
#define CORE2_PIO_OU_OU11_Msk              (0x1UL << CORE2_PIO_OU_OU11_Pos)
#define CORE2_PIO_OU_OU11                  CORE2_PIO_OU_OU11_Msk

/** @brief OU[12]: PIO12 output data */
#define CORE2_PIO_OU_OU12_Pos              (12U)
#define CORE2_PIO_OU_OU12_Msk              (0x1UL << CORE2_PIO_OU_OU12_Pos)
#define CORE2_PIO_OU_OU12                  CORE2_PIO_OU_OU12_Msk

/** @brief OU[13]: PIO13 output data */
#define CORE2_PIO_OU_OU13_Pos              (13U)
#define CORE2_PIO_OU_OU13_Msk              (0x1UL << CORE2_PIO_OU_OU13_Pos)
#define CORE2_PIO_OU_OU13                  CORE2_PIO_OU_OU13_Msk

/** @brief OU[14]: PIO14 output data */
#define CORE2_PIO_OU_OU14_Pos              (14U)
#define CORE2_PIO_OU_OU14_Msk              (0x1UL << CORE2_PIO_OU_OU14_Pos)
#define CORE2_PIO_OU_OU14                  CORE2_PIO_OU_OU14_Msk

/** @brief OU[15]: PIO15 output data */
#define CORE2_PIO_OU_OU15_Pos              (15U)
#define CORE2_PIO_OU_OU15_Msk              (0x1UL << CORE2_PIO_OU_OU15_Pos)
#define CORE2_PIO_OU_OU15                  CORE2_PIO_OU_OU15_Msk

/******************************************************************************/
/*                         Peripheral Instance Definition                     */
/******************************************************************************/

/******************************** ADC Instances *******************************/
#define IS_ADC_ALL_INSTANCE(INSTANCE)    (((INSTANCE) == ADC0) || \
                                          ((INSTANCE) == ADC1) || \
                                          ((INSTANCE) == ADC2))

/******************************** CAN Instances *******************************/
#define IS_CANFD_ALL_INSTANCE(INSTANCE)  (((INSTANCE) == CANFD0) || \
                                          ((INSTANCE) == CANFD1))

/******************************** DMA Instances *******************************/
#define IS_DMA_ALL_INSTANCE(INSTANCE)    (((INSTANCE) == DMA0) || \
                                          ((INSTANCE) == DMA1) || \
                                          ((INSTANCE) == DMA2))

/******************************* GPIO Instances *******************************/
#define IS_GPIO_ALL_INSTANCE(INSTANCE)   (((INSTANCE) == GPIO0) || \
                                          ((INSTANCE) == GPIO1) || \
                                          ((INSTANCE) == GPIO2))

/******************************** I2C Instances *******************************/
#define IS_I2C_ALL_INSTANCE(INSTANCE)    (((INSTANCE) == I2C0) || \
                                          ((INSTANCE) == I2C1) || \
                                          ((INSTANCE) == I2C2) || \
                                          ((INSTANCE) == I2C3))

/******************************** I2S Instances *******************************/
#define IS_I2S_ALL_INSTANCE(INSTANCE)    (((INSTANCE) == I2S0) || \
                                          ((INSTANCE) == I2S1))

/******************************* PWMA Instances *******************************/
#define IS_PWMA_ALL_INSTANCE(INSTANCE)   (((INSTANCE) == PWMA0) || \
                                          ((INSTANCE) == PWMA1) || \
                                          ((INSTANCE) == PWMA2) || \
                                          ((INSTANCE) == PWMA3))

/******************************* PWMG Instances *******************************/
#define IS_PWMG_ALL_INSTANCE(INSTANCE)  (((INSTANCE) == PWMG0) || \
                                         ((INSTANCE) == PWMG1))

/******************************* QE Instances *********************************/
#define IS_QE_ALL_INSTANCE(INSTANCE)     (((INSTANCE) == QE0) || \
                                          ((INSTANCE) == QE1) || \
                                          ((INSTANCE) == QE2) || \
                                          ((INSTANCE) == QE3))

/******************************* QSPI Instances *******************************/
#define IS_QSPI_ALL_INSTANCE(INSTANCE)   (((INSTANCE) == QSPI0) || \
                                          ((INSTANCE) == QSPI1))

/******************************* SPI Instances ********************************/
#define IS_SPI_ALL_INSTANCE(INSTANCE)    (((INSTANCE) == SPI0) || \
                                          ((INSTANCE) == SPI1) || \
                                          ((INSTANCE) == SPI2) || \
                                          ((INSTANCE) == SPI3))

#define IS_SPI_MASTER_INSTANCE(INSTANCE) (((INSTANCE) == SPI1) || \
                                          ((INSTANCE) == SPI3))

#define IS_SPI_SLAVE_INSTANCE(INSTANCE)  (((INSTANCE) == SPI0) || \
                                          ((INSTANCE) == SPI2))

/******************************* TIM Instances ********************************/
#define IS_TIM_ALL_INSTANCE(INSTANCE)    (((INSTANCE) == TIM0) || \
                                          ((INSTANCE) == TIM1))

/******************************* UART Instances *******************************/
#define IS_UART_ALL_INSTANCE(INSTANCE)   (((INSTANCE) == UART0) || \
                                          ((INSTANCE) == UART1) || \
                                          ((INSTANCE) == UART2) || \
                                          ((INSTANCE) == UART3) || \
                                          ((INSTANCE) == UART4) || \
                                          ((INSTANCE) == UART5) || \
                                          ((INSTANCE) == UART6) || \
                                          ((INSTANCE) == UART7))

/******************************* WDT Instances ********************************/
#define IS_WDT_ALL_INSTANCE(INSTANCE)    (((INSTANCE) == WDT0) || \
                                          ((INSTANCE) == WDT1))

#endif /* __BMCU_U_H */

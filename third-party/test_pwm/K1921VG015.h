/*****************************************************************************
 * @file:    K1921VG015.h
 * @author   NIIET
 * @version: V2.2
 * @date:    15.04.2025 
 * @brief:   K1921VG015 header file
 *****************************************************************************
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, NIIET NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2023 NIIET </center></h2>
 *****************************************************************************
 * FILE K1921VG015.h
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __K1921VG015_H
#define __K1921VG015_H

#define __I     volatile const                /*!< defines 'read only' permissions      */
#define __O     volatile                      /*!< defines 'write only' permissions     */
#define __IO    volatile                      /*!< defines 'read / write' permissions   */

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*                   Plic  Interrupt Number Definition                        */
/******************************************************************************/

typedef enum Plic_IsrVect {
    IsrVect_IRQ_0 = 0x0,
/*-- Device specific Interrupt Numbers ---------------------------------------*/
    IsrVect_IRQ_WDT                       = 1, /* Watchdog timer interrupt */
    IsrVect_IRQ_CAN0                      = 2, /* CAN0 interrupt */
    IsrVect_IRQ_CAN1                      = 3, /* CAN1 interrupt */
    IsrVect_IRQ_USB                       = 4, /* USB interrupt */
    IsrVect_IRQ_GPIO                      = 5, /* GPIO interrupt */
    IsrVect_IRQ_TMR32                     = 6, /* Timer 32bit interrupt */
    IsrVect_IRQ_TMR0                      = 7, /* Timer 0 interrupt */
    IsrVect_IRQ_TMR1                      = 8, /* Timer 1 interrupt */
    IsrVect_IRQ_TMR2                      = 9, /* Timer 2 interrupt */
    IsrVect_IRQ_QSPI                      = 10, /* QSPI interrupt */
    IsrVect_IRQ_SPI0                      = 11, /* SPI0 interrupt */
    IsrVect_IRQ_SPI1                      = 12, /* SPI1 interrupt */
    IsrVect_IRQ_DMA0                      = 13, /* DMA channel 0,1,2 interrupt */
    IsrVect_IRQ_DMA1                      = 14, /* DMA channel 3,4,5 interrupt */
    IsrVect_IRQ_DMA2                      = 15, /* DMA channel 6,7,8 interrupt */
    IsrVect_IRQ_DMA3                      = 16, /* DMA channel 9,10,11 interrupt */
    IsrVect_IRQ_DMA4                      = 17, /* DMA channel 12,13,14 interrupt */
    IsrVect_IRQ_DMA5                      = 18, /* DMA channel 15,16,17 interrupt */
    IsrVect_IRQ_DMA6                      = 19, /* DMA channel 18,19,20 interrupt */
    IsrVect_IRQ_DMA7                      = 20, /* DMA channel 21,22,23 interrupt */
    IsrVect_IRQ_I2C                       = 21, /* I2C interrupt */
    IsrVect_IRQ_UART0                     = 22, /* UART0 interrupt */
    IsrVect_IRQ_UART1                     = 23, /* UART1 interrupt */
    IsrVect_IRQ_UART2                     = 24, /* UART2 interrupt */
    IsrVect_IRQ_UART3                     = 25, /* UART3 interrupt */
    IsrVect_IRQ_UART4                     = 26, /* UART4 interrupt */
    IsrVect_IRQ_CRYPTOHASH                = 27, /* CRYPTO+HASH interrupt */
    IsrVect_IRQ_TRNG                      = 28, /* TRNG interrupt */
    IsrVect_IRQ_ADC                       = 29, /* ADC interrupt */
    IsrVect_IRQ_CMP                       = 30, /* CMP interrupt */
    IsrVect_IRQ_PMURTC                    = 31, /* PMURTC interrupt */
    IsrVect_IRQ_                          = 32, /*  */
} Plic_IsrVect_TypeDef;


//#include <system_K1921VG015.h>        /* System initialization   			      */

/******************************************************************************/
/*                         System Specific Defenitions                        */
/******************************************************************************/
/*--  System memory ----------------------------------------------------------*/
/*--  Flash size: 1Mb, RAM0 size: 256Kb, RAM1(battary domain) size: 64Kb -----*/
#define MEM_FLASH_BASE                       0x80000000UL
#define MEM_FLASH_BUS_WIDTH_WORDS            16UL
#define MEM_FLASH_PAGE_SIZE                  4096UL
#define MEM_FLASH_PAGE_SIZE_LOG2             12UL
#define MEM_FLASH_PAGE_TOTAL                 256UL
#define MEM_FLASH_SIZE                       (MEM_MFLASH_PAGE_TOTAL*MEM_MFLASH_PAGE_SIZE)
#define MEM_RAM0_BASE                         0x40000000UL
#define MEM_RAM0_SIZE                         0x40000UL
#define MEM_RAM1_BASE                         0x10000000UL
#define MEM_RAM2_SIZE                         0x10000UL

/*--  CFGWORD: System configure word -----------------------------------------*/
#define CFGWORD_BASE                          0x00001FF0UL

typedef struct {
    uint32_t JTAGEN               : 1;                              /*!< Enable JTAG pins (default 1 - enabled) */
    uint32_t CFGWE                : 1;                              /*!< CFG (NVR) flash region write enable (default 1 - enabled) */
    uint32_t FLASHWE              : 1;                              /*!< Main flash region write enable (default 1 - enabled) */
} CFGWORD_bits;

/* Bit field positions: */
#define CFGWORD_JTAGEN_Pos                    2                      /*!< Enable JTAG pins (default 1 - enabled) */
#define CFGWORD_CFGWE_Pos                     1                      /*!< CFG (NVR) flash region write enable (default 1 - enabled) */
#define CFGWORD_FLASHWE_Pos                   0                      /*!< Main flash region write enable (default 1 - enabled) */

/* Bit field masks: */
#define CFGWORD_JTAGEN_Msk                    0x00000004UL           /*!< Enable JTAG pins (default 1 - enabled) */
#define CFGWORD_CFGWE_Msk                     0x00000002UL           /*!< CFG (NVR) flash region write enable (default 1 - enabled) */
#define CFGWORD_FLASHWE_Msk                   0x00000001UL           /*!< Main flash region write enable (default 1 - enabled) */

/*--  CHANNEL_CFG: DMA channel configure word --------------------------------*/
typedef struct {
    uint32_t NEXT_USEBURST        : 1;                               /*!< Controls if the DMA->USEBURSTSET bit is set to a 1 */
    uint32_t R_POWER              : 4;                               /*!< Control how many DMA transfers can occur before the controller rearbitrates (2^R_POWER, 1024 max) */
    uint32_t N_MINUS_1            : 10;                              /*!< The total number of DMA transfers that the DMA cycle contains */
    uint32_t SRC_INC              : 2;                               /*!< Source address increment */
    uint32_t SRC_SIZE             : 2;                               /*!< Size of the source data */
    uint32_t DST_INC              : 2;                               /*!< Destination address increment */
    uint32_t DST_SIZE             : 2;                               /*!< Destination data size */  
    uint32_t DST_PROT_PRIV        : 1;                               /*!< Bus protection when the controller writes the destination data: privileged access */
    uint32_t DST_PROT_BUFF        : 1;                               /*!< Bus protection when the controller writes the destination data: bufferable access */
    uint32_t DST_PROT_CACHE       : 1;                               /*!< Bus protection when the controller writes the destination data: cacheable access */
    uint32_t SRC_PROT_PRIV        : 1;                               /*!< Bus protection when the controller reads the source data: privileged access */
    uint32_t SRC_PROT_BUFF        : 1;                               /*!< Bus protection when the controller reads the source data: bufferable access */
    uint32_t SRC_PROT_CACHE       : 1;                               /*!< Bus protection when the controller reads the source data: cacheable access */
    uint32_t CYCLE_CTRL           : 3;                               /*!< The operating mode of the DMA cycle */
} _CHANNEL_CFG_bits;

/* Bit field positions: */
#define DMA_CHANNEL_CFG_NEXT_USEBURST_Pos     0                      /*!< Controls if the DMA->USEBURSTSET bit is set to a 1 */
#define DMA_CHANNEL_CFG_R_POWER_Pos           1                      /*!< Control how many DMA transfers can occur before the controller rearbitrates */
#define DMA_CHANNEL_CFG_N_MINUS_1_Pos         5                      /*!< The total number of DMA transfers that the DMA cycle contains */
#define DMA_CHANNEL_CFG_SRC_INC_Pos           15                     /*!< Source address increment */
#define DMA_CHANNEL_CFG_SRC_SIZE_Pos          17                     /*!< Size of the source data */
#define DMA_CHANNEL_CFG_DST_INC_Pos           19                     /*!< Destination address increment */
#define DMA_CHANNEL_CFG_DST_SIZE_Pos          21                     /*!< Destination data size */
#define DMA_CHANNEL_CFG_DST_PROT_PRIV_Pos     23                     /*!< Bus protection when the controller writes the destination data: privileged access */
#define DMA_CHANNEL_CFG_DST_PROT_BUFF_Pos     24                     /*!< Bus protection when the controller writes the destination data: bufferable access */
#define DMA_CHANNEL_CFG_DST_PROT_CACHE_Pos    25                     /*!< Bus protection when the controller writes the destination data: cacheable access */
#define DMA_CHANNEL_CFG_SRC_PROT_PRIV_Pos     26                     /*!< Bus protection when the controller reads the source data: privileged access */
#define DMA_CHANNEL_CFG_SRC_PROT_BUFF_Pos     27                     /*!< Bus protection when the controller reads the source data: bufferable access */
#define DMA_CHANNEL_CFG_SRC_PROT_CACHE_Pos    28                     /*!< Bus protection when the controller reads the source data: cacheable access */
#define DMA_CHANNEL_CFG_CYCLE_CTRL_Pos        29                     /*!< The operating mode of the DMA cycle */

/* Bit field masks: */
#define DMA_CHANNEL_CFG_NEXT_USEBURST_Msk     0x00000001UL           /*!< Controls if the DMA->USEBURSTSET bit is set to a 1 */
#define DMA_CHANNEL_CFG_R_POWER_Msk           0x0000001EUL           /*!< Control how many DMA transfers can occur before the controller rearbitrates */
#define DMA_CHANNEL_CFG_N_MINUS_1_Msk         0x00007FE0UL           /*!< The total number of DMA transfers that the DMA cycle contains */
#define DMA_CHANNEL_CFG_SRC_INC_Msk           0x00018000UL           /*!< Source address increment */
#define DMA_CHANNEL_CFG_SRC_SIZE_Msk          0x00060000UL           /*!< Size of the source data */
#define DMA_CHANNEL_CFG_DST_INC_Msk           0x00180000UL           /*!< Destination address increment */
#define DMA_CHANNEL_CFG_DST_SIZE_Msk          0x00600000UL           /*!< Destination data size */
#define DMA_CHANNEL_CFG_DST_PROT_PRIV_Msk     0x00800000UL           /*!< Bus protection when the controller writes the destination data: privileged access */
#define DMA_CHANNEL_CFG_DST_PROT_BUFF_Msk     0x01000000UL           /*!< Bus protection when the controller writes the destination data: bufferable access */
#define DMA_CHANNEL_CFG_DST_PROT_CACHE_Msk    0x02000000UL           /*!< Bus protection when the controller writes the destination data: cacheable access */
#define DMA_CHANNEL_CFG_SRC_PROT_PRIV_Msk     0x04000000UL           /*!< Bus protection when the controller reads the source data: privileged access */
#define DMA_CHANNEL_CFG_SRC_PROT_BUFF_Msk     0x08000000UL           /*!< Bus protection when the controller reads the source data: bufferable access */
#define DMA_CHANNEL_CFG_SRC_PROT_CACHE_Msk    0x10000000UL           /*!< Bus protection when the controller reads the source data: cacheable access */
#define DMA_CHANNEL_CFG_CYCLE_CTRL_Msk        0xE0000000UL           /*!< The operating mode of the DMA cycle */

/* Bit field enums: */
typedef enum {
  DMA_CHANNEL_CFG_CYCLE_CTRL_Stop                    = 0x0UL,        /*!< Stop */
  DMA_CHANNEL_CFG_CYCLE_CTRL_Basic                   = 0x1UL,        /*!< Basic */
  DMA_CHANNEL_CFG_CYCLE_CTRL_AutoReq                 = 0x2UL,        /*!< Auto-request */
  DMA_CHANNEL_CFG_CYCLE_CTRL_PingPong                = 0x3UL,        /*!< Ping-pong */
  DMA_CHANNEL_CFG_CYCLE_CTRL_MemScatGathPrim         = 0x4UL,        /*!< Memory scatter-gather for primary structure */
  DMA_CHANNEL_CFG_CYCLE_CTRL_MemScatGathAlt          = 0x5UL,        /*!< Memory scatter-gather for alternative structure */
  DMA_CHANNEL_CFG_CYCLE_CTRL_PeriphScatGathPrim      = 0x6UL,        /*!< Peripheral scatter-gather for primary structure */
  DMA_CHANNEL_CFG_CYCLE_CTRL_PeriphScatGathAlt       = 0x7UL,        /*!< Peripheral scatter-gather for alternative structure */
} DMA_CHANNEL_CFG_CYCLE_CTRL_Enum;

typedef enum {
  DMA_CHANNEL_CFG_SRC_SIZE_Byte                      = 0x0UL,        /*!< 8 bit */
  DMA_CHANNEL_CFG_SRC_SIZE_Halfword                  = 0x1UL,        /*!< 16 bit */
  DMA_CHANNEL_CFG_SRC_SIZE_Word                      = 0x2UL,        /*!< 32 bit */
} DMA_CHANNEL_CFG_SRC_SIZE_Enum;

typedef enum {
  DMA_CHANNEL_CFG_SRC_INC_Byte                       = 0x0UL,        /*!< 8 bit */
  DMA_CHANNEL_CFG_SRC_INC_Halfword                   = 0x1UL,        /*!< 16 bit */
  DMA_CHANNEL_CFG_SRC_INC_Word                       = 0x2UL,        /*!< 32 bit */
  DMA_CHANNEL_CFG_SRC_INC_None                       = 0x3UL,        /*!< No increment */
} DMA_CHANNEL_CFG_SRC_INC_Enum;

typedef enum {
  DMA_CHANNEL_CFG_DST_SIZE_Byte                      = 0x0UL,        /*!< 8 bit */
  DMA_CHANNEL_CFG_DST_SIZE_Halfword                  = 0x1UL,        /*!< 16 bit */
  DMA_CHANNEL_CFG_DST_SIZE_Word                      = 0x2UL,        /*!< 32 bit */
} DMA_CHANNEL_CFG_DST_SIZE_Enum;

typedef enum {
  DMA_CHANNEL_CFG_DST_INC_Byte                       = 0x0UL,        /*!< 8 bit */
  DMA_CHANNEL_CFG_DST_INC_Halfword                   = 0x1UL,        /*!< 16 bit */
  DMA_CHANNEL_CFG_DST_INC_Word                       = 0x2UL,        /*!< 32 bit */
  DMA_CHANNEL_CFG_DST_INC_None                       = 0x3UL,        /*!< No increment */
} DMA_CHANNEL_CFG_DST_INC_Enum;

/*--  DMA channel structure --------------------------------------------------*/
typedef struct
{
    __IO uint32_t SRC_DATA_END_PTR;                                       /*!< Source data end pointer */
    __IO uint32_t DST_DATA_END_PTR;                                       /*!< Destination data end pointer */
    union {
        __IO uint32_t CHANNEL_CFG;                                        /*!< Channel configure word */
        __IO _CHANNEL_CFG_bits CHANNEL_CFG_bit;                           /*!< channel configure word: bit access */
    };
    __IO uint32_t RESERVED;
} DMA_Channel_TypeDef;

/*--  DMA control structure --------------------------------------------------*/
typedef struct
{
    DMA_Channel_TypeDef CH[24];                                      /*!< Control structure channels */
    __IO uint32_t RESERVED[32];
} DMA_CtrlStruct_TypeDef;

#define DMA_CH_GPIOA                          0                      /*!< GPIOA DMA channel */
#define DMA_CH_GPIOB                          1                      /*!< GPIOB DMA channel */
#define DMA_CH_GPIOC                          2                      /*!< GPIOC DMA channel */
#define DMA_CH_TMR32                          4                      /*!< TMR32 DMA channel */
#define DMA_CH_TMR0                           5                      /*!< TMR0 DMA channel */
#define DMA_CH_TMR1                           6                      /*!< TMR1 DMA channel */
#define DMA_CH_TMR2                           7                      /*!< TMR2 DMA channel */
#define DMA_CH_UART0TX                        8                      /*!< UART0 TX DMA channel */
#define DMA_CH_UART1TX                        9                      /*!< UART1 TX DMA channel */
#define DMA_CH_UART2TX                        10                     /*!< UART2 TX DMA channel */
#define DMA_CH_UART0RX                        11                     /*!< UART0 RX DMA channel */
#define DMA_CH_UART1RX                        12                     /*!< UART1 RX DMA channel */
#define DMA_CH_UART2RX                        13                     /*!< UART2 RX DMA channel */
#define DMA_CH_ADCSARSEQ0                     14                     /*!< ADCSAR sequencer 0 DMA channel */
#define DMA_CH_ADCSARSEQ1                     15                     /*!< ADCSAR sequencer 1 DMA channel */
#define DMA_CH_SPI0TX                         17                     /*!< SPI0 TX DMA channel */
#define DMA_CH_SPI1TX                         18                     /*!< SPI1 TX DMA channel */
#define DMA_CH_SPI0RX                         20                     /*!< SPI0 RX DMA channel */
#define DMA_CH_SPI1RX                         21                     /*!< SPI1 RX DMA channel */
#define DMA_CH_HASH                           22                     /*!< HASH DMA channel */

/*--  DMA control data summary -----------------------------------------------*/
/*!< WARNING: struct should be 1024 byte aligned! Allowed addresses 0xXXXXX000, 0xXXXXX400, 0xXXXXX800, etc */
typedef struct
{
    DMA_CtrlStruct_TypeDef PRM_DATA;                               /*!< Primary control structure */
    DMA_CtrlStruct_TypeDef ALT_DATA;                               /*!< Alternative control structure */
} DMA_CtrlData_TypeDef;


/******************************************************************************/
/*                         RCU registers                                      */
/******************************************************************************/

/*--  CGCFGAHB: Clockgate AHB configuration register ----------------------------------------------------------*/
typedef struct {
  uint32_t CANEN                  :1;                                /*!< Enable clock CAN */
  uint32_t USBEN                  :1;                                /*!< Enable clock USB */
  uint32_t CRYPTOEN               :1;                                /*!< Enable clock CRYPTO */
  uint32_t HASHEN                 :1;                                /*!< Enable clock HASH */
  uint32_t QSPIEN                 :1;                                /*!< Enable clock QSPI */
  uint32_t SPI0EN                 :1;                                /*!< Enable clock SPI0 */
  uint32_t SPI1EN                 :1;                                /*!< Enable clock SPI1 */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t GPIOAEN                :1;                                /*!< Enable clock GPIOA */
  uint32_t GPIOBEN                :1;                                /*!< Enable clock GPIOB */
  uint32_t GPIOCEN                :1;                                /*!< Enable clock GPIOC */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t CRC0EN                 :1;                                /*!< Enable clock CRC0 */
  uint32_t CRC1EN                 :1;                                /*!< Enable clock CRC1 */
} _RCU_CGCFGAHB_bits;

/* Bit field positions: */
#define RCU_CGCFGAHB_CANEN_Pos                0                      /*!< Enable clock CAN */
#define RCU_CGCFGAHB_USBEN_Pos                1                      /*!< Enable clock USB */
#define RCU_CGCFGAHB_CRYPTOEN_Pos             2                      /*!< Enable clock CRYPTO */
#define RCU_CGCFGAHB_HASHEN_Pos               3                      /*!< Enable clock HASH */
#define RCU_CGCFGAHB_QSPIEN_Pos               4                      /*!< Enable clock QSPI */
#define RCU_CGCFGAHB_SPI0EN_Pos               5                      /*!< Enable clock SPI0 */
#define RCU_CGCFGAHB_SPI1EN_Pos               6                      /*!< Enable clock SPI1 */
#define RCU_CGCFGAHB_GPIOAEN_Pos              8                      /*!< Enable clock GPIOA */
#define RCU_CGCFGAHB_GPIOBEN_Pos              9                      /*!< Enable clock GPIOB */
#define RCU_CGCFGAHB_GPIOCEN_Pos              10                     /*!< Enable clock GPIOC */
#define RCU_CGCFGAHB_CRC0EN_Pos               12                     /*!< Enable clock CRC0 */
#define RCU_CGCFGAHB_CRC1EN_Pos               13                     /*!< Enable clock CRC1 */

/* Bit field masks: */
#define RCU_CGCFGAHB_CANEN_Msk                0x00000001UL           /*!< Enable clock CAN */
#define RCU_CGCFGAHB_USBEN_Msk                0x00000002UL           /*!< Enable clock USB */
#define RCU_CGCFGAHB_CRYPTOEN_Msk             0x00000004UL           /*!< Enable clock CRYPTO */
#define RCU_CGCFGAHB_HASHEN_Msk               0x00000008UL           /*!< Enable clock HASH */
#define RCU_CGCFGAHB_QSPIEN_Msk               0x00000010UL           /*!< Enable clock QSPI */
#define RCU_CGCFGAHB_SPI0EN_Msk               0x00000020UL           /*!< Enable clock SPI0 */
#define RCU_CGCFGAHB_SPI1EN_Msk               0x00000040UL           /*!< Enable clock SPI1 */
#define RCU_CGCFGAHB_GPIOAEN_Msk              0x00000100UL           /*!< Enable clock GPIOA */
#define RCU_CGCFGAHB_GPIOBEN_Msk              0x00000200UL           /*!< Enable clock GPIOB */
#define RCU_CGCFGAHB_GPIOCEN_Msk              0x00000400UL           /*!< Enable clock GPIOC */
#define RCU_CGCFGAHB_CRC0EN_Msk               0x00001000UL           /*!< Enable clock CRC0 */
#define RCU_CGCFGAHB_CRC1EN_Msk               0x00002000UL           /*!< Enable clock CRC1 */

/*--  CGCFGAPB: Clockgate APB configuration register ----------------------------------------------------------*/
typedef struct {
  uint32_t TMR32EN                :1;                                /*!< Enable clock TMR32 */
  uint32_t TMR0EN                 :1;                                /*!< Enable clock TMR0 */
  uint32_t TMR1EN                 :1;                                /*!< Enable clock TMR1 */
  uint32_t TMR2EN                 :1;                                /*!< Enable clock TMR2 */
  uint32_t TRNGEN                 :1;                                /*!< Enable clock TRNG */
  uint32_t I2CEN                  :1;                                /*!< Enable clock I2C */
  uint32_t UART0EN                :1;                                /*!< Enable clock UART0 */
  uint32_t UART1EN                :1;                                /*!< Enable clock UART1 */
  uint32_t UART2EN                :1;                                /*!< Enable clock UART2 */
  uint32_t UART3EN                :1;                                /*!< Enable clock UART3 */
  uint32_t UART4EN                :1;                                /*!< Enable clock UART4 */
  uint32_t WDTEN                  :1;                                /*!< Enable clock WDT */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t ADCSDEN                :1;                                /*!< Enable clock ADCSD */
  uint32_t ADCSAREN               :1;                                /*!< Enable clock ADCSAR */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t CMPEN                  :1;                                /*!< Enable clock CMP */
} _RCU_CGCFGAPB_bits;

/* Bit field positions: */
#define RCU_CGCFGAPB_TMR32EN_Pos              0                      /*!< Enable clock TMR32 */
#define RCU_CGCFGAPB_TMR0EN_Pos               1                      /*!< Enable clock TMR0 */
#define RCU_CGCFGAPB_TMR1EN_Pos               2                      /*!< Enable clock TMR1 */
#define RCU_CGCFGAPB_TMR2EN_Pos               3                      /*!< Enable clock TMR2 */
#define RCU_CGCFGAPB_TRNGEN_Pos               4                      /*!< Enable clock TRNG */
#define RCU_CGCFGAPB_I2CEN_Pos                5                      /*!< Enable clock I2C */
#define RCU_CGCFGAPB_UART0EN_Pos              6                      /*!< Enable clock UART0 */
#define RCU_CGCFGAPB_UART1EN_Pos              7                      /*!< Enable clock UART1 */
#define RCU_CGCFGAPB_UART2EN_Pos              8                      /*!< Enable clock UART2 */
#define RCU_CGCFGAPB_UART3EN_Pos              9                      /*!< Enable clock UART3 */
#define RCU_CGCFGAPB_UART4EN_Pos              10                     /*!< Enable clock UART4 */
#define RCU_CGCFGAPB_WDTEN_Pos                11                     /*!< Enable clock WDT */
#define RCU_CGCFGAPB_ADCSDEN_Pos              15                     /*!< Enable clock ADCSD */
#define RCU_CGCFGAPB_ADCSAREN_Pos             16                     /*!< Enable clock ADCSAR */
#define RCU_CGCFGAPB_CMPEN_Pos                18                     /*!< Enable clock CMP */

/* Bit field masks: */
#define RCU_CGCFGAPB_TMR32EN_Msk              0x00000001UL           /*!< Enable clock TMR32 */
#define RCU_CGCFGAPB_TMR0EN_Msk               0x00000002UL           /*!< Enable clock TMR0 */
#define RCU_CGCFGAPB_TMR1EN_Msk               0x00000004UL           /*!< Enable clock TMR1 */
#define RCU_CGCFGAPB_TMR2EN_Msk               0x00000008UL           /*!< Enable clock TMR2 */
#define RCU_CGCFGAPB_TRNGEN_Msk               0x00000010UL           /*!< Enable clock TRNG */
#define RCU_CGCFGAPB_I2CEN_Msk                0x00000020UL           /*!< Enable clock I2C */
#define RCU_CGCFGAPB_UART0EN_Msk              0x00000040UL           /*!< Enable clock UART0 */
#define RCU_CGCFGAPB_UART1EN_Msk              0x00000080UL           /*!< Enable clock UART1 */
#define RCU_CGCFGAPB_UART2EN_Msk              0x00000100UL           /*!< Enable clock UART2 */
#define RCU_CGCFGAPB_UART3EN_Msk              0x00000200UL           /*!< Enable clock UART3 */
#define RCU_CGCFGAPB_UART4EN_Msk              0x00000400UL           /*!< Enable clock UART4 */
#define RCU_CGCFGAPB_WDTEN_Msk                0x00000800UL           /*!< Enable clock WDT */
#define RCU_CGCFGAPB_ADCSDEN_Msk              0x00008000UL           /*!< Enable clock ADCSD */
#define RCU_CGCFGAPB_ADCSAREN_Msk             0x00010000UL           /*!< Enable clock ADCSAR */
#define RCU_CGCFGAPB_CMPEN_Msk                0x00040000UL           /*!< Enable clock CMP */

/*--  RSTDISAHB: Reset disable AHB configuration register -----------------------------------------------------*/
typedef struct {
  uint32_t CANEN                  :1;                                /*!< Disable reset from CAN */
  uint32_t USBEN                  :1;                                /*!< Disable reset from USB */
  uint32_t CRYPTOEN               :1;                                /*!< Disable reset from CRYPTO */
  uint32_t HASHEN                 :1;                                /*!< Disable reset from HASH */
  uint32_t QSPIEN                 :1;                                /*!< Disable reset from QSPI */
  uint32_t SPI0EN                 :1;                                /*!< Disable reset from SPI0 */
  uint32_t SPI1EN                 :1;                                /*!< Disable reset from SPI1 */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t GPIOAEN                :1;                                /*!< Disable reset from GPIOA */
  uint32_t GPIOBEN                :1;                                /*!< Disable reset from GPIOB */
  uint32_t GPIOCEN                :1;                                /*!< Disable reset from GPIOC */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t CRC0EN                 :1;                                /*!< Disable reset from CRC0 */
  uint32_t CRC1EN                 :1;                                /*!< Disable reset from CRC1 */
} _RCU_RSTDISAHB_bits;

/* Bit field positions: */
#define RCU_RSTDISAHB_CANEN_Pos               0                      /*!< Disable reset from CAN */
#define RCU_RSTDISAHB_USBEN_Pos               1                      /*!< Disable reset from USB */
#define RCU_RSTDISAHB_CRYPTOEN_Pos            2                      /*!< Disable reset from CRYPTO */
#define RCU_RSTDISAHB_HASHEN_Pos              3                      /*!< Disable reset from HASH */
#define RCU_RSTDISAHB_QSPIEN_Pos              4                      /*!< Disable reset from QSPI */
#define RCU_RSTDISAHB_SPI0EN_Pos              5                      /*!< Disable reset from SPI0 */
#define RCU_RSTDISAHB_SPI1EN_Pos              6                      /*!< Disable reset from SPI1 */
#define RCU_RSTDISAHB_GPIOAEN_Pos             8                      /*!< Disable reset from GPIOA */
#define RCU_RSTDISAHB_GPIOBEN_Pos             9                      /*!< Disable reset from GPIOB */
#define RCU_RSTDISAHB_GPIOCEN_Pos             10                     /*!< Disable reset from GPIOC */
#define RCU_RSTDISAHB_CRC0EN_Pos              12                     /*!< Disable reset from CRC0 */
#define RCU_RSTDISAHB_CRC1EN_Pos              13                     /*!< Disable reset from CRC1 */

/* Bit field masks: */
#define RCU_RSTDISAHB_CANEN_Msk               0x00000001UL           /*!< Disable reset from CAN */
#define RCU_RSTDISAHB_USBEN_Msk               0x00000002UL           /*!< Disable reset from USB */
#define RCU_RSTDISAHB_CRYPTOEN_Msk            0x00000004UL           /*!< Disable reset from CRYPTO */
#define RCU_RSTDISAHB_HASHEN_Msk              0x00000008UL           /*!< Disable reset from HASH */
#define RCU_RSTDISAHB_QSPIEN_Msk              0x00000010UL           /*!< Disable reset from QSPI */
#define RCU_RSTDISAHB_SPI0EN_Msk              0x00000020UL           /*!< Disable reset from SPI0 */
#define RCU_RSTDISAHB_SPI1EN_Msk              0x00000040UL           /*!< Disable reset from SPI1 */
#define RCU_RSTDISAHB_GPIOAEN_Msk             0x00000100UL           /*!< Disable reset from GPIOA */
#define RCU_RSTDISAHB_GPIOBEN_Msk             0x00000200UL           /*!< Disable reset from GPIOB */
#define RCU_RSTDISAHB_GPIOCEN_Msk             0x00000400UL           /*!< Disable reset from GPIOC */
#define RCU_RSTDISAHB_CRC0EN_Msk              0x00001000UL           /*!< Disable reset from CRC0 */
#define RCU_RSTDISAHB_CRC1EN_Msk              0x00002000UL           /*!< Disable reset from CRC1 */

/*--  RSTDISAPB: Reset disable APB register -------------------------------------------------------------------*/
typedef struct {
  uint32_t TMR32EN                :1;                                /*!< Disable reset from TMR32 */
  uint32_t TMR0EN                 :1;                                /*!< Disable reset from TMR0 */
  uint32_t TMR1EN                 :1;                                /*!< Disable reset from TMR1 */
  uint32_t TMR2EN                 :1;                                /*!< Disable reset from TMR2 */
  uint32_t TRNGEN                 :1;                                /*!< Disable reset from TRNG */
  uint32_t I2CEN                  :1;                                /*!< Disable reset from I2C */
  uint32_t UART0EN                :1;                                /*!< Disable reset from UART0 */
  uint32_t UART1EN                :1;                                /*!< Disable reset from UART1 */
  uint32_t UART2EN                :1;                                /*!< Disable reset from UART2 */
  uint32_t UART3EN                :1;                                /*!< Disable reset from UART3 */
  uint32_t UART4EN                :1;                                /*!< Disable reset from UART4 */
  uint32_t WDTEN                  :1;                                /*!< Disable reset from WDT */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t ADCSDEN                :1;                                /*!< Disable reset from ADCSD */
  uint32_t ADCSAREN               :1;                                /*!< Disable reset from ADCSAR */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t CMPEN                  :1;                                /*!< Disable reset from CMP */
} _RCU_RSTDISAPB_bits;

/* Bit field positions: */
#define RCU_RSTDISAPB_TMR32EN_Pos             0                      /*!< Disable reset from TMR32 */
#define RCU_RSTDISAPB_TMR0EN_Pos              1                      /*!< Disable reset from TMR0 */
#define RCU_RSTDISAPB_TMR1EN_Pos              2                      /*!< Disable reset from TMR1 */
#define RCU_RSTDISAPB_TMR2EN_Pos              3                      /*!< Disable reset from TMR2 */
#define RCU_RSTDISAPB_TRNGEN_Pos              4                      /*!< Disable reset from TRNG */
#define RCU_RSTDISAPB_I2CEN_Pos               5                      /*!< Disable reset from I2C */
#define RCU_RSTDISAPB_UART0EN_Pos             6                      /*!< Disable reset from UART0 */
#define RCU_RSTDISAPB_UART1EN_Pos             7                      /*!< Disable reset from UART1 */
#define RCU_RSTDISAPB_UART2EN_Pos             8                      /*!< Disable reset from UART2 */
#define RCU_RSTDISAPB_UART3EN_Pos             9                      /*!< Disable reset from UART3 */
#define RCU_RSTDISAPB_UART4EN_Pos             10                     /*!< Disable reset from UART4 */
#define RCU_RSTDISAPB_WDTEN_Pos               11                     /*!< Disable reset from WDT */
#define RCU_RSTDISAPB_ADCSDEN_Pos             15                     /*!< Disable reset from ADCSD */
#define RCU_RSTDISAPB_ADCSAREN_Pos            16                     /*!< Disable reset from ADCSAR */
#define RCU_RSTDISAPB_CMPEN_Pos               18                     /*!< Disable reset from CMP */

/* Bit field masks: */
#define RCU_RSTDISAPB_TMR32EN_Msk             0x00000001UL           /*!< Disable reset from TMR32 */
#define RCU_RSTDISAPB_TMR0EN_Msk              0x00000002UL           /*!< Disable reset from TMR0 */
#define RCU_RSTDISAPB_TMR1EN_Msk              0x00000004UL           /*!< Disable reset from TMR1 */
#define RCU_RSTDISAPB_TMR2EN_Msk              0x00000008UL           /*!< Disable reset from TMR2 */
#define RCU_RSTDISAPB_TRNGEN_Msk              0x00000010UL           /*!< Disable reset from TRNG */
#define RCU_RSTDISAPB_I2CEN_Msk               0x00000020UL           /*!< Disable reset from I2C */
#define RCU_RSTDISAPB_UART0EN_Msk             0x00000040UL           /*!< Disable reset from UART0 */
#define RCU_RSTDISAPB_UART1EN_Msk             0x00000080UL           /*!< Disable reset from UART1 */
#define RCU_RSTDISAPB_UART2EN_Msk             0x00000100UL           /*!< Disable reset from UART2 */
#define RCU_RSTDISAPB_UART3EN_Msk             0x00000200UL           /*!< Disable reset from UART3 */
#define RCU_RSTDISAPB_UART4EN_Msk             0x00000400UL           /*!< Disable reset from UART4 */
#define RCU_RSTDISAPB_WDTEN_Msk               0x00000800UL           /*!< Disable reset from WDT */
#define RCU_RSTDISAPB_ADCSDEN_Msk             0x00008000UL           /*!< Disable reset from ADCSD */
#define RCU_RSTDISAPB_ADCSAREN_Msk            0x00010000UL           /*!< Disable reset from ADCSAR */
#define RCU_RSTDISAPB_CMPEN_Msk               0x00040000UL           /*!< Disable reset from CMP */

/*--  RSTSTAT: Reset Status Register --------------------------------------------------------------------------*/
typedef struct {
  uint32_t PORVBAT                :1;                                /*!< PowerOn Reset status */
  uint32_t POR                    :1;                                /*!< WatchDog Reset status */
  uint32_t WDOG                   :1;                                /*!< System Reset Status */
  uint32_t IWDOG                  :1;                                /*!< Lockup Reset Status */
  uint32_t SYSRST                 :1;                                /*!<  */
} _RCU_RSTSTAT_bits;

/* Bit field positions: */
#define RCU_RSTSTAT_PORVBAT_Pos               0                      /*!< PowerOn Reset status */
#define RCU_RSTSTAT_POR_Pos                   1                      /*!< WatchDog Reset status */
#define RCU_RSTSTAT_WDOG_Pos                  2                      /*!< System Reset Status */
#define RCU_RSTSTAT_IWDOG_Pos                 3                      /*!< Lockup Reset Status */
#define RCU_RSTSTAT_SYSRST_Pos                4                      /*!<  */

/* Bit field masks: */
#define RCU_RSTSTAT_PORVBAT_Msk               0x00000001UL           /*!< PowerOn Reset status */
#define RCU_RSTSTAT_POR_Msk                   0x00000002UL           /*!< WatchDog Reset status */
#define RCU_RSTSTAT_WDOG_Msk                  0x00000004UL           /*!< System Reset Status */
#define RCU_RSTSTAT_IWDOG_Msk                 0x00000008UL           /*!< Lockup Reset Status */
#define RCU_RSTSTAT_SYSRST_Msk                0x00000010UL           /*!<  */

/*--  SYSCLKCFG: System clock configuration register ----------------------------------------------------------*/
typedef struct {
  uint32_t SRC                    :2;                                /*!< System clock source */
  uint32_t                        :14;                               /*!< RESERVED */
  uint32_t SECEN                  :1;                                /*!<  */
} _RCU_SYSCLKCFG_bits;

/* Bit field positions: */
#define RCU_SYSCLKCFG_SRC_Pos                 0                      /*!< System clock source */
#define RCU_SYSCLKCFG_SECEN_Pos               16                     /*!<  */

/* Bit field masks: */
#define RCU_SYSCLKCFG_SRC_Msk                 0x00000003UL           /*!< System clock source */
#define RCU_SYSCLKCFG_SECEN_Msk               0x00010000UL           /*!<  */

/* Bit field enums: */
typedef enum {
  RCU_SYSCLKCFG_SRC_HSICLK                   = 0x0UL,                /*!< HSI 1MHz if CPE=0 or HSE(XTALL) if CPE=1 */
  RCU_SYSCLKCFG_SRC_HSECLK                   = 0x1UL,                /*!< HSE(XTALL) if CPE=0 or HSI 1MHz if CPE=1 */
  RCU_SYSCLKCFG_SRC_SYSPLL0CLK               = 0x2UL,                /*!< Clock from PLL OUT[0] */
  RCU_SYSCLKCFG_SRC_LSICLK                   = 0x3UL,                /*!< LSI (32768 Hz) from RTC */
} RCU_SYSCLKCFG_SRC_Enum;

/*--  SECCNT0: Security counter configuration 0 register ------------------------------------------------------*/
typedef struct {
  uint32_t VAL0                   :16;                               /*!<  */
  uint32_t VAL1                   :16;                               /*!<  */
} _RCU_SECCNT0_bits;

/* Bit field positions: */
#define RCU_SECCNT0_VAL0_Pos                  0                      /*!<  */
#define RCU_SECCNT0_VAL1_Pos                  16                     /*!<  */

/* Bit field masks: */
#define RCU_SECCNT0_VAL0_Msk                  0x0000FFFFUL           /*!<  */
#define RCU_SECCNT0_VAL1_Msk                  0xFFFF0000UL           /*!<  */

/*--  SECCNT1: Security counter configuration 1 register ------------------------------------------------------*/
typedef struct {
  uint32_t VAL2                   :16;                               /*!<  */
  uint32_t VAL3                   :16;                               /*!<  */
} _RCU_SECCNT1_bits;

/* Bit field positions: */
#define RCU_SECCNT1_VAL2_Pos                  0                      /*!<  */
#define RCU_SECCNT1_VAL3_Pos                  16                     /*!<  */

/* Bit field masks: */
#define RCU_SECCNT1_VAL2_Msk                  0x0000FFFFUL           /*!<  */
#define RCU_SECCNT1_VAL3_Msk                  0xFFFF0000UL           /*!<  */

/*--  CLKSTAT: System clock status register -------------------------------------------------------------------*/
typedef struct {
  uint32_t SRC                    :2;                                /*!< REFCLK:0:HSI 1MHz if CPE=0 or HSE(XTALL) if CPE=1;SRCCLK:1:HSE(XTALL) if CPE=0 or HSI 1MHz if CPE=1;SYSPLL0CLK:2: Clock from PLL OUT[0];LSICLK:3: LSI (32768 Hz) from RTC; */
  uint32_t                        :6;                                /*!< RESERVED */
  uint32_t CLKGOOD0               :1;                                /*!<  */
  uint32_t CLKGOOD1               :1;                                /*!<  */
  uint32_t CLKGOOD2               :1;                                /*!<  */
  uint32_t CLKGOOD3               :1;                                /*!<  */
  uint32_t                        :4;                                /*!< RESERVED */
  uint32_t CLKERR0                :1;                                /*!<  */
  uint32_t CLKERR1                :1;                                /*!<  */
  uint32_t CLKERR2                :1;                                /*!<  */
  uint32_t CLKERR3                :1;                                /*!<  */
} _RCU_CLKSTAT_bits;

/* Bit field positions: */
#define RCU_CLKSTAT_SRC_Pos                   0                      /*!< REFCLK:0:HSI 1MHz if CPE=0 or HSE(XTALL) if CPE=1;SRCCLK:1:HSE(XTALL) if CPE=0 or HSI 1MHz if CPE=1;SYSPLL0CLK:2: Clock from PLL OUT[0];LSICLK:3: LSI (32768 Hz) from RTC; */
#define RCU_CLKSTAT_CLKGOOD0_Pos              8                      /*!<  */
#define RCU_CLKSTAT_CLKGOOD1_Pos              9                      /*!<  */
#define RCU_CLKSTAT_CLKGOOD2_Pos              10                     /*!<  */
#define RCU_CLKSTAT_CLKGOOD3_Pos              11                     /*!<  */
#define RCU_CLKSTAT_CLKERR0_Pos               16                     /*!<  */
#define RCU_CLKSTAT_CLKERR1_Pos               17                     /*!<  */
#define RCU_CLKSTAT_CLKERR2_Pos               18                     /*!<  */
#define RCU_CLKSTAT_CLKERR3_Pos               19                     /*!<  */

/* Bit field masks: */
#define RCU_CLKSTAT_SRC_Msk                   0x00000003UL           /*!< REFCLK:0:HSI 1MHz if CPE=0 or HSE(XTALL) if CPE=1;SRCCLK:1:HSE(XTALL) if CPE=0 or HSI 1MHz if CPE=1;SYSPLL0CLK:2: Clock from PLL OUT[0];LSICLK:3: LSI (32768 Hz) from RTC; */
#define RCU_CLKSTAT_CLKGOOD0_Msk              0x00000100UL           /*!<  */
#define RCU_CLKSTAT_CLKGOOD1_Msk              0x00000200UL           /*!<  */
#define RCU_CLKSTAT_CLKGOOD2_Msk              0x00000400UL           /*!<  */
#define RCU_CLKSTAT_CLKGOOD3_Msk              0x00000800UL           /*!<  */
#define RCU_CLKSTAT_CLKERR0_Msk               0x00010000UL           /*!<  */
#define RCU_CLKSTAT_CLKERR1_Msk               0x00020000UL           /*!<  */
#define RCU_CLKSTAT_CLKERR2_Msk               0x00040000UL           /*!<  */
#define RCU_CLKSTAT_CLKERR3_Msk               0x00080000UL           /*!<  */

/* Bit field enums: */
typedef enum {
  RCU_CLKSTAT_SRC_HSICLK                   = 0x0UL,                /*!< HSI 1MHz if CPE=0 or HSE(XTALL) if CPE=1 */
  RCU_CLKSTAT_SRC_HSECLK                   = 0x1UL,                /*!< HSE(XTALL) if CPE=0 or HSI 1MHz if CPE=1 */
  RCU_CLKSTAT_SRC_SYSPLL0CLK               = 0x2UL,                /*!< Clock from PLL OUT[0] */
  RCU_CLKSTAT_SRC_LSICLK                   = 0x3UL,                /*!< LSI (32768 Hz) from RTC */
} RCU_CLKSTAT_SRC_Enum;


/*--  PLLSYSCFG0: PLL configudration 0 register ---------------------------------------------------------------*/
typedef struct {
  uint32_t PLLEN                  :1;                                /*!< PLL enable */
  uint32_t BYP                    :2;                                /*!< Bypass */
  uint32_t DACEN                  :1;                                /*!<  */
  uint32_t DSMEN                  :1;                                /*!<  */
  uint32_t FOUTEN                 :2;                                /*!< Output frequency enabled */
  uint32_t REFDIV                 :6;                                /*!<  */
  uint32_t PD0A                   :3;                                /*!<  */
  uint32_t PD0B                   :6;                                /*!<  */
  uint32_t PD1A                   :3;                                /*!<  */
  uint32_t PD1B                   :6;                                /*!<  */
} _RCU_PLLSYSCFG0_bits;

/* Bit field positions: */
#define RCU_PLLSYSCFG0_PLLEN_Pos              0                      /*!< PLL enable */
#define RCU_PLLSYSCFG0_BYP_Pos                1                      /*!< Bypass */
#define RCU_PLLSYSCFG0_DACEN_Pos              3                      /*!<  */
#define RCU_PLLSYSCFG0_DSMEN_Pos              4                      /*!<  */
#define RCU_PLLSYSCFG0_FOUTEN_Pos             5                      /*!< Output frequency enabled */
#define RCU_PLLSYSCFG0_REFDIV_Pos             7                      /*!<  */
#define RCU_PLLSYSCFG0_PD0A_Pos               13                     /*!<  */
#define RCU_PLLSYSCFG0_PD0B_Pos               16                     /*!<  */
#define RCU_PLLSYSCFG0_PD1A_Pos               22                     /*!<  */
#define RCU_PLLSYSCFG0_PD1B_Pos               25                     /*!<  */

/* Bit field masks: */
#define RCU_PLLSYSCFG0_PLLEN_Msk              0x00000001UL           /*!< PLL enable */
#define RCU_PLLSYSCFG0_BYP_Msk                0x00000006UL           /*!< Bypass */
#define RCU_PLLSYSCFG0_DACEN_Msk              0x00000008UL           /*!<  */
#define RCU_PLLSYSCFG0_DSMEN_Msk              0x00000010UL           /*!<  */
#define RCU_PLLSYSCFG0_FOUTEN_Msk             0x00000060UL           /*!< Output frequency enabled */
#define RCU_PLLSYSCFG0_REFDIV_Msk             0x00001F80UL           /*!<  */
#define RCU_PLLSYSCFG0_PD0A_Msk               0x0000E000UL           /*!<  */
#define RCU_PLLSYSCFG0_PD0B_Msk               0x003F0000UL           /*!<  */
#define RCU_PLLSYSCFG0_PD1A_Msk               0x01C00000UL           /*!<  */
#define RCU_PLLSYSCFG0_PD1B_Msk               0x7E000000UL           /*!<  */

/*--  PLLSYSCFG1: PLL configudration 1 register ---------------------------------------------------------------*/
typedef struct {
  uint32_t FRAC                   :24;                               /*!<  */
} _RCU_PLLSYSCFG1_bits;

/* Bit field positions: */
#define RCU_PLLSYSCFG1_FRAC_Pos               0                      /*!<  */

/* Bit field masks: */
#define RCU_PLLSYSCFG1_FRAC_Msk               0x00FFFFFFUL           /*!<  */

/*--  PLLSYSCFG2: PLL configudration 2 register ---------------------------------------------------------------*/
typedef struct {
  uint32_t FBDIV                  :12;                               /*!<  */
} _RCU_PLLSYSCFG2_bits;

/* Bit field positions: */
#define RCU_PLLSYSCFG2_FBDIV_Pos              0                      /*!<  */

/* Bit field masks: */
#define RCU_PLLSYSCFG2_FBDIV_Msk              0x00000FFFUL           /*!<  */

/*--  PLLSYSCFG3: PLL configudration 3 register ---------------------------------------------------------------*/
typedef struct {
  uint32_t DSKEWEN                :1;                                /*!< Bit must be 0 */
  uint32_t DSKEWCALBYP            :1;                                /*!<  */
  uint32_t DSKEWCALCNT            :3;                                /*!<  */
  uint32_t DSKEWCALEN             :1;                                /*!<  */
  uint32_t DSKEWFASTCAL           :1;                                /*!<  */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t DSKEWCALIN             :12;                               /*!<  */
  uint32_t                        :4;                                /*!< RESERVED */
  uint32_t REFSEL                 :1;                                /*!< 0-RefClk; 1- SrcClk */
} _RCU_PLLSYSCFG3_bits;

/* Bit field positions: */
#define RCU_PLLSYSCFG3_DSKEWEN_Pos            0                      /*!< Bit must be 0 */
#define RCU_PLLSYSCFG3_DSKEWCALBYP_Pos        1                      /*!<  */
#define RCU_PLLSYSCFG3_DSKEWCALCNT_Pos        2                      /*!<  */
#define RCU_PLLSYSCFG3_DSKEWCALEN_Pos         5                      /*!<  */
#define RCU_PLLSYSCFG3_DSKEWFASTCAL_Pos       6                      /*!<  */
#define RCU_PLLSYSCFG3_DSKEWCALIN_Pos         8                      /*!<  */
#define RCU_PLLSYSCFG3_REFSEL_Pos             24                     /*!< 0-RefClk; 1- SrcClk */

/* Bit field masks: */
#define RCU_PLLSYSCFG3_DSKEWEN_Msk            0x00000001UL           /*!< Bit must be 0 */
#define RCU_PLLSYSCFG3_DSKEWCALBYP_Msk        0x00000002UL           /*!<  */
#define RCU_PLLSYSCFG3_DSKEWCALCNT_Msk        0x0000001CUL           /*!<  */
#define RCU_PLLSYSCFG3_DSKEWCALEN_Msk         0x00000020UL           /*!<  */
#define RCU_PLLSYSCFG3_DSKEWFASTCAL_Msk       0x00000040UL           /*!<  */
#define RCU_PLLSYSCFG3_DSKEWCALIN_Msk         0x000FFF00UL           /*!<  */
#define RCU_PLLSYSCFG3_REFSEL_Msk             0x01000000UL           /*!< 0-RefClk; 1- SrcClk */

/*--  PLLSYSSTAT: PLL Status Register -------------------------------------------------------------------------*/
typedef struct {
  uint32_t LOCK                   :1;                                /*!< Lock status PLL */
} _RCU_PLLSYSSTAT_bits;

/* Bit field positions: */
#define RCU_PLLSYSSTAT_LOCK_Pos               0                      /*!< Lock status PLL */

/* Bit field masks: */
#define RCU_PLLSYSSTAT_LOCK_Msk               0x00000001UL           /*!< Lock status PLL */

/*--  UARTCLKCFG: UART Clock and reset configuration register -------------------------------------------------*/
typedef struct {
  uint32_t CLKEN                  :1;                                /*!< Clock enable */
  uint32_t                        :7;                                /*!< RESERVED */
  uint32_t RSTDIS                 :1;                                /*!< Reset disable */
  uint32_t                        :7;                                /*!< RESERVED */
  uint32_t CLKSEL                 :2;                                /*!< Clock select */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t DIVEN                  :1;                                /*!< Divider enabled */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t DIVN                   :6;                                /*!< Divider coefficient */
} _RCU_UARTCLKCFG_bits;

/* Bit field positions: */
#define RCU_UARTCLKCFG_CLKEN_Pos              0                      /*!< Clock enable */
#define RCU_UARTCLKCFG_RSTDIS_Pos             8                      /*!< Reset disable */
#define RCU_UARTCLKCFG_CLKSEL_Pos             16                     /*!< Clock select */
#define RCU_UARTCLKCFG_DIVEN_Pos              20                     /*!< Divider enabled */
#define RCU_UARTCLKCFG_DIVN_Pos               24                     /*!< Divider coefficient */

/* Bit field masks: */
#define RCU_UARTCLKCFG_CLKEN_Msk              0x00000001UL           /*!< Clock enable */
#define RCU_UARTCLKCFG_RSTDIS_Msk             0x00000100UL           /*!< Reset disable */
#define RCU_UARTCLKCFG_CLKSEL_Msk             0x00030000UL           /*!< Clock select */
#define RCU_UARTCLKCFG_DIVEN_Msk              0x00100000UL           /*!< Divider enabled */
#define RCU_UARTCLKCFG_DIVN_Msk               0x3F000000UL           /*!< Divider coefficient */

/* Bit field enums: */
typedef enum {
  RCU_UARTCLKCFG_CLKSEL_HSI                  = 0x0UL,                /*!< HSI 1MHz if CPE=0 or HSE(XTALL) if CPE=1 */
  RCU_UARTCLKCFG_CLKSEL_HSE                  = 0x1UL,                /*!< HSE(XTALL) if CPE=0 or HSI 1MHz if CPE=1 */
  RCU_UARTCLKCFG_CLKSEL_PLL0                 = 0x2UL,                /*!< Clock from PLL OUT[0] */
  RCU_UARTCLKCFG_CLKSEL_PLL1                 = 0x3UL,                /*!< Clock from PLL OUT[1] */
} RCU_UARTCLKCFG_CLKSEL_Enum;

/*--  SPICLKCFG: SPI Clock and Reset Configuration Register ---------------------------------------------------*/
typedef struct {
  uint32_t CLKEN                  :1;                                /*!< Clock enable */
  uint32_t                        :7;                                /*!< RESERVED */
  uint32_t RSTDIS                 :1;                                /*!< Reset disable */
  uint32_t                        :7;                                /*!< RESERVED */
  uint32_t CLKSEL                 :2;                                /*!< Clock select */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t DIVEN                  :1;                                /*!< Divider enabled */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t DIVN                   :6;                                /*!< Divider coefficient */
} _RCU_SPICLKCFG_bits;

/* Bit field positions: */
#define RCU_SPICLKCFG_CLKEN_Pos               0                      /*!< Clock enable */
#define RCU_SPICLKCFG_RSTDIS_Pos              8                      /*!< Reset disable */
#define RCU_SPICLKCFG_CLKSEL_Pos              16                     /*!< Clock select */
#define RCU_SPICLKCFG_DIVEN_Pos               20                     /*!< Divider enabled */
#define RCU_SPICLKCFG_DIVN_Pos                24                     /*!< Divider coefficient */

/* Bit field masks: */
#define RCU_SPICLKCFG_CLKEN_Msk               0x00000001UL           /*!< Clock enable */
#define RCU_SPICLKCFG_RSTDIS_Msk              0x00000100UL           /*!< Reset disable */
#define RCU_SPICLKCFG_CLKSEL_Msk              0x00030000UL           /*!< Clock select */
#define RCU_SPICLKCFG_DIVEN_Msk               0x00100000UL           /*!< Divider enabled */
#define RCU_SPICLKCFG_DIVN_Msk                0x3F000000UL           /*!< Divider coefficient */

/* Bit field enums: */
typedef enum {
  RCU_SPICLKCFG_CLKSEL_HSI                   = 0x0UL,                /*!< HSI 1MHz if CPE=0 or HSE(XTALL) if CPE=1 */
  RCU_SPICLKCFG_CLKSEL_HSE                   = 0x1UL,                /*!< HSE(XTALL) if CPE=0 or HSI 1MHz if CPE=1 */
  RCU_SPICLKCFG_CLKSEL_PLL0                  = 0x2UL,                /*!< Clock from PLL OUT[0] */
  RCU_SPICLKCFG_CLKSEL_PLL1                  = 0x3UL,                /*!< Clock from PLL OUT[1] */
} RCU_SPICLKCFG_CLKSEL_Enum;

/*--  ADCSARCLKCFG: ADC SAR Configuraton Register -------------------------------------------------------------*/
typedef struct {
  uint32_t CLKEN                  :1;                                /*!< Clock enable */
  uint32_t                        :7;                                /*!< RESERVED */
  uint32_t RSTDIS                 :1;                                /*!< Reset disable */
  uint32_t                        :7;                                /*!< RESERVED */
  uint32_t CLKSEL                 :2;                                /*!< Clock select */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t DIVEN                  :1;                                /*!< Divider enabled */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t DIVN                   :6;                                /*!< Divider coefficient */
} _RCU_ADCSARCLKCFG_bits;

/* Bit field positions: */
#define RCU_ADCSARCLKCFG_CLKEN_Pos            0                      /*!< Clock enable */
#define RCU_ADCSARCLKCFG_RSTDIS_Pos           8                      /*!< Reset disable */
#define RCU_ADCSARCLKCFG_CLKSEL_Pos           16                     /*!< Clock select */
#define RCU_ADCSARCLKCFG_DIVEN_Pos            20                     /*!< Divider enabled */
#define RCU_ADCSARCLKCFG_DIVN_Pos             24                     /*!< Divider coefficient */

/* Bit field masks: */
#define RCU_ADCSARCLKCFG_CLKEN_Msk            0x00000001UL           /*!< Clock enable */
#define RCU_ADCSARCLKCFG_RSTDIS_Msk           0x00000100UL           /*!< Reset disable */
#define RCU_ADCSARCLKCFG_CLKSEL_Msk           0x00030000UL           /*!< Clock select */
#define RCU_ADCSARCLKCFG_DIVEN_Msk            0x00100000UL           /*!< Divider enabled */
#define RCU_ADCSARCLKCFG_DIVN_Msk             0x3F000000UL           /*!< Divider coefficient */

/* Bit field enums: */
typedef enum {
  RCU_ADCSARCLKCFG_CLKSEL_HSI                = 0x0UL,                /*!< HSI 1MHz if CPE=0 or HSE(XTALL) if CPE=1 */
  RCU_ADCSARCLKCFG_CLKSEL_HSE                = 0x1UL,                /*!< HSE(XTALL) if CPE=0 or HSI 1MHz if CPE=1 */
  RCU_ADCSARCLKCFG_CLKSEL_PLL0               = 0x2UL,                /*!< Clock from PLL OUT[0] */
  RCU_ADCSARCLKCFG_CLKSEL_PLL1               = 0x3UL,                /*!< Clock from PLL OUT[1] */
} RCU_ADCSARCLKCFG_CLKSEL_Enum;

/*--  ADCSDCLKCFG: ADC Sigma Delta Configuraton Register ------------------------------------------------------*/
typedef struct {
  uint32_t CLKEN                  :1;                                /*!< Clock enable */
  uint32_t                        :7;                                /*!< RESERVED */
  uint32_t RSTDIS                 :1;                                /*!< Reset disable */
  uint32_t                        :7;                                /*!< RESERVED */
  uint32_t CLKSEL                 :2;                                /*!< Clock select */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t DIVEN                  :1;                                /*!< Divider enabled */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t DIVN                   :6;                                /*!< Divider coefficient */
} _RCU_ADCSDCLKCFG_bits;

/* Bit field positions: */
#define RCU_ADCSDCLKCFG_CLKEN_Pos             0                      /*!< Clock enable */
#define RCU_ADCSDCLKCFG_RSTDIS_Pos            8                      /*!< Reset disable */
#define RCU_ADCSDCLKCFG_CLKSEL_Pos            16                     /*!< Clock select */
#define RCU_ADCSDCLKCFG_DIVEN_Pos             20                     /*!< Divider enabled */
#define RCU_ADCSDCLKCFG_DIVN_Pos              24                     /*!< Divider coefficient */

/* Bit field masks: */
#define RCU_ADCSDCLKCFG_CLKEN_Msk             0x00000001UL           /*!< Clock enable */
#define RCU_ADCSDCLKCFG_RSTDIS_Msk            0x00000100UL           /*!< Reset disable */
#define RCU_ADCSDCLKCFG_CLKSEL_Msk            0x00030000UL           /*!< Clock select */
#define RCU_ADCSDCLKCFG_DIVEN_Msk             0x00100000UL           /*!< Divider enabled */
#define RCU_ADCSDCLKCFG_DIVN_Msk              0x3F000000UL           /*!< Divider coefficient */

/* Bit field enums: */
typedef enum {
  RCU_ADCSDCLKCFG_CLKSEL_HSI                 = 0x0UL,                /*!< HSI 1MHz if CPE=0 or HSE(XTALL) if CPE=1 */
  RCU_ADCSDCLKCFG_CLKSEL_HSE                 = 0x1UL,                /*!< HSE(XTALL) if CPE=0 or HSI 1MHz if CPE=1 */
  RCU_ADCSDCLKCFG_CLKSEL_PLL0                = 0x2UL,                /*!< Clock from PLL OUT[0] */
  RCU_ADCSDCLKCFG_CLKSEL_PLL1                = 0x3UL,                /*!< Clock from PLL OUT[1] */
} RCU_ADCSDCLKCFG_CLKSEL_Enum;

/*--  WDOGCLKCFG: Watchdog clock configuration register -------------------------------------------------------*/
typedef struct {
  uint32_t CLKEN                  :1;                                /*!< Clock enable */
  uint32_t                        :7;                                /*!< RESERVED */
  uint32_t RSTDIS                 :1;                                /*!< Reset disable */
  uint32_t                        :7;                                /*!< RESERVED */
  uint32_t CLKSEL                 :2;                                /*!< Clock select */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t DIVEN                  :1;                                /*!< Divider enabled */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t DIVN                   :6;                                /*!< Divider coefficient */
} _RCU_WDOGCLKCFG_bits;

/* Bit field positions: */
#define RCU_WDOGCLKCFG_CLKEN_Pos              0                      /*!< Clock enable */
#define RCU_WDOGCLKCFG_RSTDIS_Pos             8                      /*!< Reset disable */
#define RCU_WDOGCLKCFG_CLKSEL_Pos             16                     /*!< Clock select */
#define RCU_WDOGCLKCFG_DIVEN_Pos              20                     /*!< Divider enabled */
#define RCU_WDOGCLKCFG_DIVN_Pos               24                     /*!< Divider coefficient */

/* Bit field masks: */
#define RCU_WDOGCLKCFG_CLKEN_Msk              0x00000001UL           /*!< Clock enable */
#define RCU_WDOGCLKCFG_RSTDIS_Msk             0x00000100UL           /*!< Reset disable */
#define RCU_WDOGCLKCFG_CLKSEL_Msk             0x00030000UL           /*!< Clock select */
#define RCU_WDOGCLKCFG_DIVEN_Msk              0x00100000UL           /*!< Divider enabled */
#define RCU_WDOGCLKCFG_DIVN_Msk               0x3F000000UL           /*!< Divider coefficient */

/* Bit field enums: */
typedef enum {
  RCU_WDOGCLKCFG_CLKSEL_HSI                  = 0x0UL,                /*!< HSI 1MHz if CPE=0 or HSE(XTALL) if CPE=1 */
  RCU_WDOGCLKCFG_CLKSEL_HSE                  = 0x1UL,                /*!< HSE(XTALL) if CPE=0 or HSI 1MHz if CPE=1 */
  RCU_WDOGCLKCFG_CLKSEL_PLL0                 = 0x2UL,                /*!< Clock from PLL OUT[0] */
  RCU_WDOGCLKCFG_CLKSEL_PLL1                 = 0x3UL,                /*!< Clock from PLL OUT[1] */
} RCU_WDOGCLKCFG_CLKSEL_Enum;

/*--  CLKOUTCFG: Clockout configuration register --------------------------------------------------------------*/
typedef struct {
  uint32_t CLKEN                  :1;                                /*!< Clock enable */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t CLKSEL                 :2;                                /*!< Clock select */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t RSTDIS                 :1;                                /*!< Reset disable */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t DIVEN                  :1;                                /*!< Divider enabled */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t DIVN                   :16;                               /*!< Divider coefficient */
} _RCU_CLKOUTCFG_bits;

/* Bit field positions: */
#define RCU_CLKOUTCFG_CLKEN_Pos               0                      /*!< Clock enable */
#define RCU_CLKOUTCFG_CLKSEL_Pos              4                      /*!< Clock select */
#define RCU_CLKOUTCFG_RSTDIS_Pos              8                      /*!< Reset disable */
#define RCU_CLKOUTCFG_DIVEN_Pos               12                     /*!< Divider enabled */
#define RCU_CLKOUTCFG_DIVN_Pos                16                     /*!< Divider coefficient */

/* Bit field masks: */
#define RCU_CLKOUTCFG_CLKEN_Msk               0x00000001UL           /*!< Clock enable */
#define RCU_CLKOUTCFG_CLKSEL_Msk              0x00000030UL           /*!< Clock select */
#define RCU_CLKOUTCFG_RSTDIS_Msk              0x00000100UL           /*!< Reset disable */
#define RCU_CLKOUTCFG_DIVEN_Msk               0x00001000UL           /*!< Divider enabled */
#define RCU_CLKOUTCFG_DIVN_Msk                0xFFFF0000UL           /*!< Divider coefficient */

/* Bit field enums: */
typedef enum {
  RCU_CLKOUTCFG_CLKSEL_HSI                   = 0x0UL,                /*!< HSI 1MHz if CPE=0 or HSE(XTALL) if CPE=1 */
  RCU_CLKOUTCFG_CLKSEL_HSE                   = 0x1UL,                /*!< HSE(XTALL) if CPE=0 or HSI 1MHz if CPE=1 */
  RCU_CLKOUTCFG_CLKSEL_PLL0                  = 0x2UL,                /*!< Clock from PLL OUT[0] */
  RCU_CLKOUTCFG_CLKSEL_LSI                   = 0x3UL,                /*!< Clock from RTC */
} RCU_CLKOUTCFG_CLKSEL_Enum;

/*--  RSTSYS: System Reset Register ---------------------------------------------------------------------------*/
typedef struct {
  uint32_t RSTEN                  :1;                                /*!< Reset enable */
  uint32_t                        :15;                               /*!< RESERVED */
  uint32_t KEY                    :16;                               /*!< KEY 'A55A' in hexadecimal for unblocking */
} _RCU_RSTSYS_bits;

/* Bit field positions: */
#define RCU_RSTSYS_RSTEN_Pos                  0                      /*!< Reset enable */
#define RCU_RSTSYS_KEY_Pos                    16                     /*!< KEY 'A55A' in hexadecimal for unblocking */

/* Bit field masks: */
#define RCU_RSTSYS_RSTEN_Msk                  0x00000001UL           /*!< Reset enable */
#define RCU_RSTSYS_KEY_Msk                    0xFFFF0000UL           /*!< KEY 'A55A' in hexadecimal for unblocking */

//Cluster UARTCLKCFG:
typedef struct {
  union { 
  /*!< UART Clock and reset configuration register */
    __IO uint32_t UARTCLKCFG;                                           /*!< UARTCLKCFG : type used for word access */
    __IO _RCU_UARTCLKCFG_bits UARTCLKCFG_bit;                        /*!< UARTCLKCFG_bit: structure used for bit access */
  };
} _RCU_UARTCLKCFG_TypeDef;
//Cluster SPICLKCFG:
typedef struct {
  union { 
  /*!< SPI Clock and Reset Configuration Register */
    __IO uint32_t SPICLKCFG;                                           /*!< SPICLKCFG : type used for word access */
    __IO _RCU_SPICLKCFG_bits SPICLKCFG_bit;                          /*!< SPICLKCFG_bit: structure used for bit access */
  };
} _RCU_SPICLKCFG_TypeDef;
typedef struct {
  union {                                                               /*!< Clockgate AHB configuration register */
    __IO uint32_t CGCFGAHB;                                           /*!< CGCFGAHB    : type used for word access */
    __IO _RCU_CGCFGAHB_bits  CGCFGAHB_bit;                           /*!< CGCFGAHB_bit: structure used for bit access */
  };
    __IO uint32_t Reserved0;
  union {                                                               /*!< Clockgate APB configuration register */
    __IO uint32_t CGCFGAPB;                                           /*!< CGCFGAPB    : type used for word access */
    __IO _RCU_CGCFGAPB_bits  CGCFGAPB_bit;                           /*!< CGCFGAPB_bit: structure used for bit access */
  };
    __IO uint32_t Reserved1;
  union {                                                               /*!< Reset disable AHB configuration register */
    __IO uint32_t RSTDISAHB;                                           /*!< RSTDISAHB    : type used for word access */
    __IO _RCU_RSTDISAHB_bits  RSTDISAHB_bit;                         /*!< RSTDISAHB_bit: structure used for bit access */
  };
    __IO uint32_t Reserved2;
  union {                                                               /*!< Reset disable APB register */
    __IO uint32_t RSTDISAPB;                                           /*!< RSTDISAPB    : type used for word access */
    __IO _RCU_RSTDISAPB_bits  RSTDISAPB_bit;                         /*!< RSTDISAPB_bit: structure used for bit access */
  };
    __IO uint32_t Reserved3;
  union {                                                               /*!< Reset Status Register */
    __IO uint32_t RSTSTAT;                                           /*!< RSTSTAT    : type used for word access */
    __IO _RCU_RSTSTAT_bits  RSTSTAT_bit;                             /*!< RSTSTAT_bit: structure used for bit access */
  };
    __IO uint32_t Reserved4[3];
  union {                                                               /*!< System clock configuration register */
    __IO uint32_t SYSCLKCFG;                                           /*!< SYSCLKCFG    : type used for word access */
    __IO _RCU_SYSCLKCFG_bits  SYSCLKCFG_bit;                         /*!< SYSCLKCFG_bit: structure used for bit access */
  };
  union {                                                               /*!< Security counter configuration 0 register */
    __IO uint32_t SECCNT0;                                           /*!< SECCNT0    : type used for word access */
    __IO _RCU_SECCNT0_bits  SECCNT0_bit;                             /*!< SECCNT0_bit: structure used for bit access */
  };
  union {                                                               /*!< Security counter configuration 1 register */
    __IO uint32_t SECCNT1;                                           /*!< SECCNT1    : type used for word access */
    __IO _RCU_SECCNT1_bits  SECCNT1_bit;                             /*!< SECCNT1_bit: structure used for bit access */
  };
  union {                                                               /*!< System clock status register */
    __I uint32_t CLKSTAT;                                            /*!< CLKSTAT    : type used for word access */
    __I _RCU_CLKSTAT_bits  CLKSTAT_bit;                              /*!< CLKSTAT_bit: structure used for bit access */
  };
    __IO uint32_t Reserved5[4];
  union {                                                               /*!< PLL configudration 0 register */
    __IO uint32_t PLLSYSCFG0;                                           /*!< PLLSYSCFG0    : type used for word access */
    __IO _RCU_PLLSYSCFG0_bits  PLLSYSCFG0_bit;                       /*!< PLLSYSCFG0_bit: structure used for bit access */
  };
  union {                                                               /*!< PLL configudration 1 register */
    __IO uint32_t PLLSYSCFG1;                                           /*!< PLLSYSCFG1    : type used for word access */
    __IO _RCU_PLLSYSCFG1_bits  PLLSYSCFG1_bit;                       /*!< PLLSYSCFG1_bit: structure used for bit access */
  };
  union {                                                               /*!< PLL configudration 2 register */
    __IO uint32_t PLLSYSCFG2;                                           /*!< PLLSYSCFG2    : type used for word access */
    __IO _RCU_PLLSYSCFG2_bits  PLLSYSCFG2_bit;                       /*!< PLLSYSCFG2_bit: structure used for bit access */
  };
  union {                                                               /*!< PLL configudration 3 register */
    __IO uint32_t PLLSYSCFG3;                                           /*!< PLLSYSCFG3    : type used for word access */
    __IO _RCU_PLLSYSCFG3_bits  PLLSYSCFG3_bit;                       /*!< PLLSYSCFG3_bit: structure used for bit access */
  };
  union {                                                               /*!< PLL Status Register */
    __I uint32_t PLLSYSSTAT;                                           /*!< PLLSYSSTAT    : type used for word access */
    __I _RCU_PLLSYSSTAT_bits  PLLSYSSTAT_bit;                        /*!< PLLSYSSTAT_bit: structure used for bit access */
  };
    __IO uint32_t Reserved6[3];
  _RCU_UARTCLKCFG_TypeDef UARTCLKCFG[5];
    __IO uint32_t Reserved7[3];
  _RCU_SPICLKCFG_TypeDef QSPICLKCFG;    
  _RCU_SPICLKCFG_TypeDef SPICLKCFG[2];
    __IO uint32_t Reserved8[5];
  union {                                                               /*!< ADC SAR Configuraton Register */
    __IO uint32_t ADCSARCLKCFG;                                           /*!< ADCSARCLKCFG    : type used for word access */
    __IO _RCU_ADCSARCLKCFG_bits  ADCSARCLKCFG_bit;                   /*!< ADCSARCLKCFG_bit: structure used for bit access */
  };
  union {                                                               /*!< ADC Sigma Delta Configuraton Register */
    __IO uint32_t ADCSDCLKCFG;                                           /*!< ADCSDCLKCFG    : type used for word access */
    __IO _RCU_ADCSDCLKCFG_bits  ADCSDCLKCFG_bit;                     /*!< ADCSDCLKCFG_bit: structure used for bit access */
  };
  union {                                                               /*!< Watchdog clock configuration register */
    __IO uint32_t WDOGCLKCFG;                                           /*!< WDOGCLKCFG    : type used for word access */
    __IO _RCU_WDOGCLKCFG_bits  WDOGCLKCFG_bit;                       /*!< WDOGCLKCFG_bit: structure used for bit access */
  };
  union {                                                               /*!< Clockout configuration register */
    __IO uint32_t CLKOUTCFG;                                           /*!< CLKOUTCFG    : type used for word access */
    __IO _RCU_CLKOUTCFG_bits  CLKOUTCFG_bit;                         /*!< CLKOUTCFG_bit: structure used for bit access */
  };
  union {                                                               /*!< System Reset Register */
    __O uint32_t RSTSYS;                                             /*!< RSTSYS    : type used for word access */
    __O _RCU_RSTSYS_bits  RSTSYS_bit;                                /*!< RSTSYS_bit: structure used for bit access */
  };
} RCU_TypeDef;


/******************************************************************************/
/*                         PMUSYS registers                                   */
/******************************************************************************/

/*--  PDEN: PMU Core Periph powerdown register ----------------------------------------------------------------*/
typedef struct {
  uint32_t PLLEN                  :1;                                /*!< PLL powerdown after WFI command */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t FLASHEN                :1;                                /*!< FLASH powerdown after WFI command */
} _PMUSYS_PDEN_bits;

/* Bit field positions: */
#define PMUSYS_PDEN_PLLEN_Pos                 0                      /*!< PLL powerdown after WFI command */
#define PMUSYS_PDEN_FLASHEN_Pos               3                      /*!< FLASH powerdown after WFI command */

/* Bit field masks: */
#define PMUSYS_PDEN_PLLEN_Msk                 0x00000001UL           /*!< PLL powerdown after WFI command */
#define PMUSYS_PDEN_FLASHEN_Msk               0x00000008UL           /*!< FLASH powerdown after WFI command */

/*--  PDENFORCE: PMU Core Periph powerdown Force register -----------------------------------------------------*/
typedef struct {
  uint32_t PLLEN                  :1;                                /*!< PLL powerdown after write this bit */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t FLASHEN                :1;                                /*!< FLASH powerdown after write this bit */
} _PMUSYS_PDENFORCE_bits;

/* Bit field positions: */
#define PMUSYS_PDENFORCE_PLLEN_Pos            0                      /*!< PLL powerdown after write this bit */
#define PMUSYS_PDENFORCE_FLASHEN_Pos          3                      /*!< FLASH powerdown after write this bit */

/* Bit field masks: */
#define PMUSYS_PDENFORCE_PLLEN_Msk            0x00000001UL           /*!< PLL powerdown after write this bit */
#define PMUSYS_PDENFORCE_FLASHEN_Msk          0x00000008UL           /*!< FLASH powerdown after write this bit */

/*--  ADCPWRCFG: SAR ADC Powerdown configuration register -----------------------------------------------------*/
typedef struct {
  uint32_t LDOEN                  :1;                                /*!< Enable ADC module Low Drop Out Regulator (LDO) */
  uint32_t LVLDIS                 :1;                                /*!< Disable ADC level-shifters to go into deep LP mode */
} _PMUSYS_ADCPWRCFG_bits;

/* Bit field positions: */
#define PMUSYS_ADCPWRCFG_LDOEN_Pos            0                      /*!< Enable ADC module Low Drop Out Regulator (LDO) */
#define PMUSYS_ADCPWRCFG_LVLDIS_Pos           1                      /*!< Disable ADC level-shifters to go into deep LP mode */

/* Bit field masks: */
#define PMUSYS_ADCPWRCFG_LDOEN_Msk            0x00000001UL           /*!< Enable ADC module Low Drop Out Regulator (LDO) */
#define PMUSYS_ADCPWRCFG_LVLDIS_Msk           0x00000002UL           /*!< Disable ADC level-shifters to go into deep LP mode */

/*--  CHIPID: Chip identifier register ------------------------------------------------------------------------*/
typedef struct {
  uint32_t REV                    :4;                                /*!< Revision */
  uint32_t ID                     :28;                               /*!< Chip identifier */
} _PMUSYS_CHIPID_bits;

/* Bit field positions: */
#define PMUSYS_CHIPID_REV_Pos                 0                      /*!< Revision */
#define PMUSYS_CHIPID_ID_Pos                  4                      /*!< Chip identifier */

/* Bit field masks: */
#define PMUSYS_CHIPID_REV_Msk                 0x0000000FUL           /*!< Revision */
#define PMUSYS_CHIPID_ID_Msk                  0xFFFFFFF0UL           /*!< Chip identifier */

/*--  SERVCTL: Service mode control register ------------------------------------------------------------------*/
typedef struct {
  uint32_t SERVEN                 :1;                                /*!< SERV_EN_STAT */
  uint32_t                        :7;                                /*!< RESERVED */
  uint32_t DONE                   :1;                                /*!< WR_serv_all_erase_req__RD_serv_done */
} _PMUSYS_SERVCTL_bits;

/* Bit field positions: */
#define PMUSYS_SERVCTL_SERVEN_Pos             0                      /*!< SERV_EN_STAT */
#define PMUSYS_SERVCTL_DONE_Pos               8                      /*!< WR_serv_all_erase_req__RD_serv_done */

/* Bit field masks: */
#define PMUSYS_SERVCTL_SERVEN_Msk             0x00000001UL           /*!< SERV_EN_STAT */
#define PMUSYS_SERVCTL_DONE_Msk               0x00000100UL           /*!< WR_serv_all_erase_req__RD_serv_done */

/*--  UID: Unique ID register 128 bit -------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!<  */
} _PMUSYS_UID_bits;

/* Bit field positions: */
#define PMUSYS_UID_VAL_Pos                    0                      /*!<  */

/* Bit field masks: */
#define PMUSYS_UID_VAL_Msk                    0xFFFFFFFFUL           /*!<  */

//Cluster UID:
typedef struct {
  union { 
  /*!< Unique ID register 128 bit */
    __I uint32_t UID;                                                /*!< UID : type used for word access */
    __I _PMUSYS_UID_bits UID_bit;                                    /*!< UID_bit: structure used for bit access */
  };
} _PMUSYS_UID_TypeDef;
typedef struct {
  union {                                                               /*!< PMU Core Periph powerdown register */
    __IO uint32_t PDEN;                                              /*!< PDEN    : type used for word access */
    __IO _PMUSYS_PDEN_bits  PDEN_bit;                                /*!< PDEN_bit: structure used for bit access */
  };
  union {                                                               /*!< PMU Core Periph powerdown Force register */
    __IO uint32_t PDENFORCE;                                           /*!< PDENFORCE    : type used for word access */
    __IO _PMUSYS_PDENFORCE_bits  PDENFORCE_bit;                      /*!< PDENFORCE_bit: structure used for bit access */
  };
    __IO uint32_t Reserved0[2];
  union {                                                               /*!< SAR ADC Powerdown configuration register */
    __IO uint32_t ADCPWRCFG;                                           /*!< ADCPWRCFG    : type used for word access */
    __IO _PMUSYS_ADCPWRCFG_bits  ADCPWRCFG_bit;                      /*!< ADCPWRCFG_bit: structure used for bit access */
  };
    __IO uint32_t Reserved1[59];
  union {                                                               /*!< Chip identifier register */
    __I uint32_t CHIPID;                                             /*!< CHIPID    : type used for word access */
    __I _PMUSYS_CHIPID_bits  CHIPID_bit;                             /*!< CHIPID_bit: structure used for bit access */
  };
  union {                                                               /*!< Service mode control register */
    __IO uint32_t SERVCTL;                                           /*!< SERVCTL    : type used for word access */
    __IO _PMUSYS_SERVCTL_bits  SERVCTL_bit;                          /*!< SERVCTL_bit: structure used for bit access */
  };
    __IO uint32_t Reserved2[2];
    __I uint32_t UID[4];
} PMUSYS_TypeDef;


/******************************************************************************/
/*                         IWDT registers                                     */
/******************************************************************************/

/*--  LOAD: Watchdog Load Register ----------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Begin value counter */
} _IWDT_LOAD_bits;

/* Bit field positions: */
#define IWDT_LOAD_VAL_Pos                     0                      /*!< Begin value counter */

/* Bit field masks: */
#define IWDT_LOAD_VAL_Msk                     0xFFFFFFFFUL           /*!< Begin value counter */

/*--  VALUE: Watchdog Value Register --------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Current value counter */
} _IWDT_VALUE_bits;

/* Bit field positions: */
#define IWDT_VALUE_VAL_Pos                    0                      /*!< Current value counter */

/* Bit field masks: */
#define IWDT_VALUE_VAL_Msk                    0xFFFFFFFFUL           /*!< Current value counter */

/*--  CTRL: Watchdog Control Register -------------------------------------------------------------------------*/
typedef struct {
  uint32_t INTEN                  :1;                                /*!< Enable the interrupt event */
  uint32_t RESEN                  :1;                                /*!< Enable watchdog reset output */
  uint32_t BLK                    :1;                                /*!< Blocking configuration */
} _IWDT_CTRL_bits;

/* Bit field positions: */
#define IWDT_CTRL_INTEN_Pos                   0                      /*!< Enable the interrupt event */
#define IWDT_CTRL_RESEN_Pos                   1                      /*!< Enable watchdog reset output */
#define IWDT_CTRL_BLK_Pos                     2                      /*!< Blocking configuration */

/* Bit field masks: */
#define IWDT_CTRL_INTEN_Msk                   0x00000001UL           /*!< Enable the interrupt event */
#define IWDT_CTRL_RESEN_Msk                   0x00000002UL           /*!< Enable watchdog reset output */
#define IWDT_CTRL_BLK_Msk                     0x00000004UL           /*!< Blocking configuration */

/*--  INTCLR: Watchdog Clear Interrupt Register ---------------------------------------------------------------*/
typedef struct {
  uint32_t WDTCLR                 :32;                               /*!< Reset interrupt WDT */
} _IWDT_INTCLR_bits;

/* Bit field positions: */
#define IWDT_INTCLR_WDTCLR_Pos                0                      /*!< Reset interrupt WDT */

/* Bit field masks: */
#define IWDT_INTCLR_WDTCLR_Msk                0xFFFFFFFFUL           /*!< Reset interrupt WDT */

/*--  RIS: Watchdog Raw Interrupt Status Register -------------------------------------------------------------*/
typedef struct {
  uint32_t RAWWDTINT              :1;                                /*!< Raw interrupt status from the counter */
} _IWDT_RIS_bits;

/* Bit field positions: */
#define IWDT_RIS_RAWWDTINT_Pos                0                      /*!< Raw interrupt status from the counter */

/* Bit field masks: */
#define IWDT_RIS_RAWWDTINT_Msk                0x00000001UL           /*!< Raw interrupt status from the counter */

/*--  MIS: Watchdog Interrupt Status Register -----------------------------------------------------------------*/
typedef struct {
  uint32_t WDTINT                 :1;                                /*!< Enabled interrupt status from the counter */
} _IWDT_MIS_bits;

/* Bit field positions: */
#define IWDT_MIS_WDTINT_Pos                   0                      /*!< Enabled interrupt status from the counter */

/* Bit field masks: */
#define IWDT_MIS_WDTINT_Msk                   0x00000001UL           /*!< Enabled interrupt status from the counter */

/*--  LOCK: Watchdog Lock Register ----------------------------------------------------------------------------*/
typedef struct {
  uint32_t REGWRDIS               :1;                                /*!< Disable write to all registers Watchdog */
} _IWDT_LOCK_bits;

/* Bit field positions: */
#define IWDT_LOCK_REGWRDIS_Pos                0                      /*!< Disable write to all registers Watchdog */

/* Bit field masks: */
#define IWDT_LOCK_REGWRDIS_Msk                0x00000001UL           /*!< Disable write to all registers Watchdog */

typedef struct {
  union {                                                               /*!< Watchdog Load Register */
    __IO uint32_t LOAD;                                              /*!< LOAD    : type used for word access */
    __IO _IWDT_LOAD_bits  LOAD_bit;                                  /*!< LOAD_bit: structure used for bit access */
  };
  union {                                                               /*!< Watchdog Value Register */
    __I uint32_t VALUE;                                              /*!< VALUE    : type used for word access */
    __I _IWDT_VALUE_bits  VALUE_bit;                                 /*!< VALUE_bit: structure used for bit access */
  };
  union {                                                               /*!< Watchdog Control Register */
    __IO uint32_t CTRL;                                              /*!< CTRL    : type used for word access */
    __IO _IWDT_CTRL_bits  CTRL_bit;                                  /*!< CTRL_bit: structure used for bit access */
  };
  union {                                                               /*!< Watchdog Clear Interrupt Register */
    __O uint32_t INTCLR;                                             /*!< INTCLR    : type used for word access */
    __O _IWDT_INTCLR_bits  INTCLR_bit;                               /*!< INTCLR_bit: structure used for bit access */
  };
  union {                                                               /*!< Watchdog Raw Interrupt Status Register */
    __I uint32_t RIS;                                                /*!< RIS    : type used for word access */
    __I _IWDT_RIS_bits  RIS_bit;                                     /*!< RIS_bit: structure used for bit access */
  };
  union {                                                               /*!< Watchdog Interrupt Status Register */
    __I uint32_t MIS;                                                /*!< MIS    : type used for word access */
    __I _IWDT_MIS_bits  MIS_bit;                                     /*!< MIS_bit: structure used for bit access */
  };
    __IO uint32_t Reserved0[762];
  union {                                                               /*!< Watchdog Lock Register */
    __O uint32_t LOCK;                                               /*!< LOCK    : type used for word access */
    __O _IWDT_LOCK_bits  LOCK_bit;                                   /*!< LOCK_bit: structure used for bit access */
  };
} IWDT_TypeDef;


/******************************************************************************/
/*                         WDT registers                                      */
/******************************************************************************/

/*--  LOAD: Watchdog Load Register ----------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Begin value counter */
} _WDT_LOAD_bits;

/* Bit field positions: */
#define WDT_LOAD_VAL_Pos                      0                      /*!< Begin value counter */

/* Bit field masks: */
#define WDT_LOAD_VAL_Msk                      0xFFFFFFFFUL           /*!< Begin value counter */

/*--  VALUE: Watchdog Value Register --------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Current value counter */
} _WDT_VALUE_bits;

/* Bit field positions: */
#define WDT_VALUE_VAL_Pos                     0                      /*!< Current value counter */

/* Bit field masks: */
#define WDT_VALUE_VAL_Msk                     0xFFFFFFFFUL           /*!< Current value counter */

/*--  CTRL: Watchdog Control Register -------------------------------------------------------------------------*/
typedef struct {
  uint32_t INTEN                  :1;                                /*!< Enable the interrupt event */
  uint32_t RESEN                  :1;                                /*!< Enable watchdog reset output */
} _WDT_CTRL_bits;

/* Bit field positions: */
#define WDT_CTRL_INTEN_Pos                    0                      /*!< Enable the interrupt event */
#define WDT_CTRL_RESEN_Pos                    1                      /*!< Enable watchdog reset output */

/* Bit field masks: */
#define WDT_CTRL_INTEN_Msk                    0x00000001UL           /*!< Enable the interrupt event */
#define WDT_CTRL_RESEN_Msk                    0x00000002UL           /*!< Enable watchdog reset output */

/*--  INTCLR: Watchdog Clear Interrupt Register ---------------------------------------------------------------*/
typedef struct {
  uint32_t WDTCLR                 :32;                               /*!< Reset interrupt WDT */
} _WDT_INTCLR_bits;

/* Bit field positions: */
#define WDT_INTCLR_WDTCLR_Pos                 0                      /*!< Reset interrupt WDT */

/* Bit field masks: */
#define WDT_INTCLR_WDTCLR_Msk                 0xFFFFFFFFUL           /*!< Reset interrupt WDT */

/*--  RIS: Watchdog Raw Interrupt Status Register -------------------------------------------------------------*/
typedef struct {
  uint32_t RAWWDTINT              :1;                                /*!< Raw interrupt status from the counter */
} _WDT_RIS_bits;

/* Bit field positions: */
#define WDT_RIS_RAWWDTINT_Pos                 0                      /*!< Raw interrupt status from the counter */

/* Bit field masks: */
#define WDT_RIS_RAWWDTINT_Msk                 0x00000001UL           /*!< Raw interrupt status from the counter */

/*--  MIS: Watchdog Interrupt Status Register -----------------------------------------------------------------*/
typedef struct {
  uint32_t WDTINT                 :1;                                /*!< Enabled interrupt status from the counter */
} _WDT_MIS_bits;

/* Bit field positions: */
#define WDT_MIS_WDTINT_Pos                    0                      /*!< Enabled interrupt status from the counter */

/* Bit field masks: */
#define WDT_MIS_WDTINT_Msk                    0x00000001UL           /*!< Enabled interrupt status from the counter */

/*--  LOCK: Watchdog Lock Register ----------------------------------------------------------------------------*/
typedef struct {
  uint32_t REGWRDIS               :1;                                /*!< Disable write to all registers Watchdog */
} _WDT_LOCK_bits;

/* Bit field positions: */
#define WDT_LOCK_REGWRDIS_Pos                 0                      /*!< Disable write to all registers Watchdog */

/* Bit field masks: */
#define WDT_LOCK_REGWRDIS_Msk                 0x00000001UL           /*!< Disable write to all registers Watchdog */

typedef struct {
  union {                                                               /*!< Watchdog Load Register */
    __IO uint32_t LOAD;                                              /*!< LOAD    : type used for word access */
    __IO _WDT_LOAD_bits  LOAD_bit;                                   /*!< LOAD_bit: structure used for bit access */
  };
  union {                                                               /*!< Watchdog Value Register */
    __I uint32_t VALUE;                                              /*!< VALUE    : type used for word access */
    __I _WDT_VALUE_bits  VALUE_bit;                                  /*!< VALUE_bit: structure used for bit access */
  };
  union {                                                               /*!< Watchdog Control Register */
    __IO uint32_t CTRL;                                              /*!< CTRL    : type used for word access */
    __IO _WDT_CTRL_bits  CTRL_bit;                                   /*!< CTRL_bit: structure used for bit access */
  };
  union {                                                               /*!< Watchdog Clear Interrupt Register */
    __O uint32_t INTCLR;                                             /*!< INTCLR    : type used for word access */
    __O _WDT_INTCLR_bits  INTCLR_bit;                                /*!< INTCLR_bit: structure used for bit access */
  };
  union {                                                               /*!< Watchdog Raw Interrupt Status Register */
    __I uint32_t RIS;                                                /*!< RIS    : type used for word access */
    __I _WDT_RIS_bits  RIS_bit;                                      /*!< RIS_bit: structure used for bit access */
  };
  union {                                                               /*!< Watchdog Interrupt Status Register */
    __I uint32_t MIS;                                                /*!< MIS    : type used for word access */
    __I _WDT_MIS_bits  MIS_bit;                                      /*!< MIS_bit: structure used for bit access */
  };
    __IO uint32_t Reserved0[762];
  union {                                                               /*!< Watchdog Lock Register */
    __O uint32_t LOCK;                                               /*!< LOCK    : type used for word access */
    __O _WDT_LOCK_bits  LOCK_bit;                                    /*!< LOCK_bit: structure used for bit access */
  };
} WDT_TypeDef;


/******************************************************************************/
/*                         TMR registers                                      */
/******************************************************************************/

/*--  CTRL: Timer control register ----------------------------------------------------------------------------*/
typedef struct {
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t CLR                    :1;                                /*!< Timer Clear */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t MODE                   :2;                                /*!< Count mode */
  uint32_t DIV                    :2;                                /*!< Divider */
  uint32_t CLKSEL                 :1;                                /*!< Clock Source Select */
} _TMR_CTRL_bits;

/* Bit field positions: */
#define TMR_CTRL_CLR_Pos                      2                      /*!< Timer Clear */
#define TMR_CTRL_MODE_Pos                     4                      /*!< Count mode */
#define TMR_CTRL_DIV_Pos                      6                      /*!< Divider */
#define TMR_CTRL_CLKSEL_Pos                   8                      /*!< Clock Source Select */

/* Bit field masks: */
#define TMR_CTRL_CLR_Msk                      0x00000004UL           /*!< Timer Clear */
#define TMR_CTRL_MODE_Msk                     0x00000030UL           /*!< Count mode */
#define TMR_CTRL_DIV_Msk                      0x000000C0UL           /*!< Divider */
#define TMR_CTRL_CLKSEL_Msk                   0x00000100UL           /*!< Clock Source Select */

/* Bit field enums: */
typedef enum {
  TMR_CTRL_MODE_Stop                         = 0x0UL,                /*!< Timer is Stopped */
  TMR_CTRL_MODE_Up                           = 0x1UL,                /*!< Timer count Up */
  TMR_CTRL_MODE_Multiple                     = 0x2UL,                /*!< Timer count is multiple Up */
  TMR_CTRL_MODE_UpDown                       = 0x3UL,                /*!< Timer count is multiple Up - Down */
} TMR_CTRL_MODE_Enum;

typedef enum {
  TMR_CTRL_DIV_Div1                          = 0x0UL,                /*!< Clock not divided */
  TMR_CTRL_DIV_Div2                          = 0x1UL,                /*!< Clock divided by 2 */
  TMR_CTRL_DIV_Div4                          = 0x2UL,                /*!< Clock divided by 4 */
  TMR_CTRL_DIV_Div8                          = 0x3UL,                /*!< Clock divided by 8 */
} TMR_CTRL_DIV_Enum;

typedef enum {
  TMR_CTRL_CLKSEL_SysClk                     = 0x0UL,                /*!< Select system Clk */
  TMR_CTRL_CLKSEL_ExtClk                     = 0x1UL,                /*!< Select External Clk */
} TMR_CTRL_CLKSEL_Enum;

/*--  COUNT: Current value timer register ---------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!<  */
} _TMR_COUNT_bits;

/* Bit field positions: */
#define TMR_COUNT_VAL_Pos                     0                      /*!<  */

/* Bit field masks: */
#define TMR_COUNT_VAL_Msk                     0x0000FFFFUL           /*!<  */

/*--  IM: Interrupt mask register -----------------------------------------------------------------------------*/
typedef struct {
  uint32_t TMR                    :1;                                /*!< Timer Interrupt Enable */
  uint32_t CAP0                   :1;                                /*!< Capcom0 Interrupt Enable */
  uint32_t CAP1                   :1;                                /*!< Capcom1 Interrupt Enable */
  uint32_t CAP2                   :1;                                /*!< Capcom2 Interrupt Enable */
  uint32_t CAP3                   :1;                                /*!< Capcom3 Interrupt Enable */
} _TMR_IM_bits;

/* Bit field positions: */
#define TMR_IM_TMR_Pos                        0                      /*!< Timer Interrupt Enable */
#define TMR_IM_CAP0_Pos                       1                      /*!< Capcom0 Interrupt Enable */
#define TMR_IM_CAP1_Pos                       2                      /*!< Capcom1 Interrupt Enable */
#define TMR_IM_CAP2_Pos                       3                      /*!< Capcom2 Interrupt Enable */
#define TMR_IM_CAP3_Pos                       4                      /*!< Capcom3 Interrupt Enable */

/* Bit field masks: */
#define TMR_IM_TMR_Msk                        0x00000001UL           /*!< Timer Interrupt Enable */
#define TMR_IM_CAP0_Msk                       0x00000002UL           /*!< Capcom0 Interrupt Enable */
#define TMR_IM_CAP1_Msk                       0x00000004UL           /*!< Capcom1 Interrupt Enable */
#define TMR_IM_CAP2_Msk                       0x00000008UL           /*!< Capcom2 Interrupt Enable */
#define TMR_IM_CAP3_Msk                       0x00000010UL           /*!< Capcom3 Interrupt Enable */

/*--  RIS: Raw Interrupt Status register ----------------------------------------------------------------------*/
typedef struct {
  uint32_t TMR                    :1;                                /*!< Timer Interrupt Status */
  uint32_t CAP0                   :1;                                /*!< Capcom0 Interrupt Status */
  uint32_t CAP1                   :1;                                /*!< Capcom1 Interrupt Status */
  uint32_t CAP2                   :1;                                /*!< Capcom2 Interrupt Status */
  uint32_t CAP3                   :1;                                /*!< Capcom3 Interrupt Status */
} _TMR_RIS_bits;

/* Bit field positions: */
#define TMR_RIS_TMR_Pos                       0                      /*!< Timer Interrupt Status */
#define TMR_RIS_CAP0_Pos                      1                      /*!< Capcom0 Interrupt Status */
#define TMR_RIS_CAP1_Pos                      2                      /*!< Capcom1 Interrupt Status */
#define TMR_RIS_CAP2_Pos                      3                      /*!< Capcom2 Interrupt Status */
#define TMR_RIS_CAP3_Pos                      4                      /*!< Capcom3 Interrupt Status */

/* Bit field masks: */
#define TMR_RIS_TMR_Msk                       0x00000001UL           /*!< Timer Interrupt Status */
#define TMR_RIS_CAP0_Msk                      0x00000002UL           /*!< Capcom0 Interrupt Status */
#define TMR_RIS_CAP1_Msk                      0x00000004UL           /*!< Capcom1 Interrupt Status */
#define TMR_RIS_CAP2_Msk                      0x00000008UL           /*!< Capcom2 Interrupt Status */
#define TMR_RIS_CAP3_Msk                      0x00000010UL           /*!< Capcom3 Interrupt Status */

/*--  MIS: Masked Interrupt Status register -------------------------------------------------------------------*/
typedef struct {
  uint32_t TMR                    :1;                                /*!< Timer Interrupt Status */
  uint32_t CAP0                   :1;                                /*!< Capcom0 Interrupt Status */
  uint32_t CAP1                   :1;                                /*!< Capcom1 Interrupt Status */
  uint32_t CAP2                   :1;                                /*!< Capcom2 Interrupt Status */
  uint32_t CAP3                   :1;                                /*!< Capcom3 Interrupt Status */
} _TMR_MIS_bits;

/* Bit field positions: */
#define TMR_MIS_TMR_Pos                       0                      /*!< Timer Interrupt Status */
#define TMR_MIS_CAP0_Pos                      1                      /*!< Capcom0 Interrupt Status */
#define TMR_MIS_CAP1_Pos                      2                      /*!< Capcom1 Interrupt Status */
#define TMR_MIS_CAP2_Pos                      3                      /*!< Capcom2 Interrupt Status */
#define TMR_MIS_CAP3_Pos                      4                      /*!< Capcom3 Interrupt Status */

/* Bit field masks: */
#define TMR_MIS_TMR_Msk                       0x00000001UL           /*!< Timer Interrupt Status */
#define TMR_MIS_CAP0_Msk                      0x00000002UL           /*!< Capcom0 Interrupt Status */
#define TMR_MIS_CAP1_Msk                      0x00000004UL           /*!< Capcom1 Interrupt Status */
#define TMR_MIS_CAP2_Msk                      0x00000008UL           /*!< Capcom2 Interrupt Status */
#define TMR_MIS_CAP3_Msk                      0x00000010UL           /*!< Capcom3 Interrupt Status */

/*--  IC: Clear Interrupt Status register ---------------------------------------------------------------------*/
typedef struct {
  uint32_t TMR                    :1;                                /*!< Timer Interrupt Clear */
  uint32_t CAP0                   :1;                                /*!< Capcom0 Interrupt Clear */
  uint32_t CAP1                   :1;                                /*!< Capcom1 Interrupt Clear */
  uint32_t CAP2                   :1;                                /*!< Capcom2 Interrupt Clear */
  uint32_t CAP3                   :1;                                /*!< Capcom3 Interrupt Clear */
} _TMR_IC_bits;

/* Bit field positions: */
#define TMR_IC_TMR_Pos                        0                      /*!< Timer Interrupt Clear */
#define TMR_IC_CAP0_Pos                       1                      /*!< Capcom0 Interrupt Clear */
#define TMR_IC_CAP1_Pos                       2                      /*!< Capcom1 Interrupt Clear */
#define TMR_IC_CAP2_Pos                       3                      /*!< Capcom2 Interrupt Clear */
#define TMR_IC_CAP3_Pos                       4                      /*!< Capcom3 Interrupt Clear */

/* Bit field masks: */
#define TMR_IC_TMR_Msk                        0x00000001UL           /*!< Timer Interrupt Clear */
#define TMR_IC_CAP0_Msk                       0x00000002UL           /*!< Capcom0 Interrupt Clear */
#define TMR_IC_CAP1_Msk                       0x00000004UL           /*!< Capcom1 Interrupt Clear */
#define TMR_IC_CAP2_Msk                       0x00000008UL           /*!< Capcom2 Interrupt Clear */
#define TMR_IC_CAP3_Msk                       0x00000010UL           /*!< Capcom3 Interrupt Clear */

/*-- CAPCOM: CTRL: Capture / Compare Control register ---------------------------------------------------------*/
typedef struct {
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t OVF                    :1;                                /*!< Capture Overflow */
  uint32_t OUT                    :1;                                /*!< Output */
  uint32_t CCI                    :1;                                /*!< Capture / Compare Input */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t OUTMODE                :3;                                /*!< Output Mode */
  uint32_t CAP                    :1;                                /*!< Capture Mode */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t SCCI                   :1;                                /*!< Synchronized Capture / Compare Input */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t CCISEL                 :2;                                /*!< Capture / Compare Input Select */
  uint32_t CAPMODE                :2;                                /*!< Capture Mode */
} _TMR_CAPCOM_CTRL_bits;

/* Bit field positions: */
#define TMR_CAPCOM_CTRL_OVF_Pos               1                      /*!< Capture Overflow */
#define TMR_CAPCOM_CTRL_OUT_Pos               2                      /*!< Output */
#define TMR_CAPCOM_CTRL_CCI_Pos               3                      /*!< Capture / Compare Input */
#define TMR_CAPCOM_CTRL_OUTMODE_Pos           5                      /*!< Output Mode */
#define TMR_CAPCOM_CTRL_CAP_Pos               8                      /*!< Capture Mode */
#define TMR_CAPCOM_CTRL_SCCI_Pos              10                     /*!< Synchronized Capture / Compare Input */
#define TMR_CAPCOM_CTRL_CCISEL_Pos            12                     /*!< Capture / Compare Input Select */
#define TMR_CAPCOM_CTRL_CAPMODE_Pos           14                     /*!< Capture Mode */

/* Bit field masks: */
#define TMR_CAPCOM_CTRL_OVF_Msk               0x00000002UL           /*!< Capture Overflow */
#define TMR_CAPCOM_CTRL_OUT_Msk               0x00000004UL           /*!< Output */
#define TMR_CAPCOM_CTRL_CCI_Msk               0x00000008UL           /*!< Capture / Compare Input */
#define TMR_CAPCOM_CTRL_OUTMODE_Msk           0x000000E0UL           /*!< Output Mode */
#define TMR_CAPCOM_CTRL_CAP_Msk               0x00000100UL           /*!< Capture Mode */
#define TMR_CAPCOM_CTRL_SCCI_Msk              0x00000400UL           /*!< Synchronized Capture / Compare Input */
#define TMR_CAPCOM_CTRL_CCISEL_Msk            0x00003000UL           /*!< Capture / Compare Input Select */
#define TMR_CAPCOM_CTRL_CAPMODE_Msk           0x0000C000UL           /*!< Capture Mode */

/* Bit field enums: */
typedef enum {
  TMR_CAPCOM_CTRL_CAPMODE_None             = 0x0UL,                /*!< No Capture */
  TMR_CAPCOM_CTRL_CAPMODE_Raise            = 0x1UL,                /*!< Capture on Raise */
  TMR_CAPCOM_CTRL_CAPMODE_Fail             = 0x2UL,                /*!< Capture on Fail */
  TMR_CAPCOM_CTRL_CAPMODE_Edge             = 0x3UL,                /*!< Capture on Raise and Fail */
} TMR_CAPCOM_CTRL_CAPMODE_Enum;

typedef enum {
  TMR_CAPCOM_CTRL_OUTMODE_BitOUT          = 0x0UL,                /*!< Output control by bit OUT */
  TMR_CAPCOM_CTRL_OUTMODE_Set             = 0x1UL,                /*!< Set */
  TMR_CAPCOM_CTRL_OUTMODE_Toggle_Reset    = 0x2UL,                /*!< Toggle/Reset */
  TMR_CAPCOM_CTRL_OUTMODE_Set_Reset       = 0x3UL,                /*!< Set/Reset */
  TMR_CAPCOM_CTRL_OUTMODE_Toggle          = 0x4UL,                /*!< Toggle */
  TMR_CAPCOM_CTRL_OUTMODE_Reset           = 0x5UL,                /*!< Reset */
  TMR_CAPCOM_CTRL_OUTMODE_Toggle_Set      = 0x6UL,                /*!< Toggle/Set */
  TMR_CAPCOM_CTRL_OUTMODE_Reset_Set       = 0x7UL,                /*!< сReset/Set */
} TMR_CAPCOM_CTRL_OUTMODE_Enum;

/*-- CAPCOM: VAL: Capture/Compare Value -----------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!<  */
} _TMR_CAPCOM_VAL_bits;

/* Bit field positions: */
#define TMR_CAPCOM_VAL_VAL_Pos                0                      /*!<  */

/* Bit field masks: */
#define TMR_CAPCOM_VAL_VAL_Msk                0x0000FFFFUL           /*!<  */

/*--  DMA_IM: DMA request mask register -----------------------------------------------------------------------*/
typedef struct {
  uint32_t TMR                    :1;                                /*!< Timer DMA Request Enable */
  uint32_t CAP0                   :1;                                /*!< Capcom0 DMA Request Enable */
  uint32_t CAP1                   :1;                                /*!< Capcom1 DMA Request Enable */
  uint32_t CAP2                   :1;                                /*!< Capcom2 DMA Request Enable */
  uint32_t CAP3                   :1;                                /*!< Capcom3 DMA Request Enable */
} _TMR_DMA_IM_bits;

/* Bit field positions: */
#define TMR_DMA_IM_TMR_Pos                    0                      /*!< Timer DMA Request Enable */
#define TMR_DMA_IM_CAP0_Pos                   1                      /*!< Capcom0 DMA Request Enable */
#define TMR_DMA_IM_CAP1_Pos                   2                      /*!< Capcom1 DMA Request Enable */
#define TMR_DMA_IM_CAP2_Pos                   3                      /*!< Capcom2 DMA Request Enable */
#define TMR_DMA_IM_CAP3_Pos                   4                      /*!< Capcom3 DMA Request Enable */

/* Bit field masks: */
#define TMR_DMA_IM_TMR_Msk                    0x00000001UL           /*!< Timer DMA Request Enable */
#define TMR_DMA_IM_CAP0_Msk                   0x00000002UL           /*!< Capcom0 DMA Request Enable */
#define TMR_DMA_IM_CAP1_Msk                   0x00000004UL           /*!< Capcom1 DMA Request Enable */
#define TMR_DMA_IM_CAP2_Msk                   0x00000008UL           /*!< Capcom2 DMA Request Enable */
#define TMR_DMA_IM_CAP3_Msk                   0x00000010UL           /*!< Capcom3 DMA Request Enable */

/*--  ADC_IM: ADC request mask register -----------------------------------------------------------------------*/
typedef struct {
  uint32_t TMR                    :1;                                /*!< Timer ADC Request Enable */
  uint32_t CAP0                   :1;                                /*!< Capcom0 ADC Request Enable */
  uint32_t CAP1                   :1;                                /*!< Capcom1 ADC Request Enable */
  uint32_t CAP2                   :1;                                /*!< Capcom2 ADC Request Enable */
  uint32_t CAP3                   :1;                                /*!< Capcom3 ADC Request Enable */
} _TMR_ADC_IM_bits;

/* Bit field positions: */
#define TMR_ADC_IM_TMR_Pos                    0                      /*!< Timer ADC Request Enable */
#define TMR_ADC_IM_CAP0_Pos                   1                      /*!< Capcom0 ADC Request Enable */
#define TMR_ADC_IM_CAP1_Pos                   2                      /*!< Capcom1 ADC Request Enable */
#define TMR_ADC_IM_CAP2_Pos                   3                      /*!< Capcom2 ADC Request Enable */
#define TMR_ADC_IM_CAP3_Pos                   4                      /*!< Capcom3 ADC Request Enable */

/* Bit field masks: */
#define TMR_ADC_IM_TMR_Msk                    0x00000001UL           /*!< Timer ADC Request Enable */
#define TMR_ADC_IM_CAP0_Msk                   0x00000002UL           /*!< Capcom0 ADC Request Enable */
#define TMR_ADC_IM_CAP1_Msk                   0x00000004UL           /*!< Capcom1 ADC Request Enable */
#define TMR_ADC_IM_CAP2_Msk                   0x00000008UL           /*!< Capcom2 ADC Request Enable */
#define TMR_ADC_IM_CAP3_Msk                   0x00000010UL           /*!< Capcom3 ADC Request Enable */

//Cluster CAPCOM:
typedef struct {
  union { 
  /*!< Capture / Compare Control register */
    __IO uint32_t CTRL;                                              /*!< CTRL : type used for word access */
    __IO _TMR_CAPCOM_CTRL_bits CTRL_bit;                             /*!< CTRL_bit: structure used for bit access */
  };
  union { 
  /*!< Capture/Compare Value */
    __IO uint16_t VAL;                                               /*!< VAL : type used for word access */
    __IO _TMR_CAPCOM_VAL_bits VAL_bit;                               /*!< VAL_bit: structure used for bit access */
  };
} _TMR_CAPCOM_TypeDef;
typedef struct {
  union {                                                               /*!< Timer control register */
    __IO uint32_t CTRL;                                              /*!< CTRL    : type used for word access */
    __IO _TMR_CTRL_bits  CTRL_bit;                                   /*!< CTRL_bit: structure used for bit access */
  };
  union {                                                               /*!< Current value timer register */
    __IO uint32_t COUNT;                                             /*!< COUNT    : type used for word access */
    __IO _TMR_COUNT_bits  COUNT_bit;                                 /*!< COUNT_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt mask register */
    __IO uint32_t IM;                                                /*!< IM    : type used for word access */
    __IO _TMR_IM_bits  IM_bit;                                       /*!< IM_bit: structure used for bit access */
  };
  union {                                                               /*!< Raw Interrupt Status register */
    __I uint32_t RIS;                                                /*!< RIS    : type used for word access */
    __I _TMR_RIS_bits  RIS_bit;                                      /*!< RIS_bit: structure used for bit access */
  };
  union {                                                               /*!< Masked Interrupt Status register */
    __I uint32_t MIS;                                                /*!< MIS    : type used for word access */
    __I _TMR_MIS_bits  MIS_bit;                                      /*!< MIS_bit: structure used for bit access */
  };
  union {                                                               /*!< Clear Interrupt Status register */
    __O uint32_t IC;                                                 /*!< IC    : type used for word access */
    __O _TMR_IC_bits  IC_bit;                                        /*!< IC_bit: structure used for bit access */
  };
  _TMR_CAPCOM_TypeDef CAPCOM[4];
  union {                                                               /*!< DMA request mask register */
    __IO uint32_t DMA_IM;                                            /*!< DMA_IM    : type used for word access */
    __IO _TMR_DMA_IM_bits  DMA_IM_bit;                               /*!< DMA_IM_bit: structure used for bit access */
  };
  union {                                                               /*!< ADC request mask register */
    __IO uint32_t ADC_IM;                                            /*!< ADC_IM    : type used for word access */
    __IO _TMR_ADC_IM_bits  ADC_IM_bit;                               /*!< ADC_IM_bit: structure used for bit access */
  };
} TMR_TypeDef;


/******************************************************************************/
/*                         TMR32 registers                                    */
/******************************************************************************/

/*--  CTRL: Timer control register ----------------------------------------------------------------------------*/
typedef struct {
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t CLR                    :1;                                /*!< Timer Clear */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t MODE                   :2;                                /*!< Count mode */
  uint32_t DIV                    :2;                                /*!< Divider */
  uint32_t CLKSEL                 :1;                                /*!< Clock Source Select */
} _TMR32_CTRL_bits;

/* Bit field positions: */
#define TMR32_CTRL_CLR_Pos                    2                      /*!< Timer Clear */
#define TMR32_CTRL_MODE_Pos                   4                      /*!< Count mode */
#define TMR32_CTRL_DIV_Pos                    6                      /*!< Divider */
#define TMR32_CTRL_CLKSEL_Pos                 8                      /*!< Clock Source Select */

/* Bit field masks: */
#define TMR32_CTRL_CLR_Msk                    0x00000004UL           /*!< Timer Clear */
#define TMR32_CTRL_MODE_Msk                   0x00000030UL           /*!< Count mode */
#define TMR32_CTRL_DIV_Msk                    0x000000C0UL           /*!< Divider */
#define TMR32_CTRL_CLKSEL_Msk                 0x00000100UL           /*!< Clock Source Select */

/* Bit field enums: */
typedef enum {
  TMR32_CTRL_MODE_Stop                       = 0x0UL,                /*!< Timer is Stopped */
  TMR32_CTRL_MODE_Up                         = 0x1UL,                /*!< Timer count Up */
  TMR32_CTRL_MODE_Multiple                   = 0x2UL,                /*!< Timer count is multiple Up */
  TMR32_CTRL_MODE_UpDown                     = 0x3UL,                /*!< Timer count is multiple Up - Down */
} TMR32_CTRL_MODE_Enum;

typedef enum {
  TMR32_CTRL_DIV_Div1                        = 0x0UL,                /*!< Clock not divided */
  TMR32_CTRL_DIV_Div2                        = 0x1UL,                /*!< Clock divided by 2 */
  TMR32_CTRL_DIV_Div4                        = 0x2UL,                /*!< Clock divided by 4 */
  TMR32_CTRL_DIV_Div8                        = 0x3UL,                /*!< Clock divided by 8 */
} TMR32_CTRL_DIV_Enum;

typedef enum {
  TMR32_CTRL_CLKSEL_SysClk                   = 0x0UL,                /*!< Select system Clk */
  TMR32_CTRL_CLKSEL_ExtClk                   = 0x1UL,                /*!< Select External Clk */
} TMR32_CTRL_CLKSEL_Enum;

/*--  COUNT: Current value timer register ---------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Current value timer */
} _TMR32_COUNT_bits;

/* Bit field positions: */
#define TMR32_COUNT_VAL_Pos                   0                      /*!< Current value timer */

/* Bit field masks: */
#define TMR32_COUNT_VAL_Msk                   0xFFFFFFFFUL           /*!< Current value timer */

/*--  IM: Interrupt mask register -----------------------------------------------------------------------------*/
typedef struct {
  uint32_t TMR                    :1;                                /*!< Timer Interrupt Enable */
  uint32_t CAP0                   :1;                                /*!< Capcom0 Interrupt Enable */
  uint32_t CAP1                   :1;                                /*!< Capcom1 Interrupt Enable */
  uint32_t CAP2                   :1;                                /*!< Capcom2 Interrupt Enable */
  uint32_t CAP3                   :1;                                /*!< Capcom3 Interrupt Enable */
} _TMR32_IM_bits;

/* Bit field positions: */
#define TMR32_IM_TMR_Pos                      0                      /*!< Timer Interrupt Enable */
#define TMR32_IM_CAP0_Pos                     1                      /*!< Capcom0 Interrupt Enable */
#define TMR32_IM_CAP1_Pos                     2                      /*!< Capcom1 Interrupt Enable */
#define TMR32_IM_CAP2_Pos                     3                      /*!< Capcom2 Interrupt Enable */
#define TMR32_IM_CAP3_Pos                     4                      /*!< Capcom3 Interrupt Enable */

/* Bit field masks: */
#define TMR32_IM_TMR_Msk                      0x00000001UL           /*!< Timer Interrupt Enable */
#define TMR32_IM_CAP0_Msk                     0x00000002UL           /*!< Capcom0 Interrupt Enable */
#define TMR32_IM_CAP1_Msk                     0x00000004UL           /*!< Capcom1 Interrupt Enable */
#define TMR32_IM_CAP2_Msk                     0x00000008UL           /*!< Capcom2 Interrupt Enable */
#define TMR32_IM_CAP3_Msk                     0x00000010UL           /*!< Capcom3 Interrupt Enable */

/*--  RIS: Raw Interrupt Status register ----------------------------------------------------------------------*/
typedef struct {
  uint32_t TMR                    :1;                                /*!< Timer Interrupt Status */
  uint32_t CAP0                   :1;                                /*!< Capcom0 Interrupt Status */
  uint32_t CAP1                   :1;                                /*!< Capcom1 Interrupt Status */
  uint32_t CAP2                   :1;                                /*!< Capcom2 Interrupt Status */
  uint32_t CAP3                   :1;                                /*!< Capcom3 Interrupt Status */
} _TMR32_RIS_bits;

/* Bit field positions: */
#define TMR32_RIS_TMR_Pos                     0                      /*!< Timer Interrupt Status */
#define TMR32_RIS_CAP0_Pos                    1                      /*!< Capcom0 Interrupt Status */
#define TMR32_RIS_CAP1_Pos                    2                      /*!< Capcom1 Interrupt Status */
#define TMR32_RIS_CAP2_Pos                    3                      /*!< Capcom2 Interrupt Status */
#define TMR32_RIS_CAP3_Pos                    4                      /*!< Capcom3 Interrupt Status */

/* Bit field masks: */
#define TMR32_RIS_TMR_Msk                     0x00000001UL           /*!< Timer Interrupt Status */
#define TMR32_RIS_CAP0_Msk                    0x00000002UL           /*!< Capcom0 Interrupt Status */
#define TMR32_RIS_CAP1_Msk                    0x00000004UL           /*!< Capcom1 Interrupt Status */
#define TMR32_RIS_CAP2_Msk                    0x00000008UL           /*!< Capcom2 Interrupt Status */
#define TMR32_RIS_CAP3_Msk                    0x00000010UL           /*!< Capcom3 Interrupt Status */

/*--  MIS: Masked Interrupt Status register -------------------------------------------------------------------*/
typedef struct {
  uint32_t TMR                    :1;                                /*!< Timer Interrupt Status */
  uint32_t CAP0                   :1;                                /*!< Capcom0 Interrupt Status */
  uint32_t CAP1                   :1;                                /*!< Capcom1 Interrupt Status */
  uint32_t CAP2                   :1;                                /*!< Capcom2 Interrupt Status */
  uint32_t CAP3                   :1;                                /*!< Capcom3 Interrupt Status */
} _TMR32_MIS_bits;

/* Bit field positions: */
#define TMR32_MIS_TMR_Pos                     0                      /*!< Timer Interrupt Status */
#define TMR32_MIS_CAP0_Pos                    1                      /*!< Capcom0 Interrupt Status */
#define TMR32_MIS_CAP1_Pos                    2                      /*!< Capcom1 Interrupt Status */
#define TMR32_MIS_CAP2_Pos                    3                      /*!< Capcom2 Interrupt Status */
#define TMR32_MIS_CAP3_Pos                    4                      /*!< Capcom3 Interrupt Status */

/* Bit field masks: */
#define TMR32_MIS_TMR_Msk                     0x00000001UL           /*!< Timer Interrupt Status */
#define TMR32_MIS_CAP0_Msk                    0x00000002UL           /*!< Capcom0 Interrupt Status */
#define TMR32_MIS_CAP1_Msk                    0x00000004UL           /*!< Capcom1 Interrupt Status */
#define TMR32_MIS_CAP2_Msk                    0x00000008UL           /*!< Capcom2 Interrupt Status */
#define TMR32_MIS_CAP3_Msk                    0x00000010UL           /*!< Capcom3 Interrupt Status */

/*--  IC: Clear Interrupt Status register ---------------------------------------------------------------------*/
typedef struct {
  uint32_t TMR                    :1;                                /*!< Timer Interrupt Clear */
  uint32_t CAP0                   :1;                                /*!< Capcom0 Interrupt Clear */
  uint32_t CAP1                   :1;                                /*!< Capcom1 Interrupt Clear */
  uint32_t CAP2                   :1;                                /*!< Capcom2 Interrupt Clear */
  uint32_t CAP3                   :1;                                /*!< Capcom3 Interrupt Clear */
} _TMR32_IC_bits;

/* Bit field positions: */
#define TMR32_IC_TMR_Pos                      0                      /*!< Timer Interrupt Clear */
#define TMR32_IC_CAP0_Pos                     1                      /*!< Capcom0 Interrupt Clear */
#define TMR32_IC_CAP1_Pos                     2                      /*!< Capcom1 Interrupt Clear */
#define TMR32_IC_CAP2_Pos                     3                      /*!< Capcom2 Interrupt Clear */
#define TMR32_IC_CAP3_Pos                     4                      /*!< Capcom3 Interrupt Clear */

/* Bit field masks: */
#define TMR32_IC_TMR_Msk                      0x00000001UL           /*!< Timer Interrupt Clear */
#define TMR32_IC_CAP0_Msk                     0x00000002UL           /*!< Capcom0 Interrupt Clear */
#define TMR32_IC_CAP1_Msk                     0x00000004UL           /*!< Capcom1 Interrupt Clear */
#define TMR32_IC_CAP2_Msk                     0x00000008UL           /*!< Capcom2 Interrupt Clear */
#define TMR32_IC_CAP3_Msk                     0x00000010UL           /*!< Capcom3 Interrupt Clear */

/*-- CAPCOM: CTRL: Capture / Compare Control register ---------------------------------------------------------*/
typedef struct {
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t OVF                    :1;                                /*!< Capture Overflow */
  uint32_t OUT                    :1;                                /*!< Output */
  uint32_t CCI                    :1;                                /*!< Capture / Compare Input */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t OUTMODE                :3;                                /*!< Output Mode */
  uint32_t CAP                    :1;                                /*!< Capture Mode */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t SCCI                   :1;                                /*!< Synchronized Capture / Compare Input */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t CCISEL                 :2;                                /*!< Capture / Compare Input Select */
  uint32_t CAPMODE                :2;                                /*!< Capture Mode */
} _TMR32_CAPCOM_CTRL_bits;

/* Bit field positions: */
#define TMR32_CAPCOM_CTRL_OVF_Pos             1                      /*!< Capture Overflow */
#define TMR32_CAPCOM_CTRL_OUT_Pos             2                      /*!< Output */
#define TMR32_CAPCOM_CTRL_CCI_Pos             3                      /*!< Capture / Compare Input */
#define TMR32_CAPCOM_CTRL_OUTMODE_Pos         5                      /*!< Output Mode */
#define TMR32_CAPCOM_CTRL_CAP_Pos             8                      /*!< Capture Mode */
#define TMR32_CAPCOM_CTRL_SCCI_Pos            10                     /*!< Synchronized Capture / Compare Input */
#define TMR32_CAPCOM_CTRL_CCISEL_Pos          12                     /*!< Capture / Compare Input Select */
#define TMR32_CAPCOM_CTRL_CAPMODE_Pos         14                     /*!< Capture Mode */

/* Bit field masks: */
#define TMR32_CAPCOM_CTRL_OVF_Msk             0x00000002UL           /*!< Capture Overflow */
#define TMR32_CAPCOM_CTRL_OUT_Msk             0x00000004UL           /*!< Output */
#define TMR32_CAPCOM_CTRL_CCI_Msk             0x00000008UL           /*!< Capture / Compare Input */
#define TMR32_CAPCOM_CTRL_OUTMODE_Msk         0x000000E0UL           /*!< Output Mode */
#define TMR32_CAPCOM_CTRL_CAP_Msk             0x00000100UL           /*!< Capture Mode */
#define TMR32_CAPCOM_CTRL_SCCI_Msk            0x00000400UL           /*!< Synchronized Capture / Compare Input */
#define TMR32_CAPCOM_CTRL_CCISEL_Msk          0x00003000UL           /*!< Capture / Compare Input Select */
#define TMR32_CAPCOM_CTRL_CAPMODE_Msk         0x0000C000UL           /*!< Capture Mode */

/* Bit field enums: */
typedef enum {
  TMR32_CAPCOM_CTRL_CAPMODE_None             = 0x0UL,                /*!< No Capture */
  TMR32_CAPCOM_CTRL_CAPMODE_Raise            = 0x1UL,                /*!< Capture on Raise */
  TMR32_CAPCOM_CTRL_CAPMODE_Fail             = 0x2UL,                /*!< Capture on Fail */
  TMR32_CAPCOM_CTRL_CAPMODE_Edge             = 0x3UL,                /*!< Capture on Raise and Fail */
} TMR32_CAPCOM_CTRL_CAPMODE_Enum;

typedef enum {
  TMR32_CAPCOM_CTRL_OUTMODE_BitOUT          = 0x0UL,                /*!< Output control by bit OUT */
  TMR32_CAPCOM_CTRL_OUTMODE_Set             = 0x1UL,                /*!< Set */
  TMR32_CAPCOM_CTRL_OUTMODE_Toggle_Reset    = 0x2UL,                /*!< Toggle/Reset */
  TMR32_CAPCOM_CTRL_OUTMODE_Set_Reset       = 0x3UL,                /*!< Set/Reset */
  TMR32_CAPCOM_CTRL_OUTMODE_Toggle          = 0x4UL,                /*!< Toggle */
  TMR32_CAPCOM_CTRL_OUTMODE_Reset           = 0x5UL,                /*!< Reset */
  TMR32_CAPCOM_CTRL_OUTMODE_Toggle_Set      = 0x6UL,                /*!< Toggle/Set */
  TMR32_CAPCOM_CTRL_OUTMODE_Reset_Set       = 0x7UL,                /*!< сReset/Set */
} TMR32_CAPCOM_CTRL_OUTMODE_Enum;

/*-- CAPCOM: VAL: Capture/Compare Value -----------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!<  */
} _TMR32_CAPCOM_VAL_bits;

/* Bit field positions: */
#define TMR32_CAPCOM_VAL_VAL_Pos              0                      /*!<  */

/* Bit field masks: */
#define TMR32_CAPCOM_VAL_VAL_Msk              0xFFFFFFFFUL           /*!<  */

/*--  DMA_IM: DMA request mask register -----------------------------------------------------------------------*/
typedef struct {
  uint32_t TMR                    :1;                                /*!< Timer DMA Request Enable */
  uint32_t CAP0                   :1;                                /*!< Capcom0 DMA Request Enable */
  uint32_t CAP1                   :1;                                /*!< Capcom1 DMA Request Enable */
  uint32_t CAP2                   :1;                                /*!< Capcom2 DMA Request Enable */
  uint32_t CAP3                   :1;                                /*!< Capcom3 DMA Request Enable */
} _TMR32_DMA_IM_bits;

/* Bit field positions: */
#define TMR32_DMA_IM_TMR_Pos                  0                      /*!< Timer DMA Request Enable */
#define TMR32_DMA_IM_CAP0_Pos                 1                      /*!< Capcom0 DMA Request Enable */
#define TMR32_DMA_IM_CAP1_Pos                 2                      /*!< Capcom1 DMA Request Enable */
#define TMR32_DMA_IM_CAP2_Pos                 3                      /*!< Capcom2 DMA Request Enable */
#define TMR32_DMA_IM_CAP3_Pos                 4                      /*!< Capcom3 DMA Request Enable */

/* Bit field masks: */
#define TMR32_DMA_IM_TMR_Msk                  0x00000001UL           /*!< Timer DMA Request Enable */
#define TMR32_DMA_IM_CAP0_Msk                 0x00000002UL           /*!< Capcom0 DMA Request Enable */
#define TMR32_DMA_IM_CAP1_Msk                 0x00000004UL           /*!< Capcom1 DMA Request Enable */
#define TMR32_DMA_IM_CAP2_Msk                 0x00000008UL           /*!< Capcom2 DMA Request Enable */
#define TMR32_DMA_IM_CAP3_Msk                 0x00000010UL           /*!< Capcom3 DMA Request Enable */

/*--  ADC_IM: ADC request mask register -----------------------------------------------------------------------*/
typedef struct {
  uint32_t TMR                    :1;                                /*!< Timer ADC Request Enable */
  uint32_t CAP0                   :1;                                /*!< Capcom0 ADC Request Enable */
  uint32_t CAP1                   :1;                                /*!< Capcom1 ADC Request Enable */
  uint32_t CAP2                   :1;                                /*!< Capcom2 ADC Request Enable */
  uint32_t CAP3                   :1;                                /*!< Capcom3 ADC Request Enable */
} _TMR32_ADC_IM_bits;

/* Bit field positions: */
#define TMR32_ADC_IM_TMR_Pos                  0                      /*!< Timer ADC Request Enable */
#define TMR32_ADC_IM_CAP0_Pos                 1                      /*!< Capcom0 ADC Request Enable */
#define TMR32_ADC_IM_CAP1_Pos                 2                      /*!< Capcom1 ADC Request Enable */
#define TMR32_ADC_IM_CAP2_Pos                 3                      /*!< Capcom2 ADC Request Enable */
#define TMR32_ADC_IM_CAP3_Pos                 4                      /*!< Capcom3 ADC Request Enable */

/* Bit field masks: */
#define TMR32_ADC_IM_TMR_Msk                  0x00000001UL           /*!< Timer ADC Request Enable */
#define TMR32_ADC_IM_CAP0_Msk                 0x00000002UL           /*!< Capcom0 ADC Request Enable */
#define TMR32_ADC_IM_CAP1_Msk                 0x00000004UL           /*!< Capcom1 ADC Request Enable */
#define TMR32_ADC_IM_CAP2_Msk                 0x00000008UL           /*!< Capcom2 ADC Request Enable */
#define TMR32_ADC_IM_CAP3_Msk                 0x00000010UL           /*!< Capcom3 ADC Request Enable */

//Cluster CAPCOM:
typedef struct {
  union { 
  /*!< Capture / Compare Control register */
    __IO uint32_t CTRL;                                              /*!< CTRL : type used for word access */
    __IO _TMR32_CAPCOM_CTRL_bits CTRL_bit;                           /*!< CTRL_bit: structure used for bit access */
  };
  union { 
  /*!< Capture/Compare Value */
    __IO uint32_t VAL;                                               /*!< VAL : type used for word access */
    __IO _TMR32_CAPCOM_VAL_bits VAL_bit;                             /*!< VAL_bit: structure used for bit access */
  };
} _TMR32_CAPCOM_TypeDef;
typedef struct {
  union {                                                               /*!< Timer control register */
    __IO uint32_t CTRL;                                              /*!< CTRL    : type used for word access */
    __IO _TMR32_CTRL_bits  CTRL_bit;                                 /*!< CTRL_bit: structure used for bit access */
  };
  union {                                                               /*!< Current value timer register */
    __IO uint32_t COUNT;                                             /*!< COUNT    : type used for word access */
    __IO _TMR32_COUNT_bits  COUNT_bit;                               /*!< COUNT_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt mask register */
    __IO uint32_t IM;                                                /*!< IM    : type used for word access */
    __IO _TMR32_IM_bits  IM_bit;                                     /*!< IM_bit: structure used for bit access */
  };
  union {                                                               /*!< Raw Interrupt Status register */
    __I uint32_t RIS;                                                /*!< RIS    : type used for word access */
    __I _TMR32_RIS_bits  RIS_bit;                                    /*!< RIS_bit: structure used for bit access */
  };
  union {                                                               /*!< Masked Interrupt Status register */
    __I uint32_t MIS;                                                /*!< MIS    : type used for word access */
    __I _TMR32_MIS_bits  MIS_bit;                                    /*!< MIS_bit: structure used for bit access */
  };
  union {                                                               /*!< Clear Interrupt Status register */
    __O uint32_t IC;                                                 /*!< IC    : type used for word access */
    __O _TMR32_IC_bits  IC_bit;                                      /*!< IC_bit: structure used for bit access */
  };
  _TMR32_CAPCOM_TypeDef CAPCOM[4];
  union {                                                               /*!< DMA request mask register */
    __IO uint32_t DMA_IM;                                            /*!< DMA_IM    : type used for word access */
    __IO _TMR32_DMA_IM_bits  DMA_IM_bit;                             /*!< DMA_IM_bit: structure used for bit access */
  };
  union {                                                               /*!< ADC request mask register */
    __IO uint32_t ADC_IM;                                            /*!< ADC_IM    : type used for word access */
    __IO _TMR32_ADC_IM_bits  ADC_IM_bit;                             /*!< ADC_IM_bit: structure used for bit access */
  };
} TMR32_TypeDef;


/******************************************************************************/
/*                         TRNG registers                                     */
/******************************************************************************/

/*--  CR: Control register ------------------------------------------------------------------------------------*/
typedef struct {
  uint32_t START                  :1;                                /*!< Start/enable the NDRNG */
  uint32_t LFSR                   :1;                                /*!< Select between the NDRNG with asynchronous free running oscillators (when 0) and the Pseudo-Random generator with synchronous oscillators */
  uint32_t TESTEN                 :1;                                /*!< Select input for conditioning function and continuous tests: 0: Noise source (normal mode) 1: Test data register (test mode) */
  uint32_t CONDBYPASS             :1;                                /*!< 0: Conditioning function is used (normal mode); 1: Conditioning function is bypassed (to observe entropy source directly) */
  uint32_t TFAILINTEN             :1;                                /*!< Enable interrupt if any of the health test fails */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t FFULLINTEN             :1;                                /*!< Enable interrupt if FIFO is full */
  uint32_t SOFTRST                :1;                                /*!< Datapath content 2ush and control FSM reset: 0: Normal mode; 1: Datapath components - the health tests, the conditioning function and the FIFO - are cleared. This bit is not cleared automatically. Configuration registers preserve their values. Send control FSM to Reset state. */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t FORCEROS               :1;                                /*!< Force oscillators to run when FIFO is full */
  uint32_t IGNORHTF               :1;                                /*!< Results of the health tests during start-up and online test do not affect the control FSM state. It also bypass control FSM StartUp phase */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t COUNTBLOCK             :4;                                /*!< Number of 128-bit blocks used in conditioning (AES-CBC-MAC) post-processing. Zero value is not allowed */
  uint32_t FIFOFILLST             :1;                                /*!< Enable write of the samples in the FIFO during start-up */
  uint32_t REPTSTDIS              :1;                                /*!< All repetition tests (each share) are disabled via this single bit */
  uint32_t PROPTSTDIS             :1;                                /*!< All proportion tests (each share) are disabled via this single bit */
  uint32_t AUTOCORRDIS            :2;                                /*!< Disable speci1c delay(s) check in auto-correlation test - same RO: x1: vs. the following sample of the same RO -> (0) delay check 1x: vs. the later sample of the same RO -> (+1) delay check */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t CORRTSTDIS             :3;                                /*!< Disable speci1c delay(s) check in correlation test - different ROs: xx1: vs. the same sample of the other RO -> (0) delay check x1x: vs. the preceding sample of the other RO -> (-1) delay check 1xx: vs. the next sample of the other RO -> (+1) delay check */
  uint32_t BLENDMETHOD            :2;                                /*!< Select blending method: 00: Concatenation; 01: XOR_LEVEL_1; 10: XOR_LEVEL_2; 11: VON-NEUMANN debiasing */
} _TRNG_CR_bits;

/* Bit field positions: */
#define TRNG_CR_START_Pos                     0                      /*!< Start/enable the NDRNG */
#define TRNG_CR_LFSR_Pos                      1                      /*!< Select between the NDRNG with asynchronous free running oscillators (when 0) and the Pseudo-Random generator with synchronous oscillators */
#define TRNG_CR_TESTEN_Pos                    2                      /*!< Select input for conditioning function and continuous tests: 0: Noise source (normal mode) 1: Test data register (test mode) */
#define TRNG_CR_CONDBYPASS_Pos                3                      /*!< 0: Conditioning function is used (normal mode); 1: Conditioning function is bypassed (to observe entropy source directly) */
#define TRNG_CR_TFAILINTEN_Pos                4                      /*!< Enable interrupt if any of the health test fails */
#define TRNG_CR_FFULLINTEN_Pos                7                      /*!< Enable interrupt if FIFO is full */
#define TRNG_CR_SOFTRST_Pos                   8                      /*!< Datapath content 2ush and control FSM reset: 0: Normal mode; 1: Datapath components - the health tests, the conditioning function and the FIFO - are cleared. This bit is not cleared automatically. Configuration registers preserve their values. Send control FSM to Reset state. */
#define TRNG_CR_FORCEROS_Pos                  11                     /*!< Force oscillators to run when FIFO is full */
#define TRNG_CR_IGNORHTF_Pos                  12                     /*!< Results of the health tests during start-up and online test do not affect the control FSM state. It also bypass control FSM StartUp phase */
#define TRNG_CR_COUNTBLOCK_Pos                16                     /*!< Number of 128-bit blocks used in conditioning (AES-CBC-MAC) post-processing. Zero value is not allowed */
#define TRNG_CR_FIFOFILLST_Pos                20                     /*!< Enable write of the samples in the FIFO during start-up */
#define TRNG_CR_REPTSTDIS_Pos                 21                     /*!< All repetition tests (each share) are disabled via this single bit */
#define TRNG_CR_PROPTSTDIS_Pos                22                     /*!< All proportion tests (each share) are disabled via this single bit */
#define TRNG_CR_AUTOCORRDIS_Pos               23                     /*!< Disable speci1c delay(s) check in auto-correlation test - same RO: x1: vs. the following sample of the same RO -> (0) delay check 1x: vs. the later sample of the same RO -> (+1) delay check */
#define TRNG_CR_CORRTSTDIS_Pos                27                     /*!< Disable speci1c delay(s) check in correlation test - different ROs: xx1: vs. the same sample of the other RO -> (0) delay check x1x: vs. the preceding sample of the other RO -> (-1) delay check 1xx: vs. the next sample of the other RO -> (+1) delay check */
#define TRNG_CR_BLENDMETHOD_Pos               30                     /*!< Select blending method: 00: Concatenation; 01: XOR_LEVEL_1; 10: XOR_LEVEL_2; 11: VON-NEUMANN debiasing */

/* Bit field masks: */
#define TRNG_CR_START_Msk                     0x00000001UL           /*!< Start/enable the NDRNG */
#define TRNG_CR_LFSR_Msk                      0x00000002UL           /*!< Select between the NDRNG with asynchronous free running oscillators (when 0) and the Pseudo-Random generator with synchronous oscillators */
#define TRNG_CR_TESTEN_Msk                    0x00000004UL           /*!< Select input for conditioning function and continuous tests: 0: Noise source (normal mode) 1: Test data register (test mode) */
#define TRNG_CR_CONDBYPASS_Msk                0x00000008UL           /*!< 0: Conditioning function is used (normal mode); 1: Conditioning function is bypassed (to observe entropy source directly) */
#define TRNG_CR_TFAILINTEN_Msk                0x00000010UL           /*!< Enable interrupt if any of the health test fails */
#define TRNG_CR_FFULLINTEN_Msk                0x00000080UL           /*!< Enable interrupt if FIFO is full */
#define TRNG_CR_SOFTRST_Msk                   0x00000100UL           /*!< Datapath content 2ush and control FSM reset: 0: Normal mode; 1: Datapath components - the health tests, the conditioning function and the FIFO - are cleared. This bit is not cleared automatically. Configuration registers preserve their values. Send control FSM to Reset state. */
#define TRNG_CR_FORCEROS_Msk                  0x00000800UL           /*!< Force oscillators to run when FIFO is full */
#define TRNG_CR_IGNORHTF_Msk                  0x00001000UL           /*!< Results of the health tests during start-up and online test do not affect the control FSM state. It also bypass control FSM StartUp phase */
#define TRNG_CR_COUNTBLOCK_Msk                0x000F0000UL           /*!< Number of 128-bit blocks used in conditioning (AES-CBC-MAC) post-processing. Zero value is not allowed */
#define TRNG_CR_FIFOFILLST_Msk                0x00100000UL           /*!< Enable write of the samples in the FIFO during start-up */
#define TRNG_CR_REPTSTDIS_Msk                 0x00200000UL           /*!< All repetition tests (each share) are disabled via this single bit */
#define TRNG_CR_PROPTSTDIS_Msk                0x00400000UL           /*!< All proportion tests (each share) are disabled via this single bit */
#define TRNG_CR_AUTOCORRDIS_Msk               0x01800000UL           /*!< Disable speci1c delay(s) check in auto-correlation test - same RO: x1: vs. the following sample of the same RO -> (0) delay check 1x: vs. the later sample of the same RO -> (+1) delay check */
#define TRNG_CR_CORRTSTDIS_Msk                0x38000000UL           /*!< Disable speci1c delay(s) check in correlation test - different ROs: xx1: vs. the same sample of the other RO -> (0) delay check x1x: vs. the preceding sample of the other RO -> (-1) delay check 1xx: vs. the next sample of the other RO -> (+1) delay check */
#define TRNG_CR_BLENDMETHOD_Msk               0xC0000000UL           /*!< Select blending method: 00: Concatenation; 01: XOR_LEVEL_1; 10: XOR_LEVEL_2; 11: VON-NEUMANN debiasing */

/* Bit field enums: */
typedef enum {
  TRNG_CR_LFSR_AsyncFreeRunOsc               = 0x0UL,                /*!< NDRNG with asynchronous free running oscillators */
  TRNG_CR_LFSR_SyncOsc                       = 0x1UL,                /*!< Pseudo-Random generator with synchronous oscillators for simulation purpose */
} TRNG_CR_LFSR_Enum;

typedef enum {
  TRNG_CR_BLENDMETHOD_Concatenation          = 0x0UL,                /*!< Blending method Concatenation */
  TRNG_CR_BLENDMETHOD_XOR_LEVEL_1            = 0x1UL,                /*!< Blending method XOR_LEVEL_1 */
  TRNG_CR_BLENDMETHOD_XOR_LEVEL_2            = 0x2UL,                /*!< Blending method XOR_LEVEL_2 */
  TRNG_CR_BLENDMETHOD_VON_NEUMANN_debiasing   = 0x3UL,               /*!< Blending method VON_NEUMANN_debiasing */
} TRNG_CR_BLENDMETHOD_Enum;

/*--  FIFOLEV: FIFOLevel register -----------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Number of 32 bits words of random available in the FIFO. Any write to this register clears the FifoFull 2ag in the Status register. */
} _TRNG_FIFOLEV_bits;

/* Bit field positions: */
#define TRNG_FIFOLEV_VAL_Pos                  0                      /*!< Number of 32 bits words of random available in the FIFO. Any write to this register clears the FifoFull 2ag in the Status register. */

/* Bit field masks: */
#define TRNG_FIFOLEV_VAL_Msk                  0xFFFFFFFFUL           /*!< Number of 32 bits words of random available in the FIFO. Any write to this register clears the FifoFull 2ag in the Status register. */

/*--  FIFOTHR: FIFOThreshold register -------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :7;                                /*!< FIFO level below which the module starts to refill the FIFO, expressed in number of 128bit blocks. */
} _TRNG_FIFOTHR_bits;

/* Bit field positions: */
#define TRNG_FIFOTHR_VAL_Pos                  0                      /*!< FIFO level below which the module starts to refill the FIFO, expressed in number of 128bit blocks. */

/* Bit field masks: */
#define TRNG_FIFOTHR_VAL_Msk                  0x0000007FUL           /*!< FIFO level below which the module starts to refill the FIFO, expressed in number of 128bit blocks. */

/*--  FIFODEP: FIFODepth register -----------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Maximum number of 32 bits words that can be stored in the FIFO */
} _TRNG_FIFODEP_bits;

/* Bit field positions: */
#define TRNG_FIFODEP_VAL_Pos                  0                      /*!< Maximum number of 32 bits words that can be stored in the FIFO */

/* Bit field masks: */
#define TRNG_FIFODEP_VAL_Msk                  0xFFFFFFFFUL           /*!< Maximum number of 32 bits words that can be stored in the FIFO */

/*--  KEY: Key register ---------------------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Bits form the 128-bit AES key */
} _TRNG_KEY_bits;

/* Bit field positions: */
#define TRNG_KEY_VAL_Pos                      0                      /*!< Bits form the 128-bit AES key */

/* Bit field masks: */
#define TRNG_KEY_VAL_Msk                      0xFFFFFFFFUL           /*!< Bits form the 128-bit AES key */

/*--  TESTDATA: Test data register ----------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Test data */
} _TRNG_TESTDATA_bits;

/* Bit field positions: */
#define TRNG_TESTDATA_VAL_Pos                 0                      /*!< Test data */

/* Bit field masks: */
#define TRNG_TESTDATA_VAL_Msk                 0xFFFFFFFFUL           /*!< Test data */

/*--  REPTSTCUTOFF: Cut-off value for the repetition tests register -------------------------------------------*/
typedef struct {
  uint32_t VAL                    :6;                                /*!< Cut-off value for the repetition tests */
} _TRNG_REPTSTCUTOFF_bits;

/* Bit field positions: */
#define TRNG_REPTSTCUTOFF_VAL_Pos             0                      /*!< Cut-off value for the repetition tests */

/* Bit field masks: */
#define TRNG_REPTSTCUTOFF_VAL_Msk             0x0000003FUL           /*!< Cut-off value for the repetition tests */

/*--  PROPTESTCUTOFF: Proportion test cut-off value register --------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :9;                                /*!< Proportion test cut-off value */
} _TRNG_PROPTESTCUTOFF_bits;

/* Bit field positions: */
#define TRNG_PROPTESTCUTOFF_VAL_Pos           0                      /*!< Proportion test cut-off value */

/* Bit field masks: */
#define TRNG_PROPTESTCUTOFF_VAL_Msk           0x000001FFUL           /*!< Proportion test cut-off value */

/*--  LFSRSEED: LFSRSeed register -----------------------------------------------------------------------------*/
typedef struct {
  uint32_t SEED                   :24;                               /*!< LFSR initialization value */
  uint32_t SEL                    :2;                                /*!< Share index for which initialization value should be used */
} _TRNG_LFSRSEED_bits;

/* Bit field positions: */
#define TRNG_LFSRSEED_SEED_Pos                0                      /*!< LFSR initialization value */
#define TRNG_LFSRSEED_SEL_Pos                 24                     /*!< Share index for which initialization value should be used */

/* Bit field masks: */
#define TRNG_LFSRSEED_SEED_Msk                0x00FFFFFFUL           /*!< LFSR initialization value */
#define TRNG_LFSRSEED_SEL_Msk                 0x03000000UL           /*!< Share index for which initialization value should be used */

/*--  STAT: Status register -----------------------------------------------------------------------------------*/
typedef struct {
  uint32_t DATABUSYTEST           :1;                                /*!< High when data written to TestData register is being processed */
  uint32_t STATE                  :3;                                /*!< State of the control FSM */
  uint32_t REPTESTFAIL            :1;                                /*!< NIST Repetition test(s) failure */
  uint32_t PROPTESTFAIL           :1;                                /*!< NIST Proportion test(s) failure */
  uint32_t ANYTESTFAIL            :1;                                /*!< Any of the enabled health tests is failing */
  uint32_t FIFOFULL               :1;                                /*!< FIFO full status */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t STARTUPFAIL            :1;                                /*!< Start-up test(s) failure */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t REPTSTFAILSH           :4;                                /*!< NIST Repetition test failure per share */
  uint32_t PRTSTFAILSH            :4;                                /*!< NIST Proportion test failure per share */
  uint32_t CONDSLOW               :1;                                /*!< Conditioning consumes data slower than they are provided to it */
} _TRNG_STAT_bits;

/* Bit field positions: */
#define TRNG_STAT_DATABUSYTEST_Pos            0                      /*!< High when data written to TestData register is being processed */
#define TRNG_STAT_STATE_Pos                   1                      /*!< State of the control FSM */
#define TRNG_STAT_REPTESTFAIL_Pos             4                      /*!< NIST Repetition test(s) failure */
#define TRNG_STAT_PROPTESTFAIL_Pos            5                      /*!< NIST Proportion test(s) failure */
#define TRNG_STAT_ANYTESTFAIL_Pos             6                      /*!< Any of the enabled health tests is failing */
#define TRNG_STAT_FIFOFULL_Pos                7                      /*!< FIFO full status */
#define TRNG_STAT_STARTUPFAIL_Pos             10                     /*!< Start-up test(s) failure */
#define TRNG_STAT_REPTSTFAILSH_Pos            12                     /*!< NIST Repetition test failure per share */
#define TRNG_STAT_PRTSTFAILSH_Pos             16                     /*!< NIST Proportion test failure per share */
#define TRNG_STAT_CONDSLOW_Pos                20                     /*!< Conditioning consumes data slower than they are provided to it */

/* Bit field masks: */
#define TRNG_STAT_DATABUSYTEST_Msk            0x00000001UL           /*!< High when data written to TestData register is being processed */
#define TRNG_STAT_STATE_Msk                   0x0000000EUL           /*!< State of the control FSM */
#define TRNG_STAT_REPTESTFAIL_Msk             0x00000010UL           /*!< NIST Repetition test(s) failure */
#define TRNG_STAT_PROPTESTFAIL_Msk            0x00000020UL           /*!< NIST Proportion test(s) failure */
#define TRNG_STAT_ANYTESTFAIL_Msk             0x00000040UL           /*!< Any of the enabled health tests is failing */
#define TRNG_STAT_FIFOFULL_Msk                0x00000080UL           /*!< FIFO full status */
#define TRNG_STAT_STARTUPFAIL_Msk             0x00000400UL           /*!< Start-up test(s) failure */
#define TRNG_STAT_REPTSTFAILSH_Msk            0x0000F000UL           /*!< NIST Repetition test failure per share */
#define TRNG_STAT_PRTSTFAILSH_Msk             0x000F0000UL           /*!< NIST Proportion test failure per share */
#define TRNG_STAT_CONDSLOW_Msk                0x00100000UL           /*!< Conditioning consumes data slower than they are provided to it */

/* Bit field enums: */
typedef enum {
  TRNG_STAT_STATE_Reset                      = 0x0UL,                /*!< State Reset */
  TRNG_STAT_STATE_StartUp                    = 0x1UL,                /*!< State Startup */
  TRNG_STAT_STATE_Idle_FIFOfull              = 0x2UL,                /*!< State Idle / FIFO full */
  TRNG_STAT_STATE_FIFOfill                   = 0x4UL,                /*!< State Fill FIFO */
  TRNG_STAT_STATE_Error                      = 0x5UL,                /*!< State Error */
} TRNG_STAT_STATE_Enum;

/*--  WARMPERIOD: WarmupPeriod register -----------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Number of clock cycles to wait during warm-up sequence */
} _TRNG_WARMPERIOD_bits;

/* Bit field positions: */
#define TRNG_WARMPERIOD_VAL_Pos               0                      /*!< Number of clock cycles to wait during warm-up sequence */

/* Bit field masks: */
#define TRNG_WARMPERIOD_VAL_Msk               0xFFFFFFFFUL           /*!< Number of clock cycles to wait during warm-up sequence */

/*--  DISOSC: DisableOsc registers ----------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :8;                                /*!< Disable oscillator rings 0 to size-1 */
} _TRNG_DISOSC_bits;

/* Bit field positions: */
#define TRNG_DISOSC_VAL_Pos                   0                      /*!< Disable oscillator rings 0 to size-1 */

/* Bit field masks: */
#define TRNG_DISOSC_VAL_Msk                   0x000000FFUL           /*!< Disable oscillator rings 0 to size-1 */

/*--  SAMPERIOD: SamplingPeriod register ----------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Number of clock cycles to wait between sampling moments */
} _TRNG_SAMPERIOD_bits;

/* Bit field positions: */
#define TRNG_SAMPERIOD_VAL_Pos                0                      /*!< Number of clock cycles to wait between sampling moments */

/* Bit field masks: */
#define TRNG_SAMPERIOD_VAL_Msk                0xFFFFFFFFUL           /*!< Number of clock cycles to wait between sampling moments */

/*--  HWCFG: HW Configuration register ------------------------------------------------------------------------*/
typedef struct {
  uint32_t INVCOUNT               :8;                                /*!< Generic g_NbOfInverters value */
  uint32_t AUTOCORRTEST           :4;                                /*!< Generic g_Log2NbOfAutoCorrTestsPerShare value */
  uint32_t FIFODEP                :4;                                /*!< Generic g_Log2FifoDepth value */
} _TRNG_HWCFG_bits;

/* Bit field positions: */
#define TRNG_HWCFG_INVCOUNT_Pos               0                      /*!< Generic g_NbOfInverters value */
#define TRNG_HWCFG_AUTOCORRTEST_Pos           8                      /*!< Generic g_Log2NbOfAutoCorrTestsPerShare value */
#define TRNG_HWCFG_FIFODEP_Pos                12                     /*!< Generic g_Log2FifoDepth value */

/* Bit field masks: */
#define TRNG_HWCFG_INVCOUNT_Msk               0x000000FFUL           /*!< Generic g_NbOfInverters value */
#define TRNG_HWCFG_AUTOCORRTEST_Msk           0x00000F00UL           /*!< Generic g_Log2NbOfAutoCorrTestsPerShare value */
#define TRNG_HWCFG_FIFODEP_Msk                0x0000F000UL           /*!< Generic g_Log2FifoDepth value */

/*--  COOLDPERIOD: CooldownPeriod register --------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Number of clock cycles to wait during cool-down sequence */
} _TRNG_COOLDPERIOD_bits;

/* Bit field positions: */
#define TRNG_COOLDPERIOD_VAL_Pos              0                      /*!< Number of clock cycles to wait during cool-down sequence */

/* Bit field masks: */
#define TRNG_COOLDPERIOD_VAL_Msk              0xFFFFFFFFUL           /*!< Number of clock cycles to wait during cool-down sequence */

/*--  AUTOCORRTESTCUTOFF0: AutoCorrTestCutoff register 0 ------------------------------------------------------*/
typedef struct {
  uint32_t ZEROCUTOFF             :16;                               /*!< Auto-correlation test cut-off value for delay of 0 samples */
  uint32_t ONECUTOFF              :16;                               /*!< Auto-correlation test cut-off value for delay of +1 sample */
} _TRNG_AUTOCORRTESTCUTOFF0_bits;

/* Bit field positions: */
#define TRNG_AUTOCORRTESTCUTOFF0_ZEROCUTOFF_Pos       0              /*!< Auto-correlation test cut-off value for delay of 0 samples */
#define TRNG_AUTOCORRTESTCUTOFF0_ONECUTOFF_Pos       16              /*!< Auto-correlation test cut-off value for delay of +1 sample */

/* Bit field masks: */
#define TRNG_AUTOCORRTESTCUTOFF0_ZEROCUTOFF_Msk       0x0000FFFFUL         /*!< Auto-correlation test cut-off value for delay of 0 samples */
#define TRNG_AUTOCORRTESTCUTOFF0_ONECUTOFF_Msk       0xFFFF0000UL         /*!< Auto-correlation test cut-off value for delay of +1 sample */

/*--  AUTOCORRTESTCUTOFF1: AutoCorrTestCutoff register 1 ------------------------------------------------------*/
typedef struct {
  uint32_t TWOCUTOFF              :16;                               /*!< Auto-correlation test cut-off value for delay +2 samples */
  uint32_t THREECUTOFF            :16;                               /*!< Auto-correlation test cut-off value for delay +3 samples */
} _TRNG_AUTOCORRTESTCUTOFF1_bits;

/* Bit field positions: */
#define TRNG_AUTOCORRTESTCUTOFF1_TWOCUTOFF_Pos       0               /*!< Auto-correlation test cut-off value for delay +2 samples */
#define TRNG_AUTOCORRTESTCUTOFF1_THREECUTOFF_Pos       16            /*!< Auto-correlation test cut-off value for delay +3 samples */

/* Bit field masks: */
#define TRNG_AUTOCORRTESTCUTOFF1_TWOCUTOFF_Msk       0x0000FFFFUL         /*!< Auto-correlation test cut-off value for delay +2 samples */
#define TRNG_AUTOCORRTESTCUTOFF1_THREECUTOFF_Msk       0xFFFF0000UL         /*!< Auto-correlation test cut-off value for delay +3 samples */

/*--  CORRTESTCUTOFF0: CorrTestCutoff register 0 --------------------------------------------------------------*/
typedef struct {
  uint32_t ZEROCUTOFF             :16;                               /*!< Correlation test cut-off value for time shift/delay of 0 samples */
  uint32_t ONECUTOFF              :16;                               /*!< Correlation test cut-off value for time shift/delay of +/-1 sample */
} _TRNG_CORRTESTCUTOFF0_bits;

/* Bit field positions: */
#define TRNG_CORRTESTCUTOFF0_ZEROCUTOFF_Pos       0                  /*!< Correlation test cut-off value for time shift/delay of 0 samples */
#define TRNG_CORRTESTCUTOFF0_ONECUTOFF_Pos       16                  /*!< Correlation test cut-off value for time shift/delay of +/-1 sample */

/* Bit field masks: */
#define TRNG_CORRTESTCUTOFF0_ZEROCUTOFF_Msk       0x0000FFFFUL         /*!< Correlation test cut-off value for time shift/delay of 0 samples */
#define TRNG_CORRTESTCUTOFF0_ONECUTOFF_Msk       0xFFFF0000UL         /*!< Correlation test cut-off value for time shift/delay of +/-1 sample */

/*--  CORRTESTCUTOFF1: CorrTestCutoff register 1 --------------------------------------------------------------*/
typedef struct {
  uint32_t TWOCUTOFF              :16;                               /*!< Correlation test cut-off value for time shift/delay of +/-2 samples */
  uint32_t THREECUTOFF            :16;                               /*!< Correlation test cut-off value for time shift/delay of +/-3 samples */
} _TRNG_CORRTESTCUTOFF1_bits;

/* Bit field positions: */
#define TRNG_CORRTESTCUTOFF1_TWOCUTOFF_Pos       0                   /*!< Correlation test cut-off value for time shift/delay of +/-2 samples */
#define TRNG_CORRTESTCUTOFF1_THREECUTOFF_Pos       16                /*!< Correlation test cut-off value for time shift/delay of +/-3 samples */

/* Bit field masks: */
#define TRNG_CORRTESTCUTOFF1_TWOCUTOFF_Msk       0x0000FFFFUL         /*!< Correlation test cut-off value for time shift/delay of +/-2 samples */
#define TRNG_CORRTESTCUTOFF1_THREECUTOFF_Msk       0xFFFF0000UL         /*!< Correlation test cut-off value for time shift/delay of +/-3 samples */

/*--  AUTOCORRTESTFAILED: AutoCorrTestFailed register ---------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :8;                                /*!< Auto-correlation test failing ring(s) */
} _TRNG_AUTOCORRTESTFAILED_bits;

/* Bit field positions: */
#define TRNG_AUTOCORRTESTFAILED_VAL_Pos       0                      /*!< Auto-correlation test failing ring(s) */

/* Bit field masks: */
#define TRNG_AUTOCORRTESTFAILED_VAL_Msk       0x000000FFUL           /*!< Auto-correlation test failing ring(s) */

/*--  CORRTESTFAILED: CorrTestFailed register -----------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :8;                                /*!< Correlation test failing rings */
} _TRNG_CORRTESTFAILED_bits;

/* Bit field positions: */
#define TRNG_CORRTESTFAILED_VAL_Pos           0                      /*!< Correlation test failing rings */

/* Bit field masks: */
#define TRNG_CORRTESTFAILED_VAL_Msk           0x000000FFUL           /*!< Correlation test failing rings */

/*--  HWVERSION: HW Version register --------------------------------------------------------------------------*/
typedef struct {
  uint32_t HWVER                  :1;                                /*!<  */
} _TRNG_HWVERSION_bits;

/* Bit field positions: */
#define TRNG_HWVERSION_HWVER_Pos              0                      /*!<  */

/* Bit field masks: */
#define TRNG_HWVERSION_HWVER_Msk              0x00000001UL           /*!<  */

/*--  FIFO: FIFO register -------------------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Result */
} _TRNG_FIFO_bits;

/* Bit field positions: */
#define TRNG_FIFO_VAL_Pos                     0                      /*!< Result */

/* Bit field masks: */
#define TRNG_FIFO_VAL_Msk                     0xFFFFFFFFUL           /*!< Result */

//Cluster KEY:
typedef struct {
  union { 
  /*!< Key register */
    __IO uint32_t KEY;                                               /*!< KEY : type used for word access */
    __IO _TRNG_KEY_bits KEY_bit;                                     /*!< KEY_bit: structure used for bit access */
  };
} _TRNG_KEY_TypeDef;
//Cluster FIFO:
typedef struct {
  union { 
  /*!< FIFO register */
    __I uint32_t FIFO;                                               /*!< FIFO : type used for word access */
    __I _TRNG_FIFO_bits FIFO_bit;                                    /*!< FIFO_bit: structure used for bit access */
  };
} _TRNG_FIFO_TypeDef;
typedef struct {
  union {                                                               /*!< Control register */
    __IO uint32_t CR;                                                /*!< CR    : type used for word access */
    __IO _TRNG_CR_bits  CR_bit;                                      /*!< CR_bit: structure used for bit access */
  };
  union {                                                               /*!< FIFOLevel register */
    __IO uint32_t FIFOLEV;                                           /*!< FIFOLEV    : type used for word access */
    __IO _TRNG_FIFOLEV_bits  FIFOLEV_bit;                            /*!< FIFOLEV_bit: structure used for bit access */
  };
  union {                                                               /*!< FIFOThreshold register */
    __IO uint32_t FIFOTHR;                                           /*!< FIFOTHR    : type used for word access */
    __IO _TRNG_FIFOTHR_bits  FIFOTHR_bit;                            /*!< FIFOTHR_bit: structure used for bit access */
  };
  union {                                                               /*!< FIFODepth register */
    __I uint32_t FIFODEP;                                            /*!< FIFODEP    : type used for word access */
    __I _TRNG_FIFODEP_bits  FIFODEP_bit;                             /*!< FIFODEP_bit: structure used for bit access */
  };
  _TRNG_KEY_TypeDef KEY[4];
  union {                                                               /*!< Test data register */
    __O uint32_t TESTDATA;                                           /*!< TESTDATA    : type used for word access */
    __O _TRNG_TESTDATA_bits  TESTDATA_bit;                           /*!< TESTDATA_bit: structure used for bit access */
  };
  union {                                                               /*!< Cut-off value for the repetition tests register */
    __IO uint32_t REPTSTCUTOFF;                                           /*!< REPTSTCUTOFF    : type used for word access */
    __IO _TRNG_REPTSTCUTOFF_bits  REPTSTCUTOFF_bit;                  /*!< REPTSTCUTOFF_bit: structure used for bit access */
  };
  union {                                                               /*!< Proportion test cut-off value register */
    __IO uint32_t PROPTESTCUTOFF;                                           /*!< PROPTESTCUTOFF    : type used for word access */
    __IO _TRNG_PROPTESTCUTOFF_bits  PROPTESTCUTOFF_bit;                /*!< PROPTESTCUTOFF_bit: structure used for bit access */
  };
  union {                                                               /*!< LFSRSeed register */
    __IO uint32_t LFSRSEED;                                           /*!< LFSRSEED    : type used for word access */
    __IO _TRNG_LFSRSEED_bits  LFSRSEED_bit;                          /*!< LFSRSEED_bit: structure used for bit access */
  };
  union {                                                               /*!< Status register */
    __IO uint32_t STAT;                                              /*!< STAT    : type used for word access */
    __IO _TRNG_STAT_bits  STAT_bit;                                  /*!< STAT_bit: structure used for bit access */
  };
  union {                                                               /*!< WarmupPeriod register */
    __IO uint32_t WARMPERIOD;                                           /*!< WARMPERIOD    : type used for word access */
    __IO _TRNG_WARMPERIOD_bits  WARMPERIOD_bit;                      /*!< WARMPERIOD_bit: structure used for bit access */
  };
  union {                                                               /*!< DisableOsc registers */
    __IO uint32_t DISOSC;                                            /*!< DISOSC    : type used for word access */
    __IO _TRNG_DISOSC_bits  DISOSC_bit;                              /*!< DISOSC_bit: structure used for bit access */
  };
    __IO uint32_t Reserved0[2];
  union {                                                               /*!< SamplingPeriod register */
    __IO uint32_t SAMPERIOD;                                           /*!< SAMPERIOD    : type used for word access */
    __IO _TRNG_SAMPERIOD_bits  SAMPERIOD_bit;                        /*!< SAMPERIOD_bit: structure used for bit access */
  };
    __IO uint32_t Reserved1[4];
  union {                                                               /*!< HW Configuration register */
    __I uint32_t HWCFG;                                              /*!< HWCFG    : type used for word access */
    __I _TRNG_HWCFG_bits  HWCFG_bit;                                 /*!< HWCFG_bit: structure used for bit access */
  };
  union {                                                               /*!< CooldownPeriod register */
    __IO uint32_t COOLDPERIOD;                                           /*!< COOLDPERIOD    : type used for word access */
    __IO _TRNG_COOLDPERIOD_bits  COOLDPERIOD_bit;                    /*!< COOLDPERIOD_bit: structure used for bit access */
  };
  union {                                                               /*!< AutoCorrTestCutoff register 0 */
    __IO uint32_t AUTOCORRTESTCUTOFF0;                                           /*!< AUTOCORRTESTCUTOFF0    : type used for word access */
    __IO _TRNG_AUTOCORRTESTCUTOFF0_bits  AUTOCORRTESTCUTOFF0_bit;                /*!< AUTOCORRTESTCUTOFF0_bit: structure used for bit access */
  };
  union {                                                               /*!< AutoCorrTestCutoff register 1 */
    __IO uint32_t AUTOCORRTESTCUTOFF1;                                           /*!< AUTOCORRTESTCUTOFF1    : type used for word access */
    __IO _TRNG_AUTOCORRTESTCUTOFF1_bits  AUTOCORRTESTCUTOFF1_bit;                /*!< AUTOCORRTESTCUTOFF1_bit: structure used for bit access */
  };
  union {                                                               /*!< CorrTestCutoff register 0 */
    __IO uint32_t CORRTESTCUTOFF0;                                           /*!< CORRTESTCUTOFF0    : type used for word access */
    __IO _TRNG_CORRTESTCUTOFF0_bits  CORRTESTCUTOFF0_bit;                /*!< CORRTESTCUTOFF0_bit: structure used for bit access */
  };
  union {                                                               /*!< CorrTestCutoff register 1 */
    __IO uint32_t CORRTESTCUTOFF1;                                           /*!< CORRTESTCUTOFF1    : type used for word access */
    __IO _TRNG_CORRTESTCUTOFF1_bits  CORRTESTCUTOFF1_bit;                /*!< CORRTESTCUTOFF1_bit: structure used for bit access */
  };
  union {                                                               /*!< AutoCorrTestFailed register */
    __I uint32_t AUTOCORRTESTFAILED;                                           /*!< AUTOCORRTESTFAILED    : type used for word access */
    __I _TRNG_AUTOCORRTESTFAILED_bits  AUTOCORRTESTFAILED_bit;                /*!< AUTOCORRTESTFAILED_bit: structure used for bit access */
  };
  union {                                                               /*!< CorrTestFailed register */
    __I uint32_t CORRTESTFAILED;                                           /*!< CORRTESTFAILED    : type used for word access */
    __I _TRNG_CORRTESTFAILED_bits  CORRTESTFAILED_bit;                /*!< CORRTESTFAILED_bit: structure used for bit access */
  };
    __IO uint32_t Reserved2;
  union {                                                               /*!< HW Version register */
    __I uint32_t HWVERSION;                                           /*!< HWVERSION    : type used for word access */
    __I _TRNG_HWVERSION_bits  HWVERSION_bit;                         /*!< HWVERSION_bit: structure used for bit access */
  };
  _TRNG_FIFO_TypeDef FIFO[32];
} TRNG_TypeDef;


/******************************************************************************/
/*                         CRC registers                                      */
/******************************************************************************/

/*--  DR: CRC Data register -----------------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Data register */
} _CRC_DR_bits;

/* Bit field positions: */
#define CRC_DR_VAL_Pos                        0                      /*!< Data register */

/* Bit field masks: */
#define CRC_DR_VAL_Msk                        0xFFFFFFFFUL           /*!< Data register */

/*--  POST: CRC post register ---------------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Post-processing CRC value */
} _CRC_POST_bits;

/* Bit field positions: */
#define CRC_POST_VAL_Pos                      0                      /*!< Post-processing CRC value */

/* Bit field masks: */
#define CRC_POST_VAL_Msk                      0xFFFFFFFFUL           /*!< Post-processing CRC value */

/*--  CR: CRC control register --------------------------------------------------------------------------------*/
typedef struct {
  uint32_t RESET                  :1;                                /*!< RESET bit */
  uint32_t MODE                   :1;                                /*!< Mode calc CRC */
  uint32_t XOROUT                 :1;                                /*!< XOR for data out */
  uint32_t POLYSIZE               :2;                                /*!< Polynomial size. These bits control the size of the polynomial */
  uint32_t REV_IN                 :2;                                /*!< Reverse input data. These bits control the reversal of the bit order of the input data */
  uint32_t REV_OUT                :1;                                /*!< Reverse output data */
} _CRC_CR_bits;

/* Bit field positions: */
#define CRC_CR_RESET_Pos                      0                      /*!< RESET bit */
#define CRC_CR_MODE_Pos                       1                      /*!< Mode calc CRC */
#define CRC_CR_XOROUT_Pos                     2                      /*!< XOR for data out */
#define CRC_CR_POLYSIZE_Pos                   3                      /*!< Polynomial size. These bits control the size of the polynomial
 */
#define CRC_CR_REV_IN_Pos                     5                      /*!< Reverse input data. These bits control the reversal of the bit order of the input data */
#define CRC_CR_REV_OUT_Pos                    7                      /*!< Reverse output data */

/* Bit field masks: */
#define CRC_CR_RESET_Msk                      0x00000001UL           /*!< RESET bit */
#define CRC_CR_MODE_Msk                       0x00000002UL           /*!< Mode calc CRC */
#define CRC_CR_XOROUT_Msk                     0x00000004UL           /*!< XOR for data out */
#define CRC_CR_POLYSIZE_Msk                   0x00000018UL           /*!< Polynomial size. These bits control the size of the polynomial
 */
#define CRC_CR_REV_IN_Msk                     0x00000060UL           /*!< Reverse input data. These bits control the reversal of the bit order of the input data */
#define CRC_CR_REV_OUT_Msk                    0x00000080UL           /*!< Reverse output data */

/* Bit field enums: */
typedef enum {
  CRC_CR_MODE_StandartCRC                    = 0x0UL,                /*!< Standart mode */
  CRC_CR_MODE_STM32_CRC                      = 0x1UL,                /*!< STM32 compatible mode */
} CRC_CR_MODE_Enum;

typedef enum {
  CRC_CR_XOROUT_Disable                      = 0x0UL,                /*!< Output XOR operation is disabled */
  CRC_CR_XOROUT_Enable                       = 0x1UL,                /*!< Output XOR operation is enabled */
} CRC_CR_XOROUT_Enum;

typedef enum {
  CRC_CR_POLYSIZE_POL32                      = 0x0UL,                /*!< 32 bit polynomial */
  CRC_CR_POLYSIZE_POL16                      = 0x1UL,                /*!< 16 bit polynomial */
  CRC_CR_POLYSIZE_POL8                       = 0x2UL,                /*!< 8 bit polynomial */
  CRC_CR_POLYSIZE_POL7                       = 0x3UL,                /*!< 7 bit polynomial */
} CRC_CR_POLYSIZE_Enum;

typedef enum {
  CRC_CR_REV_IN_Disable                      = 0x0UL,                /*!< Bit order not affected(default) */
  CRC_CR_REV_IN_REV_BYTE                     = 0x1UL,                /*!< Bit reversal done by byte */
  CRC_CR_REV_IN_REV_HALFWORD                 = 0x2UL,                /*!< Bit reversal done by half-word */
  CRC_CR_REV_IN_REV_WORD                     = 0x3UL,                /*!< Bit reversal done by word */
} CRC_CR_REV_IN_Enum;

typedef enum {
  CRC_CR_REV_OUT_Disable                     = 0x0UL,                /*!< Bit order not affected */
  CRC_CR_REV_OUT_Enable                      = 0x1UL,                /*!< Bit-reversed output format */
} CRC_CR_REV_OUT_Enum;

/*--  INIT: CRC initial value register ------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Initial CRC value */
} _CRC_INIT_bits;

/* Bit field positions: */
#define CRC_INIT_VAL_Pos                      0                      /*!< Initial CRC value */

/* Bit field masks: */
#define CRC_INIT_VAL_Msk                      0xFFFFFFFFUL           /*!< Initial CRC value */

/*--  POL: CRC polynomial register ----------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Programmable polynomial value */
} _CRC_POL_bits;

/* Bit field positions: */
#define CRC_POL_VAL_Pos                       0                      /*!< Programmable polynomial value */

/* Bit field masks: */
#define CRC_POL_VAL_Msk                       0xFFFFFFFFUL           /*!< Programmable polynomial value */

typedef struct {
  union {                                                           /*!< CRC Data register */
    __IO uint8_t  DR8;                                              /*!< DR8    : type used for byte access */
    __IO uint16_t DR16;                                             /*!< DR16  : type used for 16 bit access */  
    __IO uint32_t DR;                                               /*!< DR    : type used for 32 bit access */
  };
  union {                                                               /*!< CRC post register */
    __I uint32_t POST;                                               /*!< POST    : type used for word access */
    __I _CRC_POST_bits  POST_bit;                                    /*!< POST_bit: structure used for bit access */
  };
  union {                                                               /*!< CRC control register */
    __IO uint32_t CR;                                                /*!< CR    : type used for word access */
    __IO _CRC_CR_bits  CR_bit;                                       /*!< CR_bit: structure used for bit access */
  };
  union {                                                               /*!< CRC initial value register */
    __IO uint32_t INIT;                                              /*!< INIT    : type used for word access */
    __IO _CRC_INIT_bits  INIT_bit;                                   /*!< INIT_bit: structure used for bit access */
  };
  union {                                                               /*!< CRC polynomial register */
    __IO uint32_t POL;                                               /*!< POL    : type used for word access */
    __IO _CRC_POL_bits  POL_bit;                                     /*!< POL_bit: structure used for bit access */
  };
} CRC_TypeDef;


/******************************************************************************/
/*                         HASH registers                                     */
/******************************************************************************/

/*--  CR: Control register ------------------------------------------------------------------------------------*/
typedef struct {
  uint32_t INIT                   :1;                                /*!< Initialize message digest calculation */
  uint32_t MODE                   :1;                                /*!< Mode selection */
  uint32_t LKEY                   :1;                                /*!< Long key selection */
  uint32_t SAMK                   :1;                                /*!< Same key option */
  uint32_t DATATYPE               :2;                                /*!< Datatype selection */
  uint32_t DMAE                   :1;                                /*!< DMA enable */
  uint32_t MDMAT                  :1;                                /*!< Multiple DMA transfers */
  uint32_t NBW                    :4;                                /*!< Number of words already pushed */
  uint32_t DINNE                  :1;                                /*!< DIN not empty */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t ALGO                   :2;                                /*!< Algorithm selection */
} _HASH_CR_bits;

/* Bit field positions: */
#define HASH_CR_INIT_Pos                      0                      /*!< Initialize message digest calculation */
#define HASH_CR_MODE_Pos                      1                      /*!< Mode selection */
#define HASH_CR_LKEY_Pos                      2                      /*!< Long key selection */
#define HASH_CR_SAMK_Pos                      3                      /*!< Same key option */
#define HASH_CR_DATATYPE_Pos                  4                      /*!< Datatype selection */
#define HASH_CR_DMAE_Pos                      6                      /*!< DMA enable */
#define HASH_CR_MDMAT_Pos                     7                      /*!< Multiple DMA transfers */
#define HASH_CR_NBW_Pos                       8                      /*!< Number of words already pushed */
#define HASH_CR_DINNE_Pos                     12                     /*!< DIN not empty */
#define HASH_CR_ALGO_Pos                      14                     /*!< Algorithm selection */

/* Bit field masks: */
#define HASH_CR_INIT_Msk                      0x00000001UL           /*!< Initialize message digest calculation */
#define HASH_CR_MODE_Msk                      0x00000002UL           /*!< Mode selection */
#define HASH_CR_LKEY_Msk                      0x00000004UL           /*!< Long key selection */
#define HASH_CR_SAMK_Msk                      0x00000008UL           /*!< Same key option */
#define HASH_CR_DATATYPE_Msk                  0x00000030UL           /*!< Datatype selection */
#define HASH_CR_DMAE_Msk                      0x00000040UL           /*!< DMA enable */
#define HASH_CR_MDMAT_Msk                     0x00000080UL           /*!< Multiple DMA transfers */
#define HASH_CR_NBW_Msk                       0x00000F00UL           /*!< Number of words already pushed */
#define HASH_CR_DINNE_Msk                     0x00001000UL           /*!< DIN not empty */
#define HASH_CR_ALGO_Msk                      0x0000C000UL           /*!< Algorithm selection */

/* Bit field enums: */
typedef enum {
  HASH_CR_MODE_HASH                          = 0x0UL,                /*!< Hash mode selected */
  HASH_CR_MODE_HMAC                          = 0x1UL,                /*!< HMAC mode selected. LKEY must be set if the key being used is longer than 64 bytes. SAMK option for long key must be set if the keys used for inner/outer hash functions are identical */
} HASH_CR_MODE_Enum;

typedef enum {
  HASH_CR_LKEY_HMAC_SHORT_KEY                = 0x0UL,                /*!< Short key (? 64 bytes) */
  HASH_CR_LKEY_HMAC_LONG_KEY                 = 0x1UL,                /*!< Long key (> 64 bytes) */
} HASH_CR_LKEY_Enum;

typedef enum {
  HASH_CR_DATATYPE_WORD                      = 0x0UL,                /*!< 32-bit data. The data written into HASH_DIN are directly used by the HASH processing, without reordering */
  HASH_CR_DATATYPE_HWORD                     = 0x1UL,                /*!< 16-bit data, or half-word. The data written into HASH_DIN are considered as 2 half-words, and are swapped before being used by the HASH processing */
  HASH_CR_DATATYPE_BYTE                      = 0x2UL,                /*!< 8-bit data, or bytes. The data written into HASH_DIN are considered as 4 bytes, and are swapped before being used by the HASH processing */
  HASH_CR_DATATYPE_BIT                       = 0x3UL,                /*!< bit data, or bit-string. The data written into HASH_DIN are considered as 32bits (1st bit of the sting at position 0), and are swapped before being used by the HASH processing (1st bit of the string at position 31) */
} HASH_CR_DATATYPE_Enum;

typedef enum {
  HASH_CR_ALGO_SHA1                          = 0x0UL,                /*!< SHA-1 algorithm selected */
  HASH_CR_ALGO_MD5                           = 0x1UL,                /*!< MD5 algorithm selected */
  HASH_CR_ALGO_SHA224                        = 0x2UL,                /*!< SHA224 algorithm selected */
  HASH_CR_ALGO_SHA256                        = 0x3UL,                /*!< SHA256 algorithm selected */
} HASH_CR_ALGO_Enum;

/*--  DATAIN: Data input register -----------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Data input */
} _HASH_DATAIN_bits;

/* Bit field positions: */
#define HASH_DATAIN_VAL_Pos                   0                      /*!< Data input */

/* Bit field masks: */
#define HASH_DATAIN_VAL_Msk                   0xFFFFFFFFUL           /*!< Data input */

/*--  STR: Start register -------------------------------------------------------------------------------------*/
typedef struct {
  uint32_t NBLW                   :5;                                /*!< Number of valid bits in the last word of the message */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t DCAL                   :1;                                /*!< Digest calculation */
} _HASH_STR_bits;

/* Bit field positions: */
#define HASH_STR_NBLW_Pos                     0                      /*!< Number of valid bits in the last word of the message */
#define HASH_STR_DCAL_Pos                     8                      /*!< Digest calculation */

/* Bit field masks: */
#define HASH_STR_NBLW_Msk                     0x0000001FUL           /*!< Number of valid bits in the last word of the message */
#define HASH_STR_DCAL_Msk                     0x00000100UL           /*!< Digest calculation */

/*--  IMR: Interrupt enable register --------------------------------------------------------------------------*/
typedef struct {
  uint32_t DINIE                  :1;                                /*!< Data input interrupt enable */
  uint32_t DCIE                   :1;                                /*!< Digest calculation completion interrupt enable */
} _HASH_IMR_bits;

/* Bit field positions: */
#define HASH_IMR_DINIE_Pos                    0                      /*!< Data input interrupt enable */
#define HASH_IMR_DCIE_Pos                     1                      /*!< Digest calculation completion interrupt enable */

/* Bit field masks: */
#define HASH_IMR_DINIE_Msk                    0x00000001UL           /*!< Data input interrupt enable */
#define HASH_IMR_DCIE_Msk                     0x00000002UL           /*!< Digest calculation completion interrupt enable */

/*--  SR: Interrupt status register ---------------------------------------------------------------------------*/
typedef struct {
  uint32_t DINIS                  :1;                                /*!< Data input interrupt status */
  uint32_t DCIS                   :1;                                /*!< Digest calculation completion interrupt status */
  uint32_t DMAS                   :1;                                /*!< DMA status */
  uint32_t BUSY                   :1;                                /*!< Busy bit */
} _HASH_SR_bits;

/* Bit field positions: */
#define HASH_SR_DINIS_Pos                     0                      /*!< Data input interrupt status */
#define HASH_SR_DCIS_Pos                      1                      /*!< Digest calculation completion interrupt status */
#define HASH_SR_DMAS_Pos                      2                      /*!< DMA status */
#define HASH_SR_BUSY_Pos                      3                      /*!< Busy bit */

/* Bit field masks: */
#define HASH_SR_DINIS_Msk                     0x00000001UL           /*!< Data input interrupt status */
#define HASH_SR_DCIS_Msk                      0x00000002UL           /*!< Digest calculation completion interrupt status */
#define HASH_SR_DMAS_Msk                      0x00000004UL           /*!< DMA status */
#define HASH_SR_BUSY_Msk                      0x00000008UL           /*!< Busy bit */

/*--  HR: Hash digest register --------------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!<  */
} _HASH_HR_bits;

/* Bit field positions: */
#define HASH_HR_VAL_Pos                       0                      /*!<  */

/* Bit field masks: */
#define HASH_HR_VAL_Msk                       0xFFFFFFFFUL           /*!<  */

//Cluster HR:
typedef struct {
  union { 
  /*!< Hash digest register */
    __I uint32_t HR;                                                 /*!< HR : type used for word access */
    __I _HASH_HR_bits HR_bit;                                        /*!< HR_bit: structure used for bit access */
  };
} _HASH_HR_TypeDef;
typedef struct {
  union {                                                               /*!< Control register */
    __IO uint32_t CR;                                                /*!< CR    : type used for word access */
    __IO _HASH_CR_bits  CR_bit;                                      /*!< CR_bit: structure used for bit access */
  };
  union {                                                               /*!< Data input register */
    __IO uint32_t DATAIN;                                            /*!< DATAIN    : type used for word access */
    __IO _HASH_DATAIN_bits  DATAIN_bit;                              /*!< DATAIN_bit: structure used for bit access */
  };
  union {                                                               /*!< Start register */
    __IO uint32_t STR;                                               /*!< STR    : type used for word access */
    __IO _HASH_STR_bits  STR_bit;                                    /*!< STR_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt enable register */
    __IO uint32_t IMR;                                               /*!< IMR    : type used for word access */
    __IO _HASH_IMR_bits  IMR_bit;                                    /*!< IMR_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt status register */
    __O uint32_t SR;                                                 /*!< SR    : type used for word access */
    __O _HASH_SR_bits  SR_bit;                                       /*!< SR_bit: structure used for bit access */
  };
    __IO uint32_t Reserved0[11];
  _HASH_HR_TypeDef HR[8];
} HASH_TypeDef;


/******************************************************************************/
/*                         CRYPTO registers                                   */
/******************************************************************************/

/*-- IV: IV: Initialization vector register 0 -----------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Initialization vector register (word i).  Bits 32*(4-i)-1 to 32*(3-i) for AES_128, AES_256, Kuznechik / Bits 32*(2-i)-1 to 32*(1-i) for Magma (i < 2) */
} _CRYPTO_IV_IV__bits;

/* Bit field positions: */
#define CRYPTO_IV_IV__VAL_Pos                 0                      /*!< Initialization vector register (word i).  Bits 32*(4-i)-1 to 32*(3-i) for AES_128, AES_256, Kuznechik / Bits 32*(2-i)-1 to 32*(1-i) for Magma (i < 2) */

/* Bit field masks: */
#define CRYPTO_IV_IV__VAL_Msk                 0xFFFFFFFFUL           /*!< Initialization vector register (word i).  Bits 32*(4-i)-1 to 32*(3-i) for AES_128, AES_256, Kuznechik / Bits 32*(2-i)-1 to 32*(1-i) for Magma (i < 2) */

/*-- TEXT_IN: TEXT_IN: Input text regiter 0 (plain text / cipher text) ----------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Text in registers (word i).  Bits 32*(4-i)-1 to 32*(3-i) for AES_128, AES_256, Kuznechik / Bits 32*(2-i)-1 to 32*(1-i) for Magma (i < 2) */
} _CRYPTO_TEXT_IN_TEXT_IN__bits;

/* Bit field positions: */
#define CRYPTO_TEXT_IN_TEXT_IN__VAL_Pos       0                      /*!< Text in registers (word i).  Bits 32*(4-i)-1 to 32*(3-i) for AES_128, AES_256, Kuznechik / Bits 32*(2-i)-1 to 32*(1-i) for Magma (i < 2) */

/* Bit field masks: */
#define CRYPTO_TEXT_IN_TEXT_IN__VAL_Msk       0xFFFFFFFFUL           /*!< Text in registers (word i).  Bits 32*(4-i)-1 to 32*(3-i) for AES_128, AES_256, Kuznechik / Bits 32*(2-i)-1 to 32*(1-i) for Magma (i < 2) */

/*-- KEY: KEY: Secret key register 0 --------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Key registers (word i).  Bits 32*(8-i)-1 to 32*(7-i) for AES_256, Magma, Kuznechik / Bits 32*(4-i)-1 to 32*(3-i) for AES_128 (i < 4) */
} _CRYPTO_KEY_KEY__bits;

/* Bit field positions: */
#define CRYPTO_KEY_KEY__VAL_Pos               0                      /*!< Key registers (word i).  Bits 32*(8-i)-1 to 32*(7-i) for AES_256, Magma, Kuznechik / Bits 32*(4-i)-1 to 32*(3-i) for AES_128 (i < 4) */

/* Bit field masks: */
#define CRYPTO_KEY_KEY__VAL_Msk               0xFFFFFFFFUL           /*!< Key registers (word i).  Bits 32*(8-i)-1 to 32*(7-i) for AES_256, Magma, Kuznechik / Bits 32*(4-i)-1 to 32*(3-i) for AES_128 (i < 4) */

/*--  IRQ_ENABLE: Interrupt enable register -------------------------------------------------------------------*/
typedef struct {
  uint32_t DONE                   :1;                                /*!<  */
  uint32_t WR_IGNORED             :1;                                /*!<  */
  uint32_t DMA_DONE               :1;                                /*!<  */
  uint32_t DMA_FAIL               :1;                                /*!<  */
} _CRYPTO_IRQ_ENABLE_bits;

/* Bit field positions: */
#define CRYPTO_IRQ_ENABLE_DONE_Pos            0                      /*!<  */
#define CRYPTO_IRQ_ENABLE_WR_IGNORED_Pos       1                     /*!<  */
#define CRYPTO_IRQ_ENABLE_DMA_DONE_Pos        2                      /*!<  */
#define CRYPTO_IRQ_ENABLE_DMA_FAIL_Pos        3                      /*!<  */

/* Bit field masks: */
#define CRYPTO_IRQ_ENABLE_DONE_Msk            0x00000001UL           /*!<  */
#define CRYPTO_IRQ_ENABLE_WR_IGNORED_Msk       0x00000002UL          /*!<  */
#define CRYPTO_IRQ_ENABLE_DMA_DONE_Msk        0x00000004UL           /*!<  */
#define CRYPTO_IRQ_ENABLE_DMA_FAIL_Msk        0x00000008UL           /*!<  */

/*--  CONTROL: Operation control register ---------------------------------------------------------------------*/
typedef struct {
  uint32_t UPDATE_KEY             :1;                                /*!<  */
  uint32_t START                  :1;                                /*!<  */
  uint32_t DIRECTION              :1;                                /*!<  */
  uint32_t ALGORITHM              :2;                                /*!<  */
  uint32_t MODE                   :2;                                /*!<  */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t SELF_UPDATE            :1;                                /*!<  */
  uint32_t GCM_PHASE              :2;                                /*!<  */
} _CRYPTO_CONTROL_bits;

/* Bit field positions: */
#define CRYPTO_CONTROL_UPDATE_KEY_Pos         0                      /*!<  */
#define CRYPTO_CONTROL_START_Pos              1                      /*!<  */
#define CRYPTO_CONTROL_DIRECTION_Pos          2                      /*!<  */
#define CRYPTO_CONTROL_ALGORITHM_Pos          3                      /*!<  */
#define CRYPTO_CONTROL_MODE_Pos               5                      /*!<  */
#define CRYPTO_CONTROL_SELF_UPDATE_Pos        8                      /*!<  */
#define CRYPTO_CONTROL_GCM_PHASE_Pos          9                      /*!<  */

/* Bit field masks: */
#define CRYPTO_CONTROL_UPDATE_KEY_Msk         0x00000001UL           /*!<  */
#define CRYPTO_CONTROL_START_Msk              0x00000002UL           /*!<  */
#define CRYPTO_CONTROL_DIRECTION_Msk          0x00000004UL           /*!<  */
#define CRYPTO_CONTROL_ALGORITHM_Msk          0x00000018UL           /*!<  */
#define CRYPTO_CONTROL_MODE_Msk               0x00000060UL           /*!<  */
#define CRYPTO_CONTROL_SELF_UPDATE_Msk        0x00000100UL           /*!<  */
#define CRYPTO_CONTROL_GCM_PHASE_Msk          0x00000600UL           /*!<  */

/* Bit field enums: */
typedef enum {
  CRYPTO_CONTROL_DIRECTION_Encrypt           = 0x0UL,                /*!< Do encryption */
  CRYPTO_CONTROL_DIRECTION_Decrypt           = 0x1UL,                /*!< Do decryption */
} CRYPTO_CONTROL_DIRECTION_Enum;

typedef enum {
  CRYPTO_CONTROL_ALGORITHM_AES_128           = 0x0UL,                /*!< AES 128 bit key */
  CRYPTO_CONTROL_ALGORITHM_AES_256           = 0x1UL,                /*!< AES 256 bit key */
  CRYPTO_CONTROL_ALGORITHM_Magma             = 0x2UL,                /*!< GCM payload phase */
  CRYPTO_CONTROL_ALGORITHM_Kuznechik         = 0x3UL,                /*!< GOST 34.12-2018 cipher with 128 bit block (Kuznechik) */
} CRYPTO_CONTROL_ALGORITHM_Enum;

typedef enum {
  CRYPTO_CONTROL_MODE_ECB                    = 0x0UL,                /*!< Electronic Codebook mode (ECB) */
  CRYPTO_CONTROL_MODE_CBC                    = 0x1UL,                /*!< Cipher Block Chaining mode (CBC) */
  CRYPTO_CONTROL_MODE_CTR                    = 0x2UL,                /*!< Counter mode (CTR) */
  CRYPTO_CONTROL_MODE_GCM                    = 0x3UL,                /*!< Galois/Counter mode (GCM) */
} CRYPTO_CONTROL_MODE_Enum;

typedef enum {
  CRYPTO_CONTROL_GCM_PHASE_GCM_INIT          = 0x0UL,                /*!< GCM initialization phase */
  CRYPTO_CONTROL_GCM_PHASE_GCM_HEADER        = 0x1UL,                /*!< GCM header phase */
  CRYPTO_CONTROL_GCM_PHASE_GCM_PAYLOAD       = 0x2UL,                /*!< GCM payload phase */
  CRYPTO_CONTROL_GCM_PHASE_GCM_LAST_BLOCK    = 0x3UL,                /*!< GCM last block phase */
} CRYPTO_CONTROL_GCM_PHASE_Enum;

/*--  BASE_DESCRIPTOR: Register of address of base descriptor -------------------------------------------------*/
typedef struct {
  uint32_t ZEROES                 :4;                                /*!<  */
  uint32_t ADDRESS                :28;                               /*!<  */
} _CRYPTO_BASE_DESCRIPTOR_bits;

/* Bit field positions: */
#define CRYPTO_BASE_DESCRIPTOR_ZEROES_Pos       0                    /*!<  */
#define CRYPTO_BASE_DESCRIPTOR_ADDRESS_Pos       4                   /*!<  */

/* Bit field masks: */
#define CRYPTO_BASE_DESCRIPTOR_ZEROES_Msk       0x0000000FUL         /*!<  */
#define CRYPTO_BASE_DESCRIPTOR_ADDRESS_Msk       0xFFFFFFF0UL         /*!<  */

/*--  DMA_CONTROL: DMA operations control register ------------------------------------------------------------*/
typedef struct {
  uint32_t START                  :1;                                /*!<  */
  uint32_t BYTES_SWAP             :1;                                /*!<  */
  uint32_t WORDS_SWAP             :1;                                /*!<  */
} _CRYPTO_DMA_CONTROL_bits;

/* Bit field positions: */
#define CRYPTO_DMA_CONTROL_START_Pos          0                      /*!<  */
#define CRYPTO_DMA_CONTROL_BYTES_SWAP_Pos       1                    /*!<  */
#define CRYPTO_DMA_CONTROL_WORDS_SWAP_Pos       2                    /*!<  */

/* Bit field masks: */
#define CRYPTO_DMA_CONTROL_START_Msk          0x00000001UL           /*!<  */
#define CRYPTO_DMA_CONTROL_BYTES_SWAP_Msk       0x00000002UL         /*!<  */
#define CRYPTO_DMA_CONTROL_WORDS_SWAP_Msk       0x00000004UL         /*!<  */

/*--  TERMINATE: Operation emergency terminate register (write of 0xD0 terminate current oeparation) --------------------------------*/
typedef struct {
  uint32_t CODE                   :8;                                /*!<  */
} _CRYPTO_TERMINATE_bits;

/* Bit field positions: */
#define CRYPTO_TERMINATE_CODE_Pos             0                      /*!<  */

/* Bit field masks: */
#define CRYPTO_TERMINATE_CODE_Msk             0x000000FFUL           /*!<  */

/*--  IRQ: Interrupts pending register ------------------------------------------------------------------------*/
typedef struct {
  uint32_t DONE                   :1;                                /*!<  */
  uint32_t WR_IGNORED             :1;                                /*!<  */
  uint32_t DMA_DONE               :1;                                /*!<  */
  uint32_t DMA_FAIL               :1;                                /*!<  */
} _CRYPTO_IRQ_bits;

/* Bit field positions: */
#define CRYPTO_IRQ_DONE_Pos                   0                      /*!<  */
#define CRYPTO_IRQ_WR_IGNORED_Pos             1                      /*!<  */
#define CRYPTO_IRQ_DMA_DONE_Pos               2                      /*!<  */
#define CRYPTO_IRQ_DMA_FAIL_Pos               3                      /*!<  */

/* Bit field masks: */
#define CRYPTO_IRQ_DONE_Msk                   0x00000001UL           /*!<  */
#define CRYPTO_IRQ_WR_IGNORED_Msk             0x00000002UL           /*!<  */
#define CRYPTO_IRQ_DMA_DONE_Msk               0x00000004UL           /*!<  */
#define CRYPTO_IRQ_DMA_FAIL_Msk               0x00000008UL           /*!<  */

/*-- TEXT_OUT: TEXT_OUT: Output text regiter 0 (cipher text / plain text) -------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Text out registers (word i).  Bits 32*(4-i)-1 to 32*(3-i) for AES_128, AES_256, Kuznechik / Bits 32*(2-i)-1 to 32*(1-i) for Magma (i < 2) */
} _CRYPTO_TEXT_OUT_TEXT_OUT__bits;

/* Bit field positions: */
#define CRYPTO_TEXT_OUT_TEXT_OUT__VAL_Pos       0                    /*!< Text out registers (word i).  Bits 32*(4-i)-1 to 32*(3-i) for AES_128, AES_256, Kuznechik / Bits 32*(2-i)-1 to 32*(1-i) for Magma (i < 2) */

/* Bit field masks: */
#define CRYPTO_TEXT_OUT_TEXT_OUT__VAL_Msk       0xFFFFFFFFUL         /*!< Text out registers (word i).  Bits 32*(4-i)-1 to 32*(3-i) for AES_128, AES_256, Kuznechik / Bits 32*(2-i)-1 to 32*(1-i) for Magma (i < 2) */

/*--  STATUS: Operation status register -----------------------------------------------------------------------*/
typedef struct {
  uint32_t READY                  :1;                                /*!<  */
  uint32_t KEYS_READY             :1;                                /*!<  */
  uint32_t OUT_VALID              :1;                                /*!<  */
  uint32_t IRQ_PENDING            :1;                                /*!<  */
  uint32_t KEYS_STORED            :4;                                /*!<  */
  uint32_t DMA_ACTIVE             :1;                                /*!<  */
  uint32_t AHB_ERROR              :3;                                /*!<  */
  uint32_t BAD_DESCR              :4;                                /*!<  */
  uint32_t DMA_COUNTER            :12;                               /*!<  */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t DMA_LAST               :1;                                /*!<  */
} _CRYPTO_STATUS_bits;

/* Bit field positions: */
#define CRYPTO_STATUS_READY_Pos               0                      /*!<  */
#define CRYPTO_STATUS_KEYS_READY_Pos          1                      /*!<  */
#define CRYPTO_STATUS_OUT_VALID_Pos           2                      /*!<  */
#define CRYPTO_STATUS_IRQ_PENDING_Pos         3                      /*!<  */
#define CRYPTO_STATUS_KEYS_STORED_Pos         4                      /*!<  */
#define CRYPTO_STATUS_DMA_ACTIVE_Pos          8                      /*!<  */
#define CRYPTO_STATUS_AHB_ERROR_Pos           9                      /*!<  */
#define CRYPTO_STATUS_BAD_DESCR_Pos           12                     /*!<  */
#define CRYPTO_STATUS_DMA_COUNTER_Pos         16                     /*!<  */
#define CRYPTO_STATUS_DMA_LAST_Pos            31                     /*!<  */

/* Bit field masks: */
#define CRYPTO_STATUS_READY_Msk               0x00000001UL           /*!<  */
#define CRYPTO_STATUS_KEYS_READY_Msk          0x00000002UL           /*!<  */
#define CRYPTO_STATUS_OUT_VALID_Msk           0x00000004UL           /*!<  */
#define CRYPTO_STATUS_IRQ_PENDING_Msk         0x00000008UL           /*!<  */
#define CRYPTO_STATUS_KEYS_STORED_Msk         0x000000F0UL           /*!<  */
#define CRYPTO_STATUS_DMA_ACTIVE_Msk          0x00000100UL           /*!<  */
#define CRYPTO_STATUS_AHB_ERROR_Msk           0x00000E00UL           /*!<  */
#define CRYPTO_STATUS_BAD_DESCR_Msk           0x0000F000UL           /*!<  */
#define CRYPTO_STATUS_DMA_COUNTER_Msk         0x0FFF0000UL           /*!<  */
#define CRYPTO_STATUS_DMA_LAST_Msk            0x80000000UL           /*!<  */

/*--  CURRENT_DESCRIPTOR: Register of address of current handling descriptor ----------------------------------*/
typedef struct {
  uint32_t ZEROES                 :4;                                /*!<  */
  uint32_t ADDRESS                :28;                               /*!<  */
} _CRYPTO_CURRENT_DESCRIPTOR_bits;

/* Bit field positions: */
#define CRYPTO_CURRENT_DESCRIPTOR_ZEROES_Pos       0                 /*!<  */
#define CRYPTO_CURRENT_DESCRIPTOR_ADDRESS_Pos       4                /*!<  */

/* Bit field masks: */
#define CRYPTO_CURRENT_DESCRIPTOR_ZEROES_Msk       0x0000000FUL         /*!<  */
#define CRYPTO_CURRENT_DESCRIPTOR_ADDRESS_Msk       0xFFFFFFF0UL         /*!<  */

/*--  NEXT_DESCRIPTOR: Register of address of next descriptor -------------------------------------------------*/
typedef struct {
  uint32_t ZEROES                 :4;                                /*!<  */
  uint32_t ADDRESS                :28;                               /*!<  */
} _CRYPTO_NEXT_DESCRIPTOR_bits;

/* Bit field positions: */
#define CRYPTO_NEXT_DESCRIPTOR_ZEROES_Pos       0                    /*!<  */
#define CRYPTO_NEXT_DESCRIPTOR_ADDRESS_Pos       4                   /*!<  */

/* Bit field masks: */
#define CRYPTO_NEXT_DESCRIPTOR_ZEROES_Msk       0x0000000FUL         /*!<  */
#define CRYPTO_NEXT_DESCRIPTOR_ADDRESS_Msk       0xFFFFFFF0UL         /*!<  */

//Cluster IV:
typedef struct {
  union { 
  /*!< Initialization vector register 0 */
    __IO uint32_t IV_;                                               /*!< IV_ : type used for word access */
    __IO _CRYPTO_IV_IV__bits IV__bit;                                /*!< IV__bit: structure used for bit access */
  };
} _CRYPTO_IV_TypeDef;
//Cluster TEXT_IN:
typedef struct {
  union { 
  /*!< Input text regiter 0 (plain text / cipher text) */
    __IO uint32_t TEXT_IN_;                                           /*!< TEXT_IN_ : type used for word access */
    __IO _CRYPTO_TEXT_IN_TEXT_IN__bits TEXT_IN__bit;                 /*!< TEXT_IN__bit: structure used for bit access */
  };
} _CRYPTO_TEXT_IN_TypeDef;
//Cluster KEY:
typedef struct {
  union { 
  /*!< Secret key register 0 */
    __IO uint32_t KEY_;                                              /*!< KEY_ : type used for word access */
    __IO _CRYPTO_KEY_KEY__bits KEY__bit;                             /*!< KEY__bit: structure used for bit access */
  };
} _CRYPTO_KEY_TypeDef;
//Cluster TEXT_OUT:
typedef struct {
  union { 
  /*!< Output text regiter 0 (cipher text / plain text) */
    __I uint32_t TEXT_OUT_;                                           /*!< TEXT_OUT_ : type used for word access */
    __I _CRYPTO_TEXT_OUT_TEXT_OUT__bits TEXT_OUT__bit;               /*!< TEXT_OUT__bit: structure used for bit access */
  };
} _CRYPTO_TEXT_OUT_TypeDef;

typedef struct {
  __IO uint32_t IV[4];
  __IO uint32_t TEXT_IN[4];
  __IO uint32_t KEY[8];
  union {                                                               /*!< Interrupt enable register */
    __IO uint32_t IRQ_ENABLE;                                           /*!< IRQ_ENABLE    : type used for word access */
    __IO _CRYPTO_IRQ_ENABLE_bits  IRQ_ENABLE_bit;                    /*!< IRQ_ENABLE_bit: structure used for bit access */
  };
  union {                                                               /*!< Operation control register */
    __IO uint32_t CONTROL;                                           /*!< CONTROL    : type used for word access */
    __IO _CRYPTO_CONTROL_bits  CONTROL_bit;                          /*!< CONTROL_bit: structure used for bit access */
  };
  union {                                                               /*!< Register of address of base descriptor */
    __IO uint32_t BASE_DESCRIPTOR;                                           /*!< BASE_DESCRIPTOR    : type used for word access */
    __IO _CRYPTO_BASE_DESCRIPTOR_bits  BASE_DESCRIPTOR_bit;                /*!< BASE_DESCRIPTOR_bit: structure used for bit access */
  };
  union {                                                               /*!< DMA operations control register */
    __IO uint32_t DMA_CONTROL;                                           /*!< DMA_CONTROL    : type used for word access */
    __IO _CRYPTO_DMA_CONTROL_bits  DMA_CONTROL_bit;                  /*!< DMA_CONTROL_bit: structure used for bit access */
  };
  union {                                                               /*!< Operation emergency terminate register (write of 0xD0 terminate current oeparation) */
    __O uint32_t TERMINATE;                                           /*!< TERMINATE    : type used for word access */
    __O _CRYPTO_TERMINATE_bits  TERMINATE_bit;                       /*!< TERMINATE_bit: structure used for bit access */
  };
    __IO uint32_t Reserved0[3];
  union {                                                               /*!< Interrupts pending register */
    __IO uint32_t IRQ;                                               /*!< IRQ    : type used for word access */
    __IO _CRYPTO_IRQ_bits  IRQ_bit;                                  /*!< IRQ_bit: structure used for bit access */
  };
    __IO uint32_t Reserved1[3];
  __I uint32_t TEXT_OUT[4];
  union {                                                               /*!< Operation status register */
    __I uint32_t STATUS;                                             /*!< STATUS    : type used for word access */
    __I _CRYPTO_STATUS_bits  STATUS_bit;                             /*!< STATUS_bit: structure used for bit access */
  };
  union {                                                               /*!< Register of address of current handling descriptor */
    __I uint32_t CURRENT_DESCRIPTOR;                                           /*!< CURRENT_DESCRIPTOR    : type used for word access */
    __I _CRYPTO_CURRENT_DESCRIPTOR_bits  CURRENT_DESCRIPTOR_bit;                /*!< CURRENT_DESCRIPTOR_bit: structure used for bit access */
  };
  union {                                                               /*!< Register of address of next descriptor */
    __I uint32_t NEXT_DESCRIPTOR;                                           /*!< NEXT_DESCRIPTOR    : type used for word access */
    __I _CRYPTO_NEXT_DESCRIPTOR_bits  NEXT_DESCRIPTOR_bit;                /*!< NEXT_DESCRIPTOR_bit: structure used for bit access */
  };
  __IO uint32_t Reserved2;
  __IO uint32_t GCM_HASH[4];                                            /*!< Register GCM HASH */
  __IO uint32_t GCM_TAG[4];                                             /*!< Register GCM TAG */
} CRYPTO_TypeDef;


/******************************************************************************/
/*                         TSENS registers                                    */
/******************************************************************************/

/*--  CTRL: TSENS control register ----------------------------------------------------------------------------*/
typedef struct {
  uint32_t EN                     :1;                                /*!< Enable temperatire sensor */
  uint32_t ADCSEL                 :1;                                /*!< ADC select */
  uint32_t STYP                   :1;                                /*!< Sense type select */
  uint32_t ISWAP                  :1;                                /*!<  */
  uint32_t ISEL                   :1;                                /*!< Current source */
  uint32_t ZS                     :1;                                /*!< Zero sample for calibration */
} _TSENS_CTRL_bits;

/* Bit field positions: */
#define TSENS_CTRL_EN_Pos                     0                      /*!< Enable temperatire sensor */
#define TSENS_CTRL_ADCSEL_Pos                 1                      /*!< ADC select */
#define TSENS_CTRL_STYP_Pos                   2                      /*!< Sense type select */
#define TSENS_CTRL_ISWAP_Pos                  3                      /*!<  */
#define TSENS_CTRL_ISEL_Pos                   4                      /*!< Current source */
#define TSENS_CTRL_ZS_Pos                     5                      /*!< Zero sample for calibration */

/* Bit field masks: */
#define TSENS_CTRL_EN_Msk                     0x00000001UL           /*!< Enable temperatire sensor */
#define TSENS_CTRL_ADCSEL_Msk                 0x00000002UL           /*!< ADC select */
#define TSENS_CTRL_STYP_Msk                   0x00000004UL           /*!< Sense type select */
#define TSENS_CTRL_ISWAP_Msk                  0x00000008UL           /*!<  */
#define TSENS_CTRL_ISEL_Msk                   0x00000010UL           /*!< Current source */
#define TSENS_CTRL_ZS_Msk                     0x00000020UL           /*!< Zero sample for calibration */

typedef struct {
  union {                                                               /*!< TSENS control register */
    __IO uint32_t CTRL;                                              /*!< CTRL    : type used for word access */
    __IO _TSENS_CTRL_bits  CTRL_bit;                                 /*!< CTRL_bit: structure used for bit access */
  };
} TSENS_TypeDef;


/******************************************************************************/
/*                         ADCSAR registers                                   */
/******************************************************************************/

/*--  SEQEN: Enable sequencer register ------------------------------------------------------------------------*/
typedef struct {
  uint32_t SEQEN0                 :1;                                /*!< Enable sequencer 0 */
  uint32_t SEQEN1                 :1;                                /*!< Enable sequencer 1 */
} _ADCSAR_SEQEN_bits;

/* Bit field positions: */
#define ADCSAR_SEQEN_SEQEN0_Pos               0                      /*!< Enable sequencer 0 */
#define ADCSAR_SEQEN_SEQEN1_Pos               1                      /*!< Enable sequencer 1 */

/* Bit field masks: */
#define ADCSAR_SEQEN_SEQEN0_Msk               0x00000001UL           /*!< Enable sequencer 0 */
#define ADCSAR_SEQEN_SEQEN1_Msk               0x00000002UL           /*!< Enable sequencer 1 */

/*--  SEQSYNC: Sequencer sync register ------------------------------------------------------------------------*/
typedef struct {
  uint32_t SYNC0                  :1;                                /*!< Enable sequencer 0 software sync */
  uint32_t SYNC1                  :1;                                /*!< Enable sequencer 1 software sync */
  uint32_t                        :29;                               /*!< RESERVED */
  uint32_t GSYNC                  :1;                                /*!< Sync all sequencers */
} _ADCSAR_SEQSYNC_bits;

/* Bit field positions: */
#define ADCSAR_SEQSYNC_SYNC0_Pos              0                      /*!< Enable sequencer 0 software sync */
#define ADCSAR_SEQSYNC_SYNC1_Pos              1                      /*!< Enable sequencer 1 software sync */
#define ADCSAR_SEQSYNC_GSYNC_Pos              31                     /*!< Sync all sequencers */

/* Bit field masks: */
#define ADCSAR_SEQSYNC_SYNC0_Msk              0x00000001UL           /*!< Enable sequencer 0 software sync */
#define ADCSAR_SEQSYNC_SYNC1_Msk              0x00000002UL           /*!< Enable sequencer 1 software sync */
#define ADCSAR_SEQSYNC_GSYNC_Msk              0x80000000UL           /*!< Sync all sequencers */

/*--  FSTAT: FIFO overflow status register --------------------------------------------------------------------*/
typedef struct {
  uint32_t OV0                    :1;                                /*!< Sequencer 0 FIFO overflow */
  uint32_t OV1                    :1;                                /*!< Sequencer 1 FIFO overflow */
  uint32_t                        :6;                                /*!< RESERVED */
  uint32_t UN0                    :1;                                /*!< Sequencer 0 FIFO underflow */
  uint32_t UN1                    :1;                                /*!< Sequencer 1 FIFO underflow */
  uint32_t                        :6;                                /*!< RESERVED */
  uint32_t DOV0                   :1;                                /*!< Sequencer 0 FIFO DMA request overflow */
  uint32_t DOV1                   :1;                                /*!< Sequencer 1 FIFO DMA request overflow */
} _ADCSAR_FSTAT_bits;

/* Bit field positions: */
#define ADCSAR_FSTAT_OV0_Pos                  0                      /*!< Sequencer 0 FIFO overflow */
#define ADCSAR_FSTAT_OV1_Pos                  1                      /*!< Sequencer 1 FIFO overflow */
#define ADCSAR_FSTAT_UN0_Pos                  8                      /*!< Sequencer 0 FIFO underflow */
#define ADCSAR_FSTAT_UN1_Pos                  9                      /*!< Sequencer 1 FIFO underflow */
#define ADCSAR_FSTAT_DOV0_Pos                 16                     /*!< Sequencer 0 FIFO DMA request overflow */
#define ADCSAR_FSTAT_DOV1_Pos                 17                     /*!< Sequencer 1 FIFO DMA request overflow */

/* Bit field masks: */
#define ADCSAR_FSTAT_OV0_Msk                  0x00000001UL           /*!< Sequencer 0 FIFO overflow */
#define ADCSAR_FSTAT_OV1_Msk                  0x00000002UL           /*!< Sequencer 1 FIFO overflow */
#define ADCSAR_FSTAT_UN0_Msk                  0x00000100UL           /*!< Sequencer 0 FIFO underflow */
#define ADCSAR_FSTAT_UN1_Msk                  0x00000200UL           /*!< Sequencer 1 FIFO underflow */
#define ADCSAR_FSTAT_DOV0_Msk                 0x00010000UL           /*!< Sequencer 0 FIFO DMA request overflow */
#define ADCSAR_FSTAT_DOV1_Msk                 0x00020000UL           /*!< Sequencer 1 FIFO DMA request overflow */

/*--  BSTAT: Busy status register -----------------------------------------------------------------------------*/
typedef struct {
  uint32_t SEQBUSY0               :1;                                /*!< Sequencer 0 busy */
  uint32_t SEQBUSY1               :1;                                /*!< Sequencer 1 busy */
  uint32_t                        :14;                               /*!< RESERVED */
  uint32_t ADCBUSY                :1;                                /*!< ADC module conversion busy */
} _ADCSAR_BSTAT_bits;

/* Bit field positions: */
#define ADCSAR_BSTAT_SEQBUSY0_Pos             0                      /*!< Sequencer 0 busy */
#define ADCSAR_BSTAT_SEQBUSY1_Pos             1                      /*!< Sequencer 1 busy */
#define ADCSAR_BSTAT_ADCBUSY_Pos              16                     /*!< ADC module conversion busy */

/* Bit field masks: */
#define ADCSAR_BSTAT_SEQBUSY0_Msk             0x00000001UL           /*!< Sequencer 0 busy */
#define ADCSAR_BSTAT_SEQBUSY1_Msk             0x00000002UL           /*!< Sequencer 1 busy */
#define ADCSAR_BSTAT_ADCBUSY_Msk              0x00010000UL           /*!< ADC module conversion busy */

/*--  DCTRIG: Digital comparator output trigger status register -----------------------------------------------*/
typedef struct {
  uint32_t TOS0                   :1;                                /*!< DC 0 output trigger status */
  uint32_t TOS1                   :1;                                /*!< DC 1 output trigger status */
  uint32_t TOS2                   :1;                                /*!< DC 2 output trigger status */
  uint32_t TOS3                   :1;                                /*!< DC 3 output trigger status */
  uint32_t TOS4                   :1;                                /*!< DC 4 output trigger status */
  uint32_t TOS5                   :1;                                /*!< DC 5 output trigger status */
  uint32_t TOS6                   :1;                                /*!< DC 6 output trigger status */
  uint32_t TOS7                   :1;                                /*!< DC 7 output trigger status */
} _ADCSAR_DCTRIG_bits;

/* Bit field positions: */
#define ADCSAR_DCTRIG_TOS0_Pos                0                      /*!< DC 0 output trigger status */
#define ADCSAR_DCTRIG_TOS1_Pos                1                      /*!< DC 1 output trigger status */
#define ADCSAR_DCTRIG_TOS2_Pos                2                      /*!< DC 2 output trigger status */
#define ADCSAR_DCTRIG_TOS3_Pos                3                      /*!< DC 3 output trigger status */
#define ADCSAR_DCTRIG_TOS4_Pos                4                      /*!< DC 4 output trigger status */
#define ADCSAR_DCTRIG_TOS5_Pos                5                      /*!< DC 5 output trigger status */
#define ADCSAR_DCTRIG_TOS6_Pos                6                      /*!< DC 6 output trigger status */
#define ADCSAR_DCTRIG_TOS7_Pos                7                      /*!< DC 7 output trigger status */

/* Bit field masks: */
#define ADCSAR_DCTRIG_TOS0_Msk                0x00000001UL           /*!< DC 0 output trigger status */
#define ADCSAR_DCTRIG_TOS1_Msk                0x00000002UL           /*!< DC 1 output trigger status */
#define ADCSAR_DCTRIG_TOS2_Msk                0x00000004UL           /*!< DC 2 output trigger status */
#define ADCSAR_DCTRIG_TOS3_Msk                0x00000008UL           /*!< DC 3 output trigger status */
#define ADCSAR_DCTRIG_TOS4_Msk                0x00000010UL           /*!< DC 4 output trigger status */
#define ADCSAR_DCTRIG_TOS5_Msk                0x00000020UL           /*!< DC 5 output trigger status */
#define ADCSAR_DCTRIG_TOS6_Msk                0x00000040UL           /*!< DC 6 output trigger status */
#define ADCSAR_DCTRIG_TOS7_Msk                0x00000080UL           /*!< DC 7 output trigger status */

/*--  DCEV: Digital comparator compare event status register --------------------------------------------------*/
typedef struct {
  uint32_t DCEV0                  :1;                                /*!< Digital compare event 0 */
  uint32_t DCEV1                  :1;                                /*!< Digital compare event 1 */
  uint32_t DCEV2                  :1;                                /*!< Digital compare event 2 */
  uint32_t DCEV3                  :1;                                /*!< Digital compare event 3 */
  uint32_t DCEV4                  :1;                                /*!< Digital compare event 4 */
  uint32_t DCEV5                  :1;                                /*!< Digital compare event 5 */
  uint32_t DCEV6                  :1;                                /*!< Digital compare event 6 */
  uint32_t DCEV7                  :1;                                /*!< Digital compare event 7 */
} _ADCSAR_DCEV_bits;

/* Bit field positions: */
#define ADCSAR_DCEV_DCEV0_Pos                 0                      /*!< Digital compare event 0 */
#define ADCSAR_DCEV_DCEV1_Pos                 1                      /*!< Digital compare event 1 */
#define ADCSAR_DCEV_DCEV2_Pos                 2                      /*!< Digital compare event 2 */
#define ADCSAR_DCEV_DCEV3_Pos                 3                      /*!< Digital compare event 3 */
#define ADCSAR_DCEV_DCEV4_Pos                 4                      /*!< Digital compare event 4 */
#define ADCSAR_DCEV_DCEV5_Pos                 5                      /*!< Digital compare event 5 */
#define ADCSAR_DCEV_DCEV6_Pos                 6                      /*!< Digital compare event 6 */
#define ADCSAR_DCEV_DCEV7_Pos                 7                      /*!< Digital compare event 7 */

/* Bit field masks: */
#define ADCSAR_DCEV_DCEV0_Msk                 0x00000001UL           /*!< Digital compare event 0 */
#define ADCSAR_DCEV_DCEV1_Msk                 0x00000002UL           /*!< Digital compare event 1 */
#define ADCSAR_DCEV_DCEV2_Msk                 0x00000004UL           /*!< Digital compare event 2 */
#define ADCSAR_DCEV_DCEV3_Msk                 0x00000008UL           /*!< Digital compare event 3 */
#define ADCSAR_DCEV_DCEV4_Msk                 0x00000010UL           /*!< Digital compare event 4 */
#define ADCSAR_DCEV_DCEV5_Msk                 0x00000020UL           /*!< Digital compare event 5 */
#define ADCSAR_DCEV_DCEV6_Msk                 0x00000040UL           /*!< Digital compare event 6 */
#define ADCSAR_DCEV_DCEV7_Msk                 0x00000080UL           /*!< Digital compare event 7 */

/*--  CICNT: Interrupt counter clear control ------------------------------------------------------------------*/
typedef struct {
  uint32_t ICNT0                  :1;                                /*!< Clear interrupt counter on sequencer 0 start */
  uint32_t ICNT1                  :1;                                /*!< Clear interrupt counter on sequencer 1 start */
} _ADCSAR_CICNT_bits;

/* Bit field positions: */
#define ADCSAR_CICNT_ICNT0_Pos                0                      /*!< Clear interrupt counter on sequencer 0 start */
#define ADCSAR_CICNT_ICNT1_Pos                1                      /*!< Clear interrupt counter on sequencer 1 start */

/* Bit field masks: */
#define ADCSAR_CICNT_ICNT0_Msk                0x00000001UL           /*!< Clear interrupt counter on sequencer 0 start */
#define ADCSAR_CICNT_ICNT1_Msk                0x00000002UL           /*!< Clear interrupt counter on sequencer 1 start */

/*--  EMUX: Sequencer start event selection register ----------------------------------------------------------*/
typedef struct {
  uint32_t EM0                    :4;                                /*!< Select start event for sequencer 0 */
  uint32_t EM1                    :4;                                /*!< Select start event for sequencer 1 */
} _ADCSAR_EMUX_bits;

/* Bit field positions: */
#define ADCSAR_EMUX_EM0_Pos                   0                      /*!< Select start event for sequencer 0 */
#define ADCSAR_EMUX_EM1_Pos                   4                      /*!< Select start event for sequencer 1 */

/* Bit field masks: */
#define ADCSAR_EMUX_EM0_Msk                   0x0000000FUL           /*!< Select start event for sequencer 0 */
#define ADCSAR_EMUX_EM1_Msk                   0x000000F0UL           /*!< Select start event for sequencer 1 */

/* Bit field enums: */
typedef enum {
  ADCSAR_EMUX_EM0_SwReq                      = 0x0UL,                /*!< software request by GSYNC bit */
  ADCSAR_EMUX_EM0_TMR32                      = 0x1UL,                /*!< TMR32 interrupt */
  ADCSAR_EMUX_EM0_TMR0                       = 0x2UL,                /*!< TMR0 interrupt */
  ADCSAR_EMUX_EM0_TMR1                       = 0x3UL,                /*!< TMR1 interrupt */
  ADCSAR_EMUX_EM0_TMR2                       = 0x4UL,                /*!< Timer 2 request */
  ADCSAR_EMUX_EM0_GPIOA                      = 0x5UL,                /*!< GPIOA request */
  ADCSAR_EMUX_EM0_GPIOB                      = 0x6UL,                /*!< GPIOB request */
  ADCSAR_EMUX_EM0_GPIOC                      = 0x7UL,                /*!< GPIOA request */
  ADCSAR_EMUX_EM0_Cycle                      = 0xFUL,                /*!< Cycle mode */
} ADCSAR_EMUX_EM0_Enum;

typedef enum {
  ADCSAR_EMUX_EM1_SwReq                      = 0x0UL,                /*!< software request by GSYNC bit */
  ADCSAR_EMUX_EM1_TMR32                      = 0x1UL,                /*!< TMR32 interrupt */
  ADCSAR_EMUX_EM1_TMR0                       = 0x2UL,                /*!< TMR0 interrupt */
  ADCSAR_EMUX_EM1_TMR1                       = 0x3UL,                /*!< TMR1 interrupt */
  ADCSAR_EMUX_EM1_TMR2                       = 0x4UL,                /*!< Timer 2 request */
  ADCSAR_EMUX_EM1_GPIOA                      = 0x5UL,                /*!< GPIOA request */
  ADCSAR_EMUX_EM1_GPIOB                      = 0x6UL,                /*!< GPIOB request */
  ADCSAR_EMUX_EM1_GPIOC                      = 0x7UL,                /*!< GPIOA request */
  ADCSAR_EMUX_EM1_Cycle                      = 0xFUL,                /*!< Cycle mode */
} ADCSAR_EMUX_EM1_Enum;

/*--  RIS: Raw interrupt status register ----------------------------------------------------------------------*/
typedef struct {
  uint32_t SEQRIS0                :1;                                /*!< Sequencer 0 raw interrupt status */
  uint32_t SEQRIS1                :1;                                /*!< Sequencer 1 raw interrupt status */
  uint32_t                        :6;                                /*!< RESERVED */
  uint32_t DCRIS0                 :1;                                /*!< Raw interrupt status of Digital Comparator 0 */
  uint32_t DCRIS1                 :1;                                /*!< Raw interrupt status of Digital Comparator 1 */
  uint32_t DCRIS2                 :1;                                /*!< Raw interrupt status of Digital Comparator 2 */
  uint32_t DCRIS3                 :1;                                /*!< Raw interrupt status of Digital Comparator 3 */
  uint32_t DCRIS4                 :1;                                /*!< Raw interrupt status of Digital Comparator 4 */
  uint32_t DCRIS5                 :1;                                /*!< Raw interrupt status of Digital Comparator 5 */
  uint32_t DCRIS6                 :1;                                /*!< Raw interrupt status of Digital Comparator 6 */
  uint32_t DCRIS7                 :1;                                /*!< Raw interrupt status of Digital Comparator 7 */
} _ADCSAR_RIS_bits;

/* Bit field positions: */
#define ADCSAR_RIS_SEQRIS0_Pos                0                      /*!< Sequencer 0 raw interrupt status */
#define ADCSAR_RIS_SEQRIS1_Pos                1                      /*!< Sequencer 1 raw interrupt status */
#define ADCSAR_RIS_DCRIS0_Pos                 8                      /*!< Raw interrupt status of Digital Comparator 0 */
#define ADCSAR_RIS_DCRIS1_Pos                 9                      /*!< Raw interrupt status of Digital Comparator 1 */
#define ADCSAR_RIS_DCRIS2_Pos                 10                     /*!< Raw interrupt status of Digital Comparator 2 */
#define ADCSAR_RIS_DCRIS3_Pos                 11                     /*!< Raw interrupt status of Digital Comparator 3 */
#define ADCSAR_RIS_DCRIS4_Pos                 12                     /*!< Raw interrupt status of Digital Comparator 4 */
#define ADCSAR_RIS_DCRIS5_Pos                 13                     /*!< Raw interrupt status of Digital Comparator 5 */
#define ADCSAR_RIS_DCRIS6_Pos                 14                     /*!< Raw interrupt status of Digital Comparator 6 */
#define ADCSAR_RIS_DCRIS7_Pos                 15                     /*!< Raw interrupt status of Digital Comparator 7 */

/* Bit field masks: */
#define ADCSAR_RIS_SEQRIS0_Msk                0x00000001UL           /*!< Sequencer 0 raw interrupt status */
#define ADCSAR_RIS_SEQRIS1_Msk                0x00000002UL           /*!< Sequencer 1 raw interrupt status */
#define ADCSAR_RIS_DCRIS0_Msk                 0x00000100UL           /*!< Raw interrupt status of Digital Comparator 0 */
#define ADCSAR_RIS_DCRIS1_Msk                 0x00000200UL           /*!< Raw interrupt status of Digital Comparator 1 */
#define ADCSAR_RIS_DCRIS2_Msk                 0x00000400UL           /*!< Raw interrupt status of Digital Comparator 2 */
#define ADCSAR_RIS_DCRIS3_Msk                 0x00000800UL           /*!< Raw interrupt status of Digital Comparator 3 */
#define ADCSAR_RIS_DCRIS4_Msk                 0x00001000UL           /*!< Raw interrupt status of Digital Comparator 4 */
#define ADCSAR_RIS_DCRIS5_Msk                 0x00002000UL           /*!< Raw interrupt status of Digital Comparator 5 */
#define ADCSAR_RIS_DCRIS6_Msk                 0x00004000UL           /*!< Raw interrupt status of Digital Comparator 6 */
#define ADCSAR_RIS_DCRIS7_Msk                 0x00008000UL           /*!< Raw interrupt status of Digital Comparator 7 */

/*--  IM: Interrupt mask register -----------------------------------------------------------------------------*/
typedef struct {
  uint32_t SEQIM0                 :1;                                /*!< Sequencer 0 interrupt mask */
  uint32_t SEQIM1                 :1;                                /*!< Sequencer 1 interrupt mask */
  uint32_t                        :6;                                /*!< RESERVED */
  uint32_t DCIM0                  :1;                                /*!< Interrupt mask of Digital Comparator 0 */
  uint32_t DCIM1                  :1;                                /*!< Interrupt mask of Digital Comparator 1 */
  uint32_t DCIM2                  :1;                                /*!< Interrupt mask of Digital Comparator 2 */
  uint32_t DCIM3                  :1;                                /*!< Interrupt mask of Digital Comparator 3 */
  uint32_t DCIM4                  :1;                                /*!< Interrupt mask of Digital Comparator 4 */
  uint32_t DCIM5                  :1;                                /*!< Interrupt mask of Digital Comparator 5 */
  uint32_t DCIM6                  :1;                                /*!< Interrupt mask of Digital Comparator 6 */
  uint32_t DCIM7                  :1;                                /*!< Interrupt mask of Digital Comparator 7 */
} _ADCSAR_IM_bits;

/* Bit field positions: */
#define ADCSAR_IM_SEQIM0_Pos                  0                      /*!< Sequencer 0 interrupt mask */
#define ADCSAR_IM_SEQIM1_Pos                  1                      /*!< Sequencer 1 interrupt mask */
#define ADCSAR_IM_DCIM0_Pos                   8                      /*!< Interrupt mask of Digital Comparator 0 */
#define ADCSAR_IM_DCIM1_Pos                   9                      /*!< Interrupt mask of Digital Comparator 1 */
#define ADCSAR_IM_DCIM2_Pos                   10                     /*!< Interrupt mask of Digital Comparator 2 */
#define ADCSAR_IM_DCIM3_Pos                   11                     /*!< Interrupt mask of Digital Comparator 3 */
#define ADCSAR_IM_DCIM4_Pos                   12                     /*!< Interrupt mask of Digital Comparator 4 */
#define ADCSAR_IM_DCIM5_Pos                   13                     /*!< Interrupt mask of Digital Comparator 5 */
#define ADCSAR_IM_DCIM6_Pos                   14                     /*!< Interrupt mask of Digital Comparator 6 */
#define ADCSAR_IM_DCIM7_Pos                   15                     /*!< Interrupt mask of Digital Comparator 7 */

/* Bit field masks: */
#define ADCSAR_IM_SEQIM0_Msk                  0x00000001UL           /*!< Sequencer 0 interrupt mask */
#define ADCSAR_IM_SEQIM1_Msk                  0x00000002UL           /*!< Sequencer 1 interrupt mask */
#define ADCSAR_IM_DCIM0_Msk                   0x00000100UL           /*!< Interrupt mask of Digital Comparator 0 */
#define ADCSAR_IM_DCIM1_Msk                   0x00000200UL           /*!< Interrupt mask of Digital Comparator 1 */
#define ADCSAR_IM_DCIM2_Msk                   0x00000400UL           /*!< Interrupt mask of Digital Comparator 2 */
#define ADCSAR_IM_DCIM3_Msk                   0x00000800UL           /*!< Interrupt mask of Digital Comparator 3 */
#define ADCSAR_IM_DCIM4_Msk                   0x00001000UL           /*!< Interrupt mask of Digital Comparator 4 */
#define ADCSAR_IM_DCIM5_Msk                   0x00002000UL           /*!< Interrupt mask of Digital Comparator 5 */
#define ADCSAR_IM_DCIM6_Msk                   0x00004000UL           /*!< Interrupt mask of Digital Comparator 6 */
#define ADCSAR_IM_DCIM7_Msk                   0x00008000UL           /*!< Interrupt mask of Digital Comparator 7 */

/*--  MIS: Masked interrupt status and clear register ---------------------------------------------------------*/
typedef struct {
  uint32_t SEQMIS0                :1;                                /*!< Sequencer 0 masked interrupt status */
  uint32_t SEQMIS1                :1;                                /*!< Sequencer 1 masked interrupt status  */
  uint32_t                        :6;                                /*!< RESERVED */
  uint32_t DCMIS0                 :1;                                /*!< DC 0 masked interrupt status  */
  uint32_t DCMIS1                 :1;                                /*!< DC 1 masked interrupt status  */
  uint32_t DCMIS2                 :1;                                /*!< DC 2 masked interrupt status  */
  uint32_t DCMIS3                 :1;                                /*!< DC 3 masked interrupt status  */
  uint32_t DCMIS4                 :1;                                /*!< DC 4 masked interrupt status  */
  uint32_t DCMIS5                 :1;                                /*!< DC 5 masked interrupt status  */
  uint32_t DCMIS6                 :1;                                /*!< DC 6 masked interrupt status  */
  uint32_t DCMIS7                 :1;                                /*!< DC 7 masked interrupt status  */
} _ADCSAR_MIS_bits;

/* Bit field positions: */
#define ADCSAR_MIS_SEQMIS0_Pos                0                      /*!< Sequencer 0 masked interrupt status */
#define ADCSAR_MIS_SEQMIS1_Pos                1                      /*!< Sequencer 1 masked interrupt status  */
#define ADCSAR_MIS_DCMIS0_Pos                 8                      /*!< DC 0 masked interrupt status  */
#define ADCSAR_MIS_DCMIS1_Pos                 9                      /*!< DC 1 masked interrupt status  */
#define ADCSAR_MIS_DCMIS2_Pos                 10                     /*!< DC 2 masked interrupt status  */
#define ADCSAR_MIS_DCMIS3_Pos                 11                     /*!< DC 3 masked interrupt status  */
#define ADCSAR_MIS_DCMIS4_Pos                 12                     /*!< DC 4 masked interrupt status  */
#define ADCSAR_MIS_DCMIS5_Pos                 13                     /*!< DC 5 masked interrupt status  */
#define ADCSAR_MIS_DCMIS6_Pos                 14                     /*!< DC 6 masked interrupt status  */
#define ADCSAR_MIS_DCMIS7_Pos                 15                     /*!< DC 7 masked interrupt status  */

/* Bit field masks: */
#define ADCSAR_MIS_SEQMIS0_Msk                0x00000001UL           /*!< Sequencer 0 masked interrupt status */
#define ADCSAR_MIS_SEQMIS1_Msk                0x00000002UL           /*!< Sequencer 1 masked interrupt status  */
#define ADCSAR_MIS_DCMIS0_Msk                 0x00000100UL           /*!< DC 0 masked interrupt status  */
#define ADCSAR_MIS_DCMIS1_Msk                 0x00000200UL           /*!< DC 1 masked interrupt status  */
#define ADCSAR_MIS_DCMIS2_Msk                 0x00000400UL           /*!< DC 2 masked interrupt status  */
#define ADCSAR_MIS_DCMIS3_Msk                 0x00000800UL           /*!< DC 3 masked interrupt status  */
#define ADCSAR_MIS_DCMIS4_Msk                 0x00001000UL           /*!< DC 4 masked interrupt status  */
#define ADCSAR_MIS_DCMIS5_Msk                 0x00002000UL           /*!< DC 5 masked interrupt status  */
#define ADCSAR_MIS_DCMIS6_Msk                 0x00004000UL           /*!< DC 6 masked interrupt status  */
#define ADCSAR_MIS_DCMIS7_Msk                 0x00008000UL           /*!< DC 7 masked interrupt status  */

/*--  IC: Interrupt clear register ----------------------------------------------------------------------------*/
typedef struct {
  uint32_t SEQIC0                 :1;                                /*!< Sequencer 0 interrupt status clear */
  uint32_t SEQIC1                 :1;                                /*!< Sequencer 1 interrupt status clear */
  uint32_t                        :6;                                /*!< RESERVED */
  uint32_t DCIC0                  :1;                                /*!< DC 0 interrupt status clear */
  uint32_t DCIC1                  :1;                                /*!< DC 1 interrupt status clear */
  uint32_t DCIC2                  :1;                                /*!< DC 2 interrupt status clear */
  uint32_t DCIC3                  :1;                                /*!< DC 3 interrupt status clear */
  uint32_t DCIC4                  :1;                                /*!< DC 4 interrupt status clear */
  uint32_t DCIC5                  :1;                                /*!< DC 5 interrupt status clear */
  uint32_t DCIC6                  :1;                                /*!< DC 6 interrupt status clear */
  uint32_t DCIC7                  :1;                                /*!< DC 7 interrupt status clear */
} _ADCSAR_IC_bits;

/* Bit field positions: */
#define ADCSAR_IC_SEQIC0_Pos                  0                      /*!< Sequencer 0 interrupt status clear */
#define ADCSAR_IC_SEQIC1_Pos                  1                      /*!< Sequencer 1 interrupt status clear */
#define ADCSAR_IC_DCIC0_Pos                   8                      /*!< DC 0 interrupt status clear */
#define ADCSAR_IC_DCIC1_Pos                   9                      /*!< DC 1 interrupt status clear */
#define ADCSAR_IC_DCIC2_Pos                   10                     /*!< DC 2 interrupt status clear */
#define ADCSAR_IC_DCIC3_Pos                   11                     /*!< DC 3 interrupt status clear */
#define ADCSAR_IC_DCIC4_Pos                   12                     /*!< DC 4 interrupt status clear */
#define ADCSAR_IC_DCIC5_Pos                   13                     /*!< DC 5 interrupt status clear */
#define ADCSAR_IC_DCIC6_Pos                   14                     /*!< DC 6 interrupt status clear */
#define ADCSAR_IC_DCIC7_Pos                   15                     /*!< DC 7 interrupt status clear */

/* Bit field masks: */
#define ADCSAR_IC_SEQIC0_Msk                  0x00000001UL           /*!< Sequencer 0 interrupt status clear */
#define ADCSAR_IC_SEQIC1_Msk                  0x00000002UL           /*!< Sequencer 1 interrupt status clear */
#define ADCSAR_IC_DCIC0_Msk                   0x00000100UL           /*!< DC 0 interrupt status clear */
#define ADCSAR_IC_DCIC1_Msk                   0x00000200UL           /*!< DC 1 interrupt status clear */
#define ADCSAR_IC_DCIC2_Msk                   0x00000400UL           /*!< DC 2 interrupt status clear */
#define ADCSAR_IC_DCIC3_Msk                   0x00000800UL           /*!< DC 3 interrupt status clear */
#define ADCSAR_IC_DCIC4_Msk                   0x00001000UL           /*!< DC 4 interrupt status clear */
#define ADCSAR_IC_DCIC5_Msk                   0x00002000UL           /*!< DC 5 interrupt status clear */
#define ADCSAR_IC_DCIC6_Msk                   0x00004000UL           /*!< DC 6 interrupt status clear */
#define ADCSAR_IC_DCIC7_Msk                   0x00008000UL           /*!< DC 7 interrupt status clear */

/*-- SEQ: SRQSEL: Sequencer request ADC channels selection register -------------------------------------------*/
typedef struct {
  uint32_t RQ0                    :4;                                /*!< Select ADC channel for request 0 */
  uint32_t RQ1                    :4;                                /*!< Select ADC channel for request 1 */
  uint32_t RQ2                    :4;                                /*!< Select ADC channel for request 2 */
  uint32_t RQ3                    :4;                                /*!< Select ADC channel for request 3 */
  uint32_t RQ4                    :4;                                /*!< Select ADC channel for request 4 */
  uint32_t RQ5                    :4;                                /*!< Select ADC channel for request 5 */
  uint32_t RQ6                    :4;                                /*!< Select ADC channel for request 6 */
  uint32_t RQ7                    :4;                                /*!< Select ADC channel for request 7 */
} _ADCSAR_SEQ_SRQSEL_bits;

/* Bit field positions: */
#define ADCSAR_SEQ_SRQSEL_RQ0_Pos             0                      /*!< Select ADC channel for request 0 */
#define ADCSAR_SEQ_SRQSEL_RQ1_Pos             4                      /*!< Select ADC channel for request 1 */
#define ADCSAR_SEQ_SRQSEL_RQ2_Pos             8                      /*!< Select ADC channel for request 2 */
#define ADCSAR_SEQ_SRQSEL_RQ3_Pos             12                     /*!< Select ADC channel for request 3 */
#define ADCSAR_SEQ_SRQSEL_RQ4_Pos             16                     /*!< Select ADC channel for request 4 */
#define ADCSAR_SEQ_SRQSEL_RQ5_Pos             20                     /*!< Select ADC channel for request 5 */
#define ADCSAR_SEQ_SRQSEL_RQ6_Pos             24                     /*!< Select ADC channel for request 6 */
#define ADCSAR_SEQ_SRQSEL_RQ7_Pos             28                     /*!< Select ADC channel for request 7 */

/* Bit field masks: */
#define ADCSAR_SEQ_SRQSEL_RQ0_Msk             0x0000000FUL           /*!< Select ADC channel for request 0 */
#define ADCSAR_SEQ_SRQSEL_RQ1_Msk             0x000000F0UL           /*!< Select ADC channel for request 1 */
#define ADCSAR_SEQ_SRQSEL_RQ2_Msk             0x00000F00UL           /*!< Select ADC channel for request 2 */
#define ADCSAR_SEQ_SRQSEL_RQ3_Msk             0x0000F000UL           /*!< Select ADC channel for request 3 */
#define ADCSAR_SEQ_SRQSEL_RQ4_Msk             0x000F0000UL           /*!< Select ADC channel for request 4 */
#define ADCSAR_SEQ_SRQSEL_RQ5_Msk             0x00F00000UL           /*!< Select ADC channel for request 5 */
#define ADCSAR_SEQ_SRQSEL_RQ6_Msk             0x0F000000UL           /*!< Select ADC channel for request 6 */
#define ADCSAR_SEQ_SRQSEL_RQ7_Msk             0xF0000000UL           /*!< Select ADC channel for request 7 */

/*-- SEQ: SRQCTL: Sequencer request control register ----------------------------------------------------------*/
typedef struct {
  uint32_t RQMAX                  :3;                                /*!< Request queue max depth */
  uint32_t                        :5;                                /*!< RESERVED */
  uint32_t QAVGEN                 :1;                                /*!< Queue avearage (scanning) enable */
  uint32_t QAVGVAL                :3;                                /*!< Queue average value */
} _ADCSAR_SEQ_SRQCTL_bits;

/* Bit field positions: */
#define ADCSAR_SEQ_SRQCTL_RQMAX_Pos           0                      /*!< Request queue max depth */
#define ADCSAR_SEQ_SRQCTL_QAVGEN_Pos          8                      /*!< Queue avearage (scanning) enable */
#define ADCSAR_SEQ_SRQCTL_QAVGVAL_Pos         9                      /*!< Queue average value */

/* Bit field masks: */
#define ADCSAR_SEQ_SRQCTL_RQMAX_Msk           0x00000007UL           /*!< Request queue max depth */
#define ADCSAR_SEQ_SRQCTL_QAVGEN_Msk          0x00000100UL           /*!< Queue avearage (scanning) enable */
#define ADCSAR_SEQ_SRQCTL_QAVGVAL_Msk         0x00000E00UL           /*!< Queue average value */

/* Bit field enums: */
typedef enum {
  ADCSAR_SEQ_SRQCTL_QAVGVAL_Disable          = 0x0UL,                /*!< Average disabled */
  ADCSAR_SEQ_SRQCTL_QAVGVAL_Average2         = 0x1UL,                /*!< Average with 2 measures */
  ADCSAR_SEQ_SRQCTL_QAVGVAL_Average4         = 0x2UL,                /*!< Average with 4 measures */
  ADCSAR_SEQ_SRQCTL_QAVGVAL_Average8         = 0x3UL,                /*!< Average with 8 measures */
  ADCSAR_SEQ_SRQCTL_QAVGVAL_Average16        = 0x4UL,                /*!< Average with 16 measures */
  ADCSAR_SEQ_SRQCTL_QAVGVAL_Average32        = 0x5UL,                /*!< Average with 32 measures */
  ADCSAR_SEQ_SRQCTL_QAVGVAL_Average64        = 0x6UL,                /*!< Average with 64 measures */
} ADCSAR_SEQ_SRQCTL_QAVGVAL_Enum;

/*-- SEQ: SRQSTAT: Sequencer request status register ----------------------------------------------------------*/
typedef struct {
  uint32_t RQPTR                  :3;                                /*!< Pointer to queue current request */
  uint32_t                        :5;                                /*!< RESERVED */
  uint32_t RQBUSY                 :1;                                /*!< Active request status */
} _ADCSAR_SEQ_SRQSTAT_bits;

/* Bit field positions: */
#define ADCSAR_SEQ_SRQSTAT_RQPTR_Pos          0                      /*!< Pointer to queue current request */
#define ADCSAR_SEQ_SRQSTAT_RQBUSY_Pos         8                      /*!< Active request status */

/* Bit field masks: */
#define ADCSAR_SEQ_SRQSTAT_RQPTR_Msk          0x00000007UL           /*!< Pointer to queue current request */
#define ADCSAR_SEQ_SRQSTAT_RQBUSY_Msk         0x00000100UL           /*!< Active request status */

/*-- SEQ: SDMACTL: Sequencer DMA control register -------------------------------------------------------------*/
typedef struct {
  uint32_t DMAEN                  :1;                                /*!< Enable DMA use */
  uint32_t                        :7;                                /*!< RESERVED */
  uint32_t WMARK                  :3;                                /*!< FIFO load threshold for DMA request generation */
} _ADCSAR_SEQ_SDMACTL_bits;

/* Bit field positions: */
#define ADCSAR_SEQ_SDMACTL_DMAEN_Pos          0                      /*!< Enable DMA use */
#define ADCSAR_SEQ_SDMACTL_WMARK_Pos          8                      /*!< FIFO load threshold for DMA request generation */

/* Bit field masks: */
#define ADCSAR_SEQ_SDMACTL_DMAEN_Msk          0x00000001UL           /*!< Enable DMA use */
#define ADCSAR_SEQ_SDMACTL_WMARK_Msk          0x00000700UL           /*!< FIFO load threshold for DMA request generation */

/* Bit field enums: */
typedef enum {
  ADCSAR_SEQ_SDMACTL_WMARK_Level1            = 0x1UL,                /*!< 1 measure for dma request */
  ADCSAR_SEQ_SDMACTL_WMARK_Level2            = 0x2UL,                /*!< 2 measures for dma request */
  ADCSAR_SEQ_SDMACTL_WMARK_Level4            = 0x3UL,                /*!< 4 measures for dma request */
  ADCSAR_SEQ_SDMACTL_WMARK_Level8            = 0x4UL,                /*!< 8 measures for dma request */
  ADCSAR_SEQ_SDMACTL_WMARK_Level16           = 0x5UL,                /*!< 16 measures for dma request */
  ADCSAR_SEQ_SDMACTL_WMARK_Level32           = 0x6UL,                /*!< 32 measures for dma request */
} ADCSAR_SEQ_SDMACTL_WMARK_Enum;

/*-- SEQ: SCCTL: Sequencer ADC interrupt and restart counter control register ---------------------------------*/
typedef struct {
  uint32_t RCNT                   :8;                                /*!< Current number of ADC restarts by sequencer */
  uint32_t RAVGEN                 :1;                                /*!< Average of ADC restarts enable */
  uint32_t                        :7;                                /*!< RESERVED */
  uint32_t ICNT                   :8;                                /*!< Number of ADC restarts for interrupt generation */
} _ADCSAR_SEQ_SCCTL_bits;

/* Bit field positions: */
#define ADCSAR_SEQ_SCCTL_RCNT_Pos             0                      /*!< Current number of ADC restarts by sequencer */
#define ADCSAR_SEQ_SCCTL_RAVGEN_Pos           8                      /*!< Average of ADC restarts enable */
#define ADCSAR_SEQ_SCCTL_ICNT_Pos             16                     /*!< Number of ADC restarts for interrupt generation */

/* Bit field masks: */
#define ADCSAR_SEQ_SCCTL_RCNT_Msk             0x000000FFUL           /*!< Current number of ADC restarts by sequencer */
#define ADCSAR_SEQ_SCCTL_RAVGEN_Msk           0x00000100UL           /*!< Average of ADC restarts enable */
#define ADCSAR_SEQ_SCCTL_ICNT_Msk             0x00FF0000UL           /*!< Number of ADC restarts for interrupt generation */

/*-- SEQ: SCVAL: Sequencer ADC interrupt and restart counter current value register --------------------------------*/
typedef struct {
  uint32_t RCNT                   :8;                                /*!< Current number of ADC restarts by sequencer */
  uint32_t                        :8;                                /*!< RESERVED */
  uint32_t ICNT                   :8;                                /*!< Current number of ADC restarts for interrupt generation */
  uint32_t ICLR                   :1;                                /*!< Clear interrupt counter */
} _ADCSAR_SEQ_SCVAL_bits;

/* Bit field positions: */
#define ADCSAR_SEQ_SCVAL_RCNT_Pos             0                      /*!< Current number of ADC restarts by sequencer */
#define ADCSAR_SEQ_SCVAL_ICNT_Pos             16                     /*!< Current number of ADC restarts for interrupt generation */
#define ADCSAR_SEQ_SCVAL_ICLR_Pos             24                     /*!< Clear interrupt counter */

/* Bit field masks: */
#define ADCSAR_SEQ_SCVAL_RCNT_Msk             0x000000FFUL           /*!< Current number of ADC restarts by sequencer */
#define ADCSAR_SEQ_SCVAL_ICNT_Msk             0x00FF0000UL           /*!< Current number of ADC restarts for interrupt generation */
#define ADCSAR_SEQ_SCVAL_ICLR_Msk             0x01000000UL           /*!< Clear interrupt counter */

/*-- SEQ: SDC: Sequencer digital comparator selection register ------------------------------------------------*/
typedef struct {
  uint32_t DC0                    :1;                                /*!< Enable DC 0 */
  uint32_t DC1                    :1;                                /*!< Enable DC 1 */
  uint32_t DC2                    :1;                                /*!< Enable DC 2 */
  uint32_t DC3                    :1;                                /*!< Enable DC 3 */
  uint32_t DC4                    :1;                                /*!< Enable DC 4 */
  uint32_t DC5                    :1;                                /*!< Enable DC 5 */
  uint32_t DC6                    :1;                                /*!< Enable DC 6 */
  uint32_t DC7                    :1;                                /*!< Enable DC 7 */
} _ADCSAR_SEQ_SDC_bits;

/* Bit field positions: */
#define ADCSAR_SEQ_SDC_DC0_Pos                0                      /*!< Enable DC 0 */
#define ADCSAR_SEQ_SDC_DC1_Pos                1                      /*!< Enable DC 1 */
#define ADCSAR_SEQ_SDC_DC2_Pos                2                      /*!< Enable DC 2 */
#define ADCSAR_SEQ_SDC_DC3_Pos                3                      /*!< Enable DC 3 */
#define ADCSAR_SEQ_SDC_DC4_Pos                4                      /*!< Enable DC 4 */
#define ADCSAR_SEQ_SDC_DC5_Pos                5                      /*!< Enable DC 5 */
#define ADCSAR_SEQ_SDC_DC6_Pos                6                      /*!< Enable DC 6 */
#define ADCSAR_SEQ_SDC_DC7_Pos                7                      /*!< Enable DC 7 */

/* Bit field masks: */
#define ADCSAR_SEQ_SDC_DC0_Msk                0x00000001UL           /*!< Enable DC 0 */
#define ADCSAR_SEQ_SDC_DC1_Msk                0x00000002UL           /*!< Enable DC 1 */
#define ADCSAR_SEQ_SDC_DC2_Msk                0x00000004UL           /*!< Enable DC 2 */
#define ADCSAR_SEQ_SDC_DC3_Msk                0x00000008UL           /*!< Enable DC 3 */
#define ADCSAR_SEQ_SDC_DC4_Msk                0x00000010UL           /*!< Enable DC 4 */
#define ADCSAR_SEQ_SDC_DC5_Msk                0x00000020UL           /*!< Enable DC 5 */
#define ADCSAR_SEQ_SDC_DC6_Msk                0x00000040UL           /*!< Enable DC 6 */
#define ADCSAR_SEQ_SDC_DC7_Msk                0x00000080UL           /*!< Enable DC 7 */

/*-- SEQ: SRTMR: Sequencer ADC restart timer register ---------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :24;                               /*!< Sequencer ADC restart timer value */
  uint32_t                        :7;                                /*!< RESERVED */
  uint32_t NOWAIT                 :1;                                /*!< Timer update with no waiting the end of current seq task */
} _ADCSAR_SEQ_SRTMR_bits;

/* Bit field positions: */
#define ADCSAR_SEQ_SRTMR_VAL_Pos              0                      /*!< Sequencer ADC restart timer value */
#define ADCSAR_SEQ_SRTMR_NOWAIT_Pos           31                     /*!< Timer update with no waiting the end of current seq task */

/* Bit field masks: */
#define ADCSAR_SEQ_SRTMR_VAL_Msk              0x00FFFFFFUL           /*!< Sequencer ADC restart timer value */
#define ADCSAR_SEQ_SRTMR_NOWAIT_Msk           0x80000000UL           /*!< Timer update with no waiting the end of current seq task */

/*-- SEQ: SFLOAD: Sequencer FIFO load status register ---------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :6;                                /*!< Sequencer FIFO current load value */
} _ADCSAR_SEQ_SFLOAD_bits;

/* Bit field positions: */
#define ADCSAR_SEQ_SFLOAD_VAL_Pos             0                      /*!< Sequencer FIFO current load value */

/* Bit field masks: */
#define ADCSAR_SEQ_SFLOAD_VAL_Msk             0x0000003FUL           /*!< Sequencer FIFO current load value */

/*-- SEQ: SFIFO: Sequencer FIFO register ----------------------------------------------------------------------*/
typedef struct {
  uint32_t DATA                   :12;                               /*!< AD conversion value */
} _ADCSAR_SEQ_SFIFO_bits;

/* Bit field positions: */
#define ADCSAR_SEQ_SFIFO_DATA_Pos             0                      /*!< AD conversion value */

/* Bit field masks: */
#define ADCSAR_SEQ_SFIFO_DATA_Msk             0x00000FFFUL           /*!< AD conversion value */

/*-- DC: DCTL: Digital comparator control register ------------------------------------------------------------*/
typedef struct {
  uint32_t CIM                    :2;                                /*!< DC interrupt generation mode */
  uint32_t CIC                    :2;                                /*!< DC interrupt generation compare conditions */
  uint32_t CIE                    :1;                                /*!< Enable DC interrupt generation */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t CTM                    :2;                                /*!< DC output trigger mode */
  uint32_t CTC                    :2;                                /*!< DC output trigger compare conditions */
  uint32_t CTE                    :1;                                /*!< Enable DC output trigger */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t CHNL                   :3;                                /*!< ADC channel selection */
  uint32_t                        :5;                                /*!< RESERVED */
  uint32_t SRC                    :1;                                /*!< Select data source for comparation: ADC module (0) or sequencer(1) */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t HWTCLR                 :1;                                /*!< Hardware output trigger clear */
} _ADCSAR_DC_DCTL_bits;

/* Bit field positions: */
#define ADCSAR_DC_DCTL_CIM_Pos                0                      /*!< DC interrupt generation mode */
#define ADCSAR_DC_DCTL_CIC_Pos                2                      /*!< DC interrupt generation compare conditions */
#define ADCSAR_DC_DCTL_CIE_Pos                4                      /*!< Enable DC interrupt generation */
#define ADCSAR_DC_DCTL_CTM_Pos                8                      /*!< DC output trigger mode */
#define ADCSAR_DC_DCTL_CTC_Pos                10                     /*!< DC output trigger compare conditions */
#define ADCSAR_DC_DCTL_CTE_Pos                12                     /*!< Enable DC output trigger */
#define ADCSAR_DC_DCTL_CHNL_Pos               16                     /*!< ADC channel selection */
#define ADCSAR_DC_DCTL_SRC_Pos                24                     /*!< Select data source for comparation: ADC module (0) or sequencer(1) */
#define ADCSAR_DC_DCTL_HWTCLR_Pos             28                     /*!< Hardware output trigger clear */

/* Bit field masks: */
#define ADCSAR_DC_DCTL_CIM_Msk                0x00000003UL           /*!< DC interrupt generation mode */
#define ADCSAR_DC_DCTL_CIC_Msk                0x0000000CUL           /*!< DC interrupt generation compare conditions */
#define ADCSAR_DC_DCTL_CIE_Msk                0x00000010UL           /*!< Enable DC interrupt generation */
#define ADCSAR_DC_DCTL_CTM_Msk                0x00000300UL           /*!< DC output trigger mode */
#define ADCSAR_DC_DCTL_CTC_Msk                0x00000C00UL           /*!< DC output trigger compare conditions */
#define ADCSAR_DC_DCTL_CTE_Msk                0x00001000UL           /*!< Enable DC output trigger */
#define ADCSAR_DC_DCTL_CHNL_Msk               0x00070000UL           /*!< ADC channel selection */
#define ADCSAR_DC_DCTL_SRC_Msk                0x01000000UL           /*!< Select data source for comparation: ADC module (0) or sequencer(1) */
#define ADCSAR_DC_DCTL_HWTCLR_Msk             0x10000000UL           /*!< Hardware output trigger clear */

/* Bit field enums: */
typedef enum {
  ADCSAR_DC_DCTL_CIM_Multiple                = 0x0UL,                /*!< multiple trigger mode */
  ADCSAR_DC_DCTL_CIM_Single                  = 0x1UL,                /*!< single trigger mode */
  ADCSAR_DC_DCTL_CIM_MultipleHyst            = 0x2UL,                /*!< multiple trigger mode with hysteresis */
  ADCSAR_DC_DCTL_CIM_SingleHyst              = 0x3UL,                /*!< single trigger mode with hysteresis */
} ADCSAR_DC_DCTL_CIM_Enum;

typedef enum {
  ADCSAR_DC_DCTL_CIC_Low                     = 0x0UL,                /*!< result lower or equal COMP0 */
  ADCSAR_DC_DCTL_CIC_Window                  = 0x1UL,                /*!< result between COMP0 and COMP1 or equal any of them */
  ADCSAR_DC_DCTL_CIC_High                    = 0x2UL,                /*!< result higher or equal COMP1 */
} ADCSAR_DC_DCTL_CIC_Enum;

typedef enum {
  ADCSAR_DC_DCTL_CTM_Multiple                = 0x0UL,                /*!< multiple trigger mode */
  ADCSAR_DC_DCTL_CTM_Single                  = 0x1UL,                /*!< single trigger mode */
  ADCSAR_DC_DCTL_CTM_MultipleHyst            = 0x2UL,                /*!< multiple trigger mode with hysteresis */
  ADCSAR_DC_DCTL_CTM_SingleHyst              = 0x3UL,                /*!< single trigger mode with hysteresis */
} ADCSAR_DC_DCTL_CTM_Enum;

typedef enum {
  ADCSAR_DC_DCTL_CTC_Low                     = 0x0UL,                /*!< result lower or equal COMP0 */
  ADCSAR_DC_DCTL_CTC_Window                  = 0x1UL,                /*!< result between COMP0 and COMP1 or equal any of them */
  ADCSAR_DC_DCTL_CTC_High                    = 0x2UL,                /*!< result higher or equal COMP1 */
} ADCSAR_DC_DCTL_CTC_Enum;

/*-- DC: DCMP: Digital comparator range register --------------------------------------------------------------*/
typedef struct {
  uint32_t CMPL                   :12;                               /*!< Low threshold compare value */
  uint32_t                        :4;                                /*!< RESERVED */
  uint32_t CMPH                   :12;                               /*!< High threshold compare value */
} _ADCSAR_DC_DCMP_bits;

/* Bit field positions: */
#define ADCSAR_DC_DCMP_CMPL_Pos               0                      /*!< Low threshold compare value */
#define ADCSAR_DC_DCMP_CMPH_Pos               16                     /*!< High threshold compare value */

/* Bit field masks: */
#define ADCSAR_DC_DCMP_CMPL_Msk               0x00000FFFUL           /*!< Low threshold compare value */
#define ADCSAR_DC_DCMP_CMPH_Msk               0x0FFF0000UL           /*!< High threshold compare value */

/*-- DC: DDATA: Digital comparator last compared data register ------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :12;                               /*!< Value of last compared AD conversion result */
} _ADCSAR_DC_DDATA_bits;

/* Bit field positions: */
#define ADCSAR_DC_DDATA_VAL_Pos               0                      /*!< Value of last compared AD conversion result */

/* Bit field masks: */
#define ADCSAR_DC_DDATA_VAL_Msk               0x00000FFFUL           /*!< Value of last compared AD conversion result */

/*--  ACTL: ADC module control register -----------------------------------------------------------------------*/
typedef struct {
  uint32_t ADCEN                  :1;                                /*!< Enable ADC module */
  uint32_t ADCRDY                 :1;                                /*!< ADC ready for conversions */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t SELRES                 :2;                                /*!< ADC resolution select */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t CALEN                  :1;                                /*!<  */
  uint32_t CALLOAD                :1;                                /*!<  */
  uint32_t CALSTART               :1;                                /*!<  */
  uint32_t CALBUSY                :1;                                /*!< Enable ADC internal calibration */
  uint32_t                        :4;                                /*!< RESERVED */
  uint32_t CALIN                  :7;                                /*!< ADC calibration input value */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t CALOUT                 :7;                                /*!< ADC calibration output value */
} _ADCSAR_ACTL_bits;

/* Bit field positions: */
#define ADCSAR_ACTL_ADCEN_Pos                 0                      /*!< Enable ADC module */
#define ADCSAR_ACTL_ADCRDY_Pos                1                      /*!< ADC ready for conversions */
#define ADCSAR_ACTL_SELRES_Pos                4                      /*!< ADC resolution select */
#define ADCSAR_ACTL_CALEN_Pos                 8                      /*!<  */
#define ADCSAR_ACTL_CALLOAD_Pos               9                      /*!<  */
#define ADCSAR_ACTL_CALSTART_Pos              10                     /*!<  */
#define ADCSAR_ACTL_CALBUSY_Pos               11                     /*!< Enable ADC internal calibration */
#define ADCSAR_ACTL_CALIN_Pos                 16                     /*!< ADC calibration input value */
#define ADCSAR_ACTL_CALOUT_Pos                24                     /*!< ADC calibration output value */

/* Bit field masks: */
#define ADCSAR_ACTL_ADCEN_Msk                 0x00000001UL           /*!< Enable ADC module */
#define ADCSAR_ACTL_ADCRDY_Msk                0x00000002UL           /*!< ADC ready for conversions */
#define ADCSAR_ACTL_SELRES_Msk                0x00000030UL           /*!< ADC resolution select */
#define ADCSAR_ACTL_CALEN_Msk                 0x00000100UL           /*!<  */
#define ADCSAR_ACTL_CALLOAD_Msk               0x00000200UL           /*!<  */
#define ADCSAR_ACTL_CALSTART_Msk              0x00000400UL           /*!<  */
#define ADCSAR_ACTL_CALBUSY_Msk               0x00000800UL           /*!< Enable ADC internal calibration */
#define ADCSAR_ACTL_CALIN_Msk                 0x007F0000UL           /*!< ADC calibration input value */
#define ADCSAR_ACTL_CALOUT_Msk                0x7F000000UL           /*!< ADC calibration output value */

/* Bit field enums: */
typedef enum {
  ADCSAR_ACTL_SELRES_6bit                    = 0x0UL,                /*!< 6 bit mode */
  ADCSAR_ACTL_SELRES_8bit                    = 0x1UL,                /*!< 8 bit mode */
  ADCSAR_ACTL_SELRES_10bit                   = 0x2UL,                /*!< 10 bit mode */
  ADCSAR_ACTL_SELRES_12bit                   = 0x3UL,                /*!< 12 bit mode */
} ADCSAR_ACTL_SELRES_Enum;

/*--  CHCTL: ADC channel control register ---------------------------------------------------------------------*/
typedef struct {
  uint32_t OFFTRIM                :9;                                /*!< ADC channel offset trimm value */
  uint32_t                        :7;                                /*!< RESERVED */
  uint32_t GAINTRIM               :9;                                /*!< ADC channel gain trimm value */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t PRIORITY               :1;                                /*!< ADC channel priority level */
} _ADCSAR_CHCTL_bits;

/* Bit field positions: */
#define ADCSAR_CHCTL_OFFTRIM_Pos              0                      /*!< ADC channel offset trimm value */
#define ADCSAR_CHCTL_GAINTRIM_Pos             16                     /*!< ADC channel gain trimm value */
#define ADCSAR_CHCTL_PRIORITY_Pos             28                     /*!< ADC channel priority level */

/* Bit field masks: */
#define ADCSAR_CHCTL_OFFTRIM_Msk              0x000001FFUL           /*!< ADC channel offset trimm value */
#define ADCSAR_CHCTL_GAINTRIM_Msk             0x01FF0000UL           /*!< ADC channel gain trimm value */
#define ADCSAR_CHCTL_PRIORITY_Msk             0x10000000UL           /*!< ADC channel priority level */

/*--  CHDELAY: ADC channel delay register ---------------------------------------------------------------------*/
typedef struct {
  uint32_t DELAY                  :16;                               /*!< ADC channel delay value */
} _ADCSAR_CHDELAY_bits;

/* Bit field positions: */
#define ADCSAR_CHDELAY_DELAY_Pos              0                      /*!< ADC channel delay value */

/* Bit field masks: */
#define ADCSAR_CHDELAY_DELAY_Msk              0x0000FFFFUL           /*!< ADC channel delay value */

//Cluster SEQ:
typedef struct {
  union { 
  /*!< Sequencer request ADC channels selection register */
    __IO uint32_t SRQSEL;                                            /*!< SRQSEL : type used for word access */
    __IO _ADCSAR_SEQ_SRQSEL_bits SRQSEL_bit;                         /*!< SRQSEL_bit: structure used for bit access */
  };
    __IO uint32_t Reserved0[5];
  union { 
  /*!< Sequencer request control register */
    __IO uint32_t SRQCTL;                                            /*!< SRQCTL : type used for word access */
    __IO _ADCSAR_SEQ_SRQCTL_bits SRQCTL_bit;                         /*!< SRQCTL_bit: structure used for bit access */
  };
  union { 
  /*!< Sequencer request status register */
    __IO uint32_t SRQSTAT;                                           /*!< SRQSTAT : type used for word access */
    __IO _ADCSAR_SEQ_SRQSTAT_bits SRQSTAT_bit;                       /*!< SRQSTAT_bit: structure used for bit access */
  };
  union { 
  /*!< Sequencer DMA control register */
    __IO uint32_t SDMACTL;                                           /*!< SDMACTL : type used for word access */
    __IO _ADCSAR_SEQ_SDMACTL_bits SDMACTL_bit;                       /*!< SDMACTL_bit: structure used for bit access */
  };
  union { 
  /*!< Sequencer ADC interrupt and restart counter control register */
    __IO uint32_t SCCTL;                                             /*!< SCCTL : type used for word access */
    __IO _ADCSAR_SEQ_SCCTL_bits SCCTL_bit;                           /*!< SCCTL_bit: structure used for bit access */
  };
  union { 
  /*!< Sequencer ADC interrupt and restart counter current value register */
    __O uint32_t SCVAL;                                              /*!< SCVAL : type used for word access */
    __O _ADCSAR_SEQ_SCVAL_bits SCVAL_bit;                            /*!< SCVAL_bit: structure used for bit access */
  };
  union { 
  /*!< Sequencer digital comparator selection register */
    __IO uint32_t SDC;                                               /*!< SDC : type used for word access */
    __IO _ADCSAR_SEQ_SDC_bits SDC_bit;                               /*!< SDC_bit: structure used for bit access */
  };
  union { 
  /*!< Sequencer ADC restart timer register */
    __IO uint32_t SRTMR;                                             /*!< SRTMR : type used for word access */
    __IO _ADCSAR_SEQ_SRTMR_bits SRTMR_bit;                           /*!< SRTMR_bit: structure used for bit access */
  };
  union { 
  /*!< Sequencer FIFO load status register */
    __I uint32_t SFLOAD;                                             /*!< SFLOAD : type used for word access */
    __I _ADCSAR_SEQ_SFLOAD_bits SFLOAD_bit;                          /*!< SFLOAD_bit: structure used for bit access */
  };
  union { 
  /*!< Sequencer FIFO register */
    __I uint32_t SFIFO;                                              /*!< SFIFO : type used for word access */
    __I _ADCSAR_SEQ_SFIFO_bits SFIFO_bit;                            /*!< SFIFO_bit: structure used for bit access */
  };
    __IO uint32_t Reserved1;
} _ADCSAR_SEQ_TypeDef;
//Cluster DC:
typedef struct {
  union { 
  /*!< Digital comparator control register */
    __IO uint32_t DCTL;                                              /*!< DCTL : type used for word access */
    __IO _ADCSAR_DC_DCTL_bits DCTL_bit;                              /*!< DCTL_bit: structure used for bit access */
  };
  union { 
  /*!< Digital comparator range register */
    __IO uint32_t DCMP;                                              /*!< DCMP : type used for word access */
    __IO _ADCSAR_DC_DCMP_bits DCMP_bit;                              /*!< DCMP_bit: structure used for bit access */
  };
  union { 
  /*!< Digital comparator last compared data register */
    __I uint32_t DDATA;                                              /*!< DDATA : type used for word access */
    __I _ADCSAR_DC_DDATA_bits DDATA_bit;                             /*!< DDATA_bit: structure used for bit access */
  };
} _ADCSAR_DC_TypeDef;
//Cluster CHCTL:
typedef struct {
  union { 
  /*!< ADC channel control register */
    __IO uint32_t CHCTL;                                             /*!< CHCTL : type used for word access */
    __IO _ADCSAR_CHCTL_bits CHCTL_bit;                               /*!< CHCTL_bit: structure used for bit access */
  };
} _ADCSAR_CHCTL_TypeDef;
//Cluster CHDELAY:
typedef struct {
  union { 
  /*!< ADC channel delay register */
    __IO uint32_t CHDELAY;                                           /*!< CHDELAY : type used for word access */
    __IO _ADCSAR_CHDELAY_bits CHDELAY_bit;                           /*!< CHDELAY_bit: structure used for bit access */
  };
} _ADCSAR_CHDELAY_TypeDef;
typedef struct {
  union {                                                               /*!< Enable sequencer register */
    __IO uint32_t SEQEN;                                             /*!< SEQEN    : type used for word access */
    __IO _ADCSAR_SEQEN_bits  SEQEN_bit;                              /*!< SEQEN_bit: structure used for bit access */
  };
  union {                                                               /*!< Sequencer sync register */
    __IO uint32_t SEQSYNC;                                           /*!< SEQSYNC    : type used for word access */
    __IO _ADCSAR_SEQSYNC_bits  SEQSYNC_bit;                          /*!< SEQSYNC_bit: structure used for bit access */
  };
  union {                                                               /*!< FIFO overflow status register */
    __IO uint32_t FSTAT;                                             /*!< FSTAT    : type used for word access */
    __IO _ADCSAR_FSTAT_bits  FSTAT_bit;                              /*!< FSTAT_bit: structure used for bit access */
  };
  union {                                                               /*!< Busy status register */
    __I uint32_t BSTAT;                                              /*!< BSTAT    : type used for word access */
    __I _ADCSAR_BSTAT_bits  BSTAT_bit;                               /*!< BSTAT_bit: structure used for bit access */
  };
  union {                                                               /*!< Digital comparator output trigger status register */
    __O uint32_t DCTRIG;                                             /*!< DCTRIG    : type used for word access */
    __O _ADCSAR_DCTRIG_bits  DCTRIG_bit;                             /*!< DCTRIG_bit: structure used for bit access */
  };
  union {                                                               /*!< Digital comparator compare event status register */
    __O uint32_t DCEV;                                               /*!< DCEV    : type used for word access */
    __O _ADCSAR_DCEV_bits  DCEV_bit;                                 /*!< DCEV_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt counter clear control */
    __IO uint32_t CICNT;                                             /*!< CICNT    : type used for word access */
    __IO _ADCSAR_CICNT_bits  CICNT_bit;                              /*!< CICNT_bit: structure used for bit access */
  };
  union {                                                               /*!< Sequencer start event selection register */
    __IO uint32_t EMUX;                                              /*!< EMUX    : type used for word access */
    __IO _ADCSAR_EMUX_bits  EMUX_bit;                                /*!< EMUX_bit: structure used for bit access */
  };
  union {                                                               /*!< Raw interrupt status register */
    __I uint32_t RIS;                                                /*!< RIS    : type used for word access */
    __I _ADCSAR_RIS_bits  RIS_bit;                                   /*!< RIS_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt mask register */
    __IO uint32_t IM;                                                /*!< IM    : type used for word access */
    __IO _ADCSAR_IM_bits  IM_bit;                                    /*!< IM_bit: structure used for bit access */
  };
  union {                                                               /*!< Masked interrupt status and clear register */
    __I uint32_t MIS;                                                /*!< MIS    : type used for word access */
    __I _ADCSAR_MIS_bits  MIS_bit;                                   /*!< MIS_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt clear register */
    __O uint32_t IC;                                                 /*!< IC    : type used for word access */
    __O _ADCSAR_IC_bits  IC_bit;                                     /*!< IC_bit: structure used for bit access */
  };
    __IO uint32_t Reserved0[4];
  _ADCSAR_SEQ_TypeDef SEQ[2];
    __IO uint32_t Reserved1[208];
  _ADCSAR_DC_TypeDef DC[8];
    __IO uint32_t Reserved2[56];
  union {                                                               /*!< ADC module control register */
    __IO uint32_t ACTL;                                              /*!< ACTL    : type used for word access */
    __IO _ADCSAR_ACTL_bits  ACTL_bit;                                /*!< ACTL_bit: structure used for bit access */
  };
    __IO uint32_t Reserved3[15];
  _ADCSAR_CHCTL_TypeDef CHCTL[11];
    __IO uint32_t Reserved4[53];
  _ADCSAR_CHDELAY_TypeDef CHDELAY[11];
} ADCSAR_TypeDef;


/******************************************************************************/
/*                         ADCSD registers                                    */
/******************************************************************************/

/*--  CTRL: Enable sequencer register -------------------------------------------------------------------------*/
typedef struct {
  uint32_t ENB                    :1;                                /*!< reserved */
  uint32_t PUREF                  :1;                                /*!< reserved */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t RST                    :1;                                /*!< reserved */
  uint32_t SFRST                  :1;                                /*!< reserved */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t DR                     :2;                                /*!< reserved */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t MDC                    :3;                                /*!< reserved */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t WTCYC                  :3;                                /*!< reserved */
} _ADCSD_CTRL_bits;

/* Bit field positions: */
#define ADCSD_CTRL_ENB_Pos                    0                      /*!< reserved */
#define ADCSD_CTRL_PUREF_Pos                  1                      /*!< reserved */
#define ADCSD_CTRL_RST_Pos                    4                      /*!< reserved */
#define ADCSD_CTRL_SFRST_Pos                  5                      /*!< reserved */
#define ADCSD_CTRL_DR_Pos                     8                      /*!< reserved */
#define ADCSD_CTRL_MDC_Pos                    12                     /*!< reserved */
#define ADCSD_CTRL_WTCYC_Pos                  16                     /*!< reserved */

/* Bit field masks: */
#define ADCSD_CTRL_ENB_Msk                    0x00000001UL           /*!< reserved */
#define ADCSD_CTRL_PUREF_Msk                  0x00000002UL           /*!< reserved */
#define ADCSD_CTRL_RST_Msk                    0x00000010UL           /*!< reserved */
#define ADCSD_CTRL_SFRST_Msk                  0x00000020UL           /*!< reserved */
#define ADCSD_CTRL_DR_Msk                     0x00000300UL           /*!< reserved */
#define ADCSD_CTRL_MDC_Msk                    0x00007000UL           /*!< reserved */
#define ADCSD_CTRL_WTCYC_Msk                  0x00070000UL           /*!< reserved */

/*--  MODE: Sequencer sync register ---------------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :2;                                /*!< Inverce input data on Chanel0 */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t CH1                    :2;                                /*!< Inverce input data on Chanel4 */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t CH2                    :2;                                /*!< reserved */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t CH3                    :2;                                /*!< reserved */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t CH4                    :2;                                /*!< reserved */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t CH5                    :2;                                /*!< reserved */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t CH6                    :2;                                /*!< reserved */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t CH7                    :2;                                /*!< reserved */
} _ADCSD_MODE_bits;

/* Bit field positions: */
#define ADCSD_MODE_CH0_Pos                    0                      /*!< Inverce input data on Chanel0 */
#define ADCSD_MODE_CH1_Pos                    4                      /*!< Inverce input data on Chanel4 */
#define ADCSD_MODE_CH2_Pos                    8                      /*!< reserved */
#define ADCSD_MODE_CH3_Pos                    12                     /*!< reserved */
#define ADCSD_MODE_CH4_Pos                    16                     /*!< reserved */
#define ADCSD_MODE_CH5_Pos                    20                     /*!< reserved */
#define ADCSD_MODE_CH6_Pos                    24                     /*!< reserved */
#define ADCSD_MODE_CH7_Pos                    28                     /*!< reserved */

/* Bit field masks: */
#define ADCSD_MODE_CH0_Msk                    0x00000003UL           /*!< Inverce input data on Chanel0 */
#define ADCSD_MODE_CH1_Msk                    0x00000030UL           /*!< Inverce input data on Chanel4 */
#define ADCSD_MODE_CH2_Msk                    0x00000300UL           /*!< reserved */
#define ADCSD_MODE_CH3_Msk                    0x00003000UL           /*!< reserved */
#define ADCSD_MODE_CH4_Msk                    0x00030000UL           /*!< reserved */
#define ADCSD_MODE_CH5_Msk                    0x00300000UL           /*!< reserved */
#define ADCSD_MODE_CH6_Msk                    0x03000000UL           /*!< reserved */
#define ADCSD_MODE_CH7_Msk                    0x30000000UL           /*!< reserved */

/*--  AMPL: Chanel amplifier register -------------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :3;                                /*!< Amplifying for Chanel0 */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t CH1                    :3;                                /*!< Amplifying for Chanel1 */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t CH2                    :3;                                /*!< Amplifying for Chanel2 */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t CH3                    :3;                                /*!< Amplifying for Chanel3 */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t CH4                    :3;                                /*!< Amplifying for Chanel4 */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t CH5                    :3;                                /*!< Amplifying for Chanel5 */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t CH6                    :3;                                /*!< Amplifying for Chanel6 */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t CH7                    :3;                                /*!< Amplifying for Chanel7 */
} _ADCSD_AMPL_bits;

/* Bit field positions: */
#define ADCSD_AMPL_CH0_Pos                    0                      /*!< Amplifying for Chanel0 */
#define ADCSD_AMPL_CH1_Pos                    4                      /*!< Amplifying for Chanel1 */
#define ADCSD_AMPL_CH2_Pos                    8                      /*!< Amplifying for Chanel2 */
#define ADCSD_AMPL_CH3_Pos                    12                     /*!< Amplifying for Chanel3 */
#define ADCSD_AMPL_CH4_Pos                    16                     /*!< Amplifying for Chanel4 */
#define ADCSD_AMPL_CH5_Pos                    20                     /*!< Amplifying for Chanel5 */
#define ADCSD_AMPL_CH6_Pos                    24                     /*!< Amplifying for Chanel6 */
#define ADCSD_AMPL_CH7_Pos                    28                     /*!< Amplifying for Chanel7 */

/* Bit field masks: */
#define ADCSD_AMPL_CH0_Msk                    0x00000007UL           /*!< Amplifying for Chanel0 */
#define ADCSD_AMPL_CH1_Msk                    0x00000070UL           /*!< Amplifying for Chanel1 */
#define ADCSD_AMPL_CH2_Msk                    0x00000700UL           /*!< Amplifying for Chanel2 */
#define ADCSD_AMPL_CH3_Msk                    0x00007000UL           /*!< Amplifying for Chanel3 */
#define ADCSD_AMPL_CH4_Msk                    0x00070000UL           /*!< Amplifying for Chanel4 */
#define ADCSD_AMPL_CH5_Msk                    0x00700000UL           /*!< Amplifying for Chanel5 */
#define ADCSD_AMPL_CH6_Msk                    0x07000000UL           /*!< Amplifying for Chanel6 */
#define ADCSD_AMPL_CH7_Msk                    0x70000000UL           /*!< Amplifying for Chanel7 */

/*--  ENB: Enable conversion register -------------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!< Enable conversion on Chanel0 */
  uint32_t CH1                    :1;                                /*!< Enable conversion on Chanel1 */
  uint32_t CH2                    :1;                                /*!< Enable conversion on Chanel2 */
  uint32_t CH3                    :1;                                /*!< Enable conversion on Chanel3 */
  uint32_t CH4                    :1;                                /*!< Enable conversion on Chanel4 */
  uint32_t CH5                    :1;                                /*!< Enable conversion on Chanel5 */
  uint32_t CH6                    :1;                                /*!< Enable conversion on Chanel6 */
  uint32_t CH7                    :1;                                /*!< Enable conversion on Chanel7 */
} _ADCSD_ENB_bits;

/* Bit field positions: */
#define ADCSD_ENB_CH0_Pos                     0                      /*!< Enable conversion on Chanel0 */
#define ADCSD_ENB_CH1_Pos                     1                      /*!< Enable conversion on Chanel1 */
#define ADCSD_ENB_CH2_Pos                     2                      /*!< Enable conversion on Chanel2 */
#define ADCSD_ENB_CH3_Pos                     3                      /*!< Enable conversion on Chanel3 */
#define ADCSD_ENB_CH4_Pos                     4                      /*!< Enable conversion on Chanel4 */
#define ADCSD_ENB_CH5_Pos                     5                      /*!< Enable conversion on Chanel5 */
#define ADCSD_ENB_CH6_Pos                     6                      /*!< Enable conversion on Chanel6 */
#define ADCSD_ENB_CH7_Pos                     7                      /*!< Enable conversion on Chanel7 */

/* Bit field masks: */
#define ADCSD_ENB_CH0_Msk                     0x00000001UL           /*!< Enable conversion on Chanel0 */
#define ADCSD_ENB_CH1_Msk                     0x00000002UL           /*!< Enable conversion on Chanel1 */
#define ADCSD_ENB_CH2_Msk                     0x00000004UL           /*!< Enable conversion on Chanel2 */
#define ADCSD_ENB_CH3_Msk                     0x00000008UL           /*!< Enable conversion on Chanel3 */
#define ADCSD_ENB_CH4_Msk                     0x00000010UL           /*!< Enable conversion on Chanel4 */
#define ADCSD_ENB_CH5_Msk                     0x00000020UL           /*!< Enable conversion on Chanel5 */
#define ADCSD_ENB_CH6_Msk                     0x00000040UL           /*!< Enable conversion on Chanel6 */
#define ADCSD_ENB_CH7_Msk                     0x00000080UL           /*!< Enable conversion on Chanel7 */

/*--  DIFF: Enable differencial mode for channels -------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!< Enable conversion on Chanel0 */
  uint32_t CH1                    :1;                                /*!< Enable conversion on Chanel1 */
  uint32_t CH2                    :1;                                /*!< Enable conversion on Chanel2 */
  uint32_t CH3                    :1;                                /*!< Enable conversion on Chanel3 */
  uint32_t CH4                    :1;                                /*!< Enable conversion on Chanel4 */
  uint32_t CH5                    :1;                                /*!< Enable conversion on Chanel5 */
  uint32_t CH6                    :1;                                /*!< Enable conversion on Chanel6 */
  uint32_t CH7                    :1;                                /*!< Enable conversion on Chanel7 */
} _ADCSD_DIFF_bits;

/* Bit field positions: */
#define ADCSD_DIFF_CH0_Pos                    0                      /*!< Enable conversion on Chanel0 */
#define ADCSD_DIFF_CH1_Pos                    1                      /*!< Enable conversion on Chanel1 */
#define ADCSD_DIFF_CH2_Pos                    2                      /*!< Enable conversion on Chanel2 */
#define ADCSD_DIFF_CH3_Pos                    3                      /*!< Enable conversion on Chanel3 */
#define ADCSD_DIFF_CH4_Pos                    4                      /*!< Enable conversion on Chanel4 */
#define ADCSD_DIFF_CH5_Pos                    5                      /*!< Enable conversion on Chanel5 */
#define ADCSD_DIFF_CH6_Pos                    6                      /*!< Enable conversion on Chanel6 */
#define ADCSD_DIFF_CH7_Pos                    7                      /*!< Enable conversion on Chanel7 */

/* Bit field masks: */
#define ADCSD_DIFF_CH0_Msk                    0x00000001UL           /*!< Enable conversion on Chanel0 */
#define ADCSD_DIFF_CH1_Msk                    0x00000002UL           /*!< Enable conversion on Chanel1 */
#define ADCSD_DIFF_CH2_Msk                    0x00000004UL           /*!< Enable conversion on Chanel2 */
#define ADCSD_DIFF_CH3_Msk                    0x00000008UL           /*!< Enable conversion on Chanel3 */
#define ADCSD_DIFF_CH4_Msk                    0x00000010UL           /*!< Enable conversion on Chanel4 */
#define ADCSD_DIFF_CH5_Msk                    0x00000020UL           /*!< Enable conversion on Chanel5 */
#define ADCSD_DIFF_CH6_Msk                    0x00000040UL           /*!< Enable conversion on Chanel6 */
#define ADCSD_DIFF_CH7_Msk                    0x00000080UL           /*!< Enable conversion on Chanel7 */

/*--  READY: Ready for conversion result register -------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!< Status of Chanel 0 */
  uint32_t CH1                    :1;                                /*!< Status of Chanel 1 */
  uint32_t CH2                    :1;                                /*!< Status of Chanel 2 */
  uint32_t CH3                    :1;                                /*!< Status of Chanel 3 */
  uint32_t CH4                    :1;                                /*!< Status of Chanel 4 */
  uint32_t CH5                    :1;                                /*!< Status of Chanel 5 */
  uint32_t CH6                    :1;                                /*!< Status of Chanel 6 */
  uint32_t CH7                    :1;                                /*!< Status of Chanel 7 */
} _ADCSD_READY_bits;

/* Bit field positions: */
#define ADCSD_READY_CH0_Pos                   0                      /*!< Status of Chanel 0 */
#define ADCSD_READY_CH1_Pos                   1                      /*!< Status of Chanel 1 */
#define ADCSD_READY_CH2_Pos                   2                      /*!< Status of Chanel 2 */
#define ADCSD_READY_CH3_Pos                   3                      /*!< Status of Chanel 3 */
#define ADCSD_READY_CH4_Pos                   4                      /*!< Status of Chanel 4 */
#define ADCSD_READY_CH5_Pos                   5                      /*!< Status of Chanel 5 */
#define ADCSD_READY_CH6_Pos                   6                      /*!< Status of Chanel 6 */
#define ADCSD_READY_CH7_Pos                   7                      /*!< Status of Chanel 7 */

/* Bit field masks: */
#define ADCSD_READY_CH0_Msk                   0x00000001UL           /*!< Status of Chanel 0 */
#define ADCSD_READY_CH1_Msk                   0x00000002UL           /*!< Status of Chanel 1 */
#define ADCSD_READY_CH2_Msk                   0x00000004UL           /*!< Status of Chanel 2 */
#define ADCSD_READY_CH3_Msk                   0x00000008UL           /*!< Status of Chanel 3 */
#define ADCSD_READY_CH4_Msk                   0x00000010UL           /*!< Status of Chanel 4 */
#define ADCSD_READY_CH5_Msk                   0x00000020UL           /*!< Status of Chanel 5 */
#define ADCSD_READY_CH6_Msk                   0x00000040UL           /*!< Status of Chanel 6 */
#define ADCSD_READY_CH7_Msk                   0x00000080UL           /*!< Status of Chanel 7 */

/*--  DATAUPD: Flag of successful conversion ------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!<  */
  uint32_t CH1                    :1;                                /*!<  */
  uint32_t CH2                    :1;                                /*!<  */
  uint32_t CH3                    :1;                                /*!<  */
  uint32_t CH4                    :1;                                /*!<  */
  uint32_t CH5                    :1;                                /*!<  */
  uint32_t CH6                    :1;                                /*!<  */
  uint32_t CH7                    :1;                                /*!<  */
} _ADCSD_DATAUPD_bits;

/* Bit field positions: */
#define ADCSD_DATAUPD_CH0_Pos                 0                      /*!<  */
#define ADCSD_DATAUPD_CH1_Pos                 1                      /*!<  */
#define ADCSD_DATAUPD_CH2_Pos                 2                      /*!<  */
#define ADCSD_DATAUPD_CH3_Pos                 3                      /*!<  */
#define ADCSD_DATAUPD_CH4_Pos                 4                      /*!<  */
#define ADCSD_DATAUPD_CH5_Pos                 5                      /*!<  */
#define ADCSD_DATAUPD_CH6_Pos                 6                      /*!<  */
#define ADCSD_DATAUPD_CH7_Pos                 7                      /*!<  */

/* Bit field masks: */
#define ADCSD_DATAUPD_CH0_Msk                 0x00000001UL           /*!<  */
#define ADCSD_DATAUPD_CH1_Msk                 0x00000002UL           /*!<  */
#define ADCSD_DATAUPD_CH2_Msk                 0x00000004UL           /*!<  */
#define ADCSD_DATAUPD_CH3_Msk                 0x00000008UL           /*!<  */
#define ADCSD_DATAUPD_CH4_Msk                 0x00000010UL           /*!<  */
#define ADCSD_DATAUPD_CH5_Msk                 0x00000020UL           /*!<  */
#define ADCSD_DATAUPD_CH6_Msk                 0x00000040UL           /*!<  */
#define ADCSD_DATAUPD_CH7_Msk                 0x00000080UL           /*!<  */

/*--  IM: Interrupt mask --------------------------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!<  */
  uint32_t CH1                    :1;                                /*!<  */
  uint32_t CH2                    :1;                                /*!<  */
  uint32_t CH3                    :1;                                /*!<  */
  uint32_t CH4                    :1;                                /*!<  */
  uint32_t CH5                    :1;                                /*!<  */
  uint32_t CH6                    :1;                                /*!<  */
  uint32_t CH7                    :1;                                /*!<  */
} _ADCSD_IM_bits;

/* Bit field positions: */
#define ADCSD_IM_CH0_Pos                      0                      /*!<  */
#define ADCSD_IM_CH1_Pos                      1                      /*!<  */
#define ADCSD_IM_CH2_Pos                      2                      /*!<  */
#define ADCSD_IM_CH3_Pos                      3                      /*!<  */
#define ADCSD_IM_CH4_Pos                      4                      /*!<  */
#define ADCSD_IM_CH5_Pos                      5                      /*!<  */
#define ADCSD_IM_CH6_Pos                      6                      /*!<  */
#define ADCSD_IM_CH7_Pos                      7                      /*!<  */

/* Bit field masks: */
#define ADCSD_IM_CH0_Msk                      0x00000001UL           /*!<  */
#define ADCSD_IM_CH1_Msk                      0x00000002UL           /*!<  */
#define ADCSD_IM_CH2_Msk                      0x00000004UL           /*!<  */
#define ADCSD_IM_CH3_Msk                      0x00000008UL           /*!<  */
#define ADCSD_IM_CH4_Msk                      0x00000010UL           /*!<  */
#define ADCSD_IM_CH5_Msk                      0x00000020UL           /*!<  */
#define ADCSD_IM_CH6_Msk                      0x00000040UL           /*!<  */
#define ADCSD_IM_CH7_Msk                      0x00000080UL           /*!<  */

/*--  MIS: Masked Interrupt Status ----------------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!<  */
  uint32_t CH1                    :1;                                /*!<  */
  uint32_t CH2                    :1;                                /*!<  */
  uint32_t CH3                    :1;                                /*!<  */
  uint32_t CH4                    :1;                                /*!<  */
  uint32_t CH5                    :1;                                /*!<  */
  uint32_t CH6                    :1;                                /*!<  */
  uint32_t CH7                    :1;                                /*!<  */
} _ADCSD_MIS_bits;

/* Bit field positions: */
#define ADCSD_MIS_CH0_Pos                     0                      /*!<  */
#define ADCSD_MIS_CH1_Pos                     1                      /*!<  */
#define ADCSD_MIS_CH2_Pos                     2                      /*!<  */
#define ADCSD_MIS_CH3_Pos                     3                      /*!<  */
#define ADCSD_MIS_CH4_Pos                     4                      /*!<  */
#define ADCSD_MIS_CH5_Pos                     5                      /*!<  */
#define ADCSD_MIS_CH6_Pos                     6                      /*!<  */
#define ADCSD_MIS_CH7_Pos                     7                      /*!<  */

/* Bit field masks: */
#define ADCSD_MIS_CH0_Msk                     0x00000001UL           /*!<  */
#define ADCSD_MIS_CH1_Msk                     0x00000002UL           /*!<  */
#define ADCSD_MIS_CH2_Msk                     0x00000004UL           /*!<  */
#define ADCSD_MIS_CH3_Msk                     0x00000008UL           /*!<  */
#define ADCSD_MIS_CH4_Msk                     0x00000010UL           /*!<  */
#define ADCSD_MIS_CH5_Msk                     0x00000020UL           /*!<  */
#define ADCSD_MIS_CH6_Msk                     0x00000040UL           /*!<  */
#define ADCSD_MIS_CH7_Msk                     0x00000080UL           /*!<  */

/*--  IC: Interrupt Status Clear ------------------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!<  */
  uint32_t CH1                    :1;                                /*!<  */
  uint32_t CH2                    :1;                                /*!<  */
  uint32_t CH3                    :1;                                /*!<  */
  uint32_t CH4                    :1;                                /*!<  */
  uint32_t CH5                    :1;                                /*!<  */
  uint32_t CH6                    :1;                                /*!<  */
  uint32_t CH7                    :1;                                /*!<  */
} _ADCSD_IC_bits;

/* Bit field positions: */
#define ADCSD_IC_CH0_Pos                      0                      /*!<  */
#define ADCSD_IC_CH1_Pos                      1                      /*!<  */
#define ADCSD_IC_CH2_Pos                      2                      /*!<  */
#define ADCSD_IC_CH3_Pos                      3                      /*!<  */
#define ADCSD_IC_CH4_Pos                      4                      /*!<  */
#define ADCSD_IC_CH5_Pos                      5                      /*!<  */
#define ADCSD_IC_CH6_Pos                      6                      /*!<  */
#define ADCSD_IC_CH7_Pos                      7                      /*!<  */

/* Bit field masks: */
#define ADCSD_IC_CH0_Msk                      0x00000001UL           /*!<  */
#define ADCSD_IC_CH1_Msk                      0x00000002UL           /*!<  */
#define ADCSD_IC_CH2_Msk                      0x00000004UL           /*!<  */
#define ADCSD_IC_CH3_Msk                      0x00000008UL           /*!<  */
#define ADCSD_IC_CH4_Msk                      0x00000010UL           /*!<  */
#define ADCSD_IC_CH5_Msk                      0x00000020UL           /*!<  */
#define ADCSD_IC_CH6_Msk                      0x00000040UL           /*!<  */
#define ADCSD_IC_CH7_Msk                      0x00000080UL           /*!<  */

/*--  DATA: ADC Conversion data -------------------------------------------------------------------------------*/
typedef struct {
  uint32_t DATA                   :16;                               /*!<  */
} _ADCSD_DATA_bits;

/* Bit field positions: */
#define ADCSD_DATA_DATA_Pos                   0                      /*!<  */

/* Bit field masks: */
#define ADCSD_DATA_DATA_Msk                   0x0000FFFFUL           /*!<  */

//Cluster DATA:
typedef struct {
  union { 
  /*!< ADC Conversion data */
    __I uint32_t DATA;                                               /*!< DATA : type used for word access */
    __I _ADCSD_DATA_bits DATA_bit;                                   /*!< DATA_bit: structure used for bit access */
  };
} _ADCSD_DATA_TypeDef;
typedef struct {
  union {                                                               /*!< Enable sequencer register */
    __IO uint32_t CTRL;                                              /*!< CTRL    : type used for word access */
    __IO _ADCSD_CTRL_bits  CTRL_bit;                                 /*!< CTRL_bit: structure used for bit access */
  };
  union {                                                               /*!< Sequencer sync register */
    __IO uint32_t MODE;                                              /*!< MODE    : type used for word access */
    __IO _ADCSD_MODE_bits  MODE_bit;                                 /*!< MODE_bit: structure used for bit access */
  };
  union {                                                               /*!< Chanel amplifier register */
    __IO uint32_t AMPL;                                              /*!< AMPL    : type used for word access */
    __IO _ADCSD_AMPL_bits  AMPL_bit;                                 /*!< AMPL_bit: structure used for bit access */
  };
  union {                                                               /*!< Enable conversion register */
    __IO uint32_t ENB;                                               /*!< ENB    : type used for word access */
    __IO _ADCSD_ENB_bits  ENB_bit;                                   /*!< ENB_bit: structure used for bit access */
  };
  union {                                                               /*!< Enable differencial mode for channels */
    __IO uint32_t DIFF;                                              /*!< DIFF    : type used for word access */
    __IO _ADCSD_DIFF_bits  DIFF_bit;                                 /*!< DIFF_bit: structure used for bit access */
  };
  union {                                                               /*!< Ready for conversion result register */
    __I uint32_t READY;                                              /*!< READY    : type used for word access */
    __I _ADCSD_READY_bits  READY_bit;                                /*!< READY_bit: structure used for bit access */
  };
  union {                                                               /*!< Flag of successful conversion */
    __IO uint32_t DATAUPD;                                           /*!< DATAUPD    : type used for word access */
    __IO _ADCSD_DATAUPD_bits  DATAUPD_bit;                           /*!< DATAUPD_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt mask */
    __IO uint32_t IM;                                                /*!< IM    : type used for word access */
    __IO _ADCSD_IM_bits  IM_bit;                                     /*!< IM_bit: structure used for bit access */
  };
  union {                                                               /*!< Masked Interrupt Status */
    __I uint32_t MIS;                                                /*!< MIS    : type used for word access */
    __I _ADCSD_MIS_bits  MIS_bit;                                    /*!< MIS_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt Status Clear */
    __O uint32_t IC;                                                 /*!< IC    : type used for word access */
    __O _ADCSD_IC_bits  IC_bit;                                      /*!< IC_bit: structure used for bit access */
  };
    __IO uint32_t Reserved0[6];
  _ADCSD_DATA_TypeDef DATA[8];
} ADCSD_TypeDef;


/******************************************************************************/
/*                         GPIO registers                                     */
/******************************************************************************/

/*--  DATA: Data Input register -------------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< Data input */
} _GPIO_DATA_bits;

/* Bit field positions: */
#define GPIO_DATA_VAL_Pos                     0                      /*!< Data input */

/* Bit field masks: */
#define GPIO_DATA_VAL_Msk                     0x0000FFFFUL           /*!< Data input */

/*--  DATAOUT: Data output register ---------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< Data output */
} _GPIO_DATAOUT_bits;

/* Bit field positions: */
#define GPIO_DATAOUT_VAL_Pos                  0                      /*!< Data output */

/* Bit field masks: */
#define GPIO_DATAOUT_VAL_Msk                  0x0000FFFFUL           /*!< Data output */

/*--  DATAOUTSET: Data output set bits register ---------------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :1;                                /*!< Data output set bit 0 */
  uint32_t PIN1                   :1;                                /*!< Data output set bit 1 */
  uint32_t PIN2                   :1;                                /*!< Data output set bit 2 */
  uint32_t PIN3                   :1;                                /*!< Data output set bit 3 */
  uint32_t PIN4                   :1;                                /*!< Data output set bit 4 */
  uint32_t PIN5                   :1;                                /*!< Data output set bit 5 */
  uint32_t PIN6                   :1;                                /*!< Data output set bit 6 */
  uint32_t PIN7                   :1;                                /*!< Data output set bit 7 */
  uint32_t PIN8                   :1;                                /*!< Data output set bit 8 */
  uint32_t PIN9                   :1;                                /*!< Data output set bit 9 */
  uint32_t PIN10                  :1;                                /*!< Data output set bit 10 */
  uint32_t PIN11                  :1;                                /*!< Data output set bit 11 */
  uint32_t PIN12                  :1;                                /*!< Data output set bit 12 */
  uint32_t PIN13                  :1;                                /*!< Data output set bit 13 */
  uint32_t PIN14                  :1;                                /*!< Data output set bit 14 */
  uint32_t PIN15                  :1;                                /*!< Data output set bit 15 */
} _GPIO_DATAOUTSET_bits;

/* Bit field positions: */
#define GPIO_DATAOUTSET_PIN0_Pos              0                      /*!< Data output set bit 0 */
#define GPIO_DATAOUTSET_PIN1_Pos              1                      /*!< Data output set bit 1 */
#define GPIO_DATAOUTSET_PIN2_Pos              2                      /*!< Data output set bit 2 */
#define GPIO_DATAOUTSET_PIN3_Pos              3                      /*!< Data output set bit 3 */
#define GPIO_DATAOUTSET_PIN4_Pos              4                      /*!< Data output set bit 4 */
#define GPIO_DATAOUTSET_PIN5_Pos              5                      /*!< Data output set bit 5 */
#define GPIO_DATAOUTSET_PIN6_Pos              6                      /*!< Data output set bit 6 */
#define GPIO_DATAOUTSET_PIN7_Pos              7                      /*!< Data output set bit 7 */
#define GPIO_DATAOUTSET_PIN8_Pos              8                      /*!< Data output set bit 8 */
#define GPIO_DATAOUTSET_PIN9_Pos              9                      /*!< Data output set bit 9 */
#define GPIO_DATAOUTSET_PIN10_Pos             10                     /*!< Data output set bit 10 */
#define GPIO_DATAOUTSET_PIN11_Pos             11                     /*!< Data output set bit 11 */
#define GPIO_DATAOUTSET_PIN12_Pos             12                     /*!< Data output set bit 12 */
#define GPIO_DATAOUTSET_PIN13_Pos             13                     /*!< Data output set bit 13 */
#define GPIO_DATAOUTSET_PIN14_Pos             14                     /*!< Data output set bit 14 */
#define GPIO_DATAOUTSET_PIN15_Pos             15                     /*!< Data output set bit 15 */

/* Bit field masks: */
#define GPIO_DATAOUTSET_PIN0_Msk              0x00000001UL           /*!< Data output set bit 0 */
#define GPIO_DATAOUTSET_PIN1_Msk              0x00000002UL           /*!< Data output set bit 1 */
#define GPIO_DATAOUTSET_PIN2_Msk              0x00000004UL           /*!< Data output set bit 2 */
#define GPIO_DATAOUTSET_PIN3_Msk              0x00000008UL           /*!< Data output set bit 3 */
#define GPIO_DATAOUTSET_PIN4_Msk              0x00000010UL           /*!< Data output set bit 4 */
#define GPIO_DATAOUTSET_PIN5_Msk              0x00000020UL           /*!< Data output set bit 5 */
#define GPIO_DATAOUTSET_PIN6_Msk              0x00000040UL           /*!< Data output set bit 6 */
#define GPIO_DATAOUTSET_PIN7_Msk              0x00000080UL           /*!< Data output set bit 7 */
#define GPIO_DATAOUTSET_PIN8_Msk              0x00000100UL           /*!< Data output set bit 8 */
#define GPIO_DATAOUTSET_PIN9_Msk              0x00000200UL           /*!< Data output set bit 9 */
#define GPIO_DATAOUTSET_PIN10_Msk             0x00000400UL           /*!< Data output set bit 10 */
#define GPIO_DATAOUTSET_PIN11_Msk             0x00000800UL           /*!< Data output set bit 11 */
#define GPIO_DATAOUTSET_PIN12_Msk             0x00001000UL           /*!< Data output set bit 12 */
#define GPIO_DATAOUTSET_PIN13_Msk             0x00002000UL           /*!< Data output set bit 13 */
#define GPIO_DATAOUTSET_PIN14_Msk             0x00004000UL           /*!< Data output set bit 14 */
#define GPIO_DATAOUTSET_PIN15_Msk             0x00008000UL           /*!< Data output set bit 15 */

/*--  DATAOUTCLR: Data output clear bits register -------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< Data output clear */
} _GPIO_DATAOUTCLR_bits;

/* Bit field positions: */
#define GPIO_DATAOUTCLR_VAL_Pos               0                      /*!< Data output clear */

/* Bit field masks: */
#define GPIO_DATAOUTCLR_VAL_Msk               0x0000FFFFUL           /*!< Data output clear */

/*--  DATAOUTTGL: Data output toggle bits register ------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< Data output toggle */
} _GPIO_DATAOUTTGL_bits;

/* Bit field positions: */
#define GPIO_DATAOUTTGL_VAL_Pos               0                      /*!< Data output toggle */

/* Bit field masks: */
#define GPIO_DATAOUTTGL_VAL_Msk               0x0000FFFFUL           /*!< Data output toggle */

/*--  DENSET: Digital function (PAD) enable register ----------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :1;                                /*!< Digital function (PAD) enable on pin 0 */
  uint32_t PIN1                   :1;                                /*!< Digital function (PAD) enable on pin 1 */
  uint32_t PIN2                   :1;                                /*!< Digital function (PAD) enable on pin 2 */
  uint32_t PIN3                   :1;                                /*!< Digital function (PAD) enable on pin 3 */
  uint32_t PIN4                   :1;                                /*!< Digital function (PAD) enable on pin 4 */
  uint32_t PIN5                   :1;                                /*!< Digital function (PAD) enable on pin 5 */
  uint32_t PIN6                   :1;                                /*!< Digital function (PAD) enable on pin 6 */
  uint32_t PIN7                   :1;                                /*!< Digital function (PAD) enable on pin 7 */
  uint32_t PIN8                   :1;                                /*!< Digital function (PAD) enable on pin 8 */
  uint32_t PIN9                   :1;                                /*!< Digital function (PAD) enable on pin 9 */
  uint32_t PIN10                  :1;                                /*!< Digital function (PAD) enable on pin 10 */
  uint32_t PIN11                  :1;                                /*!< Digital function (PAD) enable on pin 11 */
  uint32_t PIN12                  :1;                                /*!< Digital function (PAD) enable on pin 12 */
  uint32_t PIN13                  :1;                                /*!< Digital function (PAD) enable on pin 13 */
  uint32_t PIN14                  :1;                                /*!< Digital function (PAD) enable on pin 14 */
  uint32_t PIN15                  :1;                                /*!< Digital function (PAD) enable on pin 15 */
} _GPIO_DENSET_bits;

/* Bit field positions: */
#define GPIO_DENSET_PIN0_Pos                  0                      /*!< Digital function (PAD) enable on pin 0 */
#define GPIO_DENSET_PIN1_Pos                  1                      /*!< Digital function (PAD) enable on pin 1 */
#define GPIO_DENSET_PIN2_Pos                  2                      /*!< Digital function (PAD) enable on pin 2 */
#define GPIO_DENSET_PIN3_Pos                  3                      /*!< Digital function (PAD) enable on pin 3 */
#define GPIO_DENSET_PIN4_Pos                  4                      /*!< Digital function (PAD) enable on pin 4 */
#define GPIO_DENSET_PIN5_Pos                  5                      /*!< Digital function (PAD) enable on pin 5 */
#define GPIO_DENSET_PIN6_Pos                  6                      /*!< Digital function (PAD) enable on pin 6 */
#define GPIO_DENSET_PIN7_Pos                  7                      /*!< Digital function (PAD) enable on pin 7 */
#define GPIO_DENSET_PIN8_Pos                  8                      /*!< Digital function (PAD) enable on pin 8 */
#define GPIO_DENSET_PIN9_Pos                  9                      /*!< Digital function (PAD) enable on pin 9 */
#define GPIO_DENSET_PIN10_Pos                 10                     /*!< Digital function (PAD) enable on pin 10 */
#define GPIO_DENSET_PIN11_Pos                 11                     /*!< Digital function (PAD) enable on pin 11 */
#define GPIO_DENSET_PIN12_Pos                 12                     /*!< Digital function (PAD) enable on pin 12 */
#define GPIO_DENSET_PIN13_Pos                 13                     /*!< Digital function (PAD) enable on pin 13 */
#define GPIO_DENSET_PIN14_Pos                 14                     /*!< Digital function (PAD) enable on pin 14 */
#define GPIO_DENSET_PIN15_Pos                 15                     /*!< Digital function (PAD) enable on pin 15 */

/* Bit field masks: */
#define GPIO_DENSET_PIN0_Msk                  0x00000001UL           /*!< Digital function (PAD) enable on pin 0 */
#define GPIO_DENSET_PIN1_Msk                  0x00000002UL           /*!< Digital function (PAD) enable on pin 1 */
#define GPIO_DENSET_PIN2_Msk                  0x00000004UL           /*!< Digital function (PAD) enable on pin 2 */
#define GPIO_DENSET_PIN3_Msk                  0x00000008UL           /*!< Digital function (PAD) enable on pin 3 */
#define GPIO_DENSET_PIN4_Msk                  0x00000010UL           /*!< Digital function (PAD) enable on pin 4 */
#define GPIO_DENSET_PIN5_Msk                  0x00000020UL           /*!< Digital function (PAD) enable on pin 5 */
#define GPIO_DENSET_PIN6_Msk                  0x00000040UL           /*!< Digital function (PAD) enable on pin 6 */
#define GPIO_DENSET_PIN7_Msk                  0x00000080UL           /*!< Digital function (PAD) enable on pin 7 */
#define GPIO_DENSET_PIN8_Msk                  0x00000100UL           /*!< Digital function (PAD) enable on pin 8 */
#define GPIO_DENSET_PIN9_Msk                  0x00000200UL           /*!< Digital function (PAD) enable on pin 9 */
#define GPIO_DENSET_PIN10_Msk                 0x00000400UL           /*!< Digital function (PAD) enable on pin 10 */
#define GPIO_DENSET_PIN11_Msk                 0x00000800UL           /*!< Digital function (PAD) enable on pin 11 */
#define GPIO_DENSET_PIN12_Msk                 0x00001000UL           /*!< Digital function (PAD) enable on pin 12 */
#define GPIO_DENSET_PIN13_Msk                 0x00002000UL           /*!< Digital function (PAD) enable on pin 13 */
#define GPIO_DENSET_PIN14_Msk                 0x00004000UL           /*!< Digital function (PAD) enable on pin 14 */
#define GPIO_DENSET_PIN15_Msk                 0x00008000UL           /*!< Digital function (PAD) enable on pin 15 */

/*--  DENCLR: Digital function (PAD) disable register ---------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< Digital function (PAD) disable */
} _GPIO_DENCLR_bits;

/* Bit field positions: */
#define GPIO_DENCLR_VAL_Pos                   0                      /*!< Digital function (PAD) disable */

/* Bit field masks: */
#define GPIO_DENCLR_VAL_Msk                   0x0000FFFFUL           /*!< Digital function (PAD) disable */

/*--  INMODE: Select input mode register ----------------------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :2;                                /*!< Select input mode for pin 0 */
  uint32_t PIN1                   :2;                                /*!< Select input mode for pin 1 */
  uint32_t PIN2                   :2;                                /*!< Select input mode for pin 2 */
  uint32_t PIN3                   :2;                                /*!< Select input mode for pin 3 */
  uint32_t PIN4                   :2;                                /*!< Select input mode for pin 4 */
  uint32_t PIN5                   :2;                                /*!< Select input mode for pin 5 */
  uint32_t PIN6                   :2;                                /*!< Select input mode for pin 6 */
  uint32_t PIN7                   :2;                                /*!< Select input mode for pin 7 */
  uint32_t PIN8                   :2;                                /*!< Select input mode for pin 8 */
  uint32_t PIN9                   :2;                                /*!< Select input mode for pin 9 */
  uint32_t PIN10                  :2;                                /*!< Select input mode for pin 10 */
  uint32_t PIN11                  :2;                                /*!< Select input mode for pin 11 */
  uint32_t PIN12                  :2;                                /*!< Select input mode for pin 12 */
  uint32_t PIN13                  :2;                                /*!< Select input mode for pin 13 */
  uint32_t PIN14                  :2;                                /*!< Select input mode for pin 14 */
  uint32_t PIN15                  :2;                                /*!< Select input mode for pin 15 */
} _GPIO_INMODE_bits;

/* Bit field positions: */
#define GPIO_INMODE_PIN0_Pos                  0                      /*!< Select input mode for pin 0 */
#define GPIO_INMODE_PIN1_Pos                  2                      /*!< Select input mode for pin 1 */
#define GPIO_INMODE_PIN2_Pos                  4                      /*!< Select input mode for pin 2 */
#define GPIO_INMODE_PIN3_Pos                  6                      /*!< Select input mode for pin 3 */
#define GPIO_INMODE_PIN4_Pos                  8                      /*!< Select input mode for pin 4 */
#define GPIO_INMODE_PIN5_Pos                  10                     /*!< Select input mode for pin 5 */
#define GPIO_INMODE_PIN6_Pos                  12                     /*!< Select input mode for pin 6 */
#define GPIO_INMODE_PIN7_Pos                  14                     /*!< Select input mode for pin 7 */
#define GPIO_INMODE_PIN8_Pos                  16                     /*!< Select input mode for pin 8 */
#define GPIO_INMODE_PIN9_Pos                  18                     /*!< Select input mode for pin 9 */
#define GPIO_INMODE_PIN10_Pos                 20                     /*!< Select input mode for pin 10 */
#define GPIO_INMODE_PIN11_Pos                 22                     /*!< Select input mode for pin 11 */
#define GPIO_INMODE_PIN12_Pos                 24                     /*!< Select input mode for pin 12 */
#define GPIO_INMODE_PIN13_Pos                 26                     /*!< Select input mode for pin 13 */
#define GPIO_INMODE_PIN14_Pos                 28                     /*!< Select input mode for pin 14 */
#define GPIO_INMODE_PIN15_Pos                 30                     /*!< Select input mode for pin 15 */

/* Bit field masks: */
#define GPIO_INMODE_PIN0_Msk                  0x00000003UL           /*!< Select input mode for pin 0 */
#define GPIO_INMODE_PIN1_Msk                  0x0000000CUL           /*!< Select input mode for pin 1 */
#define GPIO_INMODE_PIN2_Msk                  0x00000030UL           /*!< Select input mode for pin 2 */
#define GPIO_INMODE_PIN3_Msk                  0x000000C0UL           /*!< Select input mode for pin 3 */
#define GPIO_INMODE_PIN4_Msk                  0x00000300UL           /*!< Select input mode for pin 4 */
#define GPIO_INMODE_PIN5_Msk                  0x00000C00UL           /*!< Select input mode for pin 5 */
#define GPIO_INMODE_PIN6_Msk                  0x00003000UL           /*!< Select input mode for pin 6 */
#define GPIO_INMODE_PIN7_Msk                  0x0000C000UL           /*!< Select input mode for pin 7 */
#define GPIO_INMODE_PIN8_Msk                  0x00030000UL           /*!< Select input mode for pin 8 */
#define GPIO_INMODE_PIN9_Msk                  0x000C0000UL           /*!< Select input mode for pin 9 */
#define GPIO_INMODE_PIN10_Msk                 0x00300000UL           /*!< Select input mode for pin 10 */
#define GPIO_INMODE_PIN11_Msk                 0x00C00000UL           /*!< Select input mode for pin 11 */
#define GPIO_INMODE_PIN12_Msk                 0x03000000UL           /*!< Select input mode for pin 12 */
#define GPIO_INMODE_PIN13_Msk                 0x0C000000UL           /*!< Select input mode for pin 13 */
#define GPIO_INMODE_PIN14_Msk                 0x30000000UL           /*!< Select input mode for pin 14 */
#define GPIO_INMODE_PIN15_Msk                 0xC0000000UL           /*!< Select input mode for pin 15 */

/* Bit field enums: */
typedef enum {
  GPIO_INMODE_PIN0_Schmitt                   = 0x0UL,                /*!< Scmitt buffer */
  GPIO_INMODE_PIN0_Disable                   = 0x3UL,                /*!< Input buffer disabled */
} GPIO_INMODE_PIN0_Enum;

typedef enum {
  GPIO_INMODE_PIN1_Schmitt                   = 0x0UL,                /*!< Scmitt buffer */
  GPIO_INMODE_PIN1_Disable                   = 0x3UL,                /*!< Input buffer disabled */
} GPIO_INMODE_PIN1_Enum;

typedef enum {
  GPIO_INMODE_PIN2_Schmitt                   = 0x0UL,                /*!< Scmitt buffer */
  GPIO_INMODE_PIN2_Disable                   = 0x3UL,                /*!< Input buffer disabled */
} GPIO_INMODE_PIN2_Enum;

typedef enum {
  GPIO_INMODE_PIN3_Schmitt                   = 0x0UL,                /*!< Scmitt buffer */
  GPIO_INMODE_PIN3_Disable                   = 0x3UL,                /*!< Input buffer disabled */
} GPIO_INMODE_PIN3_Enum;

typedef enum {
  GPIO_INMODE_PIN4_Schmitt                   = 0x0UL,                /*!< Scmitt buffer */
  GPIO_INMODE_PIN4_Disable                   = 0x3UL,                /*!< Input buffer disabled */
} GPIO_INMODE_PIN4_Enum;

typedef enum {
  GPIO_INMODE_PIN5_Schmitt                   = 0x0UL,                /*!< Scmitt buffer */
  GPIO_INMODE_PIN5_Disable                   = 0x3UL,                /*!< Input buffer disabled */
} GPIO_INMODE_PIN5_Enum;

typedef enum {
  GPIO_INMODE_PIN6_Schmitt                   = 0x0UL,                /*!< Scmitt buffer */
  GPIO_INMODE_PIN6_Disable                   = 0x3UL,                /*!< Input buffer disabled */
} GPIO_INMODE_PIN6_Enum;

typedef enum {
  GPIO_INMODE_PIN7_Schmitt                   = 0x0UL,                /*!< Scmitt buffer */
  GPIO_INMODE_PIN7_Disable                   = 0x3UL,                /*!< Input buffer disabled */
} GPIO_INMODE_PIN7_Enum;

typedef enum {
  GPIO_INMODE_PIN8_Schmitt                   = 0x0UL,                /*!< Scmitt buffer */
  GPIO_INMODE_PIN8_Disable                   = 0x3UL,                /*!< Input buffer disabled */
} GPIO_INMODE_PIN8_Enum;

typedef enum {
  GPIO_INMODE_PIN9_Schmitt                   = 0x0UL,                /*!< Scmitt buffer */
  GPIO_INMODE_PIN9_Disable                   = 0x3UL,                /*!< Input buffer disabled */
} GPIO_INMODE_PIN9_Enum;

typedef enum {
  GPIO_INMODE_PIN10_Schmitt                  = 0x0UL,                /*!< Scmitt buffer */
  GPIO_INMODE_PIN10_Disable                  = 0x3UL,                /*!< Input buffer disabled */
} GPIO_INMODE_PIN10_Enum;

typedef enum {
  GPIO_INMODE_PIN11_Schmitt                  = 0x0UL,                /*!< Scmitt buffer */
  GPIO_INMODE_PIN11_Disable                  = 0x3UL,                /*!< Input buffer disabled */
} GPIO_INMODE_PIN11_Enum;

typedef enum {
  GPIO_INMODE_PIN12_Schmitt                  = 0x0UL,                /*!< Scmitt buffer */
  GPIO_INMODE_PIN12_Disable                  = 0x3UL,                /*!< Input buffer disabled */
} GPIO_INMODE_PIN12_Enum;

typedef enum {
  GPIO_INMODE_PIN13_Schmitt                  = 0x0UL,                /*!< Scmitt buffer */
  GPIO_INMODE_PIN13_Disable                  = 0x3UL,                /*!< Input buffer disabled */
} GPIO_INMODE_PIN13_Enum;

typedef enum {
  GPIO_INMODE_PIN14_Schmitt                  = 0x0UL,                /*!< Scmitt buffer */
  GPIO_INMODE_PIN14_Disable                  = 0x3UL,                /*!< Input buffer disabled */
} GPIO_INMODE_PIN14_Enum;

typedef enum {
  GPIO_INMODE_PIN15_Schmitt                  = 0x0UL,                /*!< Scmitt buffer */
  GPIO_INMODE_PIN15_Disable                  = 0x3UL,                /*!< Input buffer disabled */
} GPIO_INMODE_PIN15_Enum;

/*--  PULLMODE: Select pull mode register ---------------------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :1;                                /*!< Select pull mode for pin 0 */
  uint32_t PIN1                   :1;                                /*!< Select pull mode for pin 1 */
  uint32_t PIN2                   :1;                                /*!< Select pull mode for pin 2 */
  uint32_t PIN3                   :1;                                /*!< Select pull mode for pin 3 */
  uint32_t PIN4                   :1;                                /*!< Select pull mode for pin 4 */
  uint32_t PIN5                   :1;                                /*!< Select pull mode for pin 5 */
  uint32_t PIN6                   :1;                                /*!< Select pull mode for pin 6 */
  uint32_t PIN7                   :1;                                /*!< Select pull mode for pin 7 */
  uint32_t PIN8                   :1;                                /*!< Select pull mode for pin 8 */
  uint32_t PIN9                   :1;                                /*!< Select pull mode for pin 9 */
  uint32_t PIN10                  :1;                                /*!< Select pull mode for pin 10 */
  uint32_t PIN11                  :1;                                /*!< Select pull mode for pin 11 */
  uint32_t PIN12                  :1;                                /*!< Select pull mode for pin 12 */
  uint32_t PIN13                  :1;                                /*!< Select pull mode for pin 13 */
  uint32_t PIN14                  :1;                                /*!< Select pull mode for pin 14 */
  uint32_t PIN15                  :1;                                /*!< Select pull mode for pin 15 */
} _GPIO_PULLMODE_bits;

/* Bit field positions: */
#define GPIO_PULLMODE_PIN0_Pos                0                      /*!< Select pull mode for pin 0 */
#define GPIO_PULLMODE_PIN1_Pos                1                      /*!< Select pull mode for pin 1 */
#define GPIO_PULLMODE_PIN2_Pos                2                      /*!< Select pull mode for pin 2 */
#define GPIO_PULLMODE_PIN3_Pos                3                      /*!< Select pull mode for pin 3 */
#define GPIO_PULLMODE_PIN4_Pos                4                      /*!< Select pull mode for pin 4 */
#define GPIO_PULLMODE_PIN5_Pos                5                      /*!< Select pull mode for pin 5 */
#define GPIO_PULLMODE_PIN6_Pos                6                      /*!< Select pull mode for pin 6 */
#define GPIO_PULLMODE_PIN7_Pos                7                      /*!< Select pull mode for pin 7 */
#define GPIO_PULLMODE_PIN8_Pos                8                      /*!< Select pull mode for pin 8 */
#define GPIO_PULLMODE_PIN9_Pos                9                      /*!< Select pull mode for pin 9 */
#define GPIO_PULLMODE_PIN10_Pos               10                     /*!< Select pull mode for pin 10 */
#define GPIO_PULLMODE_PIN11_Pos               11                     /*!< Select pull mode for pin 11 */
#define GPIO_PULLMODE_PIN12_Pos               12                     /*!< Select pull mode for pin 12 */
#define GPIO_PULLMODE_PIN13_Pos               13                     /*!< Select pull mode for pin 13 */
#define GPIO_PULLMODE_PIN14_Pos               14                     /*!< Select pull mode for pin 14 */
#define GPIO_PULLMODE_PIN15_Pos               15                     /*!< Select pull mode for pin 15 */

/* Bit field masks: */
#define GPIO_PULLMODE_PIN0_Msk                0x00000001UL           /*!< Select pull mode for pin 0 */
#define GPIO_PULLMODE_PIN1_Msk                0x00000002UL           /*!< Select pull mode for pin 1 */
#define GPIO_PULLMODE_PIN2_Msk                0x00000004UL           /*!< Select pull mode for pin 2 */
#define GPIO_PULLMODE_PIN3_Msk                0x00000008UL           /*!< Select pull mode for pin 3 */
#define GPIO_PULLMODE_PIN4_Msk                0x00000010UL           /*!< Select pull mode for pin 4 */
#define GPIO_PULLMODE_PIN5_Msk                0x00000020UL           /*!< Select pull mode for pin 5 */
#define GPIO_PULLMODE_PIN6_Msk                0x00000040UL           /*!< Select pull mode for pin 6 */
#define GPIO_PULLMODE_PIN7_Msk                0x00000080UL           /*!< Select pull mode for pin 7 */
#define GPIO_PULLMODE_PIN8_Msk                0x00000100UL           /*!< Select pull mode for pin 8 */
#define GPIO_PULLMODE_PIN9_Msk                0x00000200UL           /*!< Select pull mode for pin 9 */
#define GPIO_PULLMODE_PIN10_Msk               0x00000400UL           /*!< Select pull mode for pin 10 */
#define GPIO_PULLMODE_PIN11_Msk               0x00000800UL           /*!< Select pull mode for pin 11 */
#define GPIO_PULLMODE_PIN12_Msk               0x00001000UL           /*!< Select pull mode for pin 12 */
#define GPIO_PULLMODE_PIN13_Msk               0x00002000UL           /*!< Select pull mode for pin 13 */
#define GPIO_PULLMODE_PIN14_Msk               0x00004000UL           /*!< Select pull mode for pin 14 */
#define GPIO_PULLMODE_PIN15_Msk               0x00008000UL           /*!< Select pull mode for pin 15 */

/* Bit field enums: */
typedef enum {
  GPIO_PULLMODE_PIN0_Disable                 = 0x0UL,                /*!< Pull disabled */
  GPIO_PULLMODE_PIN0_PU                      = 0x1UL,                /*!< Pull-up */
} GPIO_PULLMODE_PIN0_Enum;

typedef enum {
  GPIO_PULLMODE_PIN1_Disable                 = 0x0UL,                /*!< Pull disabled */
  GPIO_PULLMODE_PIN1_PU                      = 0x1UL,                /*!< Pull-up */
} GPIO_PULLMODE_PIN1_Enum;

typedef enum {
  GPIO_PULLMODE_PIN2_Disable                 = 0x0UL,                /*!< Pull disabled */
  GPIO_PULLMODE_PIN2_PU                      = 0x1UL,                /*!< Pull-up */
} GPIO_PULLMODE_PIN2_Enum;

typedef enum {
  GPIO_PULLMODE_PIN3_Disable                 = 0x0UL,                /*!< Pull disabled */
  GPIO_PULLMODE_PIN3_PU                      = 0x1UL,                /*!< Pull-up */
} GPIO_PULLMODE_PIN3_Enum;

typedef enum {
  GPIO_PULLMODE_PIN4_Disable                 = 0x0UL,                /*!< Pull disabled */
  GPIO_PULLMODE_PIN4_PU                      = 0x1UL,                /*!< Pull-up */
} GPIO_PULLMODE_PIN4_Enum;

typedef enum {
  GPIO_PULLMODE_PIN5_Disable                 = 0x0UL,                /*!< Pull disabled */
  GPIO_PULLMODE_PIN5_PU                      = 0x1UL,                /*!< Pull-up */
} GPIO_PULLMODE_PIN5_Enum;

typedef enum {
  GPIO_PULLMODE_PIN6_Disable                 = 0x0UL,                /*!< Pull disabled */
  GPIO_PULLMODE_PIN6_PU                      = 0x1UL,                /*!< Pull-up */
} GPIO_PULLMODE_PIN6_Enum;

typedef enum {
  GPIO_PULLMODE_PIN7_Disable                 = 0x0UL,                /*!< Pull disabled */
  GPIO_PULLMODE_PIN7_PU                      = 0x1UL,                /*!< Pull-up */
} GPIO_PULLMODE_PIN7_Enum;

typedef enum {
  GPIO_PULLMODE_PIN8_Disable                 = 0x0UL,                /*!< Pull disabled */
  GPIO_PULLMODE_PIN8_PU                      = 0x1UL,                /*!< Pull-up */
} GPIO_PULLMODE_PIN8_Enum;

typedef enum {
  GPIO_PULLMODE_PIN9_Disable                 = 0x0UL,                /*!< Pull disabled */
  GPIO_PULLMODE_PIN9_PU                      = 0x1UL,                /*!< Pull-up */
} GPIO_PULLMODE_PIN9_Enum;

typedef enum {
  GPIO_PULLMODE_PIN10_Disable                = 0x0UL,                /*!< Pull disabled */
  GPIO_PULLMODE_PIN10_PU                     = 0x1UL,                /*!< Pull-up */
} GPIO_PULLMODE_PIN10_Enum;

typedef enum {
  GPIO_PULLMODE_PIN11_Disable                = 0x0UL,                /*!< Pull disabled */
  GPIO_PULLMODE_PIN11_PU                     = 0x1UL,                /*!< Pull-up */
} GPIO_PULLMODE_PIN11_Enum;

typedef enum {
  GPIO_PULLMODE_PIN12_Disable                = 0x0UL,                /*!< Pull disabled */
  GPIO_PULLMODE_PIN12_PU                     = 0x1UL,                /*!< Pull-up */
} GPIO_PULLMODE_PIN12_Enum;

typedef enum {
  GPIO_PULLMODE_PIN13_Disable                = 0x0UL,                /*!< Pull disabled */
  GPIO_PULLMODE_PIN13_PU                     = 0x1UL,                /*!< Pull-up */
} GPIO_PULLMODE_PIN13_Enum;

typedef enum {
  GPIO_PULLMODE_PIN14_Disable                = 0x0UL,                /*!< Pull disabled */
  GPIO_PULLMODE_PIN14_PU                     = 0x1UL,                /*!< Pull-up */
} GPIO_PULLMODE_PIN14_Enum;

typedef enum {
  GPIO_PULLMODE_PIN15_Disable                = 0x0UL,                /*!< Pull disabled */
  GPIO_PULLMODE_PIN15_PU                     = 0x1UL,                /*!< Pull-up */
} GPIO_PULLMODE_PIN15_Enum;

/*--  OUTMODE: Select output mode register --------------------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :2;                                /*!< Select output mode for pin 0 */
  uint32_t PIN1                   :2;                                /*!< Select output mode for pin 1 */
  uint32_t PIN2                   :2;                                /*!< Select output mode for pin 2 */
  uint32_t PIN3                   :2;                                /*!< Select output mode for pin 3 */
  uint32_t PIN4                   :2;                                /*!< Select output mode for pin 4 */
  uint32_t PIN5                   :2;                                /*!< Select output mode for pin 5 */
  uint32_t PIN6                   :2;                                /*!< Select output mode for pin 6 */
  uint32_t PIN7                   :2;                                /*!< Select output mode for pin 7 */
  uint32_t PIN8                   :2;                                /*!< Select output mode for pin 8 */
  uint32_t PIN9                   :2;                                /*!< Select output mode for pin 9 */
  uint32_t PIN10                  :2;                                /*!< Select output mode for pin 10 */
  uint32_t PIN11                  :2;                                /*!< Select output mode for pin 11 */
  uint32_t PIN12                  :2;                                /*!< Select output mode for pin 12 */
  uint32_t PIN13                  :2;                                /*!< Select output mode for pin 13 */
  uint32_t PIN14                  :2;                                /*!< Select output mode for pin 14 */
  uint32_t PIN15                  :2;                                /*!< Select output mode for pin 15 */
} _GPIO_OUTMODE_bits;

/* Bit field positions: */
#define GPIO_OUTMODE_PIN0_Pos                 0                      /*!< Select output mode for pin 0 */
#define GPIO_OUTMODE_PIN1_Pos                 2                      /*!< Select output mode for pin 1 */
#define GPIO_OUTMODE_PIN2_Pos                 4                      /*!< Select output mode for pin 2 */
#define GPIO_OUTMODE_PIN3_Pos                 6                      /*!< Select output mode for pin 3 */
#define GPIO_OUTMODE_PIN4_Pos                 8                      /*!< Select output mode for pin 4 */
#define GPIO_OUTMODE_PIN5_Pos                 10                     /*!< Select output mode for pin 5 */
#define GPIO_OUTMODE_PIN6_Pos                 12                     /*!< Select output mode for pin 6 */
#define GPIO_OUTMODE_PIN7_Pos                 14                     /*!< Select output mode for pin 7 */
#define GPIO_OUTMODE_PIN8_Pos                 16                     /*!< Select output mode for pin 8 */
#define GPIO_OUTMODE_PIN9_Pos                 18                     /*!< Select output mode for pin 9 */
#define GPIO_OUTMODE_PIN10_Pos                20                     /*!< Select output mode for pin 10 */
#define GPIO_OUTMODE_PIN11_Pos                22                     /*!< Select output mode for pin 11 */
#define GPIO_OUTMODE_PIN12_Pos                24                     /*!< Select output mode for pin 12 */
#define GPIO_OUTMODE_PIN13_Pos                26                     /*!< Select output mode for pin 13 */
#define GPIO_OUTMODE_PIN14_Pos                28                     /*!< Select output mode for pin 14 */
#define GPIO_OUTMODE_PIN15_Pos                30                     /*!< Select output mode for pin 15 */

/* Bit field masks: */
#define GPIO_OUTMODE_PIN0_Msk                 0x00000003UL           /*!< Select output mode for pin 0 */
#define GPIO_OUTMODE_PIN1_Msk                 0x0000000CUL           /*!< Select output mode for pin 1 */
#define GPIO_OUTMODE_PIN2_Msk                 0x00000030UL           /*!< Select output mode for pin 2 */
#define GPIO_OUTMODE_PIN3_Msk                 0x000000C0UL           /*!< Select output mode for pin 3 */
#define GPIO_OUTMODE_PIN4_Msk                 0x00000300UL           /*!< Select output mode for pin 4 */
#define GPIO_OUTMODE_PIN5_Msk                 0x00000C00UL           /*!< Select output mode for pin 5 */
#define GPIO_OUTMODE_PIN6_Msk                 0x00003000UL           /*!< Select output mode for pin 6 */
#define GPIO_OUTMODE_PIN7_Msk                 0x0000C000UL           /*!< Select output mode for pin 7 */
#define GPIO_OUTMODE_PIN8_Msk                 0x00030000UL           /*!< Select output mode for pin 8 */
#define GPIO_OUTMODE_PIN9_Msk                 0x000C0000UL           /*!< Select output mode for pin 9 */
#define GPIO_OUTMODE_PIN10_Msk                0x00300000UL           /*!< Select output mode for pin 10 */
#define GPIO_OUTMODE_PIN11_Msk                0x00C00000UL           /*!< Select output mode for pin 11 */
#define GPIO_OUTMODE_PIN12_Msk                0x03000000UL           /*!< Select output mode for pin 12 */
#define GPIO_OUTMODE_PIN13_Msk                0x0C000000UL           /*!< Select output mode for pin 13 */
#define GPIO_OUTMODE_PIN14_Msk                0x30000000UL           /*!< Select output mode for pin 14 */
#define GPIO_OUTMODE_PIN15_Msk                0xC0000000UL           /*!< Select output mode for pin 15 */

/* Bit field enums: */
typedef enum {
  GPIO_OUTMODE_PIN0_PP                       = 0x0UL,                /*!< Push-pull output */
  GPIO_OUTMODE_PIN0_OD                       = 0x1UL,                /*!< Open drain output */
  GPIO_OUTMODE_PIN0_OS                       = 0x2UL,                /*!< Open source output */
} GPIO_OUTMODE_PIN0_Enum;

typedef enum {
  GPIO_OUTMODE_PIN1_PP                       = 0x0UL,                /*!< Push-pull output */
  GPIO_OUTMODE_PIN1_OD                       = 0x1UL,                /*!< Open drain output */
  GPIO_OUTMODE_PIN1_OS                       = 0x2UL,                /*!< Open source output */
} GPIO_OUTMODE_PIN1_Enum;

typedef enum {
  GPIO_OUTMODE_PIN2_PP                       = 0x0UL,                /*!< Push-pull output */
  GPIO_OUTMODE_PIN2_OD                       = 0x1UL,                /*!< Open drain output */
  GPIO_OUTMODE_PIN2_OS                       = 0x2UL,                /*!< Open source output */
} GPIO_OUTMODE_PIN2_Enum;

typedef enum {
  GPIO_OUTMODE_PIN3_PP                       = 0x0UL,                /*!< Push-pull output */
  GPIO_OUTMODE_PIN3_OD                       = 0x1UL,                /*!< Open drain output */
  GPIO_OUTMODE_PIN3_OS                       = 0x2UL,                /*!< Open source output */
} GPIO_OUTMODE_PIN3_Enum;

typedef enum {
  GPIO_OUTMODE_PIN4_PP                       = 0x0UL,                /*!< Push-pull output */
  GPIO_OUTMODE_PIN4_OD                       = 0x1UL,                /*!< Open drain output */
  GPIO_OUTMODE_PIN4_OS                       = 0x2UL,                /*!< Open source output */
} GPIO_OUTMODE_PIN4_Enum;

typedef enum {
  GPIO_OUTMODE_PIN5_PP                       = 0x0UL,                /*!< Push-pull output */
  GPIO_OUTMODE_PIN5_OD                       = 0x1UL,                /*!< Open drain output */
  GPIO_OUTMODE_PIN5_OS                       = 0x2UL,                /*!< Open source output */
} GPIO_OUTMODE_PIN5_Enum;

typedef enum {
  GPIO_OUTMODE_PIN6_PP                       = 0x0UL,                /*!< Push-pull output */
  GPIO_OUTMODE_PIN6_OD                       = 0x1UL,                /*!< Open drain output */
  GPIO_OUTMODE_PIN6_OS                       = 0x2UL,                /*!< Open source output */
} GPIO_OUTMODE_PIN6_Enum;

typedef enum {
  GPIO_OUTMODE_PIN7_PP                       = 0x0UL,                /*!< Push-pull output */
  GPIO_OUTMODE_PIN7_OD                       = 0x1UL,                /*!< Open drain output */
  GPIO_OUTMODE_PIN7_OS                       = 0x2UL,                /*!< Open source output */
} GPIO_OUTMODE_PIN7_Enum;

typedef enum {
  GPIO_OUTMODE_PIN8_PP                       = 0x0UL,                /*!< Push-pull output */
  GPIO_OUTMODE_PIN8_OD                       = 0x1UL,                /*!< Open drain output */
  GPIO_OUTMODE_PIN8_OS                       = 0x2UL,                /*!< Open source output */
} GPIO_OUTMODE_PIN8_Enum;

typedef enum {
  GPIO_OUTMODE_PIN9_PP                       = 0x0UL,                /*!< Push-pull output */
  GPIO_OUTMODE_PIN9_OD                       = 0x1UL,                /*!< Open drain output */
  GPIO_OUTMODE_PIN9_OS                       = 0x2UL,                /*!< Open source output */
} GPIO_OUTMODE_PIN9_Enum;

typedef enum {
  GPIO_OUTMODE_PIN10_PP                      = 0x0UL,                /*!< Push-pull output */
  GPIO_OUTMODE_PIN10_OD                      = 0x1UL,                /*!< Open drain output */
  GPIO_OUTMODE_PIN10_OS                      = 0x2UL,                /*!< Open source output */
} GPIO_OUTMODE_PIN10_Enum;

typedef enum {
  GPIO_OUTMODE_PIN11_PP                      = 0x0UL,                /*!< Push-pull output */
  GPIO_OUTMODE_PIN11_OD                      = 0x1UL,                /*!< Open drain output */
  GPIO_OUTMODE_PIN11_OS                      = 0x2UL,                /*!< Open source output */
} GPIO_OUTMODE_PIN11_Enum;

typedef enum {
  GPIO_OUTMODE_PIN12_PP                      = 0x0UL,                /*!< Push-pull output */
  GPIO_OUTMODE_PIN12_OD                      = 0x1UL,                /*!< Open drain output */
  GPIO_OUTMODE_PIN12_OS                      = 0x2UL,                /*!< Open source output */
} GPIO_OUTMODE_PIN12_Enum;

typedef enum {
  GPIO_OUTMODE_PIN13_PP                      = 0x0UL,                /*!< Push-pull output */
  GPIO_OUTMODE_PIN13_OD                      = 0x1UL,                /*!< Open drain output */
  GPIO_OUTMODE_PIN13_OS                      = 0x2UL,                /*!< Open source output */
} GPIO_OUTMODE_PIN13_Enum;

typedef enum {
  GPIO_OUTMODE_PIN14_PP                      = 0x0UL,                /*!< Push-pull output */
  GPIO_OUTMODE_PIN14_OD                      = 0x1UL,                /*!< Open drain output */
  GPIO_OUTMODE_PIN14_OS                      = 0x2UL,                /*!< Open source output */
} GPIO_OUTMODE_PIN14_Enum;

typedef enum {
  GPIO_OUTMODE_PIN15_PP                      = 0x0UL,                /*!< Push-pull output */
  GPIO_OUTMODE_PIN15_OD                      = 0x1UL,                /*!< Open drain output */
  GPIO_OUTMODE_PIN15_OS                      = 0x2UL,                /*!< Open source output */
} GPIO_OUTMODE_PIN15_Enum;

/*--  DRIVEMODE: Select drive mode register -------------------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :2;                                /*!< Select drive mode for pin 0 */
  uint32_t PIN1                   :2;                                /*!< Select drive mode for pin 1 */
  uint32_t PIN2                   :2;                                /*!< Select drive mode for pin 2 */
  uint32_t PIN3                   :2;                                /*!< Select drive mode for pin 3 */
  uint32_t PIN4                   :2;                                /*!< Select drive mode for pin 4 */
  uint32_t PIN5                   :2;                                /*!< Select drive mode for pin 5 */
  uint32_t PIN6                   :2;                                /*!< Select drive mode for pin 6 */
  uint32_t PIN7                   :2;                                /*!< Select drive mode for pin 7 */
  uint32_t PIN8                   :2;                                /*!< Select drive mode for pin 8 */
  uint32_t PIN9                   :2;                                /*!< Select drive mode for pin 9 */
  uint32_t PIN10                  :2;                                /*!< Select drive mode for pin 10 */
  uint32_t PIN11                  :2;                                /*!< Select drive mode for pin 11 */
  uint32_t PIN12                  :2;                                /*!< Select drive mode for pin 12 */
  uint32_t PIN13                  :2;                                /*!< Select drive mode for pin 13 */
  uint32_t PIN14                  :2;                                /*!< Select drive mode for pin 14 */
  uint32_t PIN15                  :2;                                /*!< Select drive mode for pin 15 */
} _GPIO_DRIVEMODE_bits;

/* Bit field positions: */
#define GPIO_DRIVEMODE_PIN0_Pos               0                      /*!< Select drive mode for pin 0 */
#define GPIO_DRIVEMODE_PIN1_Pos               2                      /*!< Select drive mode for pin 1 */
#define GPIO_DRIVEMODE_PIN2_Pos               4                      /*!< Select drive mode for pin 2 */
#define GPIO_DRIVEMODE_PIN3_Pos               6                      /*!< Select drive mode for pin 3 */
#define GPIO_DRIVEMODE_PIN4_Pos               8                      /*!< Select drive mode for pin 4 */
#define GPIO_DRIVEMODE_PIN5_Pos               10                     /*!< Select drive mode for pin 5 */
#define GPIO_DRIVEMODE_PIN6_Pos               12                     /*!< Select drive mode for pin 6 */
#define GPIO_DRIVEMODE_PIN7_Pos               14                     /*!< Select drive mode for pin 7 */
#define GPIO_DRIVEMODE_PIN8_Pos               16                     /*!< Select drive mode for pin 8 */
#define GPIO_DRIVEMODE_PIN9_Pos               18                     /*!< Select drive mode for pin 9 */
#define GPIO_DRIVEMODE_PIN10_Pos              20                     /*!< Select drive mode for pin 10 */
#define GPIO_DRIVEMODE_PIN11_Pos              22                     /*!< Select drive mode for pin 11 */
#define GPIO_DRIVEMODE_PIN12_Pos              24                     /*!< Select drive mode for pin 12 */
#define GPIO_DRIVEMODE_PIN13_Pos              26                     /*!< Select drive mode for pin 13 */
#define GPIO_DRIVEMODE_PIN14_Pos              28                     /*!< Select drive mode for pin 14 */
#define GPIO_DRIVEMODE_PIN15_Pos              30                     /*!< Select drive mode for pin 15 */

/* Bit field masks: */
#define GPIO_DRIVEMODE_PIN0_Msk               0x00000003UL           /*!< Select drive mode for pin 0 */
#define GPIO_DRIVEMODE_PIN1_Msk               0x0000000CUL           /*!< Select drive mode for pin 1 */
#define GPIO_DRIVEMODE_PIN2_Msk               0x00000030UL           /*!< Select drive mode for pin 2 */
#define GPIO_DRIVEMODE_PIN3_Msk               0x000000C0UL           /*!< Select drive mode for pin 3 */
#define GPIO_DRIVEMODE_PIN4_Msk               0x00000300UL           /*!< Select drive mode for pin 4 */
#define GPIO_DRIVEMODE_PIN5_Msk               0x00000C00UL           /*!< Select drive mode for pin 5 */
#define GPIO_DRIVEMODE_PIN6_Msk               0x00003000UL           /*!< Select drive mode for pin 6 */
#define GPIO_DRIVEMODE_PIN7_Msk               0x0000C000UL           /*!< Select drive mode for pin 7 */
#define GPIO_DRIVEMODE_PIN8_Msk               0x00030000UL           /*!< Select drive mode for pin 8 */
#define GPIO_DRIVEMODE_PIN9_Msk               0x000C0000UL           /*!< Select drive mode for pin 9 */
#define GPIO_DRIVEMODE_PIN10_Msk              0x00300000UL           /*!< Select drive mode for pin 10 */
#define GPIO_DRIVEMODE_PIN11_Msk              0x00C00000UL           /*!< Select drive mode for pin 11 */
#define GPIO_DRIVEMODE_PIN12_Msk              0x03000000UL           /*!< Select drive mode for pin 12 */
#define GPIO_DRIVEMODE_PIN13_Msk              0x0C000000UL           /*!< Select drive mode for pin 13 */
#define GPIO_DRIVEMODE_PIN14_Msk              0x30000000UL           /*!< Select drive mode for pin 14 */
#define GPIO_DRIVEMODE_PIN15_Msk              0xC0000000UL           /*!< Select drive mode for pin 15 */

/* Bit field enums: */
typedef enum {
  GPIO_DRIVEMODE_PIN0_HS                     = 0x0UL,                /*!< High strength */
  GPIO_DRIVEMODE_PIN0_LS                     = 0x2UL,                /*!< Low strength */
} GPIO_DRIVEMODE_PIN0_Enum;

typedef enum {
  GPIO_DRIVEMODE_PIN1_HS                     = 0x0UL,                /*!< High strength */
  GPIO_DRIVEMODE_PIN1_LS                     = 0x2UL,                /*!< Low strength */
} GPIO_DRIVEMODE_PIN1_Enum;

typedef enum {
  GPIO_DRIVEMODE_PIN2_HS                     = 0x0UL,                /*!< High strength */
  GPIO_DRIVEMODE_PIN2_LS                     = 0x2UL,                /*!< Low strength */
} GPIO_DRIVEMODE_PIN2_Enum;

typedef enum {
  GPIO_DRIVEMODE_PIN3_HS                     = 0x0UL,                /*!< High strength */
  GPIO_DRIVEMODE_PIN3_LS                     = 0x2UL,                /*!< Low strength */
} GPIO_DRIVEMODE_PIN3_Enum;

typedef enum {
  GPIO_DRIVEMODE_PIN4_HS                     = 0x0UL,                /*!< High strength */
  GPIO_DRIVEMODE_PIN4_LS                     = 0x2UL,                /*!< Low strength */
} GPIO_DRIVEMODE_PIN4_Enum;

typedef enum {
  GPIO_DRIVEMODE_PIN5_HS                     = 0x0UL,                /*!< High strength */
  GPIO_DRIVEMODE_PIN5_LS                     = 0x2UL,                /*!< Low strength */
} GPIO_DRIVEMODE_PIN5_Enum;

typedef enum {
  GPIO_DRIVEMODE_PIN6_HS                     = 0x0UL,                /*!< High strength */
  GPIO_DRIVEMODE_PIN6_LS                     = 0x2UL,                /*!< Low strength */
} GPIO_DRIVEMODE_PIN6_Enum;

typedef enum {
  GPIO_DRIVEMODE_PIN7_HS                     = 0x0UL,                /*!< High strength */
  GPIO_DRIVEMODE_PIN7_LS                     = 0x2UL,                /*!< Low strength */
} GPIO_DRIVEMODE_PIN7_Enum;

typedef enum {
  GPIO_DRIVEMODE_PIN8_HS                     = 0x0UL,                /*!< High strength */
  GPIO_DRIVEMODE_PIN8_LS                     = 0x2UL,                /*!< Low strength */
} GPIO_DRIVEMODE_PIN8_Enum;

typedef enum {
  GPIO_DRIVEMODE_PIN9_HS                     = 0x0UL,                /*!< High strength */
  GPIO_DRIVEMODE_PIN9_LS                     = 0x2UL,                /*!< Low strength */
} GPIO_DRIVEMODE_PIN9_Enum;

typedef enum {
  GPIO_DRIVEMODE_PIN10_HS                    = 0x0UL,                /*!< High strength */
  GPIO_DRIVEMODE_PIN10_LS                    = 0x2UL,                /*!< Low strength */
} GPIO_DRIVEMODE_PIN10_Enum;

typedef enum {
  GPIO_DRIVEMODE_PIN11_HS                    = 0x0UL,                /*!< High strength */
  GPIO_DRIVEMODE_PIN11_LS                    = 0x2UL,                /*!< Low strength */
} GPIO_DRIVEMODE_PIN11_Enum;

typedef enum {
  GPIO_DRIVEMODE_PIN12_HS                    = 0x0UL,                /*!< High strength */
  GPIO_DRIVEMODE_PIN12_LS                    = 0x2UL,                /*!< Low strength */
} GPIO_DRIVEMODE_PIN12_Enum;

typedef enum {
  GPIO_DRIVEMODE_PIN13_HS                    = 0x0UL,                /*!< High strength */
  GPIO_DRIVEMODE_PIN13_LS                    = 0x2UL,                /*!< Low strength */
} GPIO_DRIVEMODE_PIN13_Enum;

typedef enum {
  GPIO_DRIVEMODE_PIN14_HS                    = 0x0UL,                /*!< High strength */
  GPIO_DRIVEMODE_PIN14_LS                    = 0x2UL,                /*!< Low strength */
} GPIO_DRIVEMODE_PIN14_Enum;

typedef enum {
  GPIO_DRIVEMODE_PIN15_HS                    = 0x0UL,                /*!< High strength */
  GPIO_DRIVEMODE_PIN15_LS                    = 0x2UL,                /*!< Low strength */
} GPIO_DRIVEMODE_PIN15_Enum;

/*--  OUTENSET: Output enable register ------------------------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :1;                                /*!< Output enable for pin 0 */
  uint32_t PIN1                   :1;                                /*!< Output enable for pin 1 */
  uint32_t PIN2                   :1;                                /*!< Output enable for pin 2 */
  uint32_t PIN3                   :1;                                /*!< Output enable for pin 3 */
  uint32_t PIN4                   :1;                                /*!< Output enable for pin 4 */
  uint32_t PIN5                   :1;                                /*!< Output enable for pin 5 */
  uint32_t PIN6                   :1;                                /*!< Output enable for pin 6 */
  uint32_t PIN7                   :1;                                /*!< Output enable for pin 7 */
  uint32_t PIN8                   :1;                                /*!< Output enable for pin 8 */
  uint32_t PIN9                   :1;                                /*!< Output enable for pin 9 */
  uint32_t PIN10                  :1;                                /*!< Output enable for pin 10 */
  uint32_t PIN11                  :1;                                /*!< Output enable for pin 11 */
  uint32_t PIN12                  :1;                                /*!< Output enable for pin 12 */
  uint32_t PIN13                  :1;                                /*!< Output enable for pin 13 */
  uint32_t PIN14                  :1;                                /*!< Output enable for pin 14 */
  uint32_t PIN15                  :1;                                /*!< Output enable for pin 15 */
} _GPIO_OUTENSET_bits;

/* Bit field positions: */
#define GPIO_OUTENSET_PIN0_Pos                0                      /*!< Output enable for pin 0 */
#define GPIO_OUTENSET_PIN1_Pos                1                      /*!< Output enable for pin 1 */
#define GPIO_OUTENSET_PIN2_Pos                2                      /*!< Output enable for pin 2 */
#define GPIO_OUTENSET_PIN3_Pos                3                      /*!< Output enable for pin 3 */
#define GPIO_OUTENSET_PIN4_Pos                4                      /*!< Output enable for pin 4 */
#define GPIO_OUTENSET_PIN5_Pos                5                      /*!< Output enable for pin 5 */
#define GPIO_OUTENSET_PIN6_Pos                6                      /*!< Output enable for pin 6 */
#define GPIO_OUTENSET_PIN7_Pos                7                      /*!< Output enable for pin 7 */
#define GPIO_OUTENSET_PIN8_Pos                8                      /*!< Output enable for pin 8 */
#define GPIO_OUTENSET_PIN9_Pos                9                      /*!< Output enable for pin 9 */
#define GPIO_OUTENSET_PIN10_Pos               10                     /*!< Output enable for pin 10 */
#define GPIO_OUTENSET_PIN11_Pos               11                     /*!< Output enable for pin 11 */
#define GPIO_OUTENSET_PIN12_Pos               12                     /*!< Output enable for pin 12 */
#define GPIO_OUTENSET_PIN13_Pos               13                     /*!< Output enable for pin 13 */
#define GPIO_OUTENSET_PIN14_Pos               14                     /*!< Output enable for pin 14 */
#define GPIO_OUTENSET_PIN15_Pos               15                     /*!< Output enable for pin 15 */

/* Bit field masks: */
#define GPIO_OUTENSET_PIN0_Msk                0x00000001UL           /*!< Output enable for pin 0 */
#define GPIO_OUTENSET_PIN1_Msk                0x00000002UL           /*!< Output enable for pin 1 */
#define GPIO_OUTENSET_PIN2_Msk                0x00000004UL           /*!< Output enable for pin 2 */
#define GPIO_OUTENSET_PIN3_Msk                0x00000008UL           /*!< Output enable for pin 3 */
#define GPIO_OUTENSET_PIN4_Msk                0x00000010UL           /*!< Output enable for pin 4 */
#define GPIO_OUTENSET_PIN5_Msk                0x00000020UL           /*!< Output enable for pin 5 */
#define GPIO_OUTENSET_PIN6_Msk                0x00000040UL           /*!< Output enable for pin 6 */
#define GPIO_OUTENSET_PIN7_Msk                0x00000080UL           /*!< Output enable for pin 7 */
#define GPIO_OUTENSET_PIN8_Msk                0x00000100UL           /*!< Output enable for pin 8 */
#define GPIO_OUTENSET_PIN9_Msk                0x00000200UL           /*!< Output enable for pin 9 */
#define GPIO_OUTENSET_PIN10_Msk               0x00000400UL           /*!< Output enable for pin 10 */
#define GPIO_OUTENSET_PIN11_Msk               0x00000800UL           /*!< Output enable for pin 11 */
#define GPIO_OUTENSET_PIN12_Msk               0x00001000UL           /*!< Output enable for pin 12 */
#define GPIO_OUTENSET_PIN13_Msk               0x00002000UL           /*!< Output enable for pin 13 */
#define GPIO_OUTENSET_PIN14_Msk               0x00004000UL           /*!< Output enable for pin 14 */
#define GPIO_OUTENSET_PIN15_Msk               0x00008000UL           /*!< Output enable for pin 15 */

/*--  OUTENCLR: Output disable register -----------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< Output disable */
} _GPIO_OUTENCLR_bits;

/* Bit field positions: */
#define GPIO_OUTENCLR_VAL_Pos                 0                      /*!< Output disable */

/* Bit field masks: */
#define GPIO_OUTENCLR_VAL_Msk                 0x0000FFFFUL           /*!< Output disable */

/*--  ALTFUNCSET: Alternative function enable register --------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :1;                                /*!< Alternative function enable for pin 0 */
  uint32_t PIN1                   :1;                                /*!< Alternative function enable for pin 1 */
  uint32_t PIN2                   :1;                                /*!< Alternative function enable for pin 2 */
  uint32_t PIN3                   :1;                                /*!< Alternative function enable for pin 3 */
  uint32_t PIN4                   :1;                                /*!< Alternative function enable for pin 4 */
  uint32_t PIN5                   :1;                                /*!< Alternative function enable for pin 5 */
  uint32_t PIN6                   :1;                                /*!< Alternative function enable for pin 6 */
  uint32_t PIN7                   :1;                                /*!< Alternative function enable for pin 7 */
  uint32_t PIN8                   :1;                                /*!< Alternative function enable for pin 8 */
  uint32_t PIN9                   :1;                                /*!< Alternative function enable for pin 9 */
  uint32_t PIN10                  :1;                                /*!< Alternative function enable for pin 10 */
  uint32_t PIN11                  :1;                                /*!< Alternative function enable for pin 11 */
  uint32_t PIN12                  :1;                                /*!< Alternative function enable for pin 12 */
  uint32_t PIN13                  :1;                                /*!< Alternative function enable for pin 13 */
  uint32_t PIN14                  :1;                                /*!< Alternative function enable for pin 14 */
  uint32_t PIN15                  :1;                                /*!< Alternative function enable for pin 15 */
} _GPIO_ALTFUNCSET_bits;

/* Bit field positions: */
#define GPIO_ALTFUNCSET_PIN0_Pos              0                      /*!< Alternative function enable for pin 0 */
#define GPIO_ALTFUNCSET_PIN1_Pos              1                      /*!< Alternative function enable for pin 1 */
#define GPIO_ALTFUNCSET_PIN2_Pos              2                      /*!< Alternative function enable for pin 2 */
#define GPIO_ALTFUNCSET_PIN3_Pos              3                      /*!< Alternative function enable for pin 3 */
#define GPIO_ALTFUNCSET_PIN4_Pos              4                      /*!< Alternative function enable for pin 4 */
#define GPIO_ALTFUNCSET_PIN5_Pos              5                      /*!< Alternative function enable for pin 5 */
#define GPIO_ALTFUNCSET_PIN6_Pos              6                      /*!< Alternative function enable for pin 6 */
#define GPIO_ALTFUNCSET_PIN7_Pos              7                      /*!< Alternative function enable for pin 7 */
#define GPIO_ALTFUNCSET_PIN8_Pos              8                      /*!< Alternative function enable for pin 8 */
#define GPIO_ALTFUNCSET_PIN9_Pos              9                      /*!< Alternative function enable for pin 9 */
#define GPIO_ALTFUNCSET_PIN10_Pos             10                     /*!< Alternative function enable for pin 10 */
#define GPIO_ALTFUNCSET_PIN11_Pos             11                     /*!< Alternative function enable for pin 11 */
#define GPIO_ALTFUNCSET_PIN12_Pos             12                     /*!< Alternative function enable for pin 12 */
#define GPIO_ALTFUNCSET_PIN13_Pos             13                     /*!< Alternative function enable for pin 13 */
#define GPIO_ALTFUNCSET_PIN14_Pos             14                     /*!< Alternative function enable for pin 14 */
#define GPIO_ALTFUNCSET_PIN15_Pos             15                     /*!< Alternative function enable for pin 15 */

/* Bit field masks: */
#define GPIO_ALTFUNCSET_PIN0_Msk              0x00000001UL           /*!< Alternative function enable for pin 0 */
#define GPIO_ALTFUNCSET_PIN1_Msk              0x00000002UL           /*!< Alternative function enable for pin 1 */
#define GPIO_ALTFUNCSET_PIN2_Msk              0x00000004UL           /*!< Alternative function enable for pin 2 */
#define GPIO_ALTFUNCSET_PIN3_Msk              0x00000008UL           /*!< Alternative function enable for pin 3 */
#define GPIO_ALTFUNCSET_PIN4_Msk              0x00000010UL           /*!< Alternative function enable for pin 4 */
#define GPIO_ALTFUNCSET_PIN5_Msk              0x00000020UL           /*!< Alternative function enable for pin 5 */
#define GPIO_ALTFUNCSET_PIN6_Msk              0x00000040UL           /*!< Alternative function enable for pin 6 */
#define GPIO_ALTFUNCSET_PIN7_Msk              0x00000080UL           /*!< Alternative function enable for pin 7 */
#define GPIO_ALTFUNCSET_PIN8_Msk              0x00000100UL           /*!< Alternative function enable for pin 8 */
#define GPIO_ALTFUNCSET_PIN9_Msk              0x00000200UL           /*!< Alternative function enable for pin 9 */
#define GPIO_ALTFUNCSET_PIN10_Msk             0x00000400UL           /*!< Alternative function enable for pin 10 */
#define GPIO_ALTFUNCSET_PIN11_Msk             0x00000800UL           /*!< Alternative function enable for pin 11 */
#define GPIO_ALTFUNCSET_PIN12_Msk             0x00001000UL           /*!< Alternative function enable for pin 12 */
#define GPIO_ALTFUNCSET_PIN13_Msk             0x00002000UL           /*!< Alternative function enable for pin 13 */
#define GPIO_ALTFUNCSET_PIN14_Msk             0x00004000UL           /*!< Alternative function enable for pin 14 */
#define GPIO_ALTFUNCSET_PIN15_Msk             0x00008000UL           /*!< Alternative function enable for pin 15 */

/*--  ALTFUNCCLR: Alternative function disable register -------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< Alternative function disable */
} _GPIO_ALTFUNCCLR_bits;

/* Bit field positions: */
#define GPIO_ALTFUNCCLR_VAL_Pos               0                      /*!< Alternative function disable */

/* Bit field masks: */
#define GPIO_ALTFUNCCLR_VAL_Msk               0x0000FFFFUL           /*!< Alternative function disable */

/*--  ALTFUNCNUM: Alternative function number register --------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :2;                                /*!< Select altfunc number for pin 0 */
  uint32_t PIN1                   :2;                                /*!< Select altfunc number for pin 0 */
  uint32_t PIN2                   :2;                                /*!< Select altfunc number for pin 1 */
  uint32_t PIN3                   :2;                                /*!< Select altfunc number for pin 1 */
  uint32_t PIN4                   :2;                                /*!< Select altfunc number for pin 2 */
  uint32_t PIN5                   :2;                                /*!< Select altfunc number for pin 2 */
  uint32_t PIN6                   :2;                                /*!< Select altfunc number for pin 3 */
  uint32_t PIN7                   :2;                                /*!< Select altfunc number for pin 3 */
  uint32_t PIN8                   :2;                                /*!< Select altfunc number for pin 4 */
  uint32_t PIN9                   :2;                                /*!< Select altfunc number for pin 4 */
  uint32_t PIN10                  :2;                                /*!< Select altfunc number for pin 5 */
  uint32_t PIN11                  :2;                                /*!< Select altfunc number for pin 5 */
  uint32_t PIN12                  :2;                                /*!< Select altfunc number for pin 6 */
  uint32_t PIN13                  :2;                                /*!< Select altfunc number for pin 6 */
  uint32_t PIN14                  :2;                                /*!< Select altfunc number for pin 7 */
  uint32_t PIN15                  :2;                                /*!< Select altfunc number for pin 7 */
} _GPIO_ALTFUNCNUM_bits;

/* Bit field positions: */
#define GPIO_ALTFUNCNUM_PIN0_Pos              0                      /*!< Select altfunc number for pin 0 */
#define GPIO_ALTFUNCNUM_PIN1_Pos              2                      /*!< Select altfunc number for pin 0 */
#define GPIO_ALTFUNCNUM_PIN2_Pos              4                      /*!< Select altfunc number for pin 1 */
#define GPIO_ALTFUNCNUM_PIN3_Pos              6                      /*!< Select altfunc number for pin 1 */
#define GPIO_ALTFUNCNUM_PIN4_Pos              8                      /*!< Select altfunc number for pin 2 */
#define GPIO_ALTFUNCNUM_PIN5_Pos              10                     /*!< Select altfunc number for pin 2 */
#define GPIO_ALTFUNCNUM_PIN6_Pos              12                     /*!< Select altfunc number for pin 3 */
#define GPIO_ALTFUNCNUM_PIN7_Pos              14                     /*!< Select altfunc number for pin 3 */
#define GPIO_ALTFUNCNUM_PIN8_Pos              16                     /*!< Select altfunc number for pin 4 */
#define GPIO_ALTFUNCNUM_PIN9_Pos              18                     /*!< Select altfunc number for pin 4 */
#define GPIO_ALTFUNCNUM_PIN10_Pos             20                     /*!< Select altfunc number for pin 5 */
#define GPIO_ALTFUNCNUM_PIN11_Pos             22                     /*!< Select altfunc number for pin 5 */
#define GPIO_ALTFUNCNUM_PIN12_Pos             24                     /*!< Select altfunc number for pin 6 */
#define GPIO_ALTFUNCNUM_PIN13_Pos             26                     /*!< Select altfunc number for pin 6 */
#define GPIO_ALTFUNCNUM_PIN14_Pos             28                     /*!< Select altfunc number for pin 7 */
#define GPIO_ALTFUNCNUM_PIN15_Pos             30                     /*!< Select altfunc number for pin 7 */

/* Bit field masks: */
#define GPIO_ALTFUNCNUM_PIN0_Msk              0x00000003UL           /*!< Select altfunc number for pin 0 */
#define GPIO_ALTFUNCNUM_PIN1_Msk              0x0000000CUL           /*!< Select altfunc number for pin 0 */
#define GPIO_ALTFUNCNUM_PIN2_Msk              0x00000030UL           /*!< Select altfunc number for pin 1 */
#define GPIO_ALTFUNCNUM_PIN3_Msk              0x000000C0UL           /*!< Select altfunc number for pin 1 */
#define GPIO_ALTFUNCNUM_PIN4_Msk              0x00000300UL           /*!< Select altfunc number for pin 2 */
#define GPIO_ALTFUNCNUM_PIN5_Msk              0x00000C00UL           /*!< Select altfunc number for pin 2 */
#define GPIO_ALTFUNCNUM_PIN6_Msk              0x00003000UL           /*!< Select altfunc number for pin 3 */
#define GPIO_ALTFUNCNUM_PIN7_Msk              0x0000C000UL           /*!< Select altfunc number for pin 3 */
#define GPIO_ALTFUNCNUM_PIN8_Msk              0x00030000UL           /*!< Select altfunc number for pin 4 */
#define GPIO_ALTFUNCNUM_PIN9_Msk              0x000C0000UL           /*!< Select altfunc number for pin 4 */
#define GPIO_ALTFUNCNUM_PIN10_Msk             0x00300000UL           /*!< Select altfunc number for pin 5 */
#define GPIO_ALTFUNCNUM_PIN11_Msk             0x00C00000UL           /*!< Select altfunc number for pin 5 */
#define GPIO_ALTFUNCNUM_PIN12_Msk             0x03000000UL           /*!< Select altfunc number for pin 6 */
#define GPIO_ALTFUNCNUM_PIN13_Msk             0x0C000000UL           /*!< Select altfunc number for pin 6 */
#define GPIO_ALTFUNCNUM_PIN14_Msk             0x30000000UL           /*!< Select altfunc number for pin 7 */
#define GPIO_ALTFUNCNUM_PIN15_Msk             0xC0000000UL           /*!< Select altfunc number for pin 7 */

/* Bit field enums: */
typedef enum {
  GPIO_ALTFUNCNUM_PIN0_Disable               = 0x0UL,                /*!< Altfunc disabled */
  GPIO_ALTFUNCNUM_PIN0_AF1                   = 0x1UL,                /*!< Altfunc 1 enable */
  GPIO_ALTFUNCNUM_PIN0_AF2                   = 0x2UL,                /*!< Altfunc 2 enable */
  GPIO_ALTFUNCNUM_PIN0_AF3                   = 0x3UL,                /*!< Altfunc 3 enable */
} GPIO_ALTFUNCNUM_PIN0_Enum;

typedef enum {
  GPIO_ALTFUNCNUM_PIN1_Disable               = 0x0UL,                /*!< Altfunc disabled */
  GPIO_ALTFUNCNUM_PIN1_AF1                   = 0x1UL,                /*!< Altfunc 1 enable */
  GPIO_ALTFUNCNUM_PIN1_AF2                   = 0x2UL,                /*!< Altfunc 2 enable */
  GPIO_ALTFUNCNUM_PIN1_AF3                   = 0x3UL,                /*!< Altfunc 3 enable */
} GPIO_ALTFUNCNUM_PIN1_Enum;

typedef enum {
  GPIO_ALTFUNCNUM_PIN2_Disable               = 0x0UL,                /*!< Altfunc disabled */
  GPIO_ALTFUNCNUM_PIN2_AF1                   = 0x1UL,                /*!< Altfunc 1 enable */
  GPIO_ALTFUNCNUM_PIN2_AF2                   = 0x2UL,                /*!< Altfunc 2 enable */
  GPIO_ALTFUNCNUM_PIN2_AF3                   = 0x3UL,                /*!< Altfunc 3 enable */
} GPIO_ALTFUNCNUM_PIN2_Enum;

typedef enum {
  GPIO_ALTFUNCNUM_PIN3_Disable               = 0x0UL,                /*!< Altfunc disabled */
  GPIO_ALTFUNCNUM_PIN3_AF1                   = 0x1UL,                /*!< Altfunc 1 enable */
  GPIO_ALTFUNCNUM_PIN3_AF2                   = 0x2UL,                /*!< Altfunc 2 enable */
  GPIO_ALTFUNCNUM_PIN3_AF3                   = 0x3UL,                /*!< Altfunc 3 enable */
} GPIO_ALTFUNCNUM_PIN3_Enum;

typedef enum {
  GPIO_ALTFUNCNUM_PIN4_Disable               = 0x0UL,                /*!< Altfunc disabled */
  GPIO_ALTFUNCNUM_PIN4_AF1                   = 0x1UL,                /*!< Altfunc 1 enable */
  GPIO_ALTFUNCNUM_PIN4_AF2                   = 0x2UL,                /*!< Altfunc 2 enable */
  GPIO_ALTFUNCNUM_PIN4_AF3                   = 0x3UL,                /*!< Altfunc 3 enable */
} GPIO_ALTFUNCNUM_PIN4_Enum;

typedef enum {
  GPIO_ALTFUNCNUM_PIN5_Disable               = 0x0UL,                /*!< Altfunc disabled */
  GPIO_ALTFUNCNUM_PIN5_AF1                   = 0x1UL,                /*!< Altfunc 1 enable */
  GPIO_ALTFUNCNUM_PIN5_AF2                   = 0x2UL,                /*!< Altfunc 2 enable */
  GPIO_ALTFUNCNUM_PIN5_AF3                   = 0x3UL,                /*!< Altfunc 3 enable */
} GPIO_ALTFUNCNUM_PIN5_Enum;

typedef enum {
  GPIO_ALTFUNCNUM_PIN6_Disable               = 0x0UL,                /*!< Altfunc disabled */
  GPIO_ALTFUNCNUM_PIN6_AF1                   = 0x1UL,                /*!< Altfunc 1 enable */
  GPIO_ALTFUNCNUM_PIN6_AF2                   = 0x2UL,                /*!< Altfunc 2 enable */
  GPIO_ALTFUNCNUM_PIN6_AF3                   = 0x3UL,                /*!< Altfunc 3 enable */
} GPIO_ALTFUNCNUM_PIN6_Enum;

typedef enum {
  GPIO_ALTFUNCNUM_PIN7_Disable               = 0x0UL,                /*!< Altfunc disabled */
  GPIO_ALTFUNCNUM_PIN7_AF1                   = 0x1UL,                /*!< Altfunc 1 enable */
  GPIO_ALTFUNCNUM_PIN7_AF2                   = 0x2UL,                /*!< Altfunc 2 enable */
  GPIO_ALTFUNCNUM_PIN7_AF3                   = 0x3UL,                /*!< Altfunc 3 enable */
} GPIO_ALTFUNCNUM_PIN7_Enum;

typedef enum {
  GPIO_ALTFUNCNUM_PIN8_Disable               = 0x0UL,                /*!< Altfunc disabled */
  GPIO_ALTFUNCNUM_PIN8_AF1                   = 0x1UL,                /*!< Altfunc 1 enable */
  GPIO_ALTFUNCNUM_PIN8_AF2                   = 0x2UL,                /*!< Altfunc 2 enable */
  GPIO_ALTFUNCNUM_PIN8_AF3                   = 0x3UL,                /*!< Altfunc 3 enable */
} GPIO_ALTFUNCNUM_PIN8_Enum;

typedef enum {
  GPIO_ALTFUNCNUM_PIN9_Disable               = 0x0UL,                /*!< Altfunc disabled */
  GPIO_ALTFUNCNUM_PIN9_AF1                   = 0x1UL,                /*!< Altfunc 1 enable */
  GPIO_ALTFUNCNUM_PIN9_AF2                   = 0x2UL,                /*!< Altfunc 2 enable */
  GPIO_ALTFUNCNUM_PIN9_AF3                   = 0x3UL,                /*!< Altfunc 3 enable */
} GPIO_ALTFUNCNUM_PIN9_Enum;

typedef enum {
  GPIO_ALTFUNCNUM_PIN10_Disable              = 0x0UL,                /*!< Altfunc disabled */
  GPIO_ALTFUNCNUM_PIN10_AF1                  = 0x1UL,                /*!< Altfunc 1 enable */
  GPIO_ALTFUNCNUM_PIN10_AF2                  = 0x2UL,                /*!< Altfunc 2 enable */
  GPIO_ALTFUNCNUM_PIN10_AF3                  = 0x3UL,                /*!< Altfunc 3 enable */
} GPIO_ALTFUNCNUM_PIN10_Enum;

typedef enum {
  GPIO_ALTFUNCNUM_PIN11_Disable              = 0x0UL,                /*!< Altfunc disabled */
  GPIO_ALTFUNCNUM_PIN11_AF1                  = 0x1UL,                /*!< Altfunc 1 enable */
  GPIO_ALTFUNCNUM_PIN11_AF2                  = 0x2UL,                /*!< Altfunc 2 enable */
  GPIO_ALTFUNCNUM_PIN11_AF3                  = 0x3UL,                /*!< Altfunc 3 enable */
} GPIO_ALTFUNCNUM_PIN11_Enum;

typedef enum {
  GPIO_ALTFUNCNUM_PIN12_Disable              = 0x0UL,                /*!< Altfunc disabled */
  GPIO_ALTFUNCNUM_PIN12_AF1                  = 0x1UL,                /*!< Altfunc 1 enable */
  GPIO_ALTFUNCNUM_PIN12_AF2                  = 0x2UL,                /*!< Altfunc 2 enable */
  GPIO_ALTFUNCNUM_PIN12_AF3                  = 0x3UL,                /*!< Altfunc 3 enable */
} GPIO_ALTFUNCNUM_PIN12_Enum;

typedef enum {
  GPIO_ALTFUNCNUM_PIN13_Disable              = 0x0UL,                /*!< Altfunc disabled */
  GPIO_ALTFUNCNUM_PIN13_AF1                  = 0x1UL,                /*!< Altfunc 1 enable */
  GPIO_ALTFUNCNUM_PIN13_AF2                  = 0x2UL,                /*!< Altfunc 2 enable */
  GPIO_ALTFUNCNUM_PIN13_AF3                  = 0x3UL,                /*!< Altfunc 3 enable */
} GPIO_ALTFUNCNUM_PIN13_Enum;

typedef enum {
  GPIO_ALTFUNCNUM_PIN14_Disable              = 0x0UL,                /*!< Altfunc disabled */
  GPIO_ALTFUNCNUM_PIN14_AF1                  = 0x1UL,                /*!< Altfunc 1 enable */
  GPIO_ALTFUNCNUM_PIN14_AF2                  = 0x2UL,                /*!< Altfunc 2 enable */
  GPIO_ALTFUNCNUM_PIN14_AF3                  = 0x3UL,                /*!< Altfunc 3 enable */
} GPIO_ALTFUNCNUM_PIN14_Enum;

typedef enum {
  GPIO_ALTFUNCNUM_PIN15_Disable              = 0x0UL,                /*!< Altfunc disabled */
  GPIO_ALTFUNCNUM_PIN15_AF1                  = 0x1UL,                /*!< Altfunc 1 enable */
  GPIO_ALTFUNCNUM_PIN15_AF2                  = 0x2UL,                /*!< Altfunc 2 enable */
  GPIO_ALTFUNCNUM_PIN15_AF3                  = 0x3UL,                /*!< Altfunc 3 enable */
} GPIO_ALTFUNCNUM_PIN15_Enum;

/*--  SYNCSET: Additional double flip-flop syncronization enable register -------------------------------------*/
typedef struct {
  uint32_t PIN0                   :1;                                /*!< Additional double flip-flop syncronization buffer enable for pin 0 */
  uint32_t PIN1                   :1;                                /*!< Additional double flip-flop syncronization buffer enable for pin 1 */
  uint32_t PIN2                   :1;                                /*!< Additional double flip-flop syncronization buffer enable for pin 2 */
  uint32_t PIN3                   :1;                                /*!< Additional double flip-flop syncronization buffer enable for pin 3 */
  uint32_t PIN4                   :1;                                /*!< Additional double flip-flop syncronization buffer enable for pin 4 */
  uint32_t PIN5                   :1;                                /*!< Additional double flip-flop syncronization buffer enable for pin 5 */
  uint32_t PIN6                   :1;                                /*!< Additional double flip-flop syncronization buffer enable for pin 6 */
  uint32_t PIN7                   :1;                                /*!< Additional double flip-flop syncronization buffer enable for pin 7 */
  uint32_t PIN8                   :1;                                /*!< Additional double flip-flop syncronization buffer enable for pin 8 */
  uint32_t PIN9                   :1;                                /*!< Additional double flip-flop syncronization buffer enable for pin 9 */
  uint32_t PIN10                  :1;                                /*!< Additional double flip-flop syncronization buffer enable for pin 10 */
  uint32_t PIN11                  :1;                                /*!< Additional double flip-flop syncronization buffer enable for pin 11 */
  uint32_t PIN12                  :1;                                /*!< Additional double flip-flop syncronization buffer enable for pin 12 */
  uint32_t PIN13                  :1;                                /*!< Additional double flip-flop syncronization buffer enable for pin 13 */
  uint32_t PIN14                  :1;                                /*!< Additional double flip-flop syncronization buffer enable for pin 14 */
  uint32_t PIN15                  :1;                                /*!< Additional double flip-flop syncronization buffer enable for pin 15 */
} _GPIO_SYNCSET_bits;

/* Bit field positions: */
#define GPIO_SYNCSET_PIN0_Pos                 0                      /*!< Additional double flip-flop syncronization buffer enable for pin 0 */
#define GPIO_SYNCSET_PIN1_Pos                 1                      /*!< Additional double flip-flop syncronization buffer enable for pin 1 */
#define GPIO_SYNCSET_PIN2_Pos                 2                      /*!< Additional double flip-flop syncronization buffer enable for pin 2 */
#define GPIO_SYNCSET_PIN3_Pos                 3                      /*!< Additional double flip-flop syncronization buffer enable for pin 3 */
#define GPIO_SYNCSET_PIN4_Pos                 4                      /*!< Additional double flip-flop syncronization buffer enable for pin 4 */
#define GPIO_SYNCSET_PIN5_Pos                 5                      /*!< Additional double flip-flop syncronization buffer enable for pin 5 */
#define GPIO_SYNCSET_PIN6_Pos                 6                      /*!< Additional double flip-flop syncronization buffer enable for pin 6 */
#define GPIO_SYNCSET_PIN7_Pos                 7                      /*!< Additional double flip-flop syncronization buffer enable for pin 7 */
#define GPIO_SYNCSET_PIN8_Pos                 8                      /*!< Additional double flip-flop syncronization buffer enable for pin 8 */
#define GPIO_SYNCSET_PIN9_Pos                 9                      /*!< Additional double flip-flop syncronization buffer enable for pin 9 */
#define GPIO_SYNCSET_PIN10_Pos                10                     /*!< Additional double flip-flop syncronization buffer enable for pin 10 */
#define GPIO_SYNCSET_PIN11_Pos                11                     /*!< Additional double flip-flop syncronization buffer enable for pin 11 */
#define GPIO_SYNCSET_PIN12_Pos                12                     /*!< Additional double flip-flop syncronization buffer enable for pin 12 */
#define GPIO_SYNCSET_PIN13_Pos                13                     /*!< Additional double flip-flop syncronization buffer enable for pin 13 */
#define GPIO_SYNCSET_PIN14_Pos                14                     /*!< Additional double flip-flop syncronization buffer enable for pin 14 */
#define GPIO_SYNCSET_PIN15_Pos                15                     /*!< Additional double flip-flop syncronization buffer enable for pin 15 */

/* Bit field masks: */
#define GPIO_SYNCSET_PIN0_Msk                 0x00000001UL           /*!< Additional double flip-flop syncronization buffer enable for pin 0 */
#define GPIO_SYNCSET_PIN1_Msk                 0x00000002UL           /*!< Additional double flip-flop syncronization buffer enable for pin 1 */
#define GPIO_SYNCSET_PIN2_Msk                 0x00000004UL           /*!< Additional double flip-flop syncronization buffer enable for pin 2 */
#define GPIO_SYNCSET_PIN3_Msk                 0x00000008UL           /*!< Additional double flip-flop syncronization buffer enable for pin 3 */
#define GPIO_SYNCSET_PIN4_Msk                 0x00000010UL           /*!< Additional double flip-flop syncronization buffer enable for pin 4 */
#define GPIO_SYNCSET_PIN5_Msk                 0x00000020UL           /*!< Additional double flip-flop syncronization buffer enable for pin 5 */
#define GPIO_SYNCSET_PIN6_Msk                 0x00000040UL           /*!< Additional double flip-flop syncronization buffer enable for pin 6 */
#define GPIO_SYNCSET_PIN7_Msk                 0x00000080UL           /*!< Additional double flip-flop syncronization buffer enable for pin 7 */
#define GPIO_SYNCSET_PIN8_Msk                 0x00000100UL           /*!< Additional double flip-flop syncronization buffer enable for pin 8 */
#define GPIO_SYNCSET_PIN9_Msk                 0x00000200UL           /*!< Additional double flip-flop syncronization buffer enable for pin 9 */
#define GPIO_SYNCSET_PIN10_Msk                0x00000400UL           /*!< Additional double flip-flop syncronization buffer enable for pin 10 */
#define GPIO_SYNCSET_PIN11_Msk                0x00000800UL           /*!< Additional double flip-flop syncronization buffer enable for pin 11 */
#define GPIO_SYNCSET_PIN12_Msk                0x00001000UL           /*!< Additional double flip-flop syncronization buffer enable for pin 12 */
#define GPIO_SYNCSET_PIN13_Msk                0x00002000UL           /*!< Additional double flip-flop syncronization buffer enable for pin 13 */
#define GPIO_SYNCSET_PIN14_Msk                0x00004000UL           /*!< Additional double flip-flop syncronization buffer enable for pin 14 */
#define GPIO_SYNCSET_PIN15_Msk                0x00008000UL           /*!< Additional double flip-flop syncronization buffer enable for pin 15 */

/*--  SYNCCLR: Additional double flip-flop syncronization disable register ------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< Additional double flip-flop syncronization disable */
} _GPIO_SYNCCLR_bits;

/* Bit field positions: */
#define GPIO_SYNCCLR_VAL_Pos                  0                      /*!< Additional double flip-flop syncronization disable */

/* Bit field masks: */
#define GPIO_SYNCCLR_VAL_Msk                  0x0000FFFFUL           /*!< Additional double flip-flop syncronization disable */

/*--  QUALSET: Qualifier enable register ----------------------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :1;                                /*!< Qualifier enable for pin 0 */
  uint32_t PIN1                   :1;                                /*!< Qualifier enable for pin 1 */
  uint32_t PIN2                   :1;                                /*!< Qualifier enable for pin 2 */
  uint32_t PIN3                   :1;                                /*!< Qualifier enable for pin 3 */
  uint32_t PIN4                   :1;                                /*!< Qualifier enable for pin 4 */
  uint32_t PIN5                   :1;                                /*!< Qualifier enable for pin 5 */
  uint32_t PIN6                   :1;                                /*!< Qualifier enable for pin 6 */
  uint32_t PIN7                   :1;                                /*!< Qualifier enable for pin 7 */
  uint32_t PIN8                   :1;                                /*!< Qualifier enable for pin 8 */
  uint32_t PIN9                   :1;                                /*!< Qualifier enable for pin 9 */
  uint32_t PIN10                  :1;                                /*!< Qualifier enable for pin 10 */
  uint32_t PIN11                  :1;                                /*!< Qualifier enable for pin 11 */
  uint32_t PIN12                  :1;                                /*!< Qualifier enable for pin 12 */
  uint32_t PIN13                  :1;                                /*!< Qualifier enable for pin 13 */
  uint32_t PIN14                  :1;                                /*!< Qualifier enable for pin 14 */
  uint32_t PIN15                  :1;                                /*!< Qualifier enable for pin 15 */
} _GPIO_QUALSET_bits;

/* Bit field positions: */
#define GPIO_QUALSET_PIN0_Pos                 0                      /*!< Qualifier enable for pin 0 */
#define GPIO_QUALSET_PIN1_Pos                 1                      /*!< Qualifier enable for pin 1 */
#define GPIO_QUALSET_PIN2_Pos                 2                      /*!< Qualifier enable for pin 2 */
#define GPIO_QUALSET_PIN3_Pos                 3                      /*!< Qualifier enable for pin 3 */
#define GPIO_QUALSET_PIN4_Pos                 4                      /*!< Qualifier enable for pin 4 */
#define GPIO_QUALSET_PIN5_Pos                 5                      /*!< Qualifier enable for pin 5 */
#define GPIO_QUALSET_PIN6_Pos                 6                      /*!< Qualifier enable for pin 6 */
#define GPIO_QUALSET_PIN7_Pos                 7                      /*!< Qualifier enable for pin 7 */
#define GPIO_QUALSET_PIN8_Pos                 8                      /*!< Qualifier enable for pin 8 */
#define GPIO_QUALSET_PIN9_Pos                 9                      /*!< Qualifier enable for pin 9 */
#define GPIO_QUALSET_PIN10_Pos                10                     /*!< Qualifier enable for pin 10 */
#define GPIO_QUALSET_PIN11_Pos                11                     /*!< Qualifier enable for pin 11 */
#define GPIO_QUALSET_PIN12_Pos                12                     /*!< Qualifier enable for pin 12 */
#define GPIO_QUALSET_PIN13_Pos                13                     /*!< Qualifier enable for pin 13 */
#define GPIO_QUALSET_PIN14_Pos                14                     /*!< Qualifier enable for pin 14 */
#define GPIO_QUALSET_PIN15_Pos                15                     /*!< Qualifier enable for pin 15 */

/* Bit field masks: */
#define GPIO_QUALSET_PIN0_Msk                 0x00000001UL           /*!< Qualifier enable for pin 0 */
#define GPIO_QUALSET_PIN1_Msk                 0x00000002UL           /*!< Qualifier enable for pin 1 */
#define GPIO_QUALSET_PIN2_Msk                 0x00000004UL           /*!< Qualifier enable for pin 2 */
#define GPIO_QUALSET_PIN3_Msk                 0x00000008UL           /*!< Qualifier enable for pin 3 */
#define GPIO_QUALSET_PIN4_Msk                 0x00000010UL           /*!< Qualifier enable for pin 4 */
#define GPIO_QUALSET_PIN5_Msk                 0x00000020UL           /*!< Qualifier enable for pin 5 */
#define GPIO_QUALSET_PIN6_Msk                 0x00000040UL           /*!< Qualifier enable for pin 6 */
#define GPIO_QUALSET_PIN7_Msk                 0x00000080UL           /*!< Qualifier enable for pin 7 */
#define GPIO_QUALSET_PIN8_Msk                 0x00000100UL           /*!< Qualifier enable for pin 8 */
#define GPIO_QUALSET_PIN9_Msk                 0x00000200UL           /*!< Qualifier enable for pin 9 */
#define GPIO_QUALSET_PIN10_Msk                0x00000400UL           /*!< Qualifier enable for pin 10 */
#define GPIO_QUALSET_PIN11_Msk                0x00000800UL           /*!< Qualifier enable for pin 11 */
#define GPIO_QUALSET_PIN12_Msk                0x00001000UL           /*!< Qualifier enable for pin 12 */
#define GPIO_QUALSET_PIN13_Msk                0x00002000UL           /*!< Qualifier enable for pin 13 */
#define GPIO_QUALSET_PIN14_Msk                0x00004000UL           /*!< Qualifier enable for pin 14 */
#define GPIO_QUALSET_PIN15_Msk                0x00008000UL           /*!< Qualifier enable for pin 15 */

/*--  QUALCLR: Qualifier disable register ---------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< Qualifier disable */
} _GPIO_QUALCLR_bits;

/* Bit field positions: */
#define GPIO_QUALCLR_VAL_Pos                  0                      /*!< Qualifier disable */

/* Bit field masks: */
#define GPIO_QUALCLR_VAL_Msk                  0x0000FFFFUL           /*!< Qualifier disable */

/*--  QUALMODESET: Qualifier mode set register ----------------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :1;                                /*!< Qualifier mode set for pin 0 */
  uint32_t PIN1                   :1;                                /*!< Qualifier mode set for pin 1 */
  uint32_t PIN2                   :1;                                /*!< Qualifier mode set for pin 2 */
  uint32_t PIN3                   :1;                                /*!< Qualifier mode set for pin 3 */
  uint32_t PIN4                   :1;                                /*!< Qualifier mode set for pin 4 */
  uint32_t PIN5                   :1;                                /*!< Qualifier mode set for pin 5 */
  uint32_t PIN6                   :1;                                /*!< Qualifier mode set for pin 6 */
  uint32_t PIN7                   :1;                                /*!< Qualifier mode set for pin 7 */
  uint32_t PIN8                   :1;                                /*!< Qualifier mode set for pin 8 */
  uint32_t PIN9                   :1;                                /*!< Qualifier mode set for pin 9 */
  uint32_t PIN10                  :1;                                /*!< Qualifier mode set for pin 10 */
  uint32_t PIN11                  :1;                                /*!< Qualifier mode set for pin 11 */
  uint32_t PIN12                  :1;                                /*!< Qualifier mode set for pin 12 */
  uint32_t PIN13                  :1;                                /*!< Qualifier mode set for pin 13 */
  uint32_t PIN14                  :1;                                /*!< Qualifier mode set for pin 14 */
  uint32_t PIN15                  :1;                                /*!< Qualifier mode set for pin 15 */
} _GPIO_QUALMODESET_bits;

/* Bit field positions: */
#define GPIO_QUALMODESET_PIN0_Pos             0                      /*!< Qualifier mode set for pin 0 */
#define GPIO_QUALMODESET_PIN1_Pos             1                      /*!< Qualifier mode set for pin 1 */
#define GPIO_QUALMODESET_PIN2_Pos             2                      /*!< Qualifier mode set for pin 2 */
#define GPIO_QUALMODESET_PIN3_Pos             3                      /*!< Qualifier mode set for pin 3 */
#define GPIO_QUALMODESET_PIN4_Pos             4                      /*!< Qualifier mode set for pin 4 */
#define GPIO_QUALMODESET_PIN5_Pos             5                      /*!< Qualifier mode set for pin 5 */
#define GPIO_QUALMODESET_PIN6_Pos             6                      /*!< Qualifier mode set for pin 6 */
#define GPIO_QUALMODESET_PIN7_Pos             7                      /*!< Qualifier mode set for pin 7 */
#define GPIO_QUALMODESET_PIN8_Pos             8                      /*!< Qualifier mode set for pin 8 */
#define GPIO_QUALMODESET_PIN9_Pos             9                      /*!< Qualifier mode set for pin 9 */
#define GPIO_QUALMODESET_PIN10_Pos            10                     /*!< Qualifier mode set for pin 10 */
#define GPIO_QUALMODESET_PIN11_Pos            11                     /*!< Qualifier mode set for pin 11 */
#define GPIO_QUALMODESET_PIN12_Pos            12                     /*!< Qualifier mode set for pin 12 */
#define GPIO_QUALMODESET_PIN13_Pos            13                     /*!< Qualifier mode set for pin 13 */
#define GPIO_QUALMODESET_PIN14_Pos            14                     /*!< Qualifier mode set for pin 14 */
#define GPIO_QUALMODESET_PIN15_Pos            15                     /*!< Qualifier mode set for pin 15 */

/* Bit field masks: */
#define GPIO_QUALMODESET_PIN0_Msk             0x00000001UL           /*!< Qualifier mode set for pin 0 */
#define GPIO_QUALMODESET_PIN1_Msk             0x00000002UL           /*!< Qualifier mode set for pin 1 */
#define GPIO_QUALMODESET_PIN2_Msk             0x00000004UL           /*!< Qualifier mode set for pin 2 */
#define GPIO_QUALMODESET_PIN3_Msk             0x00000008UL           /*!< Qualifier mode set for pin 3 */
#define GPIO_QUALMODESET_PIN4_Msk             0x00000010UL           /*!< Qualifier mode set for pin 4 */
#define GPIO_QUALMODESET_PIN5_Msk             0x00000020UL           /*!< Qualifier mode set for pin 5 */
#define GPIO_QUALMODESET_PIN6_Msk             0x00000040UL           /*!< Qualifier mode set for pin 6 */
#define GPIO_QUALMODESET_PIN7_Msk             0x00000080UL           /*!< Qualifier mode set for pin 7 */
#define GPIO_QUALMODESET_PIN8_Msk             0x00000100UL           /*!< Qualifier mode set for pin 8 */
#define GPIO_QUALMODESET_PIN9_Msk             0x00000200UL           /*!< Qualifier mode set for pin 9 */
#define GPIO_QUALMODESET_PIN10_Msk            0x00000400UL           /*!< Qualifier mode set for pin 10 */
#define GPIO_QUALMODESET_PIN11_Msk            0x00000800UL           /*!< Qualifier mode set for pin 11 */
#define GPIO_QUALMODESET_PIN12_Msk            0x00001000UL           /*!< Qualifier mode set for pin 12 */
#define GPIO_QUALMODESET_PIN13_Msk            0x00002000UL           /*!< Qualifier mode set for pin 13 */
#define GPIO_QUALMODESET_PIN14_Msk            0x00004000UL           /*!< Qualifier mode set for pin 14 */
#define GPIO_QUALMODESET_PIN15_Msk            0x00008000UL           /*!< Qualifier mode set for pin 15 */

/*--  QUALMODECLR: Qualifier mode clear register --------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< Qualifier mode clear */
} _GPIO_QUALMODECLR_bits;

/* Bit field positions: */
#define GPIO_QUALMODECLR_VAL_Pos              0                      /*!< Qualifier mode clear */

/* Bit field masks: */
#define GPIO_QUALMODECLR_VAL_Msk              0x0000FFFFUL           /*!< Qualifier mode clear */

/*--  QUALSAMPLE: Qualifier sample period register ------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :20;                               /*!< Qualifier sample period */
} _GPIO_QUALSAMPLE_bits;

/* Bit field positions: */
#define GPIO_QUALSAMPLE_VAL_Pos               0                      /*!< Qualifier sample period */

/* Bit field masks: */
#define GPIO_QUALSAMPLE_VAL_Msk               0x000FFFFFUL           /*!< Qualifier sample period */

/*--  INTENSET: Interrupt enable register ---------------------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :1;                                /*!< Interrupt enable for pin 0 */
  uint32_t PIN1                   :1;                                /*!< Interrupt enable for pin 1 */
  uint32_t PIN2                   :1;                                /*!< Interrupt enable for pin 2 */
  uint32_t PIN3                   :1;                                /*!< Interrupt enable for pin 3 */
  uint32_t PIN4                   :1;                                /*!< Interrupt enable for pin 4 */
  uint32_t PIN5                   :1;                                /*!< Interrupt enable for pin 5 */
  uint32_t PIN6                   :1;                                /*!< Interrupt enable for pin 6 */
  uint32_t PIN7                   :1;                                /*!< Interrupt enable for pin 7 */
  uint32_t PIN8                   :1;                                /*!< Interrupt enable for pin 8 */
  uint32_t PIN9                   :1;                                /*!< Interrupt enable for pin 9 */
  uint32_t PIN10                  :1;                                /*!< Interrupt enable for pin 10 */
  uint32_t PIN11                  :1;                                /*!< Interrupt enable for pin 11 */
  uint32_t PIN12                  :1;                                /*!< Interrupt enable for pin 12 */
  uint32_t PIN13                  :1;                                /*!< Interrupt enable for pin 13 */
  uint32_t PIN14                  :1;                                /*!< Interrupt enable for pin 14 */
  uint32_t PIN15                  :1;                                /*!< Interrupt enable for pin 15 */
} _GPIO_INTENSET_bits;

/* Bit field positions: */
#define GPIO_INTENSET_PIN0_Pos                0                      /*!< Interrupt enable for pin 0 */
#define GPIO_INTENSET_PIN1_Pos                1                      /*!< Interrupt enable for pin 1 */
#define GPIO_INTENSET_PIN2_Pos                2                      /*!< Interrupt enable for pin 2 */
#define GPIO_INTENSET_PIN3_Pos                3                      /*!< Interrupt enable for pin 3 */
#define GPIO_INTENSET_PIN4_Pos                4                      /*!< Interrupt enable for pin 4 */
#define GPIO_INTENSET_PIN5_Pos                5                      /*!< Interrupt enable for pin 5 */
#define GPIO_INTENSET_PIN6_Pos                6                      /*!< Interrupt enable for pin 6 */
#define GPIO_INTENSET_PIN7_Pos                7                      /*!< Interrupt enable for pin 7 */
#define GPIO_INTENSET_PIN8_Pos                8                      /*!< Interrupt enable for pin 8 */
#define GPIO_INTENSET_PIN9_Pos                9                      /*!< Interrupt enable for pin 9 */
#define GPIO_INTENSET_PIN10_Pos               10                     /*!< Interrupt enable for pin 10 */
#define GPIO_INTENSET_PIN11_Pos               11                     /*!< Interrupt enable for pin 11 */
#define GPIO_INTENSET_PIN12_Pos               12                     /*!< Interrupt enable for pin 12 */
#define GPIO_INTENSET_PIN13_Pos               13                     /*!< Interrupt enable for pin 13 */
#define GPIO_INTENSET_PIN14_Pos               14                     /*!< Interrupt enable for pin 14 */
#define GPIO_INTENSET_PIN15_Pos               15                     /*!< Interrupt enable for pin 15 */

/* Bit field masks: */
#define GPIO_INTENSET_PIN0_Msk                0x00000001UL           /*!< Interrupt enable for pin 0 */
#define GPIO_INTENSET_PIN1_Msk                0x00000002UL           /*!< Interrupt enable for pin 1 */
#define GPIO_INTENSET_PIN2_Msk                0x00000004UL           /*!< Interrupt enable for pin 2 */
#define GPIO_INTENSET_PIN3_Msk                0x00000008UL           /*!< Interrupt enable for pin 3 */
#define GPIO_INTENSET_PIN4_Msk                0x00000010UL           /*!< Interrupt enable for pin 4 */
#define GPIO_INTENSET_PIN5_Msk                0x00000020UL           /*!< Interrupt enable for pin 5 */
#define GPIO_INTENSET_PIN6_Msk                0x00000040UL           /*!< Interrupt enable for pin 6 */
#define GPIO_INTENSET_PIN7_Msk                0x00000080UL           /*!< Interrupt enable for pin 7 */
#define GPIO_INTENSET_PIN8_Msk                0x00000100UL           /*!< Interrupt enable for pin 8 */
#define GPIO_INTENSET_PIN9_Msk                0x00000200UL           /*!< Interrupt enable for pin 9 */
#define GPIO_INTENSET_PIN10_Msk               0x00000400UL           /*!< Interrupt enable for pin 10 */
#define GPIO_INTENSET_PIN11_Msk               0x00000800UL           /*!< Interrupt enable for pin 11 */
#define GPIO_INTENSET_PIN12_Msk               0x00001000UL           /*!< Interrupt enable for pin 12 */
#define GPIO_INTENSET_PIN13_Msk               0x00002000UL           /*!< Interrupt enable for pin 13 */
#define GPIO_INTENSET_PIN14_Msk               0x00004000UL           /*!< Interrupt enable for pin 14 */
#define GPIO_INTENSET_PIN15_Msk               0x00008000UL           /*!< Interrupt enable for pin 15 */

/*--  INTENCLR: Interrupt disable register --------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< Interrupt disable */
} _GPIO_INTENCLR_bits;

/* Bit field positions: */
#define GPIO_INTENCLR_VAL_Pos                 0                      /*!< Interrupt disable */

/* Bit field masks: */
#define GPIO_INTENCLR_VAL_Msk                 0x0000FFFFUL           /*!< Interrupt disable */

/*--  INTTYPESET: Interrupt type set register -----------------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :1;                                /*!< Interrupt type set for pin 0 */
  uint32_t PIN1                   :1;                                /*!< Interrupt type set for pin 1 */
  uint32_t PIN2                   :1;                                /*!< Interrupt type set for pin 2 */
  uint32_t PIN3                   :1;                                /*!< Interrupt type set for pin 3 */
  uint32_t PIN4                   :1;                                /*!< Interrupt type set for pin 4 */
  uint32_t PIN5                   :1;                                /*!< Interrupt type set for pin 5 */
  uint32_t PIN6                   :1;                                /*!< Interrupt type set for pin 6 */
  uint32_t PIN7                   :1;                                /*!< Interrupt type set for pin 7 */
  uint32_t PIN8                   :1;                                /*!< Interrupt type set for pin 8 */
  uint32_t PIN9                   :1;                                /*!< Interrupt type set for pin 9 */
  uint32_t PIN10                  :1;                                /*!< Interrupt type set for pin 10 */
  uint32_t PIN11                  :1;                                /*!< Interrupt type set for pin 11 */
  uint32_t PIN12                  :1;                                /*!< Interrupt type set for pin 12 */
  uint32_t PIN13                  :1;                                /*!< Interrupt type set for pin 13 */
  uint32_t PIN14                  :1;                                /*!< Interrupt type set for pin 14 */
  uint32_t PIN15                  :1;                                /*!< Interrupt type set for pin 15 */
} _GPIO_INTTYPESET_bits;

/* Bit field positions: */
#define GPIO_INTTYPESET_PIN0_Pos              0                      /*!< Interrupt type set for pin 0 */
#define GPIO_INTTYPESET_PIN1_Pos              1                      /*!< Interrupt type set for pin 1 */
#define GPIO_INTTYPESET_PIN2_Pos              2                      /*!< Interrupt type set for pin 2 */
#define GPIO_INTTYPESET_PIN3_Pos              3                      /*!< Interrupt type set for pin 3 */
#define GPIO_INTTYPESET_PIN4_Pos              4                      /*!< Interrupt type set for pin 4 */
#define GPIO_INTTYPESET_PIN5_Pos              5                      /*!< Interrupt type set for pin 5 */
#define GPIO_INTTYPESET_PIN6_Pos              6                      /*!< Interrupt type set for pin 6 */
#define GPIO_INTTYPESET_PIN7_Pos              7                      /*!< Interrupt type set for pin 7 */
#define GPIO_INTTYPESET_PIN8_Pos              8                      /*!< Interrupt type set for pin 8 */
#define GPIO_INTTYPESET_PIN9_Pos              9                      /*!< Interrupt type set for pin 9 */
#define GPIO_INTTYPESET_PIN10_Pos             10                     /*!< Interrupt type set for pin 10 */
#define GPIO_INTTYPESET_PIN11_Pos             11                     /*!< Interrupt type set for pin 11 */
#define GPIO_INTTYPESET_PIN12_Pos             12                     /*!< Interrupt type set for pin 12 */
#define GPIO_INTTYPESET_PIN13_Pos             13                     /*!< Interrupt type set for pin 13 */
#define GPIO_INTTYPESET_PIN14_Pos             14                     /*!< Interrupt type set for pin 14 */
#define GPIO_INTTYPESET_PIN15_Pos             15                     /*!< Interrupt type set for pin 15 */

/* Bit field masks: */
#define GPIO_INTTYPESET_PIN0_Msk              0x00000001UL           /*!< Interrupt type set for pin 0 */
#define GPIO_INTTYPESET_PIN1_Msk              0x00000002UL           /*!< Interrupt type set for pin 1 */
#define GPIO_INTTYPESET_PIN2_Msk              0x00000004UL           /*!< Interrupt type set for pin 2 */
#define GPIO_INTTYPESET_PIN3_Msk              0x00000008UL           /*!< Interrupt type set for pin 3 */
#define GPIO_INTTYPESET_PIN4_Msk              0x00000010UL           /*!< Interrupt type set for pin 4 */
#define GPIO_INTTYPESET_PIN5_Msk              0x00000020UL           /*!< Interrupt type set for pin 5 */
#define GPIO_INTTYPESET_PIN6_Msk              0x00000040UL           /*!< Interrupt type set for pin 6 */
#define GPIO_INTTYPESET_PIN7_Msk              0x00000080UL           /*!< Interrupt type set for pin 7 */
#define GPIO_INTTYPESET_PIN8_Msk              0x00000100UL           /*!< Interrupt type set for pin 8 */
#define GPIO_INTTYPESET_PIN9_Msk              0x00000200UL           /*!< Interrupt type set for pin 9 */
#define GPIO_INTTYPESET_PIN10_Msk             0x00000400UL           /*!< Interrupt type set for pin 10 */
#define GPIO_INTTYPESET_PIN11_Msk             0x00000800UL           /*!< Interrupt type set for pin 11 */
#define GPIO_INTTYPESET_PIN12_Msk             0x00001000UL           /*!< Interrupt type set for pin 12 */
#define GPIO_INTTYPESET_PIN13_Msk             0x00002000UL           /*!< Interrupt type set for pin 13 */
#define GPIO_INTTYPESET_PIN14_Msk             0x00004000UL           /*!< Interrupt type set for pin 14 */
#define GPIO_INTTYPESET_PIN15_Msk             0x00008000UL           /*!< Interrupt type set for pin 15 */

/*--  INTTYPECLR: Interrupt type clear register ---------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< Interrupt type clear */
} _GPIO_INTTYPECLR_bits;

/* Bit field positions: */
#define GPIO_INTTYPECLR_VAL_Pos               0                      /*!< Interrupt type clear */

/* Bit field masks: */
#define GPIO_INTTYPECLR_VAL_Msk               0x0000FFFFUL           /*!< Interrupt type clear */

/*--  INTPOLSET: Interrupt polarity set register --------------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :1;                                /*!< Interrupt polarity set for pin 0 */
  uint32_t PIN1                   :1;                                /*!< Interrupt polarity set for pin 1 */
  uint32_t PIN2                   :1;                                /*!< Interrupt polarity set for pin 2 */
  uint32_t PIN3                   :1;                                /*!< Interrupt polarity set for pin 3 */
  uint32_t PIN4                   :1;                                /*!< Interrupt polarity set for pin 4 */
  uint32_t PIN5                   :1;                                /*!< Interrupt polarity set for pin 5 */
  uint32_t PIN6                   :1;                                /*!< Interrupt polarity set for pin 6 */
  uint32_t PIN7                   :1;                                /*!< Interrupt polarity set for pin 7 */
  uint32_t PIN8                   :1;                                /*!< Interrupt polarity set for pin 8 */
  uint32_t PIN9                   :1;                                /*!< Interrupt polarity set for pin 9 */
  uint32_t PIN10                  :1;                                /*!< Interrupt polarity set for pin 10 */
  uint32_t PIN11                  :1;                                /*!< Interrupt polarity set for pin 11 */
  uint32_t PIN12                  :1;                                /*!< Interrupt polarity set for pin 12 */
  uint32_t PIN13                  :1;                                /*!< Interrupt polarity set for pin 13 */
  uint32_t PIN14                  :1;                                /*!< Interrupt polarity set for pin 14 */
  uint32_t PIN15                  :1;                                /*!< Interrupt polarity set for pin 15 */
} _GPIO_INTPOLSET_bits;

/* Bit field positions: */
#define GPIO_INTPOLSET_PIN0_Pos               0                      /*!< Interrupt polarity set for pin 0 */
#define GPIO_INTPOLSET_PIN1_Pos               1                      /*!< Interrupt polarity set for pin 1 */
#define GPIO_INTPOLSET_PIN2_Pos               2                      /*!< Interrupt polarity set for pin 2 */
#define GPIO_INTPOLSET_PIN3_Pos               3                      /*!< Interrupt polarity set for pin 3 */
#define GPIO_INTPOLSET_PIN4_Pos               4                      /*!< Interrupt polarity set for pin 4 */
#define GPIO_INTPOLSET_PIN5_Pos               5                      /*!< Interrupt polarity set for pin 5 */
#define GPIO_INTPOLSET_PIN6_Pos               6                      /*!< Interrupt polarity set for pin 6 */
#define GPIO_INTPOLSET_PIN7_Pos               7                      /*!< Interrupt polarity set for pin 7 */
#define GPIO_INTPOLSET_PIN8_Pos               8                      /*!< Interrupt polarity set for pin 8 */
#define GPIO_INTPOLSET_PIN9_Pos               9                      /*!< Interrupt polarity set for pin 9 */
#define GPIO_INTPOLSET_PIN10_Pos              10                     /*!< Interrupt polarity set for pin 10 */
#define GPIO_INTPOLSET_PIN11_Pos              11                     /*!< Interrupt polarity set for pin 11 */
#define GPIO_INTPOLSET_PIN12_Pos              12                     /*!< Interrupt polarity set for pin 12 */
#define GPIO_INTPOLSET_PIN13_Pos              13                     /*!< Interrupt polarity set for pin 13 */
#define GPIO_INTPOLSET_PIN14_Pos              14                     /*!< Interrupt polarity set for pin 14 */
#define GPIO_INTPOLSET_PIN15_Pos              15                     /*!< Interrupt polarity set for pin 15 */

/* Bit field masks: */
#define GPIO_INTPOLSET_PIN0_Msk               0x00000001UL           /*!< Interrupt polarity set for pin 0 */
#define GPIO_INTPOLSET_PIN1_Msk               0x00000002UL           /*!< Interrupt polarity set for pin 1 */
#define GPIO_INTPOLSET_PIN2_Msk               0x00000004UL           /*!< Interrupt polarity set for pin 2 */
#define GPIO_INTPOLSET_PIN3_Msk               0x00000008UL           /*!< Interrupt polarity set for pin 3 */
#define GPIO_INTPOLSET_PIN4_Msk               0x00000010UL           /*!< Interrupt polarity set for pin 4 */
#define GPIO_INTPOLSET_PIN5_Msk               0x00000020UL           /*!< Interrupt polarity set for pin 5 */
#define GPIO_INTPOLSET_PIN6_Msk               0x00000040UL           /*!< Interrupt polarity set for pin 6 */
#define GPIO_INTPOLSET_PIN7_Msk               0x00000080UL           /*!< Interrupt polarity set for pin 7 */
#define GPIO_INTPOLSET_PIN8_Msk               0x00000100UL           /*!< Interrupt polarity set for pin 8 */
#define GPIO_INTPOLSET_PIN9_Msk               0x00000200UL           /*!< Interrupt polarity set for pin 9 */
#define GPIO_INTPOLSET_PIN10_Msk              0x00000400UL           /*!< Interrupt polarity set for pin 10 */
#define GPIO_INTPOLSET_PIN11_Msk              0x00000800UL           /*!< Interrupt polarity set for pin 11 */
#define GPIO_INTPOLSET_PIN12_Msk              0x00001000UL           /*!< Interrupt polarity set for pin 12 */
#define GPIO_INTPOLSET_PIN13_Msk              0x00002000UL           /*!< Interrupt polarity set for pin 13 */
#define GPIO_INTPOLSET_PIN14_Msk              0x00004000UL           /*!< Interrupt polarity set for pin 14 */
#define GPIO_INTPOLSET_PIN15_Msk              0x00008000UL           /*!< Interrupt polarity set for pin 15 */

/*--  INTPOLCLR: Interrupt polarity clear register ------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< Interrupt polarity clear */
} _GPIO_INTPOLCLR_bits;

/* Bit field positions: */
#define GPIO_INTPOLCLR_VAL_Pos                0                      /*!< Interrupt polarity clear */

/* Bit field masks: */
#define GPIO_INTPOLCLR_VAL_Msk                0x0000FFFFUL           /*!< Interrupt polarity clear */

/*--  INTEDGESET: Interrupt every edge set register -----------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :1;                                /*!< Interrupt every edge set for pin 0 */
  uint32_t PIN1                   :1;                                /*!< Interrupt every edge set for pin 1 */
  uint32_t PIN2                   :1;                                /*!< Interrupt every edge set for pin 2 */
  uint32_t PIN3                   :1;                                /*!< Interrupt every edge set for pin 3 */
  uint32_t PIN4                   :1;                                /*!< Interrupt every edge set for pin 4 */
  uint32_t PIN5                   :1;                                /*!< Interrupt every edge set for pin 5 */
  uint32_t PIN6                   :1;                                /*!< Interrupt every edge set for pin 6 */
  uint32_t PIN7                   :1;                                /*!< Interrupt every edge set for pin 7 */
  uint32_t PIN8                   :1;                                /*!< Interrupt every edge set for pin 8 */
  uint32_t PIN9                   :1;                                /*!< Interrupt every edge set for pin 9 */
  uint32_t PIN10                  :1;                                /*!< Interrupt every edge set for pin 10 */
  uint32_t PIN11                  :1;                                /*!< Interrupt every edge set for pin 11 */
  uint32_t PIN12                  :1;                                /*!< Interrupt every edge set for pin 12 */
  uint32_t PIN13                  :1;                                /*!< Interrupt every edge set for pin 13 */
  uint32_t PIN14                  :1;                                /*!< Interrupt every edge set for pin 14 */
  uint32_t PIN15                  :1;                                /*!< Interrupt every edge set for pin 15 */
} _GPIO_INTEDGESET_bits;

/* Bit field positions: */
#define GPIO_INTEDGESET_PIN0_Pos              0                      /*!< Interrupt every edge set for pin 0 */
#define GPIO_INTEDGESET_PIN1_Pos              1                      /*!< Interrupt every edge set for pin 1 */
#define GPIO_INTEDGESET_PIN2_Pos              2                      /*!< Interrupt every edge set for pin 2 */
#define GPIO_INTEDGESET_PIN3_Pos              3                      /*!< Interrupt every edge set for pin 3 */
#define GPIO_INTEDGESET_PIN4_Pos              4                      /*!< Interrupt every edge set for pin 4 */
#define GPIO_INTEDGESET_PIN5_Pos              5                      /*!< Interrupt every edge set for pin 5 */
#define GPIO_INTEDGESET_PIN6_Pos              6                      /*!< Interrupt every edge set for pin 6 */
#define GPIO_INTEDGESET_PIN7_Pos              7                      /*!< Interrupt every edge set for pin 7 */
#define GPIO_INTEDGESET_PIN8_Pos              8                      /*!< Interrupt every edge set for pin 8 */
#define GPIO_INTEDGESET_PIN9_Pos              9                      /*!< Interrupt every edge set for pin 9 */
#define GPIO_INTEDGESET_PIN10_Pos             10                     /*!< Interrupt every edge set for pin 10 */
#define GPIO_INTEDGESET_PIN11_Pos             11                     /*!< Interrupt every edge set for pin 11 */
#define GPIO_INTEDGESET_PIN12_Pos             12                     /*!< Interrupt every edge set for pin 12 */
#define GPIO_INTEDGESET_PIN13_Pos             13                     /*!< Interrupt every edge set for pin 13 */
#define GPIO_INTEDGESET_PIN14_Pos             14                     /*!< Interrupt every edge set for pin 14 */
#define GPIO_INTEDGESET_PIN15_Pos             15                     /*!< Interrupt every edge set for pin 15 */

/* Bit field masks: */
#define GPIO_INTEDGESET_PIN0_Msk              0x00000001UL           /*!< Interrupt every edge set for pin 0 */
#define GPIO_INTEDGESET_PIN1_Msk              0x00000002UL           /*!< Interrupt every edge set for pin 1 */
#define GPIO_INTEDGESET_PIN2_Msk              0x00000004UL           /*!< Interrupt every edge set for pin 2 */
#define GPIO_INTEDGESET_PIN3_Msk              0x00000008UL           /*!< Interrupt every edge set for pin 3 */
#define GPIO_INTEDGESET_PIN4_Msk              0x00000010UL           /*!< Interrupt every edge set for pin 4 */
#define GPIO_INTEDGESET_PIN5_Msk              0x00000020UL           /*!< Interrupt every edge set for pin 5 */
#define GPIO_INTEDGESET_PIN6_Msk              0x00000040UL           /*!< Interrupt every edge set for pin 6 */
#define GPIO_INTEDGESET_PIN7_Msk              0x00000080UL           /*!< Interrupt every edge set for pin 7 */
#define GPIO_INTEDGESET_PIN8_Msk              0x00000100UL           /*!< Interrupt every edge set for pin 8 */
#define GPIO_INTEDGESET_PIN9_Msk              0x00000200UL           /*!< Interrupt every edge set for pin 9 */
#define GPIO_INTEDGESET_PIN10_Msk             0x00000400UL           /*!< Interrupt every edge set for pin 10 */
#define GPIO_INTEDGESET_PIN11_Msk             0x00000800UL           /*!< Interrupt every edge set for pin 11 */
#define GPIO_INTEDGESET_PIN12_Msk             0x00001000UL           /*!< Interrupt every edge set for pin 12 */
#define GPIO_INTEDGESET_PIN13_Msk             0x00002000UL           /*!< Interrupt every edge set for pin 13 */
#define GPIO_INTEDGESET_PIN14_Msk             0x00004000UL           /*!< Interrupt every edge set for pin 14 */
#define GPIO_INTEDGESET_PIN15_Msk             0x00008000UL           /*!< Interrupt every edge set for pin 15 */

/*--  INTEDGECLR: Interrupt every edge clear register ---------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< Interrupt every edge clear */
} _GPIO_INTEDGECLR_bits;

/* Bit field positions: */
#define GPIO_INTEDGECLR_VAL_Pos               0                      /*!< Interrupt every edge clear */

/* Bit field masks: */
#define GPIO_INTEDGECLR_VAL_Msk               0x0000FFFFUL           /*!< Interrupt every edge clear */

/*--  INTSTATUS: Interrupt status -----------------------------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :1;                                /*!< Interrupt status of pin 0 */
  uint32_t PIN1                   :1;                                /*!< Interrupt status of pin 1 */
  uint32_t PIN2                   :1;                                /*!< Interrupt status of pin 2 */
  uint32_t PIN3                   :1;                                /*!< Interrupt status of pin 3 */
  uint32_t PIN4                   :1;                                /*!< Interrupt status of pin 4 */
  uint32_t PIN5                   :1;                                /*!< Interrupt status of pin 5 */
  uint32_t PIN6                   :1;                                /*!< Interrupt status of pin 6 */
  uint32_t PIN7                   :1;                                /*!< Interrupt status of pin 7 */
  uint32_t PIN8                   :1;                                /*!< Interrupt status of pin 8 */
  uint32_t PIN9                   :1;                                /*!< Interrupt status of pin 9 */
  uint32_t PIN10                  :1;                                /*!< Interrupt status of pin 10 */
  uint32_t PIN11                  :1;                                /*!< Interrupt status of pin 11 */
  uint32_t PIN12                  :1;                                /*!< Interrupt status of pin 12 */
  uint32_t PIN13                  :1;                                /*!< Interrupt status of pin 13 */
  uint32_t PIN14                  :1;                                /*!< Interrupt status of pin 14 */
  uint32_t PIN15                  :1;                                /*!< Interrupt status of pin 15 */
} _GPIO_INTSTATUS_bits;

/* Bit field positions: */
#define GPIO_INTSTATUS_PIN0_Pos               0                      /*!< Interrupt status of pin 0 */
#define GPIO_INTSTATUS_PIN1_Pos               1                      /*!< Interrupt status of pin 1 */
#define GPIO_INTSTATUS_PIN2_Pos               2                      /*!< Interrupt status of pin 2 */
#define GPIO_INTSTATUS_PIN3_Pos               3                      /*!< Interrupt status of pin 3 */
#define GPIO_INTSTATUS_PIN4_Pos               4                      /*!< Interrupt status of pin 4 */
#define GPIO_INTSTATUS_PIN5_Pos               5                      /*!< Interrupt status of pin 5 */
#define GPIO_INTSTATUS_PIN6_Pos               6                      /*!< Interrupt status of pin 6 */
#define GPIO_INTSTATUS_PIN7_Pos               7                      /*!< Interrupt status of pin 7 */
#define GPIO_INTSTATUS_PIN8_Pos               8                      /*!< Interrupt status of pin 8 */
#define GPIO_INTSTATUS_PIN9_Pos               9                      /*!< Interrupt status of pin 9 */
#define GPIO_INTSTATUS_PIN10_Pos              10                     /*!< Interrupt status of pin 10 */
#define GPIO_INTSTATUS_PIN11_Pos              11                     /*!< Interrupt status of pin 11 */
#define GPIO_INTSTATUS_PIN12_Pos              12                     /*!< Interrupt status of pin 12 */
#define GPIO_INTSTATUS_PIN13_Pos              13                     /*!< Interrupt status of pin 13 */
#define GPIO_INTSTATUS_PIN14_Pos              14                     /*!< Interrupt status of pin 14 */
#define GPIO_INTSTATUS_PIN15_Pos              15                     /*!< Interrupt status of pin 15 */

/* Bit field masks: */
#define GPIO_INTSTATUS_PIN0_Msk               0x00000001UL           /*!< Interrupt status of pin 0 */
#define GPIO_INTSTATUS_PIN1_Msk               0x00000002UL           /*!< Interrupt status of pin 1 */
#define GPIO_INTSTATUS_PIN2_Msk               0x00000004UL           /*!< Interrupt status of pin 2 */
#define GPIO_INTSTATUS_PIN3_Msk               0x00000008UL           /*!< Interrupt status of pin 3 */
#define GPIO_INTSTATUS_PIN4_Msk               0x00000010UL           /*!< Interrupt status of pin 4 */
#define GPIO_INTSTATUS_PIN5_Msk               0x00000020UL           /*!< Interrupt status of pin 5 */
#define GPIO_INTSTATUS_PIN6_Msk               0x00000040UL           /*!< Interrupt status of pin 6 */
#define GPIO_INTSTATUS_PIN7_Msk               0x00000080UL           /*!< Interrupt status of pin 7 */
#define GPIO_INTSTATUS_PIN8_Msk               0x00000100UL           /*!< Interrupt status of pin 8 */
#define GPIO_INTSTATUS_PIN9_Msk               0x00000200UL           /*!< Interrupt status of pin 9 */
#define GPIO_INTSTATUS_PIN10_Msk              0x00000400UL           /*!< Interrupt status of pin 10 */
#define GPIO_INTSTATUS_PIN11_Msk              0x00000800UL           /*!< Interrupt status of pin 11 */
#define GPIO_INTSTATUS_PIN12_Msk              0x00001000UL           /*!< Interrupt status of pin 12 */
#define GPIO_INTSTATUS_PIN13_Msk              0x00002000UL           /*!< Interrupt status of pin 13 */
#define GPIO_INTSTATUS_PIN14_Msk              0x00004000UL           /*!< Interrupt status of pin 14 */
#define GPIO_INTSTATUS_PIN15_Msk              0x00008000UL           /*!< Interrupt status of pin 15 */

/*--  DMAREQSET: DMA request enable register ------------------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :1;                                /*!< DMA request enable for pin 0 */
  uint32_t PIN1                   :1;                                /*!< DMA request enable for pin 1 */
  uint32_t PIN2                   :1;                                /*!< DMA request enable for pin 2 */
  uint32_t PIN3                   :1;                                /*!< DMA request enable for pin 3 */
  uint32_t PIN4                   :1;                                /*!< DMA request enable for pin 4 */
  uint32_t PIN5                   :1;                                /*!< DMA request enable for pin 5 */
  uint32_t PIN6                   :1;                                /*!< DMA request enable for pin 6 */
  uint32_t PIN7                   :1;                                /*!< DMA request enable for pin 7 */
  uint32_t PIN8                   :1;                                /*!< DMA request enable for pin 8 */
  uint32_t PIN9                   :1;                                /*!< DMA request enable for pin 9 */
  uint32_t PIN10                  :1;                                /*!< DMA request enable for pin 10 */
  uint32_t PIN11                  :1;                                /*!< DMA request enable for pin 11 */
  uint32_t PIN12                  :1;                                /*!< DMA request enable for pin 12 */
  uint32_t PIN13                  :1;                                /*!< DMA request enable for pin 13 */
  uint32_t PIN14                  :1;                                /*!< DMA request enable for pin 14 */
  uint32_t PIN15                  :1;                                /*!< DMA request enable for pin 15 */
} _GPIO_DMAREQSET_bits;

/* Bit field positions: */
#define GPIO_DMAREQSET_PIN0_Pos               0                      /*!< DMA request enable for pin 0 */
#define GPIO_DMAREQSET_PIN1_Pos               1                      /*!< DMA request enable for pin 1 */
#define GPIO_DMAREQSET_PIN2_Pos               2                      /*!< DMA request enable for pin 2 */
#define GPIO_DMAREQSET_PIN3_Pos               3                      /*!< DMA request enable for pin 3 */
#define GPIO_DMAREQSET_PIN4_Pos               4                      /*!< DMA request enable for pin 4 */
#define GPIO_DMAREQSET_PIN5_Pos               5                      /*!< DMA request enable for pin 5 */
#define GPIO_DMAREQSET_PIN6_Pos               6                      /*!< DMA request enable for pin 6 */
#define GPIO_DMAREQSET_PIN7_Pos               7                      /*!< DMA request enable for pin 7 */
#define GPIO_DMAREQSET_PIN8_Pos               8                      /*!< DMA request enable for pin 8 */
#define GPIO_DMAREQSET_PIN9_Pos               9                      /*!< DMA request enable for pin 9 */
#define GPIO_DMAREQSET_PIN10_Pos              10                     /*!< DMA request enable for pin 10 */
#define GPIO_DMAREQSET_PIN11_Pos              11                     /*!< DMA request enable for pin 11 */
#define GPIO_DMAREQSET_PIN12_Pos              12                     /*!< DMA request enable for pin 12 */
#define GPIO_DMAREQSET_PIN13_Pos              13                     /*!< DMA request enable for pin 13 */
#define GPIO_DMAREQSET_PIN14_Pos              14                     /*!< DMA request enable for pin 14 */
#define GPIO_DMAREQSET_PIN15_Pos              15                     /*!< DMA request enable for pin 15 */

/* Bit field masks: */
#define GPIO_DMAREQSET_PIN0_Msk               0x00000001UL           /*!< DMA request enable for pin 0 */
#define GPIO_DMAREQSET_PIN1_Msk               0x00000002UL           /*!< DMA request enable for pin 1 */
#define GPIO_DMAREQSET_PIN2_Msk               0x00000004UL           /*!< DMA request enable for pin 2 */
#define GPIO_DMAREQSET_PIN3_Msk               0x00000008UL           /*!< DMA request enable for pin 3 */
#define GPIO_DMAREQSET_PIN4_Msk               0x00000010UL           /*!< DMA request enable for pin 4 */
#define GPIO_DMAREQSET_PIN5_Msk               0x00000020UL           /*!< DMA request enable for pin 5 */
#define GPIO_DMAREQSET_PIN6_Msk               0x00000040UL           /*!< DMA request enable for pin 6 */
#define GPIO_DMAREQSET_PIN7_Msk               0x00000080UL           /*!< DMA request enable for pin 7 */
#define GPIO_DMAREQSET_PIN8_Msk               0x00000100UL           /*!< DMA request enable for pin 8 */
#define GPIO_DMAREQSET_PIN9_Msk               0x00000200UL           /*!< DMA request enable for pin 9 */
#define GPIO_DMAREQSET_PIN10_Msk              0x00000400UL           /*!< DMA request enable for pin 10 */
#define GPIO_DMAREQSET_PIN11_Msk              0x00000800UL           /*!< DMA request enable for pin 11 */
#define GPIO_DMAREQSET_PIN12_Msk              0x00001000UL           /*!< DMA request enable for pin 12 */
#define GPIO_DMAREQSET_PIN13_Msk              0x00002000UL           /*!< DMA request enable for pin 13 */
#define GPIO_DMAREQSET_PIN14_Msk              0x00004000UL           /*!< DMA request enable for pin 14 */
#define GPIO_DMAREQSET_PIN15_Msk              0x00008000UL           /*!< DMA request enable for pin 15 */

/*--  DMAREQCLR: DMA request disable register -----------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< DMA request disable */
} _GPIO_DMAREQCLR_bits;

/* Bit field positions: */
#define GPIO_DMAREQCLR_VAL_Pos                0                      /*!< DMA request disable */

/* Bit field masks: */
#define GPIO_DMAREQCLR_VAL_Msk                0x0000FFFFUL           /*!< DMA request disable */

/*--  ADCSOCSET: ADC Start Of Conversion enable register ------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :1;                                /*!< ADC SOC enable for pin 0 */
  uint32_t PIN1                   :1;                                /*!< ADC SOC enable for pin 1 */
  uint32_t PIN2                   :1;                                /*!< ADC SOC enable for pin 2 */
  uint32_t PIN3                   :1;                                /*!< ADC SOC enable for pin 3 */
  uint32_t PIN4                   :1;                                /*!< ADC SOC enable for pin 4 */
  uint32_t PIN5                   :1;                                /*!< ADC SOC enable for pin 5 */
  uint32_t PIN6                   :1;                                /*!< ADC SOC enable for pin 6 */
  uint32_t PIN7                   :1;                                /*!< ADC SOC enable for pin 7 */
  uint32_t PIN8                   :1;                                /*!< ADC SOC enable for pin 8 */
  uint32_t PIN9                   :1;                                /*!< ADC SOC enable for pin 9 */
  uint32_t PIN10                  :1;                                /*!< ADC SOC enable for pin 10 */
  uint32_t PIN11                  :1;                                /*!< ADC SOC enable for pin 11 */
  uint32_t PIN12                  :1;                                /*!< ADC SOC enable for pin 12 */
  uint32_t PIN13                  :1;                                /*!< ADC SOC enable for pin 13 */
  uint32_t PIN14                  :1;                                /*!< ADC SOC enable for pin 14 */
  uint32_t PIN15                  :1;                                /*!< ADC SOC enable for pin 15 */
} _GPIO_ADCSOCSET_bits;

/* Bit field positions: */
#define GPIO_ADCSOCSET_PIN0_Pos               0                      /*!< ADC SOC enable for pin 0 */
#define GPIO_ADCSOCSET_PIN1_Pos               1                      /*!< ADC SOC enable for pin 1 */
#define GPIO_ADCSOCSET_PIN2_Pos               2                      /*!< ADC SOC enable for pin 2 */
#define GPIO_ADCSOCSET_PIN3_Pos               3                      /*!< ADC SOC enable for pin 3 */
#define GPIO_ADCSOCSET_PIN4_Pos               4                      /*!< ADC SOC enable for pin 4 */
#define GPIO_ADCSOCSET_PIN5_Pos               5                      /*!< ADC SOC enable for pin 5 */
#define GPIO_ADCSOCSET_PIN6_Pos               6                      /*!< ADC SOC enable for pin 6 */
#define GPIO_ADCSOCSET_PIN7_Pos               7                      /*!< ADC SOC enable for pin 7 */
#define GPIO_ADCSOCSET_PIN8_Pos               8                      /*!< ADC SOC enable for pin 8 */
#define GPIO_ADCSOCSET_PIN9_Pos               9                      /*!< ADC SOC enable for pin 9 */
#define GPIO_ADCSOCSET_PIN10_Pos              10                     /*!< ADC SOC enable for pin 10 */
#define GPIO_ADCSOCSET_PIN11_Pos              11                     /*!< ADC SOC enable for pin 11 */
#define GPIO_ADCSOCSET_PIN12_Pos              12                     /*!< ADC SOC enable for pin 12 */
#define GPIO_ADCSOCSET_PIN13_Pos              13                     /*!< ADC SOC enable for pin 13 */
#define GPIO_ADCSOCSET_PIN14_Pos              14                     /*!< ADC SOC enable for pin 14 */
#define GPIO_ADCSOCSET_PIN15_Pos              15                     /*!< ADC SOC enable for pin 15 */

/* Bit field masks: */
#define GPIO_ADCSOCSET_PIN0_Msk               0x00000001UL           /*!< ADC SOC enable for pin 0 */
#define GPIO_ADCSOCSET_PIN1_Msk               0x00000002UL           /*!< ADC SOC enable for pin 1 */
#define GPIO_ADCSOCSET_PIN2_Msk               0x00000004UL           /*!< ADC SOC enable for pin 2 */
#define GPIO_ADCSOCSET_PIN3_Msk               0x00000008UL           /*!< ADC SOC enable for pin 3 */
#define GPIO_ADCSOCSET_PIN4_Msk               0x00000010UL           /*!< ADC SOC enable for pin 4 */
#define GPIO_ADCSOCSET_PIN5_Msk               0x00000020UL           /*!< ADC SOC enable for pin 5 */
#define GPIO_ADCSOCSET_PIN6_Msk               0x00000040UL           /*!< ADC SOC enable for pin 6 */
#define GPIO_ADCSOCSET_PIN7_Msk               0x00000080UL           /*!< ADC SOC enable for pin 7 */
#define GPIO_ADCSOCSET_PIN8_Msk               0x00000100UL           /*!< ADC SOC enable for pin 8 */
#define GPIO_ADCSOCSET_PIN9_Msk               0x00000200UL           /*!< ADC SOC enable for pin 9 */
#define GPIO_ADCSOCSET_PIN10_Msk              0x00000400UL           /*!< ADC SOC enable for pin 10 */
#define GPIO_ADCSOCSET_PIN11_Msk              0x00000800UL           /*!< ADC SOC enable for pin 11 */
#define GPIO_ADCSOCSET_PIN12_Msk              0x00001000UL           /*!< ADC SOC enable for pin 12 */
#define GPIO_ADCSOCSET_PIN13_Msk              0x00002000UL           /*!< ADC SOC enable for pin 13 */
#define GPIO_ADCSOCSET_PIN14_Msk              0x00004000UL           /*!< ADC SOC enable for pin 14 */
#define GPIO_ADCSOCSET_PIN15_Msk              0x00008000UL           /*!< ADC SOC enable for pin 15 */

/*--  ADCSOCCLR: ADC Start Of Conversion disable register -----------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< ADC SOC disable */
} _GPIO_ADCSOCCLR_bits;

/* Bit field positions: */
#define GPIO_ADCSOCCLR_VAL_Pos                0                      /*!< ADC SOC disable */

/* Bit field masks: */
#define GPIO_ADCSOCCLR_VAL_Msk                0x0000FFFFUL           /*!< ADC SOC disable */

/*--  RXEVSET: Core RXEV request enable register --------------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :1;                                /*!< RXEV enable for pin 0 */
  uint32_t PIN1                   :1;                                /*!< RXEV enable for pin 1 */
  uint32_t PIN2                   :1;                                /*!< RXEV enable for pin 2 */
  uint32_t PIN3                   :1;                                /*!< RXEV enable for pin 3 */
  uint32_t PIN4                   :1;                                /*!< RXEV enable for pin 4 */
  uint32_t PIN5                   :1;                                /*!< RXEV enable for pin 5 */
  uint32_t PIN6                   :1;                                /*!< RXEV enable for pin 6 */
  uint32_t PIN7                   :1;                                /*!< RXEV enable for pin 7 */
  uint32_t PIN8                   :1;                                /*!< RXEV enable for pin 8 */
  uint32_t PIN9                   :1;                                /*!< RXEV enable for pin 9 */
  uint32_t PIN10                  :1;                                /*!< RXEV enable for pin 10 */
  uint32_t PIN11                  :1;                                /*!< RXEV enable for pin 11 */
  uint32_t PIN12                  :1;                                /*!< RXEV enable for pin 12 */
  uint32_t PIN13                  :1;                                /*!< RXEV enable for pin 13 */
  uint32_t PIN14                  :1;                                /*!< RXEV enable for pin 14 */
  uint32_t PIN15                  :1;                                /*!< RXEV enable for pin 15 */
} _GPIO_RXEVSET_bits;

/* Bit field positions: */
#define GPIO_RXEVSET_PIN0_Pos                 0                      /*!< RXEV enable for pin 0 */
#define GPIO_RXEVSET_PIN1_Pos                 1                      /*!< RXEV enable for pin 1 */
#define GPIO_RXEVSET_PIN2_Pos                 2                      /*!< RXEV enable for pin 2 */
#define GPIO_RXEVSET_PIN3_Pos                 3                      /*!< RXEV enable for pin 3 */
#define GPIO_RXEVSET_PIN4_Pos                 4                      /*!< RXEV enable for pin 4 */
#define GPIO_RXEVSET_PIN5_Pos                 5                      /*!< RXEV enable for pin 5 */
#define GPIO_RXEVSET_PIN6_Pos                 6                      /*!< RXEV enable for pin 6 */
#define GPIO_RXEVSET_PIN7_Pos                 7                      /*!< RXEV enable for pin 7 */
#define GPIO_RXEVSET_PIN8_Pos                 8                      /*!< RXEV enable for pin 8 */
#define GPIO_RXEVSET_PIN9_Pos                 9                      /*!< RXEV enable for pin 9 */
#define GPIO_RXEVSET_PIN10_Pos                10                     /*!< RXEV enable for pin 10 */
#define GPIO_RXEVSET_PIN11_Pos                11                     /*!< RXEV enable for pin 11 */
#define GPIO_RXEVSET_PIN12_Pos                12                     /*!< RXEV enable for pin 12 */
#define GPIO_RXEVSET_PIN13_Pos                13                     /*!< RXEV enable for pin 13 */
#define GPIO_RXEVSET_PIN14_Pos                14                     /*!< RXEV enable for pin 14 */
#define GPIO_RXEVSET_PIN15_Pos                15                     /*!< RXEV enable for pin 15 */

/* Bit field masks: */
#define GPIO_RXEVSET_PIN0_Msk                 0x00000001UL           /*!< RXEV enable for pin 0 */
#define GPIO_RXEVSET_PIN1_Msk                 0x00000002UL           /*!< RXEV enable for pin 1 */
#define GPIO_RXEVSET_PIN2_Msk                 0x00000004UL           /*!< RXEV enable for pin 2 */
#define GPIO_RXEVSET_PIN3_Msk                 0x00000008UL           /*!< RXEV enable for pin 3 */
#define GPIO_RXEVSET_PIN4_Msk                 0x00000010UL           /*!< RXEV enable for pin 4 */
#define GPIO_RXEVSET_PIN5_Msk                 0x00000020UL           /*!< RXEV enable for pin 5 */
#define GPIO_RXEVSET_PIN6_Msk                 0x00000040UL           /*!< RXEV enable for pin 6 */
#define GPIO_RXEVSET_PIN7_Msk                 0x00000080UL           /*!< RXEV enable for pin 7 */
#define GPIO_RXEVSET_PIN8_Msk                 0x00000100UL           /*!< RXEV enable for pin 8 */
#define GPIO_RXEVSET_PIN9_Msk                 0x00000200UL           /*!< RXEV enable for pin 9 */
#define GPIO_RXEVSET_PIN10_Msk                0x00000400UL           /*!< RXEV enable for pin 10 */
#define GPIO_RXEVSET_PIN11_Msk                0x00000800UL           /*!< RXEV enable for pin 11 */
#define GPIO_RXEVSET_PIN12_Msk                0x00001000UL           /*!< RXEV enable for pin 12 */
#define GPIO_RXEVSET_PIN13_Msk                0x00002000UL           /*!< RXEV enable for pin 13 */
#define GPIO_RXEVSET_PIN14_Msk                0x00004000UL           /*!< RXEV enable for pin 14 */
#define GPIO_RXEVSET_PIN15_Msk                0x00008000UL           /*!< RXEV enable for pin 15 */

/*--  RXEVCLR: Core RXEV request disable register -------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< RXEV disable */
} _GPIO_RXEVCLR_bits;

/* Bit field positions: */
#define GPIO_RXEVCLR_VAL_Pos                  0                      /*!< RXEV disable */

/* Bit field masks: */
#define GPIO_RXEVCLR_VAL_Msk                  0x0000FFFFUL           /*!< RXEV disable */

/*--  LOCKKEY: Key register to unlock LOCKSET/LOCKCLR registers for write (KEY=0xADEADBEE) --------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!<  Key to unlock LOCKSET/LOCKCLR registers for write (KEY=0xADEADBEE) */
} _GPIO_LOCKKEY_bits;

/* Bit field positions: */
#define GPIO_LOCKKEY_VAL_Pos                  0                      /*!<  Key to unlock LOCKSET/LOCKCLR registers for write (KEY=0xADEADBEE) */

/* Bit field masks: */
#define GPIO_LOCKKEY_VAL_Msk                  0xFFFFFFFFUL           /*!<  Key to unlock LOCKSET/LOCKCLR registers for write (KEY=0xADEADBEE) */

/* Bit field enums: */
typedef enum {
  GPIO_LOCKKEY_VAL_LOCK                      = -286331154,           /*!< 0xEEEEEEEE, key to lock registers */
  GPIO_LOCKKEY_VAL_UNLOCK                    = -1377117202,          /*!< 0xADEADBEE, key to unlock registers */
} GPIO_LOCKKEY_VAL_Enum;

/*--  LOCKSET: Lock pins configuration enable register --------------------------------------------------------*/
typedef struct {
  uint32_t PIN0                   :1;                                /*!< Lock configuration enable for pin 0 */
  uint32_t PIN1                   :1;                                /*!< Lock configuration enable for pin 1 */
  uint32_t PIN2                   :1;                                /*!< Lock configuration enable for pin 2 */
  uint32_t PIN3                   :1;                                /*!< Lock configuration enable for pin 3 */
  uint32_t PIN4                   :1;                                /*!< Lock configuration enable for pin 4 */
  uint32_t PIN5                   :1;                                /*!< Lock configuration enable for pin 5 */
  uint32_t PIN6                   :1;                                /*!< Lock configuration enable for pin 6 */
  uint32_t PIN7                   :1;                                /*!< Lock configuration enable for pin 7 */
  uint32_t PIN8                   :1;                                /*!< Lock configuration enable for pin 8 */
  uint32_t PIN9                   :1;                                /*!< Lock configuration enable for pin 9 */
  uint32_t PIN10                  :1;                                /*!< Lock configuration enable for pin 10 */
  uint32_t PIN11                  :1;                                /*!< Lock configuration enable for pin 11 */
  uint32_t PIN12                  :1;                                /*!< Lock configuration enable for pin 12 */
  uint32_t PIN13                  :1;                                /*!< Lock configuration enable for pin 13 */
  uint32_t PIN14                  :1;                                /*!< Lock configuration enable for pin 14 */
  uint32_t PIN15                  :1;                                /*!< Lock configuration enable for pin 15 */
} _GPIO_LOCKSET_bits;

/* Bit field positions: */
#define GPIO_LOCKSET_PIN0_Pos                 0                      /*!< Lock configuration enable for pin 0 */
#define GPIO_LOCKSET_PIN1_Pos                 1                      /*!< Lock configuration enable for pin 1 */
#define GPIO_LOCKSET_PIN2_Pos                 2                      /*!< Lock configuration enable for pin 2 */
#define GPIO_LOCKSET_PIN3_Pos                 3                      /*!< Lock configuration enable for pin 3 */
#define GPIO_LOCKSET_PIN4_Pos                 4                      /*!< Lock configuration enable for pin 4 */
#define GPIO_LOCKSET_PIN5_Pos                 5                      /*!< Lock configuration enable for pin 5 */
#define GPIO_LOCKSET_PIN6_Pos                 6                      /*!< Lock configuration enable for pin 6 */
#define GPIO_LOCKSET_PIN7_Pos                 7                      /*!< Lock configuration enable for pin 7 */
#define GPIO_LOCKSET_PIN8_Pos                 8                      /*!< Lock configuration enable for pin 8 */
#define GPIO_LOCKSET_PIN9_Pos                 9                      /*!< Lock configuration enable for pin 9 */
#define GPIO_LOCKSET_PIN10_Pos                10                     /*!< Lock configuration enable for pin 10 */
#define GPIO_LOCKSET_PIN11_Pos                11                     /*!< Lock configuration enable for pin 11 */
#define GPIO_LOCKSET_PIN12_Pos                12                     /*!< Lock configuration enable for pin 12 */
#define GPIO_LOCKSET_PIN13_Pos                13                     /*!< Lock configuration enable for pin 13 */
#define GPIO_LOCKSET_PIN14_Pos                14                     /*!< Lock configuration enable for pin 14 */
#define GPIO_LOCKSET_PIN15_Pos                15                     /*!< Lock configuration enable for pin 15 */

/* Bit field masks: */
#define GPIO_LOCKSET_PIN0_Msk                 0x00000001UL           /*!< Lock configuration enable for pin 0 */
#define GPIO_LOCKSET_PIN1_Msk                 0x00000002UL           /*!< Lock configuration enable for pin 1 */
#define GPIO_LOCKSET_PIN2_Msk                 0x00000004UL           /*!< Lock configuration enable for pin 2 */
#define GPIO_LOCKSET_PIN3_Msk                 0x00000008UL           /*!< Lock configuration enable for pin 3 */
#define GPIO_LOCKSET_PIN4_Msk                 0x00000010UL           /*!< Lock configuration enable for pin 4 */
#define GPIO_LOCKSET_PIN5_Msk                 0x00000020UL           /*!< Lock configuration enable for pin 5 */
#define GPIO_LOCKSET_PIN6_Msk                 0x00000040UL           /*!< Lock configuration enable for pin 6 */
#define GPIO_LOCKSET_PIN7_Msk                 0x00000080UL           /*!< Lock configuration enable for pin 7 */
#define GPIO_LOCKSET_PIN8_Msk                 0x00000100UL           /*!< Lock configuration enable for pin 8 */
#define GPIO_LOCKSET_PIN9_Msk                 0x00000200UL           /*!< Lock configuration enable for pin 9 */
#define GPIO_LOCKSET_PIN10_Msk                0x00000400UL           /*!< Lock configuration enable for pin 10 */
#define GPIO_LOCKSET_PIN11_Msk                0x00000800UL           /*!< Lock configuration enable for pin 11 */
#define GPIO_LOCKSET_PIN12_Msk                0x00001000UL           /*!< Lock configuration enable for pin 12 */
#define GPIO_LOCKSET_PIN13_Msk                0x00002000UL           /*!< Lock configuration enable for pin 13 */
#define GPIO_LOCKSET_PIN14_Msk                0x00004000UL           /*!< Lock configuration enable for pin 14 */
#define GPIO_LOCKSET_PIN15_Msk                0x00008000UL           /*!< Lock configuration enable for pin 15 */

/*--  LOCKCLR: Lock pins configuration disable register -------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< Lock configuration disable */
} _GPIO_LOCKCLR_bits;

/* Bit field positions: */
#define GPIO_LOCKCLR_VAL_Pos                  0                      /*!< Lock configuration disable */

/* Bit field masks: */
#define GPIO_LOCKCLR_VAL_Msk                  0x0000FFFFUL           /*!< Lock configuration disable */

/*--  MASKLB: Mask register low byte of port ------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :8;                                /*!< Mask low byte */
} _GPIO_MASKLB_bits;

/* Bit field positions: */
#define GPIO_MASKLB_VAL_Pos                   0                      /*!< Mask low byte */

/* Bit field masks: */
#define GPIO_MASKLB_VAL_Msk                   0x000000FFUL           /*!< Mask low byte */

/*--  MASKHB: Mask register High byte of port -----------------------------------------------------------------*/
typedef struct {
  uint32_t                        :8;                                /*!< RESERVED */
  uint32_t VAL                    :8;                                /*!< Mask high byte */
} _GPIO_MASKHB_bits;

/* Bit field positions: */
#define GPIO_MASKHB_VAL_Pos                   8                      /*!< Mask high byte */

/* Bit field masks: */
#define GPIO_MASKHB_VAL_Msk                   0x0000FF00UL           /*!< Mask high byte */

//Cluster MASKLB:
typedef struct {
  union { 
  /*!< Mask register low byte of port */
    __IO uint32_t MASKLB;                                            /*!< MASKLB : type used for word access */
    __IO _GPIO_MASKLB_bits MASKLB_bit;                               /*!< MASKLB_bit: structure used for bit access */
  };
} _GPIO_MASKLB_TypeDef;
//Cluster MASKHB:
typedef struct {
  union { 
  /*!< Mask register High byte of port */
    __IO uint32_t MASKHB;                                            /*!< MASKHB : type used for word access */
    __IO _GPIO_MASKHB_bits MASKHB_bit;                               /*!< MASKHB_bit: structure used for bit access */
  };
} _GPIO_MASKHB_TypeDef;
typedef struct {
  union {                                                               /*!< Data Input register */
    __I uint32_t DATA;                                               /*!< DATA    : type used for word access */
    __I _GPIO_DATA_bits  DATA_bit;                                   /*!< DATA_bit: structure used for bit access */
  };
  union {                                                               /*!< Data output register */
    __IO uint32_t DATAOUT;                                           /*!< DATAOUT    : type used for word access */
    __IO _GPIO_DATAOUT_bits  DATAOUT_bit;                            /*!< DATAOUT_bit: structure used for bit access */
  };
  union {                                                               /*!< Data output set bits register */
    __O uint32_t DATAOUTSET;                                           /*!< DATAOUTSET    : type used for word access */
    __O _GPIO_DATAOUTSET_bits  DATAOUTSET_bit;                       /*!< DATAOUTSET_bit: structure used for bit access */
  };
  union {                                                               /*!< Data output clear bits register */
    __O uint32_t DATAOUTCLR;                                           /*!< DATAOUTCLR    : type used for word access */
    __O _GPIO_DATAOUTCLR_bits  DATAOUTCLR_bit;                       /*!< DATAOUTCLR_bit: structure used for bit access */
  };
  union {                                                               /*!< Data output toggle bits register */
    __O uint32_t DATAOUTTGL;                                           /*!< DATAOUTTGL    : type used for word access */
    __O _GPIO_DATAOUTTGL_bits  DATAOUTTGL_bit;                       /*!< DATAOUTTGL_bit: structure used for bit access */
  };
  __IO uint32_t Reserved0[2];
  union {                                                               /*!< Select input mode register */
    __IO uint32_t INMODE;                                            /*!< INMODE    : type used for word access */
    __IO _GPIO_INMODE_bits  INMODE_bit;                              /*!< INMODE_bit: structure used for bit access */
  };
  union {                                                               /*!< Select pull mode register */
    __IO uint32_t PULLMODE;                                           /*!< PULLMODE    : type used for word access */
    __IO _GPIO_PULLMODE_bits  PULLMODE_bit;                          /*!< PULLMODE_bit: structure used for bit access */
  };
  union {                                                               /*!< Select output mode register */
    __IO uint32_t OUTMODE;                                           /*!< OUTMODE    : type used for word access */
    __IO _GPIO_OUTMODE_bits  OUTMODE_bit;                            /*!< OUTMODE_bit: structure used for bit access */
  };
  __IO uint32_t Reserved1;
  union {                                                               /*!< Output enable register */
    __IO uint32_t OUTENSET;                                           /*!< OUTENSET    : type used for word access */
    __IO _GPIO_OUTENSET_bits  OUTENSET_bit;                          /*!< OUTENSET_bit: structure used for bit access */
  };
  union {                                                               /*!< Output disable register */
    __O uint32_t OUTENCLR;                                           /*!< OUTENCLR    : type used for word access */
    __O _GPIO_OUTENCLR_bits  OUTENCLR_bit;                           /*!< OUTENCLR_bit: structure used for bit access */
  };
  union {                                                               /*!< Alternative function enable register */
    __IO uint32_t ALTFUNCSET;                                           /*!< ALTFUNCSET    : type used for word access */
    __IO _GPIO_ALTFUNCSET_bits  ALTFUNCSET_bit;                      /*!< ALTFUNCSET_bit: structure used for bit access */
  };
  union {                                                               /*!< Alternative function disable register */
    __O uint32_t ALTFUNCCLR;                                           /*!< ALTFUNCCLR    : type used for word access */
    __O _GPIO_ALTFUNCCLR_bits  ALTFUNCCLR_bit;                       /*!< ALTFUNCCLR_bit: structure used for bit access */
  };
  union {                                                               /*!< Alternative function number register */
    __IO uint32_t ALTFUNCNUM;                                           /*!< ALTFUNCNUM    : type used for word access */
    __IO _GPIO_ALTFUNCNUM_bits  ALTFUNCNUM_bit;                      /*!< ALTFUNCNUM_bit: structure used for bit access */
  };
    __IO uint32_t Reserved2;
  union {                                                               /*!< Additional double flip-flop syncronization enable register */
    __IO uint32_t SYNCSET;                                           /*!< SYNCSET    : type used for word access */
    __IO _GPIO_SYNCSET_bits  SYNCSET_bit;                            /*!< SYNCSET_bit: structure used for bit access */
  };
  union {                                                               /*!< Additional double flip-flop syncronization disable register */
    __O uint32_t SYNCCLR;                                            /*!< SYNCCLR    : type used for word access */
    __O _GPIO_SYNCCLR_bits  SYNCCLR_bit;                             /*!< SYNCCLR_bit: structure used for bit access */
  };
  union {                                                               /*!< Qualifier enable register */
    __IO uint32_t QUALSET;                                           /*!< QUALSET    : type used for word access */
    __IO _GPIO_QUALSET_bits  QUALSET_bit;                            /*!< QUALSET_bit: structure used for bit access */
  };
  union {                                                               /*!< Qualifier disable register */
    __O uint32_t QUALCLR;                                            /*!< QUALCLR    : type used for word access */
    __O _GPIO_QUALCLR_bits  QUALCLR_bit;                             /*!< QUALCLR_bit: structure used for bit access */
  };
  union {                                                               /*!< Qualifier mode set register */
    __IO uint32_t QUALMODESET;                                           /*!< QUALMODESET    : type used for word access */
    __IO _GPIO_QUALMODESET_bits  QUALMODESET_bit;                    /*!< QUALMODESET_bit: structure used for bit access */
  };
  union {                                                               /*!< Qualifier mode clear register */
    __O uint32_t QUALMODECLR;                                           /*!< QUALMODECLR    : type used for word access */
    __O _GPIO_QUALMODECLR_bits  QUALMODECLR_bit;                     /*!< QUALMODECLR_bit: structure used for bit access */
  };
  union {                                                               /*!< Qualifier sample period register */
    __IO uint32_t QUALSAMPLE;                                           /*!< QUALSAMPLE    : type used for word access */
    __IO _GPIO_QUALSAMPLE_bits  QUALSAMPLE_bit;                      /*!< QUALSAMPLE_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt enable register */
    __IO uint32_t INTENSET;                                           /*!< INTENSET    : type used for word access */
    __IO _GPIO_INTENSET_bits  INTENSET_bit;                          /*!< INTENSET_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt disable register */
    __O uint32_t INTENCLR;                                           /*!< INTENCLR    : type used for word access */
    __O _GPIO_INTENCLR_bits  INTENCLR_bit;                           /*!< INTENCLR_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt type set register */
    __IO uint32_t INTTYPESET;                                           /*!< INTTYPESET    : type used for word access */
    __IO _GPIO_INTTYPESET_bits  INTTYPESET_bit;                      /*!< INTTYPESET_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt type clear register */
    __O uint32_t INTTYPECLR;                                           /*!< INTTYPECLR    : type used for word access */
    __O _GPIO_INTTYPECLR_bits  INTTYPECLR_bit;                       /*!< INTTYPECLR_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt polarity set register */
    __IO uint32_t INTPOLSET;                                           /*!< INTPOLSET    : type used for word access */
    __IO _GPIO_INTPOLSET_bits  INTPOLSET_bit;                        /*!< INTPOLSET_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt polarity clear register */
    __O uint32_t INTPOLCLR;                                           /*!< INTPOLCLR    : type used for word access */
    __O _GPIO_INTPOLCLR_bits  INTPOLCLR_bit;                         /*!< INTPOLCLR_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt every edge set register */
    __IO uint32_t INTEDGESET;                                           /*!< INTEDGESET    : type used for word access */
    __IO _GPIO_INTEDGESET_bits  INTEDGESET_bit;                      /*!< INTEDGESET_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt every edge clear register */
    __O uint32_t INTEDGECLR;                                           /*!< INTEDGECLR    : type used for word access */
    __O _GPIO_INTEDGECLR_bits  INTEDGECLR_bit;                       /*!< INTEDGECLR_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt status */
    __IO uint32_t INTSTATUS;                                           /*!< INTSTATUS    : type used for word access */
    __IO _GPIO_INTSTATUS_bits  INTSTATUS_bit;                        /*!< INTSTATUS_bit: structure used for bit access */
  };
  union {                                                               /*!< DMA request enable register */
    __IO uint32_t DMAREQSET;                                           /*!< DMAREQSET    : type used for word access */
    __IO _GPIO_DMAREQSET_bits  DMAREQSET_bit;                        /*!< DMAREQSET_bit: structure used for bit access */
  };
  union {                                                               /*!< DMA request disable register */
    __O uint32_t DMAREQCLR;                                           /*!< DMAREQCLR    : type used for word access */
    __O _GPIO_DMAREQCLR_bits  DMAREQCLR_bit;                         /*!< DMAREQCLR_bit: structure used for bit access */
  };
  union {                                                               /*!< ADC Start Of Conversion enable register */
    __IO uint32_t ADCSOCSET;                                           /*!< ADCSOCSET    : type used for word access */
    __IO _GPIO_ADCSOCSET_bits  ADCSOCSET_bit;                        /*!< ADCSOCSET_bit: structure used for bit access */
  };
  union {                                                               /*!< ADC Start Of Conversion disable register */
    __O uint32_t ADCSOCCLR;                                           /*!< ADCSOCCLR    : type used for word access */
    __O _GPIO_ADCSOCCLR_bits  ADCSOCCLR_bit;                         /*!< ADCSOCCLR_bit: structure used for bit access */
  };
  __IO uint32_t Reserved3[2];
  union {                                                               /*!< Key register to unlock LOCKSET/LOCKCLR registers for write (KEY=0xADEADBEE) */
    __O uint32_t LOCKKEY;                                            /*!< LOCKKEY    : type used for word access */
    __O _GPIO_LOCKKEY_bits  LOCKKEY_bit;                             /*!< LOCKKEY_bit: structure used for bit access */
  };
  union {                                                               /*!< Lock pins configuration enable register */
    __IO uint32_t LOCKSET;                                           /*!< LOCKSET    : type used for word access */
    __IO _GPIO_LOCKSET_bits  LOCKSET_bit;                            /*!< LOCKSET_bit: structure used for bit access */
  };
  union {                                                               /*!< Lock pins configuration disable register */
    __O uint32_t LOCKCLR;                                            /*!< LOCKCLR    : type used for word access */
    __O _GPIO_LOCKCLR_bits  LOCKCLR_bit;                             /*!< LOCKCLR_bit: structure used for bit access */
  };
    __IO uint32_t Reserved4[214];
  _GPIO_MASKLB_TypeDef MASKLB[256];
  _GPIO_MASKHB_TypeDef MASKHB[256];
} GPIO_TypeDef;


/******************************************************************************/
/*                         USB registers                                      */
/******************************************************************************/

/*--  INTSTAT0: Interrupt status register ---------------------------------------------------------------------*/
typedef struct {
  uint32_t USBBUSINT              :1;                                /*!< Interrupt flag events on the USB bus */
  uint32_t CEP_INT                :1;                                /*!< Interrupt flag Control EndPoint */
  uint32_t EP0_INT                :1;                                /*!< Interrupt flags EndPoint 0 */
  uint32_t EP1_INT                :1;                                /*!< Interrupt flags EndPoint 1 */
  uint32_t EP2_INT                :1;                                /*!< Interrupt flags EndPoint 2 */
  uint32_t EP3_INT                :1;                                /*!< Interrupt flags EndPoint 3 */
} _USB_INTSTAT0_bits;

/* Bit field positions: */
#define USB_INTSTAT0_USBBUSINT_Pos            0                      /*!< Interrupt flag events on the USB bus */
#define USB_INTSTAT0_CEP_INT_Pos              1                      /*!< Interrupt flag Control EndPoint */
#define USB_INTSTAT0_EP0_INT_Pos              2                      /*!< Interrupt flags EndPoint 0 */
#define USB_INTSTAT0_EP1_INT_Pos              3                      /*!< Interrupt flags EndPoint 1 */
#define USB_INTSTAT0_EP2_INT_Pos              4                      /*!< Interrupt flags EndPoint 2 */
#define USB_INTSTAT0_EP3_INT_Pos              5                      /*!< Interrupt flags EndPoint 3 */

/* Bit field masks: */
#define USB_INTSTAT0_USBBUSINT_Msk            0x00000001UL           /*!< Interrupt flag events on the USB bus */
#define USB_INTSTAT0_CEP_INT_Msk              0x00000002UL           /*!< Interrupt flag Control EndPoint */
#define USB_INTSTAT0_EP0_INT_Msk              0x00000004UL           /*!< Interrupt flags EndPoint 0 */
#define USB_INTSTAT0_EP1_INT_Msk              0x00000008UL           /*!< Interrupt flags EndPoint 1 */
#define USB_INTSTAT0_EP2_INT_Msk              0x00000010UL           /*!< Interrupt flags EndPoint 2 */
#define USB_INTSTAT0_EP3_INT_Msk              0x00000020UL           /*!< Interrupt flags EndPoint 3 */

/*--  INTEN0: Interrupt enable register -----------------------------------------------------------------------*/
typedef struct {
  uint32_t USBBUSINTEN            :1;                                /*!< Enable interrupts from the events on the USB bus */
  uint32_t CEP_INTEN              :1;                                /*!< Interrupt enable for Control EndPoint */
  uint32_t EP0_INTEN              :1;                                /*!< Interrupt enable for EndPoint 0 */
  uint32_t EP1_INTEN              :1;                                /*!< Interrupt enable for EndPoint 1 */
  uint32_t EP2_INTEN              :1;                                /*!< Interrupt enable for EndPoint 2 */
  uint32_t EP3_INTEN              :1;                                /*!< Interrupt enable for EndPoint 3 */
} _USB_INTEN0_bits;

/* Bit field positions: */
#define USB_INTEN0_USBBUSINTEN_Pos            0                      /*!< Enable interrupts from the events on the USB bus */
#define USB_INTEN0_CEP_INTEN_Pos              1                      /*!< Interrupt enable for Control EndPoint */
#define USB_INTEN0_EP0_INTEN_Pos              2                      /*!< Interrupt enable for EndPoint 0 */
#define USB_INTEN0_EP1_INTEN_Pos              3                      /*!< Interrupt enable for EndPoint 1 */
#define USB_INTEN0_EP2_INTEN_Pos              4                      /*!< Interrupt enable for EndPoint 2 */
#define USB_INTEN0_EP3_INTEN_Pos              5                      /*!< Interrupt enable for EndPoint 3 */

/* Bit field masks: */
#define USB_INTEN0_USBBUSINTEN_Msk            0x00000001UL           /*!< Enable interrupts from the events on the USB bus */
#define USB_INTEN0_CEP_INTEN_Msk              0x00000002UL           /*!< Interrupt enable for Control EndPoint */
#define USB_INTEN0_EP0_INTEN_Msk              0x00000004UL           /*!< Interrupt enable for EndPoint 0 */
#define USB_INTEN0_EP1_INTEN_Msk              0x00000008UL           /*!< Interrupt enable for EndPoint 1 */
#define USB_INTEN0_EP2_INTEN_Msk              0x00000010UL           /*!< Interrupt enable for EndPoint 2 */
#define USB_INTEN0_EP3_INTEN_Msk              0x00000020UL           /*!< Interrupt enable for EndPoint 3 */

/*--  INTSTAT1: USB Interrupt status register -----------------------------------------------------------------*/
typedef struct {
  uint32_t SOF                    :1;                                /*!< Flag SOF packet reception */
  uint32_t RESSTATUS              :1;                                /*!< Completion flag reset the root port */
  uint32_t RESUME                 :1;                                /*!< Flag reactivate the device */
  uint32_t SUSPEND                :1;                                /*!< SUSPEND mode request bit */
  uint32_t HSSETTLE               :1;                                /*!< Completion flag reset and switching devices in high-speed mode */
  uint32_t DMACMPL                :1;                                /*!< Completion flag DMA data transfer */
  uint32_t CLKUNSTBL              :1;                                /*!< Interrupt flag, signaling an unstable clock signal UTMI and to initialize the device controller registers */
} _USB_INTSTAT1_bits;

/* Bit field positions: */
#define USB_INTSTAT1_SOF_Pos                  0                      /*!< Flag SOF packet reception */
#define USB_INTSTAT1_RESSTATUS_Pos            1                      /*!< Completion flag reset the root port */
#define USB_INTSTAT1_RESUME_Pos               2                      /*!< Flag reactivate the device */
#define USB_INTSTAT1_SUSPEND_Pos              3                      /*!< SUSPEND mode request bit */
#define USB_INTSTAT1_HSSETTLE_Pos             4                      /*!< Completion flag reset and switching devices in high-speed mode */
#define USB_INTSTAT1_DMACMPL_Pos              5                      /*!< Completion flag DMA data transfer */
#define USB_INTSTAT1_CLKUNSTBL_Pos            6                      /*!< Interrupt flag, signaling an unstable clock signal UTMI and to initialize the device controller registers */

/* Bit field masks: */
#define USB_INTSTAT1_SOF_Msk                  0x00000001UL           /*!< Flag SOF packet reception */
#define USB_INTSTAT1_RESSTATUS_Msk            0x00000002UL           /*!< Completion flag reset the root port */
#define USB_INTSTAT1_RESUME_Msk               0x00000004UL           /*!< Flag reactivate the device */
#define USB_INTSTAT1_SUSPEND_Msk              0x00000008UL           /*!< SUSPEND mode request bit */
#define USB_INTSTAT1_HSSETTLE_Msk             0x00000010UL           /*!< Completion flag reset and switching devices in high-speed mode */
#define USB_INTSTAT1_DMACMPL_Msk              0x00000020UL           /*!< Completion flag DMA data transfer */
#define USB_INTSTAT1_CLKUNSTBL_Msk            0x00000040UL           /*!< Interrupt flag, signaling an unstable clock signal UTMI and to initialize the device controller registers */

/*--  INTEN1: USB Interrupt enable register -------------------------------------------------------------------*/
typedef struct {
  uint32_t SOF                    :1;                                /*!< Enable SOF packet reception */
  uint32_t RESSTATUS              :1;                                /*!< Enable completion reset the root port */
  uint32_t RESUME                 :1;                                /*!< Enable reactivate the device */
  uint32_t SUSPEND                :1;                                /*!< Enable SUSPEND mode request bit */
  uint32_t HISPEED                :1;                                /*!< Enable completion reset and switching devices in high-speed mode */
  uint32_t DMACMPL                :1;                                /*!< Enable completion DMA data transfer */
  uint32_t CLKUNSTBL              :1;                                /*!< Enable interrupt, signaling an unstable clock signal UTMI and to initialize the device controller registers */
} _USB_INTEN1_bits;

/* Bit field positions: */
#define USB_INTEN1_SOF_Pos                    0                      /*!< Enable SOF packet reception */
#define USB_INTEN1_RESSTATUS_Pos              1                      /*!< Enable completion reset the root port */
#define USB_INTEN1_RESUME_Pos                 2                      /*!< Enable reactivate the device */
#define USB_INTEN1_SUSPEND_Pos                3                      /*!< Enable SUSPEND mode request bit */
#define USB_INTEN1_HISPEED_Pos                4                      /*!< Enable completion reset and switching devices in high-speed mode */
#define USB_INTEN1_DMACMPL_Pos                5                      /*!< Enable completion DMA data transfer */
#define USB_INTEN1_CLKUNSTBL_Pos              6                      /*!< Enable interrupt, signaling an unstable clock signal UTMI and to initialize the device controller registers */

/* Bit field masks: */
#define USB_INTEN1_SOF_Msk                    0x00000001UL           /*!< Enable SOF packet reception */
#define USB_INTEN1_RESSTATUS_Msk              0x00000002UL           /*!< Enable completion reset the root port */
#define USB_INTEN1_RESUME_Msk                 0x00000004UL           /*!< Enable reactivate the device */
#define USB_INTEN1_SUSPEND_Msk                0x00000008UL           /*!< Enable SUSPEND mode request bit */
#define USB_INTEN1_HISPEED_Msk                0x00000010UL           /*!< Enable completion reset and switching devices in high-speed mode */
#define USB_INTEN1_DMACMPL_Msk                0x00000020UL           /*!< Enable completion DMA data transfer */
#define USB_INTEN1_CLKUNSTBL_Msk              0x00000040UL           /*!< Enable interrupt, signaling an unstable clock signal UTMI and to initialize the device controller registers */

/*--  OPERATIONS: USB Operations register ---------------------------------------------------------------------*/
typedef struct {
  uint32_t GEN_RESUME             :1;                                /*!< Bit start recovery work */
  uint32_t HIGHSPEED              :1;                                /*!< Trigger bit protocol 'Chirp' */
  uint32_t CURRENTSPEED           :1;                                /*!< Speed indication device controller 1-High speed; 0 - Full speed */
} _USB_OPERATIONS_bits;

/* Bit field positions: */
#define USB_OPERATIONS_GEN_RESUME_Pos         0                      /*!< Bit start recovery work */
#define USB_OPERATIONS_HIGHSPEED_Pos          1                      /*!< Trigger bit protocol 'Chirp' */
#define USB_OPERATIONS_CURRENTSPEED_Pos       2                      /*!< Speed indication device controller 1-High speed; 0 - Full speed */

/* Bit field masks: */
#define USB_OPERATIONS_GEN_RESUME_Msk         0x00000001UL           /*!< Bit start recovery work */
#define USB_OPERATIONS_HIGHSPEED_Msk          0x00000002UL           /*!< Trigger bit protocol 'Chirp' */
#define USB_OPERATIONS_CURRENTSPEED_Msk       0x00000004UL           /*!< Speed indication device controller 1-High speed; 0 - Full speed */

/*--  FRAMECNT: USB Frame counter register --------------------------------------------------------------------*/
typedef struct {
  uint32_t MICRO_FRAME_COUNTER     :3;                               /*!< Number field current a chip */
  uint32_t FRAME_COUNTER          :11;                               /*!< Field frame counter since last SOF packet */
} _USB_FRAMECNT_bits;

/* Bit field positions: */
#define USB_FRAMECNT_MICRO_FRAME_COUNTER_Pos       0                 /*!< Number field current a chip */
#define USB_FRAMECNT_FRAME_COUNTER_Pos        3                      /*!< Field frame counter since last SOF packet */

/* Bit field masks: */
#define USB_FRAMECNT_MICRO_FRAME_COUNTER_Msk       0x00000007UL         /*!< Number field current a chip */
#define USB_FRAMECNT_FRAME_COUNTER_Msk        0x00003FF8UL           /*!< Field frame counter since last SOF packet */

/*--  USBADDR: USB Address register ---------------------------------------------------------------------------*/
typedef struct {
  uint32_t USBADDR                :7;                                /*!< Field of the current address of the device */
} _USB_USBADDR_bits;

/* Bit field positions: */
#define USB_USBADDR_USBADDR_Pos               0                      /*!< Field of the current address of the device */

/* Bit field masks: */
#define USB_USBADDR_USBADDR_Msk               0x0000007FUL           /*!< Field of the current address of the device */

/*--  CEP_DATA_BUF: Data buffer for transmission register of Control EndPoint ---------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< Data buffer for transmission */
} _USB_CEP_DATA_BUF_bits;

/* Bit field positions: */
#define USB_CEP_DATA_BUF_VAL_Pos              0                      /*!< Data buffer for transmission */

/* Bit field masks: */
#define USB_CEP_DATA_BUF_VAL_Msk              0x0000FFFFUL           /*!< Data buffer for transmission */

/*--  CEP_CTRL_STAT: Control register and buffer status of Control EndPoint -----------------------------------*/
typedef struct {
  uint32_t NAKCLEAR               :1;                                /*!< Receiving flag tags SETIP */
  uint32_t STALL                  :1;                                /*!< Enable sending 'Stall' */
  uint32_t ZEROLEN                :1;                                /*!< Enable transmission zero length packet */
  uint32_t CEPFLUSH               :1;                                /*!< Reset buffer */
} _USB_CEP_CTRL_STAT_bits;

/* Bit field positions: */
#define USB_CEP_CTRL_STAT_NAKCLEAR_Pos        0                      /*!< Receiving flag tags SETIP */
#define USB_CEP_CTRL_STAT_STALL_Pos           1                      /*!< Enable sending 'Stall' */
#define USB_CEP_CTRL_STAT_ZEROLEN_Pos         2                      /*!< Enable transmission zero length packet */
#define USB_CEP_CTRL_STAT_CEPFLUSH_Pos        3                      /*!< Reset buffer */

/* Bit field masks: */
#define USB_CEP_CTRL_STAT_NAKCLEAR_Msk        0x00000001UL           /*!< Receiving flag tags SETIP */
#define USB_CEP_CTRL_STAT_STALL_Msk           0x00000002UL           /*!< Enable sending 'Stall' */
#define USB_CEP_CTRL_STAT_ZEROLEN_Msk         0x00000004UL           /*!< Enable transmission zero length packet */
#define USB_CEP_CTRL_STAT_CEPFLUSH_Msk        0x00000008UL           /*!< Reset buffer */

/*--  CEP_IRQ_ENB: Interrupt Enable buffer register of Control EndPoint ---------------------------------------*/
typedef struct {
  uint32_t SETUPTOKEN             :1;                                /*!< Enable flag deadline tags SETUP from the host */
  uint32_t SETUPPKT               :1;                                /*!< Enable flag deadline Setup package from the host */
  uint32_t OUTTOKEN               :1;                                /*!< Enable reception end flag tags OUT from the host */
  uint32_t INTOKEN                :1;                                /*!< Enable reception end flag tags IN from the host */
  uint32_t PINGTOKEN              :1;                                /*!< Enable reception end flag tags PING from host */
  uint32_t DATAPKTTR              :1;                                /*!< Enable flag successful sending of the data packet in response to the label to obtain IN ACK confirming tags */
  uint32_t DATAPKTREC             :1;                                /*!< Enable flag successful reception of the data packet following the label OUT, in response to which the mark has been sent ACK */
  uint32_t NAKSENT                :1;                                /*!< Enable flag closure parcel labels NAK in response to mark IN or OUT */
  uint32_t STALLSENT              :1;                                /*!< Enable flag closure parcel labels STALL in response to mark IN or OUT */
  uint32_t USBERR                 :1;                                /*!< Enable error flag during the operation */
  uint32_t STATCMPLN              :1;                                /*!< Enable flag successful completion stage 'Status' operations on the USB bus */
  uint32_t BUFFFULL               :1;                                /*!< Enable flag filling control buffer */
  uint32_t BUFFEMPTY              :1;                                /*!< Enable flag emptying control buffer */
} _USB_CEP_IRQ_ENB_bits;

/* Bit field positions: */
#define USB_CEP_IRQ_ENB_SETUPTOKEN_Pos        0                      /*!< Enable flag deadline tags SETUP from the host */
#define USB_CEP_IRQ_ENB_SETUPPKT_Pos          1                      /*!< Enable flag deadline Setup package from the host */
#define USB_CEP_IRQ_ENB_OUTTOKEN_Pos          2                      /*!< Enable reception end flag tags OUT from the host */
#define USB_CEP_IRQ_ENB_INTOKEN_Pos           3                      /*!< Enable reception end flag tags IN from the host */
#define USB_CEP_IRQ_ENB_PINGTOKEN_Pos         4                      /*!< Enable reception end flag tags PING from host */
#define USB_CEP_IRQ_ENB_DATAPKTTR_Pos         5                      /*!< Enable flag successful sending of the data packet in response to the label to obtain IN ACK confirming tags */
#define USB_CEP_IRQ_ENB_DATAPKTREC_Pos        6                      /*!< Enable flag successful reception of the data packet following the label OUT, in response to which the mark has been sent ACK */
#define USB_CEP_IRQ_ENB_NAKSENT_Pos           7                      /*!< Enable flag closure parcel labels NAK in response to mark IN or OUT */
#define USB_CEP_IRQ_ENB_STALLSENT_Pos         8                      /*!< Enable flag closure parcel labels STALL in response to mark IN or OUT */
#define USB_CEP_IRQ_ENB_USBERR_Pos            9                      /*!< Enable error flag during the operation */
#define USB_CEP_IRQ_ENB_STATCMPLN_Pos         10                     /*!< Enable flag successful completion stage 'Status' operations on the USB bus */
#define USB_CEP_IRQ_ENB_BUFFFULL_Pos          11                     /*!< Enable flag filling control buffer */
#define USB_CEP_IRQ_ENB_BUFFEMPTY_Pos         12                     /*!< Enable flag emptying control buffer */

/* Bit field masks: */
#define USB_CEP_IRQ_ENB_SETUPTOKEN_Msk        0x00000001UL           /*!< Enable flag deadline tags SETUP from the host */
#define USB_CEP_IRQ_ENB_SETUPPKT_Msk          0x00000002UL           /*!< Enable flag deadline Setup package from the host */
#define USB_CEP_IRQ_ENB_OUTTOKEN_Msk          0x00000004UL           /*!< Enable reception end flag tags OUT from the host */
#define USB_CEP_IRQ_ENB_INTOKEN_Msk           0x00000008UL           /*!< Enable reception end flag tags IN from the host */
#define USB_CEP_IRQ_ENB_PINGTOKEN_Msk         0x00000010UL           /*!< Enable reception end flag tags PING from host */
#define USB_CEP_IRQ_ENB_DATAPKTTR_Msk         0x00000020UL           /*!< Enable flag successful sending of the data packet in response to the label to obtain IN ACK confirming tags */
#define USB_CEP_IRQ_ENB_DATAPKTREC_Msk        0x00000040UL           /*!< Enable flag successful reception of the data packet following the label OUT, in response to which the mark has been sent ACK */
#define USB_CEP_IRQ_ENB_NAKSENT_Msk           0x00000080UL           /*!< Enable flag closure parcel labels NAK in response to mark IN or OUT */
#define USB_CEP_IRQ_ENB_STALLSENT_Msk         0x00000100UL           /*!< Enable flag closure parcel labels STALL in response to mark IN or OUT */
#define USB_CEP_IRQ_ENB_USBERR_Msk            0x00000200UL           /*!< Enable error flag during the operation */
#define USB_CEP_IRQ_ENB_STATCMPLN_Msk         0x00000400UL           /*!< Enable flag successful completion stage 'Status' operations on the USB bus */
#define USB_CEP_IRQ_ENB_BUFFFULL_Msk          0x00000800UL           /*!< Enable flag filling control buffer */
#define USB_CEP_IRQ_ENB_BUFFEMPTY_Msk         0x00001000UL           /*!< Enable flag emptying control buffer */

/*--  CEP_IRQ_STAT: Buffer interrupt flag register of Control EndPoint ----------------------------------------*/
typedef struct {
  uint32_t SETUPTOKEN             :1;                                /*!< Flag deadline tags SETUP from the host */
  uint32_t SETUPPKT               :1;                                /*!< Flag deadline Setup package from the host */
  uint32_t OUTTOKEN               :1;                                /*!< Reception end flag tags OUT from the host */
  uint32_t INTOKEN                :1;                                /*!< Reception end flag tags IN from the host */
  uint32_t PINGTOKEN              :1;                                /*!< Reception end flag tags PING from host */
  uint32_t DATAPKTTR              :1;                                /*!< Flag successful sending of the data packet in response to the label to obtain IN ACK confirming tags */
  uint32_t DATAPKTREC             :1;                                /*!< Flag successful reception of the data packet following the label OUT, in response to which the mark has been sent ACK */
  uint32_t NAKSENT                :1;                                /*!< Flag closure parcel labels NAK in response to mark IN or OUT */
  uint32_t STALLSENT              :1;                                /*!< Flag closure parcel labels STALL in response to mark IN or OUT */
  uint32_t USBERR                 :1;                                /*!< The error flag during the operation */
  uint32_t STATCMPLN              :1;                                /*!< Flag successful completion stage 'Status' operations on the USB bus */
  uint32_t BUFFFULL               :1;                                /*!< Flag filling control buffer */
  uint32_t BUFFEMPTY              :1;                                /*!< Flag control buffer emptying */
} _USB_CEP_IRQ_STAT_bits;

/* Bit field positions: */
#define USB_CEP_IRQ_STAT_SETUPTOKEN_Pos       0                      /*!< Flag deadline tags SETUP from the host */
#define USB_CEP_IRQ_STAT_SETUPPKT_Pos         1                      /*!< Flag deadline Setup package from the host */
#define USB_CEP_IRQ_STAT_OUTTOKEN_Pos         2                      /*!< Reception end flag tags OUT from the host */
#define USB_CEP_IRQ_STAT_INTOKEN_Pos          3                      /*!< Reception end flag tags IN from the host */
#define USB_CEP_IRQ_STAT_PINGTOKEN_Pos        4                      /*!< Reception end flag tags PING from host */
#define USB_CEP_IRQ_STAT_DATAPKTTR_Pos        5                      /*!< Flag successful sending of the data packet in response to the label to obtain IN ACK confirming tags */
#define USB_CEP_IRQ_STAT_DATAPKTREC_Pos       6                      /*!< Flag successful reception of the data packet following the label OUT, in response to which the mark has been sent ACK */
#define USB_CEP_IRQ_STAT_NAKSENT_Pos          7                      /*!< Flag closure parcel labels NAK in response to mark IN or OUT */
#define USB_CEP_IRQ_STAT_STALLSENT_Pos        8                      /*!< Flag closure parcel labels STALL in response to mark IN or OUT */
#define USB_CEP_IRQ_STAT_USBERR_Pos           9                      /*!< The error flag during the operation */
#define USB_CEP_IRQ_STAT_STATCMPLN_Pos        10                     /*!< Flag successful completion stage 'Status' operations on the USB bus */
#define USB_CEP_IRQ_STAT_BUFFFULL_Pos         11                     /*!< Flag filling control buffer */
#define USB_CEP_IRQ_STAT_BUFFEMPTY_Pos        12                     /*!< Flag control buffer emptying */

/* Bit field masks: */
#define USB_CEP_IRQ_STAT_SETUPTOKEN_Msk       0x00000001UL           /*!< Flag deadline tags SETUP from the host */
#define USB_CEP_IRQ_STAT_SETUPPKT_Msk         0x00000002UL           /*!< Flag deadline Setup package from the host */
#define USB_CEP_IRQ_STAT_OUTTOKEN_Msk         0x00000004UL           /*!< Reception end flag tags OUT from the host */
#define USB_CEP_IRQ_STAT_INTOKEN_Msk          0x00000008UL           /*!< Reception end flag tags IN from the host */
#define USB_CEP_IRQ_STAT_PINGTOKEN_Msk        0x00000010UL           /*!< Reception end flag tags PING from host */
#define USB_CEP_IRQ_STAT_DATAPKTTR_Msk        0x00000020UL           /*!< Flag successful sending of the data packet in response to the label to obtain IN ACK confirming tags */
#define USB_CEP_IRQ_STAT_DATAPKTREC_Msk       0x00000040UL           /*!< Flag successful reception of the data packet following the label OUT, in response to which the mark has been sent ACK */
#define USB_CEP_IRQ_STAT_NAKSENT_Msk          0x00000080UL           /*!< Flag closure parcel labels NAK in response to mark IN or OUT */
#define USB_CEP_IRQ_STAT_STALLSENT_Msk        0x00000100UL           /*!< Flag closure parcel labels STALL in response to mark IN or OUT */
#define USB_CEP_IRQ_STAT_USBERR_Msk           0x00000200UL           /*!< The error flag during the operation */
#define USB_CEP_IRQ_STAT_STATCMPLN_Msk        0x00000400UL           /*!< Flag successful completion stage 'Status' operations on the USB bus */
#define USB_CEP_IRQ_STAT_BUFFFULL_Msk         0x00000800UL           /*!< Flag filling control buffer */
#define USB_CEP_IRQ_STAT_BUFFEMPTY_Msk        0x00001000UL           /*!< Flag control buffer emptying */

/*--  CEP_IN_XFRCNT: In transfer data count register  of Control EndPoint -------------------------------------*/
typedef struct {
  uint32_t DATACOUNT              :8;                                /*!< The number of bytes to be sent in response to a label IN */
} _USB_CEP_IN_XFRCNT_bits;

/* Bit field positions: */
#define USB_CEP_IN_XFRCNT_DATACOUNT_Pos       0                      /*!< The number of bytes to be sent in response to a label IN */

/* Bit field masks: */
#define USB_CEP_IN_XFRCNT_DATACOUNT_Msk       0x000000FFUL           /*!< The number of bytes to be sent in response to a label IN */

/*--  CEP_OUT_XFRCNT: Out-transfer data count register of Control EndPoint ------------------------------------*/
typedef struct {
  uint32_t DATACOUNT              :8;                                /*!< The number of bytes received of data */
} _USB_CEP_OUT_XFRCNT_bits;

/* Bit field positions: */
#define USB_CEP_OUT_XFRCNT_DATACOUNT_Pos       0                     /*!< The number of bytes received of data */

/* Bit field masks: */
#define USB_CEP_OUT_XFRCNT_DATACOUNT_Msk       0x000000FFUL          /*!< The number of bytes received of data */

/*--  CEP_DATA_AVL: Register number of bytes received ---------------------------------------------------------*/
typedef struct {
  uint32_t NUM_BYTES              :16;                               /*!< The number of bytes received of data */
} _USB_CEP_DATA_AVL_bits;

/* Bit field positions: */
#define USB_CEP_DATA_AVL_NUM_BYTES_Pos        0                      /*!< The number of bytes received of data */

/* Bit field masks: */
#define USB_CEP_DATA_AVL_NUM_BYTES_Msk        0x0000FFFFUL           /*!< The number of bytes received of data */

/*--  CEP_SETUP1_0: Register zero and first byte packet Setup  of Control EndPoint --------------------------------*/
typedef struct {
  uint32_t BYTE0                  :8;                                /*!< destination */
  uint32_t BYTE1                  :8;                                /*!< The first byte of the packet Setup */
} _USB_CEP_SETUP1_0_bits;

/* Bit field positions: */
#define USB_CEP_SETUP1_0_BYTE0_Pos            0                      /*!< destination */
#define USB_CEP_SETUP1_0_BYTE1_Pos            8                      /*!< The first byte of the packet Setup */

/* Bit field masks: */
#define USB_CEP_SETUP1_0_BYTE0_Msk            0x000000FFUL           /*!< destination */
#define USB_CEP_SETUP1_0_BYTE1_Msk            0x0000FF00UL           /*!< The first byte of the packet Setup */

/*--  CEP_SETUP3_2: Register 2nd and 3rd bytes of the packet Setup  of Control EndPoint --------------------------------*/
typedef struct {
  uint32_t BYTE2                  :8;                                /*!< Low byte field wValue */
  uint32_t BYTE3                  :8;                                /*!< High byte field wValue */
} _USB_CEP_SETUP3_2_bits;

/* Bit field positions: */
#define USB_CEP_SETUP3_2_BYTE2_Pos            0                      /*!< Low byte field wValue */
#define USB_CEP_SETUP3_2_BYTE3_Pos            8                      /*!< High byte field wValue */

/* Bit field masks: */
#define USB_CEP_SETUP3_2_BYTE2_Msk            0x000000FFUL           /*!< Low byte field wValue */
#define USB_CEP_SETUP3_2_BYTE3_Msk            0x0000FF00UL           /*!< High byte field wValue */

/*--  CEP_SETUP5_4: Register 4th and 5th bytes of the packet Setup  of Control EndPoint --------------------------------*/
typedef struct {
  uint32_t BYTE4                  :8;                                /*!< Low byte field windex */
  uint32_t BYTE5                  :8;                                /*!< High byte field windex */
} _USB_CEP_SETUP5_4_bits;

/* Bit field positions: */
#define USB_CEP_SETUP5_4_BYTE4_Pos            0                      /*!< Low byte field windex */
#define USB_CEP_SETUP5_4_BYTE5_Pos            8                      /*!< High byte field windex */

/* Bit field masks: */
#define USB_CEP_SETUP5_4_BYTE4_Msk            0x000000FFUL           /*!< Low byte field windex */
#define USB_CEP_SETUP5_4_BYTE5_Msk            0x0000FF00UL           /*!< High byte field windex */

/*--  CEP_SETUP7_6: Register on the 6th and 7th byte packet Setup of Control EndPoint --------------------------------*/
typedef struct {
  uint32_t BYTE6                  :8;                                /*!< Low byte field wLength */
  uint32_t BYTE7                  :8;                                /*!< High byte field wLength */
} _USB_CEP_SETUP7_6_bits;

/* Bit field positions: */
#define USB_CEP_SETUP7_6_BYTE6_Pos            0                      /*!< Low byte field wLength */
#define USB_CEP_SETUP7_6_BYTE7_Pos            8                      /*!< High byte field wLength */

/* Bit field masks: */
#define USB_CEP_SETUP7_6_BYTE6_Msk            0x000000FFUL           /*!< Low byte field wLength */
#define USB_CEP_SETUP7_6_BYTE7_Msk            0x0000FF00UL           /*!< High byte field wLength */

/*--  CEP_START_ADDR: Control EndPoint RAM start Addr register ------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< Beginning of the address space */
} _USB_CEP_START_ADDR_bits;

/* Bit field positions: */
#define USB_CEP_START_ADDR_VAL_Pos            0                      /*!< Beginning of the address space */

/* Bit field masks: */
#define USB_CEP_START_ADDR_VAL_Msk            0x0000FFFFUL           /*!< Beginning of the address space */

/*--  CEP_END_ADDR: Control EndPoint RAM end Addr register ----------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!<  The end of the address space */
} _USB_CEP_END_ADDR_bits;

/* Bit field positions: */
#define USB_CEP_END_ADDR_VAL_Pos              0                      /*!<  The end of the address space */

/* Bit field masks: */
#define USB_CEP_END_ADDR_VAL_Msk              0x0000FFFFUL           /*!<  The end of the address space */

/*--  DMA_CTRL_STS: DMA Control Register ----------------------------------------------------------------------*/
typedef struct {
  uint32_t DMA_EP_ADDR            :4;                                /*!< Buffer address accessed by the DMA */
  uint32_t DMA_RW                 :1;                                /*!< Type DMA operations 1-Read, 0-Write */
  uint32_t DMA_EN                 :1;                                /*!< Enable DMA */
} _USB_DMA_CTRL_STS_bits;

/* Bit field positions: */
#define USB_DMA_CTRL_STS_DMA_EP_ADDR_Pos       0                     /*!< Buffer address accessed by the DMA */
#define USB_DMA_CTRL_STS_DMA_RW_Pos           4                      /*!< Type DMA operations 1-Read, 0-Write */
#define USB_DMA_CTRL_STS_DMA_EN_Pos           5                      /*!< Enable DMA */

/* Bit field masks: */
#define USB_DMA_CTRL_STS_DMA_EP_ADDR_Msk       0x0000000FUL          /*!< Buffer address accessed by the DMA */
#define USB_DMA_CTRL_STS_DMA_RW_Msk           0x00000010UL           /*!< Type DMA operations 1-Read, 0-Write */
#define USB_DMA_CTRL_STS_DMA_EN_Msk           0x00000020UL           /*!< Enable DMA */

/*--  DMA_CNT: DMA Count byte Register ------------------------------------------------------------------------*/
typedef struct {
  uint32_t DMA_CNT                :16;                               /*!< The number of words transmitted by DMA */
} _USB_DMA_CNT_bits;

/* Bit field positions: */
#define USB_DMA_CNT_DMA_CNT_Pos               0                      /*!< The number of words transmitted by DMA */

/* Bit field masks: */
#define USB_DMA_CNT_DMA_CNT_Msk               0x0000FFFFUL           /*!< The number of words transmitted by DMA */

/*-- USB_EP: DATA_BUF: EndPoint Data Register -----------------------------------------------------------------*/
typedef struct {
  uint32_t DATA                   :16;                               /*!< Data exchange with the buffer */
} _USB_USB_EP_DATA_BUF_bits;

/* Bit field positions: */
#define USB_USB_EP_DATA_BUF_DATA_Pos          0                      /*!< Data exchange with the buffer */

/* Bit field masks: */
#define USB_USB_EP_DATA_BUF_DATA_Msk          0x0000FFFFUL           /*!< Data exchange with the buffer */

/*-- USB_EP: IRQ_STAT: Interrupt flag register no control buffer ----------------------------------------------*/
typedef struct {
  uint32_t BUFFFULLINT            :1;                                /*!< Flag buffer fill */
  uint32_t BUFFEMPTYINT           :1;                                /*!< Buffer empty flag */
  uint32_t SHORTPKTTRINT          :1;                                /*!< Flag size of the last packet */
  uint32_t DATAPKTTRINT           :1;                                /*!< Flag to send a data packet from the buffer to the host */
  uint32_t DATAPKTRECINT          :1;                                /*!< Flag successful reception of a data packet from a host */
  uint32_t OUTTOKENINT            :1;                                /*!< Receiving flag tags OUT from the host */
  uint32_t INTOKENINT             :1;                                /*!< Receiving flag tags IN from the host */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t NACKSENTINT            :1;                                /*!< Flag unsent last packet */
  uint32_t STALLSENTINT           :1;                                /*!< Flag Uncommitted last packet */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t ERRSENTINT             :1;                                /*!< Flag of any error during the operation */
} _USB_USB_EP_IRQ_STAT_bits;

/* Bit field positions: */
#define USB_USB_EP_IRQ_STAT_BUFFFULLINT_Pos       0                  /*!< Flag buffer fill */
#define USB_USB_EP_IRQ_STAT_BUFFEMPTYINT_Pos       1                 /*!< Buffer empty flag */
#define USB_USB_EP_IRQ_STAT_SHORTPKTTRINT_Pos       2                /*!< Flag size of the last packet */
#define USB_USB_EP_IRQ_STAT_DATAPKTTRINT_Pos       3                 /*!< Flag to send a data packet from the buffer to the host */
#define USB_USB_EP_IRQ_STAT_DATAPKTRECINT_Pos       4                /*!< Flag successful reception of a data packet from a host */
#define USB_USB_EP_IRQ_STAT_OUTTOKENINT_Pos       5                  /*!< Receiving flag tags OUT from the host */
#define USB_USB_EP_IRQ_STAT_INTOKENINT_Pos       6                   /*!< Receiving flag tags IN from the host */
#define USB_USB_EP_IRQ_STAT_NACKSENTINT_Pos       8                  /*!< Flag unsent last packet */
#define USB_USB_EP_IRQ_STAT_STALLSENTINT_Pos       9                 /*!< Flag Uncommitted last packet */
#define USB_USB_EP_IRQ_STAT_ERRSENTINT_Pos       11                  /*!< Flag of any error during the operation */

/* Bit field masks: */
#define USB_USB_EP_IRQ_STAT_BUFFFULLINT_Msk       0x00000001UL         /*!< Flag buffer fill */
#define USB_USB_EP_IRQ_STAT_BUFFEMPTYINT_Msk       0x00000002UL         /*!< Buffer empty flag */
#define USB_USB_EP_IRQ_STAT_SHORTPKTTRINT_Msk       0x00000004UL         /*!< Flag size of the last packet */
#define USB_USB_EP_IRQ_STAT_DATAPKTTRINT_Msk       0x00000008UL         /*!< Flag to send a data packet from the buffer to the host */
#define USB_USB_EP_IRQ_STAT_DATAPKTRECINT_Msk       0x00000010UL         /*!< Flag successful reception of a data packet from a host */
#define USB_USB_EP_IRQ_STAT_OUTTOKENINT_Msk       0x00000020UL         /*!< Receiving flag tags OUT from the host */
#define USB_USB_EP_IRQ_STAT_INTOKENINT_Msk       0x00000040UL         /*!< Receiving flag tags IN from the host */
#define USB_USB_EP_IRQ_STAT_NACKSENTINT_Msk       0x00000100UL         /*!< Flag unsent last packet */
#define USB_USB_EP_IRQ_STAT_STALLSENTINT_Msk       0x00000200UL         /*!< Flag Uncommitted last packet */
#define USB_USB_EP_IRQ_STAT_ERRSENTINT_Msk       0x00000800UL         /*!< Flag of any error during the operation */

/*-- USB_EP: IRQ_ENB: Enable register Interrupt no control buffer ---------------------------------------------*/
typedef struct {
  uint32_t BUFFFULLINTEN          :1;                                /*!< Enable flag buffer fill */
  uint32_t BUFFEMPTYINTEN         :1;                                /*!< Enable buffer empty flag */
  uint32_t SHORTPKTTRINTEN        :1;                                /*!< Enable flag size of the last packet */
  uint32_t DATAPKTTRINTEN         :1;                                /*!< Enable flag to send a data packet from the buffer to the host */
  uint32_t DATAPKTRECINTEN        :1;                                /*!< Enable flag successful reception of a data packet from a host */
  uint32_t OUTTOKENINTEN          :1;                                /*!< Enable receiving flag tags OUT from the host */
  uint32_t INTOKENINTEN           :1;                                /*!< Enable receiving flag tags IN from the host */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t NACKSENTINTEN          :1;                                /*!< Enable flag unsent last packet */
  uint32_t STALLSENTINTEN         :1;                                /*!< Enable flag Uncommitted last packet */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t ERRSENTINTEN           :1;                                /*!< Enable flag of any error during the operation */
} _USB_USB_EP_IRQ_ENB_bits;

/* Bit field positions: */
#define USB_USB_EP_IRQ_ENB_BUFFFULLINTEN_Pos       0                 /*!< Enable flag buffer fill */
#define USB_USB_EP_IRQ_ENB_BUFFEMPTYINTEN_Pos       1                /*!< Enable buffer empty flag */
#define USB_USB_EP_IRQ_ENB_SHORTPKTTRINTEN_Pos       2               /*!< Enable flag size of the last packet */
#define USB_USB_EP_IRQ_ENB_DATAPKTTRINTEN_Pos       3                /*!< Enable flag to send a data packet from the buffer to the host */
#define USB_USB_EP_IRQ_ENB_DATAPKTRECINTEN_Pos       4               /*!< Enable flag successful reception of a data packet from a host */
#define USB_USB_EP_IRQ_ENB_OUTTOKENINTEN_Pos       5                 /*!< Enable receiving flag tags OUT from the host */
#define USB_USB_EP_IRQ_ENB_INTOKENINTEN_Pos       6                  /*!< Enable receiving flag tags IN from the host */
#define USB_USB_EP_IRQ_ENB_NACKSENTINTEN_Pos       8                 /*!< Enable flag unsent last packet */
#define USB_USB_EP_IRQ_ENB_STALLSENTINTEN_Pos       9                /*!< Enable flag Uncommitted last packet */
#define USB_USB_EP_IRQ_ENB_ERRSENTINTEN_Pos       11                 /*!< Enable flag of any error during the operation */

/* Bit field masks: */
#define USB_USB_EP_IRQ_ENB_BUFFFULLINTEN_Msk       0x00000001UL         /*!< Enable flag buffer fill */
#define USB_USB_EP_IRQ_ENB_BUFFEMPTYINTEN_Msk       0x00000002UL         /*!< Enable buffer empty flag */
#define USB_USB_EP_IRQ_ENB_SHORTPKTTRINTEN_Msk       0x00000004UL         /*!< Enable flag size of the last packet */
#define USB_USB_EP_IRQ_ENB_DATAPKTTRINTEN_Msk       0x00000008UL         /*!< Enable flag to send a data packet from the buffer to the host */
#define USB_USB_EP_IRQ_ENB_DATAPKTRECINTEN_Msk       0x00000010UL         /*!< Enable flag successful reception of a data packet from a host */
#define USB_USB_EP_IRQ_ENB_OUTTOKENINTEN_Msk       0x00000020UL         /*!< Enable receiving flag tags OUT from the host */
#define USB_USB_EP_IRQ_ENB_INTOKENINTEN_Msk       0x00000040UL         /*!< Enable receiving flag tags IN from the host */
#define USB_USB_EP_IRQ_ENB_NACKSENTINTEN_Msk       0x00000100UL         /*!< Enable flag unsent last packet */
#define USB_USB_EP_IRQ_ENB_STALLSENTINTEN_Msk       0x00000200UL         /*!< Enable flag Uncommitted last packet */
#define USB_USB_EP_IRQ_ENB_ERRSENTINTEN_Msk       0x00000800UL         /*!< Enable flag of any error during the operation */

/*-- USB_EP: AVAIL_CNT: EndPoint Available count register -----------------------------------------------------*/
typedef struct {
  uint32_t BUFFBYTENUM            :16;                               /*!< Number of bytes in the buffer */
} _USB_USB_EP_AVAIL_CNT_bits;

/* Bit field positions: */
#define USB_USB_EP_AVAIL_CNT_BUFFBYTENUM_Pos       0                 /*!< Number of bytes in the buffer */

/* Bit field masks: */
#define USB_USB_EP_AVAIL_CNT_BUFFBYTENUM_Msk       0x0000FFFFUL         /*!< Number of bytes in the buffer */

/*-- USB_EP: RSP_SC: EndPoint Response Set/Clear register -----------------------------------------------------*/
typedef struct {
  uint32_t BUFFFLUSH              :1;                                /*!< Reset bit in the buffer and register EP_AVAIL_CNT */
  uint32_t MODE                   :2;                                /*!< Field mode of operation of the buffer type ID */
  uint32_t EPTOGGL                :1;                                /*!< Control bit by bit DATATOGGLE */
  uint32_t EPHALT                 :1;                                /*!< Select bit answer to any label from the host */
  uint32_t ZEROLENIN              :1;                                /*!< Select bit answer to mark IN */
  uint32_t PKTEND                 :1;                                /*!< Indicator number of bytes */
  uint32_t EPNAK                  :1;                                /*!< Select bit for NAK operation */
} _USB_USB_EP_RSP_SC_bits;

/* Bit field positions: */
#define USB_USB_EP_RSP_SC_BUFFFLUSH_Pos       0                      /*!< Reset bit in the buffer and register EP_AVAIL_CNT */
#define USB_USB_EP_RSP_SC_MODE_Pos            1                      /*!< Field mode of operation of the buffer type ID */
#define USB_USB_EP_RSP_SC_EPTOGGL_Pos         3                      /*!< Control bit by bit DATATOGGLE */
#define USB_USB_EP_RSP_SC_EPHALT_Pos          4                      /*!< Select bit answer to any label from the host */
#define USB_USB_EP_RSP_SC_ZEROLENIN_Pos       5                      /*!< Select bit answer to mark IN */
#define USB_USB_EP_RSP_SC_PKTEND_Pos          6                      /*!< Indicator number of bytes */
#define USB_USB_EP_RSP_SC_EPNAK_Pos           7                      /*!< Select bit for NAK operation */

/* Bit field masks: */
#define USB_USB_EP_RSP_SC_BUFFFLUSH_Msk       0x00000001UL           /*!< Reset bit in the buffer and register EP_AVAIL_CNT */
#define USB_USB_EP_RSP_SC_MODE_Msk            0x00000006UL           /*!< Field mode of operation of the buffer type ID */
#define USB_USB_EP_RSP_SC_EPTOGGL_Msk         0x00000008UL           /*!< Control bit by bit DATATOGGLE */
#define USB_USB_EP_RSP_SC_EPHALT_Msk          0x00000010UL           /*!< Select bit answer to any label from the host */
#define USB_USB_EP_RSP_SC_ZEROLENIN_Msk       0x00000020UL           /*!< Select bit answer to mark IN */
#define USB_USB_EP_RSP_SC_PKTEND_Msk          0x00000040UL           /*!< Indicator number of bytes */
#define USB_USB_EP_RSP_SC_EPNAK_Msk           0x00000080UL           /*!< Select bit for NAK operation */

/* Bit field enums: */
typedef enum {
  USB_USB_EP_RSP_SC_MODE_Auto                = 0x0UL,                /*!< Auto-Validate mode */
  USB_USB_EP_RSP_SC_MODE_Manual              = 0x1UL,                /*!< Manual-Validate mode */
  USB_USB_EP_RSP_SC_MODE_Fly                 = 0x2UL,                /*!< Fly mode */
} USB_USB_EP_RSP_SC_MODE_Enum;

/*-- USB_EP: MPS: EndPoint maximum packet size register -------------------------------------------------------*/
typedef struct {
  uint32_t MAXPKTSIZE             :11;                               /*!< Maximum packet size */
} _USB_USB_EP_MPS_bits;

/* Bit field positions: */
#define USB_USB_EP_MPS_MAXPKTSIZE_Pos         0                      /*!< Maximum packet size */

/* Bit field masks: */
#define USB_USB_EP_MPS_MAXPKTSIZE_Msk         0x000007FFUL           /*!< Maximum packet size */

/*-- USB_EP: CNT: EndPoint Transfer count register ------------------------------------------------------------*/
typedef struct {
  uint32_t EP_TFR_CNT             :11;                               /*!< Field number of bytes to send to the host mode Manual-Validate */
} _USB_USB_EP_CNT_bits;

/* Bit field positions: */
#define USB_USB_EP_CNT_EP_TFR_CNT_Pos         0                      /*!< Field number of bytes to send to the host mode Manual-Validate */

/* Bit field masks: */
#define USB_USB_EP_CNT_EP_TFR_CNT_Msk         0x000007FFUL           /*!< Field number of bytes to send to the host mode Manual-Validate */

/*-- USB_EP: USB_EP_CFG: EndPoint configuration register ------------------------------------------------------*/
typedef struct {
  uint32_t EP_VALID               :1;                                /*!< Resolution of buffer */
  uint32_t EP_TYPE                :2;                                /*!< Buffer type */
  uint32_t EP_DIR                 :1;                                /*!< Field of type (direction) buffer */
  uint32_t EP_NUM                 :4;                                /*!< Number field buffer */
  uint32_t MULT                   :2;                                /*!< Field number of operations performed in one micro frame */
} _USB_USB_EP_USB_EP_CFG_bits;

/* Bit field positions: */
#define USB_USB_EP_USB_EP_CFG_EP_VALID_Pos       0                   /*!< Resolution of buffer */
#define USB_USB_EP_USB_EP_CFG_EP_TYPE_Pos       1                    /*!< Buffer type */
#define USB_USB_EP_USB_EP_CFG_EP_DIR_Pos       3                     /*!< Field of type (direction) buffer */
#define USB_USB_EP_USB_EP_CFG_EP_NUM_Pos       4                     /*!< Number field buffer */
#define USB_USB_EP_USB_EP_CFG_MULT_Pos        8                      /*!< Field number of operations performed in one micro frame */

/* Bit field masks: */
#define USB_USB_EP_USB_EP_CFG_EP_VALID_Msk       0x00000001UL         /*!< Resolution of buffer */
#define USB_USB_EP_USB_EP_CFG_EP_TYPE_Msk       0x00000006UL         /*!< Buffer type */
#define USB_USB_EP_USB_EP_CFG_EP_DIR_Msk       0x00000008UL          /*!< Field of type (direction) buffer */
#define USB_USB_EP_USB_EP_CFG_EP_NUM_Msk       0x000000F0UL          /*!< Number field buffer */
#define USB_USB_EP_USB_EP_CFG_MULT_Msk        0x00000300UL           /*!< Field number of operations performed in one micro frame */

/* Bit field enums: */
typedef enum {
  USB_USB_EP_USB_EP_CFG_EP_TYPE_Bulk         = 0x1UL,                /*!< Bulk */
  USB_USB_EP_USB_EP_CFG_EP_TYPE_Int          = 0x2UL,                /*!< Interrupt */
  USB_USB_EP_USB_EP_CFG_EP_TYPE_Isochron     = 0x3UL,                /*!< Isochronous */
} USB_USB_EP_USB_EP_CFG_EP_TYPE_Enum;

typedef enum {
  USB_USB_EP_USB_EP_CFG_EP_DIR_OUT           = 0x0UL,                /*!< Out */
  USB_USB_EP_USB_EP_CFG_EP_DIR_IN            = 0x1UL,                /*!< In */
} USB_USB_EP_USB_EP_CFG_EP_DIR_Enum;

typedef enum {
  USB_USB_EP_USB_EP_CFG_MULT_1               = 0x0UL,                /*!< 1 operation */
  USB_USB_EP_USB_EP_CFG_MULT_2               = 0x1UL,                /*!< 2 operation */
  USB_USB_EP_USB_EP_CFG_MULT_3               = 0x2UL,                /*!< 3 operation */
} USB_USB_EP_USB_EP_CFG_MULT_Enum;

/*-- USB_EP: START_ADDR: EndPoint RAM start addres register ---------------------------------------------------*/
typedef struct {
  uint32_t STARTADDR              :10;                               /*!< Start address space allocated for the buffer */
} _USB_USB_EP_START_ADDR_bits;

/* Bit field positions: */
#define USB_USB_EP_START_ADDR_STARTADDR_Pos       0                  /*!< Start address space allocated for the buffer */

/* Bit field masks: */
#define USB_USB_EP_START_ADDR_STARTADDR_Msk       0x000003FFUL         /*!< Start address space allocated for the buffer */

/*-- USB_EP: END_ADDR: EndPoint RAM end addres register -------------------------------------------------------*/
typedef struct {
  uint32_t ENDADDR                :10;                               /*!< The end of address space allocated for the buffer */
} _USB_USB_EP_END_ADDR_bits;

/* Bit field positions: */
#define USB_USB_EP_END_ADDR_ENDADDR_Pos       0                      /*!< The end of address space allocated for the buffer */

/* Bit field masks: */
#define USB_USB_EP_END_ADDR_ENDADDR_Msk       0x000003FFUL           /*!< The end of address space allocated for the buffer */

/*--  AHB_DMA_ADDR: AHB addr register -------------------------------------------------------------------------*/
typedef struct {
  uint32_t AHBADDR                :32;                               /*!< It specifies the address from which the DMA has to read / write.The address must WORD (32- bits) aligned */
} _USB_AHB_DMA_ADDR_bits;

/* Bit field positions: */
#define USB_AHB_DMA_ADDR_AHBADDR_Pos          0                      /*!< It specifies the address from which the DMA has to read / write.The address must WORD (32- bits) aligned */

/* Bit field masks: */
#define USB_AHB_DMA_ADDR_AHBADDR_Msk          0xFFFFFFFFUL           /*!< It specifies the address from which the DMA has to read / write.The address must WORD (32- bits) aligned */

/*--  PHY_PD: USB PHY PowerDown Register ----------------------------------------------------------------------*/
typedef struct {
  uint32_t CMN                    :1;                                /*!< Enable powerdown function of common PHY logic */
  uint32_t RX                     :1;                                /*!< Enable Powerdown of RX part */
  uint32_t TX                     :1;                                /*!< Enable Powerdown of TX part */
} _USB_PHY_PD_bits;

/* Bit field positions: */
#define USB_PHY_PD_CMN_Pos                    0                      /*!< Enable powerdown function of common PHY logic */
#define USB_PHY_PD_RX_Pos                     1                      /*!< Enable Powerdown of RX part */
#define USB_PHY_PD_TX_Pos                     2                      /*!< Enable Powerdown of TX part */

/* Bit field masks: */
#define USB_PHY_PD_CMN_Msk                    0x00000001UL           /*!< Enable powerdown function of common PHY logic */
#define USB_PHY_PD_RX_Msk                     0x00000002UL           /*!< Enable Powerdown of RX part */
#define USB_PHY_PD_TX_Msk                     0x00000004UL           /*!< Enable Powerdown of TX part */

/*--  PLLUSBCFG0: PLL configudration 0 register ---------------------------------------------------------------*/
typedef struct {
  uint32_t PLLEN                  :1;                                /*!< PLL enable */
  uint32_t BYP                    :2;                                /*!< Bypass */
  uint32_t DACEN                  :1;                                /*!<  */
  uint32_t DSMEN                  :1;                                /*!<  */
  uint32_t FOUTEN                 :2;                                /*!< Output frequency enabled */
  uint32_t REFDIV                 :6;                                /*!<  */
  uint32_t PD0A                   :3;                                /*!<  */
  uint32_t PD0B                   :6;                                /*!<  */
  uint32_t PD1A                   :3;                                /*!<  */
  uint32_t PD1B                   :6;                                /*!<  */
} _USB_PLLUSBCFG0_bits;

/* Bit field positions: */
#define USB_PLLUSBCFG0_PLLEN_Pos              0                      /*!< PLL enable */
#define USB_PLLUSBCFG0_BYP_Pos                1                      /*!< Bypass */
#define USB_PLLUSBCFG0_DACEN_Pos              3                      /*!<  */
#define USB_PLLUSBCFG0_DSMEN_Pos              4                      /*!<  */
#define USB_PLLUSBCFG0_FOUTEN_Pos             5                      /*!< Output frequency enabled */
#define USB_PLLUSBCFG0_REFDIV_Pos             7                      /*!<  */
#define USB_PLLUSBCFG0_PD0A_Pos               13                     /*!<  */
#define USB_PLLUSBCFG0_PD0B_Pos               16                     /*!<  */
#define USB_PLLUSBCFG0_PD1A_Pos               22                     /*!<  */
#define USB_PLLUSBCFG0_PD1B_Pos               25                     /*!<  */

/* Bit field masks: */
#define USB_PLLUSBCFG0_PLLEN_Msk              0x00000001UL           /*!< PLL enable */
#define USB_PLLUSBCFG0_BYP_Msk                0x00000006UL           /*!< Bypass */
#define USB_PLLUSBCFG0_DACEN_Msk              0x00000008UL           /*!<  */
#define USB_PLLUSBCFG0_DSMEN_Msk              0x00000010UL           /*!<  */
#define USB_PLLUSBCFG0_FOUTEN_Msk             0x00000060UL           /*!< Output frequency enabled */
#define USB_PLLUSBCFG0_REFDIV_Msk             0x00001F80UL           /*!<  */
#define USB_PLLUSBCFG0_PD0A_Msk               0x0000E000UL           /*!<  */
#define USB_PLLUSBCFG0_PD0B_Msk               0x003F0000UL           /*!<  */
#define USB_PLLUSBCFG0_PD1A_Msk               0x01C00000UL           /*!<  */
#define USB_PLLUSBCFG0_PD1B_Msk               0x7E000000UL           /*!<  */

/*--  PLLUSBCFG1: PLL configudration 1 register ---------------------------------------------------------------*/
typedef struct {
  uint32_t FRAC                   :24;                               /*!<  */
} _USB_PLLUSBCFG1_bits;

/* Bit field positions: */
#define USB_PLLUSBCFG1_FRAC_Pos               0                      /*!<  */

/* Bit field masks: */
#define USB_PLLUSBCFG1_FRAC_Msk               0x00FFFFFFUL           /*!<  */

/*--  PLLUSBCFG2: PLL configudration 2 register ---------------------------------------------------------------*/
typedef struct {
  uint32_t FBDIV                  :12;                               /*!<  */
} _USB_PLLUSBCFG2_bits;

/* Bit field positions: */
#define USB_PLLUSBCFG2_FBDIV_Pos              0                      /*!<  */

/* Bit field masks: */
#define USB_PLLUSBCFG2_FBDIV_Msk              0x00000FFFUL           /*!<  */

/*--  PLLUSBCFG3: PLL configudration 3 register ---------------------------------------------------------------*/
typedef struct {
  uint32_t DSKEWEN                :1;                                /*!<  */
  uint32_t DSKEWCALBYP            :1;                                /*!<  */
  uint32_t DSKEWCALCNT            :3;                                /*!<  */
  uint32_t DSKEWCALEN             :1;                                /*!<  */
  uint32_t DSKEWFASTCAL           :1;                                /*!<  */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t DSKEWCALIN             :12;                               /*!<  */
  uint32_t                        :4;                                /*!< RESERVED */
  uint32_t USBCLKSEL              :1;                                /*!< 0-PLLUSBClk (FOUT0); 1- SYSClk */
} _USB_PLLUSBCFG3_bits;

/* Bit field positions: */
#define USB_PLLUSBCFG3_DSKEWEN_Pos            0                      /*!<  */
#define USB_PLLUSBCFG3_DSKEWCALBYP_Pos        1                      /*!<  */
#define USB_PLLUSBCFG3_DSKEWCALCNT_Pos        2                      /*!<  */
#define USB_PLLUSBCFG3_DSKEWCALEN_Pos         5                      /*!<  */
#define USB_PLLUSBCFG3_DSKEWFASTCAL_Pos       6                      /*!<  */
#define USB_PLLUSBCFG3_DSKEWCALIN_Pos         8                      /*!<  */
#define USB_PLLUSBCFG3_USBCLKSEL_Pos          24                     /*!< 0-PLLUSBClk (FOUT0); 1- SYSClk */

/* Bit field masks: */
#define USB_PLLUSBCFG3_DSKEWEN_Msk            0x00000001UL           /*!<  */
#define USB_PLLUSBCFG3_DSKEWCALBYP_Msk        0x00000002UL           /*!<  */
#define USB_PLLUSBCFG3_DSKEWCALCNT_Msk        0x0000001CUL           /*!<  */
#define USB_PLLUSBCFG3_DSKEWCALEN_Msk         0x00000020UL           /*!<  */
#define USB_PLLUSBCFG3_DSKEWFASTCAL_Msk       0x00000040UL           /*!<  */
#define USB_PLLUSBCFG3_DSKEWCALIN_Msk         0x000FFF00UL           /*!<  */
#define USB_PLLUSBCFG3_USBCLKSEL_Msk          0x01000000UL           /*!< 0-PLLUSBClk (FOUT0); 1- SYSClk */

/* Bit field enums: */
typedef enum {
  USB_PLLUSBCFG3_USBCLKSEL_PLLUSBClk        = 0x0UL,                /*!< PLLUSBClk (FOUT0) */
  USB_PLLUSBCFG3_USBCLKSEL_SYSClk           = 0x1UL,                /*!< SYSClk */
} USB_PLLUSBCFG3_USBCLKSEL_Enum;

/*--  PLLUSBSTAT: PLL Status Register -------------------------------------------------------------------------*/
typedef struct {
  uint32_t LOCK                   :1;                                /*!< Lock status PLL */
} _USB_PLLUSBSTAT_bits;

/* Bit field positions: */
#define USB_PLLUSBSTAT_LOCK_Pos               0                      /*!< Lock status PLL */

/* Bit field masks: */
#define USB_PLLUSBSTAT_LOCK_Msk               0x00000001UL           /*!< Lock status PLL */

//Cluster USB_EP:
typedef struct {
  union { 
  /*!< EndPoint Data Register */
    __IO uint32_t DATA_BUF;                                           /*!< DATA_BUF : type used for word access */
    __IO _USB_USB_EP_DATA_BUF_bits DATA_BUF_bit;                     /*!< DATA_BUF_bit: structure used for bit access */
  };
  union { 
  /*!< Interrupt flag register no control buffer */
    __IO uint32_t IRQ_STAT;                                           /*!< IRQ_STAT : type used for word access */
    __IO _USB_USB_EP_IRQ_STAT_bits IRQ_STAT_bit;                     /*!< IRQ_STAT_bit: structure used for bit access */
  };
  union { 
  /*!< Enable register Interrupt no control buffer */
    __IO uint32_t IRQ_ENB;                                           /*!< IRQ_ENB : type used for word access */
    __IO _USB_USB_EP_IRQ_ENB_bits IRQ_ENB_bit;                       /*!< IRQ_ENB_bit: structure used for bit access */
  };
  union { 
  /*!< EndPoint Available count register */
    __I uint32_t AVAIL_CNT;                                           /*!< AVAIL_CNT : type used for word access */
    __I _USB_USB_EP_AVAIL_CNT_bits AVAIL_CNT_bit;                    /*!< AVAIL_CNT_bit: structure used for bit access */
  };
  union { 
  /*!< EndPoint Response Set/Clear register */
    __IO uint32_t RSP_SC;                                            /*!< RSP_SC : type used for word access */
    __IO _USB_USB_EP_RSP_SC_bits RSP_SC_bit;                         /*!< RSP_SC_bit: structure used for bit access */
  };
  union { 
  /*!< EndPoint maximum packet size register */
    __IO uint32_t MPS;                                               /*!< MPS : type used for word access */
    __IO _USB_USB_EP_MPS_bits MPS_bit;                               /*!< MPS_bit: structure used for bit access */
  };
  union { 
  /*!< EndPoint Transfer count register */
    __IO uint32_t CNT;                                               /*!< CNT : type used for word access */
    __IO _USB_USB_EP_CNT_bits CNT_bit;                               /*!< CNT_bit: structure used for bit access */
  };
  union { 
  /*!< EndPoint configuration register */
    __IO uint32_t USB_EP_CFG;                                           /*!< USB_EP_CFG : type used for word access */
    __IO _USB_USB_EP_USB_EP_CFG_bits USB_EP_CFG_bit;                 /*!< USB_EP_CFG_bit: structure used for bit access */
  };
  union { 
  /*!< EndPoint RAM start addres register */
    __IO uint32_t START_ADDR;                                           /*!< START_ADDR : type used for word access */
    __IO _USB_USB_EP_START_ADDR_bits START_ADDR_bit;                 /*!< START_ADDR_bit: structure used for bit access */
  };
  union { 
  /*!< EndPoint RAM end addres register */
    __IO uint32_t END_ADDR;                                           /*!< END_ADDR : type used for word access */
    __IO _USB_USB_EP_END_ADDR_bits END_ADDR_bit;                     /*!< END_ADDR_bit: structure used for bit access */
  };
    __IO uint32_t Reserved0[6];
} _USB_USB_EP_TypeDef;
typedef struct {
  union {                                                               /*!< Interrupt status register */
    __IO uint32_t INTSTAT0;                                           /*!< INTSTAT0    : type used for word access */
    __IO _USB_INTSTAT0_bits  INTSTAT0_bit;                           /*!< INTSTAT0_bit: structure used for bit access */
  };
    __IO uint32_t Reserved0;
  union {                                                               /*!< Interrupt enable register */
    __IO uint32_t INTEN0;                                            /*!< INTEN0    : type used for word access */
    __IO _USB_INTEN0_bits  INTEN0_bit;                               /*!< INTEN0_bit: structure used for bit access */
  };
    __IO uint32_t Reserved1;
  union {                                                               /*!< USB Interrupt status register */
    __IO uint32_t INTSTAT1;                                           /*!< INTSTAT1    : type used for word access */
    __IO _USB_INTSTAT1_bits  INTSTAT1_bit;                           /*!< INTSTAT1_bit: structure used for bit access */
  };
  union {                                                               /*!< USB Interrupt enable register */
    __IO uint32_t INTEN1;                                            /*!< INTEN1    : type used for word access */
    __IO _USB_INTEN1_bits  INTEN1_bit;                               /*!< INTEN1_bit: structure used for bit access */
  };
  union {                                                               /*!< USB Operations register */
    __IO uint32_t OPERATIONS;                                           /*!< OPERATIONS    : type used for word access */
    __IO _USB_OPERATIONS_bits  OPERATIONS_bit;                       /*!< OPERATIONS_bit: structure used for bit access */
  };
  union {                                                               /*!< USB Frame counter register */
    __I uint32_t FRAMECNT;                                           /*!< FRAMECNT    : type used for word access */
    __I _USB_FRAMECNT_bits  FRAMECNT_bit;                            /*!< FRAMECNT_bit: structure used for bit access */
  };
  union {                                                               /*!< USB Address register */
    __IO uint32_t USBADDR;                                           /*!< USBADDR    : type used for word access */
    __IO _USB_USBADDR_bits  USBADDR_bit;                             /*!< USBADDR_bit: structure used for bit access */
  };
    __IO uint32_t Reserved2;
  union {                                                               /*!< Data buffer for transmission register of Control EndPoint */
    __IO uint32_t CEP_DATA_BUF;                                           /*!< CEP_DATA_BUF    : type used for word access */
    __IO _USB_CEP_DATA_BUF_bits  CEP_DATA_BUF_bit;                   /*!< CEP_DATA_BUF_bit: structure used for bit access */
  };
  union {                                                               /*!< Control register and buffer status of Control EndPoint */
    __IO uint32_t CEP_CTRL_STAT;                                           /*!< CEP_CTRL_STAT    : type used for word access */
    __IO _USB_CEP_CTRL_STAT_bits  CEP_CTRL_STAT_bit;                 /*!< CEP_CTRL_STAT_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt Enable buffer register of Control EndPoint */
    __IO uint32_t CEP_IRQ_ENB;                                           /*!< CEP_IRQ_ENB    : type used for word access */
    __IO _USB_CEP_IRQ_ENB_bits  CEP_IRQ_ENB_bit;                     /*!< CEP_IRQ_ENB_bit: structure used for bit access */
  };
  union {                                                               /*!< Buffer interrupt flag register of Control EndPoint */
    __IO uint32_t CEP_IRQ_STAT;                                           /*!< CEP_IRQ_STAT    : type used for word access */
    __IO _USB_CEP_IRQ_STAT_bits  CEP_IRQ_STAT_bit;                   /*!< CEP_IRQ_STAT_bit: structure used for bit access */
  };
  union {                                                               /*!< In transfer data count register  of Control EndPoint */
    __IO uint32_t CEP_IN_XFRCNT;                                           /*!< CEP_IN_XFRCNT    : type used for word access */
    __IO _USB_CEP_IN_XFRCNT_bits  CEP_IN_XFRCNT_bit;                 /*!< CEP_IN_XFRCNT_bit: structure used for bit access */
  };
  union {                                                               /*!< Out-transfer data count register of Control EndPoint */
    __I uint32_t CEP_OUT_XFRCNT;                                           /*!< CEP_OUT_XFRCNT    : type used for word access */
    __I _USB_CEP_OUT_XFRCNT_bits  CEP_OUT_XFRCNT_bit;                /*!< CEP_OUT_XFRCNT_bit: structure used for bit access */
  };
  union {                                                               /*!< Register number of bytes received */
    __I uint32_t CEP_DATA_AVL;                                           /*!< CEP_DATA_AVL    : type used for word access */
    __I _USB_CEP_DATA_AVL_bits  CEP_DATA_AVL_bit;                    /*!< CEP_DATA_AVL_bit: structure used for bit access */
  };
  union {                                                               /*!< Register zero and first byte packet Setup  of Control EndPoint */
    __I uint32_t CEP_SETUP1_0;                                           /*!< CEP_SETUP1_0    : type used for word access */
    __I _USB_CEP_SETUP1_0_bits  CEP_SETUP1_0_bit;                    /*!< CEP_SETUP1_0_bit: structure used for bit access */
  };
  union {                                                               /*!< Register 2nd and 3rd bytes of the packet Setup  of Control EndPoint */
    __I uint32_t CEP_SETUP3_2;                                           /*!< CEP_SETUP3_2    : type used for word access */
    __I _USB_CEP_SETUP3_2_bits  CEP_SETUP3_2_bit;                    /*!< CEP_SETUP3_2_bit: structure used for bit access */
  };
  union {                                                               /*!< Register 4th and 5th bytes of the packet Setup  of Control EndPoint */
    __I uint32_t CEP_SETUP5_4;                                           /*!< CEP_SETUP5_4    : type used for word access */
    __I _USB_CEP_SETUP5_4_bits  CEP_SETUP5_4_bit;                    /*!< CEP_SETUP5_4_bit: structure used for bit access */
  };
  union {                                                               /*!< Register on the 6th and 7th byte packet Setup of Control EndPoint */
    __I uint32_t CEP_SETUP7_6;                                           /*!< CEP_SETUP7_6    : type used for word access */
    __I _USB_CEP_SETUP7_6_bits  CEP_SETUP7_6_bit;                    /*!< CEP_SETUP7_6_bit: structure used for bit access */
  };
  union {                                                               /*!< Control EndPoint RAM start Addr register */
    __IO uint32_t CEP_START_ADDR;                                           /*!< CEP_START_ADDR    : type used for word access */
    __IO _USB_CEP_START_ADDR_bits  CEP_START_ADDR_bit;                /*!< CEP_START_ADDR_bit: structure used for bit access */
  };
  union {                                                               /*!< Control EndPoint RAM end Addr register */
    __IO uint32_t CEP_END_ADDR;                                           /*!< CEP_END_ADDR    : type used for word access */
    __IO _USB_CEP_END_ADDR_bits  CEP_END_ADDR_bit;                   /*!< CEP_END_ADDR_bit: structure used for bit access */
  };
  union {                                                               /*!< DMA Control Register */
    __IO uint32_t DMA_CTRL_STS;                                           /*!< DMA_CTRL_STS    : type used for word access */
    __IO _USB_DMA_CTRL_STS_bits  DMA_CTRL_STS_bit;                   /*!< DMA_CTRL_STS_bit: structure used for bit access */
  };
  union {                                                               /*!< DMA Count byte Register */
    __IO uint32_t DMA_CNT;                                           /*!< DMA_CNT    : type used for word access */
    __IO _USB_DMA_CNT_bits  DMA_CNT_bit;                             /*!< DMA_CNT_bit: structure used for bit access */
  };
    __IO uint32_t Reserved3[3];
  _USB_USB_EP_TypeDef USB_EP[4];
    __IO uint32_t Reserved4[356];
  union {                                                               /*!< AHB addr register */
    __IO uint32_t AHB_DMA_ADDR;                                           /*!< AHB_DMA_ADDR    : type used for word access */
    __IO _USB_AHB_DMA_ADDR_bits  AHB_DMA_ADDR_bit;                   /*!< AHB_DMA_ADDR_bit: structure used for bit access */
  };
    __IO uint32_t Reserved5[47];
  union {                                                               /*!< USB PHY PowerDown Register */
    __IO uint32_t PHY_PD;                                            /*!< PHY_PD    : type used for word access */
    __IO _USB_PHY_PD_bits  PHY_PD_bit;                               /*!< PHY_PD_bit: structure used for bit access */
  };
    __IO uint32_t Reserved6[15];
  union {                                                               /*!< PLL configudration 0 register */
    __IO uint32_t PLLUSBCFG0;                                           /*!< PLLUSBCFG0    : type used for word access */
    __IO _USB_PLLUSBCFG0_bits  PLLUSBCFG0_bit;                       /*!< PLLUSBCFG0_bit: structure used for bit access */
  };
  union {                                                               /*!< PLL configudration 1 register */
    __IO uint32_t PLLUSBCFG1;                                           /*!< PLLUSBCFG1    : type used for word access */
    __IO _USB_PLLUSBCFG1_bits  PLLUSBCFG1_bit;                       /*!< PLLUSBCFG1_bit: structure used for bit access */
  };
  union {                                                               /*!< PLL configudration 2 register */
    __IO uint32_t PLLUSBCFG2;                                           /*!< PLLUSBCFG2    : type used for word access */
    __IO _USB_PLLUSBCFG2_bits  PLLUSBCFG2_bit;                       /*!< PLLUSBCFG2_bit: structure used for bit access */
  };
  union {                                                               /*!< PLL configudration 3 register */
    __IO uint32_t PLLUSBCFG3;                                           /*!< PLLUSBCFG3    : type used for word access */
    __IO _USB_PLLUSBCFG3_bits  PLLUSBCFG3_bit;                       /*!< PLLUSBCFG3_bit: structure used for bit access */
  };
  union {                                                               /*!< PLL Status Register */
    __I uint32_t PLLUSBSTAT;                                           /*!< PLLUSBSTAT    : type used for word access */
    __I _USB_PLLUSBSTAT_bits  PLLUSBSTAT_bit;                        /*!< PLLUSBSTAT_bit: structure used for bit access */
  };
} USB_TypeDef;


/******************************************************************************/
/*                         UART registers                                     */
/******************************************************************************/

/*--  DR: Data Register ---------------------------------------------------------------------------------------*/
typedef struct {
  uint32_t DATA                   :8;                                /*!< Received/Transmitted data character */
  uint32_t FE                     :1;                                /*!< Framing error */
  uint32_t PE                     :1;                                /*!< Parity error */
  uint32_t BE                     :1;                                /*!< Break error */
  uint32_t OE                     :1;                                /*!< Overrun error */
} _UART_DR_bits;

/* Bit field positions: */
#define UART_DR_DATA_Pos                      0                      /*!< Received/Transmitted data character */
#define UART_DR_FE_Pos                        8                      /*!< Framing error */
#define UART_DR_PE_Pos                        9                      /*!< Parity error */
#define UART_DR_BE_Pos                        10                     /*!< Break error */
#define UART_DR_OE_Pos                        11                     /*!< Overrun error */

/* Bit field masks: */
#define UART_DR_DATA_Msk                      0x000000FFUL           /*!< Received/Transmitted data character */
#define UART_DR_FE_Msk                        0x00000100UL           /*!< Framing error */
#define UART_DR_PE_Msk                        0x00000200UL           /*!< Parity error */
#define UART_DR_BE_Msk                        0x00000400UL           /*!< Break error */
#define UART_DR_OE_Msk                        0x00000800UL           /*!< Overrun error */

/*--  RSR: Receive Status Register/Error Clear Register -------------------------------------------------------*/
typedef struct {
  uint32_t FE                     :1;                                /*!< Framing error */
  uint32_t PE                     :1;                                /*!< Parity error */
  uint32_t BE                     :1;                                /*!< Break error */
  uint32_t OE                     :1;                                /*!< Overrun error */
} _UART_RSR_bits;

/* Bit field positions: */
#define UART_RSR_FE_Pos                       0                      /*!< Framing error */
#define UART_RSR_PE_Pos                       1                      /*!< Parity error */
#define UART_RSR_BE_Pos                       2                      /*!< Break error */
#define UART_RSR_OE_Pos                       3                      /*!< Overrun error */

/* Bit field masks: */
#define UART_RSR_FE_Msk                       0x00000001UL           /*!< Framing error */
#define UART_RSR_PE_Msk                       0x00000002UL           /*!< Parity error */
#define UART_RSR_BE_Msk                       0x00000004UL           /*!< Break error */
#define UART_RSR_OE_Msk                       0x00000008UL           /*!< Overrun error */

/*--  FR: Flag Register ---------------------------------------------------------------------------------------*/
typedef struct {
  uint32_t CTS                    :1;                                /*!< Clear to send */
  uint32_t DSR                    :1;                                /*!< Data set ready */
  uint32_t DCD                    :1;                                /*!< Data carrier detect */
  uint32_t BUSY                   :1;                                /*!< UART busy */
  uint32_t RXFE                   :1;                                /*!< Receive FIFO empty */
  uint32_t TXFF                   :1;                                /*!< Transmit FIFO full */
  uint32_t RXFF                   :1;                                /*!< Receive FIFO full */
  uint32_t TXFE                   :1;                                /*!< Transmit FIFO empty */
  uint32_t RI                     :1;                                /*!< Ring indicator */
} _UART_FR_bits;

/* Bit field positions: */
#define UART_FR_CTS_Pos                       0                      /*!< Clear to send */
#define UART_FR_DSR_Pos                       1                      /*!< Data set ready */
#define UART_FR_DCD_Pos                       2                      /*!< Data carrier detect */
#define UART_FR_BUSY_Pos                      3                      /*!< UART busy */
#define UART_FR_RXFE_Pos                      4                      /*!< Receive FIFO empty */
#define UART_FR_TXFF_Pos                      5                      /*!< Transmit FIFO full */
#define UART_FR_RXFF_Pos                      6                      /*!< Receive FIFO full */
#define UART_FR_TXFE_Pos                      7                      /*!< Transmit FIFO empty */
#define UART_FR_RI_Pos                        8                      /*!< Ring indicator */

/* Bit field masks: */
#define UART_FR_CTS_Msk                       0x00000001UL           /*!< Clear to send */
#define UART_FR_DSR_Msk                       0x00000002UL           /*!< Data set ready */
#define UART_FR_DCD_Msk                       0x00000004UL           /*!< Data carrier detect */
#define UART_FR_BUSY_Msk                      0x00000008UL           /*!< UART busy */
#define UART_FR_RXFE_Msk                      0x00000010UL           /*!< Receive FIFO empty */
#define UART_FR_TXFF_Msk                      0x00000020UL           /*!< Transmit FIFO full */
#define UART_FR_RXFF_Msk                      0x00000040UL           /*!< Receive FIFO full */
#define UART_FR_TXFE_Msk                      0x00000080UL           /*!< Transmit FIFO empty */
#define UART_FR_RI_Msk                        0x00000100UL           /*!< Ring indicator */

/*--  ILPR: IrDA Low-Power Counter Register -------------------------------------------------------------------*/
typedef struct {
  uint32_t ILPDVSR                :8;                                /*!< 8-bit low-power divisor value */
} _UART_ILPR_bits;

/* Bit field positions: */
#define UART_ILPR_ILPDVSR_Pos                 0                      /*!< 8-bit low-power divisor value */

/* Bit field masks: */
#define UART_ILPR_ILPDVSR_Msk                 0x000000FFUL           /*!< 8-bit low-power divisor value */

/*--  IBRD: Integer Baud Rate Register ------------------------------------------------------------------------*/
typedef struct {
  uint32_t DIVINT                 :16;                               /*!< The integer baud rate divisor */
} _UART_IBRD_bits;

/* Bit field positions: */
#define UART_IBRD_DIVINT_Pos                  0                      /*!< The integer baud rate divisor */

/* Bit field masks: */
#define UART_IBRD_DIVINT_Msk                  0x0000FFFFUL           /*!< The integer baud rate divisor */

/*--  FBRD: Fractional Baud Rate Register ---------------------------------------------------------------------*/
typedef struct {
  uint32_t DIVFRAC                :6;                                /*!< The fractional baud rate divisor */
} _UART_FBRD_bits;

/* Bit field positions: */
#define UART_FBRD_DIVFRAC_Pos                 0                      /*!< The fractional baud rate divisor */

/* Bit field masks: */
#define UART_FBRD_DIVFRAC_Msk                 0x0000003FUL           /*!< The fractional baud rate divisor */

/*--  LCRH: Line Control Register -----------------------------------------------------------------------------*/
typedef struct {
  uint32_t BRK                    :1;                                /*!< Send break */
  uint32_t PEN                    :1;                                /*!< Parity enable */
  uint32_t EPS                    :1;                                /*!< Even parity select */
  uint32_t STP2                   :1;                                /*!< Two stop bits select */
  uint32_t FEN                    :1;                                /*!< Enable FIFOs */
  uint32_t WLEN                   :2;                                /*!< Word length */
  uint32_t SPS                    :1;                                /*!< Stick parity select */
} _UART_LCRH_bits;

/* Bit field positions: */
#define UART_LCRH_BRK_Pos                     0                      /*!< Send break */
#define UART_LCRH_PEN_Pos                     1                      /*!< Parity enable */
#define UART_LCRH_EPS_Pos                     2                      /*!< Even parity select */
#define UART_LCRH_STP2_Pos                    3                      /*!< Two stop bits select */
#define UART_LCRH_FEN_Pos                     4                      /*!< Enable FIFOs */
#define UART_LCRH_WLEN_Pos                    5                      /*!< Word length */
#define UART_LCRH_SPS_Pos                     7                      /*!< Stick parity select */

/* Bit field masks: */
#define UART_LCRH_BRK_Msk                     0x00000001UL           /*!< Send break */
#define UART_LCRH_PEN_Msk                     0x00000002UL           /*!< Parity enable */
#define UART_LCRH_EPS_Msk                     0x00000004UL           /*!< Even parity select */
#define UART_LCRH_STP2_Msk                    0x00000008UL           /*!< Two stop bits select */
#define UART_LCRH_FEN_Msk                     0x00000010UL           /*!< Enable FIFOs */
#define UART_LCRH_WLEN_Msk                    0x00000060UL           /*!< Word length */
#define UART_LCRH_SPS_Msk                     0x00000080UL           /*!< Stick parity select */

/* Bit field enums: */
typedef enum {
  UART_LCRH_WLEN_5bit                        = 0x0UL,                /*!< 5 bit in informational word */
  UART_LCRH_WLEN_6bit                        = 0x1UL,                /*!< 6 bit in informational word */
  UART_LCRH_WLEN_7bit                        = 0x2UL,                /*!< 7 bit in informational word */
  UART_LCRH_WLEN_8bit                        = 0x3UL,                /*!< 8 bit in informational word */
} UART_LCRH_WLEN_Enum;

/*--  CR: Control Register ------------------------------------------------------------------------------------*/
typedef struct {
  uint32_t UARTEN                 :1;                                /*!< UART enable */
  uint32_t SIREN                  :1;                                /*!< Enable IrDA SIR ENDEC */
  uint32_t SIRLP                  :1;                                /*!< Enable SIR low-power IrDA mode */
  uint32_t                        :4;                                /*!< RESERVED */
  uint32_t LBE                    :1;                                /*!< Loopback enable */
  uint32_t TXE                    :1;                                /*!< Transmit enable */
  uint32_t RXE                    :1;                                /*!< Receive enable */
  uint32_t DTR                    :1;                                /*!< Data transmit ready */
  uint32_t RTS                    :1;                                /*!< Request to send */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t RTSEN                  :1;                                /*!< RTS hardware flow control enable */
  uint32_t CTSEN                  :1;                                /*!< CTS hardware flow control enable */
} _UART_CR_bits;

/* Bit field positions: */
#define UART_CR_UARTEN_Pos                    0                      /*!< UART enable */
#define UART_CR_SIREN_Pos                     1                      /*!< Enable IrDA SIR ENDEC */
#define UART_CR_SIRLP_Pos                     2                      /*!< Enable SIR low-power IrDA mode */
#define UART_CR_LBE_Pos                       7                      /*!< Loopback enable */
#define UART_CR_TXE_Pos                       8                      /*!< Transmit enable */
#define UART_CR_RXE_Pos                       9                      /*!< Receive enable */
#define UART_CR_DTR_Pos                       10                     /*!< Data transmit ready */
#define UART_CR_RTS_Pos                       11                     /*!< Request to send */
#define UART_CR_RTSEN_Pos                     14                     /*!< RTS hardware flow control enable */
#define UART_CR_CTSEN_Pos                     15                     /*!< CTS hardware flow control enable */

/* Bit field masks: */
#define UART_CR_UARTEN_Msk                    0x00000001UL           /*!< UART enable */
#define UART_CR_SIREN_Msk                     0x00000002UL           /*!< Enable IrDA SIR ENDEC */
#define UART_CR_SIRLP_Msk                     0x00000004UL           /*!< Enable SIR low-power IrDA mode */
#define UART_CR_LBE_Msk                       0x00000080UL           /*!< Loopback enable */
#define UART_CR_TXE_Msk                       0x00000100UL           /*!< Transmit enable */
#define UART_CR_RXE_Msk                       0x00000200UL           /*!< Receive enable */
#define UART_CR_DTR_Msk                       0x00000400UL           /*!< Data transmit ready */
#define UART_CR_RTS_Msk                       0x00000800UL           /*!< Request to send */
#define UART_CR_RTSEN_Msk                     0x00004000UL           /*!< RTS hardware flow control enable */
#define UART_CR_CTSEN_Msk                     0x00008000UL           /*!< CTS hardware flow control enable */

/*--  IFLS: Interrupt FIFO Level Select Register --------------------------------------------------------------*/
typedef struct {
  uint32_t TXIFLSEL               :3;                                /*!< Transmit interrupt FIFO level select */
  uint32_t RXIFLSEL               :3;                                /*!< Receive interrupt FIFO level select */
} _UART_IFLS_bits;

/* Bit field positions: */
#define UART_IFLS_TXIFLSEL_Pos                0                      /*!< Transmit interrupt FIFO level select */
#define UART_IFLS_RXIFLSEL_Pos                3                      /*!< Receive interrupt FIFO level select */

/* Bit field masks: */
#define UART_IFLS_TXIFLSEL_Msk                0x00000007UL           /*!< Transmit interrupt FIFO level select */
#define UART_IFLS_RXIFLSEL_Msk                0x00000038UL           /*!< Receive interrupt FIFO level select */

/* Bit field enums: */
typedef enum {
  UART_IFLS_TXIFLSEL_Lvl18                   = 0x0UL,                /*!< interrupt on 1/8 */
  UART_IFLS_TXIFLSEL_Lvl14                   = 0x1UL,                /*!< interrupt on 1/4 */
  UART_IFLS_TXIFLSEL_Lvl12                   = 0x2UL,                /*!< interrupt on 1/2 */
  UART_IFLS_TXIFLSEL_Lvl34                   = 0x3UL,                /*!< interrupt on 3/4 */
  UART_IFLS_TXIFLSEL_Lvl78                   = 0x4UL,                /*!< interrupt on 7/8 */
} UART_IFLS_TXIFLSEL_Enum;

typedef enum {
  UART_IFLS_RXIFLSEL_Lvl18                   = 0x0UL,                /*!< interrupt on 1/8 */
  UART_IFLS_RXIFLSEL_Lvl14                   = 0x1UL,                /*!< interrupt on 1/4 */
  UART_IFLS_RXIFLSEL_Lvl12                   = 0x2UL,                /*!< interrupt on 1/2 */
  UART_IFLS_RXIFLSEL_Lvl34                   = 0x3UL,                /*!< interrupt on 3/4 */
  UART_IFLS_RXIFLSEL_Lvl78                   = 0x4UL,                /*!< interrupt on 7/8 */
} UART_IFLS_RXIFLSEL_Enum;

/*--  IMSC: Interrupt Mask Set/Clear Register -----------------------------------------------------------------*/
typedef struct {
  uint32_t RIMIM                  :1;                                /*!< nUARTRI modem interrupt mask */
  uint32_t CTSMIM                 :1;                                /*!< nUARTCTS modem interrupt mask */
  uint32_t DCDMIM                 :1;                                /*!< nUARTDCD modem interrupt mask */
  uint32_t DSRMIM                 :1;                                /*!< nUARTDSR modem interrupt mask */
  uint32_t RXIM                   :1;                                /*!< Receive interrupt mask */
  uint32_t TXIM                   :1;                                /*!< Transmit interrupt mask */
  uint32_t RTIM                   :1;                                /*!< Receive timeout interrupt mask */
  uint32_t FERIM                  :1;                                /*!< Framing error interrupt mask */
  uint32_t PERIM                  :1;                                /*!< Parity error interrupt mask */
  uint32_t BERIM                  :1;                                /*!< Break error interrupt mask */
  uint32_t OERIM                  :1;                                /*!< Overrun error interrupt mask */
  uint32_t TDIM                   :1;                                /*!< Transmit done interrupt mask */
} _UART_IMSC_bits;

/* Bit field positions: */
#define UART_IMSC_RIMIM_Pos                   0                      /*!< nUARTRI modem interrupt mask */
#define UART_IMSC_CTSMIM_Pos                  1                      /*!< nUARTCTS modem interrupt mask */
#define UART_IMSC_DCDMIM_Pos                  2                      /*!< nUARTDCD modem interrupt mask */
#define UART_IMSC_DSRMIM_Pos                  3                      /*!< nUARTDSR modem interrupt mask */
#define UART_IMSC_RXIM_Pos                    4                      /*!< Receive interrupt mask */
#define UART_IMSC_TXIM_Pos                    5                      /*!< Transmit interrupt mask */
#define UART_IMSC_RTIM_Pos                    6                      /*!< Receive timeout interrupt mask */
#define UART_IMSC_FERIM_Pos                   7                      /*!< Framing error interrupt mask */
#define UART_IMSC_PERIM_Pos                   8                      /*!< Parity error interrupt mask */
#define UART_IMSC_BERIM_Pos                   9                      /*!< Break error interrupt mask */
#define UART_IMSC_OERIM_Pos                   10                     /*!< Overrun error interrupt mask */
#define UART_IMSC_TDIM_Pos                    11                     /*!< Transmit done interrupt mask */

/* Bit field masks: */
#define UART_IMSC_RIMIM_Msk                   0x00000001UL           /*!< nUARTRI modem interrupt mask */
#define UART_IMSC_CTSMIM_Msk                  0x00000002UL           /*!< nUARTCTS modem interrupt mask */
#define UART_IMSC_DCDMIM_Msk                  0x00000004UL           /*!< nUARTDCD modem interrupt mask */
#define UART_IMSC_DSRMIM_Msk                  0x00000008UL           /*!< nUARTDSR modem interrupt mask */
#define UART_IMSC_RXIM_Msk                    0x00000010UL           /*!< Receive interrupt mask */
#define UART_IMSC_TXIM_Msk                    0x00000020UL           /*!< Transmit interrupt mask */
#define UART_IMSC_RTIM_Msk                    0x00000040UL           /*!< Receive timeout interrupt mask */
#define UART_IMSC_FERIM_Msk                   0x00000080UL           /*!< Framing error interrupt mask */
#define UART_IMSC_PERIM_Msk                   0x00000100UL           /*!< Parity error interrupt mask */
#define UART_IMSC_BERIM_Msk                   0x00000200UL           /*!< Break error interrupt mask */
#define UART_IMSC_OERIM_Msk                   0x00000400UL           /*!< Overrun error interrupt mask */
#define UART_IMSC_TDIM_Msk                    0x00000800UL           /*!< Transmit done interrupt mask */

/*--  RIS: Raw Interrupt Status Register ----------------------------------------------------------------------*/
typedef struct {
  uint32_t RIRMIS                 :1;                                /*!< nUARTRI modem interrupt status */
  uint32_t CTSRMIS                :1;                                /*!< nUARTCTS modem interrupt status */
  uint32_t DCDRMIS                :1;                                /*!< nUARTDCD modem interrupt status */
  uint32_t DSRRMIS                :1;                                /*!< nUARTDSR modem interrupt status */
  uint32_t RXRIS                  :1;                                /*!< Receive interrupt status */
  uint32_t TXRIS                  :1;                                /*!< Transmit interrupt status */
  uint32_t RTRIS                  :1;                                /*!< Receive timeout interrupt status */
  uint32_t FERIS                  :1;                                /*!< Framing error interrupt status */
  uint32_t PERIS                  :1;                                /*!< Parity error interrupt status */
  uint32_t BERIS                  :1;                                /*!< Break error interrupt status */
  uint32_t OERIS                  :1;                                /*!< Overrun error interrupt status */
  uint32_t TDRIS                  :1;                                /*!< Transmit done raw interrupt status */
} _UART_RIS_bits;

/* Bit field positions: */
#define UART_RIS_RIRMIS_Pos                   0                      /*!< nUARTRI modem interrupt status */
#define UART_RIS_CTSRMIS_Pos                  1                      /*!< nUARTCTS modem interrupt status */
#define UART_RIS_DCDRMIS_Pos                  2                      /*!< nUARTDCD modem interrupt status */
#define UART_RIS_DSRRMIS_Pos                  3                      /*!< nUARTDSR modem interrupt status */
#define UART_RIS_RXRIS_Pos                    4                      /*!< Receive interrupt status */
#define UART_RIS_TXRIS_Pos                    5                      /*!< Transmit interrupt status */
#define UART_RIS_RTRIS_Pos                    6                      /*!< Receive timeout interrupt status */
#define UART_RIS_FERIS_Pos                    7                      /*!< Framing error interrupt status */
#define UART_RIS_PERIS_Pos                    8                      /*!< Parity error interrupt status */
#define UART_RIS_BERIS_Pos                    9                      /*!< Break error interrupt status */
#define UART_RIS_OERIS_Pos                    10                     /*!< Overrun error interrupt status */
#define UART_RIS_TDRIS_Pos                    11                     /*!< Transmit done raw interrupt status */

/* Bit field masks: */
#define UART_RIS_RIRMIS_Msk                   0x00000001UL           /*!< nUARTRI modem interrupt status */
#define UART_RIS_CTSRMIS_Msk                  0x00000002UL           /*!< nUARTCTS modem interrupt status */
#define UART_RIS_DCDRMIS_Msk                  0x00000004UL           /*!< nUARTDCD modem interrupt status */
#define UART_RIS_DSRRMIS_Msk                  0x00000008UL           /*!< nUARTDSR modem interrupt status */
#define UART_RIS_RXRIS_Msk                    0x00000010UL           /*!< Receive interrupt status */
#define UART_RIS_TXRIS_Msk                    0x00000020UL           /*!< Transmit interrupt status */
#define UART_RIS_RTRIS_Msk                    0x00000040UL           /*!< Receive timeout interrupt status */
#define UART_RIS_FERIS_Msk                    0x00000080UL           /*!< Framing error interrupt status */
#define UART_RIS_PERIS_Msk                    0x00000100UL           /*!< Parity error interrupt status */
#define UART_RIS_BERIS_Msk                    0x00000200UL           /*!< Break error interrupt status */
#define UART_RIS_OERIS_Msk                    0x00000400UL           /*!< Overrun error interrupt status */
#define UART_RIS_TDRIS_Msk                    0x00000800UL           /*!< Transmit done raw interrupt status */

/*--  MIS: Masked Interrupt Status Register -------------------------------------------------------------------*/
typedef struct {
  uint32_t RIMMIS                 :1;                                /*!< nUARTRI modem masked interrupt status */
  uint32_t CTSMMIS                :1;                                /*!< nUARTCTS modem masked interrupt status */
  uint32_t DCDMMIS                :1;                                /*!< nUARTDCD modem masked interrupt status */
  uint32_t DSRMMIS                :1;                                /*!< nUARTDSR modem masked interrupt status */
  uint32_t RXMIS                  :1;                                /*!< Receive masked interrupt status */
  uint32_t TXMIS                  :1;                                /*!< Transmit masked interrupt status */
  uint32_t RTMIS                  :1;                                /*!< Receive timeout masked interrupt status */
  uint32_t FEMIS                  :1;                                /*!< Framing error masked interrupt status */
  uint32_t PEMIS                  :1;                                /*!< Parity error masked interrupt status */
  uint32_t BEMIS                  :1;                                /*!< Break error masked interrupt status */
  uint32_t OEMIS                  :1;                                /*!< Overrun error masked interrupt status */
  uint32_t TDMIS                  :1;                                /*!< Transmit done masked interrupt status */
} _UART_MIS_bits;

/* Bit field positions: */
#define UART_MIS_RIMMIS_Pos                   0                      /*!< nUARTRI modem masked interrupt status */
#define UART_MIS_CTSMMIS_Pos                  1                      /*!< nUARTCTS modem masked interrupt status */
#define UART_MIS_DCDMMIS_Pos                  2                      /*!< nUARTDCD modem masked interrupt status */
#define UART_MIS_DSRMMIS_Pos                  3                      /*!< nUARTDSR modem masked interrupt status */
#define UART_MIS_RXMIS_Pos                    4                      /*!< Receive masked interrupt status */
#define UART_MIS_TXMIS_Pos                    5                      /*!< Transmit masked interrupt status */
#define UART_MIS_RTMIS_Pos                    6                      /*!< Receive timeout masked interrupt status */
#define UART_MIS_FEMIS_Pos                    7                      /*!< Framing error masked interrupt status */
#define UART_MIS_PEMIS_Pos                    8                      /*!< Parity error masked interrupt status */
#define UART_MIS_BEMIS_Pos                    9                      /*!< Break error masked interrupt status */
#define UART_MIS_OEMIS_Pos                    10                     /*!< Overrun error masked interrupt status */
#define UART_MIS_TDMIS_Pos                    11                     /*!< Transmit done masked interrupt status */

/* Bit field masks: */
#define UART_MIS_RIMMIS_Msk                   0x00000001UL           /*!< nUARTRI modem masked interrupt status */
#define UART_MIS_CTSMMIS_Msk                  0x00000002UL           /*!< nUARTCTS modem masked interrupt status */
#define UART_MIS_DCDMMIS_Msk                  0x00000004UL           /*!< nUARTDCD modem masked interrupt status */
#define UART_MIS_DSRMMIS_Msk                  0x00000008UL           /*!< nUARTDSR modem masked interrupt status */
#define UART_MIS_RXMIS_Msk                    0x00000010UL           /*!< Receive masked interrupt status */
#define UART_MIS_TXMIS_Msk                    0x00000020UL           /*!< Transmit masked interrupt status */
#define UART_MIS_RTMIS_Msk                    0x00000040UL           /*!< Receive timeout masked interrupt status */
#define UART_MIS_FEMIS_Msk                    0x00000080UL           /*!< Framing error masked interrupt status */
#define UART_MIS_PEMIS_Msk                    0x00000100UL           /*!< Parity error masked interrupt status */
#define UART_MIS_BEMIS_Msk                    0x00000200UL           /*!< Break error masked interrupt status */
#define UART_MIS_OEMIS_Msk                    0x00000400UL           /*!< Overrun error masked interrupt status */
#define UART_MIS_TDMIS_Msk                    0x00000800UL           /*!< Transmit done masked interrupt status */

/*--  ICR: Interrupt Clear Register ---------------------------------------------------------------------------*/
typedef struct {
  uint32_t RIMIC                  :1;                                /*!< nUARTRI modem interrupt clear */
  uint32_t CTSMIC                 :1;                                /*!< nUARTCTS modem interrupt clear */
  uint32_t DCDMIC                 :1;                                /*!< nUARTDCD modem interrupt clear */
  uint32_t DSRMIC                 :1;                                /*!< nUARTDSR modem interrupt clear */
  uint32_t RXIC                   :1;                                /*!< Receive interrupt clear */
  uint32_t TXIC                   :1;                                /*!< Transmit interrupt clear */
  uint32_t RTIC                   :1;                                /*!< Receive timeout interrupt clear */
  uint32_t FEIC                   :1;                                /*!< Framing error interrupt clear */
  uint32_t PEIC                   :1;                                /*!< Parity error interrupt clear */
  uint32_t BEIC                   :1;                                /*!< Break error interrupt clear */
  uint32_t OEIC                   :1;                                /*!< Overrun error interrupt clear */
  uint32_t TDIC                   :1;                                /*!< Transmit done interrupt clear */
} _UART_ICR_bits;

/* Bit field positions: */
#define UART_ICR_RIMIC_Pos                    0                      /*!< nUARTRI modem interrupt clear */
#define UART_ICR_CTSMIC_Pos                   1                      /*!< nUARTCTS modem interrupt clear */
#define UART_ICR_DCDMIC_Pos                   2                      /*!< nUARTDCD modem interrupt clear */
#define UART_ICR_DSRMIC_Pos                   3                      /*!< nUARTDSR modem interrupt clear */
#define UART_ICR_RXIC_Pos                     4                      /*!< Receive interrupt clear */
#define UART_ICR_TXIC_Pos                     5                      /*!< Transmit interrupt clear */
#define UART_ICR_RTIC_Pos                     6                      /*!< Receive timeout interrupt clear */
#define UART_ICR_FEIC_Pos                     7                      /*!< Framing error interrupt clear */
#define UART_ICR_PEIC_Pos                     8                      /*!< Parity error interrupt clear */
#define UART_ICR_BEIC_Pos                     9                      /*!< Break error interrupt clear */
#define UART_ICR_OEIC_Pos                     10                     /*!< Overrun error interrupt clear */
#define UART_ICR_TDIC_Pos                     11                     /*!< Transmit done interrupt clear */

/* Bit field masks: */
#define UART_ICR_RIMIC_Msk                    0x00000001UL           /*!< nUARTRI modem interrupt clear */
#define UART_ICR_CTSMIC_Msk                   0x00000002UL           /*!< nUARTCTS modem interrupt clear */
#define UART_ICR_DCDMIC_Msk                   0x00000004UL           /*!< nUARTDCD modem interrupt clear */
#define UART_ICR_DSRMIC_Msk                   0x00000008UL           /*!< nUARTDSR modem interrupt clear */
#define UART_ICR_RXIC_Msk                     0x00000010UL           /*!< Receive interrupt clear */
#define UART_ICR_TXIC_Msk                     0x00000020UL           /*!< Transmit interrupt clear */
#define UART_ICR_RTIC_Msk                     0x00000040UL           /*!< Receive timeout interrupt clear */
#define UART_ICR_FEIC_Msk                     0x00000080UL           /*!< Framing error interrupt clear */
#define UART_ICR_PEIC_Msk                     0x00000100UL           /*!< Parity error interrupt clear */
#define UART_ICR_BEIC_Msk                     0x00000200UL           /*!< Break error interrupt clear */
#define UART_ICR_OEIC_Msk                     0x00000400UL           /*!< Overrun error interrupt clear */
#define UART_ICR_TDIC_Msk                     0x00000800UL           /*!< Transmit done interrupt clear */

/*--  DMACR: DMA Control Register -----------------------------------------------------------------------------*/
typedef struct {
  uint32_t RXDMAE                 :1;                                /*!< Receive DMA enable */
  uint32_t TXDMAE                 :1;                                /*!< Transmit DMA enable */
  uint32_t DMAONERR               :1;                                /*!< DMA on error */
} _UART_DMACR_bits;

/* Bit field positions: */
#define UART_DMACR_RXDMAE_Pos                 0                      /*!< Receive DMA enable */
#define UART_DMACR_TXDMAE_Pos                 1                      /*!< Transmit DMA enable */
#define UART_DMACR_DMAONERR_Pos               2                      /*!< DMA on error */

/* Bit field masks: */
#define UART_DMACR_RXDMAE_Msk                 0x00000001UL           /*!< Receive DMA enable */
#define UART_DMACR_TXDMAE_Msk                 0x00000002UL           /*!< Transmit DMA enable */
#define UART_DMACR_DMAONERR_Msk               0x00000004UL           /*!< DMA on error */

typedef struct {
  union {                                                               /*!< Data Register */
    __IO uint32_t DR;                                                /*!< DR    : type used for word access */
    __IO _UART_DR_bits  DR_bit;                                      /*!< DR_bit: structure used for bit access */
  };
  union {                                                               /*!< Receive Status Register/Error Clear Register */
    __IO uint32_t RSR;                                               /*!< RSR    : type used for word access */
    __IO _UART_RSR_bits  RSR_bit;                                    /*!< RSR_bit: structure used for bit access */
  };
    __IO uint32_t Reserved0[4];
  union {                                                               /*!< Flag Register */
    __I uint32_t FR;                                                 /*!< FR    : type used for word access */
    __I _UART_FR_bits  FR_bit;                                       /*!< FR_bit: structure used for bit access */
  };
    __IO uint32_t Reserved1;
  union {                                                               /*!< IrDA Low-Power Counter Register */
    __IO uint32_t ILPR;                                              /*!< ILPR    : type used for word access */
    __IO _UART_ILPR_bits  ILPR_bit;                                  /*!< ILPR_bit: structure used for bit access */
  };
  union {                                                               /*!< Integer Baud Rate Register */
    __IO uint32_t IBRD;                                              /*!< IBRD    : type used for word access */
    __IO _UART_IBRD_bits  IBRD_bit;                                  /*!< IBRD_bit: structure used for bit access */
  };
  union {                                                               /*!< Fractional Baud Rate Register */
    __IO uint32_t FBRD;                                              /*!< FBRD    : type used for word access */
    __IO _UART_FBRD_bits  FBRD_bit;                                  /*!< FBRD_bit: structure used for bit access */
  };
  union {                                                               /*!< Line Control Register */
    __IO uint32_t LCRH;                                              /*!< LCRH    : type used for word access */
    __IO _UART_LCRH_bits  LCRH_bit;                                  /*!< LCRH_bit: structure used for bit access */
  };
  union {                                                               /*!< Control Register */
    __IO uint32_t CR;                                                /*!< CR    : type used for word access */
    __IO _UART_CR_bits  CR_bit;                                      /*!< CR_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt FIFO Level Select Register */
    __IO uint32_t IFLS;                                              /*!< IFLS    : type used for word access */
    __IO _UART_IFLS_bits  IFLS_bit;                                  /*!< IFLS_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt Mask Set/Clear Register */
    __IO uint32_t IMSC;                                              /*!< IMSC    : type used for word access */
    __IO _UART_IMSC_bits  IMSC_bit;                                  /*!< IMSC_bit: structure used for bit access */
  };
  union {                                                               /*!< Raw Interrupt Status Register */
    __I uint32_t RIS;                                                /*!< RIS    : type used for word access */
    __I _UART_RIS_bits  RIS_bit;                                     /*!< RIS_bit: structure used for bit access */
  };
  union {                                                               /*!< Masked Interrupt Status Register */
    __I uint32_t MIS;                                                /*!< MIS    : type used for word access */
    __I _UART_MIS_bits  MIS_bit;                                     /*!< MIS_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt Clear Register */
    __O uint32_t ICR;                                                /*!< ICR    : type used for word access */
    __O _UART_ICR_bits  ICR_bit;                                     /*!< ICR_bit: structure used for bit access */
  };
  union {                                                               /*!< DMA Control Register */
    __IO uint32_t DMACR;                                             /*!< DMACR    : type used for word access */
    __IO _UART_DMACR_bits  DMACR_bit;                                /*!< DMACR_bit: structure used for bit access */
  };
} UART_TypeDef;


/******************************************************************************/
/*                         DMA registers                                      */
/******************************************************************************/

/*--  STATUS: Status DMA register -----------------------------------------------------------------------------*/
typedef struct {
  uint32_t STATE                  :4;                                /*!< State of DMA */
  uint32_t MASTEREN               :1;                                /*!< Indicate enable DMA */
  uint32_t CHNLS                  :5;                                /*!< Number channel DMA (write: N-1) */
} _DMA_STATUS_bits;

/* Bit field positions: */
#define DMA_STATUS_STATE_Pos                  0                      /*!< State of DMA */
#define DMA_STATUS_MASTEREN_Pos               4                      /*!< Indicate enable DMA */
#define DMA_STATUS_CHNLS_Pos                  5                      /*!< Number channel DMA (write: N-1) */

/* Bit field masks: */
#define DMA_STATUS_STATE_Msk                  0x0000000FUL           /*!< State of DMA */
#define DMA_STATUS_MASTEREN_Msk               0x00000010UL           /*!< Indicate enable DMA */
#define DMA_STATUS_CHNLS_Msk                  0x000003E0UL           /*!< Number channel DMA (write: N-1) */

/* Bit field enums: */
typedef enum {
  DMA_STATUS_MASTEREN_Free                   = 0x0UL,                /*!< At rest */
  DMA_STATUS_MASTEREN_ReadConfigData         = 0x1UL,                /*!< Reading the config data structure */
  DMA_STATUS_MASTEREN_ReadSrcDataEndPtr      = 0x2UL,                /*!< Reading sourse data end pointer */
  DMA_STATUS_MASTEREN_ReadDstDataEndPtr      = 0x3UL,                /*!< Reading destination data end pointer */
  DMA_STATUS_MASTEREN_ReadSrcData            = 0x4UL,                /*!< Reading source data */
  DMA_STATUS_MASTEREN_WrireDstData           = 0x5UL,                /*!< Writing data to the destination */
  DMA_STATUS_MASTEREN_WaitReq                = 0x6UL,                /*!< Waiting for a request */
  DMA_STATUS_MASTEREN_WriteConfigData        = 0x7UL,                /*!< Write config structure of the channel */
  DMA_STATUS_MASTEREN_Pause                  = 0x8UL,                /*!< Suspended */
  DMA_STATUS_MASTEREN_Done                   = 0x9UL,                /*!< Executed */
  DMA_STATUS_MASTEREN_PeriphScatGath         = 0xAUL,                /*!< mode "peripheral scather-gather" */
} DMA_STATUS_MASTEREN_Enum;

typedef enum {
  DMA_STATUS_CHNLS_Free                      = 0x0UL,                /*!< At rest */
  DMA_STATUS_CHNLS_ReadConfigData            = 0x1UL,                /*!< Reading the config data structure */
  DMA_STATUS_CHNLS_ReadSrcDataEndPtr         = 0x2UL,                /*!< Reading sourse data end pointer */
  DMA_STATUS_CHNLS_ReadDstDataEndPtr         = 0x3UL,                /*!< Reading destination data end pointer */
  DMA_STATUS_CHNLS_ReadSrcData               = 0x4UL,                /*!< Reading source data */
  DMA_STATUS_CHNLS_WrireDstData              = 0x5UL,                /*!< Writing data to the destination */
  DMA_STATUS_CHNLS_WaitReq                   = 0x6UL,                /*!< Waiting for a request */
  DMA_STATUS_CHNLS_WriteConfigData           = 0x7UL,                /*!< Write config structure of the channel */
  DMA_STATUS_CHNLS_Pause                     = 0x8UL,                /*!< Suspended */
  DMA_STATUS_CHNLS_Done                      = 0x9UL,                /*!< Executed */
  DMA_STATUS_CHNLS_PeriphScatGath            = 0xAUL,                /*!< mode "peripheral scather-gather" */
} DMA_STATUS_CHNLS_Enum;

/*--  CFG: DMA configuration register -------------------------------------------------------------------------*/
typedef struct {
  uint32_t CHPROT                 :3;                                /*!< Sets the AHB-Lite protection */
  uint32_t MASTEREN               :1;                                /*!< Enable DMA */
} _DMA_CFG_bits;

/* Bit field positions: */
#define DMA_CFG_CHPROT_Pos                    0                      /*!< Sets the AHB-Lite protection */
#define DMA_CFG_MASTEREN_Pos                  3                      /*!< Enable DMA */

/* Bit field masks: */
#define DMA_CFG_CHPROT_Msk                    0x00000007UL           /*!< Sets the AHB-Lite protection */
#define DMA_CFG_MASTEREN_Msk                  0x00000008UL           /*!< Enable DMA */

/*--  BASEPTR: Channel control data base pointer --------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Base address of the primary control data */
} _DMA_BASEPTR_bits;

/* Bit field positions: */
#define DMA_BASEPTR_VAL_Pos                   0                      /*!< Base address of the primary control data */

/* Bit field masks: */
#define DMA_BASEPTR_VAL_Msk                   0xFFFFFFFFUL           /*!< Base address of the primary control data */

/*--  ALTBASEPTR: Channel alternate control data base pointer -------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Base address of alternative control data */
} _DMA_ALTBASEPTR_bits;

/* Bit field positions: */
#define DMA_ALTBASEPTR_VAL_Pos                0                      /*!< Base address of alternative control data */

/* Bit field masks: */
#define DMA_ALTBASEPTR_VAL_Msk                0xFFFFFFFFUL           /*!< Base address of alternative control data */

/*--  WAITONREQ: Channel wait on request status ---------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH1                    :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH2                    :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH3                    :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH4                    :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH5                    :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH6                    :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH7                    :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH8                    :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH9                    :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH10                   :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH11                   :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH12                   :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH13                   :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH14                   :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH15                   :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH16                   :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH17                   :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH18                   :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH19                   :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH20                   :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH21                   :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH22                   :1;                                /*!< Returns the status of the DMA request signals */
  uint32_t CH23                   :1;                                /*!< Returns the status of the DMA request signals */
} _DMA_WAITONREQ_bits;

/* Bit field positions: */
#define DMA_WAITONREQ_CH0_Pos                 0                      /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH1_Pos                 1                      /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH2_Pos                 2                      /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH3_Pos                 3                      /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH4_Pos                 4                      /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH5_Pos                 5                      /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH6_Pos                 6                      /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH7_Pos                 7                      /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH8_Pos                 8                      /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH9_Pos                 9                      /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH10_Pos                10                     /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH11_Pos                11                     /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH12_Pos                12                     /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH13_Pos                13                     /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH14_Pos                14                     /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH15_Pos                15                     /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH16_Pos                16                     /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH17_Pos                17                     /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH18_Pos                18                     /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH19_Pos                19                     /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH20_Pos                20                     /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH21_Pos                21                     /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH22_Pos                22                     /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH23_Pos                23                     /*!< Returns the status of the DMA request signals */

/* Bit field masks: */
#define DMA_WAITONREQ_CH0_Msk                 0x00000001UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH1_Msk                 0x00000002UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH2_Msk                 0x00000004UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH3_Msk                 0x00000008UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH4_Msk                 0x00000010UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH5_Msk                 0x00000020UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH6_Msk                 0x00000040UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH7_Msk                 0x00000080UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH8_Msk                 0x00000100UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH9_Msk                 0x00000200UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH10_Msk                0x00000400UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH11_Msk                0x00000800UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH12_Msk                0x00001000UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH13_Msk                0x00002000UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH14_Msk                0x00004000UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH15_Msk                0x00008000UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH16_Msk                0x00010000UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH17_Msk                0x00020000UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH18_Msk                0x00040000UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH19_Msk                0x00080000UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH20_Msk                0x00100000UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH21_Msk                0x00200000UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH22_Msk                0x00400000UL           /*!< Returns the status of the DMA request signals */
#define DMA_WAITONREQ_CH23_Msk                0x00800000UL           /*!< Returns the status of the DMA request signals */

/*--  SWREQ: Channel software request -------------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!< Set software request on channel */
  uint32_t CH1                    :1;                                /*!< Set software request on channel */
  uint32_t CH2                    :1;                                /*!< Set software request on channel */
  uint32_t CH3                    :1;                                /*!< Set software request on channel */
  uint32_t CH4                    :1;                                /*!< Set software request on channel */
  uint32_t CH5                    :1;                                /*!< Set software request on channel */
  uint32_t CH6                    :1;                                /*!< Set software request on channel */
  uint32_t CH7                    :1;                                /*!< Set software request on channel */
  uint32_t CH8                    :1;                                /*!< Set software request on channel */
  uint32_t CH9                    :1;                                /*!< Set software request on channel */
  uint32_t CH10                   :1;                                /*!< Set software request on channel */
  uint32_t CH11                   :1;                                /*!< Set software request on channel */
  uint32_t CH12                   :1;                                /*!< Set software request on channel */
  uint32_t CH13                   :1;                                /*!< Set software request on channel */
  uint32_t CH14                   :1;                                /*!< Set software request on channel */
  uint32_t CH15                   :1;                                /*!< Set software request on channel */
  uint32_t CH16                   :1;                                /*!< Set software request on channel */
  uint32_t CH17                   :1;                                /*!< Set software request on channel */
  uint32_t CH18                   :1;                                /*!< Set software request on channel */
  uint32_t CH19                   :1;                                /*!< Set software request on channel */
  uint32_t CH20                   :1;                                /*!< Set software request on channel */
  uint32_t CH21                   :1;                                /*!< Set software request on channel */
  uint32_t CH22                   :1;                                /*!< Set software request on channel */
  uint32_t CH23                   :1;                                /*!< Set software request on channel */
} _DMA_SWREQ_bits;

/* Bit field positions: */
#define DMA_SWREQ_CH0_Pos                     0                      /*!< Set software request on channel */
#define DMA_SWREQ_CH1_Pos                     1                      /*!< Set software request on channel */
#define DMA_SWREQ_CH2_Pos                     2                      /*!< Set software request on channel */
#define DMA_SWREQ_CH3_Pos                     3                      /*!< Set software request on channel */
#define DMA_SWREQ_CH4_Pos                     4                      /*!< Set software request on channel */
#define DMA_SWREQ_CH5_Pos                     5                      /*!< Set software request on channel */
#define DMA_SWREQ_CH6_Pos                     6                      /*!< Set software request on channel */
#define DMA_SWREQ_CH7_Pos                     7                      /*!< Set software request on channel */
#define DMA_SWREQ_CH8_Pos                     8                      /*!< Set software request on channel */
#define DMA_SWREQ_CH9_Pos                     9                      /*!< Set software request on channel */
#define DMA_SWREQ_CH10_Pos                    10                     /*!< Set software request on channel */
#define DMA_SWREQ_CH11_Pos                    11                     /*!< Set software request on channel */
#define DMA_SWREQ_CH12_Pos                    12                     /*!< Set software request on channel */
#define DMA_SWREQ_CH13_Pos                    13                     /*!< Set software request on channel */
#define DMA_SWREQ_CH14_Pos                    14                     /*!< Set software request on channel */
#define DMA_SWREQ_CH15_Pos                    15                     /*!< Set software request on channel */
#define DMA_SWREQ_CH16_Pos                    16                     /*!< Set software request on channel */
#define DMA_SWREQ_CH17_Pos                    17                     /*!< Set software request on channel */
#define DMA_SWREQ_CH18_Pos                    18                     /*!< Set software request on channel */
#define DMA_SWREQ_CH19_Pos                    19                     /*!< Set software request on channel */
#define DMA_SWREQ_CH20_Pos                    20                     /*!< Set software request on channel */
#define DMA_SWREQ_CH21_Pos                    21                     /*!< Set software request on channel */
#define DMA_SWREQ_CH22_Pos                    22                     /*!< Set software request on channel */
#define DMA_SWREQ_CH23_Pos                    23                     /*!< Set software request on channel */

/* Bit field masks: */
#define DMA_SWREQ_CH0_Msk                     0x00000001UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH1_Msk                     0x00000002UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH2_Msk                     0x00000004UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH3_Msk                     0x00000008UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH4_Msk                     0x00000010UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH5_Msk                     0x00000020UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH6_Msk                     0x00000040UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH7_Msk                     0x00000080UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH8_Msk                     0x00000100UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH9_Msk                     0x00000200UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH10_Msk                    0x00000400UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH11_Msk                    0x00000800UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH12_Msk                    0x00001000UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH13_Msk                    0x00002000UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH14_Msk                    0x00004000UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH15_Msk                    0x00008000UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH16_Msk                    0x00010000UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH17_Msk                    0x00020000UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH18_Msk                    0x00040000UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH19_Msk                    0x00080000UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH20_Msk                    0x00100000UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH21_Msk                    0x00200000UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH22_Msk                    0x00400000UL           /*!< Set software request on channel */
#define DMA_SWREQ_CH23_Msk                    0x00800000UL           /*!< Set software request on channel */

/*--  USEBURSTSET: Channel useburst set -----------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!< Enable single requests */
  uint32_t CH1                    :1;                                /*!< Enable single requests */
  uint32_t CH2                    :1;                                /*!< Enable single requests */
  uint32_t CH3                    :1;                                /*!< Enable single requests */
  uint32_t CH4                    :1;                                /*!< Enable single requests */
  uint32_t CH5                    :1;                                /*!< Enable single requests */
  uint32_t CH6                    :1;                                /*!< Enable single requests */
  uint32_t CH7                    :1;                                /*!< Enable single requests */
  uint32_t CH8                    :1;                                /*!< Enable single requests */
  uint32_t CH9                    :1;                                /*!< Enable single requests */
  uint32_t CH10                   :1;                                /*!< Enable single requests */
  uint32_t CH11                   :1;                                /*!< Enable single requests */
  uint32_t CH12                   :1;                                /*!< Enable single requests */
  uint32_t CH13                   :1;                                /*!< Enable single requests */
  uint32_t CH14                   :1;                                /*!< Enable single requests */
  uint32_t CH15                   :1;                                /*!< Enable single requests */
  uint32_t CH16                   :1;                                /*!< Enable single requests */
  uint32_t CH17                   :1;                                /*!< Enable single requests */
  uint32_t CH18                   :1;                                /*!< Enable single requests */
  uint32_t CH19                   :1;                                /*!< Enable single requests */
  uint32_t CH20                   :1;                                /*!< Enable single requests */
  uint32_t CH21                   :1;                                /*!< Enable single requests */
  uint32_t CH22                   :1;                                /*!< Enable single requests */
  uint32_t CH23                   :1;                                /*!< Enable single requests */
} _DMA_USEBURSTSET_bits;

/* Bit field positions: */
#define DMA_USEBURSTSET_CH0_Pos               0                      /*!< Enable single requests */
#define DMA_USEBURSTSET_CH1_Pos               1                      /*!< Enable single requests */
#define DMA_USEBURSTSET_CH2_Pos               2                      /*!< Enable single requests */
#define DMA_USEBURSTSET_CH3_Pos               3                      /*!< Enable single requests */
#define DMA_USEBURSTSET_CH4_Pos               4                      /*!< Enable single requests */
#define DMA_USEBURSTSET_CH5_Pos               5                      /*!< Enable single requests */
#define DMA_USEBURSTSET_CH6_Pos               6                      /*!< Enable single requests */
#define DMA_USEBURSTSET_CH7_Pos               7                      /*!< Enable single requests */
#define DMA_USEBURSTSET_CH8_Pos               8                      /*!< Enable single requests */
#define DMA_USEBURSTSET_CH9_Pos               9                      /*!< Enable single requests */
#define DMA_USEBURSTSET_CH10_Pos              10                     /*!< Enable single requests */
#define DMA_USEBURSTSET_CH11_Pos              11                     /*!< Enable single requests */
#define DMA_USEBURSTSET_CH12_Pos              12                     /*!< Enable single requests */
#define DMA_USEBURSTSET_CH13_Pos              13                     /*!< Enable single requests */
#define DMA_USEBURSTSET_CH14_Pos              14                     /*!< Enable single requests */
#define DMA_USEBURSTSET_CH15_Pos              15                     /*!< Enable single requests */
#define DMA_USEBURSTSET_CH16_Pos              16                     /*!< Enable single requests */
#define DMA_USEBURSTSET_CH17_Pos              17                     /*!< Enable single requests */
#define DMA_USEBURSTSET_CH18_Pos              18                     /*!< Enable single requests */
#define DMA_USEBURSTSET_CH19_Pos              19                     /*!< Enable single requests */
#define DMA_USEBURSTSET_CH20_Pos              20                     /*!< Enable single requests */
#define DMA_USEBURSTSET_CH21_Pos              21                     /*!< Enable single requests */
#define DMA_USEBURSTSET_CH22_Pos              22                     /*!< Enable single requests */
#define DMA_USEBURSTSET_CH23_Pos              23                     /*!< Enable single requests */

/* Bit field masks: */
#define DMA_USEBURSTSET_CH0_Msk               0x00000001UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH1_Msk               0x00000002UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH2_Msk               0x00000004UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH3_Msk               0x00000008UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH4_Msk               0x00000010UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH5_Msk               0x00000020UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH6_Msk               0x00000040UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH7_Msk               0x00000080UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH8_Msk               0x00000100UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH9_Msk               0x00000200UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH10_Msk              0x00000400UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH11_Msk              0x00000800UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH12_Msk              0x00001000UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH13_Msk              0x00002000UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH14_Msk              0x00004000UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH15_Msk              0x00008000UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH16_Msk              0x00010000UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH17_Msk              0x00020000UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH18_Msk              0x00040000UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH19_Msk              0x00080000UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH20_Msk              0x00100000UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH21_Msk              0x00200000UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH22_Msk              0x00400000UL           /*!< Enable single requests */
#define DMA_USEBURSTSET_CH23_Msk              0x00800000UL           /*!< Enable single requests */

/*--  USEBURSTCLR: Channel useburst clear ---------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!< Disable single requests */
  uint32_t CH1                    :1;                                /*!< Disable single requests */
  uint32_t CH2                    :1;                                /*!< Disable single requests */
  uint32_t CH3                    :1;                                /*!< Disable single requests */
  uint32_t CH4                    :1;                                /*!< Disable single requests */
  uint32_t CH5                    :1;                                /*!< Disable single requests */
  uint32_t CH6                    :1;                                /*!< Disable single requests */
  uint32_t CH7                    :1;                                /*!< Disable single requests */
  uint32_t CH8                    :1;                                /*!< Disable single requests */
  uint32_t CH9                    :1;                                /*!< Disable single requests */
  uint32_t CH10                   :1;                                /*!< Disable single requests */
  uint32_t CH11                   :1;                                /*!< Disable single requests */
  uint32_t CH12                   :1;                                /*!< Disable single requests */
  uint32_t CH13                   :1;                                /*!< Disable single requests */
  uint32_t CH14                   :1;                                /*!< Disable single requests */
  uint32_t CH15                   :1;                                /*!< Disable single requests */
  uint32_t CH16                   :1;                                /*!< Disable single requests */
  uint32_t CH17                   :1;                                /*!< Disable single requests */
  uint32_t CH18                   :1;                                /*!< Disable single requests */
  uint32_t CH19                   :1;                                /*!< Disable single requests */
  uint32_t CH20                   :1;                                /*!< Disable single requests */
  uint32_t CH21                   :1;                                /*!< Disable single requests */
  uint32_t CH22                   :1;                                /*!< Disable single requests */
  uint32_t CH23                   :1;                                /*!< Disable single requests */
} _DMA_USEBURSTCLR_bits;

/* Bit field positions: */
#define DMA_USEBURSTCLR_CH0_Pos               0                      /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH1_Pos               1                      /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH2_Pos               2                      /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH3_Pos               3                      /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH4_Pos               4                      /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH5_Pos               5                      /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH6_Pos               6                      /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH7_Pos               7                      /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH8_Pos               8                      /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH9_Pos               9                      /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH10_Pos              10                     /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH11_Pos              11                     /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH12_Pos              12                     /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH13_Pos              13                     /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH14_Pos              14                     /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH15_Pos              15                     /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH16_Pos              16                     /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH17_Pos              17                     /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH18_Pos              18                     /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH19_Pos              19                     /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH20_Pos              20                     /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH21_Pos              21                     /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH22_Pos              22                     /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH23_Pos              23                     /*!< Disable single requests */

/* Bit field masks: */
#define DMA_USEBURSTCLR_CH0_Msk               0x00000001UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH1_Msk               0x00000002UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH2_Msk               0x00000004UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH3_Msk               0x00000008UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH4_Msk               0x00000010UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH5_Msk               0x00000020UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH6_Msk               0x00000040UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH7_Msk               0x00000080UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH8_Msk               0x00000100UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH9_Msk               0x00000200UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH10_Msk              0x00000400UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH11_Msk              0x00000800UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH12_Msk              0x00001000UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH13_Msk              0x00002000UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH14_Msk              0x00004000UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH15_Msk              0x00008000UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH16_Msk              0x00010000UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH17_Msk              0x00020000UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH18_Msk              0x00040000UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH19_Msk              0x00080000UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH20_Msk              0x00100000UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH21_Msk              0x00200000UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH22_Msk              0x00400000UL           /*!< Disable single requests */
#define DMA_USEBURSTCLR_CH23_Msk              0x00800000UL           /*!< Disable single requests */

/*--  REQMASKSET: Channel request mask set --------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!< External requests are enabled for channel */
  uint32_t CH1                    :1;                                /*!< External requests are enabled for channel */
  uint32_t CH2                    :1;                                /*!< External requests are enabled for channel */
  uint32_t CH3                    :1;                                /*!< External requests are enabled for channel */
  uint32_t CH4                    :1;                                /*!< External requests are enabled for channel */
  uint32_t CH5                    :1;                                /*!< External requests are enabled for channel */
  uint32_t CH6                    :1;                                /*!< External requests are enabled for channel */
  uint32_t CH7                    :1;                                /*!< External requests are enabled for channel */
  uint32_t CH8                    :1;                                /*!< External requests are enabled for channel */
  uint32_t CH9                    :1;                                /*!< External requests are enabled for channel */
  uint32_t CH10                   :1;                                /*!< External requests are enabled for channel */
  uint32_t CH11                   :1;                                /*!< External requests are enabled for channel */
  uint32_t CH12                   :1;                                /*!< External requests are enabled for channel */
  uint32_t CH13                   :1;                                /*!< External requests are enabled for channel */
  uint32_t CH14                   :1;                                /*!< External requests are enabled for channel */
  uint32_t CH15                   :1;                                /*!< External requests are enabled for channel */
  uint32_t CH16                   :1;                                /*!< External requests are enabled for channel */
  uint32_t CH17                   :1;                                /*!< External requests are enabled for channel */
  uint32_t CH18                   :1;                                /*!< External requests are enabled for channel */
  uint32_t CH19                   :1;                                /*!< External requests are enabled for channel */
  uint32_t CH20                   :1;                                /*!< External requests are enabled for channel */
  uint32_t CH21                   :1;                                /*!< External requests are enabled for channel */
  uint32_t CH22                   :1;                                /*!< External requests are enabled for channel */
  uint32_t CH23                   :1;                                /*!< External requests are enabled for channel */
} _DMA_REQMASKSET_bits;

/* Bit field positions: */
#define DMA_REQMASKSET_CH0_Pos                0                      /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH1_Pos                1                      /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH2_Pos                2                      /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH3_Pos                3                      /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH4_Pos                4                      /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH5_Pos                5                      /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH6_Pos                6                      /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH7_Pos                7                      /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH8_Pos                8                      /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH9_Pos                9                      /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH10_Pos               10                     /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH11_Pos               11                     /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH12_Pos               12                     /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH13_Pos               13                     /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH14_Pos               14                     /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH15_Pos               15                     /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH16_Pos               16                     /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH17_Pos               17                     /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH18_Pos               18                     /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH19_Pos               19                     /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH20_Pos               20                     /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH21_Pos               21                     /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH22_Pos               22                     /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH23_Pos               23                     /*!< External requests are enabled for channel */

/* Bit field masks: */
#define DMA_REQMASKSET_CH0_Msk                0x00000001UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH1_Msk                0x00000002UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH2_Msk                0x00000004UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH3_Msk                0x00000008UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH4_Msk                0x00000010UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH5_Msk                0x00000020UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH6_Msk                0x00000040UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH7_Msk                0x00000080UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH8_Msk                0x00000100UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH9_Msk                0x00000200UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH10_Msk               0x00000400UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH11_Msk               0x00000800UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH12_Msk               0x00001000UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH13_Msk               0x00002000UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH14_Msk               0x00004000UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH15_Msk               0x00008000UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH16_Msk               0x00010000UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH17_Msk               0x00020000UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH18_Msk               0x00040000UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH19_Msk               0x00080000UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH20_Msk               0x00100000UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH21_Msk               0x00200000UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH22_Msk               0x00400000UL           /*!< External requests are enabled for channel */
#define DMA_REQMASKSET_CH23_Msk               0x00800000UL           /*!< External requests are enabled for channel */

/*--  REQMASKCLR: Channel request mask clear ------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!< External requests are disabled for channel */
  uint32_t CH1                    :1;                                /*!< External requests are disabled for channel */
  uint32_t CH2                    :1;                                /*!< External requests are disabled for channel */
  uint32_t CH3                    :1;                                /*!< External requests are disabled for channel */
  uint32_t CH4                    :1;                                /*!< External requests are disabled for channel */
  uint32_t CH5                    :1;                                /*!< External requests are disabled for channel */
  uint32_t CH6                    :1;                                /*!< External requests are disabled for channel */
  uint32_t CH7                    :1;                                /*!< External requests are disabled for channel */
  uint32_t CH8                    :1;                                /*!< External requests are disabled for channel */
  uint32_t CH9                    :1;                                /*!< External requests are disabled for channel */
  uint32_t CH10                   :1;                                /*!< External requests are disabled for channel */
  uint32_t CH11                   :1;                                /*!< External requests are disabled for channel */
  uint32_t CH12                   :1;                                /*!< External requests are disabled for channel */
  uint32_t CH13                   :1;                                /*!< External requests are disabled for channel */
  uint32_t CH14                   :1;                                /*!< External requests are disabled for channel */
  uint32_t CH15                   :1;                                /*!< External requests are disabled for channel */
  uint32_t CH16                   :1;                                /*!< External requests are disabled for channel */
  uint32_t CH17                   :1;                                /*!< External requests are disabled for channel */
  uint32_t CH18                   :1;                                /*!< External requests are disabled for channel */
  uint32_t CH19                   :1;                                /*!< External requests are disabled for channel */
  uint32_t CH20                   :1;                                /*!< External requests are disabled for channel */
  uint32_t CH21                   :1;                                /*!< External requests are disabled for channel */
  uint32_t CH22                   :1;                                /*!< External requests are disabled for channel */
  uint32_t CH23                   :1;                                /*!< External requests are disabled for channel */
} _DMA_REQMASKCLR_bits;

/* Bit field positions: */
#define DMA_REQMASKCLR_CH0_Pos                0                      /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH1_Pos                1                      /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH2_Pos                2                      /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH3_Pos                3                      /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH4_Pos                4                      /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH5_Pos                5                      /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH6_Pos                6                      /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH7_Pos                7                      /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH8_Pos                8                      /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH9_Pos                9                      /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH10_Pos               10                     /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH11_Pos               11                     /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH12_Pos               12                     /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH13_Pos               13                     /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH14_Pos               14                     /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH15_Pos               15                     /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH16_Pos               16                     /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH17_Pos               17                     /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH18_Pos               18                     /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH19_Pos               19                     /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH20_Pos               20                     /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH21_Pos               21                     /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH22_Pos               22                     /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH23_Pos               23                     /*!< External requests are disabled for channel */

/* Bit field masks: */
#define DMA_REQMASKCLR_CH0_Msk                0x00000001UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH1_Msk                0x00000002UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH2_Msk                0x00000004UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH3_Msk                0x00000008UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH4_Msk                0x00000010UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH5_Msk                0x00000020UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH6_Msk                0x00000040UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH7_Msk                0x00000080UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH8_Msk                0x00000100UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH9_Msk                0x00000200UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH10_Msk               0x00000400UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH11_Msk               0x00000800UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH12_Msk               0x00001000UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH13_Msk               0x00002000UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH14_Msk               0x00004000UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH15_Msk               0x00008000UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH16_Msk               0x00010000UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH17_Msk               0x00020000UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH18_Msk               0x00040000UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH19_Msk               0x00080000UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH20_Msk               0x00100000UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH21_Msk               0x00200000UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH22_Msk               0x00400000UL           /*!< External requests are disabled for channel */
#define DMA_REQMASKCLR_CH23_Msk               0x00800000UL           /*!< External requests are disabled for channel */

/*--  ENSET: Channel enable set -------------------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!< Enable channel */
  uint32_t CH1                    :1;                                /*!< Enable channel */
  uint32_t CH2                    :1;                                /*!< Enable channel */
  uint32_t CH3                    :1;                                /*!< Enable channel */
  uint32_t CH4                    :1;                                /*!< Enable channel */
  uint32_t CH5                    :1;                                /*!< Enable channel */
  uint32_t CH6                    :1;                                /*!< Enable channel */
  uint32_t CH7                    :1;                                /*!< Enable channel */
  uint32_t CH8                    :1;                                /*!< Enable channel */
  uint32_t CH9                    :1;                                /*!< Enable channel */
  uint32_t CH10                   :1;                                /*!< Enable channel */
  uint32_t CH11                   :1;                                /*!< Enable channel */
  uint32_t CH12                   :1;                                /*!< Enable channel */
  uint32_t CH13                   :1;                                /*!< Enable channel */
  uint32_t CH14                   :1;                                /*!< Enable channel */
  uint32_t CH15                   :1;                                /*!< Enable channel */
  uint32_t CH16                   :1;                                /*!< Enable channel */
  uint32_t CH17                   :1;                                /*!< Enable channel */
  uint32_t CH18                   :1;                                /*!< Enable channel */
  uint32_t CH19                   :1;                                /*!< Enable channel */
  uint32_t CH20                   :1;                                /*!< Enable channel */
  uint32_t CH21                   :1;                                /*!< Enable channel */
  uint32_t CH22                   :1;                                /*!< Enable channel */
  uint32_t CH23                   :1;                                /*!< Enable channel */
} _DMA_ENSET_bits;

/* Bit field positions: */
#define DMA_ENSET_CH0_Pos                     0                      /*!< Enable channel */
#define DMA_ENSET_CH1_Pos                     1                      /*!< Enable channel */
#define DMA_ENSET_CH2_Pos                     2                      /*!< Enable channel */
#define DMA_ENSET_CH3_Pos                     3                      /*!< Enable channel */
#define DMA_ENSET_CH4_Pos                     4                      /*!< Enable channel */
#define DMA_ENSET_CH5_Pos                     5                      /*!< Enable channel */
#define DMA_ENSET_CH6_Pos                     6                      /*!< Enable channel */
#define DMA_ENSET_CH7_Pos                     7                      /*!< Enable channel */
#define DMA_ENSET_CH8_Pos                     8                      /*!< Enable channel */
#define DMA_ENSET_CH9_Pos                     9                      /*!< Enable channel */
#define DMA_ENSET_CH10_Pos                    10                     /*!< Enable channel */
#define DMA_ENSET_CH11_Pos                    11                     /*!< Enable channel */
#define DMA_ENSET_CH12_Pos                    12                     /*!< Enable channel */
#define DMA_ENSET_CH13_Pos                    13                     /*!< Enable channel */
#define DMA_ENSET_CH14_Pos                    14                     /*!< Enable channel */
#define DMA_ENSET_CH15_Pos                    15                     /*!< Enable channel */
#define DMA_ENSET_CH16_Pos                    16                     /*!< Enable channel */
#define DMA_ENSET_CH17_Pos                    17                     /*!< Enable channel */
#define DMA_ENSET_CH18_Pos                    18                     /*!< Enable channel */
#define DMA_ENSET_CH19_Pos                    19                     /*!< Enable channel */
#define DMA_ENSET_CH20_Pos                    20                     /*!< Enable channel */
#define DMA_ENSET_CH21_Pos                    21                     /*!< Enable channel */
#define DMA_ENSET_CH22_Pos                    22                     /*!< Enable channel */
#define DMA_ENSET_CH23_Pos                    23                     /*!< Enable channel */

/* Bit field masks: */
#define DMA_ENSET_CH0_Msk                     0x00000001UL           /*!< Enable channel */
#define DMA_ENSET_CH1_Msk                     0x00000002UL           /*!< Enable channel */
#define DMA_ENSET_CH2_Msk                     0x00000004UL           /*!< Enable channel */
#define DMA_ENSET_CH3_Msk                     0x00000008UL           /*!< Enable channel */
#define DMA_ENSET_CH4_Msk                     0x00000010UL           /*!< Enable channel */
#define DMA_ENSET_CH5_Msk                     0x00000020UL           /*!< Enable channel */
#define DMA_ENSET_CH6_Msk                     0x00000040UL           /*!< Enable channel */
#define DMA_ENSET_CH7_Msk                     0x00000080UL           /*!< Enable channel */
#define DMA_ENSET_CH8_Msk                     0x00000100UL           /*!< Enable channel */
#define DMA_ENSET_CH9_Msk                     0x00000200UL           /*!< Enable channel */
#define DMA_ENSET_CH10_Msk                    0x00000400UL           /*!< Enable channel */
#define DMA_ENSET_CH11_Msk                    0x00000800UL           /*!< Enable channel */
#define DMA_ENSET_CH12_Msk                    0x00001000UL           /*!< Enable channel */
#define DMA_ENSET_CH13_Msk                    0x00002000UL           /*!< Enable channel */
#define DMA_ENSET_CH14_Msk                    0x00004000UL           /*!< Enable channel */
#define DMA_ENSET_CH15_Msk                    0x00008000UL           /*!< Enable channel */
#define DMA_ENSET_CH16_Msk                    0x00010000UL           /*!< Enable channel */
#define DMA_ENSET_CH17_Msk                    0x00020000UL           /*!< Enable channel */
#define DMA_ENSET_CH18_Msk                    0x00040000UL           /*!< Enable channel */
#define DMA_ENSET_CH19_Msk                    0x00080000UL           /*!< Enable channel */
#define DMA_ENSET_CH20_Msk                    0x00100000UL           /*!< Enable channel */
#define DMA_ENSET_CH21_Msk                    0x00200000UL           /*!< Enable channel */
#define DMA_ENSET_CH22_Msk                    0x00400000UL           /*!< Enable channel */
#define DMA_ENSET_CH23_Msk                    0x00800000UL           /*!< Enable channel */

/*--  ENCLR: Channel enable clear -----------------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!< Disable channel */
  uint32_t CH1                    :1;                                /*!< Disable channel */
  uint32_t CH2                    :1;                                /*!< Disable channel */
  uint32_t CH3                    :1;                                /*!< Disable channel */
  uint32_t CH4                    :1;                                /*!< Disable channel */
  uint32_t CH5                    :1;                                /*!< Disable channel */
  uint32_t CH6                    :1;                                /*!< Disable channel */
  uint32_t CH7                    :1;                                /*!< Disable channel */
  uint32_t CH8                    :1;                                /*!< Disable channel */
  uint32_t CH9                    :1;                                /*!< Disable channel */
  uint32_t CH10                   :1;                                /*!< Disable channel */
  uint32_t CH11                   :1;                                /*!< Disable channel */
  uint32_t CH12                   :1;                                /*!< Disable channel */
  uint32_t CH13                   :1;                                /*!< Disable channel */
  uint32_t CH14                   :1;                                /*!< Disable channel */
  uint32_t CH15                   :1;                                /*!< Disable channel */
  uint32_t CH16                   :1;                                /*!< Disable channel */
  uint32_t CH17                   :1;                                /*!< Disable channel */
  uint32_t CH18                   :1;                                /*!< Disable channel */
  uint32_t CH19                   :1;                                /*!< Disable channel */
  uint32_t CH20                   :1;                                /*!< Disable channel */
  uint32_t CH21                   :1;                                /*!< Disable channel */
  uint32_t CH22                   :1;                                /*!< Disable channel */
  uint32_t CH23                   :1;                                /*!< Disable channel */
} _DMA_ENCLR_bits;

/* Bit field positions: */
#define DMA_ENCLR_CH0_Pos                     0                      /*!< Disable channel */
#define DMA_ENCLR_CH1_Pos                     1                      /*!< Disable channel */
#define DMA_ENCLR_CH2_Pos                     2                      /*!< Disable channel */
#define DMA_ENCLR_CH3_Pos                     3                      /*!< Disable channel */
#define DMA_ENCLR_CH4_Pos                     4                      /*!< Disable channel */
#define DMA_ENCLR_CH5_Pos                     5                      /*!< Disable channel */
#define DMA_ENCLR_CH6_Pos                     6                      /*!< Disable channel */
#define DMA_ENCLR_CH7_Pos                     7                      /*!< Disable channel */
#define DMA_ENCLR_CH8_Pos                     8                      /*!< Disable channel */
#define DMA_ENCLR_CH9_Pos                     9                      /*!< Disable channel */
#define DMA_ENCLR_CH10_Pos                    10                     /*!< Disable channel */
#define DMA_ENCLR_CH11_Pos                    11                     /*!< Disable channel */
#define DMA_ENCLR_CH12_Pos                    12                     /*!< Disable channel */
#define DMA_ENCLR_CH13_Pos                    13                     /*!< Disable channel */
#define DMA_ENCLR_CH14_Pos                    14                     /*!< Disable channel */
#define DMA_ENCLR_CH15_Pos                    15                     /*!< Disable channel */
#define DMA_ENCLR_CH16_Pos                    16                     /*!< Disable channel */
#define DMA_ENCLR_CH17_Pos                    17                     /*!< Disable channel */
#define DMA_ENCLR_CH18_Pos                    18                     /*!< Disable channel */
#define DMA_ENCLR_CH19_Pos                    19                     /*!< Disable channel */
#define DMA_ENCLR_CH20_Pos                    20                     /*!< Disable channel */
#define DMA_ENCLR_CH21_Pos                    21                     /*!< Disable channel */
#define DMA_ENCLR_CH22_Pos                    22                     /*!< Disable channel */
#define DMA_ENCLR_CH23_Pos                    23                     /*!< Disable channel */

/* Bit field masks: */
#define DMA_ENCLR_CH0_Msk                     0x00000001UL           /*!< Disable channel */
#define DMA_ENCLR_CH1_Msk                     0x00000002UL           /*!< Disable channel */
#define DMA_ENCLR_CH2_Msk                     0x00000004UL           /*!< Disable channel */
#define DMA_ENCLR_CH3_Msk                     0x00000008UL           /*!< Disable channel */
#define DMA_ENCLR_CH4_Msk                     0x00000010UL           /*!< Disable channel */
#define DMA_ENCLR_CH5_Msk                     0x00000020UL           /*!< Disable channel */
#define DMA_ENCLR_CH6_Msk                     0x00000040UL           /*!< Disable channel */
#define DMA_ENCLR_CH7_Msk                     0x00000080UL           /*!< Disable channel */
#define DMA_ENCLR_CH8_Msk                     0x00000100UL           /*!< Disable channel */
#define DMA_ENCLR_CH9_Msk                     0x00000200UL           /*!< Disable channel */
#define DMA_ENCLR_CH10_Msk                    0x00000400UL           /*!< Disable channel */
#define DMA_ENCLR_CH11_Msk                    0x00000800UL           /*!< Disable channel */
#define DMA_ENCLR_CH12_Msk                    0x00001000UL           /*!< Disable channel */
#define DMA_ENCLR_CH13_Msk                    0x00002000UL           /*!< Disable channel */
#define DMA_ENCLR_CH14_Msk                    0x00004000UL           /*!< Disable channel */
#define DMA_ENCLR_CH15_Msk                    0x00008000UL           /*!< Disable channel */
#define DMA_ENCLR_CH16_Msk                    0x00010000UL           /*!< Disable channel */
#define DMA_ENCLR_CH17_Msk                    0x00020000UL           /*!< Disable channel */
#define DMA_ENCLR_CH18_Msk                    0x00040000UL           /*!< Disable channel */
#define DMA_ENCLR_CH19_Msk                    0x00080000UL           /*!< Disable channel */
#define DMA_ENCLR_CH20_Msk                    0x00100000UL           /*!< Disable channel */
#define DMA_ENCLR_CH21_Msk                    0x00200000UL           /*!< Disable channel */
#define DMA_ENCLR_CH22_Msk                    0x00400000UL           /*!< Disable channel */
#define DMA_ENCLR_CH23_Msk                    0x00800000UL           /*!< Disable channel */

/*--  PRIALTSET: Channel primary-alternate set ----------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH1                    :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH2                    :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH3                    :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH4                    :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH5                    :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH6                    :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH7                    :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH8                    :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH9                    :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH10                   :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH11                   :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH12                   :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH13                   :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH14                   :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH15                   :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH16                   :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH17                   :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH18                   :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH19                   :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH20                   :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH21                   :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH22                   :1;                                /*!< Set primary / alternate channel control data structure */
  uint32_t CH23                   :1;                                /*!< Set primary / alternate channel control data structure */
} _DMA_PRIALTSET_bits;

/* Bit field positions: */
#define DMA_PRIALTSET_CH0_Pos                 0                      /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH1_Pos                 1                      /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH2_Pos                 2                      /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH3_Pos                 3                      /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH4_Pos                 4                      /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH5_Pos                 5                      /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH6_Pos                 6                      /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH7_Pos                 7                      /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH8_Pos                 8                      /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH9_Pos                 9                      /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH10_Pos                10                     /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH11_Pos                11                     /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH12_Pos                12                     /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH13_Pos                13                     /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH14_Pos                14                     /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH15_Pos                15                     /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH16_Pos                16                     /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH17_Pos                17                     /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH18_Pos                18                     /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH19_Pos                19                     /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH20_Pos                20                     /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH21_Pos                21                     /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH22_Pos                22                     /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH23_Pos                23                     /*!< Set primary / alternate channel control data structure */

/* Bit field masks: */
#define DMA_PRIALTSET_CH0_Msk                 0x00000001UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH1_Msk                 0x00000002UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH2_Msk                 0x00000004UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH3_Msk                 0x00000008UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH4_Msk                 0x00000010UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH5_Msk                 0x00000020UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH6_Msk                 0x00000040UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH7_Msk                 0x00000080UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH8_Msk                 0x00000100UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH9_Msk                 0x00000200UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH10_Msk                0x00000400UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH11_Msk                0x00000800UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH12_Msk                0x00001000UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH13_Msk                0x00002000UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH14_Msk                0x00004000UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH15_Msk                0x00008000UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH16_Msk                0x00010000UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH17_Msk                0x00020000UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH18_Msk                0x00040000UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH19_Msk                0x00080000UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH20_Msk                0x00100000UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH21_Msk                0x00200000UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH22_Msk                0x00400000UL           /*!< Set primary / alternate channel control data structure */
#define DMA_PRIALTSET_CH23_Msk                0x00800000UL           /*!< Set primary / alternate channel control data structure */

/*--  PRIALTCLR: Channel primary-alternate clear --------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH1                    :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH2                    :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH3                    :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH4                    :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH5                    :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH6                    :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH7                    :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH8                    :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH9                    :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH10                   :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH11                   :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH12                   :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH13                   :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH14                   :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH15                   :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH16                   :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH17                   :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH18                   :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH19                   :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH20                   :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH21                   :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH22                   :1;                                /*!< Clear primary / alternate channel control data structure */
  uint32_t CH23                   :1;                                /*!< Clear primary / alternate channel control data structure */
} _DMA_PRIALTCLR_bits;

/* Bit field positions: */
#define DMA_PRIALTCLR_CH0_Pos                 0                      /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH1_Pos                 1                      /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH2_Pos                 2                      /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH3_Pos                 3                      /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH4_Pos                 4                      /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH5_Pos                 5                      /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH6_Pos                 6                      /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH7_Pos                 7                      /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH8_Pos                 8                      /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH9_Pos                 9                      /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH10_Pos                10                     /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH11_Pos                11                     /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH12_Pos                12                     /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH13_Pos                13                     /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH14_Pos                14                     /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH15_Pos                15                     /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH16_Pos                16                     /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH17_Pos                17                     /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH18_Pos                18                     /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH19_Pos                19                     /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH20_Pos                20                     /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH21_Pos                21                     /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH22_Pos                22                     /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH23_Pos                23                     /*!< Clear primary / alternate channel control data structure */

/* Bit field masks: */
#define DMA_PRIALTCLR_CH0_Msk                 0x00000001UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH1_Msk                 0x00000002UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH2_Msk                 0x00000004UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH3_Msk                 0x00000008UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH4_Msk                 0x00000010UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH5_Msk                 0x00000020UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH6_Msk                 0x00000040UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH7_Msk                 0x00000080UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH8_Msk                 0x00000100UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH9_Msk                 0x00000200UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH10_Msk                0x00000400UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH11_Msk                0x00000800UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH12_Msk                0x00001000UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH13_Msk                0x00002000UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH14_Msk                0x00004000UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH15_Msk                0x00008000UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH16_Msk                0x00010000UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH17_Msk                0x00020000UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH18_Msk                0x00040000UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH19_Msk                0x00080000UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH20_Msk                0x00100000UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH21_Msk                0x00200000UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH22_Msk                0x00400000UL           /*!< Clear primary / alternate channel control data structure */
#define DMA_PRIALTCLR_CH23_Msk                0x00800000UL           /*!< Clear primary / alternate channel control data structure */

/*--  PRIORITYSET: Channel priority set -----------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!< Set the priority of channel */
  uint32_t CH1                    :1;                                /*!< Set the priority of channel */
  uint32_t CH2                    :1;                                /*!< Set the priority of channel */
  uint32_t CH3                    :1;                                /*!< Set the priority of channel */
  uint32_t CH4                    :1;                                /*!< Set the priority of channel */
  uint32_t CH5                    :1;                                /*!< Set the priority of channel */
  uint32_t CH6                    :1;                                /*!< Set the priority of channel */
  uint32_t CH7                    :1;                                /*!< Set the priority of channel */
  uint32_t CH8                    :1;                                /*!< Set the priority of channel */
  uint32_t CH9                    :1;                                /*!< Set the priority of channel */
  uint32_t CH10                   :1;                                /*!< Set the priority of channel */
  uint32_t CH11                   :1;                                /*!< Set the priority of channel */
  uint32_t CH12                   :1;                                /*!< Set the priority of channel */
  uint32_t CH13                   :1;                                /*!< Set the priority of channel */
  uint32_t CH14                   :1;                                /*!< Set the priority of channel */
  uint32_t CH15                   :1;                                /*!< Set the priority of channel */
  uint32_t CH16                   :1;                                /*!< Set the priority of channel */
  uint32_t CH17                   :1;                                /*!< Set the priority of channel */
  uint32_t CH18                   :1;                                /*!< Set the priority of channel */
  uint32_t CH19                   :1;                                /*!< Set the priority of channel */
  uint32_t CH20                   :1;                                /*!< Set the priority of channel */
  uint32_t CH21                   :1;                                /*!< Set the priority of channel */
  uint32_t CH22                   :1;                                /*!< Set the priority of channel */
  uint32_t CH23                   :1;                                /*!< Set the priority of channel */
} _DMA_PRIORITYSET_bits;

/* Bit field positions: */
#define DMA_PRIORITYSET_CH0_Pos               0                      /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH1_Pos               1                      /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH2_Pos               2                      /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH3_Pos               3                      /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH4_Pos               4                      /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH5_Pos               5                      /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH6_Pos               6                      /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH7_Pos               7                      /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH8_Pos               8                      /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH9_Pos               9                      /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH10_Pos              10                     /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH11_Pos              11                     /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH12_Pos              12                     /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH13_Pos              13                     /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH14_Pos              14                     /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH15_Pos              15                     /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH16_Pos              16                     /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH17_Pos              17                     /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH18_Pos              18                     /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH19_Pos              19                     /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH20_Pos              20                     /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH21_Pos              21                     /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH22_Pos              22                     /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH23_Pos              23                     /*!< Set the priority of channel */

/* Bit field masks: */
#define DMA_PRIORITYSET_CH0_Msk               0x00000001UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH1_Msk               0x00000002UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH2_Msk               0x00000004UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH3_Msk               0x00000008UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH4_Msk               0x00000010UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH5_Msk               0x00000020UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH6_Msk               0x00000040UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH7_Msk               0x00000080UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH8_Msk               0x00000100UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH9_Msk               0x00000200UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH10_Msk              0x00000400UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH11_Msk              0x00000800UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH12_Msk              0x00001000UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH13_Msk              0x00002000UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH14_Msk              0x00004000UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH15_Msk              0x00008000UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH16_Msk              0x00010000UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH17_Msk              0x00020000UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH18_Msk              0x00040000UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH19_Msk              0x00080000UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH20_Msk              0x00100000UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH21_Msk              0x00200000UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH22_Msk              0x00400000UL           /*!< Set the priority of channel */
#define DMA_PRIORITYSET_CH23_Msk              0x00800000UL           /*!< Set the priority of channel */

/*--  PRIORITYCLR: Channel priority clear ---------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!< Clear the priority */
  uint32_t CH1                    :1;                                /*!< Clear the priority */
  uint32_t CH2                    :1;                                /*!< Clear the priority */
  uint32_t CH3                    :1;                                /*!< Clear the priority */
  uint32_t CH4                    :1;                                /*!< Clear the priority */
  uint32_t CH5                    :1;                                /*!< Clear the priority */
  uint32_t CH6                    :1;                                /*!< Clear the priority */
  uint32_t CH7                    :1;                                /*!< Clear the priority */
  uint32_t CH8                    :1;                                /*!< Clear the priority */
  uint32_t CH9                    :1;                                /*!< Clear the priority */
  uint32_t CH10                   :1;                                /*!< Clear the priority */
  uint32_t CH11                   :1;                                /*!< Clear the priority */
  uint32_t CH12                   :1;                                /*!< Clear the priority */
  uint32_t CH13                   :1;                                /*!< Clear the priority */
  uint32_t CH14                   :1;                                /*!< Clear the priority */
  uint32_t CH15                   :1;                                /*!< Clear the priority */
  uint32_t CH16                   :1;                                /*!< Clear the priority */
  uint32_t CH17                   :1;                                /*!< Clear the priority */
  uint32_t CH18                   :1;                                /*!< Clear the priority */
  uint32_t CH19                   :1;                                /*!< Clear the priority */
  uint32_t CH20                   :1;                                /*!< Clear the priority */
  uint32_t CH21                   :1;                                /*!< Clear the priority */
  uint32_t CH22                   :1;                                /*!< Clear the priority */
  uint32_t CH23                   :1;                                /*!< Clear the priority */
} _DMA_PRIORITYCLR_bits;

/* Bit field positions: */
#define DMA_PRIORITYCLR_CH0_Pos               0                      /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH1_Pos               1                      /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH2_Pos               2                      /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH3_Pos               3                      /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH4_Pos               4                      /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH5_Pos               5                      /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH6_Pos               6                      /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH7_Pos               7                      /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH8_Pos               8                      /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH9_Pos               9                      /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH10_Pos              10                     /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH11_Pos              11                     /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH12_Pos              12                     /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH13_Pos              13                     /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH14_Pos              14                     /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH15_Pos              15                     /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH16_Pos              16                     /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH17_Pos              17                     /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH18_Pos              18                     /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH19_Pos              19                     /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH20_Pos              20                     /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH21_Pos              21                     /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH22_Pos              22                     /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH23_Pos              23                     /*!< Clear the priority */

/* Bit field masks: */
#define DMA_PRIORITYCLR_CH0_Msk               0x00000001UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH1_Msk               0x00000002UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH2_Msk               0x00000004UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH3_Msk               0x00000008UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH4_Msk               0x00000010UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH5_Msk               0x00000020UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH6_Msk               0x00000040UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH7_Msk               0x00000080UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH8_Msk               0x00000100UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH9_Msk               0x00000200UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH10_Msk              0x00000400UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH11_Msk              0x00000800UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH12_Msk              0x00001000UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH13_Msk              0x00002000UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH14_Msk              0x00004000UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH15_Msk              0x00008000UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH16_Msk              0x00010000UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH17_Msk              0x00020000UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH18_Msk              0x00040000UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH19_Msk              0x00080000UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH20_Msk              0x00100000UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH21_Msk              0x00200000UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH22_Msk              0x00400000UL           /*!< Clear the priority */
#define DMA_PRIORITYCLR_CH23_Msk              0x00800000UL           /*!< Clear the priority */

/*--  CIRCULARSET: Channel circular set -----------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!< Set the channel circular mode */
  uint32_t CH1                    :1;                                /*!< Set the channel circular mode */
  uint32_t CH2                    :1;                                /*!< Set the channel circular mode */
  uint32_t CH3                    :1;                                /*!< Set the channel circular mode */
  uint32_t CH4                    :1;                                /*!< Set the channel circular mode */
  uint32_t CH5                    :1;                                /*!< Set the channel circular mode */
  uint32_t CH6                    :1;                                /*!< Set the channel circular mode */
  uint32_t CH7                    :1;                                /*!< Set the channel circular mode */
  uint32_t CH8                    :1;                                /*!< Set the channel circular mode */
  uint32_t CH9                    :1;                                /*!< Set the channel circular mode */
  uint32_t CH10                   :1;                                /*!< Set the channel circular mode */
  uint32_t CH11                   :1;                                /*!< Set the channel circular mode */
  uint32_t CH12                   :1;                                /*!< Set the channel circular mode */
  uint32_t CH13                   :1;                                /*!< Set the channel circular mode */
  uint32_t CH14                   :1;                                /*!< Set the channel circular mode */
  uint32_t CH15                   :1;                                /*!< Set the channel circular mode */
  uint32_t CH16                   :1;                                /*!< Set the channel circular mode */
  uint32_t CH17                   :1;                                /*!< Set the channel circular mode */
  uint32_t CH18                   :1;                                /*!< Set the channel circular mode */
  uint32_t CH19                   :1;                                /*!< Set the channel circular mode */
  uint32_t CH20                   :1;                                /*!< Set the channel circular mode */
  uint32_t CH21                   :1;                                /*!< Set the channel circular mode */
  uint32_t CH22                   :1;                                /*!< Set the channel circular mode */
  uint32_t CH23                   :1;                                /*!< Set the channel circular mode */
} _DMA_CIRCULARSET_bits;

/* Bit field positions: */
#define DMA_CIRCULARSET_CH0_Pos               0                      /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH1_Pos               1                      /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH2_Pos               2                      /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH3_Pos               3                      /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH4_Pos               4                      /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH5_Pos               5                      /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH6_Pos               6                      /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH7_Pos               7                      /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH8_Pos               8                      /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH9_Pos               9                      /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH10_Pos              10                     /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH11_Pos              11                     /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH12_Pos              12                     /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH13_Pos              13                     /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH14_Pos              14                     /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH15_Pos              15                     /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH16_Pos              16                     /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH17_Pos              17                     /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH18_Pos              18                     /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH19_Pos              19                     /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH20_Pos              20                     /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH21_Pos              21                     /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH22_Pos              22                     /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH23_Pos              23                     /*!< Set the channel circular mode */

/* Bit field masks: */
#define DMA_CIRCULARSET_CH0_Msk               0x00000001UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH1_Msk               0x00000002UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH2_Msk               0x00000004UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH3_Msk               0x00000008UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH4_Msk               0x00000010UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH5_Msk               0x00000020UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH6_Msk               0x00000040UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH7_Msk               0x00000080UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH8_Msk               0x00000100UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH9_Msk               0x00000200UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH10_Msk              0x00000400UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH11_Msk              0x00000800UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH12_Msk              0x00001000UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH13_Msk              0x00002000UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH14_Msk              0x00004000UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH15_Msk              0x00008000UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH16_Msk              0x00010000UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH17_Msk              0x00020000UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH18_Msk              0x00040000UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH19_Msk              0x00080000UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH20_Msk              0x00100000UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH21_Msk              0x00200000UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH22_Msk              0x00400000UL           /*!< Set the channel circular mode */
#define DMA_CIRCULARSET_CH23_Msk              0x00800000UL           /*!< Set the channel circular mode */

/*--  CIRCULARCLR: Channel circular clear ---------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!< Clear the channel circular mode */
  uint32_t CH1                    :1;                                /*!< Clear the channel circular mode */
  uint32_t CH2                    :1;                                /*!< Clear the channel circular mode */
  uint32_t CH3                    :1;                                /*!< Clear the channel circular mode */
  uint32_t CH4                    :1;                                /*!< Clear the channel circular mode */
  uint32_t CH5                    :1;                                /*!< Clear the channel circular mode */
  uint32_t CH6                    :1;                                /*!< Clear the channel circular mode */
  uint32_t CH7                    :1;                                /*!< Clear the channel circular mode */
  uint32_t CH8                    :1;                                /*!< Clear the channel circular mode */
  uint32_t CH9                    :1;                                /*!< Clear the channel circular mode */
  uint32_t CH10                   :1;                                /*!< Clear the channel circular mode */
  uint32_t CH11                   :1;                                /*!< Clear the channel circular mode */
  uint32_t CH12                   :1;                                /*!< Clear the channel circular mode */
  uint32_t CH13                   :1;                                /*!< Clear the channel circular mode */
  uint32_t CH14                   :1;                                /*!< Clear the channel circular mode */
  uint32_t CH15                   :1;                                /*!< Clear the channel circular mode */
  uint32_t CH16                   :1;                                /*!< Clear the channel circular mode */
  uint32_t CH17                   :1;                                /*!< Clear the channel circular mode */
  uint32_t CH18                   :1;                                /*!< Clear the channel circular mode */
  uint32_t CH19                   :1;                                /*!< Clear the channel circular mode */
  uint32_t CH20                   :1;                                /*!< Clear the channel circular mode */
  uint32_t CH21                   :1;                                /*!< Clear the channel circular mode */
  uint32_t CH22                   :1;                                /*!< Clear the channel circular mode */
  uint32_t CH23                   :1;                                /*!< Clear the channel circular mode */
} _DMA_CIRCULARCLR_bits;

/* Bit field positions: */
#define DMA_CIRCULARCLR_CH0_Pos               0                      /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH1_Pos               1                      /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH2_Pos               2                      /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH3_Pos               3                      /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH4_Pos               4                      /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH5_Pos               5                      /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH6_Pos               6                      /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH7_Pos               7                      /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH8_Pos               8                      /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH9_Pos               9                      /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH10_Pos              10                     /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH11_Pos              11                     /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH12_Pos              12                     /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH13_Pos              13                     /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH14_Pos              14                     /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH15_Pos              15                     /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH16_Pos              16                     /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH17_Pos              17                     /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH18_Pos              18                     /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH19_Pos              19                     /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH20_Pos              20                     /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH21_Pos              21                     /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH22_Pos              22                     /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH23_Pos              23                     /*!< Clear the channel circular mode */

/* Bit field masks: */
#define DMA_CIRCULARCLR_CH0_Msk               0x00000001UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH1_Msk               0x00000002UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH2_Msk               0x00000004UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH3_Msk               0x00000008UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH4_Msk               0x00000010UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH5_Msk               0x00000020UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH6_Msk               0x00000040UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH7_Msk               0x00000080UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH8_Msk               0x00000100UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH9_Msk               0x00000200UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH10_Msk              0x00000400UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH11_Msk              0x00000800UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH12_Msk              0x00001000UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH13_Msk              0x00002000UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH14_Msk              0x00004000UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH15_Msk              0x00008000UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH16_Msk              0x00010000UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH17_Msk              0x00020000UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH18_Msk              0x00040000UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH19_Msk              0x00080000UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH20_Msk              0x00100000UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH21_Msk              0x00200000UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH22_Msk              0x00400000UL           /*!< Clear the channel circular mode */
#define DMA_CIRCULARCLR_CH23_Msk              0x00800000UL           /*!< Clear the channel circular mode */

/*--  ERRCLR: Bus error register ------------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :1;                                /*!< Indicate Error on bus AHB-Lite */
} _DMA_ERRCLR_bits;

/* Bit field positions: */
#define DMA_ERRCLR_VAL_Pos                    0                      /*!< Indicate Error on bus AHB-Lite */

/* Bit field masks: */
#define DMA_ERRCLR_VAL_Msk                    0x00000001UL           /*!< Indicate Error on bus AHB-Lite */

/*--  IRQSTAT: IRQ Status register ----------------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!<  */
  uint32_t CH1                    :1;                                /*!<  */
  uint32_t CH2                    :1;                                /*!<  */
  uint32_t CH3                    :1;                                /*!<  */
  uint32_t CH4                    :1;                                /*!<  */
  uint32_t CH5                    :1;                                /*!<  */
  uint32_t CH6                    :1;                                /*!<  */
  uint32_t CH7                    :1;                                /*!<  */
  uint32_t CH8                    :1;                                /*!<  */
  uint32_t CH9                    :1;                                /*!<  */
  uint32_t CH10                   :1;                                /*!<  */
  uint32_t CH11                   :1;                                /*!<  */
  uint32_t CH12                   :1;                                /*!<  */
  uint32_t CH13                   :1;                                /*!<  */
  uint32_t CH14                   :1;                                /*!<  */
  uint32_t CH15                   :1;                                /*!<  */
  uint32_t CH16                   :1;                                /*!<  */
  uint32_t CH17                   :1;                                /*!<  */
  uint32_t CH18                   :1;                                /*!<  */
  uint32_t CH19                   :1;                                /*!<  */
  uint32_t CH20                   :1;                                /*!<  */
  uint32_t CH21                   :1;                                /*!<  */
  uint32_t CH22                   :1;                                /*!<  */
  uint32_t CH23                   :1;                                /*!<  */
} _DMA_IRQSTAT_bits;

/* Bit field positions: */
#define DMA_IRQSTAT_CH0_Pos                   0                      /*!<  */
#define DMA_IRQSTAT_CH1_Pos                   1                      /*!<  */
#define DMA_IRQSTAT_CH2_Pos                   2                      /*!<  */
#define DMA_IRQSTAT_CH3_Pos                   3                      /*!<  */
#define DMA_IRQSTAT_CH4_Pos                   4                      /*!<  */
#define DMA_IRQSTAT_CH5_Pos                   5                      /*!<  */
#define DMA_IRQSTAT_CH6_Pos                   6                      /*!<  */
#define DMA_IRQSTAT_CH7_Pos                   7                      /*!<  */
#define DMA_IRQSTAT_CH8_Pos                   8                      /*!<  */
#define DMA_IRQSTAT_CH9_Pos                   9                      /*!<  */
#define DMA_IRQSTAT_CH10_Pos                  10                     /*!<  */
#define DMA_IRQSTAT_CH11_Pos                  11                     /*!<  */
#define DMA_IRQSTAT_CH12_Pos                  12                     /*!<  */
#define DMA_IRQSTAT_CH13_Pos                  13                     /*!<  */
#define DMA_IRQSTAT_CH14_Pos                  14                     /*!<  */
#define DMA_IRQSTAT_CH15_Pos                  15                     /*!<  */
#define DMA_IRQSTAT_CH16_Pos                  16                     /*!<  */
#define DMA_IRQSTAT_CH17_Pos                  17                     /*!<  */
#define DMA_IRQSTAT_CH18_Pos                  18                     /*!<  */
#define DMA_IRQSTAT_CH19_Pos                  19                     /*!<  */
#define DMA_IRQSTAT_CH20_Pos                  20                     /*!<  */
#define DMA_IRQSTAT_CH21_Pos                  21                     /*!<  */
#define DMA_IRQSTAT_CH22_Pos                  22                     /*!<  */
#define DMA_IRQSTAT_CH23_Pos                  23                     /*!<  */

/* Bit field masks: */
#define DMA_IRQSTAT_CH0_Msk                   0x00000001UL           /*!<  */
#define DMA_IRQSTAT_CH1_Msk                   0x00000002UL           /*!<  */
#define DMA_IRQSTAT_CH2_Msk                   0x00000004UL           /*!<  */
#define DMA_IRQSTAT_CH3_Msk                   0x00000008UL           /*!<  */
#define DMA_IRQSTAT_CH4_Msk                   0x00000010UL           /*!<  */
#define DMA_IRQSTAT_CH5_Msk                   0x00000020UL           /*!<  */
#define DMA_IRQSTAT_CH6_Msk                   0x00000040UL           /*!<  */
#define DMA_IRQSTAT_CH7_Msk                   0x00000080UL           /*!<  */
#define DMA_IRQSTAT_CH8_Msk                   0x00000100UL           /*!<  */
#define DMA_IRQSTAT_CH9_Msk                   0x00000200UL           /*!<  */
#define DMA_IRQSTAT_CH10_Msk                  0x00000400UL           /*!<  */
#define DMA_IRQSTAT_CH11_Msk                  0x00000800UL           /*!<  */
#define DMA_IRQSTAT_CH12_Msk                  0x00001000UL           /*!<  */
#define DMA_IRQSTAT_CH13_Msk                  0x00002000UL           /*!<  */
#define DMA_IRQSTAT_CH14_Msk                  0x00004000UL           /*!<  */
#define DMA_IRQSTAT_CH15_Msk                  0x00008000UL           /*!<  */
#define DMA_IRQSTAT_CH16_Msk                  0x00010000UL           /*!<  */
#define DMA_IRQSTAT_CH17_Msk                  0x00020000UL           /*!<  */
#define DMA_IRQSTAT_CH18_Msk                  0x00040000UL           /*!<  */
#define DMA_IRQSTAT_CH19_Msk                  0x00080000UL           /*!<  */
#define DMA_IRQSTAT_CH20_Msk                  0x00100000UL           /*!<  */
#define DMA_IRQSTAT_CH21_Msk                  0x00200000UL           /*!<  */
#define DMA_IRQSTAT_CH22_Msk                  0x00400000UL           /*!<  */
#define DMA_IRQSTAT_CH23_Msk                  0x00800000UL           /*!<  */

/*--  IRQSTATCLR: IRQ Clear Status register -------------------------------------------------------------------*/
typedef struct {
  uint32_t CH0                    :1;                                /*!<  */
  uint32_t CH1                    :1;                                /*!<  */
  uint32_t CH2                    :1;                                /*!<  */
  uint32_t CH3                    :1;                                /*!<  */
  uint32_t CH4                    :1;                                /*!<  */
  uint32_t CH5                    :1;                                /*!<  */
  uint32_t CH6                    :1;                                /*!<  */
  uint32_t CH7                    :1;                                /*!<  */
  uint32_t CH8                    :1;                                /*!<  */
  uint32_t CH9                    :1;                                /*!<  */
  uint32_t CH10                   :1;                                /*!<  */
  uint32_t CH11                   :1;                                /*!<  */
  uint32_t CH12                   :1;                                /*!<  */
  uint32_t CH13                   :1;                                /*!<  */
  uint32_t CH14                   :1;                                /*!<  */
  uint32_t CH15                   :1;                                /*!<  */
  uint32_t CH16                   :1;                                /*!<  */
  uint32_t CH17                   :1;                                /*!<  */
  uint32_t CH18                   :1;                                /*!<  */
  uint32_t CH19                   :1;                                /*!<  */
  uint32_t CH20                   :1;                                /*!<  */
  uint32_t CH21                   :1;                                /*!<  */
  uint32_t CH22                   :1;                                /*!<  */
  uint32_t CH23                   :1;                                /*!<  */
} _DMA_IRQSTATCLR_bits;

/* Bit field positions: */
#define DMA_IRQSTATCLR_CH0_Pos                0                      /*!<  */
#define DMA_IRQSTATCLR_CH1_Pos                1                      /*!<  */
#define DMA_IRQSTATCLR_CH2_Pos                2                      /*!<  */
#define DMA_IRQSTATCLR_CH3_Pos                3                      /*!<  */
#define DMA_IRQSTATCLR_CH4_Pos                4                      /*!<  */
#define DMA_IRQSTATCLR_CH5_Pos                5                      /*!<  */
#define DMA_IRQSTATCLR_CH6_Pos                6                      /*!<  */
#define DMA_IRQSTATCLR_CH7_Pos                7                      /*!<  */
#define DMA_IRQSTATCLR_CH8_Pos                8                      /*!<  */
#define DMA_IRQSTATCLR_CH9_Pos                9                      /*!<  */
#define DMA_IRQSTATCLR_CH10_Pos               10                     /*!<  */
#define DMA_IRQSTATCLR_CH11_Pos               11                     /*!<  */
#define DMA_IRQSTATCLR_CH12_Pos               12                     /*!<  */
#define DMA_IRQSTATCLR_CH13_Pos               13                     /*!<  */
#define DMA_IRQSTATCLR_CH14_Pos               14                     /*!<  */
#define DMA_IRQSTATCLR_CH15_Pos               15                     /*!<  */
#define DMA_IRQSTATCLR_CH16_Pos               16                     /*!<  */
#define DMA_IRQSTATCLR_CH17_Pos               17                     /*!<  */
#define DMA_IRQSTATCLR_CH18_Pos               18                     /*!<  */
#define DMA_IRQSTATCLR_CH19_Pos               19                     /*!<  */
#define DMA_IRQSTATCLR_CH20_Pos               20                     /*!<  */
#define DMA_IRQSTATCLR_CH21_Pos               21                     /*!<  */
#define DMA_IRQSTATCLR_CH22_Pos               22                     /*!<  */
#define DMA_IRQSTATCLR_CH23_Pos               23                     /*!<  */

/* Bit field masks: */
#define DMA_IRQSTATCLR_CH0_Msk                0x00000001UL           /*!<  */
#define DMA_IRQSTATCLR_CH1_Msk                0x00000002UL           /*!<  */
#define DMA_IRQSTATCLR_CH2_Msk                0x00000004UL           /*!<  */
#define DMA_IRQSTATCLR_CH3_Msk                0x00000008UL           /*!<  */
#define DMA_IRQSTATCLR_CH4_Msk                0x00000010UL           /*!<  */
#define DMA_IRQSTATCLR_CH5_Msk                0x00000020UL           /*!<  */
#define DMA_IRQSTATCLR_CH6_Msk                0x00000040UL           /*!<  */
#define DMA_IRQSTATCLR_CH7_Msk                0x00000080UL           /*!<  */
#define DMA_IRQSTATCLR_CH8_Msk                0x00000100UL           /*!<  */
#define DMA_IRQSTATCLR_CH9_Msk                0x00000200UL           /*!<  */
#define DMA_IRQSTATCLR_CH10_Msk               0x00000400UL           /*!<  */
#define DMA_IRQSTATCLR_CH11_Msk               0x00000800UL           /*!<  */
#define DMA_IRQSTATCLR_CH12_Msk               0x00001000UL           /*!<  */
#define DMA_IRQSTATCLR_CH13_Msk               0x00002000UL           /*!<  */
#define DMA_IRQSTATCLR_CH14_Msk               0x00004000UL           /*!<  */
#define DMA_IRQSTATCLR_CH15_Msk               0x00008000UL           /*!<  */
#define DMA_IRQSTATCLR_CH16_Msk               0x00010000UL           /*!<  */
#define DMA_IRQSTATCLR_CH17_Msk               0x00020000UL           /*!<  */
#define DMA_IRQSTATCLR_CH18_Msk               0x00040000UL           /*!<  */
#define DMA_IRQSTATCLR_CH19_Msk               0x00080000UL           /*!<  */
#define DMA_IRQSTATCLR_CH20_Msk               0x00100000UL           /*!<  */
#define DMA_IRQSTATCLR_CH21_Msk               0x00200000UL           /*!<  */
#define DMA_IRQSTATCLR_CH22_Msk               0x00400000UL           /*!<  */
#define DMA_IRQSTATCLR_CH23_Msk               0x00800000UL           /*!<  */

typedef struct {
  union {                                                               /*!< Status DMA register */
    __I uint32_t STATUS;                                             /*!< STATUS    : type used for word access */
    __I _DMA_STATUS_bits  STATUS_bit;                                /*!< STATUS_bit: structure used for bit access */
  };
  union {                                                               /*!< DMA configuration register */
    __IO uint32_t CFG;                                               /*!< CFG    : type used for word access */
    __IO _DMA_CFG_bits  CFG_bit;                                     /*!< CFG_bit: structure used for bit access */
  };
  union {                                                               /*!< Channel control data base pointer */
    __IO uint32_t BASEPTR;                                           /*!< BASEPTR    : type used for word access */
    __IO _DMA_BASEPTR_bits  BASEPTR_bit;                             /*!< BASEPTR_bit: structure used for bit access */
  };
  union {                                                               /*!< Channel alternate control data base pointer */
    __I uint32_t ALTBASEPTR;                                           /*!< ALTBASEPTR    : type used for word access */
    __I _DMA_ALTBASEPTR_bits  ALTBASEPTR_bit;                        /*!< ALTBASEPTR_bit: structure used for bit access */
  };
  union {                                                               /*!< Channel wait on request status */
    __I uint32_t WAITONREQ;                                           /*!< WAITONREQ    : type used for word access */
    __I _DMA_WAITONREQ_bits  WAITONREQ_bit;                          /*!< WAITONREQ_bit: structure used for bit access */
  };
  union {                                                               /*!< Channel software request */
    __O uint32_t SWREQ;                                              /*!< SWREQ    : type used for word access */
    __O _DMA_SWREQ_bits  SWREQ_bit;                                  /*!< SWREQ_bit: structure used for bit access */
  };
  union {                                                               /*!< Channel useburst set */
    __IO uint32_t USEBURSTSET;                                           /*!< USEBURSTSET    : type used for word access */
    __IO _DMA_USEBURSTSET_bits  USEBURSTSET_bit;                     /*!< USEBURSTSET_bit: structure used for bit access */
  };
  union {                                                               /*!< Channel useburst clear */
    __O uint32_t USEBURSTCLR;                                           /*!< USEBURSTCLR    : type used for word access */
    __O _DMA_USEBURSTCLR_bits  USEBURSTCLR_bit;                      /*!< USEBURSTCLR_bit: structure used for bit access */
  };
  union {                                                               /*!< Channel request mask set */
    __IO uint32_t REQMASKSET;                                           /*!< REQMASKSET    : type used for word access */
    __IO _DMA_REQMASKSET_bits  REQMASKSET_bit;                       /*!< REQMASKSET_bit: structure used for bit access */
  };
  union {                                                               /*!< Channel request mask clear */
    __O uint32_t REQMASKCLR;                                           /*!< REQMASKCLR    : type used for word access */
    __O _DMA_REQMASKCLR_bits  REQMASKCLR_bit;                        /*!< REQMASKCLR_bit: structure used for bit access */
  };
  union {                                                               /*!< Channel enable set */
    __IO uint32_t ENSET;                                             /*!< ENSET    : type used for word access */
    __IO _DMA_ENSET_bits  ENSET_bit;                                 /*!< ENSET_bit: structure used for bit access */
  };
  union {                                                               /*!< Channel enable clear */
    __O uint32_t ENCLR;                                              /*!< ENCLR    : type used for word access */
    __O _DMA_ENCLR_bits  ENCLR_bit;                                  /*!< ENCLR_bit: structure used for bit access */
  };
  union {                                                               /*!< Channel primary-alternate set */
    __IO uint32_t PRIALTSET;                                           /*!< PRIALTSET    : type used for word access */
    __IO _DMA_PRIALTSET_bits  PRIALTSET_bit;                         /*!< PRIALTSET_bit: structure used for bit access */
  };
  union {                                                               /*!< Channel primary-alternate clear */
    __O uint32_t PRIALTCLR;                                           /*!< PRIALTCLR    : type used for word access */
    __O _DMA_PRIALTCLR_bits  PRIALTCLR_bit;                          /*!< PRIALTCLR_bit: structure used for bit access */
  };
  union {                                                               /*!< Channel priority set */
    __IO uint32_t PRIORITYSET;                                           /*!< PRIORITYSET    : type used for word access */
    __IO _DMA_PRIORITYSET_bits  PRIORITYSET_bit;                     /*!< PRIORITYSET_bit: structure used for bit access */
  };
  union {                                                               /*!< Channel priority clear */
    __O uint32_t PRIORITYCLR;                                           /*!< PRIORITYCLR    : type used for word access */
    __O _DMA_PRIORITYCLR_bits  PRIORITYCLR_bit;                      /*!< PRIORITYCLR_bit: structure used for bit access */
  };
  union {                                                               /*!< Channel circular set */
    __IO uint32_t CIRCULARSET;                                           /*!< CIRCULARSET    : type used for word access */
    __IO _DMA_CIRCULARSET_bits  CIRCULARSET_bit;                     /*!< CIRCULARSET_bit: structure used for bit access */
  };
  union {                                                               /*!< Channel circular clear */
    __O uint32_t CIRCULARCLR;                                           /*!< CIRCULARCLR    : type used for word access */
    __O _DMA_CIRCULARCLR_bits  CIRCULARCLR_bit;                      /*!< CIRCULARCLR_bit: structure used for bit access */
  };
    __IO uint32_t Reserved0;
  union {                                                               /*!< Bus error register */
    __IO uint32_t ERRCLR;                                            /*!< ERRCLR    : type used for word access */
    __IO _DMA_ERRCLR_bits  ERRCLR_bit;                               /*!< ERRCLR_bit: structure used for bit access */
  };
  union {                                                               /*!< IRQ Status register */
    __I uint32_t IRQSTAT;                                            /*!< IRQSTAT    : type used for word access */
    __I _DMA_IRQSTAT_bits  IRQSTAT_bit;                              /*!< IRQSTAT_bit: structure used for bit access */
  };
  union {                                                               /*!< IRQ Clear Status register */
    __O uint32_t IRQSTATCLR;                                           /*!< IRQSTATCLR    : type used for word access */
    __O _DMA_IRQSTATCLR_bits  IRQSTATCLR_bit;                        /*!< IRQSTATCLR_bit: structure used for bit access */
  };
} DMA_TypeDef;


/******************************************************************************/
/*                         FLASH registers                                    */
/******************************************************************************/

/*--  ADDR: Address Register ----------------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :19;                               /*!< Address value for flash operations */
} _FLASH_ADDR_bits;

/* Bit field positions: */
#define FLASH_ADDR_VAL_Pos                    0                      /*!< Address value for flash operations */

/* Bit field masks: */
#define FLASH_ADDR_VAL_Msk                    0x0007FFFFUL           /*!< Address value for flash operations */

/*--  DATA: Data Register -------------------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Data register value for flash operations */
} _FLASH_DATA_bits;

/* Bit field positions: */
#define FLASH_DATA_VAL_Pos                    0                      /*!< Data register value for flash operations */

/* Bit field masks: */
#define FLASH_DATA_VAL_Msk                    0xFFFFFFFFUL           /*!< Data register value for flash operations */

/*--  CMD: Command Register -----------------------------------------------------------------------------------*/
typedef struct {
  uint32_t RD                     :1;                                /*!< Read enable command */
  uint32_t WR                     :1;                                /*!< Write enable command */
  uint32_t ERSEC                  :1;                                /*!< Erase sector enable command */
  uint32_t ALLSEC                 :1;                                /*!< Enable Erase all flash memory */
  uint32_t                        :4;                                /*!< RESERVED */
  uint32_t NVRON                  :1;                                /*!< INFO access bit */
  uint32_t                        :7;                                /*!< RESERVED */
  uint32_t KEY                    :16;                               /*!< Magic Key for flash access "C0DE" */
} _FLASH_CMD_bits;

/* Bit field positions: */
#define FLASH_CMD_RD_Pos                      0                      /*!< Read enable command */
#define FLASH_CMD_WR_Pos                      1                      /*!< Write enable command */
#define FLASH_CMD_ERSEC_Pos                   2                      /*!< Erase sector enable command */
#define FLASH_CMD_ALLSEC_Pos                  3                      /*!< Enable Erase all flash memory */
#define FLASH_CMD_NVRON_Pos                   8                      /*!< INFO access bit */
#define FLASH_CMD_KEY_Pos                     16                     /*!< Magic Key for flash access "C0DE" */

/* Bit field masks: */
#define FLASH_CMD_RD_Msk                      0x00000001UL           /*!< Read enable command */
#define FLASH_CMD_WR_Msk                      0x00000002UL           /*!< Write enable command */
#define FLASH_CMD_ERSEC_Msk                   0x00000004UL           /*!< Erase sector enable command */
#define FLASH_CMD_ALLSEC_Msk                  0x00000008UL           /*!< Enable Erase all flash memory */
#define FLASH_CMD_NVRON_Msk                   0x00000100UL           /*!< INFO access bit */
#define FLASH_CMD_KEY_Msk                     0xFFFF0000UL           /*!< Magic Key for flash access "C0DE" */

/* Bit field enums: */
typedef enum {
  FLASH_CMD_KEY_Access                       = 0xC0DEUL,             /*!< magic Key for flash access */
} FLASH_CMD_KEY_Enum;

/*--  STAT: Status Register -----------------------------------------------------------------------------------*/
typedef struct {
  uint32_t BUSY                   :1;                                /*!< Busy status bit when command is processing */
  uint32_t IRQF                   :1;                                /*!< IRQ Flag set when command done. Set by hardware only if IRQEN bit is set. */
} _FLASH_STAT_bits;

/* Bit field positions: */
#define FLASH_STAT_BUSY_Pos                   0                      /*!< Busy status bit when command is processing */
#define FLASH_STAT_IRQF_Pos                   1                      /*!< IRQ Flag set when command done. Set by hardware only if IRQEN bit is set. */

/* Bit field masks: */
#define FLASH_STAT_BUSY_Msk                   0x00000001UL           /*!< Busy status bit when command is processing */
#define FLASH_STAT_IRQF_Msk                   0x00000002UL           /*!< IRQ Flag set when command done. Set by hardware only if IRQEN bit is set. */

/*--  CTRL: Control Register ----------------------------------------------------------------------------------*/
typedef struct {
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t CEN                    :1;                                /*!< Cache enable bit */
  uint32_t                        :6;                                /*!< RESERVED */
  uint32_t CFLUSH                 :1;                                /*!< Cache bit */
  uint32_t                        :7;                                /*!< RESERVED */
  uint32_t LAT                    :4;                                /*!< Flash latency */
} _FLASH_CTRL_bits;

/* Bit field positions: */
#define FLASH_CTRL_CEN_Pos                    1                      /*!< Cache enable bit */
#define FLASH_CTRL_CFLUSH_Pos                 8                      /*!< Cache bit */
#define FLASH_CTRL_LAT_Pos                    16                     /*!< Flash latency */

/* Bit field masks: */
#define FLASH_CTRL_CEN_Msk                    0x00000002UL           /*!< Cache enable bit */
#define FLASH_CTRL_CFLUSH_Msk                 0x00000100UL           /*!< Cache bit */
#define FLASH_CTRL_LAT_Msk                    0x000F0000UL           /*!< Flash latency */

//Cluster DATA:
typedef struct {
  union { 
  /*!< Data Register */
    __IO uint32_t DATA;                                              /*!< DATA : type used for word access */
    __IO _FLASH_DATA_bits DATA_bit;                                  /*!< DATA_bit: structure used for bit access */
  };
} _FLASH_DATA_TypeDef;
typedef struct {
  union {                                                               /*!< Address Register */
    __IO uint32_t ADDR;                                              /*!< ADDR    : type used for word access */
    __IO _FLASH_ADDR_bits  ADDR_bit;                                 /*!< ADDR_bit: structure used for bit access */
  };
  _FLASH_DATA_TypeDef DATA[4];
    __IO uint32_t Reserved0[12];
  union {                                                               /*!< Command Register */
    __IO uint32_t CMD;                                               /*!< CMD    : type used for word access */
    __IO _FLASH_CMD_bits  CMD_bit;                                   /*!< CMD_bit: structure used for bit access */
  };
  union {                                                               /*!< Status Register */
    __IO uint32_t STAT;                                              /*!< STAT    : type used for word access */
    __IO _FLASH_STAT_bits  STAT_bit;                                 /*!< STAT_bit: structure used for bit access */
  };
  union {                                                               /*!< Control Register */
    __IO uint32_t CTRL;                                              /*!< CTRL    : type used for word access */
    __IO _FLASH_CTRL_bits  CTRL_bit;                                 /*!< CTRL_bit: structure used for bit access */
  };
} FLASH_TypeDef;


/******************************************************************************/
/*                         SPI registers                                      */
/******************************************************************************/

/*--  CR0: Control register 0 ---------------------------------------------------------------------------------*/
typedef struct {
  uint32_t DSS                    :4;                                /*!< Size of data */
  uint32_t FRF                    :2;                                /*!< Select protocol */
  uint32_t SPO                    :1;                                /*!< Polarity SSPCLKOUT */
  uint32_t SPH                    :1;                                /*!< Phase SSPCLKOUT */
  uint32_t SCR                    :8;                                /*!< Value divider */
} _SPI_CR0_bits;

/* Bit field positions: */
#define SPI_CR0_DSS_Pos                       0                      /*!< Size of data */
#define SPI_CR0_FRF_Pos                       4                      /*!< Select protocol */
#define SPI_CR0_SPO_Pos                       6                      /*!< Polarity SSPCLKOUT */
#define SPI_CR0_SPH_Pos                       7                      /*!< Phase SSPCLKOUT */
#define SPI_CR0_SCR_Pos                       8                      /*!< Value divider */

/* Bit field masks: */
#define SPI_CR0_DSS_Msk                       0x0000000FUL           /*!< Size of data */
#define SPI_CR0_FRF_Msk                       0x00000030UL           /*!< Select protocol */
#define SPI_CR0_SPO_Msk                       0x00000040UL           /*!< Polarity SSPCLKOUT */
#define SPI_CR0_SPH_Msk                       0x00000080UL           /*!< Phase SSPCLKOUT */
#define SPI_CR0_SCR_Msk                       0x0000FF00UL           /*!< Value divider */

/* Bit field enums: */
typedef enum {
  SPI_CR0_DSS_4bit                           = 0x3UL,                /*!< data size 4 bit */
  SPI_CR0_DSS_5bit                           = 0x4UL,                /*!< data size 5 bit */
  SPI_CR0_DSS_6bit                           = 0x5UL,                /*!< data size 6 bit */
  SPI_CR0_DSS_7bit                           = 0x6UL,                /*!< data size 7 bit */
  SPI_CR0_DSS_8bit                           = 0x7UL,                /*!< data size 8 bit */
  SPI_CR0_DSS_9bit                           = 0x8UL,                /*!< data size 9 bit */
  SPI_CR0_DSS_10bit                          = 0x9UL,                /*!< data size 10 bit */
  SPI_CR0_DSS_11bit                          = 0xAUL,                /*!< data size 11 bit */
  SPI_CR0_DSS_12bit                          = 0xBUL,                /*!< data size 12 bit */
  SPI_CR0_DSS_13bit                          = 0xCUL,                /*!< data size 13 bit */
  SPI_CR0_DSS_14bit                          = 0xDUL,                /*!< data size 14 bit */
  SPI_CR0_DSS_15bit                          = 0xEUL,                /*!< data size 15 bit */
  SPI_CR0_DSS_16bit                          = 0xFUL,                /*!< data size 16 bit */
} SPI_CR0_DSS_Enum;

typedef enum {
  SPI_CR0_FRF_SPI                            = 0x0UL,                /*!< SPI of Motorola */
  SPI_CR0_FRF_SSI                            = 0x1UL,                /*!< SSI of Texas Instruments */
  SPI_CR0_FRF_Microwire                      = 0x2UL,                /*!< Microwire of National Semiconductor */
} SPI_CR0_FRF_Enum;

/*--  CR1: Control register 1 ---------------------------------------------------------------------------------*/
typedef struct {
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t SSE                    :1;                                /*!< Enable transceiver */
  uint32_t MS                     :1;                                /*!< Select mode */
  uint32_t SOD                    :1;                                /*!< Disable bit data */
  uint32_t                        :4;                                /*!< RESERVED */
  uint32_t RXIFLSEL               :4;                                /*!< Receive interrupt FIFO level select */
  uint32_t TXIFLSEL               :4;                                /*!< Transmit interrupt FIFO level select */
} _SPI_CR1_bits;

/* Bit field positions: */
#define SPI_CR1_SSE_Pos                       1                      /*!< Enable transceiver */
#define SPI_CR1_MS_Pos                        2                      /*!< Select mode */
#define SPI_CR1_SOD_Pos                       3                      /*!< Disable bit data */
#define SPI_CR1_RXIFLSEL_Pos                  8                      /*!< Receive interrupt FIFO level select */
#define SPI_CR1_TXIFLSEL_Pos                  12                     /*!< Transmit interrupt FIFO level select */

/* Bit field masks: */
#define SPI_CR1_SSE_Msk                       0x00000002UL           /*!< Enable transceiver */
#define SPI_CR1_MS_Msk                        0x00000004UL           /*!< Select mode */
#define SPI_CR1_SOD_Msk                       0x00000008UL           /*!< Disable bit data */
#define SPI_CR1_RXIFLSEL_Msk                  0x00000F00UL           /*!< Receive interrupt FIFO level select */
#define SPI_CR1_TXIFLSEL_Msk                  0x0000F000UL           /*!< Transmit interrupt FIFO level select */

/*--  DR: Data register ---------------------------------------------------------------------------------------*/
typedef struct {
  uint32_t DATA                   :16;                               /*!<  */
} _SPI_DR_bits;

/* Bit field positions: */
#define SPI_DR_DATA_Pos                       0                      /*!<  */

/* Bit field masks: */
#define SPI_DR_DATA_Msk                       0x0000FFFFUL           /*!<  */

/*--  SR: State register --------------------------------------------------------------------------------------*/
typedef struct {
  uint32_t TFE                    :1;                                /*!< FIFO buffer empty flag transmitter */
  uint32_t TNF                    :1;                                /*!< Indicator the transmitter FIFO buffer is not full */
  uint32_t RNE                    :1;                                /*!< Indicate not empty receive buffer */
  uint32_t RFF                    :1;                                /*!< Indicate full receive buffer */
  uint32_t BSY                    :1;                                /*!< Activity flag */
} _SPI_SR_bits;

/* Bit field positions: */
#define SPI_SR_TFE_Pos                        0                      /*!< FIFO buffer empty flag transmitter */
#define SPI_SR_TNF_Pos                        1                      /*!< Indicator the transmitter FIFO buffer is not full */
#define SPI_SR_RNE_Pos                        2                      /*!< Indicate not empty receive buffer */
#define SPI_SR_RFF_Pos                        3                      /*!< Indicate full receive buffer */
#define SPI_SR_BSY_Pos                        4                      /*!< Activity flag */

/* Bit field masks: */
#define SPI_SR_TFE_Msk                        0x00000001UL           /*!< FIFO buffer empty flag transmitter */
#define SPI_SR_TNF_Msk                        0x00000002UL           /*!< Indicator the transmitter FIFO buffer is not full */
#define SPI_SR_RNE_Msk                        0x00000004UL           /*!< Indicate not empty receive buffer */
#define SPI_SR_RFF_Msk                        0x00000008UL           /*!< Indicate full receive buffer */
#define SPI_SR_BSY_Msk                        0x00000010UL           /*!< Activity flag */

/*--  CPSR: Clock division factor register --------------------------------------------------------------------*/
typedef struct {
  uint32_t CPSDVSR                :8;                                /*!< Clock division factor. Bit0 always 0 */
} _SPI_CPSR_bits;

/* Bit field positions: */
#define SPI_CPSR_CPSDVSR_Pos                  0                      /*!< Clock division factor. Bit0 always 0 */

/* Bit field masks: */
#define SPI_CPSR_CPSDVSR_Msk                  0x000000FFUL           /*!< Clock division factor. Bit0 always 0 */

/*--  IMSC: Mask interrupt register ---------------------------------------------------------------------------*/
typedef struct {
  uint32_t RORIM                  :1;                                /*!< Interrupt mask bit SSPRORINTR buffer overflow receiver */
  uint32_t RTIM                   :1;                                /*!< Interrupt mask bit SSPRTINTR timeout receiver */
  uint32_t RXIM                   :1;                                /*!< SSPRXINTR interrupt mask bit to fill 50% or less of the receiver FIFO buffer */
  uint32_t TXIM                   :1;                                /*!< SSPTXINTR interrupt mask bit to fill 50% or less of the FIFO buffer of the transmitter */
} _SPI_IMSC_bits;

/* Bit field positions: */
#define SPI_IMSC_RORIM_Pos                    0                      /*!< Interrupt mask bit SSPRORINTR buffer overflow receiver */
#define SPI_IMSC_RTIM_Pos                     1                      /*!< Interrupt mask bit SSPRTINTR timeout receiver */
#define SPI_IMSC_RXIM_Pos                     2                      /*!< SSPRXINTR interrupt mask bit to fill 50% or less of the receiver FIFO buffer */
#define SPI_IMSC_TXIM_Pos                     3                      /*!< SSPTXINTR interrupt mask bit to fill 50% or less of the FIFO buffer of the transmitter */

/* Bit field masks: */
#define SPI_IMSC_RORIM_Msk                    0x00000001UL           /*!< Interrupt mask bit SSPRORINTR buffer overflow receiver */
#define SPI_IMSC_RTIM_Msk                     0x00000002UL           /*!< Interrupt mask bit SSPRTINTR timeout receiver */
#define SPI_IMSC_RXIM_Msk                     0x00000004UL           /*!< SSPRXINTR interrupt mask bit to fill 50% or less of the receiver FIFO buffer */
#define SPI_IMSC_TXIM_Msk                     0x00000008UL           /*!< SSPTXINTR interrupt mask bit to fill 50% or less of the FIFO buffer of the transmitter */

/*--  RIS: Status register interrupt without mask -------------------------------------------------------------*/
typedef struct {
  uint32_t RORRIS                 :1;                                /*!< Interrupt status before masking SSPRORINTR */
  uint32_t RTRIS                  :1;                                /*!< Interrupt status before masking SSPRTINTR */
  uint32_t RXRIS                  :1;                                /*!< Interrupt status before masking SSPRXINTR */
  uint32_t TXRIS                  :1;                                /*!< Interrupt status before masking SSPTXINTR */
} _SPI_RIS_bits;

/* Bit field positions: */
#define SPI_RIS_RORRIS_Pos                    0                      /*!< Interrupt status before masking SSPRORINTR */
#define SPI_RIS_RTRIS_Pos                     1                      /*!< Interrupt status before masking SSPRTINTR */
#define SPI_RIS_RXRIS_Pos                     2                      /*!< Interrupt status before masking SSPRXINTR */
#define SPI_RIS_TXRIS_Pos                     3                      /*!< Interrupt status before masking SSPTXINTR */

/* Bit field masks: */
#define SPI_RIS_RORRIS_Msk                    0x00000001UL           /*!< Interrupt status before masking SSPRORINTR */
#define SPI_RIS_RTRIS_Msk                     0x00000002UL           /*!< Interrupt status before masking SSPRTINTR */
#define SPI_RIS_RXRIS_Msk                     0x00000004UL           /*!< Interrupt status before masking SSPRXINTR */
#define SPI_RIS_TXRIS_Msk                     0x00000008UL           /*!< Interrupt status before masking SSPTXINTR */

/*--  MIS: Status register interrupt masking account ----------------------------------------------------------*/
typedef struct {
  uint32_t RORMIS                 :1;                                /*!< Masked interrupt status SSPRORINTR */
  uint32_t RTMIS                  :1;                                /*!< Masked interrupt status SSPRTINTR */
  uint32_t RXMIS                  :1;                                /*!< Masked interrupt status SSPRXINTR */
  uint32_t TXMIS                  :1;                                /*!< Masked interrupt status SSPTXINTR */
} _SPI_MIS_bits;

/* Bit field positions: */
#define SPI_MIS_RORMIS_Pos                    0                      /*!< Masked interrupt status SSPRORINTR */
#define SPI_MIS_RTMIS_Pos                     1                      /*!< Masked interrupt status SSPRTINTR */
#define SPI_MIS_RXMIS_Pos                     2                      /*!< Masked interrupt status SSPRXINTR */
#define SPI_MIS_TXMIS_Pos                     3                      /*!< Masked interrupt status SSPTXINTR */

/* Bit field masks: */
#define SPI_MIS_RORMIS_Msk                    0x00000001UL           /*!< Masked interrupt status SSPRORINTR */
#define SPI_MIS_RTMIS_Msk                     0x00000002UL           /*!< Masked interrupt status SSPRTINTR */
#define SPI_MIS_RXMIS_Msk                     0x00000004UL           /*!< Masked interrupt status SSPRXINTR */
#define SPI_MIS_TXMIS_Msk                     0x00000008UL           /*!< Masked interrupt status SSPTXINTR */

/*--  ICR: Register reset interrupt ---------------------------------------------------------------------------*/
typedef struct {
  uint32_t RORIC                  :1;                                /*!< Reset interrupt SSPRORINTR */
  uint32_t RTIC                   :1;                                /*!< Reset interrupt SSPRTINTR */
} _SPI_ICR_bits;

/* Bit field positions: */
#define SPI_ICR_RORIC_Pos                     0                      /*!< Reset interrupt SSPRORINTR */
#define SPI_ICR_RTIC_Pos                      1                      /*!< Reset interrupt SSPRTINTR */

/* Bit field masks: */
#define SPI_ICR_RORIC_Msk                     0x00000001UL           /*!< Reset interrupt SSPRORINTR */
#define SPI_ICR_RTIC_Msk                      0x00000002UL           /*!< Reset interrupt SSPRTINTR */

/*--  DMACR: Control register DMA -----------------------------------------------------------------------------*/
typedef struct {
  uint32_t RXDMAE                 :1;                                /*!< DMA enable bit at reception */
  uint32_t TXDMAE                 :1;                                /*!< DMA enable bit transmission */
} _SPI_DMACR_bits;

/* Bit field positions: */
#define SPI_DMACR_RXDMAE_Pos                  0                      /*!< DMA enable bit at reception */
#define SPI_DMACR_TXDMAE_Pos                  1                      /*!< DMA enable bit transmission */

/* Bit field masks: */
#define SPI_DMACR_RXDMAE_Msk                  0x00000001UL           /*!< DMA enable bit at reception */
#define SPI_DMACR_TXDMAE_Msk                  0x00000002UL           /*!< DMA enable bit transmission */

typedef struct {
  union {                                                               /*!< Control register 0 */
    __IO uint32_t CR0;                                               /*!< CR0    : type used for word access */
    __IO _SPI_CR0_bits  CR0_bit;                                     /*!< CR0_bit: structure used for bit access */
  };
  union {                                                               /*!< Control register 1 */
    __IO uint32_t CR1;                                               /*!< CR1    : type used for word access */
    __IO _SPI_CR1_bits  CR1_bit;                                     /*!< CR1_bit: structure used for bit access */
  };
  union {                                                               /*!< Data register */
    __IO uint32_t DR;                                                /*!< DR    : type used for word access */
    __IO _SPI_DR_bits  DR_bit;                                       /*!< DR_bit: structure used for bit access */
  };
  union {                                                               /*!< State register */
    __I uint32_t SR;                                                 /*!< SR    : type used for word access */
    __I _SPI_SR_bits  SR_bit;                                        /*!< SR_bit: structure used for bit access */
  };
  union {                                                               /*!< Clock division factor register */
    __IO uint32_t CPSR;                                              /*!< CPSR    : type used for word access */
    __IO _SPI_CPSR_bits  CPSR_bit;                                   /*!< CPSR_bit: structure used for bit access */
  };
  union {                                                               /*!< Mask interrupt register */
    __IO uint32_t IMSC;                                              /*!< IMSC    : type used for word access */
    __IO _SPI_IMSC_bits  IMSC_bit;                                   /*!< IMSC_bit: structure used for bit access */
  };
  union {                                                               /*!< Status register interrupt without mask */
    __I uint32_t RIS;                                                /*!< RIS    : type used for word access */
    __I _SPI_RIS_bits  RIS_bit;                                      /*!< RIS_bit: structure used for bit access */
  };
  union {                                                               /*!< Status register interrupt masking account */
    __I uint32_t MIS;                                                /*!< MIS    : type used for word access */
    __I _SPI_MIS_bits  MIS_bit;                                      /*!< MIS_bit: structure used for bit access */
  };
  union {                                                               /*!< Register reset interrupt */
    __O uint32_t ICR;                                                /*!< ICR    : type used for word access */
    __O _SPI_ICR_bits  ICR_bit;                                      /*!< ICR_bit: structure used for bit access */
  };
  union {                                                               /*!< Control register DMA */
    __IO uint32_t DMACR;                                             /*!< DMACR    : type used for word access */
    __IO _SPI_DMACR_bits  DMACR_bit;                                 /*!< DMACR_bit: structure used for bit access */
  };
} SPI_TypeDef;


/******************************************************************************/
/*                         QSPI registers                                     */
/******************************************************************************/

/*--  HCR: Host Control Register ------------------------------------------------------------------------------*/
typedef struct {
  uint32_t DEN                    :1;                                /*!< Device enable */
  uint32_t DRS                    :1;                                /*!< Device reset   ( SW reset ) */
  uint32_t TXFCLR                 :1;                                /*!< Tx FIFO clear */
  uint32_t RXFCLR                 :1;                                /*!< Rx FIFO clear */
} _QSPI_HCR_bits;

/* Bit field positions: */
#define QSPI_HCR_DEN_Pos                      0                      /*!< Device enable */
#define QSPI_HCR_DRS_Pos                      1                      /*!< Device reset   ( SW reset ) */
#define QSPI_HCR_TXFCLR_Pos                   2                      /*!< Tx FIFO clear */
#define QSPI_HCR_RXFCLR_Pos                   3                      /*!< Rx FIFO clear */

/* Bit field masks: */
#define QSPI_HCR_DEN_Msk                      0x00000001UL           /*!< Device enable */
#define QSPI_HCR_DRS_Msk                      0x00000002UL           /*!< Device reset   ( SW reset ) */
#define QSPI_HCR_TXFCLR_Msk                   0x00000004UL           /*!< Tx FIFO clear */
#define QSPI_HCR_RXFCLR_Msk                   0x00000008UL           /*!< Rx FIFO clear */

/*--  DCR: Device Config Register -----------------------------------------------------------------------------*/
typedef struct {
  uint32_t CPHA                   :1;                                /*!< Serial clock phase */
  uint32_t CPOL                   :1;                                /*!< Serial clock polarity */
  uint32_t                        :6;                                /*!< RESERVED */
  uint32_t CDIV                   :8;                                /*!< Serial clock DIV value */
  uint32_t FMOD                   :3;                                /*!< Functional mode ( [00] - Std.SPI ; [01] - Reserved ; [10] - QSPI Memory Interface ; [11] - Test LoopBack ) */
} _QSPI_DCR_bits;

/* Bit field positions: */
#define QSPI_DCR_CPHA_Pos                     0                      /*!< Serial clock phase */
#define QSPI_DCR_CPOL_Pos                     1                      /*!< Serial clock polarity */
#define QSPI_DCR_CDIV_Pos                     8                      /*!< Serial clock DIV value */
#define QSPI_DCR_FMOD_Pos                     16                     /*!< Functional mode ( [00] - Std.SPI ; [01] - Reserved ; [10] - QSPI Memory Interface ; [11] - Test LoopBack ) */

/* Bit field masks: */
#define QSPI_DCR_CPHA_Msk                     0x00000001UL           /*!< Serial clock phase */
#define QSPI_DCR_CPOL_Msk                     0x00000002UL           /*!< Serial clock polarity */
#define QSPI_DCR_CDIV_Msk                     0x0000FF00UL           /*!< Serial clock DIV value */
#define QSPI_DCR_FMOD_Msk                     0x00070000UL           /*!< Functional mode ( [00] - Std.SPI ; [01] - Reserved ; [10] - QSPI Memory Interface ; [11] - Test LoopBack ) */

/* Bit field enums: */
typedef enum {
  QSPI_DCR_FMOD_SPI                          = 0x0UL,                /*!< Statndart SPI */
  QSPI_DCR_FMOD_QSPI                         = 0x2UL,                /*!< QSPI Memory Interface */
  QSPI_DCR_FMOD_LOOPBACK                     = 0x3UL,                /*!< Test LoopBack */
} QSPI_DCR_FMOD_Enum;

/*--  DSR: Device status register -----------------------------------------------------------------------------*/
typedef struct {
  uint32_t BUSY                   :1;                                /*!< Flag BUSY  */
  uint32_t TIP                    :1;                                /*!< Transfer In Progress */
  uint32_t TST                    :1;                                /*!< Transfer Stall  */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t TFE                    :1;                                /*!< Tx FIFO Empty */
  uint32_t TFF                    :1;                                /*!< Tx FIFO Full */
  uint32_t RFE                    :1;                                /*!< Rx FIFO Empty */
  uint32_t RFF                    :1;                                /*!< Rx FIFO Full */
  uint32_t TXFCNT                 :5;                                /*!< Curent Tx FIFO Element Count */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t RXFCNT                 :5;                                /*!< Curent Rx FIFO Element Count */
} _QSPI_DSR_bits;

/* Bit field positions: */
#define QSPI_DSR_BUSY_Pos                     0                      /*!< Flag BUSY  */
#define QSPI_DSR_TIP_Pos                      1                      /*!< Transfer In Progress */
#define QSPI_DSR_TST_Pos                      2                      /*!< Transfer Stall  */
#define QSPI_DSR_TFE_Pos                      4                      /*!< Tx FIFO Empty */
#define QSPI_DSR_TFF_Pos                      5                      /*!< Tx FIFO Full */
#define QSPI_DSR_RFE_Pos                      6                      /*!< Rx FIFO Empty */
#define QSPI_DSR_RFF_Pos                      7                      /*!< Rx FIFO Full */
#define QSPI_DSR_TXFCNT_Pos                   8                      /*!< Curent Tx FIFO Element Count */
#define QSPI_DSR_RXFCNT_Pos                   16                     /*!< Curent Rx FIFO Element Count */

/* Bit field masks: */
#define QSPI_DSR_BUSY_Msk                     0x00000001UL           /*!< Flag BUSY  */
#define QSPI_DSR_TIP_Msk                      0x00000002UL           /*!< Transfer In Progress */
#define QSPI_DSR_TST_Msk                      0x00000004UL           /*!< Transfer Stall  */
#define QSPI_DSR_TFE_Msk                      0x00000010UL           /*!< Tx FIFO Empty */
#define QSPI_DSR_TFF_Msk                      0x00000020UL           /*!< Tx FIFO Full */
#define QSPI_DSR_RFE_Msk                      0x00000040UL           /*!< Rx FIFO Empty */
#define QSPI_DSR_RFF_Msk                      0x00000080UL           /*!< Rx FIFO Full */
#define QSPI_DSR_TXFCNT_Msk                   0x00001F00UL           /*!< Curent Tx FIFO Element Count */
#define QSPI_DSR_RXFCNT_Msk                   0x001F0000UL           /*!< Curent Rx FIFO Element Count */

/*--  TCR: Transfer Config register ---------------------------------------------------------------------------*/
typedef struct {
  uint32_t LEN                    :6;                                /*!< Size of data */
  uint32_t LSBF                   :1;                                /*!< LSB First ( 0 - MSB First; 1 - LSB First ) */
  uint32_t DDR                    :1;                                /*!< Double Data Rate ( 0 - SDR ; 1 - DDR ) */
  uint32_t TXE                    :1;                                /*!< Serial Transmitter Enable */
  uint32_t RXE                    :1;                                /*!< Serial Receiver Enable */
  uint32_t IOM                    :2;                                /*!< IO Mode ( SIO/DIO/QIO )  Single/Double/Quad lines for serial IO */
} _QSPI_TCR_bits;

/* Bit field positions: */
#define QSPI_TCR_LEN_Pos                      0                      /*!< Size of data */
#define QSPI_TCR_LSBF_Pos                     6                      /*!< LSB First ( 0 - MSB First; 1 - LSB First ) */
#define QSPI_TCR_DDR_Pos                      7                      /*!< Double Data Rate ( 0 - SDR ; 1 - DDR ) */
#define QSPI_TCR_TXE_Pos                      8                      /*!< Serial Transmitter Enable */
#define QSPI_TCR_RXE_Pos                      9                      /*!< Serial Receiver Enable */
#define QSPI_TCR_IOM_Pos                      10                     /*!< IO Mode ( SIO/DIO/QIO )  Single/Double/Quad lines for serial IO */

/* Bit field masks: */
#define QSPI_TCR_LEN_Msk                      0x0000003FUL           /*!< Size of data */
#define QSPI_TCR_LSBF_Msk                     0x00000040UL           /*!< LSB First ( 0 - MSB First; 1 - LSB First ) */
#define QSPI_TCR_DDR_Msk                      0x00000080UL           /*!< Double Data Rate ( 0 - SDR ; 1 - DDR ) */
#define QSPI_TCR_TXE_Msk                      0x00000100UL           /*!< Serial Transmitter Enable */
#define QSPI_TCR_RXE_Msk                      0x00000200UL           /*!< Serial Receiver Enable */
#define QSPI_TCR_IOM_Msk                      0x00000C00UL           /*!< IO Mode ( SIO/DIO/QIO )  Single/Double/Quad lines for serial IO */

/* Bit field enums: */
typedef enum {
  QSPI_TCR_LSBF_MSB                          = 0x0UL,                /*!< MSB First */
  QSPI_TCR_LSBF_LSB                          = 0x1UL,                /*!< LSB First */
} QSPI_TCR_LSBF_Enum;

typedef enum {
  QSPI_TCR_DDR_SDR                           = 0x0UL,                /*!< Single Data Rate */
  QSPI_TCR_DDR_DDR                           = 0x1UL,                /*!< Double Data Rate */
} QSPI_TCR_DDR_Enum;

typedef enum {
  QSPI_TCR_IOM_Single                        = 0x0UL,                /*!< Single lines for serial IO */
  QSPI_TCR_IOM_Double                        = 0x1UL,                /*!< Double lines for serial IO */
  QSPI_TCR_IOM_Quad                          = 0x2UL,                /*!< Quad lines for serial IO */
} QSPI_TCR_IOM_Enum;

/*--  TDR: Transfer Data register -----------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!<  */
} _QSPI_TDR_bits;

/* Bit field positions: */
#define QSPI_TDR_VAL_Pos                      0                      /*!<  */

/* Bit field masks: */
#define QSPI_TDR_VAL_Msk                      0xFFFFFFFFUL           /*!<  */

/*--  TDS: Transfer Data size register ------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!<  */
} _QSPI_TDS_bits;

/* Bit field positions: */
#define QSPI_TDS_VAL_Pos                      0                      /*!<  */

/* Bit field masks: */
#define QSPI_TDS_VAL_Msk                      0xFFFFFFFFUL           /*!<  */

/*--  QCC: QSPI Communication Config register -----------------------------------------------------------------*/
typedef struct {
  uint32_t INST                   :8;                                /*!< Instruction to be sent to the external SPI defice */
  uint32_t IMOD                   :2;                                /*!< Instruction mode */
  uint32_t ADMOD                  :2;                                /*!< Address mode */
  uint32_t ADSIZ                  :2;                                /*!< Address size */
  uint32_t ABMOD                  :2;                                /*!< Alternate bytes mode */
  uint32_t ABSIZ                  :2;                                /*!< Alternate bytes size */
  uint32_t DCYCS                  :5;                                /*!< Number of dummy cycles ( This field defines of the dummy phase. ) It specifies a number of CLK cycles ( 0-31 ) */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t DMOD                   :2;                                /*!< Data mode */
  uint32_t DIOD                   :2;                                /*!< QSPI Data IO Direction */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t DDRM                   :1;                                /*!< QSPI Double Data Rate Mode:  ( 0 - SDR; 1 - DDR )  This bit sets the DDR mode for address, alt bytes, and data phases  */
} _QSPI_QCC_bits;

/* Bit field positions: */
#define QSPI_QCC_INST_Pos                     0                      /*!< Instruction to be sent to the external SPI defice */
#define QSPI_QCC_IMOD_Pos                     8                      /*!< Instruction mode */
#define QSPI_QCC_ADMOD_Pos                    10                     /*!< Address mode */
#define QSPI_QCC_ADSIZ_Pos                    12                     /*!< Address size */
#define QSPI_QCC_ABMOD_Pos                    14                     /*!< Alternate bytes mode */
#define QSPI_QCC_ABSIZ_Pos                    16                     /*!< Alternate bytes size */
#define QSPI_QCC_DCYCS_Pos                    18                     /*!< Number of dummy cycles ( This field defines of the dummy phase. ) It specifies a number of CLK cycles ( 0-31 ) */
#define QSPI_QCC_DMOD_Pos                     24                     /*!< Data mode */
#define QSPI_QCC_DIOD_Pos                     26                     /*!< QSPI Data IO Direction */
#define QSPI_QCC_DDRM_Pos                     31                     /*!< QSPI Double Data Rate Mode:  ( 0 - SDR; 1 - DDR )  This bit sets the DDR mode for address, alt bytes, and data phases  */

/* Bit field masks: */
#define QSPI_QCC_INST_Msk                     0x000000FFUL           /*!< Instruction to be sent to the external SPI defice */
#define QSPI_QCC_IMOD_Msk                     0x00000300UL           /*!< Instruction mode */
#define QSPI_QCC_ADMOD_Msk                    0x00000C00UL           /*!< Address mode */
#define QSPI_QCC_ADSIZ_Msk                    0x00003000UL           /*!< Address size */
#define QSPI_QCC_ABMOD_Msk                    0x0000C000UL           /*!< Alternate bytes mode */
#define QSPI_QCC_ABSIZ_Msk                    0x00030000UL           /*!< Alternate bytes size */
#define QSPI_QCC_DCYCS_Msk                    0x007C0000UL           /*!< Number of dummy cycles ( This field defines of the dummy phase. ) It specifies a number of CLK cycles ( 0-31 ) */
#define QSPI_QCC_DMOD_Msk                     0x03000000UL           /*!< Data mode */
#define QSPI_QCC_DIOD_Msk                     0x0C000000UL           /*!< QSPI Data IO Direction */
#define QSPI_QCC_DDRM_Msk                     0x80000000UL           /*!< QSPI Double Data Rate Mode:  ( 0 - SDR; 1 - DDR )  This bit sets the DDR mode for address, alt bytes, and data phases  */

/* Bit field enums: */
typedef enum {
  QSPI_QCC_IMOD_No                           = 0x0UL,                /*!< No instruction */
  QSPI_QCC_IMOD_SIO                          = 0x1UL,                /*!< Instruction on a single line */
  QSPI_QCC_IMOD_DIO                          = 0x2UL,                /*!< Instruction on two lines */
  QSPI_QCC_IMOD_QIO                          = 0x3UL,                /*!< Instruction on four lines */
} QSPI_QCC_IMOD_Enum;

typedef enum {
  QSPI_QCC_ADMOD_No                          = 0x0UL,                /*!< No address */
  QSPI_QCC_ADMOD_AddrSIO                     = 0x1UL,                /*!< Address on a single line */
  QSPI_QCC_ADMOD_AddrDIO                     = 0x2UL,                /*!< Address on two lines */
  QSPI_QCC_ADMOD_AddrQIO                     = 0x3UL,                /*!< Address on four lines */
} QSPI_QCC_ADMOD_Enum;

typedef enum {
  QSPI_QCC_ADSIZ_AddrSize_8                  = 0x0UL,                /*!< 8-bit address */
  QSPI_QCC_ADSIZ_AddrSize_16                 = 0x1UL,                /*!< 16-bit address */
  QSPI_QCC_ADSIZ_AddrSize_24                 = 0x2UL,                /*!< 24-bit address */
  QSPI_QCC_ADSIZ_AddrSize_32                 = 0x3UL,                /*!< 32-bit address */
} QSPI_QCC_ADSIZ_Enum;

typedef enum {
  QSPI_QCC_ABMOD_AltBytesNo                  = 0x0UL,                /*!< No alternate bytes */
  QSPI_QCC_ABMOD_AltBytesSIO                 = 0x1UL,                /*!< Alternate bytes on a single line */
  QSPI_QCC_ABMOD_AltBytesDIO                 = 0x2UL,                /*!< Alternate bytes on two lines */
  QSPI_QCC_ABMOD_AltBytesQIO                 = 0x3UL,                /*!< Alternate bytes on four lines */
} QSPI_QCC_ABMOD_Enum;

typedef enum {
  QSPI_QCC_ABSIZ_AltBytesSize_8              = 0x0UL,                /*!< 8-bit alternate byte */
  QSPI_QCC_ABSIZ_AltBytesSize_16             = 0x1UL,                /*!< 16-bit alternate byte */
  QSPI_QCC_ABSIZ_AltBytesSize_24             = 0x2UL,                /*!< 24-bit alternate byte */
  QSPI_QCC_ABSIZ_AltBytesSize_32             = 0x3UL,                /*!< 32-bit alternate byte */
} QSPI_QCC_ABSIZ_Enum;

typedef enum {
  QSPI_QCC_DMOD_NoData                       = 0x0UL,                /*!< No data */
  QSPI_QCC_DMOD_Single                       = 0x1UL,                /*!< Single lines for serial IO */
  QSPI_QCC_DMOD_Double                       = 0x2UL,                /*!< Double lines for serial IO */
  QSPI_QCC_DMOD_Quad                         = 0x3UL,                /*!< Quad lines for serial IO */
} QSPI_QCC_DMOD_Enum;

typedef enum {
  QSPI_QCC_DIOD_WR                           = 0x0UL,                /*!< Indirect write mode */
  QSPI_QCC_DIOD_RD                           = 0x1UL,                /*!< Indirect read  mode */
} QSPI_QCC_DIOD_Enum;

/*--  QAD: QSPI Address Register ------------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!<  */
} _QSPI_QAD_bits;

/* Bit field positions: */
#define QSPI_QAD_VAL_Pos                      0                      /*!<  */

/* Bit field masks: */
#define QSPI_QAD_VAL_Msk                      0xFFFFFFFFUL           /*!<  */

/*--  QAB: QSPI Alternate Bytes Register ----------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!<  */
} _QSPI_QAB_bits;

/* Bit field positions: */
#define QSPI_QAB_VAL_Pos                      0                      /*!<  */

/* Bit field masks: */
#define QSPI_QAB_VAL_Msk                      0xFFFFFFFFUL           /*!<  */

/*--  IMR: Interrupt Mask Register ----------------------------------------------------------------------------*/
typedef struct {
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t TCIM                   :1;                                /*!< Transfer Complete Interrupt Mask */
  uint32_t TSIM                   :1;                                /*!< Transfer Stall Interrupt Mask */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t TEIM                   :1;                                /*!< Tx FIFO  Empty Interrupt Mask */
  uint32_t TFIM                   :1;                                /*!< Tx FIFO  Full  Interrupt Mask */
  uint32_t REIM                   :1;                                /*!< Rx FIFO  Empty Interrupt Mask */
  uint32_t RFIM                   :1;                                /*!< Rx FIFO  Full  Interrupt Mask */
  uint32_t TWMIM                  :1;                                /*!< Tx FIFO  Watermark Interrupt Mask */
  uint32_t RWMIM                  :1;                                /*!< Rx FIFO  Watermark Interrupt Mask */
} _QSPI_IMR_bits;

/* Bit field positions: */
#define QSPI_IMR_TCIM_Pos                     1                      /*!< Transfer Complete Interrupt Mask */
#define QSPI_IMR_TSIM_Pos                     2                      /*!< Transfer Stall Interrupt Mask */
#define QSPI_IMR_TEIM_Pos                     4                      /*!< Tx FIFO  Empty Interrupt Mask */
#define QSPI_IMR_TFIM_Pos                     5                      /*!< Tx FIFO  Full  Interrupt Mask */
#define QSPI_IMR_REIM_Pos                     6                      /*!< Rx FIFO  Empty Interrupt Mask */
#define QSPI_IMR_RFIM_Pos                     7                      /*!< Rx FIFO  Full  Interrupt Mask */
#define QSPI_IMR_TWMIM_Pos                    8                      /*!< Tx FIFO  Watermark Interrupt Mask */
#define QSPI_IMR_RWMIM_Pos                    9                      /*!< Rx FIFO  Watermark Interrupt Mask */

/* Bit field masks: */
#define QSPI_IMR_TCIM_Msk                     0x00000002UL           /*!< Transfer Complete Interrupt Mask */
#define QSPI_IMR_TSIM_Msk                     0x00000004UL           /*!< Transfer Stall Interrupt Mask */
#define QSPI_IMR_TEIM_Msk                     0x00000010UL           /*!< Tx FIFO  Empty Interrupt Mask */
#define QSPI_IMR_TFIM_Msk                     0x00000020UL           /*!< Tx FIFO  Full  Interrupt Mask */
#define QSPI_IMR_REIM_Msk                     0x00000040UL           /*!< Rx FIFO  Empty Interrupt Mask */
#define QSPI_IMR_RFIM_Msk                     0x00000080UL           /*!< Rx FIFO  Full  Interrupt Mask */
#define QSPI_IMR_TWMIM_Msk                    0x00000100UL           /*!< Tx FIFO  Watermark Interrupt Mask */
#define QSPI_IMR_RWMIM_Msk                    0x00000200UL           /*!< Rx FIFO  Watermark Interrupt Mask */

/*--  ICR: Interrupt Clear Register ---------------------------------------------------------------------------*/
typedef struct {
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t TCIC                   :1;                                /*!< Transfer Complete Interrupt Clear */
  uint32_t TSIC                   :1;                                /*!< Transfer Stall Interrupt Clear */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t TEIC                   :1;                                /*!< Tx FIFO  Empty Interrupt Clear */
  uint32_t TFIC                   :1;                                /*!< Tx FIFO  Full  Interrupt Clear */
  uint32_t REIC                   :1;                                /*!< Rx FIFO  Empty Interrupt Clear */
  uint32_t RFIC                   :1;                                /*!< Rx FIFO  Full  Interrupt Clear */
  uint32_t TWMIC                  :1;                                /*!< Tx FIFO  Watermark Interrupt Clear */
  uint32_t RWMIC                  :1;                                /*!< Rx FIFO  Watermark Interrupt Clear */
} _QSPI_ICR_bits;

/* Bit field positions: */
#define QSPI_ICR_TCIC_Pos                     1                      /*!< Transfer Complete Interrupt Clear */
#define QSPI_ICR_TSIC_Pos                     2                      /*!< Transfer Stall Interrupt Clear */
#define QSPI_ICR_TEIC_Pos                     4                      /*!< Tx FIFO  Empty Interrupt Clear */
#define QSPI_ICR_TFIC_Pos                     5                      /*!< Tx FIFO  Full  Interrupt Clear */
#define QSPI_ICR_REIC_Pos                     6                      /*!< Rx FIFO  Empty Interrupt Clear */
#define QSPI_ICR_RFIC_Pos                     7                      /*!< Rx FIFO  Full  Interrupt Clear */
#define QSPI_ICR_TWMIC_Pos                    8                      /*!< Tx FIFO  Watermark Interrupt Clear */
#define QSPI_ICR_RWMIC_Pos                    9                      /*!< Rx FIFO  Watermark Interrupt Clear */

/* Bit field masks: */
#define QSPI_ICR_TCIC_Msk                     0x00000002UL           /*!< Transfer Complete Interrupt Clear */
#define QSPI_ICR_TSIC_Msk                     0x00000004UL           /*!< Transfer Stall Interrupt Clear */
#define QSPI_ICR_TEIC_Msk                     0x00000010UL           /*!< Tx FIFO  Empty Interrupt Clear */
#define QSPI_ICR_TFIC_Msk                     0x00000020UL           /*!< Tx FIFO  Full  Interrupt Clear */
#define QSPI_ICR_REIC_Msk                     0x00000040UL           /*!< Rx FIFO  Empty Interrupt Clear */
#define QSPI_ICR_RFIC_Msk                     0x00000080UL           /*!< Rx FIFO  Full  Interrupt Clear */
#define QSPI_ICR_TWMIC_Msk                    0x00000100UL           /*!< Tx FIFO  Watermark Interrupt Clear */
#define QSPI_ICR_RWMIC_Msk                    0x00000200UL           /*!< Rx FIFO  Watermark Interrupt Clear */

/*--  RIS: Raw Interrupt Status Register ----------------------------------------------------------------------*/
typedef struct {
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t TCRIS                  :1;                                /*!< Transfer Complete raw interrupt status */
  uint32_t TSRIS                  :1;                                /*!< Transfer Stall raw interrupt status */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t TERIS                  :1;                                /*!< Tx FIFO Empty raw interrupt status */
  uint32_t TFRIS                  :1;                                /*!< Tx FIFO Full raw interrupt status */
  uint32_t RERIS                  :1;                                /*!< Rx FIFO Empty raw interrupt status */
  uint32_t RFRIS                  :1;                                /*!< Rx FIFO Full raw interrupt status */
  uint32_t TWMRIS                 :1;                                /*!< Tx FIFO Watermark raw interrupt status */
  uint32_t RWMRIS                 :1;                                /*!< Rx FIFO Watermark raw interrupt status */
} _QSPI_RIS_bits;

/* Bit field positions: */
#define QSPI_RIS_TCRIS_Pos                    1                      /*!< Transfer Complete raw interrupt status */
#define QSPI_RIS_TSRIS_Pos                    2                      /*!< Transfer Stall raw interrupt status */
#define QSPI_RIS_TERIS_Pos                    4                      /*!< Tx FIFO Empty raw interrupt status */
#define QSPI_RIS_TFRIS_Pos                    5                      /*!< Tx FIFO Full raw interrupt status */
#define QSPI_RIS_RERIS_Pos                    6                      /*!< Rx FIFO Empty raw interrupt status */
#define QSPI_RIS_RFRIS_Pos                    7                      /*!< Rx FIFO Full raw interrupt status */
#define QSPI_RIS_TWMRIS_Pos                   8                      /*!< Tx FIFO Watermark raw interrupt status */
#define QSPI_RIS_RWMRIS_Pos                   9                      /*!< Rx FIFO Watermark raw interrupt status */

/* Bit field masks: */
#define QSPI_RIS_TCRIS_Msk                    0x00000002UL           /*!< Transfer Complete raw interrupt status */
#define QSPI_RIS_TSRIS_Msk                    0x00000004UL           /*!< Transfer Stall raw interrupt status */
#define QSPI_RIS_TERIS_Msk                    0x00000010UL           /*!< Tx FIFO Empty raw interrupt status */
#define QSPI_RIS_TFRIS_Msk                    0x00000020UL           /*!< Tx FIFO Full raw interrupt status */
#define QSPI_RIS_RERIS_Msk                    0x00000040UL           /*!< Rx FIFO Empty raw interrupt status */
#define QSPI_RIS_RFRIS_Msk                    0x00000080UL           /*!< Rx FIFO Full raw interrupt status */
#define QSPI_RIS_TWMRIS_Msk                   0x00000100UL           /*!< Tx FIFO Watermark raw interrupt status */
#define QSPI_RIS_RWMRIS_Msk                   0x00000200UL           /*!< Rx FIFO Watermark raw interrupt status */

/*--  FWM: Fifo's WaterMarks Register -------------------------------------------------------------------------*/
typedef struct {
  uint32_t TLEC                   :5;                                /*!< Tx FIFO Low Element Count limit ( Low Watermark ) */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t RHEC                   :5;                                /*!< Rx FIFO High Element Count limit ( High Watermark ) */
} _QSPI_FWM_bits;

/* Bit field positions: */
#define QSPI_FWM_TLEC_Pos                     0                      /*!< Tx FIFO Low Element Count limit ( Low Watermark ) */
#define QSPI_FWM_RHEC_Pos                     8                      /*!< Rx FIFO High Element Count limit ( High Watermark ) */

/* Bit field masks: */
#define QSPI_FWM_TLEC_Msk                     0x0000001FUL           /*!< Tx FIFO Low Element Count limit ( Low Watermark ) */
#define QSPI_FWM_RHEC_Msk                     0x00001F00UL           /*!< Rx FIFO High Element Count limit ( High Watermark ) */

/*--  MIS: Masked Interrupt Status Register -------------------------------------------------------------------*/
typedef struct {
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t TCMIS                  :1;                                /*!< Transfer Complete masked interrupt status */
  uint32_t TSMIS                  :1;                                /*!< Transfer Stall masked interrupt status */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t TEMIS                  :1;                                /*!< Tx FIFO  Empty masked interrupt status */
  uint32_t TFMIS                  :1;                                /*!< Tx FIFO  Full  masked interrupt status */
  uint32_t REMIS                  :1;                                /*!< Rx FIFO  Empty masked interrupt status */
  uint32_t RFMIS                  :1;                                /*!< Rx FIFO  Full  masked interrupt status */
  uint32_t TWMIS                  :1;                                /*!< Tx FIFO  Watermark masked interrupt status */
  uint32_t RWMIS                  :1;                                /*!< Rx FIFO  Watermark masked interrupt status */
} _QSPI_MIS_bits;

/* Bit field positions: */
#define QSPI_MIS_TCMIS_Pos                    1                      /*!< Transfer Complete masked interrupt status */
#define QSPI_MIS_TSMIS_Pos                    2                      /*!< Transfer Stall masked interrupt status */
#define QSPI_MIS_TEMIS_Pos                    4                      /*!< Tx FIFO  Empty masked interrupt status */
#define QSPI_MIS_TFMIS_Pos                    5                      /*!< Tx FIFO  Full  masked interrupt status */
#define QSPI_MIS_REMIS_Pos                    6                      /*!< Rx FIFO  Empty masked interrupt status */
#define QSPI_MIS_RFMIS_Pos                    7                      /*!< Rx FIFO  Full  masked interrupt status */
#define QSPI_MIS_TWMIS_Pos                    8                      /*!< Tx FIFO  Watermark masked interrupt status */
#define QSPI_MIS_RWMIS_Pos                    9                      /*!< Rx FIFO  Watermark masked interrupt status */

/* Bit field masks: */
#define QSPI_MIS_TCMIS_Msk                    0x00000002UL           /*!< Transfer Complete masked interrupt status */
#define QSPI_MIS_TSMIS_Msk                    0x00000004UL           /*!< Transfer Stall masked interrupt status */
#define QSPI_MIS_TEMIS_Msk                    0x00000010UL           /*!< Tx FIFO  Empty masked interrupt status */
#define QSPI_MIS_TFMIS_Msk                    0x00000020UL           /*!< Tx FIFO  Full  masked interrupt status */
#define QSPI_MIS_REMIS_Msk                    0x00000040UL           /*!< Rx FIFO  Empty masked interrupt status */
#define QSPI_MIS_RFMIS_Msk                    0x00000080UL           /*!< Rx FIFO  Full  masked interrupt status */
#define QSPI_MIS_TWMIS_Msk                    0x00000100UL           /*!< Tx FIFO  Watermark masked interrupt status */
#define QSPI_MIS_RWMIS_Msk                    0x00000200UL           /*!< Rx FIFO  Watermark masked interrupt status */

typedef struct {
  union {                                                               /*!< Host Control Register */
    __IO uint32_t HCR;                                               /*!< HCR    : type used for word access */
    __IO _QSPI_HCR_bits  HCR_bit;                                    /*!< HCR_bit: structure used for bit access */
  };
  union {                                                               /*!< Device Config Register */
    __IO uint32_t DCR;                                               /*!< DCR    : type used for word access */
    __IO _QSPI_DCR_bits  DCR_bit;                                    /*!< DCR_bit: structure used for bit access */
  };
  union {                                                               /*!< Device status register */
    __I uint32_t DSR;                                                /*!< DSR    : type used for word access */
    __I _QSPI_DSR_bits  DSR_bit;                                     /*!< DSR_bit: structure used for bit access */
  };
  union {                                                               /*!< Transfer Config register */
    __IO uint32_t TCR;                                               /*!< TCR    : type used for word access */
    __IO _QSPI_TCR_bits  TCR_bit;                                    /*!< TCR_bit: structure used for bit access */
  };
  union {                                                               /*!< Transfer Data register */
    __IO uint32_t TDR;                                               /*!< TDR    : type used for word access */
    __IO _QSPI_TDR_bits  TDR_bit;                                    /*!< TDR_bit: structure used for bit access */
  };
  union {                                                               /*!< Transfer Data size register */
    __IO uint32_t TDS;                                               /*!< TDS    : type used for word access */
    __IO _QSPI_TDS_bits  TDS_bit;                                    /*!< TDS_bit: structure used for bit access */
  };
  union {                                                               /*!< QSPI Communication Config register */
    __IO uint32_t QCC;                                               /*!< QCC    : type used for word access */
    __IO _QSPI_QCC_bits  QCC_bit;                                    /*!< QCC_bit: structure used for bit access */
  };
  union {                                                               /*!< QSPI Address Register */
    __IO uint32_t QAD;                                               /*!< QAD    : type used for word access */
    __IO _QSPI_QAD_bits  QAD_bit;                                    /*!< QAD_bit: structure used for bit access */
  };
  union {                                                               /*!< QSPI Alternate Bytes Register */
    __IO uint32_t QAB;                                               /*!< QAB    : type used for word access */
    __IO _QSPI_QAB_bits  QAB_bit;                                    /*!< QAB_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt Mask Register */
    __IO uint32_t IMR;                                               /*!< IMR    : type used for word access */
    __IO _QSPI_IMR_bits  IMR_bit;                                    /*!< IMR_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt Clear Register */
    __IO uint32_t ICR;                                               /*!< ICR    : type used for word access */
    __IO _QSPI_ICR_bits  ICR_bit;                                    /*!< ICR_bit: structure used for bit access */
  };
  union {                                                               /*!< Raw Interrupt Status Register */
    __I uint32_t RIS;                                                /*!< RIS    : type used for word access */
    __I _QSPI_RIS_bits  RIS_bit;                                     /*!< RIS_bit: structure used for bit access */
  };
  union {                                                               /*!< Fifo's WaterMarks Register */
    __IO uint32_t FWM;                                               /*!< FWM    : type used for word access */
    __IO _QSPI_FWM_bits  FWM_bit;                                    /*!< FWM_bit: structure used for bit access */
  };
  union {                                                               /*!< Masked Interrupt Status Register */
    __I uint32_t MIS;                                                /*!< MIS    : type used for word access */
    __I _QSPI_MIS_bits  MIS_bit;                                     /*!< MIS_bit: structure used for bit access */
  };
} QSPI_TypeDef;


/******************************************************************************/
/*                         CAN registers                                      */
/******************************************************************************/

/*--  CLC: Frequency control register -------------------------------------------------------------------------*/
typedef struct {
  uint32_t DISR                   :1;                                /*!< OFF module CAN */
  uint32_t DISS                   :1;                                /*!< State CAN */
} _CAN_CLC_bits;

/* Bit field positions: */
#define CAN_CLC_DISR_Pos                      0                      /*!< OFF module CAN */
#define CAN_CLC_DISS_Pos                      1                      /*!< State CAN */

/* Bit field masks: */
#define CAN_CLC_DISR_Msk                      0x00000001UL           /*!< OFF module CAN */
#define CAN_CLC_DISS_Msk                      0x00000002UL           /*!< State CAN */

/*--  ID: Identity register -----------------------------------------------------------------------------------*/
typedef struct {
  uint32_t MODREV                 :8;                                /*!< Number of modifications of the CAN */
  uint32_t MODTYPE                :8;                                /*!< Digit capacity CAN */
  uint32_t MODNUM                 :16;                               /*!< Identification number CAN */
} _CAN_ID_bits;

/* Bit field positions: */
#define CAN_ID_MODREV_Pos                     0                      /*!< Number of modifications of the CAN */
#define CAN_ID_MODTYPE_Pos                    8                      /*!< Digit capacity CAN */
#define CAN_ID_MODNUM_Pos                     16                     /*!< Identification number CAN */

/* Bit field masks: */
#define CAN_ID_MODREV_Msk                     0x000000FFUL           /*!< Number of modifications of the CAN */
#define CAN_ID_MODTYPE_Msk                    0x0000FF00UL           /*!< Digit capacity CAN */
#define CAN_ID_MODNUM_Msk                     0xFFFF0000UL           /*!< Identification number CAN */

/*--  FDR: Register divider -----------------------------------------------------------------------------------*/
typedef struct {
  uint32_t STEP                   :10;                               /*!< Step divider */
  uint32_t                        :4;                                /*!< RESERVED */
  uint32_t DM                     :2;                                /*!< Mode setting of the frequency divider */
  uint32_t RESULT                 :10;                               /*!< Count frequency divider */
  uint32_t                        :4;                                /*!< RESERVED */
  uint32_t ENHW                   :1;                                /*!< Control bit synchronization */
  uint32_t DISCLK                 :1;                                /*!< Disable bit internal clock */
} _CAN_FDR_bits;

/* Bit field positions: */
#define CAN_FDR_STEP_Pos                      0                      /*!< Step divider */
#define CAN_FDR_DM_Pos                        14                     /*!< Mode setting of the frequency divider */
#define CAN_FDR_RESULT_Pos                    16                     /*!< Count frequency divider */
#define CAN_FDR_ENHW_Pos                      30                     /*!< Control bit synchronization */
#define CAN_FDR_DISCLK_Pos                    31                     /*!< Disable bit internal clock */

/* Bit field masks: */
#define CAN_FDR_STEP_Msk                      0x000003FFUL           /*!< Step divider */
#define CAN_FDR_DM_Msk                        0x0000C000UL           /*!< Mode setting of the frequency divider */
#define CAN_FDR_RESULT_Msk                    0x03FF0000UL           /*!< Count frequency divider */
#define CAN_FDR_ENHW_Msk                      0x40000000UL           /*!< Control bit synchronization */
#define CAN_FDR_DISCLK_Msk                    0x80000000UL           /*!< Disable bit internal clock */

/* Bit field enums: */
typedef enum {
  CAN_FDR_DM_Disable                         = 0x0UL,                /*!< counter disabled */
  CAN_FDR_DM_NormalMode                      = 0x1UL,                /*!< normal operation mode */
  CAN_FDR_DM_DividerMode                     = 0x2UL,                /*!< divider operation mode */
} CAN_FDR_DM_Enum;

/*--  LIST: Register list0 ------------------------------------------------------------------------------------*/
typedef struct {
  uint32_t BEGIN                  :8;                                /*!< Number of the first message object */
  uint32_t END                    :8;                                /*!< Number of the last message object */
  uint32_t SIZE                   :8;                                /*!< List size */
  uint32_t EMPTY                  :1;                                /*!< Indicate empty list */
} _CAN_LIST_bits;

/* Bit field positions: */
#define CAN_LIST_BEGIN_Pos                    0                      /*!< Number of the first message object */
#define CAN_LIST_END_Pos                      8                      /*!< Number of the last message object */
#define CAN_LIST_SIZE_Pos                     16                     /*!< List size */
#define CAN_LIST_EMPTY_Pos                    24                     /*!< Indicate empty list */

/* Bit field masks: */
#define CAN_LIST_BEGIN_Msk                    0x000000FFUL           /*!< Number of the first message object */
#define CAN_LIST_END_Msk                      0x0000FF00UL           /*!< Number of the last message object */
#define CAN_LIST_SIZE_Msk                     0x00FF0000UL           /*!< List size */
#define CAN_LIST_EMPTY_Msk                    0x01000000UL           /*!< Indicate empty list */

/*--  MSPND: Register waiting interrupts0 ---------------------------------------------------------------------*/
typedef struct {
  uint32_t PND                    :32;                               /*!< Field waiting interrupts */
} _CAN_MSPND_bits;

/* Bit field positions: */
#define CAN_MSPND_PND_Pos                     0                      /*!< Field waiting interrupts */

/* Bit field masks: */
#define CAN_MSPND_PND_Msk                     0xFFFFFFFFUL           /*!< Field waiting interrupts */

/*--  MSID: Register messages index0 --------------------------------------------------------------------------*/
typedef struct {
  uint32_t INDEX                  :6;                                /*!<  */
} _CAN_MSID_bits;

/* Bit field positions: */
#define CAN_MSID_INDEX_Pos                    0                      /*!<  */

/* Bit field masks: */
#define CAN_MSID_INDEX_Msk                    0x0000003FUL           /*!<  */

/*--  MSIMASK: Mask register message index --------------------------------------------------------------------*/
typedef struct {
  uint32_t IM                     :32;                               /*!< Mask for waiting bit messages */
} _CAN_MSIMASK_bits;

/* Bit field positions: */
#define CAN_MSIMASK_IM_Pos                    0                      /*!< Mask for waiting bit messages */

/* Bit field masks: */
#define CAN_MSIMASK_IM_Msk                    0xFFFFFFFFUL           /*!< Mask for waiting bit messages */

/*--  PANCTR: Register command panel --------------------------------------------------------------------------*/
typedef struct {
  uint32_t PANCMD                 :8;                                /*!< Command panel */
  uint32_t BUSY                   :1;                                /*!< Busy flag panels arguments (waiting to be written at the end of the command) */
  uint32_t RBUSY                  :1;                                /*!< Busy flag panels arguments (running the command list, the result of which will be recorded in PANAR1 and PANAR2) */
  uint32_t                        :6;                                /*!< RESERVED */
  uint32_t PANAR1                 :8;                                /*!< Panel argument 8 */
  uint32_t PANAR2                 :8;                                /*!< Panel argument 9 */
} _CAN_PANCTR_bits;

/* Bit field positions: */
#define CAN_PANCTR_PANCMD_Pos                 0                      /*!< Command panel */
#define CAN_PANCTR_BUSY_Pos                   8                      /*!< Busy flag panels arguments (waiting to be written at the end of the command) */
#define CAN_PANCTR_RBUSY_Pos                  9                      /*!< Busy flag panels arguments (running the command list, the result of which will be recorded in PANAR1 and PANAR2) */
#define CAN_PANCTR_PANAR1_Pos                 16                     /*!< Panel argument 8 */
#define CAN_PANCTR_PANAR2_Pos                 24                     /*!< Panel argument 9 */

/* Bit field masks: */
#define CAN_PANCTR_PANCMD_Msk                 0x000000FFUL           /*!< Command panel */
#define CAN_PANCTR_BUSY_Msk                   0x00000100UL           /*!< Busy flag panels arguments (waiting to be written at the end of the command) */
#define CAN_PANCTR_RBUSY_Msk                  0x00000200UL           /*!< Busy flag panels arguments (running the command list, the result of which will be recorded in PANAR1 and PANAR2) */
#define CAN_PANCTR_PANAR1_Msk                 0x00FF0000UL           /*!< Panel argument 8 */
#define CAN_PANCTR_PANAR2_Msk                 0xFF000000UL           /*!< Panel argument 9 */

/*--  MCR: Control register -----------------------------------------------------------------------------------*/
typedef struct {
  uint32_t                        :12;                               /*!< RESERVED */
  uint32_t MPSEL                  :4;                                /*!< Field task position after waiting message bit transmit / receive messages */
} _CAN_MCR_bits;

/* Bit field positions: */
#define CAN_MCR_MPSEL_Pos                     12                     /*!< Field task position after waiting message bit transmit / receive messages */

/* Bit field masks: */
#define CAN_MCR_MPSEL_Msk                     0x0000F000UL           /*!< Field task position after waiting message bit transmit / receive messages */

/*--  MITR: Interrupt register --------------------------------------------------------------------------------*/
typedef struct {
  uint32_t IT                     :16;                               /*!< Field  generate interrupt */
} _CAN_MITR_bits;

/* Bit field positions: */
#define CAN_MITR_IT_Pos                       0                      /*!< Field  generate interrupt */

/* Bit field masks: */
#define CAN_MITR_IT_Msk                       0x0000FFFFUL           /*!< Field  generate interrupt */

/*-- Node: NCR: Register control node0 ------------------------------------------------------------------------*/
typedef struct {
  uint32_t INIT                   :1;                                /*!< Node initialization */
  uint32_t TRIE                   :1;                                /*!< Interrupt enable bit of the assembly at the end of the transmission / reception */
  uint32_t LECIE                  :1;                                /*!< Interrupt enable bit of the assembly when it detects the last error code */
  uint32_t ALIE                   :1;                                /*!< Enable interrupt ALERT from node */
  uint32_t CANDIS                 :1;                                /*!< Off node */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t CCE                    :1;                                /*!< Permission node configuration changes */
  uint32_t CALM                   :1;                                /*!< Activate the node analysis */
} _CAN_Node_NCR_bits;

/* Bit field positions: */
#define CAN_Node_NCR_INIT_Pos                 0                      /*!< Node initialization */
#define CAN_Node_NCR_TRIE_Pos                 1                      /*!< Interrupt enable bit of the assembly at the end of the transmission / reception */
#define CAN_Node_NCR_LECIE_Pos                2                      /*!< Interrupt enable bit of the assembly when it detects the last error code */
#define CAN_Node_NCR_ALIE_Pos                 3                      /*!< Enable interrupt ALERT from node */
#define CAN_Node_NCR_CANDIS_Pos               4                      /*!< Off node */
#define CAN_Node_NCR_CCE_Pos                  6                      /*!< Permission node configuration changes */
#define CAN_Node_NCR_CALM_Pos                 7                      /*!< Activate the node analysis */

/* Bit field masks: */
#define CAN_Node_NCR_INIT_Msk                 0x00000001UL           /*!< Node initialization */
#define CAN_Node_NCR_TRIE_Msk                 0x00000002UL           /*!< Interrupt enable bit of the assembly at the end of the transmission / reception */
#define CAN_Node_NCR_LECIE_Msk                0x00000004UL           /*!< Interrupt enable bit of the assembly when it detects the last error code */
#define CAN_Node_NCR_ALIE_Msk                 0x00000008UL           /*!< Enable interrupt ALERT from node */
#define CAN_Node_NCR_CANDIS_Msk               0x00000010UL           /*!< Off node */
#define CAN_Node_NCR_CCE_Msk                  0x00000040UL           /*!< Permission node configuration changes */
#define CAN_Node_NCR_CALM_Msk                 0x00000080UL           /*!< Activate the node analysis */

/*-- Node: NSR: Register state node0 --------------------------------------------------------------------------*/
typedef struct {
  uint32_t LEC                    :3;                                /*!< Last error code */
  uint32_t TXOK                   :1;                                /*!< Flag successful message transmission */
  uint32_t RXOK                   :1;                                /*!< Flag successful reception of messages */
  uint32_t ALERT                  :1;                                /*!< Warning flag ALERT */
  uint32_t EWRN                   :1;                                /*!< Flag critical errors */
  uint32_t BOFF                   :1;                                /*!< Status flag 'is disconnected from the bus' */
  uint32_t LLE                    :1;                                /*!< Error flag list */
  uint32_t LOE                    :1;                                /*!< Error Flag Room list */
} _CAN_Node_NSR_bits;

/* Bit field positions: */
#define CAN_Node_NSR_LEC_Pos                  0                      /*!< Last error code */
#define CAN_Node_NSR_TXOK_Pos                 3                      /*!< Flag successful message transmission */
#define CAN_Node_NSR_RXOK_Pos                 4                      /*!< Flag successful reception of messages */
#define CAN_Node_NSR_ALERT_Pos                5                      /*!< Warning flag ALERT */
#define CAN_Node_NSR_EWRN_Pos                 6                      /*!< Flag critical errors */
#define CAN_Node_NSR_BOFF_Pos                 7                      /*!< Status flag 'is disconnected from the bus' */
#define CAN_Node_NSR_LLE_Pos                  8                      /*!< Error flag list */
#define CAN_Node_NSR_LOE_Pos                  9                      /*!< Error Flag Room list */

/* Bit field masks: */
#define CAN_Node_NSR_LEC_Msk                  0x00000007UL           /*!< Last error code */
#define CAN_Node_NSR_TXOK_Msk                 0x00000008UL           /*!< Flag successful message transmission */
#define CAN_Node_NSR_RXOK_Msk                 0x00000010UL           /*!< Flag successful reception of messages */
#define CAN_Node_NSR_ALERT_Msk                0x00000020UL           /*!< Warning flag ALERT */
#define CAN_Node_NSR_EWRN_Msk                 0x00000040UL           /*!< Flag critical errors */
#define CAN_Node_NSR_BOFF_Msk                 0x00000080UL           /*!< Status flag 'is disconnected from the bus' */
#define CAN_Node_NSR_LLE_Msk                  0x00000100UL           /*!< Error flag list */
#define CAN_Node_NSR_LOE_Msk                  0x00000200UL           /*!< Error Flag Room list */

/* Bit field enums: */
typedef enum {
  CAN_Node_NSR_LEC_NoErr                     = 0x0UL,                /*!< no error */
  CAN_Node_NSR_LEC_StuffErr                  = 0x1UL,                /*!< stuff error */
  CAN_Node_NSR_LEC_FormErr                   = 0x2UL,                /*!< form error */
  CAN_Node_NSR_LEC_AckErr                    = 0x3UL,                /*!< acknowlegment error */
  CAN_Node_NSR_LEC_Bit1Err                   = 0x4UL,                /*!< bit 1 error */
  CAN_Node_NSR_LEC_Bit0Err                   = 0x5UL,                /*!< bit 0 error */
  CAN_Node_NSR_LEC_CRCErr                    = 0x6UL,                /*!< CRC error */
  CAN_Node_NSR_LEC_WriteEn                   = 0x7UL,                /*!< enable hardware write */
} CAN_Node_NSR_LEC_Enum;

/*-- Node: NIPR: Interrupt pointer register node0 -------------------------------------------------------------*/
typedef struct {
  uint32_t ALINP                  :4;                                /*!<  */
  uint32_t LECINP                 :4;                                /*!<  */
  uint32_t TRINP                  :4;                                /*!<  */
  uint32_t CFCINP                 :4;                                /*!<  */
} _CAN_Node_NIPR_bits;

/* Bit field positions: */
#define CAN_Node_NIPR_ALINP_Pos               0                      /*!<  */
#define CAN_Node_NIPR_LECINP_Pos              4                      /*!<  */
#define CAN_Node_NIPR_TRINP_Pos               8                      /*!<  */
#define CAN_Node_NIPR_CFCINP_Pos              12                     /*!<  */

/* Bit field masks: */
#define CAN_Node_NIPR_ALINP_Msk               0x0000000FUL           /*!<  */
#define CAN_Node_NIPR_LECINP_Msk              0x000000F0UL           /*!<  */
#define CAN_Node_NIPR_TRINP_Msk               0x00000F00UL           /*!<  */
#define CAN_Node_NIPR_CFCINP_Msk              0x0000F000UL           /*!<  */

/*-- Node: NPCR: Port control register node0 ------------------------------------------------------------------*/
typedef struct {
  uint32_t                        :8;                                /*!< RESERVED */
  uint32_t LBM                    :1;                                /*!< Enable mode Loop-Back */
} _CAN_Node_NPCR_bits;

/* Bit field positions: */
#define CAN_Node_NPCR_LBM_Pos                 8                      /*!< Enable mode Loop-Back */

/* Bit field masks: */
#define CAN_Node_NPCR_LBM_Msk                 0x00000100UL           /*!< Enable mode Loop-Back */

/*-- Node: NBTR: Timing register bits 0 -----------------------------------------------------------------------*/
typedef struct {
  uint32_t BRP                    :6;                                /*!< Prescaler rate */
  uint32_t SJW                    :2;                                /*!< Transition width resynchronization */
  uint32_t TSEG1                  :4;                                /*!< Parameter 1 */
  uint32_t TSEG2                  :3;                                /*!< Parameter 2 */
  uint32_t DIV8                   :1;                                /*!< Frequency divider by 8 */
} _CAN_Node_NBTR_bits;

/* Bit field positions: */
#define CAN_Node_NBTR_BRP_Pos                 0                      /*!< Prescaler rate */
#define CAN_Node_NBTR_SJW_Pos                 6                      /*!< Transition width resynchronization */
#define CAN_Node_NBTR_TSEG1_Pos               8                      /*!< Parameter 1 */
#define CAN_Node_NBTR_TSEG2_Pos               12                     /*!< Parameter 2 */
#define CAN_Node_NBTR_DIV8_Pos                15                     /*!< Frequency divider by 8 */

/* Bit field masks: */
#define CAN_Node_NBTR_BRP_Msk                 0x0000003FUL           /*!< Prescaler rate */
#define CAN_Node_NBTR_SJW_Msk                 0x000000C0UL           /*!< Transition width resynchronization */
#define CAN_Node_NBTR_TSEG1_Msk               0x00000F00UL           /*!< Parameter 1 */
#define CAN_Node_NBTR_TSEG2_Msk               0x00007000UL           /*!< Parameter 2 */
#define CAN_Node_NBTR_DIV8_Msk                0x00008000UL           /*!< Frequency divider by 8 */

/*-- Node: NECNT: Counter error register node0 ----------------------------------------------------------------*/
typedef struct {
  uint32_t REC                    :8;                                /*!< Receive Error Counter field messages */
  uint32_t TEC                    :8;                                /*!< Counter field messaging error */
  uint32_t EWRNLVL                :8;                                /*!< Error limit at which a flag is set in the register EWRN NSR */
  uint32_t LETD                   :1;                                /*!< Flag last transmission errors */
  uint32_t LEINC                  :1;                                /*!< Indicator increment at the last error */
} _CAN_Node_NECNT_bits;

/* Bit field positions: */
#define CAN_Node_NECNT_REC_Pos                0                      /*!< Receive Error Counter field messages */
#define CAN_Node_NECNT_TEC_Pos                8                      /*!< Counter field messaging error */
#define CAN_Node_NECNT_EWRNLVL_Pos            16                     /*!< Error limit at which a flag is set in the register EWRN NSR */
#define CAN_Node_NECNT_LETD_Pos               24                     /*!< Flag last transmission errors */
#define CAN_Node_NECNT_LEINC_Pos              25                     /*!< Indicator increment at the last error */

/* Bit field masks: */
#define CAN_Node_NECNT_REC_Msk                0x000000FFUL           /*!< Receive Error Counter field messages */
#define CAN_Node_NECNT_TEC_Msk                0x0000FF00UL           /*!< Counter field messaging error */
#define CAN_Node_NECNT_EWRNLVL_Msk            0x00FF0000UL           /*!< Error limit at which a flag is set in the register EWRN NSR */
#define CAN_Node_NECNT_LETD_Msk               0x01000000UL           /*!< Flag last transmission errors */
#define CAN_Node_NECNT_LEINC_Msk              0x02000000UL           /*!< Indicator increment at the last error */

/*-- Node: NFCR: Register message counter node0 ---------------------------------------------------------------*/
typedef struct {
  uint32_t CFC                    :16;                               /*!< Field of the message counter  */
  uint32_t CFSEL                  :3;                                /*!< Field parameter setting mode selected message counter */
  uint32_t CFMOD                  :2;                                /*!< Field task mode message counter */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t CFCIE                  :1;                                /*!< Interrupt enable bit of the message counter */
  uint32_t CFCOV                  :1;                                /*!< Counter overflow flag messages */
} _CAN_Node_NFCR_bits;

/* Bit field positions: */
#define CAN_Node_NFCR_CFC_Pos                 0                      /*!< Field of the message counter  */
#define CAN_Node_NFCR_CFSEL_Pos               16                     /*!< Field parameter setting mode selected message counter */
#define CAN_Node_NFCR_CFMOD_Pos               19                     /*!< Field task mode message counter */
#define CAN_Node_NFCR_CFCIE_Pos               22                     /*!< Interrupt enable bit of the message counter */
#define CAN_Node_NFCR_CFCOV_Pos               23                     /*!< Counter overflow flag messages */

/* Bit field masks: */
#define CAN_Node_NFCR_CFC_Msk                 0x0000FFFFUL           /*!< Field of the message counter  */
#define CAN_Node_NFCR_CFSEL_Msk               0x00070000UL           /*!< Field parameter setting mode selected message counter */
#define CAN_Node_NFCR_CFMOD_Msk               0x00180000UL           /*!< Field task mode message counter */
#define CAN_Node_NFCR_CFCIE_Msk               0x00400000UL           /*!< Interrupt enable bit of the message counter */
#define CAN_Node_NFCR_CFCOV_Msk               0x00800000UL           /*!< Counter overflow flag messages */

//Cluster LIST:
typedef struct {
  union { 
  /*!< Register list0 */
    __I uint32_t LIST;                                               /*!< LIST : type used for word access */
    __I _CAN_LIST_bits LIST_bit;                                     /*!< LIST_bit: structure used for bit access */
  };
} _CAN_LIST_TypeDef;
//Cluster MSPND:
typedef struct {
  union { 
  /*!< Register waiting interrupts0 */
    __IO uint32_t MSPND;                                             /*!< MSPND : type used for word access */
    __IO _CAN_MSPND_bits MSPND_bit;                                  /*!< MSPND_bit: structure used for bit access */
  };
} _CAN_MSPND_TypeDef;
//Cluster MSID:
typedef struct {
  union { 
  /*!< Register messages index0 */
    __I uint32_t MSID;                                               /*!< MSID : type used for word access */
    __I _CAN_MSID_bits MSID_bit;                                     /*!< MSID_bit: structure used for bit access */
  };
} _CAN_MSID_TypeDef;
//Cluster Node:
typedef struct {
  union { 
  /*!< Register control node0 */
    __IO uint32_t NCR;                                               /*!< NCR : type used for word access */
    __IO _CAN_Node_NCR_bits NCR_bit;                                 /*!< NCR_bit: structure used for bit access */
  };
  union { 
  /*!< Register state node0 */
    __IO uint32_t NSR;                                               /*!< NSR : type used for word access */
    __IO _CAN_Node_NSR_bits NSR_bit;                                 /*!< NSR_bit: structure used for bit access */
  };
  union { 
  /*!< Interrupt pointer register node0 */
    __IO uint32_t NIPR;                                              /*!< NIPR : type used for word access */
    __IO _CAN_Node_NIPR_bits NIPR_bit;                               /*!< NIPR_bit: structure used for bit access */
  };
  union { 
  /*!< Port control register node0 */
    __IO uint32_t NPCR;                                              /*!< NPCR : type used for word access */
    __IO _CAN_Node_NPCR_bits NPCR_bit;                               /*!< NPCR_bit: structure used for bit access */
  };
  union { 
  /*!< Timing register bits 0 */
    __IO uint32_t NBTR;                                              /*!< NBTR : type used for word access */
    __IO _CAN_Node_NBTR_bits NBTR_bit;                               /*!< NBTR_bit: structure used for bit access */
  };
  union { 
  /*!< Counter error register node0 */
    __IO uint32_t NECNT;                                             /*!< NECNT : type used for word access */
    __IO _CAN_Node_NECNT_bits NECNT_bit;                             /*!< NECNT_bit: structure used for bit access */
  };
  union { 
  /*!< Register message counter node0 */
    __IO uint32_t NFCR;                                              /*!< NFCR : type used for word access */
    __IO _CAN_Node_NFCR_bits NFCR_bit;                               /*!< NFCR_bit: structure used for bit access */
  };
    __IO uint32_t Reserved0[57];
} _CAN_Node_TypeDef;
typedef struct {
  union {                                                               /*!< Frequency control register */
    __IO uint32_t CLC;                                               /*!< CLC    : type used for word access */
    __IO _CAN_CLC_bits  CLC_bit;                                     /*!< CLC_bit: structure used for bit access */
  };
    __IO uint32_t Reserved0;
  union {                                                               /*!< Identity register */
    __IO uint32_t ID;                                                /*!< ID    : type used for word access */
    __IO _CAN_ID_bits  ID_bit;                                       /*!< ID_bit: structure used for bit access */
  };
  union {                                                               /*!< Register divider */
    __IO uint32_t FDR;                                               /*!< FDR    : type used for word access */
    __IO _CAN_FDR_bits  FDR_bit;                                     /*!< FDR_bit: structure used for bit access */
  };
    __IO uint32_t Reserved1[60];
  _CAN_LIST_TypeDef LIST[8];
    __IO uint32_t Reserved2[8];
  _CAN_MSPND_TypeDef MSPND[8];
    __IO uint32_t Reserved3[8];
  _CAN_MSID_TypeDef MSID[8];
    __IO uint32_t Reserved4[8];
  union {                                                               /*!< Mask register message index */
    __IO uint32_t MSIMASK;                                           /*!< MSIMASK    : type used for word access */
    __IO _CAN_MSIMASK_bits  MSIMASK_bit;                             /*!< MSIMASK_bit: structure used for bit access */
  };
  union {                                                               /*!< Register command panel */
    __IO uint32_t PANCTR;                                            /*!< PANCTR    : type used for word access */
    __IO _CAN_PANCTR_bits  PANCTR_bit;                               /*!< PANCTR_bit: structure used for bit access */
  };
  union {                                                               /*!< Control register */
    __IO uint32_t MCR;                                               /*!< MCR    : type used for word access */
    __IO _CAN_MCR_bits  MCR_bit;                                     /*!< MCR_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt register */
    __O uint32_t MITR;                                               /*!< MITR    : type used for word access */
    __O _CAN_MITR_bits  MITR_bit;                                    /*!< MITR_bit: structure used for bit access */
  };
    __IO uint32_t Reserved5[12];
  _CAN_Node_TypeDef Node[2];
} CAN_TypeDef;


/******************************************************************************/
/*                         CANMSG registers                                   */
/******************************************************************************/

/*-- Msg: MOFCR: Register control the operation of the message object 0 ---------------------------------------*/
typedef struct {
  uint32_t MMC                    :4;                                /*!<  */
  uint32_t                        :4;                                /*!< RESERVED */
  uint32_t GDFS                   :1;                                /*!<  */
  uint32_t IDC                    :1;                                /*!<  */
  uint32_t DLCC                   :1;                                /*!<  */
  uint32_t DATC                   :1;                                /*!<  */
  uint32_t                        :4;                                /*!< RESERVED */
  uint32_t RXIE                   :1;                                /*!< Interrupt enable after taking the messages */
  uint32_t TXIE                   :1;                                /*!< Interrupt enable at the end of the message */
  uint32_t OVIE                   :1;                                /*!< Interrupt enable FIFO to fill the message object 0 */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t FRREN                  :1;                                /*!< Enable remote request */
  uint32_t RMM                    :1;                                /*!< Enable remote monitoring of the communication object */
  uint32_t SDT                    :1;                                /*!< Bit single of the message object 0 participation in shipment */
  uint32_t STT                    :1;                                /*!< Bit single data transfer */
  uint32_t DLC                    :4;                                /*!< Data length code */
} _CANMSG_Msg_MOFCR_bits;

/* Bit field positions: */
#define CANMSG_Msg_MOFCR_MMC_Pos              0                      /*!<  */
#define CANMSG_Msg_MOFCR_GDFS_Pos             8                      /*!<  */
#define CANMSG_Msg_MOFCR_IDC_Pos              9                      /*!<  */
#define CANMSG_Msg_MOFCR_DLCC_Pos             10                     /*!<  */
#define CANMSG_Msg_MOFCR_DATC_Pos             11                     /*!<  */
#define CANMSG_Msg_MOFCR_RXIE_Pos             16                     /*!< Interrupt enable after taking the messages */
#define CANMSG_Msg_MOFCR_TXIE_Pos             17                     /*!< Interrupt enable at the end of the message */
#define CANMSG_Msg_MOFCR_OVIE_Pos             18                     /*!< Interrupt enable FIFO to fill the message object 0 */
#define CANMSG_Msg_MOFCR_FRREN_Pos            20                     /*!< Enable remote request */
#define CANMSG_Msg_MOFCR_RMM_Pos              21                     /*!< Enable remote monitoring of the communication object */
#define CANMSG_Msg_MOFCR_SDT_Pos              22                     /*!< Bit single of the message object 0 participation in shipment */
#define CANMSG_Msg_MOFCR_STT_Pos              23                     /*!< Bit single data transfer */
#define CANMSG_Msg_MOFCR_DLC_Pos              24                     /*!< Data length code */

/* Bit field masks: */
#define CANMSG_Msg_MOFCR_MMC_Msk              0x0000000FUL           /*!<  */
#define CANMSG_Msg_MOFCR_GDFS_Msk             0x00000100UL           /*!<  */
#define CANMSG_Msg_MOFCR_IDC_Msk              0x00000200UL           /*!<  */
#define CANMSG_Msg_MOFCR_DLCC_Msk             0x00000400UL           /*!<  */
#define CANMSG_Msg_MOFCR_DATC_Msk             0x00000800UL           /*!<  */
#define CANMSG_Msg_MOFCR_RXIE_Msk             0x00010000UL           /*!< Interrupt enable after taking the messages */
#define CANMSG_Msg_MOFCR_TXIE_Msk             0x00020000UL           /*!< Interrupt enable at the end of the message */
#define CANMSG_Msg_MOFCR_OVIE_Msk             0x00040000UL           /*!< Interrupt enable FIFO to fill the message object 0 */
#define CANMSG_Msg_MOFCR_FRREN_Msk            0x00100000UL           /*!< Enable remote request */
#define CANMSG_Msg_MOFCR_RMM_Msk              0x00200000UL           /*!< Enable remote monitoring of the communication object */
#define CANMSG_Msg_MOFCR_SDT_Msk              0x00400000UL           /*!< Bit single of the message object 0 participation in shipment */
#define CANMSG_Msg_MOFCR_STT_Msk              0x00800000UL           /*!< Bit single data transfer */
#define CANMSG_Msg_MOFCR_DLC_Msk              0x0F000000UL           /*!< Data length code */

/* Bit field enums: */
typedef enum {
  CANMSG_Msg_MOFCR_MMC_MsgObj                = 0x0UL,                /*!< message object */
  CANMSG_Msg_MOFCR_MMC_RXObj                 = 0x1UL,                /*!< receiver FIFO structure object */
  CANMSG_Msg_MOFCR_MMC_TXObj                 = 0x2UL,                /*!< transmitter FIFO structure object */
  CANMSG_Msg_MOFCR_MMC_SlaveTXObj            = 0x3UL,                /*!< transmitter FIFO structure slave object */
  CANMSG_Msg_MOFCR_MMC_SrcObj                = 0x4UL,                /*!< gateway source object */
} CANMSG_Msg_MOFCR_MMC_Enum;

/*-- Msg: MOFGPR: Pointer register FIFO / gateway message object 0 --------------------------------------------*/
typedef struct {
  uint32_t BOT                    :8;                                /*!< FIFO pointer to the lower element */
  uint32_t TOP                    :8;                                /*!< FIFO pointer to the top element  */
  uint32_t CUR                    :8;                                /*!< A pointer to the current object within the FIFO or gateway */
  uint32_t SEL                    :8;                                /*!< Object pointer message */
} _CANMSG_Msg_MOFGPR_bits;

/* Bit field positions: */
#define CANMSG_Msg_MOFGPR_BOT_Pos             0                      /*!< FIFO pointer to the lower element */
#define CANMSG_Msg_MOFGPR_TOP_Pos             8                      /*!< FIFO pointer to the top element  */
#define CANMSG_Msg_MOFGPR_CUR_Pos             16                     /*!< A pointer to the current object within the FIFO or gateway */
#define CANMSG_Msg_MOFGPR_SEL_Pos             24                     /*!< Object pointer message */

/* Bit field masks: */
#define CANMSG_Msg_MOFGPR_BOT_Msk             0x000000FFUL           /*!< FIFO pointer to the lower element */
#define CANMSG_Msg_MOFGPR_TOP_Msk             0x0000FF00UL           /*!< FIFO pointer to the top element  */
#define CANMSG_Msg_MOFGPR_CUR_Msk             0x00FF0000UL           /*!< A pointer to the current object within the FIFO or gateway */
#define CANMSG_Msg_MOFGPR_SEL_Msk             0xFF000000UL           /*!< Object pointer message */

/*-- Msg: MOIPR: Pointer register interrupt message object 0 --------------------------------------------------*/
typedef struct {
  uint32_t RXINP                  :4;                                /*!< Pointer interrupt line to interrupt after receiving */
  uint32_t TXINP                  :4;                                /*!< Pointer interrupt line to interrupt after transfer */
  uint32_t MPN                    :8;                                /*!< Number message waiting bit */
  uint32_t CFCVAL                 :16;                               /*!< Number of frames */
} _CANMSG_Msg_MOIPR_bits;

/* Bit field positions: */
#define CANMSG_Msg_MOIPR_RXINP_Pos            0                      /*!< Pointer interrupt line to interrupt after receiving */
#define CANMSG_Msg_MOIPR_TXINP_Pos            4                      /*!< Pointer interrupt line to interrupt after transfer */
#define CANMSG_Msg_MOIPR_MPN_Pos              8                      /*!< Number message waiting bit */
#define CANMSG_Msg_MOIPR_CFCVAL_Pos           16                     /*!< Number of frames */

/* Bit field masks: */
#define CANMSG_Msg_MOIPR_RXINP_Msk            0x0000000FUL           /*!< Pointer interrupt line to interrupt after receiving */
#define CANMSG_Msg_MOIPR_TXINP_Msk            0x000000F0UL           /*!< Pointer interrupt line to interrupt after transfer */
#define CANMSG_Msg_MOIPR_MPN_Msk              0x0000FF00UL           /*!< Number message waiting bit */
#define CANMSG_Msg_MOIPR_CFCVAL_Msk           0xFFFF0000UL           /*!< Number of frames */

/*-- Msg: MOAMR: Mask register message object 0 ---------------------------------------------------------------*/
typedef struct {
  uint32_t AM                     :29;                               /*!< Mask ID */
  uint32_t MIDE                   :1;                                /*!< Mask bit IDE message */
} _CANMSG_Msg_MOAMR_bits;

/* Bit field positions: */
#define CANMSG_Msg_MOAMR_AM_Pos               0                      /*!< Mask ID */
#define CANMSG_Msg_MOAMR_MIDE_Pos             29                     /*!< Mask bit IDE message */

/* Bit field masks: */
#define CANMSG_Msg_MOAMR_AM_Msk               0x1FFFFFFFUL           /*!< Mask ID */
#define CANMSG_Msg_MOAMR_MIDE_Msk             0x20000000UL           /*!< Mask bit IDE message */

/*-- Msg: MODATAL: Low data registers of the message object 0 -------------------------------------------------*/
typedef struct {
  uint32_t DB0                    :8;                                /*!< Zero byte data */
  uint32_t DB1                    :8;                                /*!< The first data byte */
  uint32_t DB2                    :8;                                /*!< Second data byte */
  uint32_t DB3                    :8;                                /*!< The third data byte */
} _CANMSG_Msg_MODATAL_bits;

/* Bit field positions: */
#define CANMSG_Msg_MODATAL_DB0_Pos            0                      /*!< Zero byte data */
#define CANMSG_Msg_MODATAL_DB1_Pos            8                      /*!< The first data byte */
#define CANMSG_Msg_MODATAL_DB2_Pos            16                     /*!< Second data byte */
#define CANMSG_Msg_MODATAL_DB3_Pos            24                     /*!< The third data byte */

/* Bit field masks: */
#define CANMSG_Msg_MODATAL_DB0_Msk            0x000000FFUL           /*!< Zero byte data */
#define CANMSG_Msg_MODATAL_DB1_Msk            0x0000FF00UL           /*!< The first data byte */
#define CANMSG_Msg_MODATAL_DB2_Msk            0x00FF0000UL           /*!< Second data byte */
#define CANMSG_Msg_MODATAL_DB3_Msk            0xFF000000UL           /*!< The third data byte */

/*-- Msg: MODATAH: High data registers of the message object 0 ------------------------------------------------*/
typedef struct {
  uint32_t DB4                    :8;                                /*!< The fourth data byte */
  uint32_t DB5                    :8;                                /*!< Fifth byte data */
  uint32_t DB6                    :8;                                /*!< Sixth byte data  */
  uint32_t DB7                    :8;                                /*!< Seventh byte of data  */
} _CANMSG_Msg_MODATAH_bits;

/* Bit field positions: */
#define CANMSG_Msg_MODATAH_DB4_Pos            0                      /*!< The fourth data byte */
#define CANMSG_Msg_MODATAH_DB5_Pos            8                      /*!< Fifth byte data */
#define CANMSG_Msg_MODATAH_DB6_Pos            16                     /*!< Sixth byte data  */
#define CANMSG_Msg_MODATAH_DB7_Pos            24                     /*!< Seventh byte of data  */

/* Bit field masks: */
#define CANMSG_Msg_MODATAH_DB4_Msk            0x000000FFUL           /*!< The fourth data byte */
#define CANMSG_Msg_MODATAH_DB5_Msk            0x0000FF00UL           /*!< Fifth byte data */
#define CANMSG_Msg_MODATAH_DB6_Msk            0x00FF0000UL           /*!< Sixth byte data  */
#define CANMSG_Msg_MODATAH_DB7_Msk            0xFF000000UL           /*!< Seventh byte of data  */

/*-- Msg: MOAR: Register arbitration message object 0 ---------------------------------------------------------*/
typedef struct {
  uint32_t ID                     :29;                               /*!< Object ID message 0 */
  uint32_t IDE                    :1;                                /*!< Bit extension identifier of the message object 0 */
  uint32_t PRI                    :2;                                /*!< Priority class */
} _CANMSG_Msg_MOAR_bits;

/* Bit field positions: */
#define CANMSG_Msg_MOAR_ID_Pos                0                      /*!< Object ID message 0 */
#define CANMSG_Msg_MOAR_IDE_Pos               29                     /*!< Bit extension identifier of the message object 0 */
#define CANMSG_Msg_MOAR_PRI_Pos               30                     /*!< Priority class */

/* Bit field masks: */
#define CANMSG_Msg_MOAR_ID_Msk                0x1FFFFFFFUL           /*!< Object ID message 0 */
#define CANMSG_Msg_MOAR_IDE_Msk               0x20000000UL           /*!< Bit extension identifier of the message object 0 */
#define CANMSG_Msg_MOAR_PRI_Msk               0xC0000000UL           /*!< Priority class */

/*-- Msg: MOCTR: Control register Message object 0 ------------------------------------------------------------*/
typedef struct {
  uint32_t RESRXPND               :1;                                /*!< Reset bit RXPND */
  uint32_t RESTXPND               :1;                                /*!< Reset bit TXPND */
  uint32_t RESRXUPD               :1;                                /*!< Reset bit RXUPD */
  uint32_t RESNEWDAT              :1;                                /*!< Reset bit NEWDAT */
  uint32_t RESMSGLST              :1;                                /*!< Reset bit MSGLST */
  uint32_t RESMSGVAL              :1;                                /*!< Reset bit MSGVAL */
  uint32_t RESRTSEL               :1;                                /*!< Reset bit RTSEL */
  uint32_t RESRXEN                :1;                                /*!< Reset bit RXEN */
  uint32_t RESTXRQ                :1;                                /*!< Reset bit TXRQ */
  uint32_t RESTXEN0               :1;                                /*!< Reset bit TXEN0 */
  uint32_t RESTXEN1               :1;                                /*!< Reset bit TXEN1 */
  uint32_t RESDIR                 :1;                                /*!< Reset bit DIR */
  uint32_t                        :4;                                /*!< RESERVED */
  uint32_t SETRXPND               :1;                                /*!< Set bit RXPND */
  uint32_t SETTXPND               :1;                                /*!< Set bit TXPND */
  uint32_t SETRXUPD               :1;                                /*!< Set bit RXUPD */
  uint32_t SETNEWDAT              :1;                                /*!< Set bit NEWDAT */
  uint32_t SETMSGLST              :1;                                /*!< Set bit MSGLST */
  uint32_t SETMSGVAL              :1;                                /*!< Set bit MSGVAL */
  uint32_t SETRTSEL               :1;                                /*!< Set bit RTSEL */
  uint32_t SETRXEN                :1;                                /*!< Set bit RXEN */
  uint32_t SETTXRQ                :1;                                /*!< Set bit TXRQ */
  uint32_t SETTXEN0               :1;                                /*!< Set bit TXEN0 */
  uint32_t SETTXEN1               :1;                                /*!< Set bit TXEN1 */
  uint32_t SETDIR                 :1;                                /*!< Set bit DIR */
} _CANMSG_Msg_MOCTR_bits;

/* Bit field positions: */
#define CANMSG_Msg_MOCTR_RESRXPND_Pos         0                      /*!< Reset bit RXPND */
#define CANMSG_Msg_MOCTR_RESTXPND_Pos         1                      /*!< Reset bit TXPND */
#define CANMSG_Msg_MOCTR_RESRXUPD_Pos         2                      /*!< Reset bit RXUPD */
#define CANMSG_Msg_MOCTR_RESNEWDAT_Pos        3                      /*!< Reset bit NEWDAT */
#define CANMSG_Msg_MOCTR_RESMSGLST_Pos        4                      /*!< Reset bit MSGLST */
#define CANMSG_Msg_MOCTR_RESMSGVAL_Pos        5                      /*!< Reset bit MSGVAL */
#define CANMSG_Msg_MOCTR_RESRTSEL_Pos         6                      /*!< Reset bit RTSEL */
#define CANMSG_Msg_MOCTR_RESRXEN_Pos          7                      /*!< Reset bit RXEN */
#define CANMSG_Msg_MOCTR_RESTXRQ_Pos          8                      /*!< Reset bit TXRQ */
#define CANMSG_Msg_MOCTR_RESTXEN0_Pos         9                      /*!< Reset bit TXEN0 */
#define CANMSG_Msg_MOCTR_RESTXEN1_Pos         10                     /*!< Reset bit TXEN1 */
#define CANMSG_Msg_MOCTR_RESDIR_Pos           11                     /*!< Reset bit DIR */
#define CANMSG_Msg_MOCTR_SETRXPND_Pos         16                     /*!< Set bit RXPND */
#define CANMSG_Msg_MOCTR_SETTXPND_Pos         17                     /*!< Set bit TXPND */
#define CANMSG_Msg_MOCTR_SETRXUPD_Pos         18                     /*!< Set bit RXUPD */
#define CANMSG_Msg_MOCTR_SETNEWDAT_Pos        19                     /*!< Set bit NEWDAT */
#define CANMSG_Msg_MOCTR_SETMSGLST_Pos        20                     /*!< Set bit MSGLST */
#define CANMSG_Msg_MOCTR_SETMSGVAL_Pos        21                     /*!< Set bit MSGVAL */
#define CANMSG_Msg_MOCTR_SETRTSEL_Pos         22                     /*!< Set bit RTSEL */
#define CANMSG_Msg_MOCTR_SETRXEN_Pos          23                     /*!< Set bit RXEN */
#define CANMSG_Msg_MOCTR_SETTXRQ_Pos          24                     /*!< Set bit TXRQ */
#define CANMSG_Msg_MOCTR_SETTXEN0_Pos         25                     /*!< Set bit TXEN0 */
#define CANMSG_Msg_MOCTR_SETTXEN1_Pos         26                     /*!< Set bit TXEN1 */
#define CANMSG_Msg_MOCTR_SETDIR_Pos           27                     /*!< Set bit DIR */

/* Bit field masks: */
#define CANMSG_Msg_MOCTR_RESRXPND_Msk         0x00000001UL           /*!< Reset bit RXPND */
#define CANMSG_Msg_MOCTR_RESTXPND_Msk         0x00000002UL           /*!< Reset bit TXPND */
#define CANMSG_Msg_MOCTR_RESRXUPD_Msk         0x00000004UL           /*!< Reset bit RXUPD */
#define CANMSG_Msg_MOCTR_RESNEWDAT_Msk        0x00000008UL           /*!< Reset bit NEWDAT */
#define CANMSG_Msg_MOCTR_RESMSGLST_Msk        0x00000010UL           /*!< Reset bit MSGLST */
#define CANMSG_Msg_MOCTR_RESMSGVAL_Msk        0x00000020UL           /*!< Reset bit MSGVAL */
#define CANMSG_Msg_MOCTR_RESRTSEL_Msk         0x00000040UL           /*!< Reset bit RTSEL */
#define CANMSG_Msg_MOCTR_RESRXEN_Msk          0x00000080UL           /*!< Reset bit RXEN */
#define CANMSG_Msg_MOCTR_RESTXRQ_Msk          0x00000100UL           /*!< Reset bit TXRQ */
#define CANMSG_Msg_MOCTR_RESTXEN0_Msk         0x00000200UL           /*!< Reset bit TXEN0 */
#define CANMSG_Msg_MOCTR_RESTXEN1_Msk         0x00000400UL           /*!< Reset bit TXEN1 */
#define CANMSG_Msg_MOCTR_RESDIR_Msk           0x00000800UL           /*!< Reset bit DIR */
#define CANMSG_Msg_MOCTR_SETRXPND_Msk         0x00010000UL           /*!< Set bit RXPND */
#define CANMSG_Msg_MOCTR_SETTXPND_Msk         0x00020000UL           /*!< Set bit TXPND */
#define CANMSG_Msg_MOCTR_SETRXUPD_Msk         0x00040000UL           /*!< Set bit RXUPD */
#define CANMSG_Msg_MOCTR_SETNEWDAT_Msk        0x00080000UL           /*!< Set bit NEWDAT */
#define CANMSG_Msg_MOCTR_SETMSGLST_Msk        0x00100000UL           /*!< Set bit MSGLST */
#define CANMSG_Msg_MOCTR_SETMSGVAL_Msk        0x00200000UL           /*!< Set bit MSGVAL */
#define CANMSG_Msg_MOCTR_SETRTSEL_Msk         0x00400000UL           /*!< Set bit RTSEL */
#define CANMSG_Msg_MOCTR_SETRXEN_Msk          0x00800000UL           /*!< Set bit RXEN */
#define CANMSG_Msg_MOCTR_SETTXRQ_Msk          0x01000000UL           /*!< Set bit TXRQ */
#define CANMSG_Msg_MOCTR_SETTXEN0_Msk         0x02000000UL           /*!< Set bit TXEN0 */
#define CANMSG_Msg_MOCTR_SETTXEN1_Msk         0x04000000UL           /*!< Set bit TXEN1 */
#define CANMSG_Msg_MOCTR_SETDIR_Msk           0x08000000UL           /*!< Set bit DIR */

/*-- Msg: MOSTAT: Status register of the message object 0 -----------------------------------------------------*/
typedef struct {
  uint32_t RXPND                  :1;                                /*!< Indicator deadline */
  uint32_t TXPND                  :1;                                /*!< Indicator end of transmission */
  uint32_t RXUPD                  :1;                                /*!< Indicator changes */
  uint32_t NEWDAT                 :1;                                /*!< New data indicator */
  uint32_t MSGLST                 :1;                                /*!< Bit message loss */
  uint32_t MSGVAL                 :1;                                /*!< Activity bit of the message object 0 */
  uint32_t RTSEL                  :1;                                /*!< The indication of transmission / reception */
  uint32_t RXEN                   :1;                                /*!< Bits allow reception */
  uint32_t TXRQ                   :1;                                /*!< Initiate transmission */
  uint32_t TXEN0                  :1;                                /*!< Enable bit transmission frame */
  uint32_t TXEN1                  :1;                                /*!< Enable bit transmission frame */
  uint32_t DIR                    :1;                                /*!< Bit allocation */
  uint32_t LIST                   :4;                                /*!< Number list  the message object 0 */
  uint32_t PPREV                  :8;                                /*!< Pointer to the previous entry */
  uint32_t PNEXT                  :8;                                /*!< Pointer to the next item in the list */
} _CANMSG_Msg_MOSTAT_bits;

/* Bit field positions: */
#define CANMSG_Msg_MOSTAT_RXPND_Pos           0                      /*!< Indicator deadline */
#define CANMSG_Msg_MOSTAT_TXPND_Pos           1                      /*!< Indicator end of transmission */
#define CANMSG_Msg_MOSTAT_RXUPD_Pos           2                      /*!< Indicator changes */
#define CANMSG_Msg_MOSTAT_NEWDAT_Pos          3                      /*!< New data indicator */
#define CANMSG_Msg_MOSTAT_MSGLST_Pos          4                      /*!< Bit message loss */
#define CANMSG_Msg_MOSTAT_MSGVAL_Pos          5                      /*!< Activity bit of the message object 0 */
#define CANMSG_Msg_MOSTAT_RTSEL_Pos           6                      /*!< The indication of transmission / reception */
#define CANMSG_Msg_MOSTAT_RXEN_Pos            7                      /*!< Bits allow reception */
#define CANMSG_Msg_MOSTAT_TXRQ_Pos            8                      /*!< Initiate transmission */
#define CANMSG_Msg_MOSTAT_TXEN0_Pos           9                      /*!< Enable bit transmission frame */
#define CANMSG_Msg_MOSTAT_TXEN1_Pos           10                     /*!< Enable bit transmission frame */
#define CANMSG_Msg_MOSTAT_DIR_Pos             11                     /*!< Bit allocation */
#define CANMSG_Msg_MOSTAT_LIST_Pos            12                     /*!< Number list  the message object 0 */
#define CANMSG_Msg_MOSTAT_PPREV_Pos           16                     /*!< Pointer to the previous entry */
#define CANMSG_Msg_MOSTAT_PNEXT_Pos           24                     /*!< Pointer to the next item in the list */

/* Bit field masks: */
#define CANMSG_Msg_MOSTAT_RXPND_Msk           0x00000001UL           /*!< Indicator deadline */
#define CANMSG_Msg_MOSTAT_TXPND_Msk           0x00000002UL           /*!< Indicator end of transmission */
#define CANMSG_Msg_MOSTAT_RXUPD_Msk           0x00000004UL           /*!< Indicator changes */
#define CANMSG_Msg_MOSTAT_NEWDAT_Msk          0x00000008UL           /*!< New data indicator */
#define CANMSG_Msg_MOSTAT_MSGLST_Msk          0x00000010UL           /*!< Bit message loss */
#define CANMSG_Msg_MOSTAT_MSGVAL_Msk          0x00000020UL           /*!< Activity bit of the message object 0 */
#define CANMSG_Msg_MOSTAT_RTSEL_Msk           0x00000040UL           /*!< The indication of transmission / reception */
#define CANMSG_Msg_MOSTAT_RXEN_Msk            0x00000080UL           /*!< Bits allow reception */
#define CANMSG_Msg_MOSTAT_TXRQ_Msk            0x00000100UL           /*!< Initiate transmission */
#define CANMSG_Msg_MOSTAT_TXEN0_Msk           0x00000200UL           /*!< Enable bit transmission frame */
#define CANMSG_Msg_MOSTAT_TXEN1_Msk           0x00000400UL           /*!< Enable bit transmission frame */
#define CANMSG_Msg_MOSTAT_DIR_Msk             0x00000800UL           /*!< Bit allocation */
#define CANMSG_Msg_MOSTAT_LIST_Msk            0x0000F000UL           /*!< Number list  the message object 0 */
#define CANMSG_Msg_MOSTAT_PPREV_Msk           0x00FF0000UL           /*!< Pointer to the previous entry */
#define CANMSG_Msg_MOSTAT_PNEXT_Msk           0xFF000000UL           /*!< Pointer to the next item in the list */

//Cluster Msg:
typedef struct {
  union { 
  /*!< Register control the operation of the message object 0 */
    __IO uint32_t MOFCR;                                             /*!< MOFCR : type used for word access */
    __IO _CANMSG_Msg_MOFCR_bits MOFCR_bit;                           /*!< MOFCR_bit: structure used for bit access */
  };
  union { 
  /*!< Pointer register FIFO / gateway message object 0 */
    __IO uint32_t MOFGPR;                                            /*!< MOFGPR : type used for word access */
    __IO _CANMSG_Msg_MOFGPR_bits MOFGPR_bit;                         /*!< MOFGPR_bit: structure used for bit access */
  };
  union { 
  /*!< Pointer register interrupt message object 0 */
    __IO uint32_t MOIPR;                                             /*!< MOIPR : type used for word access */
    __IO _CANMSG_Msg_MOIPR_bits MOIPR_bit;                           /*!< MOIPR_bit: structure used for bit access */
  };
  union { 
  /*!< Mask register message object 0 */
    __IO uint32_t MOAMR;                                             /*!< MOAMR : type used for word access */
    __IO _CANMSG_Msg_MOAMR_bits MOAMR_bit;                           /*!< MOAMR_bit: structure used for bit access */
  };
  union { 
  /*!< Low data registers of the message object 0 */
    __IO uint32_t MODATAL;                                           /*!< MODATAL : type used for word access */
    __IO _CANMSG_Msg_MODATAL_bits MODATAL_bit;                       /*!< MODATAL_bit: structure used for bit access */
  };
  union { 
  /*!< High data registers of the message object 0 */
    __IO uint32_t MODATAH;                                           /*!< MODATAH : type used for word access */
    __IO _CANMSG_Msg_MODATAH_bits MODATAH_bit;                       /*!< MODATAH_bit: structure used for bit access */
  };
  union { 
  /*!< Register arbitration message object 0 */
    __IO uint32_t MOAR;                                              /*!< MOAR : type used for word access */
    __IO _CANMSG_Msg_MOAR_bits MOAR_bit;                             /*!< MOAR_bit: structure used for bit access */
  };
  union {
      union { 
                                                                     /*!< Control register Message object 0 */
        __O uint32_t MOCTR;                                           /*!< MOCTR : type used for word access */
        __O _CANMSG_Msg_MOCTR_bits MOCTR_bit;                        /*!< MOCTR_bit: structure used for bit access */
      };
      union { 
                                                                     /*!< Status register of the message object 0 */
        __I uint32_t MOSTAT;                                           /*!< MOSTAT : type used for word access */
        __I _CANMSG_Msg_MOSTAT_bits MOSTAT_bit;                      /*!< MOSTAT_bit: structure used for bit access */
      };
  };
} _CANMSG_Msg_TypeDef;
typedef struct {
  _CANMSG_Msg_TypeDef Msg[128];
} CANMSG_TypeDef;


/******************************************************************************/
/*                         I2C registers                                      */
/******************************************************************************/

/*--  SDA: Data register --------------------------------------------------------------------------------------*/
typedef struct {
  uint32_t DATA                   :8;                                /*!< Data field */
} _I2C_SDA_bits;

/* Bit field positions: */
#define I2C_SDA_DATA_Pos                      0                      /*!< Data field */

/* Bit field masks: */
#define I2C_SDA_DATA_Msk                      0x000000FFUL           /*!< Data field */

/*--  ST: Status register -------------------------------------------------------------------------------------*/
typedef struct {
  uint32_t MODE                   :6;                                /*!< Status code */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t INT                    :1;                                /*!< Interrupt flag */
} _I2C_ST_bits;

/* Bit field positions: */
#define I2C_ST_MODE_Pos                       0                      /*!< Status code */
#define I2C_ST_INT_Pos                        7                      /*!< Interrupt flag */

/* Bit field masks: */
#define I2C_ST_MODE_Msk                       0x0000003FUL           /*!< Status code */
#define I2C_ST_INT_Msk                        0x00000080UL           /*!< Interrupt flag */

/* Bit field enums: */
typedef enum {
  I2C_ST_MODE_IDLE                           = 0x0UL,                /*!< General - Idle, no valid status information available */
  I2C_ST_MODE_STDONE                         = 0x1UL,                /*!< FS master - Start condition generated */
  I2C_ST_MODE_RSDONE                         = 0x2UL,                /*!< FS master - Repeated start condition generated */
  I2C_ST_MODE_IDLARL                         = 0x3UL,                /*!< FS master - Arbitration lost, unaddressed slave mode entered */
  I2C_ST_MODE_MTADPA                         = 0x4UL,                /*!< FS master transmit - Slave address sent, positive ACK */
  I2C_ST_MODE_MTADNA                         = 0x5UL,                /*!< FS master transmit - Slave address sent, negative ACK */
  I2C_ST_MODE_MTDAPA                         = 0x6UL,                /*!< FS master transmit - Data byte sent, positive ACK */
  I2C_ST_MODE_MTDANA                         = 0x7UL,                /*!< FS master transmit - Data byte sent, negative ACK */
  I2C_ST_MODE_MRADPA                         = 0x8UL,                /*!< FS master receive - Slave addres sent, positive ACK */
  I2C_ST_MODE_MRADNA                         = 0x9UL,                /*!< FS master receive - Slave addres sent, negative ACK */
  I2C_ST_MODE_MRDAPA                         = 0xAUL,                /*!< FS master receive - Data byte received, positive ACK */
  I2C_ST_MODE_MRDANA                         = 0xBUL,                /*!< FS master receive - Data byte received, negative ACK */
  I2C_ST_MODE_MTMCER                         = 0xCUL,                /*!< FS master - Mastercode transmitted, error detected (positive ACK) */
  I2C_ST_MODE_SRADPA                         = 0x10UL,               /*!< FS slave receive - Slave address received, positive ACK */
  I2C_ST_MODE_SRAAPA                         = 0x11UL,               /*!< FS slave receive - Slave address received after arbitration loss, positive ACK */
  I2C_ST_MODE_SRDAPA                         = 0x12UL,               /*!< FS slave receive - Data byte received, positive ACK */
  I2C_ST_MODE_SRDANA                         = 0x13UL,               /*!< FS slave receive - Data byte received, negative ACK */
  I2C_ST_MODE_STADPA                         = 0x14UL,               /*!< FS slave transmit - Slave address received, positive ACK */
  I2C_ST_MODE_STAAPA                         = 0x15UL,               /*!< FS slave transmit - Slave address received, negative ACK */
  I2C_ST_MODE_STDAPA                         = 0x16UL,               /*!< FS slave transmit - Data byte sent, positive ACK */
  I2C_ST_MODE_STDANA                         = 0x17UL,               /*!< FS slave transmit - Data byte sent, negative ACK */
  I2C_ST_MODE_SATADP                         = 0x18UL,               /*!< FS slave transmit alert response - Alert response address received, positive ACK */
  I2C_ST_MODE_SATAAP                         = 0x19UL,               /*!< FS slave transmit alert response - Alert response address received after arbitration loss, positive ACK */
  I2C_ST_MODE_SATDAP                         = 0x1AUL,               /*!< FS slave transmit alert response - Alert response data byte sent, positive ACK */
  I2C_ST_MODE_SATDAN                         = 0x1BUL,               /*!< FS slave transmit alert response - Alert response data byte sent, negative ACK */
  I2C_ST_MODE_SSTOP                          = 0x1CUL,               /*!< FS slave - Slave mode stop condition detected */
  I2C_ST_MODE_SGADPA                         = 0x1DUL,               /*!< FS slave - Global call address received, positive ACK */
  I2C_ST_MODE_SDAAPA                         = 0x1EUL,               /*!< FS slave - Global call address received after arbitration loss, positive ACK */
  I2C_ST_MODE_BERROR                         = 0x1FUL,               /*!< General - Bus error detected (invalid start or stop condition */
  I2C_ST_MODE_HMTMCOK                        = 0x21UL,               /*!< HS master - Master code transmitted OK - switched to HS mode */
  I2C_ST_MODE_HRSDONE                        = 0x22UL,               /*!< HS master - Repeated start condition generated */
  I2C_ST_MODE_HIDLARL                        = 0x23UL,               /*!< HS master - Arbitration lost, HS unaddressed slave mode entered */
  I2C_ST_MODE_HMTADPA                        = 0x24UL,               /*!< HS master transmit - Slave address sent, positive ACK */
  I2C_ST_MODE_HMTADNA                        = 0x25UL,               /*!< HS master transmit - Slave address sent, negative ACK */
  I2C_ST_MODE_HMTDAPA                        = 0x26UL,               /*!< HS master transmit - Data byte sent, positive ACK */
  I2C_ST_MODE_HMTDANA                        = 0x27UL,               /*!< HS master transmit - Data byte sent, negative ACK */
  I2C_ST_MODE_HMRADPA                        = 0x28UL,               /*!< HS master receive - Slave address sent, positive ACK */
  I2C_ST_MODE_HMRADNA                        = 0x29UL,               /*!< HS master receive - Slave address sent, negative ACK */
  I2C_ST_MODE_HMRDAPA                        = 0x2AUL,               /*!< HS master receive - Data byte received, positive ACK */
  I2C_ST_MODE_HMRDANA                        = 0x2BUL,               /*!< HS master receive - Data byte received, negative ACK */
  I2C_ST_MODE_HSRADPA                        = 0x30UL,               /*!< HS slave receive - Slave address received, positive ACK */
  I2C_ST_MODE_HSRDAPA                        = 0x32UL,               /*!< HS slave receive - Data byte received, positive ACK */
  I2C_ST_MODE_HSRDANA                        = 0x33UL,               /*!< HS slave receive - Data byte received, negative ACK */
  I2C_ST_MODE_HSTADPA                        = 0x34UL,               /*!< HS slave transmit - Slave address received, positive ACK */
  I2C_ST_MODE_HSTDAPA                        = 0x36UL,               /*!< HS slave transmit - Data byte sent, positive ACK */
  I2C_ST_MODE_HSTDANA                        = 0x37UL,               /*!< HS slave transmit - Data byte sent, negative ACK */
} I2C_ST_MODE_Enum;

/*--  CST: Status and control register ------------------------------------------------------------------------*/
typedef struct {
  uint32_t BB                     :1;                                /*!< Flag employment bus */
  uint32_t TOCDIV                 :2;                                /*!< Coeff. div */
  uint32_t TOERR                  :1;                                /*!< Flag error simple bus */
  uint32_t TSDA                   :1;                                /*!< Bit test SDA */
  uint32_t TGSCL                  :1;                                /*!< Bit switch SCL */
  uint32_t PECNEXT                :1;                                /*!< Bit control transmit CRC */
  uint32_t PECFAULT               :1;                                /*!< Error flag */
} _I2C_CST_bits;

/* Bit field positions: */
#define I2C_CST_BB_Pos                        0                      /*!< Flag employment bus */
#define I2C_CST_TOCDIV_Pos                    1                      /*!< Coeff. div */
#define I2C_CST_TOERR_Pos                     3                      /*!< Flag error simple bus */
#define I2C_CST_TSDA_Pos                      4                      /*!< Bit test SDA */
#define I2C_CST_TGSCL_Pos                     5                      /*!< Bit switch SCL */
#define I2C_CST_PECNEXT_Pos                   6                      /*!< Bit control transmit CRC */
#define I2C_CST_PECFAULT_Pos                  7                      /*!< Error flag */

/* Bit field masks: */
#define I2C_CST_BB_Msk                        0x00000001UL           /*!< Flag employment bus */
#define I2C_CST_TOCDIV_Msk                    0x00000006UL           /*!< Coeff. div */
#define I2C_CST_TOERR_Msk                     0x00000008UL           /*!< Flag error simple bus */
#define I2C_CST_TSDA_Msk                      0x00000010UL           /*!< Bit test SDA */
#define I2C_CST_TGSCL_Msk                     0x00000020UL           /*!< Bit switch SCL */
#define I2C_CST_PECNEXT_Msk                   0x00000040UL           /*!< Bit control transmit CRC */
#define I2C_CST_PECFAULT_Msk                  0x00000080UL           /*!< Error flag */

/* Bit field enums: */
typedef enum {
  I2C_CST_TOCDIV_Disable                     = 0x0UL,                /*!< disable clock */
  I2C_CST_TOCDIV_Div4                        = 0x1UL,                /*!< clock divided by 4 */
  I2C_CST_TOCDIV_Div8                        = 0x2UL,                /*!< clock divided by 8 */
  I2C_CST_TOCDIV_Div16                       = 0x3UL,                /*!< clock divided by 16 */
} I2C_CST_TOCDIV_Enum;

/*--  CTL0: Control register 0 --------------------------------------------------------------------------------*/
typedef struct {
  uint32_t START                  :1;                                /*!< Start bit */
  uint32_t STOP                   :1;                                /*!< Stop bit */
  uint32_t INTEN                  :1;                                /*!< Interrupt enable bit */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t ACK                    :1;                                /*!< Acknowledgment bit reception */
  uint32_t GCMEN                  :1;                                /*!< Control bit part a response to the general call address */
  uint32_t SMBARE                 :1;                                /*!< Control bit part a response to the response address */
  uint32_t CLRST                  :1;                                /*!< Bit reset flag interrupt */
} _I2C_CTL0_bits;

/* Bit field positions: */
#define I2C_CTL0_START_Pos                    0                      /*!< Start bit */
#define I2C_CTL0_STOP_Pos                     1                      /*!< Stop bit */
#define I2C_CTL0_INTEN_Pos                    2                      /*!< Interrupt enable bit */
#define I2C_CTL0_ACK_Pos                      4                      /*!< Acknowledgment bit reception */
#define I2C_CTL0_GCMEN_Pos                    5                      /*!< Control bit part a response to the general call address */
#define I2C_CTL0_SMBARE_Pos                   6                      /*!< Control bit part a response to the response address */
#define I2C_CTL0_CLRST_Pos                    7                      /*!< Bit reset flag interrupt */

/* Bit field masks: */
#define I2C_CTL0_START_Msk                    0x00000001UL           /*!< Start bit */
#define I2C_CTL0_STOP_Msk                     0x00000002UL           /*!< Stop bit */
#define I2C_CTL0_INTEN_Msk                    0x00000004UL           /*!< Interrupt enable bit */
#define I2C_CTL0_ACK_Msk                      0x00000010UL           /*!< Acknowledgment bit reception */
#define I2C_CTL0_GCMEN_Msk                    0x00000020UL           /*!< Control bit part a response to the general call address */
#define I2C_CTL0_SMBARE_Msk                   0x00000040UL           /*!< Control bit part a response to the response address */
#define I2C_CTL0_CLRST_Msk                    0x00000080UL           /*!< Bit reset flag interrupt */

/*--  ADDR: Register own address ------------------------------------------------------------------------------*/
typedef struct {
  uint32_t ADDR                   :7;                                /*!< Own 7-bit address */
  uint32_t SAEN                   :1;                                /*!< Enable bit address recognition */
} _I2C_ADDR_bits;

/* Bit field positions: */
#define I2C_ADDR_ADDR_Pos                     0                      /*!< Own 7-bit address */
#define I2C_ADDR_SAEN_Pos                     7                      /*!< Enable bit address recognition */

/* Bit field masks: */
#define I2C_ADDR_ADDR_Msk                     0x0000007FUL           /*!< Own 7-bit address */
#define I2C_ADDR_SAEN_Msk                     0x00000080UL           /*!< Enable bit address recognition */

/*--  CTL1: Control register 1 --------------------------------------------------------------------------------*/
typedef struct {
  uint32_t ENABLE                 :1;                                /*!< Enable I2C */
  uint32_t SCLFRQ                 :7;                                /*!< Field frequency selection signal at pin SCL in master mode (bits [6:0]) */
} _I2C_CTL1_bits;

/* Bit field positions: */
#define I2C_CTL1_ENABLE_Pos                   0                      /*!< Enable I2C */
#define I2C_CTL1_SCLFRQ_Pos                   1                      /*!< Field frequency selection signal at pin SCL in master mode (bits [6:0]) */

/* Bit field masks: */
#define I2C_CTL1_ENABLE_Msk                   0x00000001UL           /*!< Enable I2C */
#define I2C_CTL1_SCLFRQ_Msk                   0x000000FEUL           /*!< Field frequency selection signal at pin SCL in master mode (bits [6:0]) */

/*--  TOPR: Prescaler load register ---------------------------------------------------------------------------*/
typedef struct {
  uint32_t SMBTOPR                :8;                                /*!< Prescaler reload value field */
} _I2C_TOPR_bits;

/* Bit field positions: */
#define I2C_TOPR_SMBTOPR_Pos                  0                      /*!< Prescaler reload value field */

/* Bit field masks: */
#define I2C_TOPR_SMBTOPR_Msk                  0x000000FFUL           /*!< Prescaler reload value field */

/*--  CTL2: Control register 2 --------------------------------------------------------------------------------*/
typedef struct {
  uint32_t S10ADR                 :3;                                /*!< Upper bits of 10-bit slave address */
  uint32_t S10EN                  :1;                                /*!< Bit enabled 10-bit addressing slave */
  uint32_t HSDIV                  :4;                                /*!< Field frequency selection signal at pin SCL in HS master mode (bits [3:0]) */
} _I2C_CTL2_bits;

/* Bit field positions: */
#define I2C_CTL2_S10ADR_Pos                   0                      /*!< Upper bits of 10-bit slave address */
#define I2C_CTL2_S10EN_Pos                    3                      /*!< Bit enabled 10-bit addressing slave */
#define I2C_CTL2_HSDIV_Pos                    4                      /*!< Field frequency selection signal at pin SCL in HS master mode (bits [3:0]) */

/* Bit field masks: */
#define I2C_CTL2_S10ADR_Msk                   0x00000007UL           /*!< Upper bits of 10-bit slave address */
#define I2C_CTL2_S10EN_Msk                    0x00000008UL           /*!< Bit enabled 10-bit addressing slave */
#define I2C_CTL2_HSDIV_Msk                    0x000000F0UL           /*!< Field frequency selection signal at pin SCL in HS master mode (bits [3:0]) */

/*--  CTL3: Control register 3 --------------------------------------------------------------------------------*/
typedef struct {
  uint32_t SCLFRQ                 :8;                                /*!< Field frequency selection signal at pin SCL in master mode (bits [14:7]) */
} _I2C_CTL3_bits;

/* Bit field positions: */
#define I2C_CTL3_SCLFRQ_Pos                   0                      /*!< Field frequency selection signal at pin SCL in master mode (bits [14:7]) */

/* Bit field masks: */
#define I2C_CTL3_SCLFRQ_Msk                   0x000000FFUL           /*!< Field frequency selection signal at pin SCL in master mode (bits [14:7]) */

/*--  CTL4: Control Register 4 --------------------------------------------------------------------------------*/
typedef struct {
  uint32_t HSDIV                  :8;                                /*!< Field frequency selection signal at pin SCL in HS master mode (bits [11:4]) */
} _I2C_CTL4_bits;

/* Bit field positions: */
#define I2C_CTL4_HSDIV_Pos                    0                      /*!< Field frequency selection signal at pin SCL in HS master mode (bits [11:4]) */

/* Bit field masks: */
#define I2C_CTL4_HSDIV_Msk                    0x000000FFUL           /*!< Field frequency selection signal at pin SCL in HS master mode (bits [11:4]) */


typedef struct {
  union {                                                               /*!< Data register */
    __IO uint32_t SDA;                                               /*!< SDA    : type used for word access */
    __IO _I2C_SDA_bits  SDA_bit;                                     /*!< SDA_bit: structure used for bit access */
  };
  union {                                                               /*!< Status register */
    __I uint32_t ST;                                                 /*!< ST    : type used for word access */
    __I _I2C_ST_bits  ST_bit;                                        /*!< ST_bit: structure used for bit access */
  };
  union {                                                               /*!< Status and control register */
    __IO uint32_t CST;                                               /*!< CST    : type used for word access */
    __IO _I2C_CST_bits  CST_bit;                                     /*!< CST_bit: structure used for bit access */
  };
  union {                                                               /*!< Control register 0 */
    __IO uint32_t CTL0;                                              /*!< CTL0    : type used for word access */
    __IO _I2C_CTL0_bits  CTL0_bit;                                   /*!< CTL0_bit: structure used for bit access */
  };
  union {                                                               /*!< Register own address */
    __IO uint32_t ADDR;                                              /*!< ADDR    : type used for word access */
    __IO _I2C_ADDR_bits  ADDR_bit;                                   /*!< ADDR_bit: structure used for bit access */
  };
  union {                                                               /*!< Control register 1 */
    __IO uint32_t CTL1;                                              /*!< CTL1    : type used for word access */
    __IO _I2C_CTL1_bits  CTL1_bit;                                   /*!< CTL1_bit: structure used for bit access */
  };
  union {                                                               /*!< Prescaler load register */
    __IO uint32_t TOPR;                                              /*!< TOPR    : type used for word access */
    __IO _I2C_TOPR_bits  TOPR_bit;                                   /*!< TOPR_bit: structure used for bit access */
  };
  union {                                                               /*!< Control register 2 */
    __IO uint32_t CTL2;                                              /*!< CTL2    : type used for word access */
    __IO _I2C_CTL2_bits  CTL2_bit;                                   /*!< CTL2_bit: structure used for bit access */
  };
  union {                                                               /*!< Control register 3 */
    __IO uint32_t CTL3;                                              /*!< CTL3    : type used for word access */
    __IO _I2C_CTL3_bits  CTL3_bit;                                   /*!< CTL3_bit: structure used for bit access */
  };
  union {                                                               /*!< Control Register 4 */
    __IO uint32_t CTL4;                                              /*!< CTL4    : type used for word access */
    __IO _I2C_CTL4_bits  CTL4_bit;                                   /*!< CTL4_bit: structure used for bit access */
  };
} I2C_TypeDef;


/******************************************************************************/
/*                         CMP registers                                      */
/******************************************************************************/

/*--  DACCTL: DAC control register ----------------------------------------------------------------------------*/
typedef struct {
  uint32_t REF0                   :4;                                /*!<  */
  uint32_t                        :4;                                /*!< RESERVED */
  uint32_t REF1                   :4;                                /*!<  */
  uint32_t                        :4;                                /*!< RESERVED */
  uint32_t EN                     :1;                                /*!<  */
} _CMP_DACCTL_bits;

/* Bit field positions: */
#define CMP_DACCTL_REF0_Pos                   0                      /*!<  */
#define CMP_DACCTL_REF1_Pos                   8                      /*!<  */
#define CMP_DACCTL_EN_Pos                     16                     /*!<  */

/* Bit field masks: */
#define CMP_DACCTL_REF0_Msk                   0x0000000FUL           /*!<  */
#define CMP_DACCTL_REF1_Msk                   0x00000F00UL           /*!<  */
#define CMP_DACCTL_EN_Msk                     0x00010000UL           /*!<  */

/*--  ACMP0CTL: ACMP0 control register ------------------------------------------------------------------------*/
typedef struct {
  uint32_t EN                     :1;                                /*!<  */
  uint32_t RST                    :1;                                /*!<  */
  uint32_t INV                    :1;                                /*!<  */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t SELREF                 :2;                                /*!<  */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t INTSRC                 :3;                                /*!<  */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t TRIGSRC                :3;                                /*!<  */
} _CMP_ACMP0CTL_bits;

/* Bit field positions: */
#define CMP_ACMP0CTL_EN_Pos                   0                      /*!<  */
#define CMP_ACMP0CTL_RST_Pos                  1                      /*!<  */
#define CMP_ACMP0CTL_INV_Pos                  2                      /*!<  */
#define CMP_ACMP0CTL_SELREF_Pos               4                      /*!<  */
#define CMP_ACMP0CTL_INTSRC_Pos               8                      /*!<  */
#define CMP_ACMP0CTL_TRIGSRC_Pos              12                     /*!<  */

/* Bit field masks: */
#define CMP_ACMP0CTL_EN_Msk                   0x00000001UL           /*!<  */
#define CMP_ACMP0CTL_RST_Msk                  0x00000002UL           /*!<  */
#define CMP_ACMP0CTL_INV_Msk                  0x00000004UL           /*!<  */
#define CMP_ACMP0CTL_SELREF_Msk               0x00000030UL           /*!<  */
#define CMP_ACMP0CTL_INTSRC_Msk               0x00000700UL           /*!<  */
#define CMP_ACMP0CTL_TRIGSRC_Msk              0x00007000UL           /*!<  */

/*--  ACMP1CTL: ACMP1 control register ------------------------------------------------------------------------*/
typedef struct {
  uint32_t EN                     :1;                                /*!<  */
  uint32_t RST                    :1;                                /*!<  */
  uint32_t INV                    :1;                                /*!<  */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t SELREF                 :2;                                /*!<  */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t INTSRC                 :3;                                /*!<  */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t TRIGSRC                :3;                                /*!<  */
} _CMP_ACMP1CTL_bits;

/* Bit field positions: */
#define CMP_ACMP1CTL_EN_Pos                   0                      /*!<  */
#define CMP_ACMP1CTL_RST_Pos                  1                      /*!<  */
#define CMP_ACMP1CTL_INV_Pos                  2                      /*!<  */
#define CMP_ACMP1CTL_SELREF_Pos               4                      /*!<  */
#define CMP_ACMP1CTL_INTSRC_Pos               8                      /*!<  */
#define CMP_ACMP1CTL_TRIGSRC_Pos              12                     /*!<  */

/* Bit field masks: */
#define CMP_ACMP1CTL_EN_Msk                   0x00000001UL           /*!<  */
#define CMP_ACMP1CTL_RST_Msk                  0x00000002UL           /*!<  */
#define CMP_ACMP1CTL_INV_Msk                  0x00000004UL           /*!<  */
#define CMP_ACMP1CTL_SELREF_Msk               0x00000030UL           /*!<  */
#define CMP_ACMP1CTL_INTSRC_Msk               0x00000700UL           /*!<  */
#define CMP_ACMP1CTL_TRIGSRC_Msk              0x00007000UL           /*!<  */

/*--  ACSTATUS: ACMP status -----------------------------------------------------------------------------------*/
typedef struct {
  uint32_t STATE0                 :1;                                /*!<  */
  uint32_t STATE1                 :1;                                /*!<  */
} _CMP_ACSTATUS_bits;

/* Bit field positions: */
#define CMP_ACSTATUS_STATE0_Pos               0                      /*!<  */
#define CMP_ACSTATUS_STATE1_Pos               1                      /*!<  */

/* Bit field masks: */
#define CMP_ACSTATUS_STATE0_Msk               0x00000001UL           /*!<  */
#define CMP_ACSTATUS_STATE1_Msk               0x00000002UL           /*!<  */

/*--  INTEN: Interrupt enable ---------------------------------------------------------------------------------*/
typedef struct {
  uint32_t CMP0                   :1;                                /*!<  */
  uint32_t CMP1                   :1;                                /*!<  */
} _CMP_INTEN_bits;

/* Bit field positions: */
#define CMP_INTEN_CMP0_Pos                    0                      /*!<  */
#define CMP_INTEN_CMP1_Pos                    1                      /*!<  */

/* Bit field masks: */
#define CMP_INTEN_CMP0_Msk                    0x00000001UL           /*!<  */
#define CMP_INTEN_CMP1_Msk                    0x00000002UL           /*!<  */

/*--  RIS: Raw interrupt status -------------------------------------------------------------------------------*/
typedef struct {
  uint32_t CMP0                   :1;                                /*!<  */
  uint32_t CMP1                   :1;                                /*!<  */
} _CMP_RIS_bits;

/* Bit field positions: */
#define CMP_RIS_CMP0_Pos                      0                      /*!<  */
#define CMP_RIS_CMP1_Pos                      1                      /*!<  */

/* Bit field masks: */
#define CMP_RIS_CMP0_Msk                      0x00000001UL           /*!<  */
#define CMP_RIS_CMP1_Msk                      0x00000002UL           /*!<  */

/*--  MIS: Masked interrupt status ----------------------------------------------------------------------------*/
typedef struct {
  uint32_t CMP0                   :1;                                /*!<  */
  uint32_t CMP1                   :1;                                /*!<  */
} _CMP_MIS_bits;

/* Bit field positions: */
#define CMP_MIS_CMP0_Pos                      0                      /*!<  */
#define CMP_MIS_CMP1_Pos                      1                      /*!<  */

/* Bit field masks: */
#define CMP_MIS_CMP0_Msk                      0x00000001UL           /*!<  */
#define CMP_MIS_CMP1_Msk                      0x00000002UL           /*!<  */

/*--  ICLR: Clear interrupt status ----------------------------------------------------------------------------*/
typedef struct {
  uint32_t CMP0                   :1;                                /*!<  */
  uint32_t CMP1                   :1;                                /*!<  */
} _CMP_ICLR_bits;

/* Bit field positions: */
#define CMP_ICLR_CMP0_Pos                     0                      /*!<  */
#define CMP_ICLR_CMP1_Pos                     1                      /*!<  */

/* Bit field masks: */
#define CMP_ICLR_CMP0_Msk                     0x00000001UL           /*!<  */
#define CMP_ICLR_CMP1_Msk                     0x00000002UL           /*!<  */

typedef struct {
  union {                                                               /*!< DAC control register */
    __IO uint32_t DACCTL;                                            /*!< DACCTL    : type used for word access */
    __IO _CMP_DACCTL_bits  DACCTL_bit;                               /*!< DACCTL_bit: structure used for bit access */
  };
  union {                                                               /*!< ACMP0 control register */
    __IO uint32_t ACMP0CTL;                                           /*!< ACMP0CTL    : type used for word access */
    __IO _CMP_ACMP0CTL_bits  ACMP0CTL_bit;                           /*!< ACMP0CTL_bit: structure used for bit access */
  };
  union {                                                               /*!< ACMP1 control register */
    __IO uint32_t ACMP1CTL;                                           /*!< ACMP1CTL    : type used for word access */
    __IO _CMP_ACMP1CTL_bits  ACMP1CTL_bit;                           /*!< ACMP1CTL_bit: structure used for bit access */
  };
  union {                                                               /*!< ACMP status */
    __I uint32_t ACSTATUS;                                           /*!< ACSTATUS    : type used for word access */
    __I _CMP_ACSTATUS_bits  ACSTATUS_bit;                            /*!< ACSTATUS_bit: structure used for bit access */
  };
  union {                                                               /*!< Interrupt enable */
    __IO uint32_t INTEN;                                             /*!< INTEN    : type used for word access */
    __IO _CMP_INTEN_bits  INTEN_bit;                                 /*!< INTEN_bit: structure used for bit access */
  };
  union {                                                               /*!< Raw interrupt status */
    __I uint32_t RIS;                                                /*!< RIS    : type used for word access */
    __I _CMP_RIS_bits  RIS_bit;                                      /*!< RIS_bit: structure used for bit access */
  };
  union {                                                               /*!< Masked interrupt status */
    __I uint32_t MIS;                                                /*!< MIS    : type used for word access */
    __I _CMP_MIS_bits  MIS_bit;                                      /*!< MIS_bit: structure used for bit access */
  };
  union {                                                               /*!< Clear interrupt status */
    __O uint32_t ICLR;                                               /*!< ICLR    : type used for word access */
    __O _CMP_ICLR_bits  ICLR_bit;                                    /*!< ICLR_bit: structure used for bit access */
  };
} CMP_TypeDef;


/******************************************************************************/
/*                         PMURTC registers                                   */
/******************************************************************************/

/*--  RTC_CFG0: Config0 register ------------------------------------------------------------------------------*/
typedef struct {
  uint32_t LPCK                   :3;                                /*!< defines the refresh period of comparators, in LP and ULP */
  uint32_t FC                     :2;                                /*!< (must stay 0) force power mode of the PMU power controller submodule */
  uint32_t UVDIS                  :1;                                /*!< undervoltage disable (default is 0 - undervoltage detection enable) */
  uint32_t CD                     :1;                                /*!< lpclk pulse disable (default is 0 - pulse enabled) */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t DE                     :1;                                /*!< clkdiv output enable (defauls is 0, output disabled) */
  uint32_t EL                     :1;                                /*!< (must stay 0) forced enable levelshifters of LDO0 */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t LS                     :1;                                /*!< lpclk == 1 period duration. 0 - 15 us, 1 - 30 us */
  uint32_t AT2                    :3;                                /*!< 2 - AT2 disabled (default). 001 to 111 - freq limit: 4.7/7.8/10.9/14.1/17.2/20.3/23.4 [%] */
  uint32_t EXTOSC                 :1;                                /*!< Use external crystal oscillator and disables RC osc. (except if antitamper2 is active) */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t FREQDIFF               :9;                                /*!< Value of the antitamper 2 clock cycles difference relative to expected value */
} _PMURTC_RTC_CFG0_bits;

/* Bit field positions: */
#define PMURTC_RTC_CFG0_LPCK_Pos              0                      /*!< defines the refresh period of comparators, in LP and ULP */
#define PMURTC_RTC_CFG0_FC_Pos                3                      /*!< (must stay 0) force power mode of the PMU power controller submodule */
#define PMURTC_RTC_CFG0_UVDIS_Pos             5                      /*!< undervoltage disable (default is 0 - undervoltage detection enable) */
#define PMURTC_RTC_CFG0_CD_Pos                6                      /*!< lpclk pulse disable (default is 0 - pulse enabled) */
#define PMURTC_RTC_CFG0_DE_Pos                10                     /*!< clkdiv output enable (defauls is 0, output disabled) */
#define PMURTC_RTC_CFG0_EL_Pos                11                     /*!< (must stay 0) forced enable levelshifters of LDO0 */
#define PMURTC_RTC_CFG0_LS_Pos                13                     /*!< lpclk == 1 period duration. 0 - 15 us, 1 - 30 us */
#define PMURTC_RTC_CFG0_AT2_Pos               14                     /*!< 2 - AT2 disabled (default). 001 to 111 - freq limit: 4.7/7.8/10.9/14.1/17.2/20.3/23.4 [%] */
#define PMURTC_RTC_CFG0_EXTOSC_Pos            17                     /*!< Use external crystal oscillator and disables RC osc. (except if antitamper2 is active) */
#define PMURTC_RTC_CFG0_FREQDIFF_Pos          19                     /*!< Value of the antitamper 2 clock cycles difference relative to expected value */

/* Bit field masks: */
#define PMURTC_RTC_CFG0_LPCK_Msk              0x00000007UL           /*!< defines the refresh period of comparators, in LP and ULP */
#define PMURTC_RTC_CFG0_FC_Msk                0x00000018UL           /*!< (must stay 0) force power mode of the PMU power controller submodule */
#define PMURTC_RTC_CFG0_UVDIS_Msk             0x00000020UL           /*!< undervoltage disable (default is 0 - undervoltage detection enable) */
#define PMURTC_RTC_CFG0_CD_Msk                0x00000040UL           /*!< lpclk pulse disable (default is 0 - pulse enabled) */
#define PMURTC_RTC_CFG0_DE_Msk                0x00000400UL           /*!< clkdiv output enable (defauls is 0, output disabled) */
#define PMURTC_RTC_CFG0_EL_Msk                0x00000800UL           /*!< (must stay 0) forced enable levelshifters of LDO0 */
#define PMURTC_RTC_CFG0_LS_Msk                0x00002000UL           /*!< lpclk == 1 period duration. 0 - 15 us, 1 - 30 us */
#define PMURTC_RTC_CFG0_AT2_Msk               0x0001C000UL           /*!< 2 - AT2 disabled (default). 001 to 111 - freq limit: 4.7/7.8/10.9/14.1/17.2/20.3/23.4 [%] */
#define PMURTC_RTC_CFG0_EXTOSC_Msk            0x00020000UL           /*!< Use external crystal oscillator and disables RC osc. (except if antitamper2 is active) */
#define PMURTC_RTC_CFG0_FREQDIFF_Msk          0x0FF80000UL           /*!< Value of the antitamper 2 clock cycles difference relative to expected value */

/* Bit field enums: */
typedef enum {
  PMURTC_RTC_CFG0_FC_Def                     = 0x0UL,                /*!< default */
  PMURTC_RTC_CFG0_FC_ForceLP                 = 0x1UL,                /*!< Force low power */
  PMURTC_RTC_CFG0_FC_ForceHP                 = 0x2UL,                /*!< Force high power */
  PMURTC_RTC_CFG0_FC_ForceLP2HP              = 0x3UL,                /*!< Force high power from low power */
} PMURTC_RTC_CFG0_FC_Enum;

typedef enum {
  PMURTC_RTC_CFG0_AT2_Disable                = 0x0UL,                /*!< AT2 disabled (default) */
  PMURTC_RTC_CFG0_AT2_FREQ_4_7per            = 0x1UL,                /*!< freq limit: 4.7% */
  PMURTC_RTC_CFG0_AT2_FREQ_7_8per            = 0x2UL,                /*!< freq limit: 7.8% */
  PMURTC_RTC_CFG0_AT2_FREQ_10_9per           = 0x3UL,                /*!< freq limit: 10.9% */
  PMURTC_RTC_CFG0_AT2_FREQ_14_1per           = 0x4UL,                /*!< freq limit: 14.1% */
  PMURTC_RTC_CFG0_AT2_FREQ_17_2per           = 0x5UL,                /*!< freq limit: 17.2% */
  PMURTC_RTC_CFG0_AT2_FREQ_20_3per           = 0x6UL,                /*!< freq limit: 20.3% */
  PMURTC_RTC_CFG0_AT2_FREQ_23_4per           = 0x7UL,                /*!< freq limit: 23.4% */
} PMURTC_RTC_CFG0_AT2_Enum;

/*--  RTC_TRIMRC: Trim RC OSC register ------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :11;                               /*!< Trim for rc osc */
} _PMURTC_RTC_TRIMRC_bits;

/* Bit field positions: */
#define PMURTC_RTC_TRIMRC_VAL_Pos             0                      /*!< Trim for rc osc */

/* Bit field masks: */
#define PMURTC_RTC_TRIMRC_VAL_Msk             0x000007FFUL           /*!< Trim for rc osc */

/*--  RTC_HISTORY: History register ---------------------------------------------------------------------------*/
typedef struct {
  uint32_t WAKE0                  :1;                                /*!< when a wake event occurs, the corresponding bit is set. */
  uint32_t WAKE1                  :1;                                /*!< when a wake event occurs, the corresponding bit is set. */
  uint32_t WAKE2                  :1;                                /*!< when a wake event occurs, the corresponding bit is set. */
  uint32_t WKVBATPER              :1;                                /*!< when a wake event occurs, the corresponding bit is set. */
  uint32_t WKUVLO                 :1;                                /*!< when a wake event occurs, the corresponding bit is set. */
  uint32_t WKVFVBAK               :1;                                /*!< when a wake event occurs, the corresponding bit is set. */
  uint32_t FREQERR                :1;                                /*!< frequency error detection history */
  uint32_t TAMPER0                :1;                                /*!< Anti-tamper 0 detection  */
  uint32_t TAMPER1                :1;                                /*!< Anti-tamper 1 detection  */
  uint32_t TAMPER2                :1;                                /*!< Anti-tamper 1 detection  */
  uint32_t TIMEALARM              :1;                                /*!< time alarm: time=alarm */
} _PMURTC_RTC_HISTORY_bits;

/* Bit field positions: */
#define PMURTC_RTC_HISTORY_WAKE0_Pos          0                      /*!< when a wake event occurs, the corresponding bit is set. */
#define PMURTC_RTC_HISTORY_WAKE1_Pos          1                      /*!< when a wake event occurs, the corresponding bit is set. */
#define PMURTC_RTC_HISTORY_WAKE2_Pos          2                      /*!< when a wake event occurs, the corresponding bit is set. */
#define PMURTC_RTC_HISTORY_WKVBATPER_Pos       3                     /*!< when a wake event occurs, the corresponding bit is set. */
#define PMURTC_RTC_HISTORY_WKUVLO_Pos         4                      /*!< when a wake event occurs, the corresponding bit is set. */
#define PMURTC_RTC_HISTORY_WKVFVBAK_Pos       5                      /*!< when a wake event occurs, the corresponding bit is set. */
#define PMURTC_RTC_HISTORY_FREQERR_Pos        6                      /*!< frequency error detection history */
#define PMURTC_RTC_HISTORY_TAMPER0_Pos        7                      /*!< Anti-tamper 0 detection  */
#define PMURTC_RTC_HISTORY_TAMPER1_Pos        8                      /*!< Anti-tamper 1 detection  */
#define PMURTC_RTC_HISTORY_TAMPER2_Pos        9                      /*!< Anti-tamper 1 detection  */
#define PMURTC_RTC_HISTORY_TIMEALARM_Pos       10                    /*!< time alarm: time=alarm */

/* Bit field masks: */
#define PMURTC_RTC_HISTORY_WAKE0_Msk          0x00000001UL           /*!< when a wake event occurs, the corresponding bit is set. */
#define PMURTC_RTC_HISTORY_WAKE1_Msk          0x00000002UL           /*!< when a wake event occurs, the corresponding bit is set. */
#define PMURTC_RTC_HISTORY_WAKE2_Msk          0x00000004UL           /*!< when a wake event occurs, the corresponding bit is set. */
#define PMURTC_RTC_HISTORY_WKVBATPER_Msk       0x00000008UL          /*!< when a wake event occurs, the corresponding bit is set. */
#define PMURTC_RTC_HISTORY_WKUVLO_Msk         0x00000010UL           /*!< when a wake event occurs, the corresponding bit is set. */
#define PMURTC_RTC_HISTORY_WKVFVBAK_Msk       0x00000020UL           /*!< when a wake event occurs, the corresponding bit is set. */
#define PMURTC_RTC_HISTORY_FREQERR_Msk        0x00000040UL           /*!< frequency error detection history */
#define PMURTC_RTC_HISTORY_TAMPER0_Msk        0x00000080UL           /*!< Anti-tamper 0 detection  */
#define PMURTC_RTC_HISTORY_TAMPER1_Msk        0x00000100UL           /*!< Anti-tamper 1 detection  */
#define PMURTC_RTC_HISTORY_TAMPER2_Msk        0x00000200UL           /*!< Anti-tamper 1 detection  */
#define PMURTC_RTC_HISTORY_TIMEALARM_Msk       0x00000400UL          /*!< time alarm: time=alarm */

/*--  RTC_TIME: Time register ---------------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< Value of Time */
} _PMURTC_RTC_TIME_bits;

/* Bit field positions: */
#define PMURTC_RTC_TIME_VAL_Pos               0                      /*!< Value of Time */

/* Bit field masks: */
#define PMURTC_RTC_TIME_VAL_Msk               0xFFFFFFFFUL           /*!< Value of Time */

/*--  RTC_ALARM: Alarm register -------------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!< time value that will set alarm if t=1'b1 */
} _PMURTC_RTC_ALARM_bits;

/* Bit field positions: */
#define PMURTC_RTC_ALARM_VAL_Pos              0                      /*!< time value that will set alarm if t=1'b1 */

/* Bit field masks: */
#define PMURTC_RTC_ALARM_VAL_Msk              0xFFFFFFFFUL           /*!< time value that will set alarm if t=1'b1 */

/*--  RTC_TRIM: Trim register ---------------------------------------------------------------------------------*/
typedef struct {
  uint32_t TRIM1S                 :11;                               /*!< active oscillator (RTC_CFG0.OSCSEL) 1sec trim word (to apply each 64sec) */
  uint32_t ITRIM                  :3;                                /*!< bias current trim word for APC */
  uint32_t VTRIM                  :5;                                /*!< reference voltage offset trim word for APC */
  uint32_t TEMPTRIM               :3;                                /*!< temperature trim word for APC */
  uint32_t BYPASS                 :1;                                /*!< Crystal oscillator cmos bypass mode (default is 0 = disabled) */
  uint32_t CXT                    :4;                                /*!< bits reserved (must be 0 unless otherwise advised) */
  uint32_t CLKDIV                 :5;                                /*!< division of clk32 used as anti-tamper source and output at clkdiv */
} _PMURTC_RTC_TRIM_bits;

/* Bit field positions: */
#define PMURTC_RTC_TRIM_TRIM1S_Pos            0                      /*!< active oscillator (RTC_CFG0.OSCSEL) 1sec trim word (to apply each 64sec) */
#define PMURTC_RTC_TRIM_ITRIM_Pos             11                     /*!< bias current trim word for APC */
#define PMURTC_RTC_TRIM_VTRIM_Pos             14                     /*!< reference voltage offset trim word for APC */
#define PMURTC_RTC_TRIM_TEMPTRIM_Pos          19                     /*!< temperature trim word for APC */
#define PMURTC_RTC_TRIM_BYPASS_Pos            22                     /*!< Crystal oscillator cmos bypass mode (default is 0 = disabled) */
#define PMURTC_RTC_TRIM_CXT_Pos               23                     /*!< bits reserved (must be 0 unless otherwise advised) */
#define PMURTC_RTC_TRIM_CLKDIV_Pos            27                     /*!< division of clk32 used as anti-tamper source and output at clkdiv */

/* Bit field masks: */
#define PMURTC_RTC_TRIM_TRIM1S_Msk            0x000007FFUL           /*!< active oscillator (RTC_CFG0.OSCSEL) 1sec trim word (to apply each 64sec) */
#define PMURTC_RTC_TRIM_ITRIM_Msk             0x00003800UL           /*!< bias current trim word for APC */
#define PMURTC_RTC_TRIM_VTRIM_Msk             0x0007C000UL           /*!< reference voltage offset trim word for APC */
#define PMURTC_RTC_TRIM_TEMPTRIM_Msk          0x00380000UL           /*!< temperature trim word for APC */
#define PMURTC_RTC_TRIM_BYPASS_Msk            0x00400000UL           /*!< Crystal oscillator cmos bypass mode (default is 0 = disabled) */
#define PMURTC_RTC_TRIM_CXT_Msk               0x07800000UL           /*!< bits reserved (must be 0 unless otherwise advised) */
#define PMURTC_RTC_TRIM_CLKDIV_Msk            0xF8000000UL           /*!< division of clk32 used as anti-tamper source and output at clkdiv */

/*--  RTC_CFG1: Config1 register ------------------------------------------------------------------------------*/
typedef struct {
  uint32_t CLKSEL                 :1;                                /*!< time register clock selection (32kHz if ck=0; 1Hz if ck=1 - default) */
  uint32_t ALARMZEN               :1;                                /*!< if z=1, alarmz open drain output is active; if z=0 OD output is not active (default is z=1) */
  uint32_t ALARMEN                :1;                                /*!< enable alarm being set by time==alarm (default is 1, set possibility enabled) */
  uint32_t                        :6;                                /*!< RESERVED */
  uint32_t ULP                    :2;                                /*!< when ulp[k]=1, regulator k operates in ultra-low power (default is 00) */
  uint32_t LP                     :2;                                /*!< when lp[k]=1, regulator k operates in low power (default is 00) */
  uint32_t REGEN                  :2;                                /*!< when en[k]=1, regulator k is enabled even with alarm=0 (default is 00) */
  uint32_t REGDIS                 :2;                                /*!< when dis[k]=1, regulator k is disabled even with alarm=1 (default is 00) */
  uint32_t LDOLV                  :5;                                /*!< LDOs low voltage for low power and ultra low power. Default 000101 - 0.9V */
  uint32_t                        :1;                                /*!< RESERVED */
  uint32_t LDOHV                  :5;                                /*!< LDOs high voltage for low power and ultra low power. Default 011010 - 1.32V */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t ALARMRST               :1;                                /*!< alarm reset (resets alarm if r=1) */
} _PMURTC_RTC_CFG1_bits;

/* Bit field positions: */
#define PMURTC_RTC_CFG1_CLKSEL_Pos            0                      /*!< time register clock selection (32kHz if ck=0; 1Hz if ck=1 - default) */
#define PMURTC_RTC_CFG1_ALARMZEN_Pos          1                      /*!< if z=1, alarmz open drain output is active; if z=0 OD output is not active (default is z=1) */
#define PMURTC_RTC_CFG1_ALARMEN_Pos           2                      /*!< enable alarm being set by time==alarm (default is 1, set possibility enabled) */
#define PMURTC_RTC_CFG1_ULP_Pos               9                      /*!< when ulp[k]=1, regulator k operates in ultra-low power (default is 00) */
#define PMURTC_RTC_CFG1_LP_Pos                11                     /*!< when lp[k]=1, regulator k operates in low power (default is 00) */
#define PMURTC_RTC_CFG1_REGEN_Pos             13                     /*!< when en[k]=1, regulator k is enabled even with alarm=0 (default is 00) */
#define PMURTC_RTC_CFG1_REGDIS_Pos            15                     /*!< when dis[k]=1, regulator k is disabled even with alarm=1 (default is 00) */
#define PMURTC_RTC_CFG1_LDOLV_Pos             17                     /*!< LDOs low voltage for low power and ultra low power. Default 000101 - 0.9V */
#define PMURTC_RTC_CFG1_LDOHV_Pos             23                     /*!< LDOs high voltage for low power and ultra low power. Default 011010 - 1.32V */
#define PMURTC_RTC_CFG1_ALARMRST_Pos          30                     /*!< alarm reset (resets alarm if r=1) */

/* Bit field masks: */
#define PMURTC_RTC_CFG1_CLKSEL_Msk            0x00000001UL           /*!< time register clock selection (32kHz if ck=0; 1Hz if ck=1 - default) */
#define PMURTC_RTC_CFG1_ALARMZEN_Msk          0x00000002UL           /*!< if z=1, alarmz open drain output is active; if z=0 OD output is not active (default is z=1) */
#define PMURTC_RTC_CFG1_ALARMEN_Msk           0x00000004UL           /*!< enable alarm being set by time==alarm (default is 1, set possibility enabled) */
#define PMURTC_RTC_CFG1_ULP_Msk               0x00000600UL           /*!< when ulp[k]=1, regulator k operates in ultra-low power (default is 00) */
#define PMURTC_RTC_CFG1_LP_Msk                0x00001800UL           /*!< when lp[k]=1, regulator k operates in low power (default is 00) */
#define PMURTC_RTC_CFG1_REGEN_Msk             0x00006000UL           /*!< when en[k]=1, regulator k is enabled even with alarm=0 (default is 00) */
#define PMURTC_RTC_CFG1_REGDIS_Msk            0x00018000UL           /*!< when dis[k]=1, regulator k is disabled even with alarm=1 (default is 00) */
#define PMURTC_RTC_CFG1_LDOLV_Msk             0x003E0000UL           /*!< LDOs low voltage for low power and ultra low power. Default 000101 - 0.9V */
#define PMURTC_RTC_CFG1_LDOHV_Msk             0x0F800000UL           /*!< LDOs high voltage for low power and ultra low power. Default 011010 - 1.32V */
#define PMURTC_RTC_CFG1_ALARMRST_Msk          0x40000000UL           /*!< alarm reset (resets alarm if r=1) */

/*--  RTC_WAKECFG: Wake config register -----------------------------------------------------------------------*/
typedef struct {
  uint32_t WAKEEN                 :6;                                /*!< crystal oscillator cmos bypass mode (default is 0 = disabled) */
  uint32_t WAKEPOL                :6;                                /*!< if wp[i]=0, wake[i] is active high; if wp[i]=1, wake[i] is active low (default is wp[i]=0) */
  uint32_t DIS5                   :2;                                /*!< values that are copied to dis[1:0] when wake[5] occurs (default is 00) */
  uint32_t DIS4                   :2;                                /*!< values that are copied to dis[1:0] when wake[4] occurs (default is 00) */
  uint32_t RST5                   :2;                                /*!< reset configuration for porz1 and porz0 if wake[5] occurs (default is 00) */
  uint32_t RST4                   :2;                                /*!< reset configuration for porz1 and porz0 if wake[4] occurs (default is 00) */
  uint32_t TAMPER0EN              :1;                                /*!< Anti-tamper input 0 enable bit */
  uint32_t TAMPER1EN              :1;                                /*!< Anti-tamper input 1 enable bit */
  uint32_t TAMPER2EN              :1;                                /*!< Anti-tamper input 2 enable bit */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t VPROG                  :5;                                /*!< programming voltage for high power operation of both LDOs. Default is 010100 - 1.2V */
} _PMURTC_RTC_WAKECFG_bits;

/* Bit field positions: */
#define PMURTC_RTC_WAKECFG_WAKEEN_Pos         0                      /*!< crystal oscillator cmos bypass mode (default is 0 = disabled) */
#define PMURTC_RTC_WAKECFG_WAKEPOL_Pos        6                      /*!< if wp[i]=0, wake[i] is active high; if wp[i]=1, wake[i] is active low (default is wp[i]=0) */
#define PMURTC_RTC_WAKECFG_DIS5_Pos           12                     /*!< values that are copied to dis[1:0] when wake[5] occurs (default is 00) */
#define PMURTC_RTC_WAKECFG_DIS4_Pos           14                     /*!< values that are copied to dis[1:0] when wake[4] occurs (default is 00) */
#define PMURTC_RTC_WAKECFG_RST5_Pos           16                     /*!< reset configuration for porz1 and porz0 if wake[5] occurs (default is 00) */
#define PMURTC_RTC_WAKECFG_RST4_Pos           18                     /*!< reset configuration for porz1 and porz0 if wake[4] occurs (default is 00) */
#define PMURTC_RTC_WAKECFG_TAMPER0EN_Pos       20                    /*!< Anti-tamper input 0 enable bit */
#define PMURTC_RTC_WAKECFG_TAMPER1EN_Pos       21                    /*!< Anti-tamper input 1 enable bit */
#define PMURTC_RTC_WAKECFG_TAMPER2EN_Pos       22                    /*!< Anti-tamper input 2 enable bit */
#define PMURTC_RTC_WAKECFG_VPROG_Pos          26                     /*!< programming voltage for high power operation of both LDOs. Default is 010100 - 1.2V */

/* Bit field masks: */
#define PMURTC_RTC_WAKECFG_WAKEEN_Msk         0x0000003FUL           /*!< crystal oscillator cmos bypass mode (default is 0 = disabled) */
#define PMURTC_RTC_WAKECFG_WAKEPOL_Msk        0x00000FC0UL           /*!< if wp[i]=0, wake[i] is active high; if wp[i]=1, wake[i] is active low (default is wp[i]=0) */
#define PMURTC_RTC_WAKECFG_DIS5_Msk           0x00003000UL           /*!< values that are copied to dis[1:0] when wake[5] occurs (default is 00) */
#define PMURTC_RTC_WAKECFG_DIS4_Msk           0x0000C000UL           /*!< values that are copied to dis[1:0] when wake[4] occurs (default is 00) */
#define PMURTC_RTC_WAKECFG_RST5_Msk           0x00030000UL           /*!< reset configuration for porz1 and porz0 if wake[5] occurs (default is 00) */
#define PMURTC_RTC_WAKECFG_RST4_Msk           0x000C0000UL           /*!< reset configuration for porz1 and porz0 if wake[4] occurs (default is 00) */
#define PMURTC_RTC_WAKECFG_TAMPER0EN_Msk       0x00100000UL          /*!< Anti-tamper input 0 enable bit */
#define PMURTC_RTC_WAKECFG_TAMPER1EN_Msk       0x00200000UL          /*!< Anti-tamper input 1 enable bit */
#define PMURTC_RTC_WAKECFG_TAMPER2EN_Msk       0x00400000UL          /*!< Anti-tamper input 2 enable bit */
#define PMURTC_RTC_WAKECFG_VPROG_Msk          0x7C000000UL           /*!< programming voltage for high power operation of both LDOs. Default is 010100 - 1.2V */

/*--  RTC_REG: Register user value ----------------------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :32;                               /*!<  */
} _PMURTC_RTC_REG_bits;

/* Bit field positions: */
#define PMURTC_RTC_REG_VAL_Pos                0                      /*!<  */

/* Bit field masks: */
#define PMURTC_RTC_REG_VAL_Msk                0xFFFFFFFFUL           /*!<  */

/*--  PMU_WK3EN: PMU WKVBATPER Event Enable register ----------------------------------------------------------*/
typedef struct {
  uint32_t CMP0                   :1;                                /*!< Comparator 0 event enable for WKVBATPER */
  uint32_t CMP1                   :1;                                /*!< Comparator 1 event enable for WKVBATPER */
  uint32_t IWDG                   :1;                                /*!< Independent WatchDog event enable for WKVBATPER */
  uint32_t EXTRST                 :1;                                /*!< External reset event enable for WKVBATPER */
  uint32_t CLK1S                  :1;                                /*!< CLK1S posedge event enable for WKVBATPER */
} _PMURTC_PMU_WK3EN_bits;

/* Bit field positions: */
#define PMURTC_PMU_WK3EN_CMP0_Pos             0                      /*!< Comparator 0 event enable for WKVBATPER */
#define PMURTC_PMU_WK3EN_CMP1_Pos             1                      /*!< Comparator 1 event enable for WKVBATPER */
#define PMURTC_PMU_WK3EN_IWDG_Pos             2                      /*!< Independent WatchDog event enable for WKVBATPER */
#define PMURTC_PMU_WK3EN_EXTRST_Pos           3                      /*!< External reset event enable for WKVBATPER */
#define PMURTC_PMU_WK3EN_CLK1S_Pos            4                      /*!< CLK1S posedge event enable for WKVBATPER */

/* Bit field masks: */
#define PMURTC_PMU_WK3EN_CMP0_Msk             0x00000001UL           /*!< Comparator 0 event enable for WKVBATPER */
#define PMURTC_PMU_WK3EN_CMP1_Msk             0x00000002UL           /*!< Comparator 1 event enable for WKVBATPER */
#define PMURTC_PMU_WK3EN_IWDG_Msk             0x00000004UL           /*!< Independent WatchDog event enable for WKVBATPER */
#define PMURTC_PMU_WK3EN_EXTRST_Msk           0x00000008UL           /*!< External reset event enable for WKVBATPER */
#define PMURTC_PMU_WK3EN_CLK1S_Msk            0x00000010UL           /*!< CLK1S posedge event enable for WKVBATPER */

/*--  PMU_WK3STAT: PMU WKVBATPER Event Status register --------------------------------------------------------*/
typedef struct {
  uint32_t CMP0                   :1;                                /*!< Comaparator 0 event status for WKVBATPER */
  uint32_t CMP1                   :1;                                /*!< Comaparator 1 event status for WKVBATPER */
  uint32_t IWDG                   :1;                                /*!< Independent WatchDog event status for WKVBATPER */
  uint32_t EXTRST                 :1;                                /*!< External reset event status for WKVBATPER */
  uint32_t CLK1S                  :1;                                /*!< CLK1S posedge event status for WKVBATPER */
} _PMURTC_PMU_WK3STAT_bits;

/* Bit field positions: */
#define PMURTC_PMU_WK3STAT_CMP0_Pos           0                      /*!< Comaparator 0 event status for WKVBATPER */
#define PMURTC_PMU_WK3STAT_CMP1_Pos           1                      /*!< Comaparator 1 event status for WKVBATPER */
#define PMURTC_PMU_WK3STAT_IWDG_Pos           2                      /*!< Independent WatchDog event status for WKVBATPER */
#define PMURTC_PMU_WK3STAT_EXTRST_Pos         3                      /*!< External reset event status for WKVBATPER */
#define PMURTC_PMU_WK3STAT_CLK1S_Pos          4                      /*!< CLK1S posedge event status for WKVBATPER */

/* Bit field masks: */
#define PMURTC_PMU_WK3STAT_CMP0_Msk           0x00000001UL           /*!< Comaparator 0 event status for WKVBATPER */
#define PMURTC_PMU_WK3STAT_CMP1_Msk           0x00000002UL           /*!< Comaparator 1 event status for WKVBATPER */
#define PMURTC_PMU_WK3STAT_IWDG_Msk           0x00000004UL           /*!< Independent WatchDog event status for WKVBATPER */
#define PMURTC_PMU_WK3STAT_EXTRST_Msk         0x00000008UL           /*!< External reset event status for WKVBATPER */
#define PMURTC_PMU_WK3STAT_CLK1S_Msk          0x00000010UL           /*!< CLK1S posedge event status for WKVBATPER */

/*--  PMU_WCYC: PMU Wait cycles Register Access ---------------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :8;                                /*!< Wait cycles value */
} _PMURTC_PMU_WCYC_bits;

/* Bit field positions: */
#define PMURTC_PMU_WCYC_VAL_Pos               0                      /*!< Wait cycles value */

/* Bit field masks: */
#define PMURTC_PMU_WCYC_VAL_Msk               0x000000FFUL           /*!< Wait cycles value */

/*--  WFI_PDEN: WFI Powerdown function configuration ----------------------------------------------------------*/
typedef struct {
  uint32_t EN                     :1;                                /*!< Enable powerdown function for WFI command */
} _PMURTC_WFI_PDEN_bits;

/* Bit field positions: */
#define PMURTC_WFI_PDEN_EN_Pos                0                      /*!< Enable powerdown function for WFI command */

/* Bit field masks: */
#define PMURTC_WFI_PDEN_EN_Msk                0x00000001UL           /*!< Enable powerdown function for WFI command */

/*--  WFI_ENTR: WFI Powerdown Entry configuration -------------------------------------------------------------*/
typedef struct {
  uint32_t LDO0EN                 :1;                                /*!< LDO0 enabled after WFI command */
  uint32_t LDO1EN                 :1;                                /*!< LDO1 enabled after WFI command */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t LDO0LP                 :1;                                /*!< LDO0 in lowpower mode after WFI command */
  uint32_t LDO1LP                 :1;                                /*!< LDO1 in lowpower mode after WFI command */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t LDO0ULP                :1;                                /*!< LDO0 in ultralowpower mode after WFI command */
  uint32_t LDO1ULP                :1;                                /*!< LDO1 in ultralowpower mode after WFI command */
  uint32_t                        :6;                                /*!< RESERVED */
  uint32_t VL                     :5;                                /*!< Lower LDO voltage value in LP/ULP mode */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t VH                     :5;                                /*!< Upper LDO voltage value in LP/ULP mode */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t ALR                    :1;                                /*!< Automatic alarm reset RTC_CFG1.ALARMRST bit after WFI command */
} _PMURTC_WFI_ENTR_bits;

/* Bit field positions: */
#define PMURTC_WFI_ENTR_LDO0EN_Pos            0                      /*!< LDO0 enabled after WFI command */
#define PMURTC_WFI_ENTR_LDO1EN_Pos            1                      /*!< LDO1 enabled after WFI command */
#define PMURTC_WFI_ENTR_LDO0LP_Pos            4                      /*!< LDO0 in lowpower mode after WFI command */
#define PMURTC_WFI_ENTR_LDO1LP_Pos            5                      /*!< LDO1 in lowpower mode after WFI command */
#define PMURTC_WFI_ENTR_LDO0ULP_Pos           8                      /*!< LDO0 in ultralowpower mode after WFI command */
#define PMURTC_WFI_ENTR_LDO1ULP_Pos           9                      /*!< LDO1 in ultralowpower mode after WFI command */
#define PMURTC_WFI_ENTR_VL_Pos                16                     /*!< Lower LDO voltage value in LP/ULP mode */
#define PMURTC_WFI_ENTR_VH_Pos                24                     /*!< Upper LDO voltage value in LP/ULP mode */
#define PMURTC_WFI_ENTR_ALR_Pos               31                     /*!< Automatic alarm reset RTC_CFG1.ALARMRST bit after WFI command */

/* Bit field masks: */
#define PMURTC_WFI_ENTR_LDO0EN_Msk            0x00000001UL           /*!< LDO0 enabled after WFI command */
#define PMURTC_WFI_ENTR_LDO1EN_Msk            0x00000002UL           /*!< LDO1 enabled after WFI command */
#define PMURTC_WFI_ENTR_LDO0LP_Msk            0x00000010UL           /*!< LDO0 in lowpower mode after WFI command */
#define PMURTC_WFI_ENTR_LDO1LP_Msk            0x00000020UL           /*!< LDO1 in lowpower mode after WFI command */
#define PMURTC_WFI_ENTR_LDO0ULP_Msk           0x00000100UL           /*!< LDO0 in ultralowpower mode after WFI command */
#define PMURTC_WFI_ENTR_LDO1ULP_Msk           0x00000200UL           /*!< LDO1 in ultralowpower mode after WFI command */
#define PMURTC_WFI_ENTR_VL_Msk                0x001F0000UL           /*!< Lower LDO voltage value in LP/ULP mode */
#define PMURTC_WFI_ENTR_VH_Msk                0x1F000000UL           /*!< Upper LDO voltage value in LP/ULP mode */
#define PMURTC_WFI_ENTR_ALR_Msk               0x80000000UL           /*!< Automatic alarm reset RTC_CFG1.ALARMRST bit after WFI command */

/*--  WFI_EXIT: WFI Powerdown Exit configuration --------------------------------------------------------------*/
typedef struct {
  uint32_t LDO0EN                 :1;                                /*!< LDO0 enabled after WFI command */
  uint32_t LDO1EN                 :1;                                /*!< LDO1 enabled after WFI command */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t LDO0LP                 :1;                                /*!< LDO0 in lowpower mode after WFI command */
  uint32_t LDO1LP                 :1;                                /*!< LDO1 in lowpower mode after WFI command */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t LDO0ULP                :1;                                /*!< LDO0 in ultralowpower mode after WFI command */
  uint32_t LDO1ULP                :1;                                /*!< LDO1 in ultralowpower mode after WFI command */
  uint32_t                        :6;                                /*!< RESERVED */
  uint32_t VL                     :5;                                /*!< Lower LDO voltage value in LP/ULP mode */
  uint32_t                        :3;                                /*!< RESERVED */
  uint32_t VH                     :5;                                /*!< Upper LDO voltage value in LP/ULP mode */
  uint32_t                        :2;                                /*!< RESERVED */
  uint32_t ALR                    :1;                                /*!< Automatic alarm reset CFG1.ALARMRST bit after WFI command */
} _PMURTC_WFI_EXIT_bits;

/* Bit field positions: */
#define PMURTC_WFI_EXIT_LDO0EN_Pos            0                      /*!< LDO0 enabled after WFI command */
#define PMURTC_WFI_EXIT_LDO1EN_Pos            1                      /*!< LDO1 enabled after WFI command */
#define PMURTC_WFI_EXIT_LDO0LP_Pos            4                      /*!< LDO0 in lowpower mode after WFI command */
#define PMURTC_WFI_EXIT_LDO1LP_Pos            5                      /*!< LDO1 in lowpower mode after WFI command */
#define PMURTC_WFI_EXIT_LDO0ULP_Pos           8                      /*!< LDO0 in ultralowpower mode after WFI command */
#define PMURTC_WFI_EXIT_LDO1ULP_Pos           9                      /*!< LDO1 in ultralowpower mode after WFI command */
#define PMURTC_WFI_EXIT_VL_Pos                16                     /*!< Lower LDO voltage value in LP/ULP mode */
#define PMURTC_WFI_EXIT_VH_Pos                24                     /*!< Upper LDO voltage value in LP/ULP mode */
#define PMURTC_WFI_EXIT_ALR_Pos               31                     /*!< Automatic alarm reset CFG1.ALARMRST bit after WFI command */

/* Bit field masks: */
#define PMURTC_WFI_EXIT_LDO0EN_Msk            0x00000001UL           /*!< LDO0 enabled after WFI command */
#define PMURTC_WFI_EXIT_LDO1EN_Msk            0x00000002UL           /*!< LDO1 enabled after WFI command */
#define PMURTC_WFI_EXIT_LDO0LP_Msk            0x00000010UL           /*!< LDO0 in lowpower mode after WFI command */
#define PMURTC_WFI_EXIT_LDO1LP_Msk            0x00000020UL           /*!< LDO1 in lowpower mode after WFI command */
#define PMURTC_WFI_EXIT_LDO0ULP_Msk           0x00000100UL           /*!< LDO0 in ultralowpower mode after WFI command */
#define PMURTC_WFI_EXIT_LDO1ULP_Msk           0x00000200UL           /*!< LDO1 in ultralowpower mode after WFI command */
#define PMURTC_WFI_EXIT_VL_Msk                0x001F0000UL           /*!< Lower LDO voltage value in LP/ULP mode */
#define PMURTC_WFI_EXIT_VH_Msk                0x1F000000UL           /*!< Upper LDO voltage value in LP/ULP mode */
#define PMURTC_WFI_EXIT_ALR_Msk               0x80000000UL           /*!< Automatic alarm reset CFG1.ALARMRST bit after WFI command */

/*--  WFI_DELENTR: WFI Delay Entering PD mode register --------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< Value in ticks of REFCLK when entering powerdown mode */
} _PMURTC_WFI_DELENTR_bits;

/* Bit field positions: */
#define PMURTC_WFI_DELENTR_VAL_Pos            0                      /*!< Value in ticks of REFCLK when entering powerdown mode */

/* Bit field masks: */
#define PMURTC_WFI_DELENTR_VAL_Msk            0x0000FFFFUL           /*!< Value in ticks of REFCLK when entering powerdown mode */

/*--  WFI_DELEXIT: WFI Delay Exiting PD mode register ---------------------------------------------------------*/
typedef struct {
  uint32_t VAL                    :16;                               /*!< Value in ticks of REFCLK when exiting powerdown mode */
} _PMURTC_WFI_DELEXIT_bits;

/* Bit field positions: */
#define PMURTC_WFI_DELEXIT_VAL_Pos            0                      /*!< Value in ticks of REFCLK when exiting powerdown mode */

/* Bit field masks: */
#define PMURTC_WFI_DELEXIT_VAL_Msk            0x0000FFFFUL           /*!< Value in ticks of REFCLK when exiting powerdown mode */

/*--  PMU_IRQEVT: PMU Interrupt event flag --------------------------------------------------------------------*/
typedef struct {
  uint32_t IRQEVT                 :1;                                /*!< Flag automatic sets when ALARM, Tamper, WAKE events occurs. Need to clear for resetting request for interrupt controller */
} _PMURTC_PMU_IRQEVT_bits;

/* Bit field positions: */
#define PMURTC_PMU_IRQEVT_IRQEVT_Pos          0                      /*!< Flag automatic sets when ALARM, Tamper, WAKE events occurs. Need to clear for resetting request for interrupt controller */

/* Bit field masks: */
#define PMURTC_PMU_IRQEVT_IRQEVT_Msk          0x00000001UL           /*!< Flag automatic sets when ALARM, Tamper, WAKE events occurs. Need to clear for resetting request for interrupt controller */

/*--  PMU_VBATPER_FORCE: PMU vbat domain periph powerdown force register --------------------------------------*/
typedef struct {
  uint32_t DACPD                  :1;                                /*!< DAC powerdown alltime */
  uint32_t CMP0PD                 :1;                                /*!< CMP0 powerdown alltime */
  uint32_t CMP1PD                 :1;                                /*!< CMP1 powerdown alltime */
  uint32_t SRAM1LO_PD             :1;                                /*!< Lower 32k SRAM1 powerdown alltime */
  uint32_t SRAM1HI_PD             :1;                                /*!< Upper 32k SRAM1 powerdown alltime */
  uint32_t EXTOSC                 :1;                                /*!< Stop clock generation of external crystal oscillator alltime */
  uint32_t INTOSC                 :1;                                /*!< Stop clock generation of internal RC oscillator alltime */
  uint32_t                        :11;                               /*!< RESERVED */
} _PMURTC_PMU_VBATPER_FORCE_bits;

/* Bit field positions: */
#define PMURTC_PMU_VBATPER_FORCE_DACPD_Pos       0                   /*!< DAC powerdown alltime */
#define PMURTC_PMU_VBATPER_FORCE_CMP0PD_Pos       1                  /*!< CMP0 powerdown alltime */
#define PMURTC_PMU_VBATPER_FORCE_CMP1PD_Pos       2                  /*!< CMP1 powerdown alltime */
#define PMURTC_PMU_VBATPER_FORCE_SRAM1LO_PD_Pos       3              /*!< Lower 32k SRAM1 powerdown alltime */
#define PMURTC_PMU_VBATPER_FORCE_SRAM1HI_PD_Pos       4              /*!< Upper 32k SRAM1 powerdown alltime */
#define PMURTC_PMU_VBATPER_FORCE_EXTOSC_Pos       5                  /*!< Stop clock generation of external crystal oscillator alltime */
#define PMURTC_PMU_VBATPER_FORCE_INTOSC_Pos       6                  /*!< Stop clock generation of internal RC oscillator alltime */

/* Bit field masks: */
#define PMURTC_PMU_VBATPER_FORCE_DACPD_Msk       0x00000001UL         /*!< DAC powerdown alltime */
#define PMURTC_PMU_VBATPER_FORCE_CMP0PD_Msk       0x00000002UL         /*!< CMP0 powerdown alltime */
#define PMURTC_PMU_VBATPER_FORCE_CMP1PD_Msk       0x00000004UL         /*!< CMP1 powerdown alltime */
#define PMURTC_PMU_VBATPER_FORCE_SRAM1LO_PD_Msk       0x00000008UL         /*!< Lower 32k SRAM1 powerdown alltime */
#define PMURTC_PMU_VBATPER_FORCE_SRAM1HI_PD_Msk       0x00000010UL         /*!< Upper 32k SRAM1 powerdown alltime */
#define PMURTC_PMU_VBATPER_FORCE_EXTOSC_Msk       0x00000020UL         /*!< Stop clock generation of external crystal oscillator alltime */
#define PMURTC_PMU_VBATPER_FORCE_INTOSC_Msk       0x00000040UL         /*!< Stop clock generation of internal RC oscillator alltime */

/*--  PMU_VBATPER_WFI: PMU vbat domain periph powerdown WFI register ------------------------------------------*/
typedef struct {
  uint32_t DACPD                  :1;                                /*!< DAC powerdown after WFI command */
  uint32_t CMP0PD                 :1;                                /*!< CMP0 powerdown after WFI command */
  uint32_t CMP1PD                 :1;                                /*!< CMP1 powerdown after WFI command */
  uint32_t SRAM1LO_PD             :1;                                /*!< Lower 32k SRAM1 powerdown after WFI command */
  uint32_t SRAM1HI_PD             :1;                                /*!< Upper 32k SRAM1 powerdown after WFI command */
  uint32_t EXTOSC                 :1;                                /*!< Stop clock generation of external crystal oscillator after WFI command */
  uint32_t INTOSC                 :1;                                /*!< Stop clock generation of internal RC oscillator after WFI command */
  uint32_t                        :11;                               /*!< RESERVED */
} _PMURTC_PMU_VBATPER_WFI_bits;

/* Bit field positions: */
#define PMURTC_PMU_VBATPER_WFI_DACPD_Pos       0                     /*!< DAC powerdown after WFI command */
#define PMURTC_PMU_VBATPER_WFI_CMP0PD_Pos       1                    /*!< CMP0 powerdown after WFI command */
#define PMURTC_PMU_VBATPER_WFI_CMP1PD_Pos       2                    /*!< CMP1 powerdown after WFI command */
#define PMURTC_PMU_VBATPER_WFI_SRAM1LO_PD_Pos       3                /*!< Lower 32k SRAM1 powerdown after WFI command */
#define PMURTC_PMU_VBATPER_WFI_SRAM1HI_PD_Pos       4                /*!< Upper 32k SRAM1 powerdown after WFI command */
#define PMURTC_PMU_VBATPER_WFI_EXTOSC_Pos       5                    /*!< Stop clock generation of external crystal oscillator after WFI command */
#define PMURTC_PMU_VBATPER_WFI_INTOSC_Pos       6                    /*!< Stop clock generation of internal RC oscillator after WFI command */

/* Bit field masks: */
#define PMURTC_PMU_VBATPER_WFI_DACPD_Msk       0x00000001UL          /*!< DAC powerdown after WFI command */
#define PMURTC_PMU_VBATPER_WFI_CMP0PD_Msk       0x00000002UL         /*!< CMP0 powerdown after WFI command */
#define PMURTC_PMU_VBATPER_WFI_CMP1PD_Msk       0x00000004UL         /*!< CMP1 powerdown after WFI command */
#define PMURTC_PMU_VBATPER_WFI_SRAM1LO_PD_Msk       0x00000008UL         /*!< Lower 32k SRAM1 powerdown after WFI command */
#define PMURTC_PMU_VBATPER_WFI_SRAM1HI_PD_Msk       0x00000010UL         /*!< Upper 32k SRAM1 powerdown after WFI command */
#define PMURTC_PMU_VBATPER_WFI_EXTOSC_Msk       0x00000020UL         /*!< Stop clock generation of external crystal oscillator after WFI command */
#define PMURTC_PMU_VBATPER_WFI_INTOSC_Msk       0x00000040UL         /*!< Stop clock generation of internal RC oscillator after WFI command */

/*--  IWDG_CFG: IWDG Clock configuration register -------------------------------------------------------------*/
typedef struct {
  uint32_t CLKSRC                 :2;                                /*!< Independent watchdog clock source control */
  uint32_t                        :6;                                /*!< RESERVED */
  uint32_t RSTDIS                 :1;                                /*!< Disabling reset from IWDG timer counter */
} _PMURTC_IWDG_CFG_bits;

/* Bit field positions: */
#define PMURTC_IWDG_CFG_CLKSRC_Pos            0                      /*!< Independent watchdog clock source control */
#define PMURTC_IWDG_CFG_RSTDIS_Pos            8                      /*!< Disabling reset from IWDG timer counter */

/* Bit field masks: */
#define PMURTC_IWDG_CFG_CLKSRC_Msk            0x00000003UL           /*!< Independent watchdog clock source control */
#define PMURTC_IWDG_CFG_RSTDIS_Msk            0x00000100UL           /*!< Disabling reset from IWDG timer counter */

/* Bit field enums: */
typedef enum {
  PMURTC_IWDG_CFG_CLKSRC_HSI                 = 0x0UL,                /*!< HighSpeed Internal clock (default) */
  PMURTC_IWDG_CFG_CLKSRC_HSE                 = 0x1UL,                /*!< HighSpeed External clock source */
  PMURTC_IWDG_CFG_CLKSRC_LSI                 = 0x2UL,                /*!< LowSpeed Internal clock */
} PMURTC_IWDG_CFG_CLKSRC_Enum;

/*--  VBATRST: VBAT Reset Register ----------------------------------------------------------------------------*/
typedef struct {
  uint32_t RSTEN                  :1;                                /*!< Reset enable */
  uint32_t                        :15;                               /*!< RESERVED */
  uint32_t KEY                    :16;                               /*!< KEY 'C0DE' in hexadecimal for unblocking */
} _PMURTC_VBATRST_bits;

/* Bit field positions: */
#define PMURTC_VBATRST_RSTEN_Pos              0                      /*!< Reset enable */
#define PMURTC_VBATRST_KEY_Pos                16                     /*!< KEY 'C0DE' in hexadecimal for unblocking */

/* Bit field masks: */
#define PMURTC_VBATRST_RSTEN_Msk              0x00000001UL           /*!< Reset enable */
#define PMURTC_VBATRST_KEY_Msk                0xFFFF0000UL           /*!< KEY 'C0DE' in hexadecimal for unblocking */

/*--  HSI_TRIM: HSI TRIM Register ---------------------------------------------------------------------------*/
typedef struct {
  uint32_t TRIM                    :4;                                /*!< HSI TRIM */
} _PMURTC_HSI_TRIM_bits;

/* Bit field positions: */
#define PMURTC_HSI_TRIM_TRIM_Pos               0                      /*!< HSI TRIM */

/* Bit field masks: */
#define PMURTC_HSI_TRIM_TRIM_Msk               0x0000000FUL           /*!< HSI TRIM */

//Cluster RTC_REG:
typedef struct {
  union { 
  /*!< Register user value */
    __IO uint32_t RTC_REG;                                           /*!< RTC_REG : type used for word access */
    __IO _PMURTC_RTC_REG_bits RTC_REG_bit;                           /*!< RTC_REG_bit: structure used for bit access */
  };
} _PMURTC_RTC_REG_TypeDef;
typedef struct {
  union {                                                               /*!< Config0 register */
    __IO uint32_t RTC_CFG0;                                           /*!< RTC_CFG0    : type used for word access */
    __IO _PMURTC_RTC_CFG0_bits  RTC_CFG0_bit;                        /*!< RTC_CFG0_bit: structure used for bit access */
  };
  union {                                                               /*!< Trim RC OSC register */
    __IO uint32_t RTC_TRIMRC;                                           /*!< RTC_TRIMRC    : type used for word access */
    __IO _PMURTC_RTC_TRIMRC_bits  RTC_TRIMRC_bit;                    /*!< RTC_TRIMRC_bit: structure used for bit access */
  };
  union {                                                               /*!< History register */
    __IO uint32_t RTC_HISTORY;                                           /*!< RTC_HISTORY    : type used for word access */
    __IO _PMURTC_RTC_HISTORY_bits  RTC_HISTORY_bit;                  /*!< RTC_HISTORY_bit: structure used for bit access */
  };
  union {                                                               /*!< Time register */
    __IO uint32_t RTC_TIME;                                           /*!< RTC_TIME    : type used for word access */
    __IO _PMURTC_RTC_TIME_bits  RTC_TIME_bit;                        /*!< RTC_TIME_bit: structure used for bit access */
  };
  union {                                                               /*!< Alarm register */
    __IO uint32_t RTC_ALARM;                                           /*!< RTC_ALARM    : type used for word access */
    __IO _PMURTC_RTC_ALARM_bits  RTC_ALARM_bit;                      /*!< RTC_ALARM_bit: structure used for bit access */
  };
  union {                                                               /*!< Trim register */
    __IO uint32_t RTC_TRIM;                                           /*!< RTC_TRIM    : type used for word access */
    __IO _PMURTC_RTC_TRIM_bits  RTC_TRIM_bit;                        /*!< RTC_TRIM_bit: structure used for bit access */
  };
  union {                                                               /*!< Config1 register */
    __IO uint32_t RTC_CFG1;                                           /*!< RTC_CFG1    : type used for word access */
    __IO _PMURTC_RTC_CFG1_bits  RTC_CFG1_bit;                        /*!< RTC_CFG1_bit: structure used for bit access */
  };
  union {                                                               /*!< Wake config register */
    __IO uint32_t RTC_WAKECFG;                                           /*!< RTC_WAKECFG    : type used for word access */
    __IO _PMURTC_RTC_WAKECFG_bits  RTC_WAKECFG_bit;                  /*!< RTC_WAKECFG_bit: structure used for bit access */
  };
  _PMURTC_RTC_REG_TypeDef RTC_REG[16];
    __IO uint32_t Reserved0[8];
  union {                                                               /*!< PMU WKVBATPER Event Enable register */
    __IO uint32_t PMU_WK3EN;                                           /*!< PMU_WK3EN    : type used for word access */
    __IO _PMURTC_PMU_WK3EN_bits  PMU_WK3EN_bit;                      /*!< PMU_WK3EN_bit: structure used for bit access */
  };
  union {                                                               /*!< PMU WKVBATPER Event Status register */
    __IO uint32_t PMU_WK3STAT;                                           /*!< PMU_WK3STAT    : type used for word access */
    __IO _PMURTC_PMU_WK3STAT_bits  PMU_WK3STAT_bit;                  /*!< PMU_WK3STAT_bit: structure used for bit access */
  };
  union {                                                               /*!< PMU Wait cycles Register Access */
    __IO uint32_t PMU_WCYC;                                           /*!< PMU_WCYC    : type used for word access */
    __IO _PMURTC_PMU_WCYC_bits  PMU_WCYC_bit;                        /*!< PMU_WCYC_bit: structure used for bit access */
  };
  union {                                                               /*!< WFI Powerdown function configuration */
    __IO uint32_t WFI_PDEN;                                           /*!< WFI_PDEN    : type used for word access */
    __IO _PMURTC_WFI_PDEN_bits  WFI_PDEN_bit;                        /*!< WFI_PDEN_bit: structure used for bit access */
  };
  union {                                                               /*!< WFI Powerdown Entry configuration */
    __IO uint32_t WFI_ENTR;                                           /*!< WFI_ENTR    : type used for word access */
    __IO _PMURTC_WFI_ENTR_bits  WFI_ENTR_bit;                        /*!< WFI_ENTR_bit: structure used for bit access */
  };
  union {                                                               /*!< WFI Powerdown Exit configuration */
    __IO uint32_t WFI_EXIT;                                           /*!< WFI_EXIT    : type used for word access */
    __IO _PMURTC_WFI_EXIT_bits  WFI_EXIT_bit;                        /*!< WFI_EXIT_bit: structure used for bit access */
  };
  union {                                                               /*!< WFI Delay Entering PD mode register */
    __IO uint32_t WFI_DELENTR;                                           /*!< WFI_DELENTR    : type used for word access */
    __IO _PMURTC_WFI_DELENTR_bits  WFI_DELENTR_bit;                  /*!< WFI_DELENTR_bit: structure used for bit access */
  };
  union {                                                               /*!< WFI Delay Exiting PD mode register */
    __IO uint32_t WFI_DELEXIT;                                           /*!< WFI_DELEXIT    : type used for word access */
    __IO _PMURTC_WFI_DELEXIT_bits  WFI_DELEXIT_bit;                  /*!< WFI_DELEXIT_bit: structure used for bit access */
  };
  union {                                                               /*!< PMU Interrupt event flag */
    __IO uint32_t PMU_IRQEVT;                                           /*!< PMU_IRQEVT    : type used for word access */
    __IO _PMURTC_PMU_IRQEVT_bits  PMU_IRQEVT_bit;                    /*!< PMU_IRQEVT_bit: structure used for bit access */
  };
  union {                                                               /*!< PMU vbat domain periph powerdown force register */
    __IO uint32_t PMU_VBATPER_FORCE;                                           /*!< PMU_VBATPER_FORCE    : type used for word access */
    __IO _PMURTC_PMU_VBATPER_FORCE_bits  PMU_VBATPER_FORCE_bit;                /*!< PMU_VBATPER_FORCE_bit: structure used for bit access */
  };
  union {                                                               /*!< PMU vbat domain periph powerdown WFI register */
    __IO uint32_t PMU_VBATPER_WFI;                                           /*!< PMU_VBATPER_WFI    : type used for word access */
    __IO _PMURTC_PMU_VBATPER_WFI_bits  PMU_VBATPER_WFI_bit;                /*!< PMU_VBATPER_WFI_bit: structure used for bit access */
  };
  union {                                                               /*!< IWDG Clock configuration register */
    __IO uint32_t IWDG_CFG;                                           /*!< IWDG_CFG    : type used for word access */
    __IO _PMURTC_IWDG_CFG_bits  IWDG_CFG_bit;                        /*!< IWDG_CFG_bit: structure used for bit access */
  };
    __IO uint32_t Reserved1[4];
  union {                                                               /*!< VBAT Reset Register */
    __O uint32_t VBATRST;                                            /*!< VBATRST    : type used for word access */
    __O _PMURTC_VBATRST_bits  VBATRST_bit;                           /*!< VBATRST_bit: structure used for bit access */
  };
    __IO uint32_t Reserved2;
  union {                                                               /*!< HSI TRIM Register */
    __I uint32_t HSI_TRIM;                                           /*!< HSI_TRIM    : type used for word access */
    __I _PMURTC_HSI_TRIM_bits  HSI_TRIM_bit;                         /*!< HSI_TRIM_bit: structure used for bit access */
  };
} PMURTC_TypeDef;




/******************************************************************************/
/*                          Peripheral memory map                             */
/******************************************************************************/
#define CAN_BASE               (0x20000000UL)
#define CANMSG_BASE            (0x20001000UL)
#define USB_BASE               (0x20010000UL)
#define CRYPTO_BASE            (0x20020000UL)
#define CRC0_BASE              (0x20030000UL)
#define CRC1_BASE              (0x20031000UL)
#define HASH_BASE              (0x20032000UL)
#define QSPI_BASE              (0x20040000UL)
#define SPI0_BASE              (0x20050000UL)
#define SPI1_BASE              (0x20060000UL)
#define GPIOA_BASE             (0x28000000UL)
#define GPIOB_BASE             (0x28001000UL)
#define GPIOC_BASE             (0x28002000UL)
#define TMR32_BASE             (0x30000000UL)
#define TMR0_BASE              (0x30001000UL)
#define TMR1_BASE              (0x30002000UL)
#define TMR2_BASE              (0x30003000UL)
#define TRNG_BASE              (0x30004000UL)
#define I2C_BASE               (0x30005000UL)
#define UART0_BASE             (0x30006000UL)
#define UART1_BASE             (0x30007000UL)
#define UART2_BASE             (0x30008000UL)
#define UART3_BASE             (0x30009000UL)
#define UART4_BASE             (0x3000A000UL)
#define WDT_BASE               (0x3000B000UL)
#define DMA_BASE               (0x3000C000UL)
#define FLASH_BASE             (0x3000D000UL)
#define RCU_BASE               (0x3000E000UL)
#define PMUSYS_BASE            (0x3000F000UL)
#define ADCSAR_BASE            (0x30010000UL)
#define TSENS_BASE             (0x30011000UL)
#define ADCSD_BASE             (0x30012000UL)
#define CMP_BASE               (0x38010000UL)
#define PMURTC_BASE            (0x38011000UL)
#define IWDT_BASE              (0x38012000UL)

/******************************************************************************/
/*                          Peripheral declaration                            */
/******************************************************************************/
#define CAN                  ((CAN_TypeDef                 *) CAN_BASE)
#define CANMSG               ((CANMSG_TypeDef              *) CANMSG_BASE)
#define USB                  ((USB_TypeDef                 *) USB_BASE)
#define CRYPTO               ((CRYPTO_TypeDef              *) CRYPTO_BASE)
#define CRC0                 ((CRC_TypeDef                 *) CRC0_BASE)
#define CRC1                 ((CRC_TypeDef                 *) CRC1_BASE)
#define HASH                 ((HASH_TypeDef                *) HASH_BASE)
#define QSPI                 ((QSPI_TypeDef                *) QSPI_BASE)
#define SPI0                 ((SPI_TypeDef                 *) SPI0_BASE)
#define SPI1                 ((SPI_TypeDef                 *) SPI1_BASE)
#define GPIOA                ((GPIO_TypeDef                *) GPIOA_BASE)
#define GPIOB                ((GPIO_TypeDef                *) GPIOB_BASE)
#define GPIOC                ((GPIO_TypeDef                *) GPIOC_BASE)
#define TMR32                ((TMR32_TypeDef               *) TMR32_BASE)
#define TMR0                 ((TMR_TypeDef                 *) TMR0_BASE)
#define TMR1                 ((TMR_TypeDef                 *) TMR1_BASE)
#define TMR2                 ((TMR_TypeDef                 *) TMR2_BASE)
#define TRNG                 ((TRNG_TypeDef                *) TRNG_BASE)
#define I2C                  ((I2C_TypeDef                 *) I2C_BASE)
#define UART0                ((UART_TypeDef                *) UART0_BASE)
#define UART1                ((UART_TypeDef                *) UART1_BASE)
#define UART2                ((UART_TypeDef                *) UART2_BASE)
#define UART3                ((UART_TypeDef                *) UART3_BASE)
#define UART4                ((UART_TypeDef                *) UART4_BASE)
#define WDT                  ((WDT_TypeDef                 *) WDT_BASE)
#define DMA                  ((DMA_TypeDef                 *) DMA_BASE)
#define FLASH                ((FLASH_TypeDef               *) FLASH_BASE)
#define RCU                  ((RCU_TypeDef                 *) RCU_BASE)
#define PMUSYS               ((PMUSYS_TypeDef              *) PMUSYS_BASE)
#define ADCSAR               ((ADCSAR_TypeDef              *) ADCSAR_BASE)
#define TSENS                ((TSENS_TypeDef               *) TSENS_BASE)
#define ADCSD                ((ADCSD_TypeDef               *) ADCSD_BASE)
#define CMP                  ((CMP_TypeDef                 *) CMP_BASE)
#define PMURTC               ((PMURTC_TypeDef              *) PMURTC_BASE)
#define IWDT                 ((IWDT_TypeDef                *) IWDT_BASE)

/******************************************************************************/
/*                          Peripheral capabilities                           */
/******************************************************************************/
#define CAN_PRESENT
#define CAN_TOTAL     1
typedef enum {
  CAN_Num
} CAN_Num_TypeDef;

#define CANMSG_PRESENT
#define CANMSG_TOTAL  1
typedef enum {
  CANMSG_Num
} CANMSG_Num_TypeDef;

#define USB_PRESENT
#define USB_TOTAL     1
typedef enum {
  USB_Num
} USB_Num_TypeDef;

#define CRYPTO_PRESENT
#define CRYPTO_TOTAL  1
typedef enum {
  CRYPTO_Num
} CRYPTO_Num_TypeDef;

#define CRC_PRESENT
#define CRC_TOTAL     2
typedef enum {
  CRC0_Num,
  CRC1_Num
} CRC_Num_TypeDef;

#define HASH_PRESENT
#define HASH_TOTAL    1
typedef enum {
  HASH_Num
} HASH_Num_TypeDef;

#define QSPI_PRESENT
#define QSPI_TOTAL    1
typedef enum {
  QSPI_Num
} QSPI_Num_TypeDef;

#define SPI_PRESENT
#define SPI_TOTAL     2
typedef enum {
  SPI0_Num,
  SPI1_Num
} SPI_Num_TypeDef;

#define GPIO_PRESENT
#define GPIO_TOTAL    3
typedef enum {
  GPIOA_Num,
  GPIOB_Num,
  GPIOC_Num
} GPIO_Num_TypeDef;

#define TMR32_PRESENT
#define TMR32_TOTAL   1
typedef enum {
  TMR32__Num
} TMR32_Num_TypeDef;

#define TMR_PRESENT
#define TMR_TOTAL     3
typedef enum {
  TMR0_Num,
  TMR1_Num,
  TMR2_Num
} TMR_Num_TypeDef;

#define TRNG_PRESENT
#define TRNG_TOTAL    1
typedef enum {
  TRNG_Num
} TRNG_Num_TypeDef;

#define I2C_PRESENT
#define I2C_TOTAL     1
typedef enum {
  I2C_Num
} I2C_Num_TypeDef;

#define UART_PRESENT
#define UART_TOTAL    5
typedef enum {
  UART0_Num,
  UART1_Num,
  UART2_Num,
  UART3_Num,
  UART4_Num
} UART_Num_TypeDef;

#define WDT_PRESENT
#define WDT_TOTAL     1
typedef enum {
  WDT_Num
} WDT_Num_TypeDef;

#define DMA_PRESENT
#define DMA_TOTAL     1
typedef enum {
  DMA_Num
} DMA_Num_TypeDef;

#define FLASH_PRESENT
#define FLASH_TOTAL   1
typedef enum {
  FLASH_Num
} FLASH_Num_TypeDef;

#define RCU_PRESENT
#define RCU_TOTAL     1
typedef enum {
  RCU_Num
} RCU_Num_TypeDef;

#define PMUSYS_PRESENT
#define PMUSYS_TOTAL  1
typedef enum {
  PMUSYS_Num
} PMUSYS_Num_TypeDef;

#define ADCSAR_PRESENT
#define ADCSAR_TOTAL  1
typedef enum {
  ADCSAR_Num
} ADCSAR_Num_TypeDef;

#define TSENS_PRESENT
#define TSENS_TOTAL   1
typedef enum {
  TSENS_Num
} TSENS_Num_TypeDef;

#define ADCSD_PRESENT
#define ADCSD_TOTAL   1
typedef enum {
  ADCSD_Num
} ADCSD_Num_TypeDef;

#define CMP_PRESENT
#define CMP_TOTAL     1
typedef enum {
  CMP_Num
} CMP_Num_TypeDef;

#define PMURTC_PRESENT
#define PMURTC_TOTAL  1
typedef enum {
  PMURTC_Num
} PMURTC_Num_TypeDef;

#define IWDT_PRESENT
#define IWDT_TOTAL    1
typedef enum {
  IWDT_Num
} IWDT_Num_TypeDef;

/******************************************************************************/
/*                          Peripheral assertions                             */
/******************************************************************************/
#define IS_CAN_PERIPH(PERIPH)                    (((PERIPH) == CAN))
#define IS_CANMSG_PERIPH(PERIPH)                 (((PERIPH) == CANMSG))
#define IS_USB_PERIPH(PERIPH)                    (((PERIPH) == USB))
#define IS_CRYPTO_PERIPH(PERIPH)                 (((PERIPH) == CRYPTO))
#define IS_CRC_PERIPH(PERIPH)                    (((PERIPH) == CRC0) || \
                                                  ((PERIPH) == CRC1))
#define IS_HASH_PERIPH(PERIPH)                   (((PERIPH) == HASH))
#define IS_QSPI_PERIPH(PERIPH)                   (((PERIPH) == QSPI))
#define IS_SPI_PERIPH(PERIPH)                    (((PERIPH) == SPI0) || \
                                                  ((PERIPH) == SPI1))
#define IS_GPIO_PERIPH(PERIPH)                   (((PERIPH) == GPIOA) || \
                                                  ((PERIPH) == GPIOB) || \
                                                  ((PERIPH) == GPIOC))
#define IS_TMR32_PERIPH(PERIPH)                  (((PERIPH) == TMR32_))
#define IS_TMR_PERIPH(PERIPH)                    (((PERIPH) == TMR0) || \
                                                  ((PERIPH) == TMR1) || \
                                                  ((PERIPH) == TMR2))
#define IS_TRNG_PERIPH(PERIPH)                   (((PERIPH) == TRNG))
#define IS_I2C_PERIPH(PERIPH)                    (((PERIPH) == I2C))
#define IS_UART_PERIPH(PERIPH)                   (((PERIPH) == UART0) || \
                                                  ((PERIPH) == UART1) || \
                                                  ((PERIPH) == UART2) || \
                                                  ((PERIPH) == UART3) || \
                                                  ((PERIPH) == UART4))
#define IS_WDT_PERIPH(PERIPH)                    (((PERIPH) == WDT))
#define IS_DMA_PERIPH(PERIPH)                    (((PERIPH) == DMA))
#define IS_FLASH_PERIPH(PERIPH)                  (((PERIPH) == FLASH))
#define IS_RCU_PERIPH(PERIPH)                    (((PERIPH) == RCU))
#define IS_PMUSYS_PERIPH(PERIPH)                 (((PERIPH) == PMUSYS))
#define IS_ADCSAR_PERIPH(PERIPH)                 (((PERIPH) == ADCSAR))
#define IS_TSENS_PERIPH(PERIPH)                  (((PERIPH) == TSENS))
#define IS_ADCSD_PERIPH(PERIPH)                  (((PERIPH) == ADCSD))
#define IS_CMP_PERIPH(PERIPH)                    (((PERIPH) == CMP))
#define IS_PMURTC_PERIPH(PERIPH)                 (((PERIPH) == PMURTC))
#define IS_IWDT_PERIPH(PERIPH)                   (((PERIPH) == IWDT))


/******************************************************************************/
/*                         CRYPTO DMA descriptor                              */
/******************************************************************************/

//--  CRYPTO_DMA_DESCR_CONTROL: CRYPTO DMA operations control word
typedef struct {
  uint32_t UPDATE_KEY             :1;
  uint32_t LAST_DESCRIPTOR        :1;
  uint32_t DIRECTION              :1;
  uint32_t ALGORITHM              :2;
  uint32_t MODE                   :2;
  uint32_t                        :6;
  uint32_t GCM_PHASE              :2;
  uint32_t INTERRUPT_ENABLE       :1;
  uint32_t BLOCKS_COUNT           :12;
  uint32_t                        :4;
} _CRYPTO_DMA_DESCR_CONTROL_bits;

// Bit field positions:
#define CRYPTO_DMA_DESCR_CONTROL_UPDATE_KEY_Pos          0
#define CRYPTO_DMA_DESCR_CONTROL_LAST_DESCRIPTOR_Pos     1
#define CRYPTO_DMA_DESCR_CONTROL_DIRECTION_Pos           2
#define CRYPTO_DMA_DESCR_CONTROL_ALGORITHM_Pos           3
#define CRYPTO_DMA_DESCR_CONTROL_MODE_Pos                5
#define CRYPTO_DMA_DESCR_CONTROL_GCM_PHASE_Pos          13
#define CRYPTO_DMA_DESCR_CONTROL_INTERRUPT_ENABLE_Pos   15
#define CRYPTO_DMA_DESCR_CONTROL_BLOCKS_COUNT_Pos       16

// Bit field masks:
#define CRYPTO_DMA_DESCR_CONTROL_UPDATE_KEY_Msk         0x00000001UL
#define CRYPTO_DMA_DESCR_CONTROL_LAST_DESCRIPTOR_Msk    0x00000002UL
#define CRYPTO_DMA_DESCR_CONTROL_DIRECTION_Msk          0x00000004UL
#define CRYPTO_DMA_DESCR_CONTROL_ALGORITHM_Msk          0x00000018UL
#define CRYPTO_DMA_DESCR_CONTROL_MODE_Msk               0x00000060UL
#define CRYPTO_DMA_DESCR_CONTROL_GCM_PHASE_Msk          0x00006000UL
#define CRYPTO_DMA_DESCR_CONTROL_INTERRUPR_ENABLE_Msk   0x00008000UL
#define CRYPTO_DMA_DESCR_CONTROL_BLOCKS_COUNT_Msk       0x0FFF0000UL

typedef CRYPTO_CONTROL_DIRECTION_Enum  CRYPTO_DMA_DESCR_CONTROL_DIRECTION_Enum;
typedef CRYPTO_CONTROL_ALGORITHM_Enum  CRYPTO_DMA_DESCR_CONTROL_ALGORITHM_Enum;
typedef CRYPTO_CONTROL_MODE_Enum       CRYPTO_DMA_DESCR_CONTROL_MODE_Enum;
typedef CRYPTO_CONTROL_GCM_PHASE_Enum  CRYPTO_DMA_DESCR_CONTROL_GCM_PHASE_Enum;


//--  CRYPTO_DMA_DESCR_SRC_ADDR and CRYPTO_DMA_DESCR_DST_ADDR

typedef struct {
  uint32_t ZEROES                 :2;
  uint32_t ADDRESS                :30;
} _CRYPTO_WORD_ALIGNED_ADDRESS_bits;

// Bit field positions:
#define CRYPTO_WORD_ALIGNED_ADDRESS_ZEROES_Pos          0
#define CRYPTO_WORD_ALIGNED_ADDRESS_ADDRESS_Pos         2

// Bit field masks:
#define CRYPTO_WORD_ALIGNED_ADDRESS_ZEROES_Msk          0x00000003UL
#define CRYPTO_WORD_ALIGNED_ADDRESS_ADDRESS_Msk         0xFFFFFFFCUL


typedef struct {
  union {
    __IO uint32_t                           CONTROL;
    __IO _CRYPTO_DMA_DESCR_CONTROL_bits     CONTROL_bit;
  };
  union {
    __IO uint32_t                           SRC_ADDR;
    __IO _CRYPTO_WORD_ALIGNED_ADDRESS_bits  SRC_ADDR_bit;
  };
  union {
    __IO uint32_t                           DST_ADDR;
    __IO _CRYPTO_WORD_ALIGNED_ADDRESS_bits  DST_ADDR_bit;
  };
  union {
    __IO uint32_t                           NEXT_DESCR;
    __IO _CRYPTO_BASE_DESCRIPTOR_bits       NEXT_DESCR_bit;
  };
} CRYPTO_DMA_DESCR_TypeDef;



#ifdef __cplusplus
}
#endif

#endif /* __K1921VG015_H */

/************************** (C) COPYRIGHT 2025 NIIET ***************************
*
* END OF FILE K1921VG015.h */
